import picoweb
import ujson

class UserInterface:
    def __init__(self, settings, sensors, mode, time_sync):
        # Initialize web server
        self.app = picoweb.WebApp(__name__)

        # Store settings, sensors, mode, and time_sync
        self.settings = settings
        self.sensors = sensors
        self.mode = mode
        self.time_sync = time_sync

    def setup_ui(self):
        # Setup routes
        @self.app.route("/")
        def index(req, resp):
            # Serve the main page
            yield from picoweb.start_response(resp, content_type="text/html")
            with open('templates/index.html', 'r') as file:
                html = file.read()
            yield from resp.awrite(html)

        @self.app.route("/static/script.js")
        def script(req, resp):
            # Serve the JavaScript file
            yield from picoweb.start_response(resp, content_type = "text/javascript")
            script = """
            function updateSensorData() {
                // JavaScript code to fetch sensor data from the server and update the display
            }

            function extract() {
                // JavaScript code to send an HTTP request to the server when the button is clicked
            }

            // Call updateSensorData every second
            setInterval(updateSensorData, 1000);
            """
            yield from resp.awrite(script)

        @self.app.route("/sensors")
        def sensors(req, resp):
            # Serve sensor data
            yield from picoweb.jsonify(resp, self.sensors.check_sensors())

        @self.app.route("/extract", method="POST")
        def extract(req, resp):
            # Handle extract command
            duration = yield from req.read_form_data()
            print("Extract command received with duration:", duration)
            yield from picoweb.start_response(resp)
            yield from resp.awrite("OK")

        @self.app.route("/data")
        def data(req, resp):
            # Get the data
            sensors = self.sensors.check_sensors()
            mode = self.mode.get_current_mode()
            cam_relay_values = self.mode.get_cam_relay_values()
            time = self.time_sync.get_time()

            # Create a dictionary with the data
            data = {
                "sensors": sensors,
                "mode": mode,
                "cam_relay_values": cam_relay_values,
                "time": time
            }

            # Serve the data as a JSON object
            yield from picoweb.jsonify(resp, data)

    def run(self):
        # Run the web server
        self.app.run(host=self.settings["host"], port=self.settings["port"])
