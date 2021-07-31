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
  delay(preReadDelay);
//  Serial.print("sensor_value is : ");
//  Serial.println(sensor_value);
  
  switchRelay(sensor[0], LOW);
//  Serial.println("now return");
  return sensor_value;
}


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


/////////////Main tank water level sensor/////////////
float mainLevelCalFact = 0.00003051757;
float mainLevelCalOffset = 0;
int main_Level_Sensor[2] = {9, Sensors.WaterLevelSensor}; // x: sensor power pin, y:sensor analog in pin

float mainTankLevel() {//todo : add system state for water level to avoid the consecutive reads.
  float Water_Level;
  digitalWrite(D7, HIGH);
  Water_Level = sensorRead(main_Level_Sensor) * mainLevelCalFact + mainLevelCalOffset;
  Serial.print("WaterLevel : ");
  Serial.println(Water_Level);
  digitalWrite(D7, LOW);
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


// ---------------------------------------- population management ---------------------------------
/*  photodensity sensor
*/
////////////////////////////////////////////////////////
int densityPhotoresistor[2] = {Relays.DensitySensor, Sensors.DensitySensor}; // x:photoresistor power pin y:photoresistor analog input pin

float PopulationManagement() {
  digitalWrite(D7, HIGH);
  float density;
  density = sensorRead(densityPhotoresistor);
  if (density*0.0012872774 > 2){
     extraction_on();
  }else{
    extraction_off();
  }
  digitalWrite(D7, LOW);
  return density;
}





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





///////////////// thermometers input ////////////
int tempCalFact = 1;
int tempCalOffset = 0 ;
int numberOfDevices;
#define ONE_WIRE_BUS 14 //x: the pin for thermometer read
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

void setup_thermometer(){
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
}
float temperature_sens() {  // todo : add a power up and reset of ds18b20
  float temp;
  sensors.requestTemperatures();
 // oneWire.reset();
  temp = sensors.getTempC(tempDeviceAddress) * tempCalFact + tempCalOffset;
  return temp;
}
