# Import necessary libraries
import wifi_manager
import time_sync
import notifications
import sensors
import modes
import user_interface
import file_manager
import time
# Import necessary libraries
import cli  # Import the CLI module


class SpirulinaBioreactor:
    def __init__(self):
        # Initialize components
        self.wifi_manager = wifi_manager.WifiManager()
        self.time_sync = time_sync.TimeSync()
        self.notifications = notifications.Notifications()
        self.sensors = sensors.Sensors()
        self.modes = modes.Modes()
        self.user_interface = user_interface.UserInterface()
        self.file_manager = file_manager.FileManager()

        self.settings: dict = {}

        # Initialize settings and parameters
        self.default_settings = {
            "wifi": {},
            "time": {},
            "notifications": {},
            "sensors": {},
            "modes": {},
            "ui": {}
        }

    def setup(self):
        # Load settings
        loaded_settings = self.file_manager.load_settings()

        # Merge loaded settings with default settings
        self.settings = {**self.default_settings, **loaded_settings}

        # Setup WiFi
        self.wifi_manager.setup_wifi(self.settings["wifi"])

        # Sync time
        self.time_sync.sync_time(self.settings["time"])

        # Setup sensors
        self.sensors.setup_sensors(self.settings["sensors"])

        # Setup modes
        self.modes.setup_modes(self.settings["modes"])

        # Setup user interface
        self.user_interface.setup_ui(self.settings["ui"])

    def loop(self):
        # Main loop
        while True:
            # Check sensors
            self.sensors.check_sensors()

            # Update modes based on sensor readings
            self.modes.update_modes()

            # Update user interface
            self.user_interface.update_ui()

            # Send notifications if necessary
            self.notifications.send_notifications()

            # Sleep for a bit to avoid overloading the MCU
            time.sleep(0.1)

    def shutdown(self):
        # Save settings before shutdown
        self.file_manager.save_settings(self.settings)

if __name__ == "__main__":
    # Create a new bioreactor
    bioreactor = SpirulinaBioreactor()

    # Setup the bioreactor
    bioreactor.setup()

    # Create a new CLI
    cli = cli.CLI('/dev/ttyUSB0', 115200)

    # Start the CLI in a separate thread
    cli_thread = threading.Thread(target=cli.run)
    cli_thread.start()

    # Start the main loop
    bioreactor.loop()

    # Shutdown the bioreactor
    bioreactor.shutdown()