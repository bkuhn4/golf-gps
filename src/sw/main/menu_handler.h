#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "menu_defs.h"
#include "button_handler.h"

class AppController; // Forward declaration

// --- UI STATE ---
struct UIState {
  int currentMenuIndex;
  int currentSelection;
};

class MenuHandler {
public:
    MenuHandler();
    void begin(U8G2_SSD1306_128X64_NONAME_F_HW_I2C* displayObj, AppController* appObj);
    void handleInput(ButtonEvent event);
    void update(); // Call this in loop to handle drawing and timers

    // Public for AppController to use
    void drawActionScreen(const char* actionText);
    void drawSensorScreen();
    void drawShotResult(const char* club, const char* dist, const char* env);
    
    MenuItem* getMenuItems(); // Accessor for AppController

    // Public for AppController to use
    void setCurrentClub(String club);
    String getCurrentClub();
    void navigateTo(int menuIndex, int selection);
    void requestUpdate();
    void setActionScreenTimer(unsigned long duration);

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C* u8g2;
    AppController* app;
    
    UIState ui;
    bool displayNeedsUpdate;
    unsigned long actionScreenVisibleUntil;
    String currentClub;
    
    unsigned long lastSensorUpdate;
    
    // Helper methods
    void drawMenu();
    
    // Helpers
    int getMenuDepth();
    void initMenuItems(); // Initialize menu items
    void goBack(); // Helper for back navigation
    
    char tsSelectClubTitle[32]; // Cache for Track Shot club selection title
};

#endif // MENU_HANDLER_H

