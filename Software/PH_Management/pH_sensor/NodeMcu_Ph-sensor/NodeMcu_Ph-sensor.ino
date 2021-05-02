#include <Arduino.h>
int pHSense = A0;
float adc_resolution = 1024.0;

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.println("cimpleo pH Sense");
}

float ph (float voltage) {
  return 7 + (32*(0.82 - voltage));
}

void loop () {
  int measurings=0;
  measurings=analogRead(pHSense);
  delay(10);
  
  float voltage = measurings / adc_resolution;
  Serial.println(voltage);
  Serial.print("pH= ");
  Serial.println(ph(voltage));
  delay(500);
}
