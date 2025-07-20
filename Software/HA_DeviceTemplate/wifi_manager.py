# Import print_control so the custom print replacement is active
try:
        from print_control import custom_print as print #noqa: F401  
except Exception:
    pass
"""Wi-Fi management utilities for the ESP32S3 running MicroPython."""

import json
import time
import ure
import os
import network
import socket
from machine import reset, Timer
import sys

_setup_timer = None

try:
    getenv = os.getenv  # type: ignore[attr-defined]
except AttributeError:  # MicroPython may not implement os.getenv
    def getenv(key: str, default: str | None = None) -> str | None:
        return default

# Allow time to connect the debugger on boot
time.sleep(1)


def unquote(s: str) -> str:
    """Decode a minimal set of URL-encoded characters in ``s``."""
    s = s.replace("+", " ")
    s = s.replace("%20", " ")
    s = s.replace("%21", "!")
    s = s.replace("%22", '"')
    s = s.replace("%27", "'")
    return s


class WifiManager:
    """Helper for connecting to Wi-Fi networks and optionally starting an AP."""
    ap_config = None
    preferred_networks = None
    _server_socket = None

    #     wlan_sta = network.WLAN(network.STA_IF)
    #     wlan_ap = network.WLAN(network.AP_IF)

    _ap_start_policy = "never"
    # Path to the Wi-Fi configuration file.  ``getenv`` falls back to the
    # provided default when environment variables are unavailable (as on
    # MicroPython).
    config_file = getenv("WIFI_CONFIG_PATH", "/networks.json")
    available_networks = []
    verbose = False
    connected = False

    _server_timer = None

    @classmethod
    def _server_loop(cls, timer):
        if not cls._server_socket:
            return
        try:
            conn, addr = cls._server_socket.accept()
        except OSError:
            return
        print(f'Got a connection from{addr}')
        request = conn.recv(1024).decode("utf-8")
        print(f'Request: {request}')
        first_line = request.split('\r\n')[0] if request else ""
        method, path, _ = first_line.split(' ') if first_line else ("", "", "")
        if path.startswith("/configure"):
            cls.handle_configure(conn, request)
        else:
            cls.handle_root(conn)

    @classmethod
    def start_server(cls):
        """Start a minimal configuration web server using a timer."""
        print("start server")
        if cls._server_socket is not None:
            print(f"closing existing socket")
            cls._server_socket.close()
            time.sleep(0.1)

        addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
        print(f"address info : {addr}")
        try:
            cls._server_socket = socket.socket()
            print(f" server socket: {cls._server_socket}")
            cls._server_socket.bind(addr)
            print("socket binded")
            cls._server_socket.listen(1)
            print(f"Listening on : {addr}")
        except Exception as e:
            print(f"error : {e}")

        print(f" server socket is : {cls._server_socket}")
        if cls._server_socket:
            cls._server_timer = Timer(10)
            cls._server_timer.init(period=100, mode=Timer.PERIODIC, callback=cls._server_loop)

    @classmethod
    def stop_server(cls):
        """Close the running web server, if any."""
        if cls._server_socket:
            cls._server_socket.close()
        cls._server_socket = None
        if cls._server_timer:
            cls._server_timer.deinit()
        cls._server_timer = None

    @classmethod
    def handle_root(cls, client):
        """Serve a simple HTML form listing available networks."""
        cls.wlan().active(True)
        networks = cls.wlan().scan()
        networks_html = ''.join(
            '<option value="{0}">{0}</option>'.format(ssid.decode('utf-8')) for ssid, *_ in networks)

        html = """\
        <html>
            <h1>Select WiFi network</h1>
            <form action="configure" method="post">
                <select name="ssid">{networks}</select>
                <input name="password" type="password" placeholder="Password">
                <input type="submit" value="Connect">
            </form>
        </html>
        """.format(networks=networks_html)

        client.send('HTTP/1.1 200 OK\n')
        client.send('Content-Type: text/html\n')
        client.send('Connection: close\n\n')
        client.sendall(html)

    @classmethod
    def handle_configure(cls, client, request):
        """Handle form submission from the configuration page."""
        # You'll need to adjust  to properly parse POST data from the request body
        body = request.split('\r\n\r\n', 1)[1] if len(request.split('\r\n\r\n', 1)) > 1 else ""
        match = ure.search("ssid=([^&]+)&password=([^&]+)", body)
        if match:
            ssid, password = match.group(1), match.group(2)
            ssid, password = unquote(ssid).replace("+", " "), unquote(password).replace("+", " ")
            if cls.connect_to(ssid=ssid, password=password):
                response = "Connected successfully to {}".format(ssid)
            else:
                response = "Failed to connect to {}".format(ssid)
        else:
            response = "Failed to extract SSID and password from request"

        client.send('HTTP/1.1 200 OK\n')
        client.send('Content-Type: text/html\n')
        client.send('Connection: close\n\n')
        client.sendall(response)

    @classmethod
    def load_config(cls):
        """Load network configuration from :data:`config_file`.

        Returns
        -------
        bool
            ``True`` on success, ``False`` otherwise.
        """
        try:
            with open(cls.config_file, "r") as f:
                config = json.load(f)
                cls.preferred_networks = config.get('known_networks', [])
                cls.ap_config = config.get("access_point", {})
                if config.get("schema", 0) != 2:
                    print("Did not get expected schema [2] in JSON config.")
        except Exception as e:
            print(f"Failed to load config file, error: {e}")
            cls.preferred_networks = []
            cls.ap_config = {}
            return False
        return True

    @classmethod
    def setup_network(cls, verbose=None):
        """Attempt to connect to a preferred network.

        Parameters
        ----------
        verbose : bool | None
            Enable verbose logging during setup when ``True``.
        """
        if verbose :
            cls.verbose = verbose
            
        if not cls.load_config():
            print(f" Configuration failed to load")
            time.sleep(0.1)
            return  # Configuration failed to load
        
        print(f"activate  wlan")
        cls.activate_wlan()
        
        print(f"find networks")
        for i in range(5):
            cls.available_networks = []
            cls.available_networks = [ssid.decode() for ssid, *_ in cls.wlan().scan()]
            if cls.available_networks == []:
                print(f".")
                time.sleep(0.1)
            else:
                print(f"{cls.available_networks}")
                time.sleep(0.1)
                break
        if cls.available_networks == []:
            print(f"no network found")
            cls.wlan().active(False)
            time.sleep(0.5)
        # Find the first preferred network that is available
        # Initialize connection to None
        connection = None

        # Iterate over preferred networks to find the first available one
        for pref in cls.preferred_networks:
            if pref["ssid"] in cls.available_networks:
                connection = pref
                time.sleep(0.1)
                break
            
        # if preffered connection is found, try connecting to it
        if connection:
            if connection["ssid"]==cls.current_network():
                cls.connected = True
                print(f"already connected to prefered network {connection['ssid']}")
                time.sleep(0.1)
                return
            
            # log the attemp parameters
            print(f"Attempting to connect to network {connection['ssid']}...")
            print(f"ssid : {connection['ssid']}")
            print(f"password : {(connection['password'])}")
            time.sleep(0.1)
            
            # attemps to connect
            if cls.connect_to(ssid=connection['ssid'], password=connection['password']):
                # log if it has succedded and with which parameters
                cls.connected = True
                print(f" Successfully connected to {connection['ssid']}")
                print(f" connection parameters : {WifiManager.wlan().ifconfig()}")
                time.sleep(0.1)
                
            else:
                cls.connected = False
                # log if it failed and to which ssid
                print(f"Failed to connect to {connection['ssid']}")
                time.sleep(0.5)

      

    @classmethod
    def wlan_status(cls):
        """Return ``True`` if the station is connected to a Wi-Fi network."""
        return cls.wlan().isconnected()

    @classmethod
    def current_network(cls):
        """Return the SSID of the currently connected network or ``None``."""
        if cls.wlan_status():
            return cls.wlan().config('essid')
        else:
            return None

    @classmethod
    def ap_enabled(cls):
        """Return ``True`` if the access point interface is active."""
        return cls.accesspoint().active()

    @classmethod
    def wlan_enabled(cls):
        """Return ``True`` if the station interface is active."""
        return cls.wlan().active()

    @classmethod
    def activate_wlan(cls):
        """Ensure the station interface is active."""
        if cls.ap_enabled():
            cls.accesspoint().active(False)
            time.sleep(0.5)
        if not cls.wlan_enabled():
            cls.wlan().active(True)
            time.sleep(0.5)
            print('wlan has been activated')
        else:
            print('wlan already active')

    @classmethod
    def start_ap_mode(cls):
        """Start the device in access point mode."""
        print("Starting AP mode...")
        if cls._ap_start_policy != "never":
            cls.accesspoint().active(True)
            essid = cls.ap_config.get("config", {}).get("essid", "MicroPython-AP")
            password = cls.ap_config.get("config", {}).get("password", "micropythoN")
            print(f"configuring AP Mode Enabled: ESSID='{essid}', PASSWORD='{password}'")
            
            cls.accesspoint().config(essid=essid, password=password)
            
            print(f"AP Mode Enabled: ESSID='{essid}', PASSWORD='{password}'")

            # Start serving the webpage using timer callbacks
            cls.start_server()
        else:
            print(f"AP mode not started due to policy:{cls._ap_start_policy}")



    @classmethod
    def wlan(cls):
        """Return the station :class:`network.WLAN` instance."""
        return network.WLAN(network.STA_IF)
    
    @classmethod
    def accesspoint(cls):
        """Return the access point :class:`network.WLAN` instance."""
        return network.WLAN(network.AP_IF)
    
    @classmethod
    def disconnect(cls):
        """Disconnect from the current Wi-Fi network."""
        if cls.wlan_status():
            cls.wlan().disconnect()
            print('wlan has been disconnected')
        else:
            print('wlan already disconnected')

    @classmethod
    def deactivate(cls):
        """Turn off the station interface."""
        if cls.wlan_enabled():
            cls.wlan().active(False)
            print('wlan has been deactivated')
        else:
            print('wlan already deactivated')
            
    @classmethod
    def sync_time():
        """Synchronise the device RTC with an NTP server."""
        try:
            print("Synchronizing time with NTP server...")
            ntptime.settime()  # Sync RTC with NTP
            rtc = RTC()
            current_time = rtc.datetime()
            formatted_time = "{:02}-{:02}-{:02} {:02}:{:02}:{:02}".format(
                current_time[0], current_time[1], current_time[2],  # Year, Month, Day
                current_time[4], current_time[5], current_time[6]   # Hour, Minute, Second
            )
            print("Device time synchronized:", formatted_time)
        except Exception as e:
            print("Failed to sync time:", e)

    @classmethod
    def connect_to(cls, *, ssid, password, **kwargs):
        """Attempt to connect to ``ssid`` using ``password``."""
        if not cls.wlan_enabled():
            print("wlan not active to connect")
            return
        if cls.ap_enabled():
            print("apmode activated. cant connect to wlan")
            return
        if cls.wlan().isconnected():
            print(f"wlan already connected to : {cls.current_network()}, cannot connect again")
            return
        
        cls.wlan().connect(ssid, password, **kwargs)
        cls._connect_attempts = 0

        def _check_conn(timer):
            cls._connect_attempts += 1
            if cls.wlan().isconnected() or cls._connect_attempts >= 20:
                timer.deinit()

        cls._connect_timer = Timer(11)
        cls._connect_timer.init(period=250, mode=Timer.PERIODIC, callback=_check_conn)

    @classmethod
    def status(cls):
        """Return a formatted status report string."""
        cls.status_dict = {
            network.STAT_GOT_IP: "network.STAT_GOT_IP",
            network.STAT_IDLE: "network.STAT_IDLE",
            network.STAT_CONNECTING: "network.STAT_CONNECTING",
            network.STAT_WRONG_PASSWORD: "network.STAT_WRONG_PASSWORD",
            network.STAT_NO_AP_FOUND: "network.STAT_NO_AP_FOUND",
            network.STAT_ASSOC_FAIL: "network.STAT_ASSOC_FAIL",
            network.STAT_BEACON_TIMEOUT: "network.STAT_BEACON_TIMEOUT",
            network.STAT_HANDSHAKE_TIMEOUT: "network.STAT_HANDSHAKE_TIMEOUT"
        }

        return f'''
ap_enabled : {cls.ap_enabled()},
wlan_enabled : {cls.wlan_enabled()},
is connected : {cls.wlan_status()},
current_network : {cls.current_network()},
cls.preferred_networks : {cls.preferred_networks},
availlable networks : {cls.available_networks},
wlan_ifconfig: {cls.wlan().ifconfig()},
wlanstatusstring : {cls.status_dict.get(cls.wlan().status(), cls.wlan().status())}

'''



def setup():
    """Initialise :class:`WifiManager` and connect to a network."""


    def _attempt(timer):
        if not WifiManager.connected:
            WifiManager.setup_network()
        else:
            timer.deinit()

    _setup_timer = Timer(12)
    _setup_timer.init(period=1000, mode=Timer.PERIODIC, callback=_attempt)

    
