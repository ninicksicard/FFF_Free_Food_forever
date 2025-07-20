"""Minimal MQTT helper using ``umqtt.simple``."""

from umqtt.simple import MQTTClient
import wifi_manager
# Import print_control so the custom print replacement is active
try:
    from print_control  import print # noqa: F401
except Exception:
    pass

import time
import machine
from machine import Timer
import json
import os

try:
    getenv = os.getenv  # type: ignore[attr-defined]
except AttributeError:
    def getenv(key: str, default: str | None = None) -> str | None:
        return default

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
    """Handle the connection to an MQTT broker."""

    def __init__(self) -> None:
        """Initialise the MQTT client using configuration defaults."""
        self.client = None

        config = {}
        try:
            with open("mqtt_config.json", "r") as f:
                config = json.load(f)
        except Exception:
            pass

        self.MQTT_BROKER = config.get("MQTT_BROKER", getenv("MQTT_BROKER", "10.0.0.12"))
        env_client_id = config.get("MQTT_CLIENT_ID", getenv("MQTT_CLIENT_ID"))
        self.CLIENT_ID = env_client_id if env_client_id else self.generate_client_id()
        print(f"self.CLIENT_ID = {self.CLIENT_ID}")
        self.USERNAME = config.get("MQTT_USERNAME", getenv("MQTT_USERNAME", "homeassistant"))
        self.PASSWORD = config.get("MQTT_PASSWORD", getenv("MQTT_PASSWORD", ""))
        self.topics = {}
        
        self.reconnect = True
        self.ON = [b"1", True, 1, "on", b"on", "ON", b"ON", "True", b"True", "true", b"true"]
        self.last_heartbeat = 0
        self.heartbeat_interval = 5  # seconds

    def generate_client_id(self):
        """Return an identifier based on the device's unique ID."""
        uid = machine.unique_id()
        return "ESP32S3_" + ''.join('{:02x}'.format(b) for b in uid)

    def connect_mqtt(self):
        """Public entry point to connect to the MQTT broker."""
        print(f"connectMQTT")
        if not wifi_manager.WifiManager.wlan_status():
            print("WiFi not connected, delaying MQTT connection")
            self._wifi_timer = Timer(8)
            self._wifi_timer.init(period=5000, mode=Timer.ONE_SHOT,
                                  callback=lambda t: self.connect_mqtt())
            return

        print(f"starting connection loop timer from connect_mqtt")
        self._conn_timer = Timer(9)
        self._conn_timer.init(period=100, mode=Timer.PERIODIC,
                              callback=self._connect_client_step)

    def _connect_client_step(self, timer):
        """Timer callback to attempt MQTT connection."""
        if self.reconnect:
            try:
                success = self.connect_client()
                if success:
                    print("connect_client_loop done")
                    timer.deinit()
            except Exception as e:
                print(f"connection failed : {e}")
        else:
            timer.deinit()
    
    def check_msg(self):
        """Check for incoming MQTT messages."""
        try:
            self.client.check_msg()
        except Exception as e:
            print(f"error occurred in check_msg: {e}")
            if self.reconnect:
                self.connect_client()
                        
        
    def connect_client(self):
        """Create and connect the underlying ``MQTTClient``."""
        try:
            print(f"connect_client")
            self.client = MQTTClient(self.CLIENT_ID, self.MQTT_BROKER, user=self.USERNAME,
                                     password=self.PASSWORD, keepalive=200)
            self.client.set_callback(self.mqtt_callback)
            self.client.connect()
            print("connection successful")
            self.subscribe()
            return True
        except OSError as e:
            print(f"Failed to connect to MQTT broker at {self.MQTT_BROKER}: {e}")
            if self.reconnect:
                self.connect_client()

    def subscribe(self):
        """Subscribe to all topics listed in ``self.topics``."""
        try:
            if not self.topics:
                print(f"topic list is empty")
                return
            
            if not self.client:
                print(f"No clients exist")
                return
            
            for topic in self.topics.keys():
                print(f"topic :")
                print(f"{topic.as_bytes()}")
                try:
                    self.client.subscribe(topic.as_bytes())  # Explicitly convert the topic to bytes
                    print(f'Connected to {self.MQTT_BROKER} MQTT broker, subscribed to {topic} topic')
                except OSError as e:
                    print(f"Failed to subscribe to topic {topic}: {e}")
        except Exception as e:
            print(f"error occured in subscribe : {e}")


    def mqtt_callback(self, topic: str, msg):
        """Dispatch incoming messages to the registered callbacks."""
        try:
            topic_obj = Topic(topic)  # Convert the incoming topic to a Topic object
            if topic_obj in self.topics:
                function, arg = self.topics[topic_obj]
                print(f"function : {function}, args : {arg}, msg : {msg}")
                function(arg, msg)
            else:
                print("Topic not found:", topic_obj)
        except Exception as e:
            print(f"error occured in mqtt_callback :  {e}")
            
    def mqtt_send(self, topic, msg):
        """Publish ``msg`` on ``topic``."""
        print(f"mqtt_send")
        self.client.publish(topic, str(msg).encode())
    
    def heartbeat(self):
        """Send a periodic heartbeat message."""
        current_time = time.time()
        if current_time - self.last_heartbeat >= self.heartbeat_interval:
            self.mqtt_send("heartbeat", "alive")
            self.last_heartbeat = current_time

    def restart_and_reconnect(self):
        """Restart the device when connection attempts repeatedly fail."""
        machine.reset()



