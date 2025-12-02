#include "battery_fuel_gauge.h"

BatteryFuelGauge::BatteryFuelGauge() {
    // Constructor
}

void BatteryFuelGauge::printStats() {
    Serial.println("\n--- Battery Stats ---");
    Serial.print("Voltage: "); Serial.print(lipo.voltage()); Serial.println(" mV");
    Serial.print("State of Charge: "); Serial.print(lipo.soc()); Serial.println(" %");
    Serial.print("Average Current: "); Serial.print(lipo.current(AVG)); Serial.println(" mA");
}

bool BatteryFuelGauge::begin() {
    Serial.print("Initializing BQ27441 Fuel Gauge... ");
    if (!lipo.begin()) {
        Serial.println("FAILED");
        return false;
    } else {
        Serial.println("OK");
        lipo.setCapacity(1500);
        return true;
    }
}

unsigned int BatteryFuelGauge::getVoltage() {
    return lipo.voltage();
}

unsigned int BatteryFuelGauge::getSOC() {
    return lipo.soc();
}

int BatteryFuelGauge::getAverageCurrent() {
    return lipo.current(AVG);
}
