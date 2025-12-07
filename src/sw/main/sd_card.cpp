#include "sd_card.h"

SdCardHandler::SdCardHandler() : spi_sd(FSPI) {
    mounted = false;
}

bool SdCardHandler::begin() {
    Serial.print("Initializing SD Card... ");
    
    // Initialize SPI bus for SD Card
    spi_sd.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SD_CS_PIN);
    
    // Initialize SD Card
    if (!SD.begin(SD_CS_PIN, spi_sd)) {
        Serial.println("Mount FAILED");
        mounted = false;
    } else {
        Serial.println("Mount OK");
        mounted = true;
    }
    return mounted;
}

int SdCardHandler::isMounted() {
    return mounted ? 1 : 0;
}

void SdCardHandler::ensureDirectory(const char* path) {
    if (mounted && !SD.exists(path)) {
        SD.mkdir(path);
    }
}

void SdCardHandler::saveSettings(const SystemSettings& settings) {
    if (!mounted) {
        Serial.println("SD: Cannot save settings (Not Mounted)");
        return;
    }

    Serial.println("SD: Saving Settings...");
    // Remove old settings file to overwrite
    if (SD.exists("/settings.txt")) {
        SD.remove("/settings.txt");
    }

    File file = SD.open("/settings.txt", FILE_WRITE);
    if (file) {
        file.print("METRIC=");
        file.println(settings.useMetric ? "1" : "0");
        file.print("PROFILE=");
        file.println(settings.currentProfile);
        file.close();
        Serial.println("SD: Settings saved successfully.");
    } else {
        Serial.println("SD: Error saving settings.");
    }
}

SystemSettings SdCardHandler::loadSettings() {
    SystemSettings settings;
    // Defaults
    settings.useMetric = false; // Default to Yards
    strcpy(settings.currentProfile, "Default");

    if (!mounted) {
        Serial.println("SD: Cannot load settings (Not Mounted)");
        return settings;
    }

    Serial.println("SD: Loading Settings...");
    // Auto-create settings file if missing
    if (!SD.exists("/settings.txt")) {
        Serial.println("SD: Settings file not found, creating defaults.");
        saveSettings(settings);
        return settings;
    }

    File file = SD.open("/settings.txt", FILE_READ);
    if (file) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            line.trim();
            int separatorIndex = line.indexOf('=');
            if (separatorIndex != -1) {
                String key = line.substring(0, separatorIndex);
                String value = line.substring(separatorIndex + 1);

                if (key == "METRIC") {
                    settings.useMetric = (value == "1");
                } else if (key == "PROFILE") {
                    value.toCharArray(settings.currentProfile, sizeof(settings.currentProfile));
                }
            }
        }
        file.close();
        Serial.println("SD: Settings loaded.");
    } else {
        Serial.println("SD: Settings file not found, using defaults.");
    }
    return settings;
}

void SdCardHandler::createProfile(const char* profileName) {
    if (!mounted) return;
    
    ensureDirectory("/profiles");
    
    char path[64];
    snprintf(path, sizeof(path), "/profiles/%s.csv", profileName);
    
    if (!SD.exists(path)) {
        File file = SD.open(path, FILE_WRITE);
        if (file) {
            file.println("Club,Distance,Temp,Humidity,Date,Time"); // Header
            file.close();
            Serial.print("Created profile: ");
            Serial.println(profileName);
        }
    }
}

void SdCardHandler::removeProfile(const char* profileName) {
    if (!mounted) return;
    char path[64];
    snprintf(path, sizeof(path), "/profiles/%s.csv", profileName);
    if (SD.exists(path)) {
        SD.remove(path);
        Serial.print("Removed profile: ");
        Serial.println(profileName);
    }
}

void SdCardHandler::renameProfile(const char* oldName, const char* newName) {
    if (!mounted) return;
    char oldPath[64];
    char newPath[64];
    snprintf(oldPath, sizeof(oldPath), "/profiles/%s.csv", oldName);
    snprintf(newPath, sizeof(newPath), "/profiles/%s.csv", newName);
    
    if (SD.exists(oldPath) && !SD.exists(newPath)) {
        SD.rename(oldPath, newPath);
        Serial.print("Renamed profile: ");
        Serial.print(oldName);
        Serial.print(" to ");
        Serial.println(newName);
    }
}

bool SdCardHandler::profileExists(const char* profileName) {
    if (!mounted) return false;
    char path[64];
    snprintf(path, sizeof(path), "/profiles/%s.csv", profileName);
    return SD.exists(path);
}

int SdCardHandler::getProfiles(char profiles[][32], int maxProfiles) {
    if (!mounted) return 0;
    
    int count = 0;
    // Ensure directory exists
    if (!SD.exists("/profiles")) return 0;

    File dir = SD.open("/profiles");
    if (!dir || !dir.isDirectory()) return 0;

    while (count < maxProfiles) {
        File entry = dir.openNextFile();
        if (!entry) break;
        
        if (!entry.isDirectory()) {
            String name = entry.name();
            // Simple check for CSV
            if (name.indexOf(".csv") != -1 || name.indexOf(".CSV") != -1) {
                // Strip extension
                int dotIndex = name.lastIndexOf('.');
                if (dotIndex != -1) name = name.substring(0, dotIndex);
                
                // Remove leading slash if present (some SD libs do this)
                int slashIndex = name.lastIndexOf('/');
                if (slashIndex != -1) name = name.substring(slashIndex + 1);

                // Copy to buffer
                strncpy(profiles[count], name.c_str(), 31);
                profiles[count][31] = '\0';
                count++;
            }
        }
        entry.close();
    }
    dir.close();
    return count;
}

