#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

// Pin Definitions
#define GPS_EN_N_PIN 3
#define GPS_RESET_N_PIN 21

class GpsHandler {
public:
    GpsHandler();
    bool begin();
    
    // Data Getters
    long getLatitude();
    long getLongitude();
    byte getSIV(); // Satellites In View
    bool isLocked(); // Check if we have a valid fix
    
    // Time and Date
    // Returns time as HHMMSS
    uint32_t getTime();
    // Returns date as DDMMYY
    uint32_t getDate();

    // Hardware Reset
    void reset();
    
    // Distance Calculation (Vincenty Formula)
    // Returns distance in meters between two coordinates (in decimal degrees)
    double calculateDistanceVincenty(double lat1, double lon1, double lat2, double lon2);
    
    // Direct access to the GNSS object if needed
    SFE_UBLOX_GNSS myGNSS;
};

#endif // GPS_H
