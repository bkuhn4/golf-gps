#include "menu_handler.h"
#include "app_controller.h"

// Define the menu items array here (Global to this file)
// Removed const to allow dynamic updates
MenuItem menuItems[MENU_COUNT];

void MenuHandler::initMenuItems() {
  // --- ROOT ---
  menuItems[ROOT_MAIN] = { "Main Menu", nullptr, -1, MAIN_START_ROUND, 5, false };

  // --- LEVEL 1 ---
  menuItems[MAIN_START_ROUND] = { "Start Round", epd_bitmap_golf_ball_icon, ROOT_MAIN, START_ROUND_9, 3, false };
  menuItems[MAIN_TRACK_SHOT] = { "Track Shot", epd_bitmap_track_shot_icon, ROOT_MAIN, TS_GPS_LOCK, 7, false };
  menuItems[MAIN_SHOT_LOG]   = { "Shot Log",   epd_bitmap_folder_icon,     ROOT_MAIN, SL_CLUB_SLOT_1, 2, false }; // Default 2: Slot1 + Back
  menuItems[MAIN_SENSOR_INFO]= { "Sensor Info",epd_bitmap_placeholder_icon,ROOT_MAIN, SENSOR_GPS, 4, false };
  menuItems[MAIN_SETTINGS]   = { "Settings",   epd_bitmap_settings_icon,   ROOT_MAIN, SETTINGS_PROFILES, 5, false };

  // --- LEVEL 2: START ROUND ---
  menuItems[START_ROUND_9]    = { "9 Holes",     epd_bitmap_golf_ball_icon,   MAIN_START_ROUND, 0, 0, true };
  menuItems[START_ROUND_18]   = { "18 Holes",    epd_bitmap_golf_ball_icon,   MAIN_START_ROUND, 0, 0, true };
  menuItems[START_ROUND_BACK] = { "Back",        epd_bitmap_back_arrow_icon, MAIN_START_ROUND, 0, 0, false };

  // --- LEVEL 2: TRACK SHOT ---
  menuItems[TS_GPS_LOCK]    = { "GPS Lock",    epd_bitmap_cross_icon,       MAIN_TRACK_SHOT, 0,            0, true };
  menuItems[TS_SELECT_CLUB] = { "Select Club", epd_bitmap_cross_icon,       MAIN_TRACK_SHOT, TS_CAT_WOODS, 5, false };
  menuItems[TS_SET_START]   = { "Set Start",   epd_bitmap_placeholder_icon, MAIN_TRACK_SHOT, 0,            0, true };
  menuItems[TS_SET_END]     = { "Set End",     epd_bitmap_placeholder_icon, MAIN_TRACK_SHOT, 0,            0, true };
  menuItems[TS_MEASURE]     = { "Measure",     epd_bitmap_cross_icon,       MAIN_TRACK_SHOT, 0,            0, true };
  menuItems[TS_LOG_SHOT]    = { "Log Shot",    epd_bitmap_cross_icon,       MAIN_TRACK_SHOT, 0,            0, true };
  menuItems[TS_BACK]        = { "Cancel",      epd_bitmap_back_arrow_icon, MAIN_TRACK_SHOT, 0,            0, false };

  // --- LEVEL 2: SHOT LOG (Dynamic) ---
  menuItems[SL_CLUB_SLOT_1]       = { "", nullptr, MAIN_SHOT_LOG, 0, 0, false };
  menuItems[SL_CLUB_SLOT_2]       = { "", nullptr, MAIN_SHOT_LOG, 0, 0, false };
  menuItems[SL_CLUB_SLOT_3]       = { "", nullptr, MAIN_SHOT_LOG, 0, 0, false };
  menuItems[SL_CLUB_SLOT_4]       = { "", nullptr, MAIN_SHOT_LOG, 0, 0, false };
  menuItems[SL_CLUB_SLOT_5]       = { "", nullptr, MAIN_SHOT_LOG, 0, 0, false };
  menuItems[SL_CLUB_BACK_DYNAMIC] = { "Back", epd_bitmap_back_arrow_icon, MAIN_SHOT_LOG, 0, 0, false };

  // --- LEVEL 3: SHOT LOG SHOTS (Dynamic) ---
  menuItems[SL_SHOT_SLOT_1]       = { "", nullptr, SL_CLUB_SLOT_1, 0, 0, true }; // Parent will be dynamic
  menuItems[SL_SHOT_SLOT_2]       = { "", nullptr, SL_CLUB_SLOT_1, 0, 0, true };
  menuItems[SL_SHOT_SLOT_3]       = { "", nullptr, SL_CLUB_SLOT_1, 0, 0, true };
  menuItems[SL_SHOT_SLOT_4]       = { "", nullptr, SL_CLUB_SLOT_1, 0, 0, true };
  menuItems[SL_SHOT_SLOT_5]       = { "", nullptr, SL_CLUB_SLOT_1, 0, 0, true };
  menuItems[SL_SHOT_BACK_DYNAMIC] = { "Back", epd_bitmap_back_arrow_icon, SL_CLUB_SLOT_1, 0, 0, false };

  // --- LEVEL 2: SENSOR INFO ---
  menuItems[SENSOR_GPS]        = { "GPS",              epd_bitmap_gps_icon,         MAIN_SENSOR_INFO, 0, 0, true };
  menuItems[SENSOR_TEMP_HUMID] = { "Temp. & Hum.",     epd_bitmap_placeholder_icon, MAIN_SENSOR_INFO, 0, 0, true };
  menuItems[SENSOR_BATTERY]    = { "Battery",          epd_bitmap_battery_icon,     MAIN_SENSOR_INFO, 0, 0, true };
  menuItems[SENSOR_BACK]       = { "Back",             epd_bitmap_back_arrow_icon, MAIN_SENSOR_INFO, 0, 0, false };

  // --- LEVEL 2: SETTINGS ---
  menuItems[SETTINGS_PROFILES] = { "Profiles", epd_bitmap_user_icon, MAIN_SETTINGS, SETTINGS_PROFILES_CHANGE, 4, false };
  menuItems[SETTINGS_RESET]   = { "Reset GPS",    epd_bitmap_gps_icon,     MAIN_SETTINGS, 0, 0, true };
  menuItems[SETTINGS_BATTERY] = { "Battery Info", epd_bitmap_battery_icon, MAIN_SETTINGS, 0, 0, true };
  menuItems[SETTINGS_UNITS]   = { "Change Units", epd_bitmap_placeholder_icon, MAIN_SETTINGS, SETTINGS_UNITS_IMPERIAL, 3, false };
  menuItems[SETTINGS_BACK]    = { "Back",         epd_bitmap_back_arrow_icon, MAIN_SETTINGS, 0, 0, false };

  // --- LEVEL 3: SETTINGS UNITS ---
  menuItems[SETTINGS_UNITS_IMPERIAL] = { "Imperial", epd_bitmap_placeholder_icon, SETTINGS_UNITS, 0, 0, true };
  menuItems[SETTINGS_UNITS_METRIC]   = { "Metric",   epd_bitmap_placeholder_icon, SETTINGS_UNITS, 0, 0, true };
  menuItems[SETTINGS_UNITS_BACK]     = { "Back",     epd_bitmap_back_arrow_icon, SETTINGS_UNITS, 0, 0, false };

  // --- LEVEL 3: PROFILES MENU ---
  menuItems[SETTINGS_PROFILES_CHANGE] = { "Change Profile", epd_bitmap_user_icon, SETTINGS_PROFILES, SETTINGS_CHANGE_SLOT_1, 1, false };
  menuItems[SETTINGS_PROFILES_ADD]    = { "Add",            epd_bitmap_plus_icon, SETTINGS_PROFILES, 0, 0, true };
  menuItems[SETTINGS_PROFILES_REMOVE] = { "Remove",         epd_bitmap_minus_icon, SETTINGS_PROFILES, SETTINGS_REMOVE_SLOT_1, 1, false };
  menuItems[SETTINGS_PROFILES_RENAME] = { "Rename",         epd_bitmap_user_icon, SETTINGS_PROFILES, SETTINGS_RENAME_SLOT_1, 1, false };
  menuItems[SETTINGS_PROFILES_BACK]   = { "Back",           epd_bitmap_back_arrow_icon, SETTINGS_PROFILES, 0, 0, false };

  // --- LEVEL 4: CHANGE PROFILE LIST (Dynamic) ---
  menuItems[SETTINGS_CHANGE_SLOT_1]       = { "", nullptr, SETTINGS_PROFILES_CHANGE, 0, 0, true };
  menuItems[SETTINGS_CHANGE_SLOT_2]       = { "", nullptr, SETTINGS_PROFILES_CHANGE, 0, 0, true };
  menuItems[SETTINGS_CHANGE_SLOT_3]       = { "", nullptr, SETTINGS_PROFILES_CHANGE, 0, 0, true };
  menuItems[SETTINGS_CHANGE_SLOT_4]       = { "", nullptr, SETTINGS_PROFILES_CHANGE, 0, 0, true };
  menuItems[SETTINGS_CHANGE_SLOT_5]       = { "", nullptr, SETTINGS_PROFILES_CHANGE, 0, 0, true };
  menuItems[SETTINGS_CHANGE_BACK_DYNAMIC] = { "Back", epd_bitmap_back_arrow_icon, SETTINGS_PROFILES_CHANGE, 0, 0, false };

  // --- LEVEL 4: REMOVE PROFILE LIST (Dynamic) ---
  menuItems[SETTINGS_REMOVE_SLOT_1]       = { "", nullptr, SETTINGS_PROFILES_REMOVE, 0, 0, true };
  menuItems[SETTINGS_REMOVE_SLOT_2]       = { "", nullptr, SETTINGS_PROFILES_REMOVE, 0, 0, true };
  menuItems[SETTINGS_REMOVE_SLOT_3]       = { "", nullptr, SETTINGS_PROFILES_REMOVE, 0, 0, true };
  menuItems[SETTINGS_REMOVE_SLOT_4]       = { "", nullptr, SETTINGS_PROFILES_REMOVE, 0, 0, true };
  menuItems[SETTINGS_REMOVE_SLOT_5]       = { "", nullptr, SETTINGS_PROFILES_REMOVE, 0, 0, true };
  menuItems[SETTINGS_REMOVE_BACK_DYNAMIC] = { "Back", epd_bitmap_back_arrow_icon, SETTINGS_PROFILES_REMOVE, 0, 0, false };

  // --- LEVEL 4: RENAME PROFILE LIST (Dynamic) ---
  menuItems[SETTINGS_RENAME_SLOT_1]       = { "", nullptr, SETTINGS_PROFILES_RENAME, 0, 0, true };
  menuItems[SETTINGS_RENAME_SLOT_2]       = { "", nullptr, SETTINGS_PROFILES_RENAME, 0, 0, true };
  menuItems[SETTINGS_RENAME_SLOT_3]       = { "", nullptr, SETTINGS_PROFILES_RENAME, 0, 0, true };
  menuItems[SETTINGS_RENAME_SLOT_4]       = { "", nullptr, SETTINGS_PROFILES_RENAME, 0, 0, true };
  menuItems[SETTINGS_RENAME_SLOT_5]       = { "", nullptr, SETTINGS_PROFILES_RENAME, 0, 0, true };
  menuItems[SETTINGS_RENAME_BACK_DYNAMIC] = { "Back", epd_bitmap_back_arrow_icon, SETTINGS_PROFILES_RENAME, 0, 0, false };

  // --- LEVEL 3: TS CATEGORIES ---
  menuItems[TS_CAT_WOODS]   = { "Woods",   epd_bitmap_club_icon, TS_SELECT_CLUB, TS_WOOD_DRIVER, 4, false };
  menuItems[TS_CAT_HYBRIDS] = { "Hybrids", epd_bitmap_club_icon, TS_SELECT_CLUB, TS_HYB_4,       2, false };
  menuItems[TS_CAT_IRONS]   = { "Irons",   epd_bitmap_club_icon, TS_SELECT_CLUB, TS_IRON_5,      6, false };
  menuItems[TS_CAT_WEDGES]  = { "Wedges",  epd_bitmap_club_icon, TS_SELECT_CLUB, TS_WEDGE_56,    5, false };
  menuItems[TS_CAT_BACK]    = { "Back",    epd_bitmap_back_arrow_icon, TS_SELECT_CLUB, 0,              0, false };

  // --- LEVEL 3: SL CATEGORIES ---
  menuItems[SL_CAT_WOODS]   = { "Woods",   epd_bitmap_club_icon, SL_SELECT_CLUB, SL_WOOD_DRIVER, 4, false };
  menuItems[SL_CAT_HYBRIDS] = { "Hybrids", epd_bitmap_club_icon, SL_SELECT_CLUB, SL_HYB_4,       2, false };
  menuItems[SL_CAT_IRONS]   = { "Irons",   epd_bitmap_club_icon, SL_SELECT_CLUB, SL_IRON_5,      2, false };
  menuItems[SL_CAT_WEDGES]  = { "Wedges",  epd_bitmap_club_icon, SL_SELECT_CLUB, SL_WEDGE_56,    2, false };
  menuItems[SL_CAT_BACK]    = { "Back",    epd_bitmap_back_arrow_icon, SL_SELECT_CLUB, 0,              0, false };

  // --- LEVEL 4: TS CLUBS ---
  menuItems[TS_WOOD_DRIVER] = { "Driver",     epd_bitmap_driver_icon, TS_CAT_WOODS, 0, 0, true };
  menuItems[TS_WOOD_3]      = { "3 Wood",     epd_bitmap_3_wood_icon, TS_CAT_WOODS, 0, 0, true };
  menuItems[TS_WOOD_5]      = { "5 Wood",     epd_bitmap_5_wood_icon, TS_CAT_WOODS, 0, 0, true };
  menuItems[TS_WOOD_BACK]   = { "Back",       epd_bitmap_back_arrow_icon, TS_CAT_WOODS, 0, 0, false };

  menuItems[TS_HYB_4]       = { "4 Hybrid",   epd_bitmap_4_hybrid_icon, TS_CAT_HYBRIDS, 0, 0, true };
  menuItems[TS_HYB_BACK]    = { "Back",       epd_bitmap_back_arrow_icon, TS_CAT_HYBRIDS, 0, 0, false };

  menuItems[TS_IRON_5]      = { "5 Iron",     epd_bitmap_5_iron_icon, TS_CAT_IRONS, 0, 0, true };
  menuItems[TS_IRON_6]      = { "6 Iron",     epd_bitmap_6_iron_icon, TS_CAT_IRONS, 0, 0, true };
  menuItems[TS_IRON_7]      = { "7 Iron",     epd_bitmap_7_iron_icon, TS_CAT_IRONS, 0, 0, true };
  menuItems[TS_IRON_8]      = { "8 Iron",     epd_bitmap_8_iron_icon, TS_CAT_IRONS, 0, 0, true };
  menuItems[TS_IRON_9]      = { "9 Iron",     epd_bitmap_9_iron_icon, TS_CAT_IRONS, 0, 0, true };
  menuItems[TS_IRON_BACK]   = { "Back",       epd_bitmap_back_arrow_icon, TS_CAT_IRONS, 0, 0, false };

  menuItems[TS_WEDGE_56]    = { "56 Degree",  epd_bitmap_sw_icon, TS_CAT_WEDGES, 0, 0, true };
  menuItems[TS_WEDGE_60]    = { "60 Degree",  epd_bitmap_sw_icon, TS_CAT_WEDGES, 0, 0, true };
  menuItems[TS_WEDGE_PW]    = { "PW",         epd_bitmap_pw_icon, TS_CAT_WEDGES, 0, 0, true };
  menuItems[TS_WEDGE_SW]    = { "SW",         epd_bitmap_sw_icon, TS_CAT_WEDGES, 0, 0, true };
  menuItems[TS_WEDGE_BACK]  = { "Back",       epd_bitmap_back_arrow_icon, TS_CAT_WEDGES, 0, 0, false };

  // --- LEVEL 4: SL CLUBS ---
  menuItems[SL_WOOD_DRIVER] = { "Driver",     epd_bitmap_driver_icon, SL_CAT_WOODS, LOG_DR_1, 4, false };
  menuItems[SL_WOOD_3]      = { "3 Wood",     epd_bitmap_3_wood_icon, SL_CAT_WOODS, 0, 0, true };
  menuItems[SL_WOOD_5]      = { "5 Wood",     epd_bitmap_5_wood_icon, SL_CAT_WOODS, 0, 0, true };
  menuItems[SL_WOOD_BACK]   = { "Back",       epd_bitmap_back_arrow_icon, SL_CAT_WOODS, 0, 0, false };

  menuItems[SL_HYB_4]       = { "4 Hybrid",   epd_bitmap_4_hybrid_icon, SL_CAT_HYBRIDS, 0, 0, true };
  menuItems[SL_HYB_BACK]    = { "Back",       epd_bitmap_back_arrow_icon, SL_CAT_HYBRIDS, 0, 0, false };

  menuItems[SL_IRON_5]      = { "5 Iron",     epd_bitmap_5_iron_icon, SL_CAT_IRONS, 0, 0, true };
  menuItems[SL_IRON_BACK]   = { "Back",       epd_bitmap_back_arrow_icon, SL_CAT_IRONS, 0, 0, false };

  menuItems[SL_WEDGE_56]    = { "56 Deg",     epd_bitmap_sw_icon, SL_CAT_WEDGES, 0, 0, true };
  menuItems[SL_WEDGE_BACK]  = { "Back",       epd_bitmap_back_arrow_icon, SL_CAT_WEDGES, 0, 0, false };

  // --- LEVEL 5: LOGS ---
  menuItems[LOG_DR_1] = { "265 yds", epd_bitmap_placeholder_icon, SL_WOOD_DRIVER, 0, 0, true };
  menuItems[LOG_DR_2] = { "240 yds", epd_bitmap_placeholder_icon, SL_WOOD_DRIVER, 0, 0, true };
  menuItems[LOG_DR_3] = { "272 yds", epd_bitmap_placeholder_icon, SL_WOOD_DRIVER, 0, 0, true };
  menuItems[LOG_BACK] = { "Back",    epd_bitmap_back_arrow_icon, SL_WOOD_DRIVER, 0, 0, false };
}

