#include "config.h"
#ifdef USE_ACTOR_LEDMATRIX
#include "common.h"
#include "actor_ledmatrix.h"

LED_Matrix matrix(LEDMATRIX_DIN, LEDMATRIX_CLK, LEDMATRIX_CS, LEDMATRIX_DEVICES_X, LEDMATRIX_DEVICES_Y);

void Actor_LEDMatrix::begin(const char* _html_place, const char* _label, const char* _mqtt_name, const char* _keyword,
               bool _start_value, bool _on_value, bool _is_state, uint8_t _slider_val, uint8_t _slider_no, 
               const char* _slider_label, const char* _slider_mqtt_name, const char* _slider_keyword,
               const char* _mqtt_line, const char* _mqtt_graph) {
  Switch_OnOff::begin(_html_place, _label, _mqtt_name, _keyword, 
                      _start_value, _on_value, _is_state, _slider_val, 15, _slider_no, 
                      _slider_label, _slider_mqtt_name, _slider_keyword);
  mqtt_line = _mqtt_line;
  mqtt_graph = _mqtt_graph;
  matrix.begin();
  for (unsigned int address=0; address < LEDMATRIX_DEVICES_X * LEDMATRIX_DEVICES_Y ; address++) {
    matrix.displayTest(address, true);
    delay(200);
    matrix.displayTest(address, false);
  }
  matrix.setIntensity(_slider_val);
  matrix.clear();
  matrix.setCursor(8,8);
  matrix.print("init");
  matrix.display();
  delay(1000);
  matrix.clear();
  matrix.display();
  if (get_switch_val()) {
    matrix.on();
  } else {
    matrix.off();
  }

  html_info += String("\"tab_head_ldr\":\"Matrixdislay\"") +
               String(",\"tab_line1_matrix\":\"CLK:#GPIO: ") + String(LEDMATRIX_CLK) + String("\"")+
               String(",\"tab_line2_matrix\":\"DIN:#GPIO: ") + String(LEDMATRIX_DIN) + String("\"")+
               String(",\"tab_line3_matrix\":\"CS: #GPIO: ") + String(LEDMATRIX_CS) + String("\"")+
               String(",\"tab_line4_matrix\":\"X Devices:# ") + String(LEDMATRIX_DEVICES_X) + String("\"")+
               String(",\"tab_line5_matrix\":\"Y Devices:# ") + String(LEDMATRIX_DEVICES_Y) + String("\"");
  mqtt_info += String("\"Display-HW\":\"MAX7219 / MAX7221\"");
  mqtt_has_info = true; 

}

bool Actor_LEDMatrix::set(const String& _cmnd, const String& _val) {
  bool retval = false;
  if (Switch_OnOff::set(_cmnd, _val)) {
    if (get_switch_val()) {
        matrix.on();
    } else {
        matrix.off();
    }
    matrix.setIntensity(get_slider_val());
    retval = true;
  } else {
    if ( _cmnd == mqtt_line ) {
      print_line(_val.c_str());
      graph_change_time = now;
      retval = true;
    }
    if ( _cmnd == mqtt_graph ) {
      print_graph(_val.c_str());
      graph_change_time = now;
      retval = true;
    }
  }
  return retval;
}

void Actor_LEDMatrix::html_init() {
  Switch_OnOff::html_init();
  html_json += String(",\"matrix_x\":") + String(matrix.getNumDevicesX() * 8) +
              String(",\"matrix_y\":") + String(matrix.getNumDevicesY() * 8) + 
              String(",\"show_matrix\":1,");
  html_upd_data();
  html_json_filled = true;
}

void Actor_LEDMatrix::html_upd_data() {
  html_json += String("\"") + html_place + String("\":") + String(switch_value?"1":"0") + 
               String(",\"slider") + String(slider_no) + String("val\":\"") + String(slider_value) + 
               String("\"") + String(",\"matrix\":\"");
  getMatrixFB(html_json);
  html_json += String("\"");
}

void Actor_LEDMatrix::loop(time_t now) {
  if ( graph_change_time > 0 && now - graph_change_time > 2 ) {
    matrix.display();
    html_json = "";
    html_upd_data();
    html_update();
    graph_change_time = 0;
  }
}

void Actor_LEDMatrix::print_line(const char* rohtext ) {
  unsigned int cursor_x, cursor_y;
  char linetext[LINE_SIZE];
  uint8_t line = rohtext[0]-'0';
  uint8_t offset = (rohtext[1]-'0')*10 + (rohtext[2]-'0');
  uint8_t font = (rohtext[3]-'0');
  unsigned char changemode = rohtext[4]-'0';
  uint8_t textallign = rohtext[5];
  uint8_t textlen = strlen(rohtext)-6;
  if (textlen > LINE_SIZE -1) textlen = LINE_SIZE -1;
  if (textlen > 0) {
    memcpy( linetext, &rohtext[6], textlen );
    linetext[textlen]='\0';
  } else {
    linetext[0]='\0';
  }
  switch (changemode) {
 /*   case 9:      // Sprite
      {
        for (int x1 = 0; x1 < 6; x1++) {
          for (int x2 = 0; x2 < 6; x2++) {
            drawSprite( (byte*)&sprite[x2], x1 * 6 + x2, (line-1)*8, 8, 8 );
            matrix.display();
            delay(LED_MATRIX_ANIM_DELAY);
          }
        }            
      }
    break; */
    case 0:      // just wipe
    default:
      {
        matrix.clear(0,(line-1)*8,31,line*8-1);            
      }
  }
  matrix.setFont(font);
  switch (line) {
    case 2:
      cursor_y = 8;
    break;
    case 3:
      cursor_y = 17;
    break;
    default:
      cursor_y = 0;
  }
  cursor_x = offset;
  if (textallign == 'L') {
    matrix.setCursor(cursor_x, cursor_y);
  }
  if (textallign == 'R') {
    cursor_x = matrix.getNumDevicesX()*8 - matrix.getSize(linetext) - offset;
    matrix.setCursor(cursor_x,cursor_y);
  }
  matrix.print(linetext); 
//  matrix.display();
}

void Actor_LEDMatrix::print_graph(const char* rohtext ) {
  unsigned int length = strlen(rohtext);
  for (unsigned int pos = 0; pos + 5 <= length; pos += 5) {
    unsigned int cur_x = (rohtext[pos] - '0') * 10 + (rohtext[pos + 1] - '0');
    unsigned int cur_y = (rohtext[pos + 2] - '0') * 10 + (rohtext[pos + 3] - '0');
    matrix.setPixel(cur_x, cur_y, rohtext[pos + 4] - '0');
  }
//  matrix.display();
}

void Actor_LEDMatrix::getMatrixFB(String& fb_cont) {
  unsigned int dotx = matrix.getNumDevicesX() * 8;
  unsigned int doty = matrix.getNumDevicesY() * 8;  
  for (unsigned int y=0; y<doty; y++) {
    for (unsigned int x=0; x<dotx; x++) {
      fb_cont += matrix.getPixel(x,y)? "1" : "0";
    }
  }
}

#endif