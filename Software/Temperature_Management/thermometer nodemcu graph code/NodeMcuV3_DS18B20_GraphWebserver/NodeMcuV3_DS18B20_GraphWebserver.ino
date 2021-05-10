#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <sensorplot_webinterface.h>

int numberOfDevices;
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress; 

ESP8266WebServer server(80);
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();

int interfaceCallback(String input) {
  // use the input for example as a password and trigger some action
  // or use it as a command and execute accordingly
  // when the input was valid you should return a 1
  // in case of a invalid input return a 0
  if (input == "password") {
    // do something
    return 1;
  }
  return 0;
}


void sensorReading(float *measurements, int *measurementsCount, int maxMeasurements, int *measurementsTimestamp, float sensorInput) {
  
  if (*measurementsCount < maxMeasurements) {
    *measurementsCount += 1;
    measurements[(*measurementsCount - 1)] = sensorInput;
    } else {
      *measurementsCount = maxMeasurements;
      for(int i = 0; i < (*measurementsCount - 1); i++) {
        measurements[i] = measurements[i + 1];
      }
      measurements[(*measurementsCount - 1)] = sensorInput;
    }
    *measurementsTimestamp = millis();
  }






// Graph 2
int measurementsCount2 = 0;
float measurements2[800] = {};
int measurementsTimestamp2 = millis();
int cycleDuration2 = 60; // duration in seconds

void sensorReading2() {
  sensors.requestTemperatures();
  float sensorInput = sensors.getTempC(tempDeviceAddress);   // <- sensor reading for second input
  sensorReading(measurements2, &measurementsCount2, 1000, &measurementsTimestamp2, sensorInput);
  }

void configWebInterface() {
  String name2 = "Temperature";
  String unit2 = "°C";
  int good2 = 30;
  int bad2 = 50;
  int min2 = 20;
  int max2 = 35;
  int stepsize2 = 5;
  int cycleStepsize2 = 600;
  webInterface.addPlot(name2, unit2, cycleDuration2, good2, bad2, min2, max2, stepsize2, cycleDuration2, cycleStepsize2, &measurementsCount2, measurements2, &measurementsTimestamp2);
}








void setup() {
  Serial.begin(115200);
  sensors.begin(); 

  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");
  for(int i=0;i<numberOfDevices; i++){
    Serial.print("address : ");
    Serial.print(sensors.getAddress(tempDeviceAddress, i));
            // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      // Output the device ID
      Serial.print(sensors.getAddress(tempDeviceAddress, i));
      Serial.println(i,DEC);
      // Print the data
      
      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp C: ");
      Serial.print(tempC);

        }
      }


  configWebInterface();
  webInterface.interfaceConfig("Sensor Measurements", "Inputfield", "Perform Action");
  webInterface.serverResponseSetup(&server, &interfaceCallback);
  server.begin();

  sensorReading2();

  WiFi.begin("Sylvie", "Bertrande");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("Use this URL: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}


void loop() {

  if ((millis() - measurementsTimestamp2) > (cycleDuration2 * 1000)) {
    sensorReading2();
  }
  server.handleClient();

  delay(100);
}
