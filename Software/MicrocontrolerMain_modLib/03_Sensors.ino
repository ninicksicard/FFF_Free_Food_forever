/////////////////////// general purpos sensor read //////////////////////////////

float sensorRead(int sensor[]) {
  Serialprintln("reading sensor");
  //sensor is a list of 2 ints, [power pin, read pin]

  if (sensor[0] != 22) {
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


  if (sensor[0] != 22) {
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
  //todo : remove return and use global variables
  if (!FeatureAvailable.DensityRead) {
    Serial.println("WaterLevel unavaillable");

    return -1;
  }
  digitalWrite(SensorsOut.WaterLevelSensor, HIGH);
  delay(100);

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

// todo : add set density treshold memory write that works with the button

float PopulationManagement() {
  if (!FeatureAvailable.DensityRead) {
    Serial.println("DensityRead unavaillable");
    extraction_off();
    return -1;
  }
  digitalWrite(SensorsOut.DensitySensor, HIGH);
  delay(100);
  float sensorread = sensorRead(densityPhotoresistor);
  Variables.Density = Calibration.DensityFactor * sensorread + Calibration.DensityOffset;
  LastValue.PopulationRead = Variables.Density;
  Serial.print("population density : ");
  Serial.println(Variables.Density);

  // todo : remove activation from here and put seperately in controls.  --->
  // todo : add system state for extraction
  if (Variables.Density > Variables.DensityHighTreshold) {
    extraction_on();
  } else {
    extraction_off();
  }
  //<--

  digitalWrite(SensorsOut.DensitySensor, LOW);

  return Variables.Density; //remove returns and just use the global variables
}





// -------------------------------------------- Ph management --------------------------------------
/*  ph sensor
    ph dosing(screw motor)
*/

int phSensor[2] = {SensorsOut.PhSensor, Sensors.PhSensor}; //x: main power relay pin or number y: ph analog input

void Setup_pH_Sensor() {
  double storedoffset = eeGetInt(1);
  double storedfact = eeGetInt(2)/1000;
  Serial.print("stored phOffset = ");
  Serial.print(storedoffset);
  Serial.print("stored phFact = ");
  Serial.print(storedfact);
  if (storedoffset != 0 && storedfact != 0) {
    Serial.print("stored phOffset = ");
    Serial.print(storedoffset);
    Calibration.phOffset = storedoffset;
    Serial.print("stored phFact = ");
    Serial.print(storedfact);
    Calibration.phFactor = storedfact ;
  } else {
    Serial.println("no calibration data stored on device");
  }
}

void Calibrate_pH_sensor() {
  if (Ph_Sensor_Variables.calibrate_7 != 0) {
    if (Ph_Sensor_Variables.calibrate_4 != 0) {
      if (Ph_Sensor_Variables.calibrate_10 != 0) {
        double fact = 3 / (Ph_Sensor_Variables.calibrate_10 - Ph_Sensor_Variables.calibrate_7);
        double offset = 7 - fact * Ph_Sensor_Variables.calibrate_7;

        Serial.print("imprecision at 10 : ");
        Serial.print( Ph_Sensor_Variables.calibrate_10 * fact + offset);
        Serial.print(" - 10 = ");
        Serial.println(Ph_Sensor_Variables.calibrate_10 * fact + offset - 10);
        Serial.print("imprecision at 4 : ");
        Serial.print(Ph_Sensor_Variables.calibrate_4 * fact + offset);
        Serial.print(" - 4 = ");
        Serial.println(Ph_Sensor_Variables.calibrate_4 * fact + offset - 4);

        Calibration.phOffset = offset;
        Calibration.phFactor = fact;
        Serial.print("Calibration.phoffet = ");
        Serial.println(Calibration.phOffset);
        Serial.print("Calibration.phfactor = ");
        Serial.println(Calibration.phFactor);
        eeWriteInt(1, Calibration.phOffset);
        eeWriteInt(2, Calibration.phFactor*1000);
      }
      else {
        Serial.println("please calibrate at ph 10");
      }
    } else {
      Serial.println("please calibrate at ph 4");
    }
  } else {
    Serial.println("please calibrate at ph 7");
  }
}

void Set_pH_Calibrate_7() {
  if (!FeatureAvailable.PhRead) {
    Serial.println("phread unavaillable");
    return;
  }
  Aeration_off();
  delay(1000);
  Ph_Sensor_Variables.calibrate_7 = sensorRead(phSensor);
  Serial.print("Ph_Sensor_Variables.calibrate_7 = ");
  Serial.println(Ph_Sensor_Variables.calibrate_7);
  Calibrate_pH_sensor();
  Aeration_on();
}

void Set_pH_Calibrate_10() {
  if (!FeatureAvailable.PhRead) {
    Serial.println("phread unavaillable");
    return;
  }
  Aeration_off();
  delay(1000);
  Ph_Sensor_Variables.calibrate_10 = sensorRead(phSensor);
  Serial.print("Ph_Sensor_Variables.calibrate_10 = ");
  Serial.println(Ph_Sensor_Variables.calibrate_10);
  Calibrate_pH_sensor();
  Aeration_on();
}

void Set_pH_Calibrate_4() {
  if (!FeatureAvailable.PhRead) {
    Serial.println("phread unavaillable");
    return;
  }
  Aeration_off();
  delay(1000);
  Ph_Sensor_Variables.calibrate_4 = sensorRead(phSensor);
  Serial.print("Ph_Sensor_Variables.calibrate_4 = ");
  Serial.println(Ph_Sensor_Variables.calibrate_4);
  Calibrate_pH_sensor();
  Aeration_on();
}

float ph_sens() {
  if (!FeatureAvailable.PhRead) {
    Serial.println("phread unavaillable");
    return -1;
  }
  Aeration_off();
  double ph;
  double phCalFact = Calibration.phFactor;        //-776.833333333
  double phCalOffset = Calibration.phOffset;                   //-4.84617034971
  double phread;

  delay(4000);        //todo : replace delay with timestamps
  phread = sensorRead(phSensor);
  delay(100);

  ph = phread * phCalFact * + phCalOffset;

  Serial.println();
  Serial.print("phread : ");
  Serial.println(phread);
  Serial.print("phCalFact*phread : ");
  Serial.println(phCalFact * phread);
  Serial.print("phCalOffset : ");
  Serial.println(phCalOffset);
  Serial.print("phCalFact : ");
  Serial.println(phCalFact);
  Serial.print("ph : ");
  Serial.println(ph);

  Aeration_on();
  if (ph > 0) {
    return ph;
  } else
    return -1;

}




///////////////// thermometers input ////////////
int numberOfDevices;

#define ONE_WIRE_BUS Sensors.Thermometer          //x: the pin for thermometer read
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

void setup_thermometer() {

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
  temp = sensors.getTempC(tempDeviceAddress); // this sensor should not need calibration
  Serial.print("temperature sensed is : ");
  Serial.println(temp);
  if (temp == -127){
    setup_thermometer();
  }
  Serial.print("temperature sensed is : ");
  Serial.println(temp);
  Serial.print("versus last tmep : ");
  Serial.println(LastValue.TempRead);
  return temp;
}
