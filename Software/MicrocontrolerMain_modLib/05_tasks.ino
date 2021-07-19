void setup_routine(){
  sensorReading1();
  sensorReading2();
  sensorReading3();
  sensorReading4();
  sensorReading5();
}
void routine(){
  //temperature
  if ((millis() - Timestamps.TempRead) > (GraphUpdates.TempRead * 1000)) {
    Serial.println("reading temperature");
    sensorReading1();
    TemperatureCycleManagement();
  }
  //light 
  if ((millis() - Timestamps.LightRead) > (GraphUpdates.LightRead * 1000)) {
    Serial.println("reading light level");
    sensorReading2();
    LightCycleManager();
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


  // manage clocks
  if ((millis() - Timestamps.TimeCheck) > (GraphUpdates.TimeCheck * 1000)) {
    TimeProfileManagement();
    Timestamps.TimeCheck = millis();
  }
  // update web interface
  for (int u = 0; u <= 10; u++){ 
    server.handleClient();  
  }
}
