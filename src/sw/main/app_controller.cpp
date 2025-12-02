#include "app_controller.h"
#include "menu_handler.h" // Need full definition for drawActionScreen

AppController::AppController() {
    currentSensorView = 0;
    useMetric = false;
    currentClubIcon = nullptr;
    state = STATE_IDLE;
}

void AppController::begin(SdCardHandler* sdCardObj, GpsHandler* gpsObj, TempHumidSensor* tempObj, BatteryFuelGauge* batObj, TrackShot* trackShotObj) {
    sdCard = sdCardObj;
    gps = gpsObj;
    tempSensor = tempObj;
    battery = batObj;
    trackShot = trackShotObj;

    if (sdCard) {
        SystemSettings settings = sdCard->loadSettings();
        useMetric = settings.useMetric;
    }
}

void AppController::refreshChangeProfileMenu(MenuItem* menuItems) {
    if (!sdCard) return;
    Serial.println("Refreshing Change Profile Menu...");

    SystemSettings settings = sdCard->loadSettings();

    for (int i = 0; i < 5; i++) {
        menuItems[SETTINGS_CHANGE_SLOT_1 + i].title = "";
        menuItems[SETTINGS_CHANGE_SLOT_1 + i].isAction = true;
    }

    int count = sdCard->getProfiles(profileNames, 5);
    
    for (int i = 0; i < count; i++) {
        menuItems[SETTINGS_CHANGE_SLOT_1 + i].title = profileNames[i];
        if (strcmp(profileNames[i], settings.currentProfile) == 0) {
            menuItems[SETTINGS_CHANGE_SLOT_1 + i].icon = epd_bitmap_check_icon;
        } else {
            menuItems[SETTINGS_CHANGE_SLOT_1 + i].icon = epd_bitmap_user_icon;
        }
    }

    int backIndex = SETTINGS_CHANGE_SLOT_1 + count;
    menuItems[backIndex].title = "Back";
    menuItems[backIndex].icon = epd_bitmap_back_arrow_icon;
    menuItems[backIndex].isAction = false;
    
    menuItems[SETTINGS_PROFILES_CHANGE].numChildren = count + 1;
}

void AppController::refreshRemoveProfileMenu(MenuItem* menuItems) {
    if (!sdCard) return;
    Serial.println("Refreshing Remove Profile Menu...");

    for (int i = 0; i < 5; i++) {
        menuItems[SETTINGS_REMOVE_SLOT_1 + i].title = "";
        menuItems[SETTINGS_REMOVE_SLOT_1 + i].isAction = true;
    }

    int count = sdCard->getProfiles(profileNames, 5);
    
    for (int i = 0; i < count; i++) {
        menuItems[SETTINGS_REMOVE_SLOT_1 + i].title = profileNames[i];
        menuItems[SETTINGS_REMOVE_SLOT_1 + i].icon = epd_bitmap_user_icon;
    }

    int backIndex = SETTINGS_REMOVE_SLOT_1 + count;
    menuItems[backIndex].title = "Back";
    menuItems[backIndex].icon = epd_bitmap_back_arrow_icon;
    menuItems[backIndex].isAction = false;
    
    menuItems[SETTINGS_PROFILES_REMOVE].numChildren = count + 1;
}

void AppController::refreshRenameProfileMenu(MenuItem* menuItems) {
    if (!sdCard) return;
    Serial.println("Refreshing Rename Profile Menu...");

    for (int i = 0; i < 5; i++) {
        menuItems[SETTINGS_RENAME_SLOT_1 + i].title = "";
        menuItems[SETTINGS_RENAME_SLOT_1 + i].isAction = true;
    }

    int count = sdCard->getProfiles(profileNames, 5);
    
    for (int i = 0; i < count; i++) {
        menuItems[SETTINGS_RENAME_SLOT_1 + i].title = profileNames[i];
        menuItems[SETTINGS_RENAME_SLOT_1 + i].icon = epd_bitmap_user_icon;
    }

    int backIndex = SETTINGS_RENAME_SLOT_1 + count;
    menuItems[backIndex].title = "Back";
    menuItems[backIndex].icon = epd_bitmap_back_arrow_icon;
    menuItems[backIndex].isAction = false;
    
    menuItems[SETTINGS_PROFILES_RENAME].numChildren = count + 1;
}

