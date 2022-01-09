#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
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


// wifi server
WiFiServer server(80);


void setup() {
  // for serial communication with a pc
  Serial.begin(115200);

  // internal memory for calibration values
  EEPROM.begin(512);

  // initialize time 
  timeClient.begin();

  // initialize output pin for water level and population density sensor
  
  
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
//  digitalWrite(D3, HIGH); 
//  digitalWrite(D5, HIGH); 
//  digitalWrite(D6, HIGH); 
//  digitalWrite(D7, HIGH); 
  // initialize everything 
  setup_routine();

  // connect to wifi and do first tasks
  setup_wifi();
  
  // check for time
  GetHour();
}

void loop() {
  /*  trigger the routine function. 
   *  it is a function located at the end of the code to make sure 
   *  all the tasks are designated before it
   */
  routine();
}
