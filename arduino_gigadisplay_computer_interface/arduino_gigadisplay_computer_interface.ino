/*
  Arduino Giga Display Shield Computer HID Controller

  Created by Julián Caro Linares for Arduino INC

  CC-BY-SA
*/


// Additional documentation: https://docs.lvgl.io/master/API/misc/lv_area.html#_CPPv410lv_align_t

#include "buttons_config.h" // Go to this file to customize your buttons

// Giga Display Shield Libraries
#include <Arduino_GigaDisplay.h>
#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

// This sketch needs LVGL 8.3.x. Newer LVGL versions don't build with the GIGA core.
// Install it from the Library Manager and don't accept LVGL update prompts.
#if LVGL_VERSION_MAJOR != 8
  #error "Install LVGL 8.3.11 from the Library Manager (newer versions are not supported)"
#endif

// Bundled font (ui_font_montserrat_32.c in this sketch folder), so it works
// without enabling extra fonts in the core's lv_conf.h
LV_FONT_DECLARE(ui_font_montserrat_32);

// HID Libraries
#include "PluggableUSBHID.h"
#include "USBKeyboard.h"

// Microphone library (clap detection)
#include <PDM.h>

// Objects declaration
GigaDisplayRGB rgb;
Arduino_H7_Video Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch TouchDetector;
USBKeyboard Keyboard;

// Callbacks
// Buttons actions callback function. The pressed button's index (row * 4 + column)
// is stored in the event user data; its action is defined in buttons_config.h
static void button_event_callback(lv_event_t * e) {
  int index = (int)(intptr_t)lv_event_get_user_data(e);
  const ButtonConfig &config = buttons[index / 4][index % 4];

  if (config.action != nullptr) {
    rgb.on(0, 0, 255);
    config.action();
    rgb.off();
  }
}

// Clap detection - settings and clap_action() are in buttons_config.h
#define CLAP_MIN_GAP_MS     150   // Ignore sounds closer than this (echo of the same clap)
#define CLAP_MAX_GAP_MS     800   // Longer pause between claps restarts the count
#define CLAP_COOLDOWN_MS    3000  // Ignore claps for a while after the action runs
#define MIC_WARMUP_MS       2000  // Ignore the microphone right after it starts (startup noise)

short micBuffer[512];               // Audio samples, filled by the microphone interrupt
volatile int micPeak = 0;           // Loudest sample since the last check
volatile bool micDataReady = false;

int clapCount = 0;
unsigned long lastClapTime = 0;
unsigned long actionTime = 0;
bool inCooldown = false;
bool wasLoud = false;               // Last chunk was above the threshold
unsigned long micStartTime = 0;

// Called by the microphone (interrupt): keep it short, just store the loudest sample
void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(micBuffer, bytesAvailable);
  int samples = bytesAvailable / 2; // 16-bit samples

  int peak = micPeak;
  for (int i = 0; i < samples; i++) {
    int level = abs((int)micBuffer[i]);
    if (level > peak) peak = level;
  }
  micPeak = peak;
  micDataReady = true;
}

// Called from loop(): counts claps and runs clap_action() after CLAPS_REQUIRED
void check_claps() {
  if (!micDataReady) return;

  noInterrupts();
  int peak = micPeak;
  micPeak = 0;
  micDataReady = false;
  interrupts();

  unsigned long now = millis();

  if (CLAP_DEBUG) Serial.println(peak);

  // A clap is the moment sound goes from quiet to loud, so a long
  // loud sound (music, noise) counts as one clap at most
  bool loud = peak >= CLAP_THRESHOLD;
  bool newClap = loud && !wasLoud;
  wasLoud = loud;

  // Microphone still settling after start
  if (now - micStartTime < MIC_WARMUP_MS) return;

  // After the action, wait before listening again
  if (inCooldown) {
    if (now - actionTime < CLAP_COOLDOWN_MS) return;
    inCooldown = false;
    rgb.off();
  }

  // Pause too long: start over
  if (clapCount > 0 && now - lastClapTime > CLAP_MAX_GAP_MS) {
    clapCount = 0;
    rgb.off();
  }

  if (!newClap) return;
  if (clapCount > 0 && now - lastClapTime < CLAP_MIN_GAP_MS) return; // Same clap

  clapCount++;
  lastClapTime = now;
  rgb.on(255, 255, 0); // Yellow: clap counted

  if (clapCount >= CLAPS_REQUIRED) {
    rgb.on(255, 0, 0); // Red: action sent
    clap_action();
    clapCount = 0;
    actionTime = now;
    inCooldown = true;
  }
}

void setup() {
  Serial.begin(115200);

  Display.begin();
  TouchDetector.begin();
  rgb.begin();

  // Microphone for clap detection
  if (CLAP_ENABLED) {
    PDM.onReceive(onPDMdata);
    if (!PDM.begin(1, 16000)) { // Mono, 16 kHz
      Serial.println("Failed to start the microphone!");
    }
    micStartTime = millis();
  }

  // Default font for the whole screen (inherited by all labels)
  lv_obj_set_style_text_font(lv_scr_act(), &ui_font_montserrat_32, 0);

  //Graphical Layout Creation

  /* Create a container with grid 4x4 */
  static lv_coord_t col_dsc[] = {377, 377, 377, 377, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {215, 215, 215, 215, LV_GRID_TEMPLATE_LAST};
  lv_obj_t * cont = lv_obj_create(lv_scr_act());
  lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
  lv_obj_set_size(cont, Display.width(), Display.height());
  lv_obj_set_style_bg_color(cont, lv_color_hex(0x63cbcf), LV_PART_MAIN); // 0x03989e // ade4e6
  lv_obj_center(cont);

  lv_obj_t * label;
  lv_obj_t * obj;
  lv_obj_t * button;

  // Buttons Layout Genertion - Go to buttons_config.h to edit them
  for (int i=0; i<4; i++){
    for (int j=0; j<4; j++){
      Serial.print("i,j, tag: ");
      Serial.print(i);
      Serial.print(j);
      Serial.println(buttons[i][j].label);
      button = lv_btn_create(cont);
      lv_obj_set_grid_cell(button, LV_GRID_ALIGN_STRETCH, j, 1, LV_GRID_ALIGN_STRETCH, i, 1);
      // lv_obj_set_size(button, 370, 215);
      lv_obj_center(button);
      label = lv_label_create(button);
      lv_label_set_text(label, buttons[i][j].label);
      lv_obj_center(label);
      lv_obj_add_event_cb(button, button_event_callback, LV_EVENT_CLICKED, (void *)(intptr_t)(i * 4 + j));
    }
  }
}

void loop() {
  lv_timer_handler();
  if (CLAP_ENABLED) check_claps();
}
