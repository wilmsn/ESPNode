#include "config.h"
#ifdef USE_ACTOR_LEDMATRIX
#include "common.h"
#include "actor_ledmatrix.h"

LED_Matrix matrix(LEDMATRIX_DIN, LEDMATRIX_CLK, LEDMATRIX_CS, LEDMATRIX_DEVICES_X, LEDMATRIX_DEVICES_Y);

void Actor_LEDMatrix::begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               bool start_value, bool on_value, bool is_state, uint8_t slider_val, uint8_t slider_no, 
               const char* slider_label, const char* slider_mqtt_name, const char* slider_keyword,
               const char* mqtt_line, const char* mqtt_graph) {
  Switch_OnOff::begin(html_place, label, mqtt_name, keyword, 
                      start_value, on_value, is_state, slider_val, 15, slider_no, 
                      slider_label, slider_mqtt_name, slider_keyword);
  obj_mqtt_line = mqtt_line;
  obj_mqtt_graph = mqtt_graph;
  matrix.begin();
  for (unsigned int address=0; address < LEDMATRIX_DEVICES_X * LEDMATRIX_DEVICES_Y ; address++) {
    matrix.displayTest(address, true);
    delay(200);
    matrix.displayTest(address, false);
  }
  matrix.setIntensity(slider_val);
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

  obj_html_info = String("\"tab_head_ldr\":\"Matrixdislay\"") +
                  String(",\"tab_line1_matrix\":\"CLK:#GPIO: ") + String(LEDMATRIX_CLK) + String("\"")+
                  String(",\"tab_line2_matrix\":\"DIN:#GPIO: ") + String(LEDMATRIX_DIN) + String("\"")+
                  String(",\"tab_line3_matrix\":\"CS: #GPIO: ") + String(LEDMATRIX_CS) + String("\"")+
                  String(",\"tab_line4_matrix\":\"X Devices:# ") + String(LEDMATRIX_DEVICES_X) + String("\"")+
                  String(",\"tab_line5_matrix\":\"Y Devices:# ") + String(LEDMATRIX_DEVICES_Y) + String("\"");
  obj_mqtt_has_info = true; 
}

bool Actor_LEDMatrix::set(const String& keyword, const String& value) {
  bool retval = false;
  if (Switch_OnOff::set(keyword, value)) {
    if (get_switch_val()) {
        matrix.on();
    } else {
        matrix.off();
    }
    matrix.setIntensity(get_slider_val());
    retval = true;
    html_refresh();
  } else {
    if ( keyword == obj_mqtt_line ) {
      print_line(value.c_str());
      graph_change_time = now;
      retval = true;
    }
    if ( keyword == obj_mqtt_graph ) {
      print_graph(value.c_str());
      graph_change_time = now;
      retval = true;
    }
  }
  return retval;
}

void Actor_LEDMatrix::loop(time_t now) {
  if ( graph_change_time > 0 && now - graph_change_time > 2 ) {
    obj_html_stat = "\"matrix\":\"";
    getMatrixFB(obj_html_stat);
    obj_html_stat += "\"";
    matrix.display();
    html_refresh();
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

void Actor_LEDMatrix::html_create(String& json) {
  Switch_OnOff::html_create(json);
  json += ",\"slider1max\":15,\"matrix_x\":";
  json += matrix.getNumDevicesX() * 8;
  json += ",\"matrix_y\":";
  json += matrix.getNumDevicesY() * 8;
  json += ",\"show_matrix\":1";
  json += ",\"matrix\":\"";
  getMatrixFB(json);
  json += "\"";
}
#endif