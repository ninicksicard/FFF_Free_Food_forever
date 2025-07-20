import json
import time
import ure
import os
from _thread import start_new_thread
import network
import socket
from machine import reset
import sys

led = sys.modules["System"].system.led



# allow time to debug device
time.sleep(1)  # Pause 1 seconds


def unquote(s):
    """
    Simple unquote implementation for special caracters
    :param s:
    :return:
    """
    s = s.replace("+", " ")
    s = s.replace("%20", " ")
    s = s.replace("%21", "!")
    s = s.replace("%22", '"')
    s = s.replace("%27", "'")
    return s


class WifiManager:
    """
    Wi-Fi manager object
    """
    ap_config = None
    preferred_networks = None
    _server_socket = None

    #     wlan_sta = network.WLAN(network.STA_IF)
    #     wlan_ap = network.WLAN(network.AP_IF)

    _ap_start_policy = "never"
    config_file = '/networks.json'
    available_networks = []
    verbose = False
    connected = False

    @classmethod
    def start_server(cls):
        """
        start the server
        """
        if cls._server_socket is not None:
            # Close the existing socket if it's already open
            cls._server_socket.close()
            time.sleep(0.1)

        # Create a new socket and listen for connections
        addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
        try:

            cls._server_socket = socket.socket()
            cls._server_socket.bind(addr)
            cls._server_socket.listen(1)
        except Exception as e:
            pass


        if cls._server_socket:
            try:
                while True:
                    led.value(not led.value())
                    conn, addr = cls._server_socket.accept()
                    request = conn.recv(1024).decode("utf-8")

                    # Simple parsing to determine request method and path
                    first_line = request.split('\r\n')[0] if request else ""
                    method, path, _ = first_line.split(' ') if first_line else ("", "", "")

                    if path.startswith("/configure"):
                        # The form has been submitted; extract data and attempt to connect
                        cls.handle_configure(conn, request)
                    else:
                        # Serve the main page with network selection
                        cls.handle_root(conn)
            except KeyboardInterrupt:
                return

    @classmethod
    def stop_server(cls):
        """
        close the server socket
        """
        if cls._server_socket:
            cls._server_socket.close()
        cls._server_socket = None

    @classmethod
    def handle_root(cls, client):
        """
        generate the web ui handling the wifi connection
        :param client:
        """
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
        """
        manage the request to configure the connection
        :param client:
        :param request:
        """
        # You'll need to adjust this to properly parse POST data from the request body
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
        """
        load the configuration file of the Wi-Fi
        :return:
        """
        try:
            with open(cls.config_file, "r") as f:
                config = json.load(f)
                cls.preferred_networks = config.get('known_networks', [])
                cls.ap_config = config.get("access_point", {})
                if config.get("schema", 0) != 2:
                    append_to_queue("Did not get expected schema [2] in JSON config.")
        except Exception as e:
            cls.preferred_networks = []
            cls.ap_config = {}
            return False
        return True

    @classmethod
    def setup_network(cls, verbose=None):
        """
        start the network howerver it can work
        :return:
        """
        if verbose :
            cls.verbose = verbose
            
        if not cls.load_config():
            time.sleep(0.1)
            return  # Configuration failed to load
        
        cls.activate_wlan()
        
        for i in range(5):
            led.value(not led.value())
            cls.available_networks = []
            cls.available_networks = [ssid.decode() for ssid, *_ in cls.wlan().scan()]
            if cls.available_networks == []:
                time.sleep(0.1)
            else:
                time.sleep(0.1)
                break
        if cls.available_networks == []:
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
                time.sleep(0.1)
                return
            
            # log the attemp parameters
            time.sleep(0.1)
            
            # attemps to connect
            if cls.connect_to(ssid=connection['ssid'], password=connection['password']):
                # log if it has succedded and with which parameters
                cls.connected = True
                time.sleep(0.1)
                
            else:
                cls.connected = False
                # log if it failed and to which ssid
                time.sleep(0.5)

      

    @classmethod
    def wlan_status(cls):
        """Check if the station is connected to a Wi-Fi network."""
        return cls.wlan().isconnected()

    @classmethod
    def current_network(cls):
        """Get the SSID of the currently connected network."""
        if cls.wlan_status():
            return cls.wlan().config('essid')
        else:
            return None

    @classmethod
    def ap_enabled(cls):
        """Check if the access point mode is active."""
        return cls.accesspoint().active()

    @classmethod
    def wlan_enabled(cls):
        """Check if the access point mode is active."""
        return cls.wlan().active()

    @classmethod
    def activate_wlan(cls):
        if cls.ap_enabled():
            cls.accesspoint().active(False)
            time.sleep(0.5)  # Allow AP mode to fully deactivate
        if not cls.wlan_enabled():
            cls.wlan().active(True)
            time.sleep(0.5)
            try:
                while not cls.wlan_enabled():
                    time.sleep(0.1)
                    led.value(not led.value())
            except KeyboardInterrupt:
                return
            time.sleep(0.5)

    @classmethod
    def start_ap_mode(cls):
        """
        start the network as a hotspot
        """
        if cls._ap_start_policy != "never":
            cls.accesspoint().active(True)
            essid = cls.ap_config.get("config", {}).get("essid", "MicroPython-AP")
            password = cls.ap_config.get("config", {}).get("password", "micropythoN")
            
            cls.accesspoint().config(essid=essid, password=password)
            


            # Start serving the webpage in a new thread
            start_new_thread(cls.start_server, ())



    @classmethod
    def wlan(cls):
        """
        define the network type
        :return:
        """
        return network.WLAN(network.STA_IF)
    
    @classmethod
    def accesspoint(cls):
        """
        define the network type
        :return:
        """
        return network.WLAN(network.AP_IF)
    
    @classmethod
    def disconnect(cls):
        if cls.wlan_status():
            cls.wlan().disconnect()
            try:
                while cls.wlan_status():
                    time.sleep(0.1)
            except KeyboardInterrupt:
                return

    @classmethod
    def deactivate(cls):
        if cls.wlan_enabled():
            cls.wlan().active(False)
            try:
                while cls.wlan_enabled():
                    time.sleep(0.1)
            except KeyboardInterrupt:
                return
            
    @classmethod
    def sync_time():
        """
        Sync the device time with an NTP server.
        """
        try:
            ntptime.settime()  # Sync RTC with NTP
            rtc = RTC()
            current_time = rtc.datetime()
            formatted_time = "{:02}-{:02}-{:02} {:02}:{:02}:{:02}".format(
                current_time[0], current_time[1], current_time[2],  # Year, Month, Day
                current_time[4], current_time[5], current_time[6]   # Hour, Minute, Second
            )
            led.value(not led.value())
        except Exception as e:
            pass

    @classmethod
    def connect_to(cls, *, ssid, password, **kwargs):
        """
        attempt connection to the specified network.
        :param ssid:
        :param password:
        :param kwargs:
        :return:
        """
        if not cls.wlan_enabled():
            return
        if cls.ap_enabled():
            return
        if cls.wlan().isconnected():
            return
        
        cls.wlan().connect(ssid, password, **kwargs)
        for _ in range(5):  # 5-second timeout
            led.value(not led.value())
            if cls.wlan().isconnected():
                return True
            time.sleep(0.25)
        return False

    @classmethod
    def status(cls):

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



def setup(_system):
    _system.wifimanager = WifiManager()
    while not _system.wifimanager.connected:
        led.value(not led.value())
        _system.wifimanager.setup_network()
    
