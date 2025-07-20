"""Entry point and runtime logic for the example device controller."""

from ESP32S3_IO_MAPPING import *
# Import print_control so the custom print replacement is active
try:
    from print_control import custom_print as print #noqa: F401
    from print_control import set_print_enabled 
except Exception:
    pass
import time
from machine import Pin, PWM, Timer
from MOTOR import MOTORDriver
import wifi_manager
import MYmqtt
import json
import sys


mqttclient = None
wait_timer = None
mqtt_connect_timer = None
retry_timer = None
main_timer = None
wait_count = 0

motor_PIN_A = AO0
motor_PIN_B = AO1
LED_PIN = DO4
LED = None
BUTTON_PIN = DI3

# Initialize the servo motor (PWM) and potentiometers (ADC)
motor = None

# MQTT topic for receiving new loop data
MQTT_LOOP_TOPIC = 'home/template/device/loop'


BUTTON = None
last_button_ms = 0
DEBOUNCE_MS = 200

# Create MQTT client
# will be initialized in run()

# Function to set motor state via MQTT
def mqtt_set_state(self, state: str, *args):
    """Handle MQTT command to set the motor state."""
    print(f"mqtt_set_state received {type(state)} instead. state:{state}, args:{args}")
    _state = state in sys.modules[__name__].mqttclient.ON
    sys.modules[__name__].set_state(_state)
    sys.modules[__name__].set_standby(not _state)





def run():
    """Initialise hardware, connect to Wi-Fi and enter the main loop."""

    try:
        print_control.set_print_enabled(True)
    except Exception:
        pass
    print("booting")
    time.sleep(1)

    sys.modules[__name__].LED = Pin(LED_PIN, Pin.OUT)
    sys.modules[__name__].motor = MOTORDriver(pwm_pin_A=motor_PIN_A, pwm_pin_B=motor_PIN_B)
    
    # Main loop for motor sequence playback and MQTT message checking
    def loop_play_sequence():
        """Advance the current motor sequence."""
        sys.modules[__name__].motor.play_sequence()
    sys.modules[__name__].loop_play_sequence = loop_play_sequence
    
    # Function to toggle motor state
    def toggle_state(*args):
        """Callback to toggle the motor state."""
        global last_button_ms
        now = time.ticks_ms()
        if time.ticks_diff(now, last_button_ms) < DEBOUNCE_MS:
            return
        last_button_ms = now
        print(f"toggle_state received args:{args}")
        sys.modules[__name__].motor.toggle_state()
        set_standby(not sys.modules[__name__].motor.state)
    sys.modules[__name__].toggle_state = toggle_state

    # Function to set motor state
    def set_state(state: bool, *args):
        """Set the motor ``state`` and update the LED."""
        print(f"set_state received {type(state)}, state:{state}, args:{args}")
        sys.modules[__name__].motor.set_state(state)
        print(f"set_state - motor.state : {motor.state}")
        sys.modules[__name__].LED.value(sys.modules[__name__].motor.state)
    sys.modules[__name__].set_state = set_state

    # Function to set standby mode
    def set_standby(standby: bool, *args):
        """Enable or disable standby mode."""
        print(f"set_standby received {type(standby)} instead. standby:{standby}, args:{args}")
        sys.modules[__name__].motor.set_standby(standby)
        print(f"set_standby - motor.standby : {motor.standby}")
        # Function to handle receiving new loop data via MQTT
    sys.modules[__name__].set_standby = set_standby
        
    def mqtt_receive_loop(topic, message):
        """Receive a new motion sequence via MQTT."""
        print(f"Received loop data on topic {topic}: {message}")
        try:
            # Parse the received message as JSON
            new_loop = json.loads(message)
            
            # Validate the structure of the loop data
            if isinstance(new_loop, list) and all(
                isinstance(item, list) and len(item) == 2 and isinstance(item[0], (int, float)) and isinstance(item[1], int)
                for item in new_loop
            ):
                sys.modules[__name__].motor.save_loop_to_file(new_loop)  # Save the new loop to the file
                sys.modules[__name__].motor.loop = sys.modules[__name__].motor.load_loop_from_file() or sys.modules[__name__].motor.loop  # Reload the loop
                print("New loop data successfully updated and reloaded.")
            else:
                print("Invalid loop format received.")
        except Exception as e:
            print(f"Error processing loop data: {e}")
    sys.modules[__name__].mqtt_receive_loop = mqtt_receive_loop
            
    sys.modules[__name__].BUTTON = Pin(BUTTON_PIN, Pin.IN, pull=Pin.PULL_UP)
    sys.modules[__name__].BUTTON.irq(trigger=Pin.IRQ_RISING, handler=sys.modules[__name__].toggle_state)

    sys.modules[__name__].mqttclient = MYmqtt.myMQTT()

    sys.modules[__name__].mqttclient.topics.update({
        MYmqtt.Topic('home/template/device'): [mqtt_set_state, None],
        MYmqtt.Topic(MQTT_LOOP_TOPIC): [mqtt_receive_loop, None],
    })


    sys.modules[__name__].wifi_manager.setup()

    sys.modules[__name__].mqttclient.reconnect = True
    sys.modules[__name__].wait_count = 0

    def _check_wifi(timer):
        if wifi_manager.WifiManager.connected:
            timer.deinit()
            sys.modules[__name__].mqttclient.connect_mqtt()
            sys.modules[__name__].wait_timer.init(period=1000, mode=Timer.PERIODIC, callback=sys.modules[__name__]._wait_mqtt)
        else:
            print("wait_wifi")
    sys.modules[__name__]._check_wifi = _check_wifi
    
    
    def _wait_mqtt(timer):

        if sys.modules[__name__].mqttclient.client:
            timer.deinit()
            sys.modules[__name__].wait_count = 0
            sys.modules[__name__].mqttclient.subscribe()
        else:
            sys.modules[__name__].wait_count += 1
            print(".")
            if sys.modules[__name__].wait_count >= 30:
                print("MQTT connection timed out")
                timer.deinit()
                def _restart(t):
                    sys.modules[__name__].mqtt_connect_timer = Timer(3)
                    sys.modules[__name__].mqtt_connect_timer.init(period=5000, mode=Timer.PERIODIC, callback=sys.modules[__name__]._check_wifi)
                sys.modules[__name__].retry_timer = Timer(4)
                sys.modules[__name__].retry_timer.init(period=5000, mode=Timer.ONE_SHOT, callback=sys.modules[__name__]._restart)
    sys.modules[__name__]._wait_mqtt = _wait_mqtt
    
    
    def _main_loop(timer):
        if sys.modules[__name__].mqttclient.client:
            sys.modules[__name__].mqttclient.heartbeat()
            sys.modules[__name__].mqttclient.check_msg()
        sys.modules[__name__].loop_play_sequence()
        
    sys.modules[__name__]._main_loop = _main_loop
    
    
    sys.modules[__name__].main_timer = Timer(5)
    sys.modules[__name__].main_timer.init(period=100, mode=Timer.PERIODIC, callback=sys.modules[__name__]._main_loop)

    sys.modules[__name__].wait_timer = Timer(6)

    sys.modules[__name__].mqtt_connect_timer = Timer(7)
    sys.modules[__name__].mqtt_connect_timer.init(period=1000, mode=Timer.PERIODIC, callback=sys.modules[__name__]._check_wifi)

    # Keep function alive; actual loop handled by timers

if __name__ == "__main__":
    run()

