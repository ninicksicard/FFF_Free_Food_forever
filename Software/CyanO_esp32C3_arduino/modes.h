#ifndef MODES_H
#define MODES_H

#include "Arduino.h"

#define MAX_MODES 6

extern int lastServoPosition;

class Mode {
public:
  Mode(String name, bool light, bool heat, bool water, int servoPosition, int pumpSpeed, float pumpDutyCycle)
    : name(name),
      light(light),
      heat(heat),
      water(water),
      servoPosition(servoPosition),
      pumpSpeed(pumpSpeed),
      pumpDutyCycle(pumpDutyCycle) {}

  String name;
  bool light;
  bool heat;
  bool water;
  int servoPosition;
  int pumpSpeed;
  float pumpDutyCycle;
};

class ModeManager {
public:
  ModeManager();
  
  void setCurrentMode(int modeNumber);
  void applyCurrentMode();
  int getCurrentModePumpSpeed();
  int getCurrentModeServoPosition();

  // New methods for getting mode properties
  String getCurrentModeName();
  bool getCurrentModeLight();
  bool getCurrentModeHeat();
  bool getCurrentModeWater();

  // New methods for toggling mode properties
  void toggleLight();
  void toggleHeat();
  void toggleWater();

private:
  Mode modes[MAX_MODES];
  Mode* currentMode;
  Mode* lastAppliedMode;
  unsigned long lastPumpToggleTime = 0;
  bool isPumpRunning = false;
  void controlPump();
};

extern ModeManager modeManager;

void setupModes();
void updateModes();
void setPump(int speed);
void setServo(int position);
void setCurrentMode(int modeNumber);

#endif
