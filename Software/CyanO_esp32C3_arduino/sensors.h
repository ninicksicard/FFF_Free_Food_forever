#ifndef SENSORS_H
#define SENSORS_H

#include "Arduino.h"
#include <esp_log.h>

class WaterLevelSensor {
public:
    WaterLevelSensor(int pin);
    void setup();
    void updateWaterLevel();
    bool isWaterDetected(); // Change to return boolean for digital signal

private:
    int pin;
    bool waterDetected; // Store the state of water detection
};

#endif
