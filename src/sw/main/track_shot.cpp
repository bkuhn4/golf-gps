#include "track_shot.h"

TrackShot::TrackShot() {
    startLat = 0;
    startLon = 0;
    endLat = 0;
    endLon = 0;
    startTemp = 0;
    startHumid = 0;
    startSet = false;
    endSet = false;
}

bool TrackShot::begin(GpsHandler* gpsObj) {
    gps = gpsObj;
    return true;
}

bool TrackShot::setStartPoint(float temp, float humid) {
    Serial.println("TrackShot: Attempting to set Start Point...");
    if (getAveragedPosition(startLat, startLon)) {
        startSet = true;
        startTemp = temp;
        startHumid = humid;
        Serial.print("TrackShot: Start Point Set. Lat: "); Serial.print(startLat, 6);
        Serial.print(" Lon: "); Serial.print(startLon, 6);
        Serial.print(" Temp: "); Serial.print(temp);
        Serial.print(" Humid: "); Serial.println(humid);
        return true;
    }
    Serial.println("TrackShot: Failed to set Start Point (GPS Error)");
    return false;
}

bool TrackShot::setEndPoint() {
    Serial.println("TrackShot: Attempting to set End Point...");
    if (getAveragedPosition(endLat, endLon)) {
        endSet = true;
        Serial.print("TrackShot: End Point Set. Lat: "); Serial.print(endLat, 6);
        Serial.print(" Lon: "); Serial.println(endLon, 6);
        return true;
    }
    Serial.println("TrackShot: Failed to set End Point (GPS Error)");
    return false;
}

double TrackShot::getDistance() {
    if (startSet && endSet) {
        Serial.println("TrackShot: Calculating Distance...");
        double dist = gps->calculateDistanceVincenty(startLat, startLon, endLat, endLon);
        Serial.print("TrackShot: Distance Calculated: "); Serial.println(dist);
        return dist;
    }
    Serial.println("TrackShot: Cannot Calculate Distance (Points not set)");
    return 0.0;
}

bool TrackShot::hasStartPoint() {
    return startSet;
}

bool TrackShot::hasEndPoint() {
    return endSet;
}

double TrackShot::getStartLat() { return startLat; }
double TrackShot::getStartLon() { return startLon; }
double TrackShot::getEndLat() { return endLat; }
double TrackShot::getEndLon() { return endLon; }
float TrackShot::getStartTemp() { return startTemp; }
float TrackShot::getStartHumid() { return startHumid; }

bool TrackShot::getAveragedPosition(double& lat, double& lon, int samples) {
    if (!gps || !gps->isLocked()) {
        Serial.println("TrackShot: GPS Not Locked or Not Initialized");
        return false;
    }

    Serial.print("TrackShot: Averaging Position over "); Serial.print(samples); Serial.println(" samples...");
    double sumLat = 0;
    double sumLon = 0;
    int validSamples = 0;

    for (int i = 0; i < samples; i++) {
        if (gps->isLocked()) {
            sumLat += gps->getLatitude() / 10000000.0;
            sumLon += gps->getLongitude() / 10000000.0;
            validSamples++;
        }
        delay(100); // Small delay between samples
    }

    if (validSamples > 0) {
        lat = sumLat / validSamples;
        lon = sumLon / validSamples;
        Serial.println("TrackShot: Averaging Complete.");
        return true;
    }
    Serial.println("TrackShot: Averaging Failed (No valid samples)");
    return false;
}
