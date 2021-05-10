//This code contain everything to control the whole system.

// import and setup ads1115 for more analog inputs.
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <sensorplot_webinterface.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_ADS1115 ads;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
ESP8266WebServer server(80);
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();


// adding reading delay
int preReadDelay = 10; //ms before reading analog input to ensure stabilised sensor voltage





int x = 14;






















/////////////////////// general purpos sensor read //////////////////////////////
float sensorRead(int sensor[]) {
  Serial.println("reading sensor");
  //sensor is a list of 2 ints, [power pin, read pin]
  //digitalWrite(sensor[0], HIGH);
  delay(preReadDelay);
  Serial.println(sensor[1]);
  int16_t sensor_value;
  Serial.println("sensor_value");
  sensor_value = ads.readADC_SingleEnded(sensor[1]);
  Serial.println(sensor_value);
  //digitalWrite(sensor[0], LOW);
  Serial.print("now return");
  return sensor_value;
}




////////////////////// general purpose relay switch control /////////////////////
void switchRelay(int relay, uint8_t state) {
  // code this according to the right relay board choosen(choose board once number of channel is known)
  if (state==HIGH){
    pwm.setPWM(relay, 4096, 0);  
  }else if (state==LOW){
  pwm.setPWM(relay, 0, 4096);
  }
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------ -

/*  Here are all the modules that sould be in this code
    Light_management
    Gas_and_pressure_management
    Aeration
    Fluids_management
    population_management
    Cleaning
    Extraction_management
    Nutrient_Management
    PH_Management
    Temperature_management
*/




// ------------------------------------------- Light management -------------------------------------------
/* Light level sensor
   Light temperature sensor
   Light Switch
   Light Logs?
*/



////////////////// light level sensor /////////////////
/*  photoresistor power on
    analog read of photoresistor
    photoresistor power off
    adjust value with calibration data
    log or save temperature value
*/
int LightCalFact = 1;
int LightCalOffset = 0;
int Light_Sensor[2] = {x, 0}; // x:photoresistor powerpin y:photoresistor analog input pin number(on ads1115)

float lightSens() {
  float light_level;
  light_level = sensorRead(Light_Sensor) * LightCalFact + LightCalOffset;
  return light_level;
}



/////////////////// Light Switch /////////////////
int Light_Switch = 0; //x:relay number or pin

void lightSwitch_on() {
  switchRelay(Light_Switch, HIGH);
}

void lightSwitch_off() {
  switchRelay(Light_Switch, LOW);
}



// ---------------------------------- Pressure Management ---------------------------------------
/*  system parameter (open/sealed system)
    air pressure sensor
    air compressor/fan
    pressure log and alarms (to notify for leaks)
*/


//------------------------------------- Cleaning ----------------------------------------------
/*  surface cleaning

*/




// -------------------------------------- Aeration ----------------------------------------------
/* control of the pump for bubbler stone, spray or other.
*/


/////////////////////////////////////
int Aeration_Pump = x; //x:relay number for aeration pump

void Aeration_on() {
  switchRelay(Aeration_Pump, HIGH);
}

void Aeration_off() {
  switchRelay(Aeration_Pump, LOW);
}




// ------------------------------------ Fluids management --------------------------------------
/*  Main tank water level sensor
    recycled tank water level sensor
    tap water input valve
    recycled water pump (from extraction process)
*/


/////////////Main tank water level sensor/////////////
int mainLevelCalFact = 1;
int mainLevelCalOffset = 0;
int main_Level_Sensor[2] = {x, 1}; // x: sensor power pin, y:sensor analog in pin

float mainTankLevel() {
  float Water_Level;
  Water_Level = sensorRead(main_Level_Sensor) * mainLevelCalFact + mainLevelCalOffset;
  return Water_Level;
}



/////////////secondary tank water level sensor/////////
int secondaryLevelCalFact = 1;
int secondaryLevelCalOffset = 0;
int secondary_Level_Sensor[2] = {x, 4}; // x: sensor power pin, y:sensor analog in pin

float secondaryTankLevel() {
  float Water_Level;
  Water_Level = sensorRead(secondary_Level_Sensor) * secondaryLevelCalFact + secondaryLevelCalOffset;
  return Water_Level;
}



// ---------------------------------------- population management ---------------------------------
/*  photodensity sensor
*/
////////////////////////////////////////////////////////
int densityPhotoresistor[2] = {x, 2}; // x:photoresistor power pin y:photoresistor analog input pin

float populationDensity() {
  float density;
  density = sensorRead(densityPhotoresistor);
  return density;
}


// --------------------------------------------- Extraction ----------------------------------------
/*  filter convayor motor control
    vacuum switch
    extraction pump/valve
*/

int extractionPump = x; //x:extraction pump relay switch
int vacuumPump = x; // x:vaccume pump relay number

void extraction_on() { // extraction time is in seconds
  switchRelay(extractionPump, HIGH);
}

void extraction_off() {
  switchRelay(extractionPump, LOW);
}

void vaccumePump_on() {
  switchRelay(vacuumPump, HIGH);
}

void vacuumPump_off() {
  switchRelay(vacuumPump, LOW);
}

// ------------------------------------------Nutrient management ------------------------------------
/*  solid fertiliser dosing actuator(s)(archimedes screw motor)
    water input pump/valve
    magnetic stiring motor
    nutrient output pump/valve
*/
//right now, considering a premixed fluid that is added as the algea is extracted

int peristalticPump = x; //x: peristaltic pump power pins
int stirerMotor = x; //x: stiring motor power pin relay

void nutrienPump_on() {
  switchRelay(peristalticPump, HIGH);
}

void nutrientPump_off() {
  switchRelay(peristalticPump, LOW);
}

// -------------------------------------------- Ph management --------------------------------------
/*  ph sensor
    ph dosing(screw motor)
*/

int phSensor[2] = {x, 3}; //x: main power relay pin or number y: ph analog input
int phCalFact = 32;
int phCalOffset1 = 0.82;
int phCalOffset2 = 7;

float ph_sens() {
  float ph;
  ph = (phCalOffset1 - sensorRead(phSensor)) * phCalFact + phCalOffset2;
  return ph;
}

// ----------------------------------------  Temperature management --------------------------------
/*  thermometers input
    thermal plate control
    fan control
    temperature logs/alert
*/
int numberOfDevices;
#define ONE_WIRE_BUS D5 //x: the pin for thermometer read
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

///////////////// thermometers input ////////////
int tempCalFact = 1;
int tempCalOffset = 0 ;
// int thermometer[2] = {x, y}; // x:thermometer power pin, y:thermometer analog input pin

float temperature_sens() {
  float temp;
  temp = sensors.getTempC(tempDeviceAddress) * tempCalFact + tempCalOffset;
  return temp;
}

//////////////// thermal plate control ////////////

//////////////// fan control /////////////////////
int fan_switch = x; //x: fan power pin

void fan_on() {
  switchRelay(fan_switch, HIGH);
}

void fan_off() {
  switchRelay(fan_switch, LOW);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------- webserver graphs --------------------------------------------------------

// should allow for wifi control


int interfaceCallback(String input) {
  // use the input for example as a password and trigger some action
  // or use it as a command and execute accordingly
  // when the input was valid you should return a 1
  // in case of a invalid input return a 0
  if (input == "lightSwitch_on") {
    lightSwitch_on();
    return 1;
  }
  if (input == "lightSwitch_off") {
    lightSwitch_off();
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
    for (int i = 0; i < (*measurementsCount - 1); i++) {
      measurements[i] = measurements[i + 1];
    }
    measurements[(*measurementsCount - 1)] = sensorInput;
  }
  *measurementsTimestamp = millis();
}


// You can input up to 32 Plots which will be displayed as graphs on the WebInterface

// Graph 1 temperature
int measurementsCount1 = 0;
float measurements1[128] = {};
int measurementsTimestamp1 = millis();
int cycleDuration1 = 20; // duration in seconds
void sensorReading1() {
  float sensorInput = temperature_sens();    // <- sensor reading for first input
  sensorReading(measurements1, &measurementsCount1, 128, &measurementsTimestamp1, sensorInput);
}

// Graph 2 Light Level
int measurementsCount2 = 0;
float measurements2[128] = {};
int measurementsTimestamp2 = millis();
int cycleDuration2 = 20; // duration in seconds
void sensorReading2() {
  float sensorInput = lightSens();    // <- sensor reading for second input
  sensorReading(measurements2, &measurementsCount2, 128, &measurementsTimestamp2, sensorInput);
}

// Graph 3 main tank water level
int measurementsCount3 = 0;
float measurements3[128] = {};
int measurementsTimestamp3 = millis();
int cycleDuration3 = 20; // duration in seconds
void sensorReading3() {
  float sensorInput = secondaryTankLevel();    // <- sensor reading for second input
  sensorReading(measurements3, &measurementsCount3, 128, &measurementsTimestamp3, sensorInput);
}

// Graph 4 populationDensity
int measurementsCount4 = 0;
float measurements4[128] = {};
int measurementsTimestamp4 = millis();
int cycleDuration4 = 20; // duration in seconds
void sensorReading4() {
  float sensorInput = populationDensity();    // <- sensor reading for second input
  sensorReading(measurements4, &measurementsCount4, 128, &measurementsTimestamp4, sensorInput);
}
//// Graph 5 Ph graph
//int measurementsCount5 = 0;
//float measurements5[128] = {};
//int measurementsTimestamp5 = millis();
//int cycleDuration5 = 20; // duration in seconds
//void sensorReading5() {
//  float sensorInput = ph_sens();    // <- sensor reading for second input
//  sensorReading(measurements5, &measurementsCount5, 128, &measurementsTimestamp5, sensorInput);
//}

void configWebInterface() {
  // Graph 1 temperature
  String name1 = "Temperature";
  String unit1 = "°C";
  int good1 = 31;
  int bad1 = 32;
  int min1 = 20;
  int max1 = 35;
  int stepsize1 = 200;
  int cycleStepsize1 = 600;
  webInterface.addPlot(name1, unit1, cycleDuration1, good1, bad1, min1, max1, stepsize1, cycleDuration1, cycleStepsize1, &measurementsCount1, measurements1, &measurementsTimestamp1);

  // Graph 2 Light Level
  String name2 = "Light Level";
  String unit2 = "units";
  int good2 = 24450;
  int bad2 = 24500;
  int min2 = 24450;
  int max2 = 24500;
  int stepsize2 = 200;
  int cycleStepsize2 = 600;
  webInterface.addPlot(name2, unit2, cycleDuration2, good2, bad2, min2, max2, stepsize2, cycleDuration2, cycleStepsize2, &measurementsCount2, measurements2, &measurementsTimestamp2);


  // Graph 3 main tank water level
  String name3 = "Main water level";
  String unit3 = "units";
  int good3 = 30;
  int bad3 = 50;
  int min3 = 0;
  int max3 = 10;
  int stepsize3 = 5;
  int cycleStepsize3 = 600;
  webInterface.addPlot(name3, unit3, cycleDuration3, good3, bad3, min3, max3, stepsize3, cycleDuration3, cycleStepsize3, &measurementsCount3, measurements3, &measurementsTimestamp3);

  // Graph 4 population density
  String name4 = "Population Density";
  String unit4 = "units";
  int good4 = 24450;
  int bad4 = 24500;
  int min4 = 24450;
  int max4 = 24500;
  int stepsize4 = 200;
  int cycleStepsize4 = 600;
  webInterface.addPlot(name4, unit4, cycleDuration4, good4, bad4, min4, max4, stepsize4, cycleDuration4, cycleStepsize4, &measurementsCount4, measurements4, &measurementsTimestamp4);

//  // Graph 5 ph graph
//  String name5 = "pH";
//  String unit5 = "pH";
//  int good5 = 8;
//  int bad5 = 9;
//  int min5 = 7;
//  int max5 = 12;
//  int stepsize5 = 1;
//  int cycleStepsize5 = 600;
//  webInterface.addPlot(name5, unit5, cycleDuration5, good5, bad5, min5, max5, stepsize5, cycleDuration5, cycleStepsize5, &measurementsCount5, measurements5, &measurementsTimestamp5);
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------------------












//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:


  Serial.begin(115200);
  Wire.begin(D1, D2);
  Serial.print(Wire.available());

  ads.begin();
  pwm.begin();
//  pwm.setPWMFreq(1600);
  
//  pinMode(x, OUTPUT);
//  pinMode(D0, INPUT);

  


  //-----------------------------------------thermometer setup ----------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  for (int i = 0; i < numberOfDevices; i++) {
    Serial.print("address : ");
    Serial.print(sensors.getAddress(tempDeviceAddress, i));

    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i)) {

      // Output the device ID
      Serial.print(sensors.getAddress(tempDeviceAddress, i));
      Serial.println(i, DEC);
      // Print the data

      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp C: ");
      Serial.print(tempC);

    }
  }



  //------------------------------------wifi graph setup ------------------------------------------------ -
  //////////////////////////////////////////webinterface
  configWebInterface();
  Serial.println("webinterfaceconfigured");
  webInterface.interfaceConfig("Sensor Measurements", "Inputfield", "Perform Action");
  webInterface.serverResponseSetup(&server, &interfaceCallback);
  server.begin();
  Serial.println("server began");

  Serial.println("reading sensor1");
  sensorReading1();
  Serial.println("reading sensor2");
  sensorReading2();
  Serial.println("reading sensor3");
  sensorReading3();
  Serial.println("reading sensor4");
  sensorReading4();
//  Serial.println("reading sensor5");
//  sensorReading5();

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

// ------------------------------------------------------------------------------------------------------------












void loop() {
  // put your main code here, to run repeatedly:
  // --------------------------------------------------------graph loop --------------------------------------------
  
  if ((millis() - measurementsTimestamp1) > (cycleDuration1 * 1000)) {
    Serial.println("reading sensor1");
    sensorReading1();
  }
  if ((millis() - measurementsTimestamp2) > (cycleDuration2 * 1000)) {
    Serial.println("reading sensor2");
    sensorReading2();
  }
  if ((millis() - measurementsTimestamp3) > (cycleDuration3 * 1000)) {
    Serial.println("reading sensor3");
    sensorReading3();
  }
  if ((millis() - measurementsTimestamp4) > (cycleDuration4 * 1000)) {
    Serial.println("reading sensor4");
    sensorReading4();
  }
//  if ((millis() - measurementsTimestamp5) > (cycleDuration5 * 1000)) {
//    Serial.println("reading sensor5");
//    sensorReading5();
//  }
  delay(400);
  Serial.println("handleclient");
  server.handleClient();
  delay(400);
}
// -------------------------------------------------------------------------------------------- -
