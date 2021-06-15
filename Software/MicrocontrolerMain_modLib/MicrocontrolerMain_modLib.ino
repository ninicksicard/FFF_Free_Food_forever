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
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>



const long utcOffsetInSeconds = -4*3600;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


Adafruit_ADS1115 ads;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
ESP8266WebServer server(80);
SensorPlot_WebInterface webInterface = SensorPlot_WebInterface();


// adding reading delay
int preReadDelay = 200; //ms before reading analog input to ensure stabilised sensor voltage

///////////////////////////////////////////////////////All variables////////////////////////////////////

typedef struct {
  int TempRead;
  int LightRead;
  int WaterLevel;
  int PopulationRead;
  int PhRead;
  int TimeCheck;
  int Global;
}CycleDurations;

CycleDurations GraphUpdates = {
  400,    //   GraphUpdates.TempRead
  200,    //   GraphUpdates.LightRead
  20,    //   GraphUpdates.WaterLevel
  200 ,   //   GraphUpdates.PopulationRead
  400,    //   GraphUpdates.PhRead
  3600,   //   GraphUpdates.TimeCheck
  10      //   
};

CycleDurations ControlCycles = {
  120,    //   ControlCycles.TempRead;
  200,    //   ControlCycles.LightRead;
  60,    //   ControlCycles.WaterLevel;
  200,   //   ControlCycles.PopulationRead;
  400,    //   ControlCycles.PhRead;
  3600,   //   ControlCycles.TimeCheck;
  10      //   ControlCycles.Global
};

CycleDurations Timestamps = {
  millis(),   //Timestamps.TempRead
  millis(),   //Timestamps.LightRead
  millis(),   //Timestamps.WaterLevel
  millis(),   //Timestamps.PopulationRead
  millis(),   //Timestamps.PhRead
  millis(),   //Timestamps.TimeCheck
  millis()    //Timestamps.Global
};

typedef struct{
  float DayMax;
  float DayMin;
  float NightMax;
  float NightMin;
  float MinTreshold;
  float MaxTreshold;
} Temperatures;

Temperatures Temps = {
  31, //  Temps.DayMax
  30, //  Temps.DayMin
  27, //  Temps.NightMax
  26, //  Temps.NightMin
  30, //  Temps.MinTreshold
  31  //  Temps.MaxTreshold
};

typedef struct{
  bool Lights;
  bool Heat; // true = daytimeheat , false = nightime heat
  bool WaterPump;
  bool WaterWheel;
  bool AirPump;
} states;

states SystemStates = {
  true, //  bool Lights;
  true, //  bool Heat; // true = daytimeheat , false = nightime heat
  true, //  bool WaterPump;
  true, //  bool WaterWheel;
  true  //  bool AirPump;
};

typedef struct{
  int Lights;
  int Heat;
  int WaterPump;
  int WaterWheel;
  int AirPump;
  int ExtractionPump;
  int VacuumPump;
  int Fan;
  int DensitySensor;
}PinOut;

PinOut Relays = {
  0,  // Lights;        120v  AC
  2,  // Heat;          120v  AC
  8, // WaterPump;     5v    DC
  14, // WaterWheel;    5v    DC
  13,  // AirPump;       12v   DC
  12, // ExtractionPump 5v    DC
  1,  // VacuumPump     ??
  5,  // Fan            12v   DC
  10  // density sensor 5v    DC
};


typedef struct{
//  int Thermometer;
  int PhSensor;
  int WaterLevelSensor;
  int DensitySensor;
  int LightSensor;
}PinIn;

PinIn Sensors = {
      // using a nodemcu pin Thermometer;
   3,   // 3 = A3   PhSensor;
   0,   // 0 = A0   WaterLevelSensor;
   1,   // 1 = A1   DensitySensor;
   2    // 2 = A2   LightSensor;
};


///////////////////////////////////////////////////////TimeManagement///////////////////////////////////



int morningtime = 6;
int noontime = 20;



void printTime(){
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
}

