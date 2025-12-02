#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

// CONFIGURATION
// Uncomment the following line to use Mock SD data (for testing without hardware)
// #define USE_MOCK_SD

// Data Structures
struct SystemSettings {
    bool useMetric; // true = meters/celsius, false = yards/fahrenheit
    char currentProfile[32];
};

struct ShotData {
    char club[16];
    float distance;
    float temperature;
    float humidity;
    uint32_t date; // DDMMYY
    uint32_t time; // HHMMSS
};

// Pin Definitions
#define SD_CS_PIN 5
#define SPI_MOSI_PIN 6
#define SPI_MISO_PIN 7
#define SPI_SCK_PIN 8

class SdCardHandler {
public:
    SdCardHandler();
    bool begin();
    int isMounted(); // Returns 1 if mounted, 0 otherwise

    // Data Logging Methods
    void saveSettings(const SystemSettings& settings);
    SystemSettings loadSettings();
    
    void logShot(const char* profileName, const ShotData& shot);
    void createProfile(const char* profileName);
    void removeProfile(const char* profileName);
    void renameProfile(const char* oldName, const char* newName);
    bool profileExists(const char* profileName);
    
    // Returns number of profiles found. Fills profiles array with names.
    int getProfiles(char profiles[][32], int maxProfiles);

    // Returns number of unique clubs found. Fills clubs array.
    int getUsedClubs(const char* profileName, char clubs[][16], int maxClubs);
    
    // Returns number of shots found for club. Fills shots array.
    int getShotsForClub(const char* profileName, const char* club, ShotData* shots, int maxShots);

private:
    SPIClass spi_sd;
    bool mounted;
    
    // Helper to ensure directory exists
    void ensureDirectory(const char* path);
    
    // Helper for sorting clubs
    int getClubRank(const char* club);
};

#endif // SD_CARD_H