MenuHandler::MenuHandler() {
    ui.currentMenuIndex = ROOT_MAIN;
    ui.currentSelection = 0;
    displayNeedsUpdate = true;
    actionScreenVisibleUntil = 0;
    currentClub = "None";
    lastSensorUpdate = 0;
    app = nullptr;
}

void MenuHandler::begin(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayObj, AppController* appObj) {
    u8g2 = displayObj;
    app = appObj;
    
    initMenuItems(); // Initialize menu items

    // Load initial settings via AppController
    if (app) {
        app->refreshChangeProfileMenu(menuItems); // Load profiles on startup
    }
}

MenuItem* MenuHandler::getMenuItems() {
    return menuItems;
}

// Dynamic menu refresh logic moved to AppController

void MenuHandler::goBack() {
    int parentIndex = menuItems[ui.currentMenuIndex].parent;
    if (parentIndex != -1) {
        Serial.print("Going Back. Parent Index: ");
        Serial.println(parentIndex);
        int firstChildIndex = menuItems[parentIndex].firstChild;
        // Calculate previous selection based on the current menu index
        // The current menu index IS the item index in the parent's list that was selected
        int previousSelection = ui.currentMenuIndex - firstChildIndex;
        
        // Safety check
        if (previousSelection < 0 || previousSelection >= menuItems[parentIndex].numChildren) {
            previousSelection = 0;
        }

        ui.currentMenuIndex = parentIndex;
        ui.currentSelection = previousSelection;
        displayNeedsUpdate = true;
    } else {
        Serial.println("Cannot Go Back: At Root");
    }
}

