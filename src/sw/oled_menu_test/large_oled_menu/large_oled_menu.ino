/*
 * ESP32 SSD1306 128x64 OLED (Using Top Half)
 * FUTURE-PROOFED EDITION
 */

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// --- Debug Macro ---
#define DEBUG 1
#if DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

// --- Pin Definitions ---
#define SDA 10
#define SCL 4
#define BUTTON1 9   // Up
#define BUTTON2 20  // Down
#define BUTTON3 2  // Select / Back

// --- Display Setup ---
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, SCL, SDA);

// --- BITMAPS (Blank as requested) ---
const unsigned char epd_bitmap_battery_icon[] PROGMEM = {};
const unsigned char epd_bitmap_folder_icon[] PROGMEM = {};
const unsigned char epd_bitmap_gps_icon[] PROGMEM = {};
const unsigned char epd_bitmap_location_icon[] PROGMEM = {};
const unsigned char epd_bitmap_placeholder_icon[] PROGMEM = {};
const unsigned char epd_bitmap_settings_icon[] PROGMEM = {};
const unsigned char epd_bitmap_track_shot_icon[] PROGMEM = {};
const unsigned char epd_bitmap_hierarchy_marker[] PROGMEM = {};
const unsigned char epd_bitmap_scroll_bar_knob[] PROGMEM = {};
const unsigned char epd_bitmap_down_arrow[] PROGMEM = {};
const unsigned char epd_bitmap_up_arrow[] PROGMEM = {};
const unsigned char epd_bitmap_scroll_bar[] PROGMEM = {};
const unsigned char epd_bitmap_item_outline[] PROGMEM = {};

// ==============================================================================
//  STEP 1: DEFINE YOUR MENU IDS HERE
//  Order implies the "Index". Keep children of a specific menu grouped together.
// ==============================================================================
enum MenuIndex {
  // Root
  ROOT_MAIN,

  // Level 1
  MAIN_TRACK_SHOT,
  MAIN_SHOT_LOG,
  MAIN_SETTINGS,

  // Level 2: Track Shot
  TS_SELECT_CLUB,
  TS_START,

  // Level 2: Shot Log
  SL_SELECT_CLUB,

  // Level 2: Settings
  SETTINGS_RESET,
  SETTINGS_BATTERY,

  // Level 3: Club Categories (Track Shot)
  TS_CAT_WOODS,
  TS_CAT_HYBRIDS,
  TS_CAT_IRONS,
  TS_CAT_WEDGES,

  // Level 3: Club Categories (Shot Log)
  SL_CAT_WOODS,
  SL_CAT_HYBRIDS,
  SL_CAT_IRONS,
  SL_CAT_WEDGES,

  // Level 4: Specific Clubs (Track Shot)
  // Woods
  TS_WOOD_DRIVER,
  TS_WOOD_3,
  TS_WOOD_5,
  // Hybrids
  TS_HYB_4,
  // Irons
  TS_IRON_5,
  TS_IRON_6,
  TS_IRON_7,
  TS_IRON_8,
  TS_IRON_9,
  // Wedges
  TS_WEDGE_56,
  TS_WEDGE_PW,
  TS_WEDGE_SW,

  // Level 4: Specific Clubs (Shot Log)
  // Woods
  SL_WOOD_DRIVER, // This one will have logs
  SL_WOOD_3,
  SL_WOOD_5,
  // Hybrids
  SL_HYB_4,
  // Irons
  SL_IRON_5, // Placeholder for rest of irons
  // Wedges
  SL_WEDGE_56, // Placeholder for rest of wedges

  // Level 5: Logs (Example for Driver)
  LOG_DR_1,
  LOG_DR_2,
  LOG_DR_3,

  // LAST ITEM (Used for array sizing)
  MENU_COUNT
};

// ==============================================================================
//  STEP 2: MENU STRUCTURE
// ==============================================================================

struct MenuItem {
  const char* title;
  const unsigned char* icon;
  int8_t parent;       // The Enum ID of the parent
  int8_t firstChild;   // The Enum ID of the first child option
  uint8_t numChildren; // Total number of options in this menu
  bool isAction;       // true = execute code, false = open submenu
};

