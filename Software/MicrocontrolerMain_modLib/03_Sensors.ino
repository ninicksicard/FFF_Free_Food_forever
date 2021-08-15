/////////////////////// general purpos sensor read //////////////////////////////

float sensorRead(int sensor[]) {
  Serialprintln("reading sensor");
  //sensor is a list of 2 ints, [power pin, read pin]
  
  if (sensor[0] != 22){
    switchRelay(sensor[0], HIGH);
  }
  
  delay(preReadDelay);
  
  Serialprint("ads1115 port is : ");
  Serialprintln(String(sensor[1]));
  Serialprintln("trying to read the ads1115");

  float sensor_value;
  sensor_value = ads.readADC_SingleEnded(sensor[1]);
  delay(preReadDelay);


  Serialprint("sensor_value is : ");
  Serialprintln(String(sensor_value));


  if (sensor[0] != 22){
    switchRelay(sensor[0], LOW);
  }

  
  Serialprintln("now return");
  return sensor_value;
}


////////////////// light level sensor /////////////////
/*  photoresistor power on
    analog read of photoresistor
    photoresistor power off
    adjust value with calibration data
    log or save temperature value
*/

int Light_Sensor[2] = {SensorsOut.LightSensor, Sensors.LightSensor};

float lightSens() {
  float light_level;
  light_level = sensorRead(Light_Sensor) * Calibration.LightLevelFactor + Calibration.LightLevelOffset;
  return light_level;
}


/////////////Main tank water level sensor/////////////
int main_Level_Sensor[2] = {22, Sensors.WaterLevelSensor}; // x: sensor power pin, y:sensor analog in pin

float mainTankLevel() {//todo : add system state for water level to avoid the consecutive reads.
  
  digitalWrite(SensorsOut.WaterLevelSensor, HIGH);
  delay(400);
  float sensorread = sensorRead(main_Level_Sensor);
  Variables.WaterLevel = Calibration.WaterLevelFactor * sensorread  + Calibration.WaterLevelOffset;
  
  Serial.print("WaterLevel : ");
  Serial.println(Variables.WaterLevel);
  
  digitalWrite(SensorsOut.WaterLevelSensor, LOW);
  return Variables.WaterLevel; //remove returns and just use the global variables
}


// ---------------------------------------- population management ---------------------------------
/*  photodensity sensor
*/

int densityPhotoresistor[2] = {22, Sensors.DensitySensor};

float PopulationManagement() {
  digitalWrite(SensorsOut.DensitySensor, HIGH);
  
  Variables.Density = Calibration.DensityFactor * sensorRead(densityPhotoresistor) + Calibration.DensityOffset;
  
  Serial.print("population density : ");
  Serial.println(Variables.Density);
  // remove activation from here and put seperately in controls.  --->
  if (Variables.Density * 0.0012872774 > 2){//todo replace with calib vars
     extraction_on();
  }else{
    extraction_off();
  }
  //<--
  
  digitalWrite(SensorsOut.DensitySensor, LOW);
  return Variables.Density;//remove returns and just use the global variables
}





// -------------------------------------------- Ph management --------------------------------------
/*  ph sensor
    ph dosing(screw motor)
*/

int phSensor[2] = {SensorsOut.PhSensor, Sensors.PhSensor}; //x: main power relay pin or number y: ph analog input


float ph_sens() {
  Aeration_off();
  float ph;
  float phCalFact = -0.0012872774;        //-776.833333333
  float phCalOffset = 24.6461703497;                   //-4.84617034971
  float phread;
  
  delay(5000);        //todo : replace delay with timestamps
  phread = sensorRead(phSensor);
  delay(200);
  
  ph = phread* phCalFact* + phCalOffset;
  
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
  
  Aeration_on();
  
  return ph;
}




///////////////// thermometers input ////////////

int tempCalFact = 1;
int tempCalOffset = 0 ;
int numberOfDevices;
#define ONE_WIRE_BUS 14          //x: the pin for thermometer read
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
