#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#include <Arduino.h>
#include "menu_defs.h"
#include "sd_card.h"
#include "gps.h"
#include "temp_humid.h"
#include "battery_fuel_gauge.h"
#include "track_shot.h"
#include "bitmaps.h"

class MenuHandler; // Forward declaration

class AppController {
public:
    AppController();
    void begin(SdCardHandler* sdCardObj, GpsHandler* gpsObj, TempHumidSensor* tempObj, BatteryFuelGauge* batObj, TrackShot* trackShotObj);
    
    // Core Logic
    void executeMenuAction(int itemIndex, MenuHandler* menu);
    void update(MenuHandler* menu);
    
    // Data Providers for Menu
    void refreshChangeProfileMenu(MenuItem* menuItems);
    void refreshRemoveProfileMenu(MenuItem* menuItems);
    void refreshRenameProfileMenu(MenuItem* menuItems);
    void refreshShotLogClubs(MenuItem* menuItems);
    void refreshShotLogShots(MenuItem* menuItems, const char* club);
    void updateTrackShotIcons(MenuItem* menuItems, String currentClub);

    // State Accessors
    int getCurrentSensorView() const { return currentSensorView; }
    void clearSensorView() { currentSensorView = 0; }
    bool getUseMetric() const { return useMetric; }

    // Data Accessors for View
    bool isGpsLocked();
    float getGpsLat();
    float getGpsLon();
    float getTemperature();
    float getHumidity();
    float getBatteryVoltage();
    float getBatteryPercentage();
    int getGpsSIV();
    float getBatteryCurrent();

private:
    SdCardHandler* sdCard;
    GpsHandler* gps;
    TempHumidSensor* tempSensor;
    BatteryFuelGauge* battery;
    TrackShot* trackShot;

    // State
    int currentSensorView;
    bool useMetric;
    const unsigned char* currentClubIcon;
    
    enum AppState {
        STATE_IDLE,
        STATE_WAITING_FOR_PROFILE_NAME,
        STATE_WAITING_FOR_PROFILE_RENAME
    };
    AppState state;
    char profileToRename[32];

    // Helpers
    char profileNames[5][32];
    char clubNames[5][16];
    ShotData shotCache[5];
    char shotTitles[5][32];
};

#endif // APP_CONTROLLER_H