// We use [ENUM_NAME] = { ... } so the order in code doesn't break the links if you shuffle them.
MenuItem menuItems[MENU_COUNT] = {

  // --- ROOT ---
  [ROOT_MAIN] = { "Main Menu", nullptr, -1, MAIN_TRACK_SHOT, 3, false },

  // --- LEVEL 1 ---
  [MAIN_TRACK_SHOT] = { "Track Shot", epd_bitmap_track_shot_icon, ROOT_MAIN, TS_SELECT_CLUB, 2, false },
  [MAIN_SHOT_LOG]   = { "Shot Log",   epd_bitmap_folder_icon,     ROOT_MAIN, SL_SELECT_CLUB, 1, false },
  [MAIN_SETTINGS]   = { "Settings",   epd_bitmap_settings_icon,   ROOT_MAIN, SETTINGS_RESET, 2, false },

  // --- LEVEL 2: TRACK SHOT ---
  [TS_SELECT_CLUB] = { "Select Club", epd_bitmap_placeholder_icon, MAIN_TRACK_SHOT, TS_CAT_WOODS, 4, false },
  [TS_START]       = { "Start",       epd_bitmap_placeholder_icon, MAIN_TRACK_SHOT, 0,            0, true },

  // --- LEVEL 2: SHOT LOG ---
  [SL_SELECT_CLUB] = { "Select Club", epd_bitmap_placeholder_icon, MAIN_SHOT_LOG, SL_CAT_WOODS, 4, false },

  // --- LEVEL 2: SETTINGS ---
  [SETTINGS_RESET]   = { "Reset GPS",    epd_bitmap_gps_icon,     MAIN_SETTINGS, 0, 0, true },
  [SETTINGS_BATTERY] = { "Battery Info", epd_bitmap_battery_icon, MAIN_SETTINGS, 0, 0, true },

  // --- LEVEL 3: TS CATEGORIES ---
  [TS_CAT_WOODS]   = { "Woods",   epd_bitmap_placeholder_icon, TS_SELECT_CLUB, TS_WOOD_DRIVER, 3, false },
  [TS_CAT_HYBRIDS] = { "Hybrids", epd_bitmap_placeholder_icon, TS_SELECT_CLUB, TS_HYB_4,       1, false },
  [TS_CAT_IRONS]   = { "Irons",   epd_bitmap_placeholder_icon, TS_SELECT_CLUB, TS_IRON_5,      5, false },
  [TS_CAT_WEDGES]  = { "Wedges",  epd_bitmap_placeholder_icon, TS_SELECT_CLUB, TS_WEDGE_56,    3, false },

  // --- LEVEL 3: SL CATEGORIES ---
  [SL_CAT_WOODS]   = { "Woods",   epd_bitmap_placeholder_icon, SL_SELECT_CLUB, SL_WOOD_DRIVER, 3, false },
  [SL_CAT_HYBRIDS] = { "Hybrids", epd_bitmap_placeholder_icon, SL_SELECT_CLUB, SL_HYB_4,       1, false },
  [SL_CAT_IRONS]   = { "Irons",   epd_bitmap_placeholder_icon, SL_SELECT_CLUB, SL_IRON_5,      1, false },
  [SL_CAT_WEDGES]  = { "Wedges",  epd_bitmap_placeholder_icon, SL_SELECT_CLUB, SL_WEDGE_56,    1, false },

  // --- LEVEL 4: TS CLUBS (Actions: Set Current Club) ---
  [TS_WOOD_DRIVER] = { "Driver",     epd_bitmap_placeholder_icon, TS_CAT_WOODS, 0, 0, true },
  [TS_WOOD_3]      = { "3 Wood",     epd_bitmap_placeholder_icon, TS_CAT_WOODS, 0, 0, true },
  [TS_WOOD_5]      = { "5 Wood",     epd_bitmap_placeholder_icon, TS_CAT_WOODS, 0, 0, true },
  
  [TS_HYB_4]       = { "4 Hybrid",   epd_bitmap_placeholder_icon, TS_CAT_HYBRIDS, 0, 0, true },

  [TS_IRON_5]      = { "5 Iron",     epd_bitmap_placeholder_icon, TS_CAT_IRONS, 0, 0, true },
  [TS_IRON_6]      = { "6 Iron",     epd_bitmap_placeholder_icon, TS_CAT_IRONS, 0, 0, true },
  [TS_IRON_7]      = { "7 Iron",     epd_bitmap_placeholder_icon, TS_CAT_IRONS, 0, 0, true },
  [TS_IRON_8]      = { "8 Iron",     epd_bitmap_placeholder_icon, TS_CAT_IRONS, 0, 0, true },
  [TS_IRON_9]      = { "9 Iron",     epd_bitmap_placeholder_icon, TS_CAT_IRONS, 0, 0, true },

  [TS_WEDGE_56]    = { "56 Degree",  epd_bitmap_placeholder_icon, TS_CAT_WEDGES, 0, 0, true },
  [TS_WEDGE_PW]    = { "PW",         epd_bitmap_placeholder_icon, TS_CAT_WEDGES, 0, 0, true },
  [TS_WEDGE_SW]    = { "SW",         epd_bitmap_placeholder_icon, TS_CAT_WEDGES, 0, 0, true },

  // --- LEVEL 4: SL CLUBS (Opening Logs) ---
  [SL_WOOD_DRIVER] = { "Driver",     epd_bitmap_placeholder_icon, SL_CAT_WOODS, LOG_DR_1, 3, false },
  [SL_WOOD_3]      = { "3 Wood",     epd_bitmap_placeholder_icon, SL_CAT_WOODS, 0, 0, true },
  [SL_WOOD_5]      = { "5 Wood",     epd_bitmap_placeholder_icon, SL_CAT_WOODS, 0, 0, true },
  
  [SL_HYB_4]       = { "4 Hybrid",   epd_bitmap_placeholder_icon, SL_CAT_HYBRIDS, 0, 0, true },
  [SL_IRON_5]      = { "5 Iron",     epd_bitmap_placeholder_icon, SL_CAT_IRONS, 0, 0, true },
  [SL_WEDGE_56]    = { "56 Deg",     epd_bitmap_placeholder_icon, SL_CAT_WEDGES, 0, 0, true },

  // --- LEVEL 5: LOGS (For Driver) ---
  [LOG_DR_1] = { "265 yds", epd_bitmap_placeholder_icon, SL_WOOD_DRIVER, 0, 0, true },
  [LOG_DR_2] = { "240 yds", epd_bitmap_placeholder_icon, SL_WOOD_DRIVER, 0, 0, true },
  [LOG_DR_3] = { "272 yds", epd_bitmap_placeholder_icon, SL_WOOD_DRIVER, 0, 0, true },

};

