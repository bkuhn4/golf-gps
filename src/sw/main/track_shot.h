#ifndef TRACK_SHOT_H
#define TRACK_SHOT_H

#include <Arduino.h>
#include "gps.h"

// Removed U8g2lib.h and button_handler.h as they are no longer needed here
// Removed TrackShotState enum

class TrackShot {
public:
    TrackShot();
    bool begin(GpsHandler* gpsObj);
    
    // GPS Actions
    // Returns true if successful (GPS locked), false otherwise
    bool setStartPoint(float temp, float humid);
    bool setEndPoint();
    
    // Getters
    double getDistance(); // Returns distance in meters
    bool hasStartPoint();
    bool hasEndPoint();
    
    // For display purposes
    double getStartLat();
    double getStartLon();
    double getEndLat();
    double getEndLon();
    float getStartTemp();
    float getStartHumid();

private:
    GpsHandler* gps;
    
    // Coordinates
    double startLat;
    double startLon;
    double endLat;
    double endLon;
    
    // Environment at start
    float startTemp;
    float startHumid;
    
    bool startSet;
    bool endSet;
    
    // Helper to get averaged position
    bool getAveragedPosition(double& lat, double& lon, int samples = 10);
};

#endif // TRACK_SHOT_H