void AppController::refreshShotLogClubs(MenuItem* menuItems) {
    if (!sdCard) return;
    Serial.println("Refreshing Shot Log Clubs...");

    SystemSettings settings = sdCard->loadSettings();
    
    for (int i = 0; i < 5; i++) {
        menuItems[SL_CLUB_SLOT_1 + i].title = "";
        menuItems[SL_CLUB_SLOT_1 + i].isAction = false;
        menuItems[SL_CLUB_SLOT_1 + i].firstChild = SL_SHOT_SLOT_1;
        menuItems[SL_CLUB_SLOT_1 + i].numChildren = 0;
    }

    int count = sdCard->getUsedClubs(settings.currentProfile, clubNames, 5);

    for (int i = 0; i < count; i++) {
        menuItems[SL_CLUB_SLOT_1 + i].title = clubNames[i];
        menuItems[SL_CLUB_SLOT_1 + i].icon = epd_bitmap_club_icon;
    }

    int backIndex = SL_CLUB_SLOT_1 + count;
    menuItems[backIndex].title = "Back";
    menuItems[backIndex].icon = epd_bitmap_back_arrow_icon;
    menuItems[backIndex].isAction = false;
    menuItems[backIndex].parent = MAIN_SHOT_LOG;

    menuItems[MAIN_SHOT_LOG].numChildren = count + 1;
}

void AppController::refreshShotLogShots(MenuItem* menuItems, const char* club) {
    if (!sdCard) return;
    Serial.print("Refreshing Shot Log Shots for Club: ");
    Serial.println(club);

    SystemSettings settings = sdCard->loadSettings();
    
    for (int i = 0; i < 5; i++) {
        menuItems[SL_SHOT_SLOT_1 + i].title = "";
        menuItems[SL_SHOT_SLOT_1 + i].isAction = true;
    }

    int count = sdCard->getShotsForClub(settings.currentProfile, club, shotCache, 5);

    for (int i = 0; i < count; i++) {
        float dist = shotCache[i].distance;
        int day = (shotCache[i].date / 10000) % 100;
        int month = (shotCache[i].date / 100) % 100;
        
        snprintf(shotTitles[i], 32, "%.0f%s %d/%d", dist, useMetric ? "m" : "y", month, day);
        menuItems[SL_SHOT_SLOT_1 + i].title = shotTitles[i];
        menuItems[SL_SHOT_SLOT_1 + i].icon = epd_bitmap_placeholder_icon;
    }

    int backIndex = SL_SHOT_SLOT_1 + count;
    menuItems[backIndex].title = "Back";
    menuItems[backIndex].icon = epd_bitmap_back_arrow_icon;
    menuItems[backIndex].isAction = false;
}

void AppController::updateTrackShotIcons(MenuItem* menuItems, String currentClub) {
    if (gps && gps->isLocked()) {
        menuItems[TS_GPS_LOCK].icon = epd_bitmap_check_icon;
    } else {
        menuItems[TS_GPS_LOCK].icon = epd_bitmap_cross_icon;
    }

    if (currentClub != "None") {
        menuItems[TS_SELECT_CLUB].icon = epd_bitmap_check_icon;
    } else {
        menuItems[TS_SELECT_CLUB].icon = epd_bitmap_cross_icon;
    }

    if (trackShot && trackShot->hasStartPoint()) {
        menuItems[TS_SET_START].icon = epd_bitmap_check_icon;
    } else {
        menuItems[TS_SET_START].icon = epd_bitmap_location_icon;
    }

    if (trackShot && trackShot->hasEndPoint()) {
        menuItems[TS_SET_END].icon = epd_bitmap_check_icon;
    } else {
        menuItems[TS_SET_END].icon = epd_bitmap_golf_ball_icon;
    }

    if (trackShot && trackShot->hasStartPoint() && trackShot->hasEndPoint() && gps && gps->isLocked()) {
        menuItems[TS_MEASURE].icon = epd_bitmap_check_icon;
        menuItems[TS_LOG_SHOT].icon = epd_bitmap_folder_icon;
    } else {
        menuItems[TS_MEASURE].icon = epd_bitmap_cross_icon;
        menuItems[TS_LOG_SHOT].icon = epd_bitmap_cross_icon;
    }
}

