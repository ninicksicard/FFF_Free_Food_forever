from umqtt.simple import MQTTClient
from CustomLock import CustomLock
import time
from machine import unique_id, Timer
import sys


led = sys.modules["System"].system.led


def append_to_queue(*args):
    led.value(not led.value())
    pass

class Topic:
    def __init__(self, topic):
        self._topic = topic

    @property
    def topic(self):
        """
        Property to get the topic as a regular string.
        """
        if isinstance(self._topic, bytes):
            return self._topic.decode()
        return self._topic

    @topic.setter
    def topic(self, value):
        """
        Property setter to set the topic, accepts both string and byte string.
        """
        self._topic = value

    def as_bytes(self):
        """
        Returns the topic as a byte string.
        """
        if isinstance(self._topic, str):
            return self._topic.encode()
        return self._topic

    def __str__(self):
        """
        When used in a string context, returns the topic as a regular string.
        """
        return self.topic

    def __bytes__(self):
        """
        When used in a bytes context, returns the topic as a byte string.
        """
        return self.as_bytes()

    def __hash__(self):
        return hash(self.as_str())

    def __eq__(self, other):
        if isinstance(other, Topic):
            return self.topic == other.topic
        return False

    def as_str(self):
        return self.topic


class myMQTT:
    """
    object to handle the connection to mqtt
    """

    def __init__(self):
        self.client = None
        self.MQTT_BROKER = '10.0.0.12'  # The IP address of your MQTT broker
        self.CLIENT_ID = self.generate_client_id()
        append_to_queue(f"self.CLIENT_ID = {self.CLIENT_ID}")
        self.USERNAME = 'homeassistant'  # MQTT Username
        self.PASSWORD = 'shiewiekoocaed5eepie3Qui6oe4uquipooGhaothoh2quechu9Ahjohsheicee3'  # MQTT Password
        self.lock_topics = CustomLock()
        self.topics = {}
        
        self.reconnect = True
        self.ON = [b"1", True, 1, "on", b"on", "ON", b"ON", "True", b"True", "true", b"true"]
        self.last_heartbeat = 0
        self.heartbeat_interval_sec = 5  # seconds
        self.heartbeat_interval_ms = self.heartbeat_interval_sec * 1000
        
        self.check_msg_interval_ms = 1000
        self.check_msg_interval_sec = self.check_msg_interval_ms/1000
        
        self.timer_heartbeat = Timer(7)
        self.timer_check_msg = Timer(8)
        
    def generate_client_id(self):
        _unique_id = unique_id()
        append_to_queue(f"generate_client_id")
        return "ESP32S3_" + ''.join('{:02x}'.format(b) for b in _unique_id)
    
    def connect_mqtt(self):
        """
        try to connect to the mqtt server
        """
        append_to_queue(f"connectMQTT")
        append_to_queue(f"starting connection loop thread from connect_mqtt")
        self.connect_client_loop()
    
    def connect_client_loop(self):
        append_to_queue(f"connect_client_loop")
        while True:
            time.sleep(0.1)
            if self.reconnect:
                try:
                    success = self.connect_client()
                    if success :
                        append_to_queue(f"connect_client_loop done")
                        break
                except Exception as e:
                    append_to_queue(f"connection failed : {e}")
            else:
                append_to_queue(f"connection failed : {e}")
                break
    
    def check_msg(self, *args):
        try:
            self.client.check_msg()
            append_to_queue(f"check_msg")
        except Exception as e:
            append_to_queue(f"error occurred in check_msg: {e}")
            if self.reconnect:
                self.connect_client()
                        
    def check_msg_thread(self):
        append_to_queue(f"check_msg_thread")
        while True:
            try:
                self.client.check_msg()
            except Exception as e:
                append_to_queue(f"error occured in check_msg_thread :  {e}")
                if self.reconnect:
                    self.connect_client()

        
    def connect_client(self):
        try:
            append_to_queue(f"connect_client")
            self.client = MQTTClient(self.CLIENT_ID, self.MQTT_BROKER, user=self.USERNAME,
                                     password=self.PASSWORD, keepalive=200)
            self.client.set_callback(self.mqtt_callback)
            self.client.connect()
            append_to_queue("connection successful")
            self.subscribe()
            return True
        except OSError as e:
            append_to_queue(f"Failed to connect to MQTT broker at {self.MQTT_BROKER}: {e}")
            if self.reconnect:
                self.connect_client()

    def subscribe(self):
        """
        Connect to the MQTT broker and subscribe to the topics, with error handling.
        """
        try:
            if not self.topics:
                append_to_queue(f"topic list is empty")
                return
            
            if not self.client:
                append_to_queue(f"No clients exist")
                return
            
            for topic in self.topics.keys():
                append_to_queue(f"topic :")
                append_to_queue(f"{topic.as_bytes()}")
                try:
                    self.client.subscribe(topic.as_bytes())  # Explicitly convert the topic to bytes
                    append_to_queue(f'Connected to {self.MQTT_BROKER} MQTT broker, subscribed to {topic} topic')
                except OSError as e:
                    append_to_queue(f"Failed to subscribe to topic {topic}: {e}")
        except Exception as e:
            append_to_queue(f"error occured in subscribe : {e}")


    def mqtt_callback(self, topic: str, msg):
        """
        Call the function corresponding to the correct topic and pass the message as its parameter.
        :param topic: The topic as a byte string or string.
        :param msg: The message associated with the topic.
        """
        try:
            topic_obj = Topic(topic)  # Convert the incoming topic to a Topic object
            if topic_obj in self.topics:
                function, arg = self.topics[topic_obj]
                append_to_queue(f"function : {function}, args : {arg}, msg : {msg}")
                function(arg, msg)
            else:
                append_to_queue("Topic not found:", topic_obj)
        except Exception as e:
            append_to_queue(f"error occured in mqtt_callback :  {e}")
            
    def mqtt_send(self, topic, msg):
        """
        :param topic:
        :param msg:
        """
        #append_to_queue(f"mqtt_send : {topic, str(msg).encode()}")
        self.client.publish(topic, str(msg).encode())
        
    def send_heartbeat(self, *args):
        self.mqtt_send(f"{self.CLIENT_ID}/heartbeat", "alive")
        
    def heartbeat(self, *args):
        current_time = time.time()
        if current_time - self.last_heartbeat >= self.heartbeat_interval_sec:
            send_heartbeat()
            current_time = time.time()
            
    def start_heartbeat(self, heartbeat_interval_ms=None, heartbeat_interval_sec=None):
        assert not heartbeat_interval_ms or not heartbeat_interval_sec , "only set ms or sec"
        self.heartbeat_interval_ms  = self.heartbeat_interval_sec*1000
        self.timer_heartbeat.init(mode=Timer.PERIODIC, period=self.heartbeat_interval_ms, callback=self.send_heartbeat)
        
    def start_check_msg(self, check_msg_interval_ms=None, check_msg_interval_sec=None):
        assert not check_msg_interval_ms or not check_msg_interval_sec , "only set ms or sec"
        if check_msg_interval_sec :
            self.check_msg_interval_sec  = check_msg_interval_sec
            
        elif check_msg_interval_ms:
            self.check_msg_interval_ms = check_msg_interval_ms
            self.check_msg_interval_sec = self.check_msg_interval_ms/1000
        self.check_msg_interval_ms = self.check_msg_interval_sec * 1000
        
        self.timer_check_msg.init(mode=Timer.PERIODIC, period=int(self.check_msg_interval_ms), callback=self.check_msg)
        
        
            

    def restart_and_reconnect(self):
        """
        restart the device after 10 seconds if connecting to mqtt broker had failed
        """
        machine.reset()



