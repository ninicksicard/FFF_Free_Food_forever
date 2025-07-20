#include <WiFiSetup.h>
#include <TaskScheduler.h>
#include "sensors.h"
#include "modes.h"
#include "user_interface.h"
#include "time_sync.h"
#include "cli.h"
// #include "web_server.h"
// #include "notifications.h"
// #include "file_manager.h"

#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ArduinoOTA.h>

WiFiSetup wifiSetup("FFF");  // You can customize the AP name if needed

unsigned long resetTimerStart = 0;
const unsigned long resetInterval = 120000;  // 2 minutes in milliseconds
bool is_host = false;

WaterLevelSensor tank1(D4);  //TOUCH_PAD_NUM4);
WaterLevelSensor tank2(D5);  //TOUCH_PAD_NUM5);
UserInterface ui = UserInterface();

Scheduler runner;

// Define tasks
Task t1(5000, TASK_FOREVER, []() {
  tank1.updateWaterLevel();
  tank2.updateWaterLevel();
});

Task t2(500, TASK_FOREVER, []() {
  handleCLI();
  updateModes();
});

Task t3(10000, TASK_FOREVER, []() {
  syncTime();
});

void setup() {
  tank1.setup();
  tank2.setup();
  setupCLI();
  setupModes();   // Initialize the modes
  updateModes();  // Apply the current mode

  wifiSetup.begin();
  Serial.print("Connected with IP :");
  Serial.println(WiFi.localIP());
  IPAddress ip = WiFi.localIP();  // Get the local IP address
  String ipStr = ip.toString();   // Convert the IP address to a String

  ui.setupUserInterface();
  setupTimeSync();

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else  // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();



  is_host = ipStr.endsWith(".1");
  if (!MDNS.begin("FFF")) {
    Serial.println("Error setting up MDNS responder!");
  }
  // Add tasks to the runner
  runner.addTask(t1);
  runner.addTask(t2);
  runner.addTask(t3);

  // Enable tasks
  t1.enable();
  t2.enable();
  t3.enable();
}

void loop() {
  ArduinoOTA.handle();
  if (is_host) {
    if (millis() - resetTimerStart >= resetInterval) {
      Serial.println("Resetting device...");
      ESP.restart();  // Reset the device
    }
  }

  // sendNotifications();
  // manageFiles();

  runner.execute();  // Execute the scheduler runner
}
