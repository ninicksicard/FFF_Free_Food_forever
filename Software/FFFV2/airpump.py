from machine import Pin,PWM, Timer
import sys
led = sys.modules["System"].system.led
class AirPump:
    def __init__(self, pin, pump_on_time_min, pump_off_time_min):
        self.air_pump = PWM(Pin(pin, Pin.OUT))
        self.pump_on_time_ms = pump_on_time_min*60*1000
        self.pump_off_time_ms = pump_off_time_min*60*1000
        self.timer = Timer(9)
        
    def pump_on(self, *args):
        led.value(not led.value())
        self.air_pump.init(freq=21000, duty_u16=55000)
        self.timer.init(mode=Timer.ONE_SHOT, period=self.pump_on_time_ms, callback=self.pump_off)
        
    def pump_off(self, *args):
        led.value(not led.value())
        self.air_pump.init(freq=21000,duty_u16=0)
        self.timer.init(mode=Timer.ONE_SHOT, period=self.pump_off_time_ms, callback=self.pump_on)
    
    def start(self):
        self.pump_on()