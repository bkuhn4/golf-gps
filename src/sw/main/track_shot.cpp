#include "track_shot.h"

TrackShot::TrackShot() {
    startLat = 0;
    startLon = 0;
    endLat = 0;
    endLon = 0;
    startTemp = 0;
    startHumid = 0;
    startDate = 0;
    startTime = 0;
    startSet = false;
    endSet = false;
    u8g2 = nullptr;
}

bool TrackShot::begin(GpsHandler* gpsObj, U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayObj) {
    gps = gpsObj;
    u8g2 = displayObj;
    return true;
}

bool TrackShot::setStartPoint(float temp, float humid) {
    Serial.println("TrackShot: Attempting to set Start Point...");
    if (getAveragedPosition(startLat, startLon, 50)) {
        startSet = true;
        startTemp = temp;
        startHumid = humid;
        if (gps) {
            startDate = gps->getDate();
            startTime = gps->getTime();
        }
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
    if (getAveragedPosition(endLat, endLon, 50)) {
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
uint32_t TrackShot::getStartDate() { return startDate; }
uint32_t TrackShot::getStartTime() { return startTime; }

void TrackShot::drawAveragingProgress(int percent) {
    // Validate object and clamp the percentage into the displayable range
    if (!u8g2) return;
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    // Prepare the frame buffer and header text
    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_7x14B_tf);
    u8g2->drawStr(0, 14, "Averaging GPS...");

    // Render the progress bar frame and the filled portion based on percent
    // Frame dimensions mirror the reference sketch (104x10)
    u8g2->drawFrame(0, 40, 104, 10);
    int barWidth = (percent * 100) / 100; // Map percentage directly to pixel width (0..100)
    u8g2->drawBox(2, 42, barWidth, 6);

    // Draw the percentage text to the right of the bar
    char buf[8];
    snprintf(buf, sizeof(buf), "%d%%", percent);
    u8g2->drawStr(108, 48, buf);

    // Commit the buffer to the display
    u8g2->sendBuffer();
}

// Updated to use GNSS PVT sampling with 64-bit accumulation and a simple progress indicator.
bool TrackShot::getAveragedPosition(double& lat, double& lon, int samples) {
    // Verify prerequisites: GNSS instance and a valid fix are required for averaging
    if (!gps || !gps->isLocked()) {
        Serial.println("TrackShot: GPS Not Locked or Not Initialized");
        return false;
    }

    Serial.print("TrackShot: Averaging Position...");

    // Use 64-bit accumulators to prevent overflow when summing many 1e-7 scaled coordinates
    int64_t sumLat = 0;
    int64_t sumLon = 0;
    int validSamples = 0;

    // Poll for new PVT data and accumulate latitude/longitude when fresh data is available
    // This loop exits once the requested number of samples has been collected
    while (validSamples < samples) {
        // getPVT() returns true only when a new navigation solution packet has been received
        if (gps->myGNSS.getPVT()) {
            // Coordinates are returned as 32-bit integers scaled by 1e-7 degrees
            int32_t curLat = gps->myGNSS.getLatitude();
            int32_t curLon = gps->myGNSS.getLongitude();

            // Accumulate into 64-bit sums for precision and safety
            sumLat += curLat;
            sumLon += curLon;
            validSamples++;

            // Update the on-screen progress indicator without exposing sample or satellite counts
            int percent = (validSamples * 100) / samples;
            drawAveragingProgress(percent);
        }
        // Short delay reduces I2C contention and yields to other tasks
        delay(10);
    }

    Serial.println(" done");

    // Compute the mean of the accumulated integer coordinates
    if (validSamples > 0) {
        double avgLat = (double)sumLat / (double)validSamples; // still scaled by 1e-7
        double avgLon = (double)sumLon / (double)validSamples; // still scaled by 1e-7

        // Convert back to double degrees for downstream calculations and display
        lat = avgLat / 10000000.0;
        lon = avgLon / 10000000.0;

        Serial.println("TrackShot: Averaging Complete.");
        return true;
    }

    // Defensive path: no valid samples were collected
    Serial.println("TrackShot: Averaging Failed (No valid samples)");
    return false;
}
