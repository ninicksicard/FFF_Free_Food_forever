from random import randint
from machine import Pin, PWM
from utime import sleep
import Angle


class DS3218pro:
    _minlength = 500000
    _maxlength = 2500000
    _rangelength = _maxlength-_minlength
    def __init__(self, pin):
        self.servo = PWM(Pin(pin, Pin.OUT), freq=60, duty_u16=int(1.5 / 20 * 65535))  # Middle position duty cycle
        
    def move(self, angle):
        duty = int((angle.hom * self._rangelength) + self._minlength)
        self.servo.init(freq=60, duty_ns=duty)

à
# Define pin for servo
D2 = 3

angle = Angle.Angle(0, mode = 1)

# Initialize servo on pin D2. Initial frequency doesn't matter much, as we'll change it.
servo = DS3218pro(D2)


# Function to test servo at different frequencies
def test_angles():
    increments = 5 
    for angle_deg in range(0, 180, 5):
        
        angle.set(angle_deg,1)
        servo.move(angle)   # Set new frequency
        sleep(1)
        
        percent = int(angle.hom*100)
        print(f"Testing duty: {angle_deg}°, \n as homogeneous : {angle.hom}, \n as percentage : {percent}%")
        
        
        
def main():
    while True :
        test_angles()  # Test servo with a range of frequencies

if __name__ == "__main__":
    main()

