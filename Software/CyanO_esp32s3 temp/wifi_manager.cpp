#include "Arduino.h"
#include "WiFi.h"
#include "wifi_manager.h"


void setupWiFi(const char* ssid_ = ssid, const char* password_ = password) {
  connectToWiFi(ssid_, password_);

  if (WiFi.status() != WL_CONNECTED) {
    createAP(ap_ssid, ap_password);
  }
}



void connectToWiFi(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);

  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 10) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
  } else {
    Serial.println("Failed to connect to WiFi");
  }
}

void createAP(const char* ssid_, const char* password_) {
  WiFi.softAP(ssid_, password_);

  Serial.println("Access point created");
}




void checkWiFi() {
  // Check WiFi status
  if (WiFi.getMode() == WIFI_STA && WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, reconnecting...");
    setupWiFi();
  }
}


vstd::vector<String> scanNetworks() {
  // WiFi scan mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Start the scan
  int n = WiFi.scanNetworks();

  // Vector to hold the network information
  std::vector<String> networks;

  if (n == 0) {
    networks.push_back("No networks found");
  } else {
    for (int i = 0; i < n; ++i) {
      // Create a string with the SSID and RSSI for each network found
      String network = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ")";
      networks.push_back(network);
    }
  }

  return networks;
}


