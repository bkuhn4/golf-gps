#ifndef BATTERY_FUEL_GAUGE_H
#define BATTERY_FUEL_GAUGE_H

#include <Arduino.h>
#include <Wire.h>
#include <SparkFunBQ27441.h>

class BatteryFuelGauge {
public:
    BatteryFuelGauge();
    bool begin();
    void printStats();
    unsigned int getVoltage();
    unsigned int getSOC();
    int getAverageCurrent();

private:
    // The SparkFunBQ27441 library uses a global 'lipo' object, 
    // but we can wrap the calls here.
};

#endif // BATTERY_FUEL_GAUGE_H