void MenuHandler::handleInput(ButtonEvent event) {
    if (event != EVENT_NONE) {
        Serial.print("MenuHandler::handleInput Event: ");
        Serial.println(event);
    }

    if (actionScreenVisibleUntil > 0) {
        Serial.println("Action screen active, ignoring input.");
        return;
    }

    // If in Sensor View, only allow BACK
    if (app && app->getCurrentSensorView() != 0) {
        if (event == EVENT_BACK) {
            Serial.println("Exiting Sensor View.");
            app->clearSensorView();
            displayNeedsUpdate = true;
        }
        return;
    }

    switch (event) {
        case EVENT_UP:
            ui.currentSelection--;
            if (ui.currentSelection < 0) {
                ui.currentSelection = menuItems[ui.currentMenuIndex].numChildren - 1;
            }
            displayNeedsUpdate = true;
            break;

        case EVENT_DOWN:
            ui.currentSelection++;
            if (ui.currentSelection >= menuItems[ui.currentMenuIndex].numChildren) {
                ui.currentSelection = 0;
            }
            displayNeedsUpdate = true;
            break;

        case EVENT_SELECT:
            {
                int selectedItemIndex = menuItems[ui.currentMenuIndex].firstChild + ui.currentSelection;
                
                // Check GPS Lock on Track Shot Entry
                if (selectedItemIndex == MAIN_TRACK_SHOT && app) {
                    app->updateTrackShotIcons(menuItems, currentClub);
                }
                
                // Check for SD Card on Shot Log (handled in AppController logic if needed, but here for UI feedback)
                // We can move this check to AppController if we want strict separation, but simple checks are fine here.
                if (selectedItemIndex == MAIN_SHOT_LOG && app) {
                     app->refreshShotLogClubs(menuItems);
                }
                
                // Refresh Profiles on Entry
                if (selectedItemIndex == SETTINGS_PROFILES_CHANGE && app) {
                    app->refreshChangeProfileMenu(menuItems);
                }
                if (selectedItemIndex == SETTINGS_PROFILES_REMOVE && app) {
                    app->refreshRemoveProfileMenu(menuItems);
                }
                if (selectedItemIndex == SETTINGS_PROFILES_RENAME && app) {
                    app->refreshRenameProfileMenu(menuItems);
                }

                // Handle Back Navigation
                String title = String(menuItems[selectedItemIndex].title);
                if (title == "Back" || title == "Cancel") {
                    goBack();
                    return;
                }

                if (menuItems[selectedItemIndex].isAction) {
                    if (app) app->executeMenuAction(selectedItemIndex, this);
                } 
                // Special handling for Shot Log Clubs (Dynamic loading)
                else if (selectedItemIndex >= SL_CLUB_SLOT_1 && selectedItemIndex <= SL_CLUB_SLOT_5) {
                    if (app) {
                        app->refreshShotLogShots(menuItems, menuItems[selectedItemIndex].title);
                        
                        // Update parent pointers of the shot slots
                        for(int i=0; i<6; i++) {
                                menuItems[SL_SHOT_SLOT_1 + i].parent = selectedItemIndex;
                        }
                        
                        // Calculate children count (Shots + Back)
                        int childCount = 0;
                        for(int i=0; i<6; i++) {
                            if (String(menuItems[SL_SHOT_SLOT_1 + i].title) == "Back") {
                                childCount = i + 1;
                                break;
                            }
                        }
                        menuItems[selectedItemIndex].numChildren = childCount;

                        // Enter menu
                        ui.currentMenuIndex = selectedItemIndex;
                        ui.currentSelection = 0;
                        displayNeedsUpdate = true;
                    }
                }
                else if (menuItems[selectedItemIndex].numChildren > 0) {
                    ui.currentMenuIndex = selectedItemIndex;
                    ui.currentSelection = 0;
                    displayNeedsUpdate = true;
                }
            }
            break;

        case EVENT_BACK:
            goBack();
            break;
            
        case EVENT_NONE:
            break;
    }
}

