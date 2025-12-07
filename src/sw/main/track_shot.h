#ifndef TRACK_SHOT_H
#define TRACK_SHOT_H

#include <Arduino.h>
#include "gps.h"
#include <U8g2lib.h>

class TrackShot {
public:
    TrackShot();
    bool begin(GpsHandler* gpsObj, U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayObj);
    
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
    uint32_t getStartDate();
    uint32_t getStartTime();

private:
    GpsHandler* gps;
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C* u8g2; // for progress bar
    
    // Coordinates
    double startLat;
    double startLon;
    double endLat;
    double endLon;
    
    // Environment at start
    float startTemp;
    float startHumid;
    uint32_t startDate;
    uint32_t startTime;
    
    bool startSet;
    bool endSet;
    
    // Helper to get averaged position
    bool getAveragedPosition(double& lat, double& lon, int samples = 10);
    void drawAveragingProgress(int percent);
};

#endif // TRACK_SHOT_H
