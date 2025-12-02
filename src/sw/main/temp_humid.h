#ifndef TEMP_HUMID_H
#define TEMP_HUMID_H

#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"

class TempHumidSensor {
public:
    TempHumidSensor();
    bool begin();
    float getTemperature();
    float getHumidity();

private:
    Adafruit_SHT31 sht31;
};

#endif // TEMP_HUMID_H
