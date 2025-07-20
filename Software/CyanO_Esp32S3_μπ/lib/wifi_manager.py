import json
import time
import ure
import os
import _thread
import network
import socket




def unquote(s):
    # Simple unquote implementation for %20 (space)
    # Extend this function based on your needs
    s = s.replace("+", " ")  # Treat plus (+) as space
    s = s.replace("%20", " ")
    s = s.replace("%21", "!")
    s = s.replace("%22", '"')
    s = s.replace("%27", "'")
    # Add more replacements as needed for your application
    return s

class WifiManager:
    _server_socket = None
    
    
#     wlan_sta = network.WLAN(network.STA_IF)
#     wlan_ap = network.WLAN(network.AP_IF)
    
    
    _ap_start_policy = "never"
    config_file = '/networks.json'
    _lock = _thread.allocate_lock()
    print_queue = []
    
    @classmethod
    def start_server(cls):
        cls.print_queue.append("start server")
        if cls._server_socket is not None:
            # Close the existing socket if it's already open
            cls.print_queue.append(f"closing existing socket")
            cls._server_socket.close()
            time.sleep(1)

        # Create a new socket and listen for connections
        addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
        cls.print_queue.append(f"address info : {addr}")
        try:
            
            cls._server_socket = socket.socket()
            cls.print_queue.append(f" server socket: {cls._server_socket}")
            cls._server_socket.bind(addr)
            cls.print_queue.append("socket binded")
            cls._server_socket.listen(1)
            cls.print_queue.append(f"Listening on : {addr}")
        except Exception as e:
            cls.print_queue.append(f"error : {e}")
        
        cls.print_queue.append(f" server socket is : {cls._server_socket }")
        if cls._server_socket :
            while True:
                conn, addr = cls._server_socket.accept()
                cls.print_queue.append('Got a connection from', addr)
                request = conn.recv(1024).decode("utf-8")
                cls.print_queue.append(f'Request: {request}')

                # Simple parsing to determine request method and path
                first_line = request.split('\r\n')[0] if request else ""
                method, path, _ = first_line.split(' ') if first_line else ("", "", "")

                if path.startswith("/configure"):
                    # The form has been submitted; extract data and attempt to connect
                    cls.handle_configure(conn, request)
                else:
                    # Serve the main page with network selection
                    cls.handle_root(conn)
        
            conn.close()    

    
    @classmethod
    def stop_server(cls):
        if cls._server_socket:
            cls._server_socket.close()
        cls._server_socket = None

            
    @classmethod
    def handle_root(cls, client):
        cls.wlan().active(True)
        networks = cls.wlan().scan()
        networks_html = ''.join('<option value="{0}">{0}</option>'.format(ssid.decode('utf-8')) for ssid, *_ in networks)
        
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
        try:
            with open(cls.config_file, "r") as f:
                config = json.load(f)
                cls.preferred_networks = config.get('known_networks', [])
                cls.ap_config = config.get("access_point", {})
                if config.get("schema", 0) != 2:
                    cls.print_queue.append("Did not get expected schema [2] in JSON config.")
        except Exception as e:
            cls.print_queue.append(f"Failed to load config file, error: {e}")
            cls.preferred_networks = []
            cls.ap_config = {}
            return False
        return True

    @classmethod
    def setup_network(cls):
        if not cls.load_config():
            return  # Configuration failed to load

        cls.wlan().active(True)
        available_networks = [ssid.decode() for ssid, *_ in cls.wlan().scan()]

        # Find the first preferred network that is available
        # Initialize connection to None
        connection = None

        # Iterate over preferred networks to find the first available one
        for pref in cls.preferred_networks:
            if pref["ssid"] in available_networks:
                connection = pref
                break

        if connection:
            cls.print_queue.append(f"Attempting to connect to network {connection['ssid']}...")
            if cls.connect_to(ssid=connection['ssid'], password=connection['password']):
                cls.print_queue.append(f"Successfully connected to {connection['ssid']}")
                cls.print_queue.append(f" connection parameters : {WifiManager.wlan().ifconfig()}")
                cls.accesspoint().active(False)  # Disable AP mode if connected
                
                
                return
            else:
                cls.print_queue.append(f"Failed to connect to {connection['ssid']}")

        # If no known networks are found or connection fails, check AP mode policy
        cls.print_queue.append("start ap mode")
        cls.start_ap_mode()
        cls.print_queue.append("start ap mode done")
        
        
    @classmethod
    def start_ap_mode(cls):
        cls.print_queue.append("Starting AP mode...")
        if cls._ap_start_policy != "never":
            cls.accesspoint().active(True)
            essid = cls.ap_config.get("config", {}).get("essid", "MicroPython-AP")
            password = cls.ap_config.get("config", {}).get("password", "micropythoN")
            cls.print_queue.append(f"configuring AP Mode Enabled: ESSID='{essid}', PASSWORD='{password}'")
            cls.accesspoint().config(essid=essid, password=password)
            cls.print_queue.append(f"AP Mode Enabled: ESSID='{essid}', PASSWORD='{password}'")
            
            # Start serving the webpage in a new thread
            _thread.start_new_thread(cls.start_server, ())
        else:
            cls.print_queue.append(f"AP mode not started due to policy:{ cls._ap_start_policy}")

            
    @classmethod
    def is_connected(cls):
        """Check if the station is connected to a WiFi network."""
        return cls.wlan().isconnected()

    @classmethod
    def current_network(cls):
        """Get the SSID of the currently connected network."""
        if cls.is_connected():
            return cls.wlan().config('essid')
        else:
            return None

    @classmethod
    def is_ap_active(cls):
        """Check if the access point mode is active."""
        return cls.accesspoint().active()
    
    @classmethod
    def start_managin(cls):
        # Start the managing thread
        _thread.start_new_thread(cls.manage, ())

    @classmethod
    def manage(cls):
        while True:
            with cls._lock:
                status = cls.wlan().status()
                if (status != network.STAT_GOT_IP) or (cls.wlan().ifconfig()[0] == '0.0.0.0'):
                    cls.setup_network()
            time.sleep(5)  # Pause 10 seconds

    @classmethod
    def wlan(cls):
        return network.WLAN(network.STA_IF)
    
    @classmethod
    def wants_accesspoint(cls) -> bool:
        static_policies = {"never": False, "always": True}
        if cls._ap_start_policy in static_policies:
            return static_policies[cls._ap_start_policy]
        # By default, that leaves "Fallback"
        return cls.wlan().status() != network.STAT_GOT_IP  # Discard intermediate states and check for not connected/ok
    
    
    @classmethod
    def accesspoint(cls):
        return network.WLAN(network.AP_IF)

    
    @classmethod
    def connect_to(cls, *, ssid, password, **kwargs):
        
        cls.wlan().connect(ssid, password, **kwargs)
        for _ in range(20):  # 5-second timeout
            if cls.wlan().isconnected():
                return True
            time.sleep(0.5)
        return False

