#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// Replace with your network credentials
const char* ssid     = "your_SSID";
const char* password = "your_PASSWORD";

// Replace with your hotspot credentials
const char* ap_ssid     = "your_AP_SSID";
const char* ap_password = "your_AP_PASSWORD";

void setupWiFi();
void connectToWiFi(const char* ssid, const char* password);
void createAP(const char* ssid, const char* password);
void setupWiFi(const char* ssid, const char* password);
void checkWiFi();
vstd::vector<String> scanNetworks();

#endif
