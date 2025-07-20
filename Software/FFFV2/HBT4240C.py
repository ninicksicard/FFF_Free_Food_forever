from ESP32S3_IO_MAPPING import *
from machine import Pin, Timer, PWM
import time
import json

class StepperMotor:
    def __init__(self, step_pin, dir_pin, enable_pin, alarm_pin, limit_min_pin, limit_max_pin, steps_per_revolution=200):
        self.step = Pin(step_pin, Pin.OUT)
        self.direction = Pin(dir_pin, Pin.OUT)
        self.enable = Pin(enable_pin, Pin.OUT)
        self.alarm = Pin(alarm_pin, Pin.IN, pull=Pin.PULL_DOWN)
        self.limit_min = Pin(limit_min_pin, Pin.IN, pull=Pin.PULL_UP)
        self.limit_min_debounce = self.limit_min.value()
        
        self.limit_max = Pin(limit_max_pin, Pin.IN, pull=Pin.PULL_UP)
        self.limit_max_debounce = self.limit_max.value()
        self.steps_per_revolution = steps_per_revolution
        
        self.position_saved = 1
        
        self.max_position = 0 
        self.position = 0  # Current position in pulses
        self.speed = 0  # Current speed in pulses/sec
        self.acceleration = 0  # Acceleration in pulses/sec^2
        self.target_speed = 0  # Target speed in pulses/sec
        self._load_params()
        
        
        self.target_position = self.position  # Target position in pulses
        self.last_step_time = time.ticks_us()  # Last step timestamp

        # Timer for asynchronous control
        self.timer = Timer(10)
        self.timer.init(period=10, mode=Timer.PERIODIC, callback=self._update)
        self.timer_active = True

        # Initialize the motor as disabled
        self.enable.value(1)
        self.step_interval = 0
        self.steps_to_do = 0

    def set_direction(self, clockwise):
        self.direction.value(1 if clockwise else 0)

    def enable_motor(self, enable):
        self.enable.value(0 if enable else 1)

    def do_step(self):
        current_time = time.ticks_us()
        self.step_interval = int(1_000_000 / self.target_speed) if self.target_speed > 0 else 0

        if self.step_interval != 0:
            time_elapsed = time.ticks_diff(current_time, self.last_step_time)

            if time_elapsed >= self.step_interval:
                self.steps_to_do = min(int(time_elapsed // self.step_interval), int(abs(self.target_position - self.position)))

                for _ in range(self.steps_to_do):
                    self.step.value(1)
                    time.sleep_us(2)  # Pulse width
                    self.step.value(0)
                    time.sleep_us(2)

                self.position += (1 if self.direction.value() == 1 else -1) * self.steps_to_do
                
                self.last_step_time = time.ticks_us()

    def set_speed(self, speed):
        self.target_speed = speed

    def set_acceleration(self, acceleration):
        self.acceleration = acceleration

    def move_to(self, target_position):
        self.target_position = target_position

    def stop(self):
        self.target_position = self.position


    def _update(self, timer):
        if self.limit_min.value() == 0:
            if self.limit_min_debounce != 0:        
                #print("min_limit_switch")
                pass
            if self.position != 0:
                self.position = -20
                self.target_position = 0

        if self.limit_max.value() == 0:
            if self.limit_max_debounce != 0:        
                #print("max_limit_switch")
                pass
            if self.position != self.max_position:
                self.max_position = self.position-20
                self.target_position = self.max_position

        if self.target_position is not None:
            error = self.target_position - self.position

            direction = 1 if error > 0 else -1
            self.set_direction(clockwise=(direction > 0))

            if abs(error) > 0:
                self.do_step()
                self.position_saved = 0
            else:
                #if not self.position_saved :
                    #self._save_position()  # Persist position
                    #self.position_saved = 1
                    
                self.last_step_time = time.ticks_us()
        self.limit_min_debounce = self.limit_min.value()
        self.limit_max_debounce = self.limit_max.value()

#     def _save_position(self):
#         with open("stepper_position.json", "w") as f:
#             json.dump({"position": self.position, "max_position": self.max_position, "target_speed": self.target_speed, "speed": self.speed, "acceleration": self.acceleration}, f)
#         
    def _load_params(self):
        
        try:
            with open("stepper_position.json", "r") as f:
                data = json.load(f)
                self.position = data.get("position", 0)
                self.max_position = data.get("max_position", 83329)
                self.speed = data.get("speed", 1000)
                self.target_speed = data.get("target_speed", 1000)
                self.acceleration = data.get("acceleration", 100)
        except (OSError, ValueError):
            return None

    def convert_position(self, unit="rad"):
        if unit == "rad":
            return (self.position / self.steps_per_revolution) * 2 * 3.14159265359
        elif unit == "deg":
            return (self.position / self.steps_per_revolution) * 360
        elif unit == "turns":
            return self.position / self.steps_per_revolution
        else:
            raise ValueError("Invalid unit")

    def convert_speed(self, unit="rad/s", lead_screw_pitch=None):
        if unit == "rad/s":
            return (self.speed / self.steps_per_revolution) * 2 * 3.14159265359
        elif unit == "deg/s":
            return (self.speed / self.steps_per_revolution) * 360
        elif unit == "turns/s":
            return self.speed / self.steps_per_revolution
        elif unit == "linear" and lead_screw_pitch is not None:
            return (self.speed / self.steps_per_revolution) * lead_screw_pitch
        else:
            raise ValueError("Invalid unit or missing lead_screw_pitch")

    def shutdown(self):
        self.timer.deinit()
        self.enable_motor(False)

# Example usage
if __name__ == "__main__":
    try:
        stepper = StepperMotor(step_pin=DO10, dir_pin=DO8, enable_pin=DO9, alarm_pin=DI5, limit_min_pin=DI2, limit_max_pin=DI0, steps_per_revolution=200)

        # Enable the motor
        stepper.enable_motor(True)


        # Move to position
        stepper.move_to(0)

        # Wait a bit and observe holding position
        time.sleep(3)

    except KeyboardInterrupt:
        stepper.shutdown()


