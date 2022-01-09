// todo : new hotpspot if no wifi
// + add a connection setting in the UI
// + find how the esp8266 memorises wifis

void TemperatureCycleManagement() {
  int nowhour = GetHour();
  if (morningtime < nowhour && nowhour < noontime) {
    Temps.MinTreshold = Temps.DayMin;
    Temps.MaxTreshold = Temps.DayMax;
  } else {
    Temps.MinTreshold = Temps.NightMin;
    Temps.MaxTreshold = Temps.NightMax;
  }
  if (LastValue.TempRead > Temps.MaxTreshold) {
    heating_off();
  } else if (LastValue.TempRead < Temps.MinTreshold) {
    if (LastValue.TempRead == -127) {
      Serial.println("thermometer disconnected");
      setup_thermometer();
    } else {
      heating_on();
    }
  }
}

void setup_wifi() {
  Serial.println("connecting to wifi");
  server.begin();
  Serial.println("server began");

  WiFi.begin("WIFI_NAME", "Bertrande");
  Serial.println("Wifi Began");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED and i < 20) {
    Serial.print(".");
    delay(500);
    i++;
  }
  if (WiFi.status() == WL_CONNECTED ) {
    Serial.println("connected");
    Serial.print("Use this URL: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");
    Serial.println();
    Serial.print("TIME is  : ");
    Serial.println(GetHour());
  } else {
    Serial.println("did not connect to internet");
    Serial.print(WiFi.localIP());
  }
}
