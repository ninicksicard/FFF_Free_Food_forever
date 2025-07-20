# import schedule
# import schedule_thread
# import CyanOSensors
# from CyanOSensors import *
# import CyanOModes
# from CyanOModes import *
# import ServoController
import utime as time
import machine
import time
from umqtt.simple import MQTTClient

time.sleep(1)  # Pause 10 seconds
from wifi_manager import WifiManager
time.sleep(1)  # Pause 10 seconds
WifiManager.setup_network()

def print_queue():
    while len(WifiManager.print_queue)>0:
        print(WifiManager.print_queue.pop(0))
        
        
print_queue()

if WifiManager.is_connected():
    
    # Your MQTT details
    MQTT_BROKER = '10.0.0.12'  # The IP address of your MQTT broker
    CLIENT_ID = 'ESP32S3_Lights'
    USERNAME = 'homeassistant'  # MQTT Username
    PASSWORD = 'shiewiekoocaed5eepie3Qui6oe4uquipooGhaothoh2quechu9Ahjohsheicee3'  # MQTT Password

    # Example GPIO pins for the lights
    LIGHT_PIN_1 = 1
    LIGHT_PIN_2 = 2

    # Setup GPIO
    light_1 = machine.Pin(LIGHT_PIN_1, machine.Pin.OUT)
    light_2 = machine.Pin(LIGHT_PIN_2, machine.Pin.OUT)

    def mqtt_callback(topic, msg):
        print((topic, msg))
        if topic == b'home/esp32s3/lights':
            command = msg.decode()
            print(command)
            if command == 'light1_on':
                light_1.on()
            elif command == 'light1_off':
                light_1.off()
            elif command == 'light2_on':
                light_2.on()
            elif command == 'light2_off':
                light_2.off()

    def connect_and_subscribe():
        client = MQTTClient(CLIENT_ID, MQTT_BROKER, user=USERNAME, password=PASSWORD)
        client.set_callback(mqtt_callback)
        client.connect()
        client.subscribe(b'home/esp32s3/lights')
        print('Connected to %s MQTT broker, subscribed to %s topic' % (MQTT_BROKER, 'home/esp32s3/lights'))
        return client

    def restart_and_reconnect():
        print('Failed to connect to MQTT broker. Reconnecting...')
        time.sleep(10)
        machine.reset()

    try:
        client = connect_and_subscribe()
    except OSError as e:
        restart_and_reconnect()

    while True:
        try:
            client.wait_msg()
        except OSError as e:
            restart_and_reconnect()






#import serviceresponderexemple
# 
# #sensors
# # require pins 
# culture_bottom_level = Sensor(pin=2, condition_func=lt, threshold_value=1, interval_seconds=5)
# culture_top_level = Sensor(pin=3, condition_func=gt, threshold_value=1, interval_seconds=5)
# nutrient_bottom_level = Sensor(pin=4, condition_func=lt, threshold_value=1, interval_seconds=5)
# trash_bottom_level = Sensor(pin=5, condition_func=lt, threshold_value=1, interval_seconds=5)
# extract_top_level = Sensor(pin=6, condition_func=lt, threshold_value=1, interval_seconds=5)
# 
# 
# # modes
# aeration = Mode(name='aeration', servo_position=20, pump_speed=540, duration=5, delay=5)
# idle = Mode(name='idle', servo_position=20, pump_speed=0, duration=30, delay=0)
# idle.follow_up_mode = idle
# extraction = Mode(name='extraction', servo_position=20, pump_speed=540, duration=60*20, delay=0, follow_up_mode=aeration)
# aeration.fallback_mode = idle
# # schedules
# 
# 
# schedule.every().day.at("6:30").do(Mode.start, extraction)
# 
# schedules = {
#     "aeration" : [schedule.every().day.at("6:30").do(Mode.start, aeration),
#                 schedule.every().day.at("19:30").do(Mode.start, idle)]}
# 
# # set to expected state at boot
# for category, sch_list in schedules.items():
#     max(sch_list, key=lambda sch: sch.next_run).run()
# 
# 
# 
# 
# 




