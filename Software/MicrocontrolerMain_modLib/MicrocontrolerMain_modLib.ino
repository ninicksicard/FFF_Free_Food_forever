#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_PWMServoDriver.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DS3231.h>
#include <EEPROM.h>

// data transport protocol for wifi
WiFiUDP ntpUDP;

// ntp server object(online time)
const long utcOffsetInSeconds = -4 * 3600;

NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// clock module object (battery powered clock module)
DS3231 t;

// pwm module pca9685
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// analog read module
Adafruit_ADS1115 ads;

// wifi server
WiFiServer server(80);


void setup() {
  // for serial communication with a pc
  Serial.begin(115200);

  // internal memory for calibration values
  EEPROM.begin(512);

  // for I2C communication between modules
  Wire.begin(D1, D2);     // Wire.begin(sda,scl);

  // initialize analog read module
  ads.begin();

  // initialize pwm module
  pwm.begin();
  pwm.setPWMFreq(1600);

  // initialize time 
  timeClient.begin();

  // todo : verify what could be done without pwm


  // initialize output pin for water level and population density sensor
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);

  // initialize everything 
  setup_routine();

  // connect to wifi and do first tasks
  setup_wifi();
  
  // check for time
  GetHour();
  unsigned long epochTime = timeClient.getEpochTime();
}

void loop() {
  /*  trigger the routine function. 
   *  it is a function located at the end of the code to make sure 
   *  all the tasks are designated before it
   */
  routine();
}
