"""PWM motor driver with simple sequencing support."""

import json
from machine import Pin, PWM
# Import print_control so the custom print replacement is active
try:
    from print_control import custom_print as print #noqa: F401
except Exception:
    pass
import time

class MOTORDriver:
    """Control a bidirectional DC motor using two PWM outputs."""

    def __init__(
        self,
        pwm_pin_A: int,
        pwm_pin_B: int,
        freq: int = 8000,
        cw_range: tuple[int, int] = (0, 65535),
        ccw_range: tuple[int, int] = (0, 65535),
        stop_pwm: int = 0,
        loop_file: str = "motor_loop.json",
    ) -> None:
        """Initialise the driver and load an optional sequence file."""
        self.pwm_a = PWM(Pin(pwm_pin_A, Pin.OUT, pull=Pin.PULL_DOWN))
        self.pwm_a.freq(freq)
        self.pwm_b = PWM(Pin(pwm_pin_B, Pin.OUT, pull=Pin.PULL_DOWN))
        self.pwm_b.freq(freq)
        self.duty_u16 = 0
        self.cw_range = cw_range
        self.ccw_range = ccw_range
        self.stop_pwm = stop_pwm

        self.state = False
        self.standby = True
        self.speed = 0

        self.sequence_index = 0
        self.last_time = time.ticks_ms()
        self.loop_file = loop_file
        self.loop = self.load_loop_from_file() or [
            [0.5, 100],
            [1, -100],
            [0.3, 0],
            [2, 100],
            [2, 0]
        ]

        self.update_pwm()

    def load_loop_from_file(self, *args, **kwargs):
        """Return a sequence list loaded from ``self.loop_file`` or ``None``."""
        try:
            with open(self.loop_file, 'r') as file:
                data = json.load(file)
                if isinstance(data, list) and all(isinstance(item, list) and len(item) == 2 for item in data):
                    print(f"Loaded loop sequence from {self.loop_file}")
                    return data
                else:
                    print(f"Invalid loop format in {self.loop_file}")
        except FileNotFoundError:
            print(f"Loop file {self.loop_file} not found. Using default loop.")
        except Exception as e:
            print(f"Error loading loop file: {e}")
        return None

    def save_loop_to_file(self, loop_data, *args, **kwargs):
        """Persist ``loop_data`` to ``self.loop_file``."""
        try:
            with open(self.loop_file, 'w') as file:
                json.dump(loop_data, file)
                print(f"Loop sequence saved to {self.loop_file}")
        except Exception as e:
            print(f"Error saving loop file: {e}")

    def toggle_state(self, *args, **kwargs):
        """Flip the motor's running state."""
        self.state = not self.state

    def set_standby(self, standby, *args, **kwargs):
        """Enter or leave standby mode."""
        self.standby = standby

    def set_state(self, state, *args, **kwargs):
        """Set running state to ``state``."""
        self.state = state

    def set_speed(self, speed, *args, **kwargs):
        """Set the motor speed in percent (-100..100)."""
        self.speed = speed

    def update_pwm(self, *args, **kwargs):
        """Update PWM outputs based on current state."""
        standby = self.standby
        state = self.state
        speed = self.speed

        if standby or not state or speed == 0:
            self.pwm_a.duty_u16(self.stop_pwm)
            self.pwm_b.duty_u16(self.stop_pwm)
            self.duty_u16 = self.stop_pwm
            return

        self.duty_u16_a = self.map_range(abs(self.speed), 0, 100, *self.cw_range)
        self.duty_u16_b = self.map_range(abs(self.speed), 0, 100, *self.ccw_range)

        if self.speed > 0:  # Clockwise
            self.pwm_a.duty_u16(int(self.duty_u16_a))
            self.pwm_b.duty_u16(self.stop_pwm)
        elif self.speed < 0:  # Anticlockwise
            self.pwm_a.duty_u16(self.stop_pwm)
            self.pwm_b.duty_u16(int(self.duty_u16_b))

    def play_sequence(self, *args, **kwargs):
        """Run the next step of the configured movement sequence."""
        duration, speed = self.loop[self.sequence_index]
        now = time.ticks_ms()
        if time.ticks_diff(now, self.last_time) > int(duration * 1000):
            self.last_time = now
            self.sequence_index = (self.sequence_index + 1) % len(self.loop)
            duration, speed = self.loop[self.sequence_index]
            self.set_speed(speed)
        self.update_pwm()

    @staticmethod
    def map_range(value, in_min, in_max, out_min, out_max):
        """Return ``value`` mapped from one range to another."""
        return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

    def stop(self):
        """Stop the motor and clear PWM outputs."""
        print(f"MOTOR - stop")
        self.set_state(False)
        self.set_speed(0)
        self.update_pwm()

    def deinit(self):
        """Disable PWM peripherals."""
        print(f"MOTOR - deinit")
        self.pwm_a.deinit()
        self.pwm_b.deinit()
