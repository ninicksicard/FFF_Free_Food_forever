#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "src/sensorplot_webinterface/sensorplot_webinterface.h"
#include <Adafruit_ADS1X15.h>
#include <Adafruit_PWMServoDriver.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <DS3231.h>

// -------- time ------
const long utcOffsetInSeconds = -4 * 3600;
int morningtime = 6;
int noontime = 20;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
DS3231 t;
bool h12Flag;
bool pmFlag;


// -------PWM -----------
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
int preReadDelay = 400; //ms before reading analog input to ensure stabilised sensor voltage


//--------sensors---------
Adafruit_ADS1115 ads;


//-------- wifi -----------
ESP8266WebServer server(80);
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();


////////////////////////////

void setup() {
  Serial.begin(115200);
  Wire.begin(D1, D2);     //sda,scl
  Serial.print(Wire.available());
  ads.begin();
  pwm.begin();
  pwm.setPWMFreq(1600);
  timeClient.begin();
  pinMode(D7, OUTPUT);
  digitalWrite(D7, HIGH);

  setup_relays();
  setup_thermometer();
  setup_routine();
  Serial.println("connecting to wifi");
  setup_wifi();

  GetHour();

  unsigned long epochTime = timeClient.getEpochTime();
 
}


void loop() {
  routine();
  delay(50);
  server.handleClient();
}