// ==============================================================================
//  LOGIC & VARIABLES
// ==============================================================================

struct UIState {
  int currentMenuIndex; // This now holds an Enum Value
  int currentSelection; // 0 to numChildren-1
};
UIState ui = { ROOT_MAIN, 0 };

bool displayNeedsUpdate = true;
unsigned long actionScreenVisibleUntil = 0;
String currentClub = "None"; 

// --- Button State ---
const unsigned long LONG_PRESS_DURATION = 700;
const unsigned long DEBOUNCE_DELAY = 40;

int lastB1State=LOW; int currentB1State=LOW; unsigned long lastB1DebounceTime=0;
int lastB2State=LOW; int currentB2State=LOW; unsigned long lastB2DebounceTime=0;
int lastB3State=LOW; int currentB3State=LOW; unsigned long lastB3DebounceTime=0;
unsigned long b3PressStartTime = 0;
bool b3IsHeld = false;
bool b3LongActionTaken = false;

// Prototypes
void checkButtons();
void drawMenu();
void drawActionScreen(const char* actionText);
void executeAction(int itemIndex);
int getMenuDepth();

void setup(void) {
  Serial.begin(115200);
  Serial.println("\n--- Golf Tracker Setup ---");

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);

  u8g2.begin();
  Serial.println("Display initialized.");
}

void loop(void) {
  checkButtons();

  if (actionScreenVisibleUntil > 0 && millis() >= actionScreenVisibleUntil) {
    displayNeedsUpdate = true;
    actionScreenVisibleUntil = 0;
  }

  if (displayNeedsUpdate) {
    if (actionScreenVisibleUntil == 0) {
      drawMenu();
    }
    displayNeedsUpdate = false;
  }
}

// Button Logic
void checkButtons() {
  unsigned long currentMillis = millis();

  // UP BUTTON
  int reading1 = digitalRead(BUTTON1);
  if (reading1 != lastB1State) lastB1DebounceTime = currentMillis;
  if ((currentMillis - lastB1DebounceTime) > DEBOUNCE_DELAY) {
    if (reading1 != currentB1State) {
      currentB1State = reading1;
      if (currentB1State == HIGH) { 
        ui.currentSelection--;
        if (ui.currentSelection < 0) {
          ui.currentSelection = menuItems[ui.currentMenuIndex].numChildren - 1;
        }
        displayNeedsUpdate = true;
      }
    }
  }
  lastB1State = reading1;

  // DOWN BUTTON
  int reading2 = digitalRead(BUTTON2);
  if (reading2 != lastB2State) lastB2DebounceTime = currentMillis;
  if ((currentMillis - lastB2DebounceTime) > DEBOUNCE_DELAY) {
    if (reading2 != currentB2State) {
      currentB2State = reading2;
      if (currentB2State == HIGH) { 
        ui.currentSelection++;
        if (ui.currentSelection >= menuItems[ui.currentMenuIndex].numChildren) {
          ui.currentSelection = 0;
        }
        displayNeedsUpdate = true;
      }
    }
  }
  lastB2State = reading2;

  // SELECT / BACK BUTTON
  int reading3 = digitalRead(BUTTON3);
  if (reading3 != lastB3State) lastB3DebounceTime = currentMillis;
  if ((currentMillis - lastB3DebounceTime) > DEBOUNCE_DELAY) {
    if (reading3 != currentB3State) {
      currentB3State = reading3;
      if (currentB3State == LOW) { 
        b3PressStartTime = currentMillis;
        b3IsHeld = true;
        b3LongActionTaken = false;
      } else { 
        b3IsHeld = false;
        if (!b3LongActionTaken) {
          // Short Press logic
          int selectedItemIndex = menuItems[ui.currentMenuIndex].firstChild + ui.currentSelection;
          
          if (menuItems[selectedItemIndex].isAction) {
            executeAction(selectedItemIndex);
          } else if (menuItems[selectedItemIndex].numChildren > 0) {
            ui.currentMenuIndex = selectedItemIndex;
            ui.currentSelection = 0;
            displayNeedsUpdate = true;
          }
        }
      }
    }
  }
  lastB3State = reading3;

  // HOLD Logic
  if (b3IsHeld && !b3LongActionTaken) {
    if ((currentMillis - b3PressStartTime) > LONG_PRESS_DURATION) {
      b3LongActionTaken = true;
      // Navigate Back
      if (menuItems[ui.currentMenuIndex].parent != -1) {
        ui.currentMenuIndex = menuItems[ui.currentMenuIndex].parent;
        ui.currentSelection = 0;
        displayNeedsUpdate = true;
      }
    }
  }
}

