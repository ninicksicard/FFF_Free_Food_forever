// todo : new hotpspot if no wifi
// + add a connection setting in the UI
// + find how the esp8266 memorises wifis

void setup_wifi() {
  Serial.println("connecting to wifi");
  server.begin();
  Serial.println("server began");

  WiFi.begin("Sylvie", "Bertrande");
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