void AppController::executeMenuAction(int itemIndex, MenuHandler* menu) {
    // Handle Sensor Views
    if (itemIndex == SENSOR_GPS || itemIndex == SENSOR_TEMP_HUMID || itemIndex == SENSOR_BATTERY) {
        Serial.println("Entering Sensor View");
        currentSensorView = itemIndex;
        menu->drawSensorScreen(); 
        return;
    }

    // Handle GPS Lock Check
    if (itemIndex == TS_GPS_LOCK) {
        if (gps && gps->isLocked()) {
            menu->drawActionScreen("GPS Locked");
        } else {
            menu->drawActionScreen("No GPS Lock");
        }
        menu->setActionScreenTimer(2000);
    }
    // Handle Set Start
    else if (itemIndex == TS_SET_START) {
        if (trackShot) {
            menu->drawActionScreen("Averaging GPS...");
            float t = 0, h = 0;
            if (tempSensor) {
                t = tempSensor->getTemperature();
                h = tempSensor->getHumidity();
            }
            if (trackShot->setStartPoint(t, h)) {
                char coords[32];
                snprintf(coords, 32, "Start: %.5f, %.5f", trackShot->getStartLat(), trackShot->getStartLon());
                menu->drawActionScreen(coords);
                menu->setActionScreenTimer(2000);
                updateTrackShotIcons(menu->getMenuItems(), menu->getCurrentClub());
            } else {
                menu->drawActionScreen("GPS Lock Failed");
                menu->setActionScreenTimer(2000);
            }
        }
    }
    // Handle Set End
    else if (itemIndex == TS_SET_END) {
        if (trackShot) {
            menu->drawActionScreen("Averaging GPS...");
            if (trackShot->setEndPoint()) {
                char coords[32];
                snprintf(coords, 32, "End: %.5f, %.5f", trackShot->getEndLat(), trackShot->getEndLon());
                menu->drawActionScreen(coords);
                menu->setActionScreenTimer(2000);
                updateTrackShotIcons(menu->getMenuItems(), menu->getCurrentClub());
            } else {
                menu->drawActionScreen("GPS Lock Failed");
                menu->setActionScreenTimer(2000);
            }
        }
    }
    // Handle Measure
    else if (itemIndex == TS_MEASURE) {
        if (trackShot && trackShot->hasStartPoint() && trackShot->hasEndPoint() && gps && gps->isLocked()) {
            double dist = trackShot->getDistance();
            float t = trackShot->getStartTemp();
            float h = trackShot->getStartHumid();
            
            char distStr[32];
            char envStr[32];
            
            if (!useMetric) {
                dist = dist * 1.09361; // Meters to Yards
                t = (t * 9.0 / 5.0) + 32.0;
                snprintf(distStr, 32, "Dist: %.0f yds", dist);
                snprintf(envStr, 32, "%.0fF  %.0f%%", t, h);
            } else {
                snprintf(distStr, 32, "Dist: %.0f m", dist);
                snprintf(envStr, 32, "%.0fC  %.0f%%", t, h);
            }
            
            menu->drawShotResult("Club", distStr, envStr); // TODO: Pass actual club name
            menu->setActionScreenTimer(5000);
        } else {
            Serial.println("Cannot Measure: Missing Points or GPS Lock");
            menu->drawActionScreen("Measure Failed");
            menu->setActionScreenTimer(2000);
        }
    }
    // Handle Log Shot
    else if (itemIndex == TS_LOG_SHOT) {
        if (trackShot && trackShot->hasStartPoint() && trackShot->hasEndPoint() && gps && gps->isLocked()) {
             if (sdCard) {
                 SystemSettings settings = sdCard->loadSettings();
                 ShotData shot;
                 strncpy(shot.club, menu->getCurrentClub().c_str(), 15);
                 shot.club[15] = '\0';
                 shot.distance = trackShot->getDistance();
                 shot.temperature = trackShot->getStartTemp();
                 shot.humidity = trackShot->getStartHumid();
                 shot.date = gps->getDate();
                 shot.time = gps->getTime();
                 
                 sdCard->logShot(settings.currentProfile, shot);
                 menu->drawActionScreen("Shot Logged!");
                 menu->setActionScreenTimer(2000);
             }
        } else {
             menu->drawActionScreen("Cannot Log");
             menu->setActionScreenTimer(2000);
        }
    }

    // Extended Logic moved from MenuHandler
    MenuItem* menuItems = menu->getMenuItems();
    
    // Handle Club Selection (Track Shot branch)
    if (menuItems[itemIndex].parent == TS_CAT_WOODS || 
             menuItems[itemIndex].parent == TS_CAT_HYBRIDS ||
             menuItems[itemIndex].parent == TS_CAT_IRONS ||
             menuItems[itemIndex].parent == TS_CAT_WEDGES) {
             
        String title = String(menuItems[itemIndex].title);
        menu->setCurrentClub(title);
        currentClubIcon = menuItems[itemIndex].icon; // Save the icon
        
        Serial.print("Selected Club: ");
        Serial.println(title);

        updateTrackShotIcons(menuItems, title);

        // Return to Track Shot menu
        menu->navigateTo(MAIN_TRACK_SHOT, 2); // Highlight Set Start
        return;
    }
    // Handle Log View
    else if (menuItems[itemIndex].parent == SL_WOOD_DRIVER) {
        Serial.println("Viewing Log");
        menu->drawActionScreen(menuItems[itemIndex].title);
        menu->setActionScreenTimer(3000);
    }
    // Handle GPS Reset
    else if (itemIndex == SETTINGS_RESET) {
        Serial.println("Resetting GPS Module...");
        menu->drawActionScreen("Resetting GPS...");
        if (gps) {
            gps->reset();
        }
        menu->setActionScreenTimer(2000);
    }
    // Handle Units
    else if (itemIndex == SETTINGS_UNITS_IMPERIAL) {
        Serial.println("Setting Units to Imperial");
        if (sdCard) {
            SystemSettings settings = sdCard->loadSettings();
            settings.useMetric = false;
            sdCard->saveSettings(settings);
            useMetric = false; // Update cache
        }
        menu->drawActionScreen("Set to Imperial");
        menu->setActionScreenTimer(2000);
        // Navigate back to Settings after action screen
        menu->navigateTo(MAIN_SETTINGS, 0);
    }
    else if (itemIndex == SETTINGS_UNITS_METRIC) {
        Serial.println("Setting Units to Metric");
        if (sdCard) {
            SystemSettings settings = sdCard->loadSettings();
            settings.useMetric = true;
            sdCard->saveSettings(settings);
            useMetric = true; // Update cache
        }
        menu->drawActionScreen("Set to Metric");
        menu->setActionScreenTimer(2000);
        // Navigate back to Settings after action screen
        menu->navigateTo(MAIN_SETTINGS, 0);
    }
    // Handle Profiles Change Menu (Dynamic Load)
    else if (itemIndex == SETTINGS_PROFILES_CHANGE) {
        refreshChangeProfileMenu(menuItems);
    }
    // Handle Profiles Remove Menu (Dynamic Load)
    else if (itemIndex == SETTINGS_PROFILES_REMOVE) {
        refreshRemoveProfileMenu(menuItems);
    }
    // Handle Profiles Rename Menu (Dynamic Load)
    else if (itemIndex == SETTINGS_PROFILES_RENAME) {
        refreshRenameProfileMenu(menuItems);
    }
    // Handle Add Profile
    else if (itemIndex == SETTINGS_PROFILES_ADD) {
        Serial.println("Adding New Profile...");
        if (sdCard) {
            state = STATE_WAITING_FOR_PROFILE_NAME;
            menu->drawActionScreen("Enter Name in Serial...");
            menu->setActionScreenTimer(30000); // Give 30 seconds
        }
    }
    // Handle Change Profile Selection
    else if (itemIndex >= SETTINGS_CHANGE_SLOT_1 && itemIndex <= SETTINGS_CHANGE_SLOT_5) {
        String profileName = String(menuItems[itemIndex].title);
        if (profileName != "Back" && profileName != "") {
            Serial.print("Selecting Profile: "); Serial.println(profileName);
            if (sdCard) {
                SystemSettings settings = sdCard->loadSettings();
                profileName.toCharArray(settings.currentProfile, 32);
                sdCard->saveSettings(settings);
                refreshChangeProfileMenu(menuItems); // Update checkmarks
            }
            // No action screen, just update view
            menu->requestUpdate();
        }
    }
    // Handle Remove Profile Selection
    else if (itemIndex >= SETTINGS_REMOVE_SLOT_1 && itemIndex <= SETTINGS_REMOVE_SLOT_5) {
        String profileName = String(menuItems[itemIndex].title);
        if (profileName != "Back" && profileName != "") {
             if (sdCard) {
                 sdCard->removeProfile(profileName.c_str());
                 refreshRemoveProfileMenu(menuItems);
                 menu->drawActionScreen("Removed");
                 menu->setActionScreenTimer(1000);
                 menu->navigateTo(SETTINGS_PROFILES, 2); // Return to Profiles menu, highlight Remove
             }
        }
    }
    // Handle Rename Profile Selection
    else if (itemIndex >= SETTINGS_RENAME_SLOT_1 && itemIndex <= SETTINGS_RENAME_SLOT_5) {
        String profileName = String(menuItems[itemIndex].title);
        if (profileName != "Back" && profileName != "") {
             strncpy(profileToRename, profileName.c_str(), 31);
             profileToRename[31] = '\0';
             state = STATE_WAITING_FOR_PROFILE_RENAME;
             menu->drawActionScreen("Enter new name...");
             menu->setActionScreenTimer(30000);
        }
    }
    // Handle Shot Detail View
    else if (itemIndex >= SL_SHOT_SLOT_1 && itemIndex <= SL_SHOT_SLOT_5) {
        Serial.println("Viewing Shot Details");
        // Show full details
        int shotIndex = itemIndex - SL_SHOT_SLOT_1;
        ShotData shot = shotCache[shotIndex];
        
        char details[64];
        snprintf(details, 64, "T:%.0f H:%.0f", shot.temperature, shot.humidity);
        menu->drawActionScreen(details);
        menu->setActionScreenTimer(3000);
    }
}