void executeAction(int itemIndex) {
  MenuItem item = menuItems[itemIndex];
  String title = String(item.title);

  // Handle "Start" Action
  if (itemIndex == TS_START) {
    Serial.print("ACTION: Starting. Club: ");
    Serial.println(currentClub);
    drawActionScreen(("Track: " + currentClub).c_str());
  }
  // Handle Club Selection (Track Shot branch)
  // We can detect this by checking if the Parent is one of the Categories
  else if (menuItems[itemIndex].parent == TS_CAT_WOODS || 
           menuItems[itemIndex].parent == TS_CAT_HYBRIDS ||
           menuItems[itemIndex].parent == TS_CAT_IRONS ||
           menuItems[itemIndex].parent == TS_CAT_WEDGES) {
             
    currentClub = title;
    Serial.print("Selected Club: ");
    Serial.println(currentClub);
    drawActionScreen(("Set: " + title).c_str());
  }
  // Handle Log View (Parent is a specific club in SL tree)
  else if (menuItems[itemIndex].parent == SL_WOOD_DRIVER) {
    Serial.println("Viewing Log");
    drawActionScreen(title.c_str());
  }
  // Default
  else {
    Serial.println(title);
    drawActionScreen(title.c_str());
  }

  actionScreenVisibleUntil = millis() + 1500;
}

int getMenuDepth() {
  int depth = 0;
  int tempIndex = ui.currentMenuIndex;
  while (menuItems[tempIndex].parent != -1) {
    tempIndex = menuItems[tempIndex].parent;
    depth++;
  }
  return depth;
}

void drawMenu() {
  u8g2.clearBuffer();
  u8g2.setBitmapMode(1);

  MenuItem currentMenu = menuItems[ui.currentMenuIndex];
  
  // Calculate index of selected item based on firstChild offset
  int selectedItemIndex = currentMenu.firstChild + ui.currentSelection;
  MenuItem selectedItem = menuItems[selectedItemIndex];

  // Draw Item Outline (Top 32px only)
  u8g2.drawXBM(0, 0, 128, 32, epd_bitmap_item_outline);

  // Draw Hierarchy Markers
  int menuDepth = getMenuDepth();
  for (int i = 0; i < menuDepth; i++) {
    u8g2.drawXBM(i * 8, 0, 8, 8, epd_bitmap_hierarchy_marker);
  }

  // Draw Icon & Text
  u8g2.drawXBM(4, 8, 16, 16, selectedItem.icon);
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.drawStr(23, 20, selectedItem.title);

  // Draw Scrollbar
  if (currentMenu.numChildren > 1) {
    u8g2.drawXBM(120, 0, 8, 32, epd_bitmap_scroll_bar);
    if (ui.currentSelection > 0) {
      u8g2.drawXBM(54, 0, 16, 8, epd_bitmap_up_arrow);
    }
    if (ui.currentSelection < currentMenu.numChildren - 1) {
      u8g2.drawXBM(54, 24, 16, 8, epd_bitmap_down_arrow);
    }
    int knobY = map(ui.currentSelection, 0, currentMenu.numChildren - 1, 0, 24);
    u8g2.drawXBM(120, knobY, 8, 8, epd_bitmap_scroll_bar_knob);
  }

  u8g2.sendBuffer();
}

void drawActionScreen(const char* actionText) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_7x14B_tf);
  u8g2.drawStr(5, 12, "Action:");
  u8g2.drawStr(5, 28, actionText);
  u8g2.sendBuffer();
}