void SyncOnlineTime(){
  timeClient.update();
  printTime();
}

void TimeProfileManagement(){
  SyncOnlineTime();
  //Setup Daytime Profile
  int nowhour = timeClient.getHours();
  if (morningtime < nowhour&&nowhour<noontime){
    Serial.println("day time");
    GraphUpdates.PhRead = 50; // duration in seconds
    Aeration_on();
    //Lights on
    if (!SystemStates.Lights){
      lightSwitch_on();
    }
    
    //Daytime temperature
    if (!SystemStates.Heat){
      Temps.MinTreshold = Temps.DayMin;
      Temps.MaxTreshold = Temps.DayMax;
      Serial.println("DayTime temperatures");
      
    }
  }
  //SetupNight time profile
  
  else{
    Serial.println("night time");
    GraphUpdates.PhRead = 3600; // duration in seconds
    //Aeration_off();
    //Lights off
    if (SystemStates.Lights){
      lightSwitch_off();
    }
    //NightTime temperatures
    if (SystemStates.Heat){
      Temps.MinTreshold = Temps.NightMin;
      Temps.MaxTreshold = Temps.NightMax;
      Serial.println("NightTime temperatures");
      
    }
  }
}



////////////////////// general purpose relay switch control /////////////////////
void switchRelay(int relay, uint8_t state) {
  // code this according to the right relay board choosen(choose board once number of channel is known)
  if (state==HIGH){
    pwm.setPWM(relay, 0, 4096);  
  }else if (state==LOW){
    pwm.setPWM(relay, 4096, 0);
  }
}



