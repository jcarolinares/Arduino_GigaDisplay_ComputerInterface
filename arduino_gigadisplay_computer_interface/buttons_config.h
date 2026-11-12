/*
  Buttons configuration

  Each button has a label (text shown on screen) and an action (what it does
  when pressed). The grid is 4 rows x 4 columns, laid out as it appears on screen.

  Useful calls inside an action:
    Keyboard.key_code('d', KEY_CTRL);              // Shortcut: key + modifiers
    Keyboard.key_code('z', KEY_CTRL | KEY_SHIFT);  // Combine modifiers with |
    Keyboard.key_code(KEY_F11);                    // Special key, no modifiers
    Keyboard.media_control(KEY_VOLUME_UP);         // Media keys
    Keyboard.printf("Hello!");                     // Type a text

  Modifiers: KEY_CTRL, KEY_SHIFT, KEY_ALT, KEY_LOGO (Windows / Super key)
  An action can run several calls in a row, see "MUTE AUDIO".
*/

#include "USBKeyboard.h"

extern USBKeyboard Keyboard; // Defined in the main sketch

struct ButtonConfig {
  const char* label;
  void (*action)();
};

const ButtonConfig buttons[4][4] = {
  // Section 0,0
  {
    {"TALK",       []{ Keyboard.key_code('d', KEY_CTRL); }},
    {"CAMERA",     []{ Keyboard.key_code('e', KEY_CTRL); }},
    {"PLAY/PAUSE", []{ Keyboard.media_control(KEY_PLAY_PAUSE); }},
    {"VOL+",       []{ Keyboard.media_control(KEY_VOLUME_UP); }},
  },
  // Section 0,1
  {
    {"MUTE AUDIO", []{ Keyboard.media_control(KEY_MUTE);
                       Keyboard.key_code('c', KEY_SHIFT); }},
    {"RAISE HAND", []{ Keyboard.key_code('h', KEY_CTRL | KEY_ALT); }},
    {"NEXT SONG",  []{ Keyboard.media_control(KEY_NEXT_TRACK); }},
    {"VOL-",       []{ Keyboard.media_control(KEY_VOLUME_DOWN); }},
  },
  // Section 1,0
  {
    {"SCREENSHOT", []{ Keyboard.key_code(KEY_PRINT_SCREEN); }},
    {"LOCK",       []{ Keyboard.key_code('l', KEY_LOGO | KEY_SHIFT); }},
    {"SLACK",      []{ Keyboard.key_code('6', KEY_LOGO); }},
    {"SPOTIFY",    []{ Keyboard.key_code('4', KEY_LOGO); }},
  },
  // Section 1,1
  {
    {"UNDO",       []{ Keyboard.key_code('z', KEY_CTRL); }},
    {"REDO",       []{ Keyboard.key_code('z', KEY_CTRL | KEY_SHIFT); }},
    {"BROWSER",    []{ Keyboard.key_code('1', KEY_LOGO); }},
    {"VSCODE",     []{ Keyboard.key_code('5', KEY_LOGO); }},
  },
};

/*
  Clap detection

  Clap CLAPS_REQUIRED times in a steady rhythm to run clap_action().
  It is independent from the buttons: change clap_action() to do anything else.

  Tuning: set CLAP_DEBUG to true and open the Serial Plotter. Clap a few times
  and set CLAP_THRESHOLD a bit below the peaks you see.
*/
#define CLAP_ENABLED        true
#define CLAP_THRESHOLD      7000  // Loudness (0-32767) that counts as a clap
#define CLAPS_REQUIRED      3
#define CLAP_DEBUG          false  // Print loudness to the Serial Plotter

void clap_action() {
  Keyboard.key_code('l', KEY_LOGO | KEY_SHIFT); // Lock the computer
}