bool AppController::isGpsLocked() {
    return gps && gps->isLocked();
}

float AppController::getGpsLat() {
    return gps ? gps->getLatitude() : 0.0;
}

float AppController::getGpsLon() {
    return gps ? gps->getLongitude() : 0.0;
}

float AppController::getTemperature() {
    return tempSensor ? tempSensor->getTemperature() : 0.0;
}

float AppController::getHumidity() {
    return tempSensor ? tempSensor->getHumidity() : 0.0;
}

float AppController::getBatteryVoltage() {
    return battery ? battery->getVoltage() : 0.0;
}

float AppController::getBatteryPercentage() {
    return battery ? battery->getSOC() : 0.0;
}

int AppController::getGpsSIV() {
    return gps ? gps->getSIV() : 0;
}

float AppController::getBatteryCurrent() {
    return battery ? battery->getAverageCurrent() : 0.0;
}

void AppController::update(MenuHandler* menu) {
    if (state == STATE_WAITING_FOR_PROFILE_NAME) {
        if (Serial.available() > 0) {
            String name = Serial.readStringUntil('\n');
            name.trim();
            if (name.length() > 0) {
                if (sdCard) {
                    sdCard->createProfile(name.c_str());
                    refreshChangeProfileMenu(menu->getMenuItems());
                    
                    // Find the new profile index
                    int newIndex = 0;
                    MenuItem* items = menu->getMenuItems();
                    for(int i=0; i<5; i++) {
                        if (String(items[SETTINGS_CHANGE_SLOT_1 + i].title) == name) {
                            // Found it.
                            newIndex = i;
                            break;
                        }
                    }
                    
                    state = STATE_IDLE;
                    menu->drawActionScreen(("Created: " + name).c_str());
                    menu->setActionScreenTimer(2000);
                    
                    // Navigate to Change Profile and select the new user
                    menu->navigateTo(SETTINGS_PROFILES_CHANGE, newIndex);
                }
            }
        }
    }
    else if (state == STATE_WAITING_FOR_PROFILE_RENAME) {
        if (Serial.available() > 0) {
            String newName = Serial.readStringUntil('\n');
            newName.trim();
            if (newName.length() > 0) {
                if (sdCard) {
                    sdCard->renameProfile(profileToRename, newName.c_str());
                    
                    // If we renamed the current profile, update settings
                    SystemSettings settings = sdCard->loadSettings();
                    if (strcmp(settings.currentProfile, profileToRename) == 0) {
                        strncpy(settings.currentProfile, newName.c_str(), 31);
                        sdCard->saveSettings(settings);
                    }

                    state = STATE_IDLE;
                    menu->drawActionScreen(("Renamed to: " + newName).c_str());
                    menu->setActionScreenTimer(2000);
                    
                    // Navigate back to Rename list
                    refreshRenameProfileMenu(menu->getMenuItems());
                    menu->navigateTo(SETTINGS_PROFILES_RENAME, 0);
                }
            }
        }
    }
}