void MenuHandler::update() {
    // Check if action screen timer expired
    if (actionScreenVisibleUntil > 0 && millis() >= actionScreenVisibleUntil) {
        Serial.println("Action screen timer expired.");
        displayNeedsUpdate = true;
        actionScreenVisibleUntil = 0;
    }

    // Handle Sensor View Updates (1Hz)
    if (app && app->getCurrentSensorView() != 0) {
        if (millis() - lastSensorUpdate >= 1000) {
            Serial.println("Updating Sensor Screen...");
            drawSensorScreen();
            lastSensorUpdate = millis();
        }
        return; // Skip normal menu drawing
    }

    // Redraw if needed
    if (displayNeedsUpdate) {
        if (actionScreenVisibleUntil == 0) {
            Serial.println("Drawing Menu...");
            drawMenu();
            Serial.println("Menu Drawn.");
        }
        displayNeedsUpdate = false;
    }
}



int MenuHandler::getMenuDepth() {
    int depth = 0;
    int tempIndex = ui.currentMenuIndex;
    while (menuItems[tempIndex].parent != -1) {
        tempIndex = menuItems[tempIndex].parent;
        depth++;
    }
    return depth;
}

void MenuHandler::drawMenu() {
    if (!u8g2) return;

    u8g2->clearBuffer();
    u8g2->setBitmapMode(1);

    MenuItem currentMenu = menuItems[ui.currentMenuIndex];
    
    // Calculate View Window
    // We show 2 items. 
    // If selection is at the end, we show [End-1, End]
    // Otherwise we show [Selection, Selection+1]
    int topItemIndex = ui.currentSelection;
    int cursorY = 0;

    if (currentMenu.numChildren > 1 && ui.currentSelection == currentMenu.numChildren - 1) {
        topItemIndex = ui.currentSelection - 1;
        cursorY = 31;
    }

    // Draw Cursor (Item Outline)
    u8g2->drawXBM(0, cursorY, 128, 32, epd_bitmap_item_outline);

    // Draw Hierarchy Markers (always at top)
    int menuDepth = getMenuDepth();
    for (int i = 0; i < menuDepth; i++) {
        u8g2->drawXBM(i * 8, 0, 8, 8, epd_bitmap_hierarchy_marker);
    }

    // Draw Visible Items
    for (int i = 0; i < 2; i++) {
        int itemIndex = topItemIndex + i;
        if (itemIndex >= currentMenu.numChildren) break;

        int yPos = i * 31;
        
        // Get the actual item from the global array
        int globalItemIndex = currentMenu.firstChild + itemIndex;
        MenuItem item = menuItems[globalItemIndex];

        // Draw Icon
        u8g2->drawXBM(4, yPos + 8, 16, 16, item.icon);
        
        // Draw Text
        if (itemIndex == ui.currentSelection) {
            u8g2->setFont(u8g2_font_7x14B_tf);
        } else {
            u8g2->setFont(u8g2_font_7x14_tf);
        }
        u8g2->drawStr(23, yPos + 20, item.title);
    }

    // Draw Scrollbar
    if (currentMenu.numChildren > 1) {
        u8g2->drawXBM(120, 0, 8, 64, epd_bitmap_scroll_bar);
        
        // Knob
        int knobY = map(ui.currentSelection, 0, currentMenu.numChildren - 1, 0, 56);
        u8g2->drawXBM(120, knobY, 8, 8, epd_bitmap_scroll_bar_knob);
    }

    u8g2->sendBuffer();
}

