/******************************************************************************
 * PROJECT:     Pocket Caddy - Golf GPS
 * FILE:        bitmaps.h
 * AUTHOR:      Brady Kuhn, Bryan York (Based on main.ino header)
 * DATE:        11/21/2025
 * VERSION:     0.1
 * DESCRIPTION: External declarations for all bitmap image data used in the
 *              Pocket Caddy UI.
 ******************************************************************************/

#ifndef BITMAPS_H
#define BITMAPS_H

#include <Arduino.h> // Required for the PROGMEM keyword

// --- Menu Icon Bitmaps (16x16px) ---

extern const unsigned char epd_bitmap_battery_icon[] PROGMEM;
extern const unsigned char epd_bitmap_folder_icon[] PROGMEM;
extern const unsigned char epd_bitmap_gps_icon[] PROGMEM;
extern const unsigned char epd_bitmap_location_icon[] PROGMEM;
extern const unsigned char epd_bitmap_placeholder_icon[] PROGMEM;
extern const unsigned char epd_bitmap_settings_icon[] PROGMEM;
extern const unsigned char epd_bitmap_track_shot_icon[] PROGMEM;
extern const unsigned char epd_bitmap_back_arrow_icon[] PROGMEM;
extern const unsigned char epd_bitmap_golf_ball_icon[] PROGMEM;
extern const unsigned char epd_bitmap_check_icon[] PROGMEM;
extern const unsigned char epd_bitmap_cross_icon[] PROGMEM;
extern const unsigned char epd_bitmap_blank_icon[] PROGMEM;
extern const unsigned char epd_bitmap_user_icon[] PROGMEM;
extern const unsigned char epd_bitmap_plus_icon[] PROGMEM;
extern const unsigned char epd_bitmap_minus_icon[] PROGMEM;

// --- Club Icons ---

extern const unsigned char epd_bitmap_club_icon[] PROGMEM;
extern const unsigned char epd_bitmap_driver_icon[] PROGMEM;
extern const unsigned char epd_bitmap_3_wood_icon[] PROGMEM;
extern const unsigned char epd_bitmap_5_wood_icon[] PROGMEM;
extern const unsigned char epd_bitmap_4_hybrid_icon[] PROGMEM;
extern const unsigned char epd_bitmap_4_iron_icon[] PROGMEM;
extern const unsigned char epd_bitmap_5_iron_icon[] PROGMEM;
extern const unsigned char epd_bitmap_6_iron_icon[] PROGMEM;
extern const unsigned char epd_bitmap_7_iron_icon[] PROGMEM;
extern const unsigned char epd_bitmap_8_iron_icon[] PROGMEM;
extern const unsigned char epd_bitmap_9_iron_icon[] PROGMEM;
extern const unsigned char epd_bitmap_sw_icon[] PROGMEM;
extern const unsigned char epd_bitmap_pw_icon[] PROGMEM;

// --- UI Element Bitmaps ---

// Markers and Knobs (8x8px)
extern const unsigned char epd_bitmap_hierarchy_marker[] PROGMEM;
extern const unsigned char epd_bitmap_scroll_bar_knob[] PROGMEM;

// Arrows (16x8px)
extern const unsigned char epd_bitmap_down_arrow[] PROGMEM;
extern const unsigned char epd_bitmap_up_arrow[] PROGMEM;

// Scroll Bar (8x32px)
extern const unsigned char epd_bitmap_scroll_bar[] PROGMEM;

// Item Outline (128x32px)
extern const unsigned char epd_bitmap_item_outline[] PROGMEM;

#endif // BITMAPS_H