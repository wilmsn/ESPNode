#include "common.h"
#include "commands.h"

void show_settings() {
  cons_str = "{\"stat\":\"looptimealarm: ";
  cons_str += loop_time_alarm;
  cons_str += " ms\"}";
  ws.textAll(cons_str);
  cons_str += "{\"stat\":\"ENDE\"}";
  ws.textAll(cons_str);
}


void console_help() {
  cons_str = "{\"stat\":\"settings >>> Zeigt aktuelle Einstellungen\"}";
  ws.textAll(cons_str);
  cons_str = "{\"stat\":\"looptimealarm=<Maximalzeit in ms>\"}";
  ws.textAll(cons_str);
//  cons_str = "{\"stat\":\"help6\"}";
//  ws.textAll(cons_str);
//  cons_str = "{\"stat\":\"help7\"}";
//  ws.textAll(cons_str);

}

// Kommentiert in main.h
void prozess_cmd(const String cmd, const String value)  {
  write2log(LOG_SYS,4,"prozess_cmd Cmd:",cmd.c_str(),"Val:",value.c_str());
  cmd_valid = false;
  cons_str = "{\"statclear\":1}";
  ws.textAll(cons_str);
#if defined(SWITCH1)
  if ( switch1.set( cmd, value ) ) {
    html_json = switch1.html_stat_json();
    write2log(LOG_SENSOR,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch1.show_mqtt_name()), switch1.show_value());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH2)
  if ( switch2.set( cmd, value ) ) {
    html_json = switch2.html_stat_json();
    write2log(LOG_SENSOR,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch2.show_mqtt_name()), switch2.show_value());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH3)
  if ( switch3.set( cmd, value ) ) {
    html_json = switch3.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch3.show_mqtt_name()), switch3.show_value());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(SWITCH4)
  if ( switch4.set( cmd, value ) ) {
    html_json = switch4.html_stat_json();
    write2log(log_sensor,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, switch4.show_mqtt_name()), switch4.show_value());
#endif
  }
#endif
#if defined(MQTT)
  if ( cmd == "log_mqtt" ) {
    cmd_valid = true;
    if ( (value == "1") != do_log_mqtt) {
      do_log_mqtt = (value == "1");
      cmd_no++;
    }
    preferences.begin("settings",false);
    preferences.putBool("do_log_mqtt", do_log_mqtt);
    preferences.end();
  }
  if ( cmd == "mqtt_active" ) {
    cmd_valid = true;
    if ( (value == "1") != do_mqtt ) {
      do_mqtt = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_mqtt", do_mqtt);
      preferences.end();
      cmd_no++;
    }
  }
  if ( cmd == "mqttclient" ) {
    cmd_valid = true;
    if ( mqtt_client != value ) {
      mqtt_client = value;
      preferences.begin("settings",false);
      preferences.putString("mqtt_client", mqtt_client);
      preferences.end();
      cmd_no++;
    }
  }
  if ( cmd == "mqtttopicp2" ) {
    cmd_valid = true;
    if ( mqtt_topicP2 != value ) {
      mqtt_topicP2 = value;
      preferences.begin("settings",false);
      preferences.putString("mqtt_topicP2", mqtt_topicP2);
      preferences.end();
      cmd_no++;
    }
  }
  if ( cmd == "mqttserver" ) {
    cmd_valid = true;
    if ( mqtt_server != value ) {
      mqtt_server = value;
      preferences.begin("settings",false);
      preferences.putString("mqtt_server", mqtt_server);
      preferences.end();
      cmd_no++;
    }
  }
#endif
#if defined(RF24GW)
  if ( cmd == "rf24gw_active" ) {
    if ( (value == "1") != do_rf24gw) {
      do_rf24gw = (value == "1");
      preferences.begin("settings",false);
      preferences.putBool("do_rf24gw", do_rf24gw);
      preferences.end();
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "log_rf24" ) {
    if ( (value == "1") != do_log_rf24) {
      do_log_rf24 = (value == "1");
      preferences.begin("settings",false);
      preferences.putBool("do_log_rf24", do_log_rf24);
      preferences.end();
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "rf24hubname" ) {
    rf24gw_hub_server = value;
    preferences.begin("settings",false);
    preferences.putString("rf24gw_hub_server", rf24gw_hub_server);
    preferences.end();
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "rf24hubport" ) {
    rf24gw_hub_port = value.toInt();
    preferences.begin("settings",false);
    preferences.putInt("rf24gw_hub_port", rf24gw_hub_port);
    preferences.end();
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "rf24gwport" ) {
    rf24gw_gw_port = value.toInt();
    preferences.begin("settings",false);
    preferences.putInt("rf24gw_gw_port", rf24gw_gw_port);
    preferences.end();
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "rf24gwno" ) {
    rf24gw_gw_no = value.toInt();
    preferences.begin("settings",false);
    preferences.putInt("rf24gw_gw_no", rf24gw_gw_no);
    preferences.end();
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "log_rf24" ) {
    do_log_rf24 = ( value == "1" );
    preferences.begin("settings",false);
    preferences.putBool("do_log_rf24", do_log_rf24);
    preferences.end();
    cmd_valid = true;
    cmd_no++;
  }
#endif
  if ( cmd == "?" || cmd == "help" ) {
    console_help();
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "settings"  ) {
    show_settings();
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "looptimealarm" ) {
    loop_time_alarm = value.toInt();
    preferences.begin("settings",false);
    preferences.putUInt("loop_time_alarm", loop_time_alarm);
    preferences.end();
    ws.textAll("{\"statclear\":1}");
    tmp_str = "{\"stat\":\"looptimealarm: set to ";
    tmp_str += loop_time_alarm;
    tmp_str += "\"}";
    ws.textAll(tmp_str);
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "wifi_ssid" ) {
    if ( wifi_ssid != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_ssid", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "wifi_pass" ) {
    if ( wifi_pass != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_pass", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "log_sensor" ) {
    if ( do_log_sensor != ( value == "1" ) ) {
      do_log_sensor = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_log_sensor", do_log_sensor);
      preferences.end();
      write2log(LOG_SYS,2,"do_log_sensor:",do_log_sensor?"1":"0");
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "log_web" ) {
    if ( do_log_web != ( value == "1" ) ) {
      do_log_web = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_log_web", do_log_web);
      preferences.end();
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "log_sys" ) {
    if ( do_log_sys != ( value == "1" ) ) {
      do_log_sys = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_log_sys", do_log_sys);
      preferences.end();
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "log_critical" ) {
    if ( do_log_critical != ( value == "1" ) ) {
      do_log_critical = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_log_critical", do_log_critical);
      preferences.end();
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "dellogfile" ) {
    LittleFS.remove(DEBUGFILE);
    File f = LittleFS.open( DEBUGFILE, "a");
    if (f) {
      f.printf("----%d.%d.%d----\n",timeinfo.tm_mday, 1 + timeinfo.tm_mon, 1900 + timeinfo.tm_year);
      f.close();
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "restart" || cmd == "reboot" ) {
    rebootflag = true;
    cmd_valid = true;
    cmd_no++;
  }
  if ( ! cmd_valid ) {
    tmp_str  = "{\"stat\":\"Ungültiges Komando:";
    tmp_str += cmd;
    tmp_str += "\"}";
    ws.textAll(tmp_str);
  }
}