void MenuHandler::drawActionScreen(const char* actionText) {
    if (!u8g2) return;

    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_7x14B_tf);
    u8g2->drawStr(5, 12, "Action:");
    u8g2->drawStr(5, 28, actionText);
    u8g2->sendBuffer();
}

void MenuHandler::drawSensorScreen() {
    if (!u8g2 || !app) return;
    Serial.println("Drawing Sensor Screen...");

    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_6x10_tf); // Smaller font for data

    int currentSensorView = app->getCurrentSensorView();

    if (currentSensorView == SENSOR_GPS) {
        u8g2->drawStr(0, 8, "GPS Info:");
        if (app->isGpsLocked()) {
            u8g2->setCursor(0, 18);
            u8g2->print("Lat: "); u8g2->print(app->getGpsLat());
            u8g2->setCursor(0, 28);
            u8g2->print("Lon: "); u8g2->print(app->getGpsLon());
            u8g2->setCursor(80, 28);
            u8g2->print("SIV:"); u8g2->print(app->getGpsSIV());
        } else {
            u8g2->drawStr(0, 20, "No Lock...");
            u8g2->setCursor(0, 30);
            u8g2->print("SIV: "); u8g2->print(app->getGpsSIV());
        }
    } 
    else if (currentSensorView == SENSOR_TEMP_HUMID) {
        u8g2->drawStr(0, 8, "Environment:");
        float temp = app->getTemperature();
        char unit = 'C';
        
        if (!app->getUseMetric()) {
            temp = (temp * 9.0 / 5.0) + 32.0;
            unit = 'F';
        }

        u8g2->setCursor(0, 20);
        u8g2->print("Temp: "); u8g2->print(temp, 1); u8g2->print(" "); u8g2->print(unit);
        u8g2->setCursor(0, 30);
        u8g2->print("Hum:  "); u8g2->print(app->getHumidity(), 1); u8g2->print(" %");
    }
    else if (currentSensorView == SENSOR_BATTERY) {
        u8g2->drawStr(0, 8, "Battery:");
        u8g2->setCursor(0, 20);
        u8g2->print("SOC: "); u8g2->print(app->getBatteryPercentage(), 0); u8g2->print("%");
        u8g2->setCursor(60, 20);
        u8g2->print(app->getBatteryVoltage(), 0); u8g2->print("mV");
        u8g2->setCursor(0, 30);
        u8g2->print("Cur: "); u8g2->print(app->getBatteryCurrent(), 0); u8g2->print("mA");
    }

    u8g2->sendBuffer();
    Serial.println("Sensor Screen Drawn.");
}

void MenuHandler::drawShotResult(const char* club, const char* dist, const char* env) {
    if (!u8g2) return;

    u8g2->clearBuffer();
    u8g2->setFont(u8g2_font_7x14B_tf);
    u8g2->drawStr(0, 12, club);
    
    u8g2->setFont(u8g2_font_7x14_tf);
    u8g2->drawStr(0, 30, dist);
    u8g2->drawStr(0, 48, env);
    
    u8g2->sendBuffer();
}

void MenuHandler::setCurrentClub(String club) {
    currentClub = club;
    snprintf(tsSelectClubTitle, 32, "Club: %s", currentClub.c_str());
    menuItems[TS_SELECT_CLUB].title = tsSelectClubTitle;
}

String MenuHandler::getCurrentClub() {
    return currentClub;
}

void MenuHandler::navigateTo(int menuIndex, int selection) {
    ui.currentMenuIndex = menuIndex;
    ui.currentSelection = selection;
    displayNeedsUpdate = true;
}

void MenuHandler::requestUpdate() {
    displayNeedsUpdate = true;
}

void MenuHandler::setActionScreenTimer(unsigned long duration) {
    actionScreenVisibleUntil = millis() + duration;
}

