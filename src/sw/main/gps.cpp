#include "gps.h"

GpsHandler::GpsHandler() {
    // Constructor
}

bool GpsHandler::begin() {
    // Initialize Pins
    pinMode(GPS_EN_N_PIN, OUTPUT);
    pinMode(GPS_RESET_N_PIN, OUTPUT);
    
    // Enable GPS (Active Low Enable, Active Low Reset)
    digitalWrite(GPS_EN_N_PIN, LOW);
    digitalWrite(GPS_RESET_N_PIN, HIGH);

    Serial.print("Initializing GPS... ");
    
    // Initialize I2C if not already done (Main usually does this, but safe to check)
    // Note: Wire.begin() should ideally be called in main setup before this.
    
    if (!myGNSS.begin(Wire)) {
        Serial.println("FAILED. Halting.");
        return false;
    }
    
    Serial.println("OK");
    
    // Configuration
    myGNSS.setI2COutput(COM_TYPE_UBX);
    myGNSS.saveConfiguration();
    return true;
}

long GpsHandler::getLatitude() {
    // Check for fresh data, but always return the latest known value
    myGNSS.getPVT(); 
    return myGNSS.getLatitude();
}

long GpsHandler::getLongitude() {
    myGNSS.getPVT();
    return myGNSS.getLongitude();
}

byte GpsHandler::getSIV() {
    myGNSS.getPVT();
    return myGNSS.getSIV();
}

bool GpsHandler::isLocked() {
    // Simple check: do we have a fix type?
    // 0 = No Fix, 1 = Dead Reckoning, 2 = 2D, 3 = 3D, 4 = GNSS + Dead Reckoning, 5 = Time only
    byte fixType = myGNSS.getFixType();
    return (fixType >= 2); 
}

uint32_t GpsHandler::getTime() {
    // Returns time in HHMMSS format
    if (myGNSS.getPVT()) {
        uint8_t hour = myGNSS.getHour();
        uint8_t minute = myGNSS.getMinute();
        uint8_t second = myGNSS.getSecond();
        return (hour * 10000) + (minute * 100) + second;
    }
    return 0;
}

uint32_t GpsHandler::getDate() {
    // Returns date in DDMMYY format
    if (myGNSS.getPVT()) {
        uint8_t day = myGNSS.getDay();
        uint8_t month = myGNSS.getMonth();
        uint16_t year = myGNSS.getYear(); // Returns full year e.g. 2025
        uint8_t shortYear = year % 100;
        return (day * 10000) + (month * 100) + shortYear;
    }
    return 0;
}

void GpsHandler::reset() {
    Serial.println("Resetting GPS...");
    digitalWrite(GPS_RESET_N_PIN, LOW);
    delay(1000);
    digitalWrite(GPS_RESET_N_PIN, HIGH);
    Serial.println("GPS Reset Complete.");
}

double GpsHandler::calculateDistanceVincenty(double lat1, double lon1, double lat2, double lon2) {
    Serial.print("GPS: Calculating Distance (Vincenty). P1: "); Serial.print(lat1, 6); Serial.print(","); Serial.print(lon1, 6);
    Serial.print(" P2: "); Serial.print(lat2, 6); Serial.print(","); Serial.println(lon2, 6);

    // WGS-84 ellipsoid parameters
    const double a = 6378137.0; // major axis
    const double b = 6356752.314245; // minor axis
    const double f = 1 / 298.257223563; // flattening

    double L = (lon2 - lon1) * DEG_TO_RAD;
    double U1 = atan((1 - f) * tan(lat1 * DEG_TO_RAD));
    double U2 = atan((1 - f) * tan(lat2 * DEG_TO_RAD));
    double sinU1 = sin(U1), cosU1 = cos(U1);
    double sinU2 = sin(U2), cosU2 = cos(U2);

    double lambda = L;
    double lambdaP;
    double iterLimit = 100;
    double sinLambda, cosLambda, sinSigma, cosSigma, sigma, sinAlpha, cosSqAlpha, cos2SigmaM, C;

    do {
        sinLambda = sin(lambda);
        cosLambda = cos(lambda);
        sinSigma = sqrt((cosU2 * sinLambda) * (cosU2 * sinLambda) +
                        (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda) * (cosU1 * sinU2 - sinU1 * cosU2 * cosLambda));
        if (sinSigma == 0) {
            Serial.println("GPS: Points are co-incident. Distance 0.");
            return 0; // co-incident points
        }
        cosSigma = sinU1 * sinU2 + cosU1 * cosU2 * cosLambda;
        sigma = atan2(sinSigma, cosSigma);
        sinAlpha = cosU1 * cosU2 * sinLambda / sinSigma;
        cosSqAlpha = 1 - sinAlpha * sinAlpha;
        cos2SigmaM = cosSigma - 2 * sinU1 * sinU2 / cosSqAlpha;
        if (isnan(cos2SigmaM)) cos2SigmaM = 0; // equatorial line: cosSqAlpha=0
        C = f / 16 * cosSqAlpha * (4 + f * (4 - 3 * cosSqAlpha));
        lambdaP = lambda;
        lambda = L + (1 - C) * f * sinAlpha * (sigma + C * sinSigma * (cos2SigmaM + C * cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM)));
    } while (fabs(lambda - lambdaP) > 1e-12 && --iterLimit > 0);

    if (iterLimit == 0) {
        Serial.println("GPS: Vincenty formula failed to converge.");
        return 0; // formula failed to converge
    }

    double uSq = cosSqAlpha * (a * a - b * b) / (b * b);
    double A = 1 + uSq / 16384 * (4096 + uSq * (-768 + uSq * (320 - 175 * uSq)));
    double B = uSq / 1024 * (256 + uSq * (-128 + uSq * (74 - 47 * uSq)));
    double deltaSigma = B * sinSigma * (cos2SigmaM + B / 4 * (cosSigma * (-1 + 2 * cos2SigmaM * cos2SigmaM) -
                                        B / 6 * cos2SigmaM * (-3 + 4 * sinSigma * sinSigma) * (-3 + 4 * cos2SigmaM * cos2SigmaM) ));
    double s = b * A * (sigma - deltaSigma);

    Serial.print("GPS: Distance Calculated: "); Serial.println(s);
    return s;
}
