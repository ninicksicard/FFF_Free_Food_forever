#include "sensors.h"
#include "esp32-hal-touch.h"
#include "cli.h"
#include <esp_log.h>
WaterLevelSensor::WaterLevelSensor(int pin) : pin(pin), waterDetected(false) {
}

void WaterLevelSensor::setup() {
    pinMode(pin, INPUT);
    ESP_LOGI(TAG, "Digital Water Level Sensor on pin %d setup completed", pin);
}

void WaterLevelSensor::updateWaterLevel() {
    waterDetected = digitalRead(pin) == HIGH; // Read the digital signal
    ESP_LOGD(TAG, "Touch read value on pin %d: %d", pin, touchValue);
}

bool WaterLevelSensor::isWaterDetected() {
    return waterDetected;
}
