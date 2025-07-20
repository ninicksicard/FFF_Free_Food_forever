import machine
import uos


class CLI:
    def __init__(self, uart):
        self.uart = uart

    def run(self):
        while True:
            if self.uart.any():
                # Read a line from the UART
                line = self.uart.readline()

                # Decode the line to a string and strip the trailing newline
                command = line.decode().strip()

                # Handle the command
                self.handle_command(command)

    def handle_command(self, command):
        # For now, just print the command
        print("Received command:", command)

# Create a UART object
uart = machine.UART(1, baudrate=115200)

# Create a CLI object
cli = CLI(uart)

# Run the CLI
cli.run()
