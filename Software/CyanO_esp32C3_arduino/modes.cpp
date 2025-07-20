#include "Arduino.h"
#include "modes.h"
#include <ESP32Servo.h>
#include <esp_log.h>
#include "cli.h" 
#define AIR_PUMP_PIN D10
#define SERVO_PIN D6
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 10 // 10 bit resolution
#define PWM_FREQUENCY 5000 // 5 kHz frequency
#define MAX_PWM 1023 // Maximum for 10  x-bit
#define SERVO_OVERSHOOT 20 // Overshoot in degrees
#define PUMP_ON_DURATION 600000 // 10 minute in milliseconds

int lastServoPosition = 0;
Servo servo;


ModeManager::ModeManager()
    // name, light, heat, water, servoPosition, pumpSpeed, pumpDutyCycle
    : modes{
      Mode("standby", false, false, false, 90, 0, 0.1),
      Mode("aeration", true, true, false, 20, 520, 0.1),
      Mode("extract", true, true, false, 160, MAX_PWM, 1.0),
      Mode("water", true, true, true, 70, 0, 1.0),
      Mode("clean", true, false, true, 120, 0, 1.0),
      Mode("Mode6", true, true, false, 20, 0, 1.0)
      } {
    currentMode = &modes[1];
    lastAppliedMode = nullptr;
}

void ModeManager::setCurrentMode(int modeNumber) {
    ESP_LOGI("Modes", "Modes update called");
    if (modeNumber >= 1 && modeNumber <= 6) {
      currentMode = &modes[modeNumber - 1];
      ESP_LOGI("Modes", "Current mode set to %d", modeNumber);
    } else {
      ESP_LOGE("Modes", "Invalid mode number: %d", modeNumber);
    }
}

void ModeManager::applyCurrentMode() {
    if (currentMode != lastAppliedMode) {
      setPump(currentMode->pumpSpeed);
      setServo(currentMode->servoPosition);
      lastAppliedMode = currentMode;
      lastPumpToggleTime = millis();
    }
    controlPump();
}

int ModeManager::getCurrentModePumpSpeed() {
    return currentMode->pumpSpeed;
}

int ModeManager::getCurrentModeServoPosition() {
    return currentMode->servoPosition;
}


String ModeManager::getCurrentModeName() {
    return currentMode->name;
}

bool ModeManager::getCurrentModeLight() {
    return currentMode->light;
}

bool ModeManager::getCurrentModeHeat() {
    return currentMode->heat;
}

bool ModeManager::getCurrentModeWater() {
    return currentMode->water;
}

void ModeManager::toggleLight() {
    currentMode->light = !currentMode->light;
}

void ModeManager::toggleHeat() {
    currentMode->heat = !currentMode->heat;
}

void ModeManager::toggleWater() {
    currentMode->water = !currentMode->water;
}

void ModeManager::controlPump() {
  unsigned long currentMillis = millis();
  unsigned long cycleDuration = isPumpRunning ? currentMode->pumpDutyCycle * PUMP_ON_DURATION
                                              : (1 - currentMode->pumpDutyCycle) * PUMP_ON_DURATION;

  if (currentMillis - lastPumpToggleTime >= cycleDuration) {
    isPumpRunning = !isPumpRunning;
    setPump(isPumpRunning ? currentMode->pumpSpeed : 0);
    lastPumpToggleTime = currentMillis;
  }
}

ModeManager modeManager;

void setupModes() {
  // Setup PWM
  pinMode(AIR_PUMP_PIN, OUTPUT);
  //servo.attach(SERVO_PIN);
  ESP_LOGI("Modes", "Modes setup completed");

}

void updateModes() {
  modeManager.applyCurrentMode();
}

void setPump(int speed) {
  analogWrite(AIR_PUMP_PIN, speed);
  ESP_LOGI("Modes", "Pump set to speed %d", speed);
}

void setServo(int position) {
  int overshootPosition = position + (position > lastServoPosition ? SERVO_OVERSHOOT : -SERVO_OVERSHOOT);
  customPrint("position : ");
  customPrintln(position);
  customPrint("overshootPosition : ");
  customPrintln(overshootPosition);
  servo.attach(SERVO_PIN); // attach the servo to move
  delay(5);
  servo.write(overshootPosition);
  delay(2000); // Wait for servo to reach overshoot position
  servo.write(position);
  delay(1000); // Wait for servo to reach final position
  
  servo.detach(); // Detach the servo to not overheat
  
  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW); // or HIGH, depending on what works best for your servo
  
  lastServoPosition = position;
  ESP_LOGI("Modes", "Servo set to position %d", position);
}

void setCurrentMode(int modeNumber) {
  modeManager.setCurrentMode(modeNumber);
}
