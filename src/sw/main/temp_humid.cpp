#include "temp_humid.h"

TempHumidSensor::TempHumidSensor() {
    // Constructor
}

bool TempHumidSensor::begin() {
    Serial.print("Initializing SHT31 Sensor... ");
    // The Adafruit SHT31 library's begin() method only takes the address.
    // It assumes Wire is already initialized or initializes it internally.
    if (!sht31.begin(0x44)) {
        Serial.println("FAILED");
        return false;
    } else {
        Serial.println("OK");
        return true;
    }
}

float TempHumidSensor::getTemperature() {
    return sht31.readTemperature();
}

float TempHumidSensor::getHumidity() {
    return sht31.readHumidity();
}
