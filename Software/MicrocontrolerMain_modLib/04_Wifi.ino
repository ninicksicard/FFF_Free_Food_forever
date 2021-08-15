// todo rename sensorreads properly

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------- webserver graphs --------------------------------------------------------

// should allow for wifi control
// Graph 1 temperature
int measurementsCount1 = 0;
float measurements1[63] = {};
// Graph 2 Light Level
int measurementsCount2 = 0;
float measurements2[63] = {};
// Graph 3 main tank water level
int measurementsCount3 = 0;
float measurements3[63] = {};
// Graph 4 populationDensity
int measurementsCount4 = 0;
float measurements4[63] = {};
// Graph 5 Ph graph
int measurementsCount5 = 0;
float measurements5[63] = {};


int interfaceCallback(String input) {
  // use the input for example as a password and trigger some action
  // or use it as a command and execute accordingly
  // when the input was valid you should return a 1
  // in case of a invalid input return a 0
  
  if (input == "lightSwitch_on") {
    if (FeatureAvailable.Lighting){
      FeatureEnable.Lighting = true;  
    }
    lightSwitch_on();
    return 1;
  }
  if (input == "lightSwitch_off") {
    FeatureEnable.Lighting = false; 
    lightSwitch_off();
    return 1;
  }
  
  if (input == "heating_on") {
    if (FeatureAvailable.Heating){
      FeatureEnable.Heating = true;  
    }
    Temps.MinTreshold = Temps.DayMin;
    Temps.MaxTreshold = Temps.DayMax;
    heating_on();
    return 1;
  }
  if (input == "heating_off") {
    FeatureEnable.Heating = false; 
    Temps.MinTreshold = 0;
    Temps.MaxTreshold = 1;
    heating_off();
    return 1;
  }
  
  if (input == "Aeration_on") {
    if (FeatureAvailable.Aeration){
      FeatureEnable.Aeration = true;  
    }
    Aeration_on();
    return 1;
  }
  if (input == "Aeration_off") {
    FeatureEnable.Aeration = false;  
    Aeration_off();
    return 1;
  }
  if (input == "DensityRead_on") {
    FeatureEnable.DensityRead = true;  
    return 1;
  }
  if (input == "DensityRead_off") {
    FeatureEnable.DensityRead = false;  
    return 1;
  }
  if (input == "WaterLevel_on") {
    FeatureEnable.WaterLevel = true;  
    return 1;
  }
  if (input == "Water_Level_off") {
    FeatureEnable.WaterLevel = false;  
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


void TemperatureCycleManagement(){
  int nowhour = GetHour();
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


void sensorReading2() {
  float sensorInput = lightSens()*0.00003051757;    // <- sensor reading for second input
  sensorReading(measurements2, &measurementsCount2, 128, &Timestamps.LightRead, sensorInput);
}


void sensorReading3() {
  float sensorInput = mainTankLevel();    // <- sensor reading for second input
  Variables.WaterLevel = sensorInput;
  sensorReading(measurements3, &measurementsCount3, 128, &Timestamps.WaterLevel, sensorInput);
}


void sensorReading4() {
  float sensorInput = PopulationManagement()*0.0012872774;    // <- sensor reading for second input
  sensorReading(measurements4, &measurementsCount4, 128, &Timestamps.PopulationRead, sensorInput);
}

void sensorReading5() {
  float sensorInput = ph_sens();    // <- sensor reading for second input 
  sensorReading(measurements5, &measurementsCount5, 128, &Timestamps.PhRead, sensorInput);
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


void setup_wifi(){
  //------------------------------------wifi graph setup ------------------------------------------------ -
  //////////////////////////////////////////webinterface
  configWebInterface();
  Serial.println("Web Interface Configuration");
  webInterface.interfaceConfig("Sensor Measurements", "Inputfield", "Perform Action", "otherButton");
  webInterface.serverResponseSetup(&server, &interfaceCallback);
  server.begin();
  Serial.println("server began");
  
  WiFi.begin("BELL123-2G", "123456789");
  Serial.println("Wifi Began");
  int i=0;
  while (WiFi.status() != WL_CONNECTED and i<20) {
    Serial.print(".");
    delay(500);
    i++;
  }
  if (WiFi.status() == WL_CONNECTED ){
    Serial.println("connected");
    Serial.print("Use this URL: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    Serial.println();
    Serial.print("TIME is  : "); 
    Serial.println(GetHour());
  }else{
    Serial.println("did not connect to internet");
    Serial.print(WiFi.localIP());
  }
}
