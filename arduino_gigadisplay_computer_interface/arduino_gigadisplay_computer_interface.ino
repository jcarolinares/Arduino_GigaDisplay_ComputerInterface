/*
  Arduino Giga Display Shield Computer HID Controller

  Created by Julián Caro Linares for Arduino INC

  CC-BY-SA
*/

/*
TASKS LIST
DONE - Improve buttons aligment, small disaligment right side more space than left one
DONE - Experiment with color of the background
TODO - Experiment with colors of the buttons
DONE - Make blink the RGB LED as an additional feedback
DONE - Extend Buttons and capabilities by groups
  DONE - 1-Meetings control 
  DONE - 2-Music Control
  DONE -3-Computer tools/control
*/

// Additional documentation: https://docs.lvgl.io/master/API/misc/lv_area.html#_CPPv410lv_align_t
    // lv_label_set_text_fmt(label, "Clicked!");

#include "buttons_config.h" // Go to this file to customize your buttons

// Giga Display Shield Libraries
#include <Arduino_GigaDisplay.h>
#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

// HID Libraries
#include "PluggableUSBHID.h"
#include "USBKeyboard.h"

// Objects declaration
GigaDisplayRGB rgb;
Arduino_H7_Video Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch TouchDetector;
USBKeyboard Keyboard;

// Callbacks
// Buttons actions callback function
static void button_event_callback(lv_event_t * e) {
  // Get the button object that triggered the event
  lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
  // Get the first child of the button, which is the label in this case
  lv_obj_t * label = lv_obj_get_child(btn, 0);

  String label_read = String(lv_label_get_text(label));
  // Serial.println(label_read);

  // Button pressed actions
  if(label_read=="TALK"){
    Keyboard.key_code('d', KEY_CTRL);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="MUTE AUDIO"){
    Keyboard.media_control(KEY_MUTE);
    Keyboard.key_code('c', KEY_SHIFT);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="CAMERA"){
    Keyboard.key_code('e', KEY_CTRL);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="RAISE HAND"){
    Keyboard.key_code('h', KEY_CTRL | KEY_ALT);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="FULLSCREEN"){
    Keyboard.key_code(KEY_F11);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="PLAY/PAUSE"){
    Keyboard.media_control(KEY_PLAY_PAUSE);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="NEXT SONG"){
    Keyboard.media_control(KEY_NEXT_TRACK);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="VOL+"){
    Keyboard.media_control(KEY_VOLUME_UP);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="VOL-"){
    Keyboard.media_control(KEY_VOLUME_DOWN);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="SCREENSHOT"){
    Keyboard.key_code(KEY_PRINT_SCREEN);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="LOCK"){
    Keyboard.key_code('l',KEY_LOGO);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="UNDO"){
    Keyboard.key_code('z',KEY_CTRL);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="REDO"){
    Keyboard.key_code('z',KEY_CTRL | KEY_SHIFT);
    rgb.on(0, 0, 255);
  }
  else if(label_read=="SLACK"){
    Keyboard.key_code('6',KEY_LOGO); // Ubuntu layout. Depends of app bar
    rgb.on(0, 0, 255);
  }
  else if(label_read=="SPOTIFY"){
    Keyboard.key_code('4',KEY_LOGO); // Ubuntu layout. Depends of app bar
    rgb.on(0, 0, 255);
  }
  else if(label_read=="BROWSER"){
    Keyboard.key_code('1',KEY_LOGO); // Ubuntu layout. Depends of app bar
    rgb.on(0, 0, 255);
  }
  else if(label_read=="VSCODE"){
    Keyboard.key_code('5',KEY_LOGO); // Ubuntu layout. Depends of app bar
    rgb.on(0, 0, 255);
  }
  rgb.off();
}

void setup() {
  Serial.begin(115200);

  Display.begin();
  TouchDetector.begin();
  rgb.begin();

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
      Serial.println(button_labels[i][j]);
      button = lv_btn_create(cont);
      lv_obj_set_grid_cell(button, LV_GRID_ALIGN_STRETCH, j, 1, LV_GRID_ALIGN_STRETCH, i, 1);
      // lv_obj_set_size(button, 370, 215);
      lv_obj_center(button);
      label = lv_label_create(button);
      lv_label_set_text(label, button_labels[i][j]);
      lv_obj_center(label);
      lv_obj_add_event_cb(button, button_event_callback, LV_EVENT_CLICKED, NULL);
    }
  }
}

void loop() { 
  lv_timer_handler();
}
