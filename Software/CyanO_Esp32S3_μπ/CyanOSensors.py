import _thread
import utime
from machine import Pin
from operator import gt, lt, ge, le, eq, ne
import _thread

class Sensor:
    _instances = []  # Class-level list to track all instances
    _lock = _thread.allocate_lock()  # Create a lock for thread-safe operations

    def __init__(self, pin, condition_func, threshold_value, interval_seconds):
        self.pin = Pin(pin, Pin.IN)
        self.condition_func = condition_func
        self.threshold_value = threshold_value
        self.interval = interval_seconds
        self.result = False
        
        # Use the lock to safely add this instance to the list of instances
        with Sensor._lock:
            Sensor._instances.append(self)
        
        # Start the background check in a new thread
        _thread.start_new_thread(self.check, ())

    def check(self):
        while True:
            read_value = self.pin.value()
            self.result = self.condition_func(read_value, self.threshold_value)
            utime.sleep(self.interval)

    def __bool__(self):
        return self.result

    @classmethod
    def all(cls):
        # You might also want to protect this read operation with the lock,
        # depending on how it's used and what it's used for.
        with cls._lock:
            return all(instance for instance in cls._instances)
