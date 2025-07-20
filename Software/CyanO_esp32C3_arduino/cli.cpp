#include "Arduino.h"
#include "cli.h"
#include "modes.h"
#include "esp_log.h"
#include "WiFi.h"
#include "user_interface.h"

static const char* TAG = "CLI";

template <typename T>
void customPrintln(T message) {
  Serial.println(message);
  ui.printToWebln(String(message));
}

template <typename T>
void customPrint(T message) {
  Serial.print(message);
  ui.printToWeb(String(message));
}

// Specialization for IPAddress type
template <>
void customPrintln<IPAddress>(IPAddress message) {
  Serial.println(message);
  ui.printToWebln(String(message[0]) + "." + String(message[1]) + "." + String(message[2]) + "." + String(message[3]));
}

template <>
void customPrint<IPAddress>(IPAddress message) {
  Serial.print(message);
  ui.printToWeb(String(message[0]) + "." + String(message[1]) + "." + String(message[2]) + "." + String(message[3]));
}

template void customPrintln<String>(String message);
template void customPrint<String>(String message);
template void customPrintln<int>(int message);
template void customPrint<int>(int message);
template void customPrintln<char const*>(char const* message);
template void customPrint<char const*>(char const* message);
template void customPrintln<char*>(char* message);
template void customPrint<char*>(char* message);
template void customPrintln<touch_pad_t>(touch_pad_t);
template void customPrint<touch_pad_t>(touch_pad_t);
template void customPrint<unsigned short>(unsigned short);

void setupCLI() {
  Serial.begin(115200);  // Start the Serial communication to send messages to the computer
  delay(10);
  ESP_LOGI(TAG, "Starting...");
  // Any other CLI setup code goes here
}

void handleCLI() {
  // Check if there's any incoming data
  if (Serial.available() > 0) {
    // Read the incoming data
    String command = Serial.readStringUntil('\n');

    // Remove any trailing carriage return
    command.trim();

    // Process the command
    processCommand(command);
  }
}

void processCommand(String command) {
  if (command.startsWith("pump ") or command.startsWith("Pump ")) {
    int speed = command.substring(5).toInt();
    setPump(speed);
    ESP_LOGI(TAG, "Pump speed set to %d", speed);
  } else if (command == "mode status" or command == "get status") {
    customPrintln("Mode name: " + modeManager.getCurrentModeName());
    customPrintln("Light status: " + String(modeManager.getCurrentModeLight() ? "on" : "off"));
    customPrintln("Heat status: " + String(modeManager.getCurrentModeHeat() ? "on" : "off"));
    customPrintln("Water status: " + String(modeManager.getCurrentModeWater() ? "on" : "off"));
    customPrintln("Servo position: " + String(modeManager.getCurrentModeServoPosition()));
    customPrintln("Last Servo position: " + String(lastServoPosition));
    customPrintln("Pump speed: " + String(modeManager.getCurrentModePumpSpeed()));
    
  } else if (command.startsWith("servo ") or command.startsWith("Servo ")) {
    int position = command.substring(6).toInt();
    setServo(position);
    ESP_LOGI(TAG, "Servo set to position %d", position);
  } else if (command.startsWith("mode ")or command.startsWith("Mode ")) {
    int modeNumber = command.substring(5).toInt();
    setCurrentMode(modeNumber);
    customPrintln("Mode set to " + String(modeNumber));
  } else if (command == "light" or command == "Light") {
    modeManager.toggleLight();
    customPrintln("Light toggled");
  } else if (command == "heat" or command == "Heat") {
    modeManager.toggleHeat();
    customPrintln("Heat toggled");
  } else if (command == "water" or command == "Water") {
    modeManager.toggleWater();
    customPrintln("Water toggled");
  
  } else if (command == "get IP" or command == "Get IP" or command == "Get ip" or command == "get ip"){
    if (WiFi.status() == WL_CONNECTED) {
        customPrintln((WiFi.localIP()));
    } else {
        customPrintln("Not connected to WiFi");
    }
  } else if (command == "clear" or command == "Clear") {
    ui.clearMessageBuffer();
  } else {
    ESP_LOGI(TAG, "Unknown command: %s", command.c_str());
  }
}