/////////////////////// general purpos sensor read //////////////////////////////
float sensorRead(int sensor[]) {
//  Serial.println("reading sensor");
  //sensor is a list of 2 ints, [power pin, read pin]
  switchRelay(sensor[0], HIGH);
  delay(preReadDelay);
  
//  Serial.print("ads1115 port is : ");
//  Serial.println(sensor[1]);
  float sensor_value;
//  Serial.println("trying to read the ads1115");
  sensor_value = ads.readADC_SingleEnded(sensor[1]);
  delay(preReadDelay*10);
//  Serial.print("sensor_value is : ");
//  Serial.println(sensor_value);
  
  switchRelay(sensor[0], LOW);
//  Serial.println("now return");
  return sensor_value;
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


float LightCalFact = 1;
float LightCalOffset = 0;
int Light_Sensor[2] = {4, Sensors.LightSensor}; // x:photoresistor powerpin y:photoresistor analog input pin number(on ads1115)

float lightSens() {
  float light_level;
  light_level = sensorRead(Light_Sensor) * LightCalFact + LightCalOffset;
  return light_level;
}


/////////////////// Light Switch /////////////////
int Light_Switch = 0; //x:relay number or pin

void lightSwitch_on() {
  switchRelay(Light_Switch, HIGH);
  Serial.println("light switched on");
  SystemStates.Lights = true;
}

void lightSwitch_off() {
  switchRelay(Light_Switch, LOW);
  Serial.println("light switched off");
  SystemStates.Lights = false;    
}

                          void LightCycleManager(){ //todo : move to time cycle management 
                            int nowhour = timeClient.getHours();
                            if (morningtime < nowhour&&nowhour<noontime && !SystemStates.Lights){
                              lightSwitch_on();
                              
                              }
                            else if(!(morningtime < nowhour&&nowhour<noontime) && SystemStates.Lights){
                              lightSwitch_off();
                              
                            }
                          }

// ---------------------------------- Pressure Management ---------------------------------------
/*   system parameter (open/sealed system)
 *   air pressure sensor
 *   air compressor/fan
 *   pressure log and alarms (to notify for leaks)
 */


//------------------------------------- Cleaning ----------------------------------------------
/*  surface cleaning
 * 
 */




// -------------------------------------- Aeration ----------------------------------------------
/* control of the pump for bubbler stone, spray or other.
*/
void Aeration_on()  {
  
  switchRelay(Relays.AirPump, HIGH);
  Serial.println("AirPump On");
  SystemStates.AirPump = true;
}
  
void Aeration_off() {
  switchRelay(Relays.AirPump, LOW);
  Serial.println("AirPump off");
  SystemStates.AirPump = false;
}




// ------------------------------------ Fluids management --------------------------------------
/*  Main tank water level sensor
    recycled tank water level sensor
    tap water input valve
    recycled water pump (from extraction process)
*/


/////////////Main tank water level sensor/////////////
float mainLevelCalFact = 0.00003051757;
float mainLevelCalOffset = 0;
int main_Level_Sensor[2] = {9, Sensors.WaterLevelSensor}; // x: sensor power pin, y:sensor analog in pin

float mainTankLevel() {
  float Water_Level;
  Water_Level = sensorRead(main_Level_Sensor) * mainLevelCalFact + mainLevelCalOffset;
  Serial.print("WaterLevel : ");
  Serial.println(Water_Level);
  return Water_Level;
}



/////////////secondary tank water level sensor/////////
//float secondaryLevelCalFact = 0.00003051757;
//float secondaryLevelCalOffset = 0;
//int secondary_Level_Sensor[2] = {9, 4}; // x: sensor power pin, y:sensor analog in pin
//
//float secondaryTankLevel() {
//  float Water_Level;
//  Water_Level = sensorRead(secondary_Level_Sensor) * secondaryLevelCalFact + secondaryLevelCalOffset;
//  Serial.print("water level sensor value : ");
//  Serial.println(Water_Level);
//  return Water_Level;
//}



/////////////water level control////////

// relay number for the water pump





// Direct water pump actions
void waterPump_on(){
  switchRelay(Relays.WaterPump,HIGH);
  Serial.println("water Pump turned on");
  SystemStates.WaterPump=true;
}

void waterPump_off(){
  switchRelay(Relays.WaterPump,LOW);
  Serial.println("water Pump turned off");
  SystemStates.WaterPump=false;
}



// water pump control cycle 
// todo : duplicate this code for the sensor and pmp of the second water tank

float waterLevelTreshold = 0.05;

int waterLevelControl(){

  if (mainTankLevel()<waterLevelTreshold && !SystemStates.WaterPump){
    waterPump_on();
    return 1;
  }else if (mainTankLevel()>= waterLevelTreshold && SystemStates.WaterPump){
    waterPump_off(); 
    return 0;
  }  
}
  
  


// --------------------------------------------- Extraction ----------------------------------------
/*  filter convayor motor control
    vacuum switch
    extraction pump/valve
*/


int vacuumPump = 1; // x:vaccume pump relay number

void extraction_on() { // extraction time is in seconds
  switchRelay(Relays.ExtractionPump, HIGH);
}

void extraction_off() {
  switchRelay(Relays.ExtractionPump, LOW);
}

void vaccumePump_on() {
  switchRelay(vacuumPump, HIGH);
}

void vacuumPump_off() {
  switchRelay(vacuumPump, LOW);
}


// ---------------------------------------- population management ---------------------------------
/*  photodensity sensor
*/
////////////////////////////////////////////////////////
int densityPhotoresistor[2] = {Relays.DensitySensor, Sensors.DensitySensor}; // x:photoresistor power pin y:photoresistor analog input pin

float PopulationManagement() {
  float density;
  density = sensorRead(densityPhotoresistor);
  if (density*0.0012872774 > 2){
     extraction_on();
  }else{
    extraction_off();
  }
  return density;
}



// ------------------------------------------Nutrient management ------------------------------------
/*  solid fertiliser dosing actuator(s)(archimedes screw motor)
    water input pump/valve
    magnetic stiring motor
    nutrient output pump/valve
*/
//right now, considering a premixed fluid that is added as the algea is extracted

int peristalticPump = 12; //x: peristaltic pump power pins
int stirerMotor = 14; //x: stiring motor power pin relay

void nutrienPump_on() {
  switchRelay(peristalticPump, HIGH);
}

void nutrientPump_off() {
  switchRelay(peristalticPump, LOW);
}

// ---------------------------------------waterlevel --------------------------------






// -------------------------------------------- Ph management --------------------------------------
/*  ph sensor
    ph dosing(screw motor)
*/

int phSensor[2] = {3, Sensors.PhSensor}; //x: main power relay pin or number y: ph analog input
//int phCalFact = 34;
//float phCalOffset1 = 2.55/5-0.1;
//int phCalOffset2 = 7;




float ph_sens() {
  float ph;
  float phCalFact = -0.0012872774;        //-776.833333333
  float phCalOffset = 24.6461703497;                   //-4.84617034971
  float phread = sensorRead(phSensor);
  
  ph = phCalFact*phread + phCalOffset;
//  ph = (phCalOffset1*phCalFact - phread*phCalFact/32768) + phCalOffset2;
  Serial.println();
  Serial.print("phread : ");
  Serial.println(phread);
  Serial.print("phCalFact*phread : ");
  Serial.println(phCalFact*phread);
  Serial.print("phCalOffset : ");
  Serial.println(phCalOffset);
  Serial.print("phCalFact : ");
  Serial.println(phCalFact);
  Serial.print("ph : ");
  Serial.println(ph);
  return ph;
}
// calibrating ph sensor with a linear fonctions
//int calPh4;
//int calPh13;
//void calPh4_read(){
//  sensorRead(phSensor);  
//  r
//
//}
//
//void 
//
//ph=a*v+b
//
//int a = (ph2-ph1)/(v2-v1);


// ----------------------------------------  Temperature management --------------------------------
/*  thermometers input
    thermal plate control
    fan control
    temperature logs/alert
*/
int numberOfDevices;
#define ONE_WIRE_BUS 14 //x: the pin for thermometer read
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;



///////////////// thermometers input ////////////
int tempCalFact = 1;
int tempCalOffset = 0 ;


float temperature_sens() {  // todo : add a power up and reset of ds18b20
  float temp;
  sensors.requestTemperatures();
 // oneWire.reset();
  temp = sensors.getTempC(tempDeviceAddress) * tempCalFact + tempCalOffset;
  return temp;
}

//////////////// thermal plate control ////////////

int heatingRelay = 2; // x: relay number for heating plate

void heating_on(){
  switchRelay(Relays.Heat, HIGH);
}

void heating_off(){
  switchRelay(Relays.Heat, LOW);
}


//////////////// fan control /////////////////////


void fan_on() {
  switchRelay(Relays.Fan, HIGH);
}

void fan_off() {
  switchRelay(Relays.Fan, LOW);
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
  
  if (input == "heating_on") {
    Temps.MinTreshold = Temps.DayMin;
    Temps.MaxTreshold = Temps.DayMax;
    heating_on();
    return 1;
  }
  if (input == "heating_off") {
    Temps.MinTreshold = 0;
    Temps.MaxTreshold = 1;
    heating_off();
    return 1;
  }
  
  if (input == "Aeration_on") {
    Aeration_on();
    return 1;
  }
  if (input == "Aeration_off") {
    Aeration_off();
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


// *******************************************************************************


// You can input up to 32 Plots which will be displayed as graphs on the WebInterface

// Graph 1 temperature
int measurementsCount1 = 0;
float measurements1[63] = {};

void TemperatureCycleManagement(){
  timeClient.update(); //todo: put this in system time managment function

  int nowhour = timeClient.getHours();
  if (morningtime < nowhour && nowhour < noontime) {
    Temps.MinTreshold = Temps.DayMin;
    Temps.MaxTreshold = Temps.DayMax;
  } else {
    Temps.MinTreshold = Temps.NightMin;
    Temps.MaxTreshold = Temps.NightMax;
  }
  float sensorInput = temperature_sens();    // <- sensor reading for first input
  if (sensorInput> Temps.MaxTreshold){
    heating_off();
  }else if (sensorInput<Temps.MinTreshold){
    if (sensorInput == -127){
      Serial.println("thermometer disconnected");
    }else{
      heating_on();
    }
   
  }
}

//Graph1 thermometer

void sensorReading1() {
  float sensorInput = temperature_sens();    // <- sensor reading for first input  
  sensorReading(measurements1, &measurementsCount1, 128, &Timestamps.TempRead, sensorInput);
}

// Graph 2 Light Level
int measurementsCount2 = 0;
float measurements2[63] = {};

void sensorReading2() {
  float sensorInput = lightSens()*0.00003051757;    // <- sensor reading for second input
  sensorReading(measurements2, &measurementsCount2, 128, &Timestamps.LightRead, sensorInput);
}

// Graph 3 main tank water level
int measurementsCount3 = 0;
float measurements3[63] = {};

void sensorReading3() {
  float sensorInput = mainTankLevel();    // <- sensor reading for second input
  sensorReading(measurements3, &measurementsCount3, 128, &Timestamps.WaterLevel, sensorInput);
}

// Graph 4 populationDensity
int measurementsCount4 = 0;
float measurements4[63] = {};

void sensorReading4() {
  float sensorInput = PopulationManagement()*0.0012872774;    // <- sensor reading for second input
  sensorReading(measurements4, &measurementsCount4, 128, &Timestamps.PopulationRead, sensorInput);
}
// Graph 5 Ph graph
int measurementsCount5 = 0;
float measurements5[63] = {};

void sensorReading5() {
  Aeration_off();
  delay(10000);
  float sensorInput = ph_sens();    // <- sensor reading for second input 
  sensorReading(measurements5, &measurementsCount5, 128, &Timestamps.PhRead, sensorInput);
  delay(500);
  Aeration_on();
}



//**********************************************************************



void configWebInterface() {
  // Graph 1 temperature
  String name1 = "Temperature";
  String unit1 = "°C";
  int min1 = 30;
  int max1 = 35;
  int stepsize1 = 1;
  int cycleStepsize1 = 1200;
  webInterface.addPlot(name1, unit1, GraphUpdates.TempRead, Temps.MinTreshold-0.5, Temps.MaxTreshold+0.5, min1, max1, stepsize1, GraphUpdates.TempRead, cycleStepsize1, &measurementsCount1, measurements1, &Timestamps.TempRead);

  // Graph 2 Light Level
  String name2 = "Light Level";
  String unit2 = "units";
  int lower2 = 0;
  int upper2 = 3;
  int min2 = 2;
  int max2 = 4;
  int stepsize2 = 20000;
  int cycleStepsize2 = 600;
  webInterface.addPlot(name2, unit2, GraphUpdates.LightRead, lower2, upper2, min2, max2, stepsize2, GraphUpdates.LightRead, cycleStepsize2, &measurementsCount2, measurements2, &Timestamps.LightRead);


  // Graph 3 main tank water level
  String name3 = "Main water level";
  String unit3 = "units";
  int lower3 = 0;
  int upper3 = 0.9;
  int min3 = 0;
  int max3 = 1;
  int stepsize3 = 0.5;
  int cycleStepsize3 = 600;
  webInterface.addPlot(name3, unit3, GraphUpdates.WaterLevel, lower3, upper3, min3, max3, stepsize3, GraphUpdates.WaterLevel, cycleStepsize3, &measurementsCount3, measurements3, &Timestamps.WaterLevel);

  // Graph 4 population density
  String name4 = "Population Density";
  String unit4 = "units";
  int lower4 = 0;
  int upper4 = 4;
  int min4 = 2;
  int max4 = 4;
  int stepsize4 = 20000;
  int cycleStepsize4 = 600;
  webInterface.addPlot(name4, unit4, GraphUpdates.PopulationRead, lower4, upper4, min4, max4, stepsize4, GraphUpdates.PopulationRead, cycleStepsize4, &measurementsCount4, measurements4, &Timestamps.PopulationRead);

  // Graph 5 ph graph
  String name5 = "pH";
  String unit5 = "pH";
  int lower5 = 8.5;
  int upper5 = 11.5;
  int min5 = 7;
  int max5 = 12;
  int stepsize5 = 1;
  int cycleStepsize5 = 600;
  webInterface.addPlot(name5, unit5, GraphUpdates.PhRead, lower5, upper5, min5, max5, stepsize5, GraphUpdates.PhRead, cycleStepsize5, &measurementsCount5, measurements5, &Timestamps.PhRead);
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------------------------------------------------------------












//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------------------------------------
  
void setup() {
  // put your setup code here, to run once:


  Serial.begin(115200);
  Wire.begin(D1, D2);     //sda,scl
  Serial.print(Wire.available());

  ads.begin();
  pwm.begin();
  pwm.setPWMFreq(1600);
  timeClient.begin();
  


  //-----------------------------------------thermometer setup ----------------------------------------------
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  sensors.begin();
  numberOfDevices = sensors.getDeviceCount();
  // locate devices on the bus
  Serial.println();
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  for (int i = 0; i < numberOfDevices; i++) {
    Serial.print("address : ");
    Serial.println(sensors.getAddress(tempDeviceAddress, i));

    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i)) {

      // Output the device ID
      Serial.print(sensors.getAddress(tempDeviceAddress, i));
      Serial.println(i, DEC);
      // Print the data

      float tempC = sensors.getTempC(tempDeviceAddress);
      Serial.print("Temp C: ");
      Serial.println(tempC);

    }
  }



  //------------------------------------wifi graph setup ------------------------------------------------ -
  //////////////////////////////////////////webinterface
  configWebInterface();
  Serial.println("webinterfaceconfigured");
  webInterface.interfaceConfig("Sensor Measurements", "Inputfield", "Perform Action", "otherButton");
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
  Serial.println("reading sensor5");
  sensorReading5();

  WiFi.begin("BELL123-2G", "123456789");
  while (WiFi.status() != WL_CONNECTED and millis() < 3000 ) {
    delay(500);
    Serial.print(".");
  }
  if (millis() < 3000 ||WiFi.status() == WL_CONNECTED ){
    Serial.println("connected");
    Serial.print("Use this URL: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    
    SyncOnlineTime();
    Serial.println();
    Serial.print("TIME is  : "); 
    Serial.println(timeClient.getHours());
  }else{
    Serial.println("did not connect to internet");
  }
}

// ------------------------------------------------------------------------------------------------------------












void loop() {
  // put your main code here, to run repeatedly:
  // --------------------------------------------------------graph loop --------------------------------------------
  
  //temperature
  if ((millis() - Timestamps.TempRead) > (GraphUpdates.TempRead * 1000)) {
    Serial.println("reading temperature");
    sensorReading1();
  }
  //light 
  if ((millis() - Timestamps.LightRead) > (GraphUpdates.LightRead * 1000)) {
    Serial.println("reading light level");
    sensorReading2();
  }
  //water level
  if ((millis() - Timestamps.WaterLevel) > (GraphUpdates.WaterLevel * 1000)) {
    Serial.println("reading water level");
    waterLevelControl();
    sensorReading3();
  }
   // Graph 4 population density
  if ((millis() - Timestamps.PopulationRead) > (GraphUpdates.PopulationRead * 1000)) {
    Serial.println("reading population density");
    sensorReading4();
  }
  // Graph 5 Ph graph
  if ((millis() - Timestamps.PhRead) > (GraphUpdates.PhRead * 1000)) {
    Serial.println("reading Ph value");
    sensorReading5();
  }
  if ((millis() - Timestamps.TimeCheck) > (GraphUpdates.TimeCheck * 1000)) {
    TimeProfileManagement();
    Timestamps.TimeCheck = millis();
  }
//  printTime();
  TemperatureCycleManagement();
  
  LightCycleManager();

  for (int i = 0; i <= 30; i++){ 
    //Serial.println("handleclient");
    server.handleClient();  
  }
  delay(50);
    
}
// -------------------------------------------------------------------------------------------- -
