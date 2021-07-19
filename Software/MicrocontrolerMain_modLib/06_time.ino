///////////////////////////////////////////////////////TimeManagement///////////////////////////////////

void printTime(){
  Serial.print(t.getHour(h12Flag, pmFlag));
  Serial.print(":");
  Serial.print(t.getMinute());
  Serial.print(":");
  Serial.println(t.getSecond());
}

void UpdateRTC(){
  t.setClockMode(false);  // set to 24h
  t.setHour((int)timeClient.getHours());
  t.setMinute((int)timeClient.getMinutes());
  t.setSecond((int)timeClient.getSeconds()); 
}

void SyncOnlineTime(){
  timeClient.update();
  UpdateRTC();
}


int GetHour(){
  if (WiFi.status() == WL_CONNECTED ){
    SyncOnlineTime();
  }
  printTime();
  return t.getHour(h12Flag, pmFlag);
}

void TimeProfileManagement(){
  int nowhour = GetHour();
  if (morningtime < nowhour&&nowhour<noontime){
    Serial.println("day time");
    GraphUpdates.PhRead = 50; // duration in seconds
    Aeration_on();
    //Lights on
    if (!SystemStates.Lights){
      lightSwitch_on();
    }
    
    //Daytime temperature
    if (!SystemStates.Heat){//todo make sure day/night have their own system states
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
