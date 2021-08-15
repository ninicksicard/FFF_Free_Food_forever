///////////////////////////////////////////////////////TimeManagement///////////////////////////////////

void printTime() {
  Serial.print(t.getHour(h12Flag, pmFlag));
  Serial.print(":");
  Serial.print(t.getMinute());
  Serial.print(":");
  Serial.println(t.getSecond());
}

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


int GetHour() {
  if (WiFi.status() == WL_CONNECTED ) {
    SyncOnlineTime();
  }
  printTime();
  return t.getHour(h12Flag, pmFlag);
}
int GetMinute() {
  if (WiFi.status() == WL_CONNECTED ) {
    SyncOnlineTime();
  }
  printTime();
  return t.getMinute();
}

bool IsDaytime() {
  int nowhour = GetHour();
  if (morningtime < nowhour && nowhour < noontime) {
    return true;
  }
  return false;
}

void TimeProfileManagement() {
  int nowhour = GetHour();
  if (IsDaytime()) {
    Serial.println("day time");
    GraphUpdates.PhRead = 50; // duration in seconds
    if (SystemStates.NightTime) {
      FeatureEnable.Aeration = true;
      FeatureEnable.DensityRead = false;
      FeatureEnable.WaterLevel = false;
      FeatureEnable.PhRead = true;
      SystemStates.NightTime = false;
    }
    Aeration_on();
    //Lights on
    if (!SystemStates.Lights) {
      lightSwitch_on();
    }

    //Daytime temperature
    if (!SystemStates.Heat) { //todo make sure day/night have their own system states
      Temps.MinTreshold = Temps.DayMin;
      Temps.MaxTreshold = Temps.DayMax;
      Serial.println("DayTime temperatures");

    }
  }
  //SetupNight time profile

  else {
    Serial.println("night time");
    GraphUpdates.PhRead = 3600; // duration in seconds
    if (!SystemStates.NightTime) {
      Aeration_off();
      FeatureEnable.Aeration = false;
      FeatureEnable.DensityRead = false;
      FeatureEnable.WaterLevel = false;
      FeatureEnable.PhRead = false;
      SystemStates.NightTime = true;
    }

    //Lights off
    if (SystemStates.Lights) {
      lightSwitch_off();
    }

    //NightTime temperatures
    if (SystemStates.Heat) {
      Temps.MinTreshold = Temps.NightMin;
      Temps.MaxTreshold = Temps.NightMax;
      Serial.println("NightTime temperatures");

    }
  }
}
