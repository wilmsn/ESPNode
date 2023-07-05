#ifndef _ACTOR_LEDMATRIX_H_
#define _ACTOR_LEDMATRIX_H_
#include "switch_onoff.h"
#include "LED_Matrix.h"

/// @brief Ein abgeleitetes Objekt für eine eine LED Matrix.\n 
/// Benötigt: https://github.com/wilmsn/LED_Matrix als Gerätetreiber.\n 
#define LEDMATRIX_CLK        5  /* D1 */
#define LEDMATRIX_DIN        0  /* D3 */
#define LEDMATRIX_CS         2  /* D4 */
#define LEDMATRIX_DEVICES_X  4
#define LEDMATRIX_DEVICES_Y  3

#define LINE_SIZE            15

class Actor_LEDMatrix : public Switch_OnOff {

public:

    void begin(const char* html_place, const char* label, const char* mqtt_name, const char* keyword,
               bool start_value, bool on_value, unsigned int slider_val, uint8_t slider_no,
               const char* mqtt_line , const char* mqtt_graph, const char* slider_mqtt_name);

    bool set(const String& keyword, const String& value);
    
    void getMatrixFB(String& fb_cont);

    void html_create_json_part(String& json);
 
private:

    void print_line(const char* rohtext);
    void print_graph(const char* rohtext);
    String obj_mqtt_line, obj_mqtt_graph;
};

#endif