int SdCardHandler::getClubRank(const char* club) {
    if (strstr(club, "Driver")) return 1;
    if (strstr(club, "3 Wood")) return 2;
    if (strstr(club, "5 Wood")) return 3;
    if (strstr(club, "Hybrid")) return 4;
    if (strstr(club, "3 Iron")) return 5;
    if (strstr(club, "4 Iron")) return 6;
    if (strstr(club, "5 Iron")) return 7;
    if (strstr(club, "6 Iron")) return 8;
    if (strstr(club, "7 Iron")) return 9;
    if (strstr(club, "8 Iron")) return 10;
    if (strstr(club, "9 Iron")) return 11;
    if (strstr(club, "PW")) return 12;
    if (strstr(club, "GW")) return 13;
    if (strstr(club, "SW")) return 14;
    if (strstr(club, "LW")) return 15;
    if (strstr(club, "Degree")) return 16;
    return 99;
}

int SdCardHandler::getUsedClubs(const char* profileName, char clubs[][16], int maxClubs) {
    if (!mounted) return 0;
    
    char path[64];
    snprintf(path, sizeof(path), "/profiles/%s.csv", profileName);
    
    if (!SD.exists(path)) return 0;

    File file = SD.open(path, FILE_READ);
    if (!file) return 0;

    // Skip Header
    file.readStringUntil('\n');

    int count = 0;
    while (file.available() && count < maxClubs) {
        String line = file.readStringUntil('\n');
        int commaIndex = line.indexOf(',');
        if (commaIndex != -1) {
            String clubName = line.substring(0, commaIndex);
            clubName.trim();
            
            // Check if already exists
            bool exists = false;
            for (int i = 0; i < count; i++) {
                if (strcmp(clubs[i], clubName.c_str()) == 0) {
                    exists = true;
                    break;
                }
            }
            
            if (!exists) {
                strncpy(clubs[count], clubName.c_str(), 15);
                clubs[count][15] = '\0';
                count++;
            }
        }
    }
    file.close();

    // Sort Clubs
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (getClubRank(clubs[j]) > getClubRank(clubs[j + 1])) {
                char temp[16];
                strcpy(temp, clubs[j]);
                strcpy(clubs[j], clubs[j + 1]);
                strcpy(clubs[j + 1], temp);
            }
        }
    }

    return count;
}

int SdCardHandler::getShotsForClub(const char* profileName, const char* club, ShotData* shots, int maxShots) {
    if (!mounted) return 0;
    
    char path[64];
    snprintf(path, sizeof(path), "/profiles/%s.csv", profileName);
    
    if (!SD.exists(path)) return 0;

    File file = SD.open(path, FILE_READ);
    if (!file) return 0;

    // Skip Header
    file.readStringUntil('\n');

    int count = 0;
    while (file.available() && count < maxShots) {
        String line = file.readStringUntil('\n');
        // Parse CSV: Club,Distance,Temp,Humidity,Date,Time
        // Simple parsing
        int idx1 = line.indexOf(',');
        int idx2 = line.indexOf(',', idx1 + 1);
        int idx3 = line.indexOf(',', idx2 + 1);
        int idx4 = line.indexOf(',', idx3 + 1);
        int idx5 = line.indexOf(',', idx4 + 1);
        
        if (idx5 != -1) {
            String cName = line.substring(0, idx1);
            cName.trim(); // Ensure no whitespace issues
            
            if (cName.equals(club)) {
                strncpy(shots[count].club, cName.c_str(), 15);
                shots[count].distance = line.substring(idx1 + 1, idx2).toFloat();
                shots[count].temperature = line.substring(idx2 + 1, idx3).toFloat();
                shots[count].humidity = line.substring(idx3 + 1, idx4).toFloat();
                shots[count].date = line.substring(idx4 + 1, idx5).toInt();
                shots[count].time = line.substring(idx5 + 1).toInt();
                count++;
            }
        }
    }
    file.close();

    // Sort by Date/Time Descending (Bubble Sort for simplicity on small datasets)
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // Compare Date
            bool swap = false;
            if (shots[j].date < shots[j + 1].date) {
                swap = true;
            } else if (shots[j].date == shots[j + 1].date) {
                if (shots[j].time < shots[j + 1].time) {
                    swap = true;
                }
            }
            
            if (swap) {
                ShotData temp = shots[j];
                shots[j] = shots[j + 1];
                shots[j + 1] = temp;
            }
        }
    }

    return count;
}

void SdCardHandler::logShot(const char* profileName, const ShotData& shot) {
    if (!mounted) return;
    
    // Ensure profiles directory exists
    ensureDirectory("/profiles");

    char path[64];
    snprintf(path, sizeof(path), "/profiles/%s.csv", profileName);
    
    // Create profile if it doesn't exist (ensures header on first creation)
    if (!SD.exists(path)) {
        createProfile(profileName);
    }

    // Open the file in append mode to avoid truncation
    // FILE_APPEND is critical here to add to the end of the file
    File file = SD.open(path, FILE_APPEND);
    if (file) {
        // If the file is empty (size == 0), write the header to restore missing headers
        if (file.size() == 0) {
            file.println("Club,Distance,Temp,Humidity,Date,Time");
        }

        // Write CSV line: Club,Distance,Temp,Humidity,Date,Time
        file.print(shot.club);
        file.print(",");
        file.print(shot.distance);
        file.print(",");
        file.print(shot.temperature);
        file.print(",");
        file.print(shot.humidity);
        file.print(",");
        file.print(shot.date);
        file.print(",");
        file.println(shot.time);
        
        file.close();
        Serial.println("Shot logged.");
    } else {
        Serial.print("Error logging shot: Could not open ");
        Serial.print(path);
        Serial.println(" for appending.");
    }
}


