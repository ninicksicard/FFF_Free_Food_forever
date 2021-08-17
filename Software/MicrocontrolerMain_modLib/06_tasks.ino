void setup_routine(){
  LastValue.TempRead = temperature_sens();
  LastValue.LightRead = lightSens();
  LastValue.WaterLevel = mainTankLevel();
  LastValue.PopulationRead = PopulationManagement();
  LastValue.PhRead = ph_sens();
  Setup_pH_Sensor();  
  
  //LightCycleManager();
  TemperatureCycleManagement();
  TimeProfileManagement();
}
void routine(){
  //temperature
  if ((millis() - Timestamps.TempRead) > (GraphUpdates.TempRead * 1000)) {
    Serial.println("reading temperature");
    LastValue.TempRead = temperature_sens();
    TemperatureCycleManagement();
    Timestamps.TempRead = millis();
  }
  //light 
  if ((millis() - Timestamps.LightRead) > (GraphUpdates.LightRead * 1000)) {
    Serial.println("reading light level");
    LastValue.LightRead = lightSens();
    //LightCycleManager();
    Timestamps.LightRead = millis();
  }
  //water level
  if ((millis() - Timestamps.WaterLevel) > (GraphUpdates.WaterLevel * 1000)) {
    Serial.println("reading water level");
    LastValue.WaterLevel = mainTankLevel();
    waterLevelControl();
    Timestamps.WaterLevel = millis();
    
  }
   // Graph 4 population density
  if ((millis() - Timestamps.PopulationRead) > (GraphUpdates.PopulationRead * 1000)) {
    Serial.println("reading population density");
    LastValue.PopulationRead = PopulationManagement();
    Timestamps.PopulationRead = millis();
  }
  
  // Graph 5 Ph graph   
  if (FeatureEnable.PhRead && (millis() - Timestamps.PhRead) > (GraphUpdates.PhRead * 1000)) {
    Serial.println("reading Ph value");
    LastValue.PhRead = ph_sens();
    Timestamps.PhRead = millis();
  }


  // manage clocks
  if ((millis() - Timestamps.TimeCheck) > (GraphUpdates.TimeCheck * 1000)) {
    TimeProfileManagement();
    Timestamps.TimeCheck = millis();
  }
  // update web interface
  UI_loop();
}
