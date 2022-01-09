///////////////////////////////////////////////////////TimeManagement///////////////////////////////////

void UpdateRTC() {
  t.setClockMode(false);  // set to 24h
  t.setHour((int)timeClient.getHours());
  t.setMinute((int)timeClient.getMinutes());
  t.setSecond((int)timeClient.getSeconds());
}
void SyncOnlineTime() {
  timeClient.update();
  UpdateRTC();
}

// not sure why it needs to be as variables.
bool h12flag = true;
bool pmflag = false;

int GetHour() {
  if (!(WiFi.status() == WL_CONNECTED) ) {
    Serial.println("not connected");
    return t.getHour(h12flag, pmflag);
  }
  SyncOnlineTime();
  
  return t.getHour(h12flag, pmflag);
}


bool IsDaytime() {
  int nowhour = GetHour();
  return Variables.morningtime <= nowhour && nowhour < Variables.noontime;
}

void DayTimeVerif(){
  int nowhour = GetHour();
  is_day_time = ((Variables.morningtime <= nowhour) && (nowhour < Variables.noontime));
  Serial.print("is_day_time : ");
  Serial.println(is_day_time);
  return;
}

void TimeProfileManagement() {
//  if (IsDaytime()) {
//    Serial.println("day time");
//    if (SystemStates.NightTime) {
//      FeatureEnable.Aeration = true;
//      FeatureEnable.WaterLevel = true;
//      SystemStates.NightTime = false;
//    }
//    //Aeration_on();
//    //Lights on
//    if (!SystemStates.Lights) {
//      lightSwitch_on();
//    }
//
//  } else {
//   //SetupNight time profile
//    Serial.println("night time");
//    if (!SystemStates.NightTime) {
//      //Aeration_off();
//      FeatureEnable.Aeration = true;
//      FeatureEnable.WaterLevel = true;
//      SystemStates.NightTime = true;
//    }
//
//    //Lights off
//    if (SystemStates.Lights) {
//      lightSwitch_off();
//    }
//  }
}
