/////////////Main tank water level sensor/////////////
void mainTankLevel() {//todo : add system state for water level to avoid the consecutive reads.
//  if (!FeatureAvailable.WaterLevel || !FeatureEnable.WaterLevel) {
//    Serial.println("WaterLevel unavaillable");
//    Variables.WaterLevel = -1;
//  }
//  else {
  float sensorread = analogRead(Sensors.WaterLevelSensor);
  Variables.WaterLevel = Calibration.WaterLevelFactor * sensorread  + Calibration.WaterLevelOffset;
//    Serial.print("WaterLevel : ");
//    Serial.println(Variables.WaterLevel);
//  }
}
