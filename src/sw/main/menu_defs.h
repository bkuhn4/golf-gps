#ifndef MENU_DEFS_H
#define MENU_DEFS_H

#include <Arduino.h>

// --- MENU IDS ---
enum MenuIndex {
  // Root
  ROOT_MAIN,

  // Level 1
  MAIN_START_ROUND,
  MAIN_TRACK_SHOT,
  MAIN_SHOT_LOG,
  MAIN_SENSOR_INFO,
  MAIN_SETTINGS,

  // Level 2: Start Round
  START_ROUND_9,
  START_ROUND_18,
  START_ROUND_BACK,

  // Level 2: Track Shot
  TS_GPS_LOCK,
  TS_SELECT_CLUB,
  TS_SET_START,
  TS_SET_END,
  TS_MEASURE,
  TS_LOG_SHOT,
  TS_BACK,

  // Level 2: Shot Log
  SL_SELECT_CLUB,
  SL_BACK,

  // Level 2: Sensor Info
  SENSOR_GPS,
  SENSOR_TEMP_HUMID,
  SENSOR_BATTERY,
  SENSOR_BACK,

  // Level 2: Settings
  SETTINGS_PROFILES,
  SETTINGS_RESET,
  SETTINGS_BATTERY,
  SETTINGS_UNITS,
  SETTINGS_BACK,

  // Level 3: Settings Units
  SETTINGS_UNITS_IMPERIAL,
  SETTINGS_UNITS_METRIC,
  SETTINGS_UNITS_BACK,

  // Level 3: Profiles Menu
  SETTINGS_PROFILES_CHANGE,
  SETTINGS_PROFILES_ADD,
  SETTINGS_PROFILES_REMOVE,
  SETTINGS_PROFILES_RENAME,
  SETTINGS_PROFILES_BACK,

  // Level 4: Change Profile List (Dynamic)
  SETTINGS_CHANGE_SLOT_1,
  SETTINGS_CHANGE_SLOT_2,
  SETTINGS_CHANGE_SLOT_3,
  SETTINGS_CHANGE_SLOT_4,
  SETTINGS_CHANGE_SLOT_5,
  SETTINGS_CHANGE_BACK_DYNAMIC,

  // Level 4: Remove Profile List (Dynamic)
  SETTINGS_REMOVE_SLOT_1,
  SETTINGS_REMOVE_SLOT_2,
  SETTINGS_REMOVE_SLOT_3,
  SETTINGS_REMOVE_SLOT_4,
  SETTINGS_REMOVE_SLOT_5,
  SETTINGS_REMOVE_BACK_DYNAMIC,

  // Level 4: Rename Profile List (Dynamic)
  SETTINGS_RENAME_SLOT_1,
  SETTINGS_RENAME_SLOT_2,
  SETTINGS_RENAME_SLOT_3,
  SETTINGS_RENAME_SLOT_4,
  SETTINGS_RENAME_SLOT_5,
  SETTINGS_RENAME_BACK_DYNAMIC,

  // Level 3: Shot Log Clubs (Dynamic)
  SL_CLUB_SLOT_1,
  SL_CLUB_SLOT_2,
  SL_CLUB_SLOT_3,
  SL_CLUB_SLOT_4,
  SL_CLUB_SLOT_5,
  SL_CLUB_BACK_DYNAMIC,

  // Level 4: Shot Log Shots (Dynamic)
  SL_SHOT_SLOT_1,
  SL_SHOT_SLOT_2,
  SL_SHOT_SLOT_3,
  SL_SHOT_SLOT_4,
  SL_SHOT_SLOT_5,
  SL_SHOT_BACK_DYNAMIC,

  // Level 3: Club Categories (Track Shot)
  TS_CAT_WOODS,
  TS_CAT_HYBRIDS,
  TS_CAT_IRONS,
  TS_CAT_WEDGES,
  TS_CAT_BACK,

  // Level 3: Club Categories (Shot Log)
  SL_CAT_WOODS,
  SL_CAT_HYBRIDS,
  SL_CAT_IRONS,
  SL_CAT_WEDGES,
  SL_CAT_BACK,

  // Level 4: Specific Clubs (Track Shot)
  TS_WOOD_DRIVER,
  TS_WOOD_3,
  TS_WOOD_5,
  TS_WOOD_BACK,

  TS_HYB_4,
  TS_HYB_BACK,

  TS_IRON_5,
  TS_IRON_6,
  TS_IRON_7,
  TS_IRON_8,
  TS_IRON_9,
  TS_IRON_BACK,

  TS_WEDGE_56,
  TS_WEDGE_60,
  TS_WEDGE_PW,
  TS_WEDGE_SW,
  TS_WEDGE_BACK,

  // Level 4: Specific Clubs (Shot Log)
  SL_WOOD_DRIVER,
  SL_WOOD_3,
  SL_WOOD_5,
  SL_WOOD_BACK,

  SL_HYB_4,
  SL_HYB_BACK,

  SL_IRON_5,
  SL_IRON_BACK,

  SL_WEDGE_56,
  SL_WEDGE_BACK,

  // Level 5: Logs
  LOG_DR_1,
  LOG_DR_2,
  LOG_DR_3,
  LOG_BACK,

  MENU_COUNT
};

// --- MENU STRUCTURE ---
struct MenuItem {
  const char* title;
  const unsigned char* icon;
  int8_t parent;       // The Enum ID of the parent
  int8_t firstChild;   // The Enum ID of the first child option
  uint8_t numChildren; // Total number of options in this menu
  bool isAction;       // true = execute code, false = open submenu
};

#endif // MENU_DEFS_H
