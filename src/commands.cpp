#include "common.h"
#include "commands.h"

void show_settings() {
  html_json = "{\"stat\":\"looptimealarm: ";
  html_json += loop_time_alarm;
  html_json += " ms\"}";
  write2log(LOG_WEB,1,html_json.c_str());
  ws.textAll(html_json);
  html_json += "{\"stat\":\"ENDE\"}";
  write2log(LOG_WEB,1,html_json.c_str());
  ws.textAll(html_json);
}


void console_help() {
  html_json = "{\"stat\":\"settings >>> Zeigt aktuelle Einstellungen\"}";
  write2log(LOG_WEB,1,html_json.c_str());
  ws.textAll(html_json);
  html_json = "{\"stat\":\"looptimealarm=<Maximalzeit in ms>\"}";
  write2log(LOG_WEB,1,html_json.c_str());
  ws.textAll(html_json);
}

// Kommentiert in main.h
void prozess_cmd(const String cmd, const String value)  {
  write2log(LOG_SYSTEM,4,"prozess_cmd Cmd:",cmd.c_str(),"Val:",value.c_str());
  cmd_valid = false;
  html_json = "{\"statclear\":1}";
  write2log(LOG_WEB,1,html_json.c_str());
  ws.textAll(html_json);
#if defined(MODULE1)
  if ( module1.set( cmd, value ) ) {
    html_json = module1.html_stat_json();
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, module1.show_mqtt_name().c_str()), module1.show_value().c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(MODULE2)
  if ( module2.set( cmd, value ) ) {
    html_json = module2.html_stat_json();
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, module2.show_mqtt_name().c_str()), module2.show_value().c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(MODULE3)
  if ( module3.set( cmd, value ) ) {
    html_json = module3.html_stat_json();
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, module3.show_mqtt_name().c_str()), module3.show_value().c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(MODULE4)
  if ( module4.set( cmd, value ) ) {
    html_json = module4.html_stat_json();
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, module4.show_mqtt_name().c_str()), module4.show_value().c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(MODULE5)
  if ( module5.set( cmd, value ) ) {
    html_json = module5.html_stat_json();
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, module5.show_mqtt_name().c_str()), module5.show_value().c_str());
    do_send_mqtt_stat = true;
#endif
  }
#endif
#if defined(MODULE6)
  if ( module6.set( cmd, value ) ) {
    html_json = module6.html_stat_json();
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
#if defined(MQTT)
    mqttClient.publish(mk_topic(MQTT_STATUS, module6.show_mqtt_name().c_str()), module6.show_value().c_str());
    do_send_mqtt_stat = true;
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
#ifdef WEBRADIO


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
    html_json = "{\"statclear\":1}"; 
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    html_json = "{\"stat\":\"looptimealarm: set to ";
    html_json += loop_time_alarm;
    html_json += "\"}";
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
    cmd_valid = true;
    cmd_no++;
  }
/*  if ( cmd == "wifi_ssid" ) {
    if ( wifi_ssid1 != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_ssid", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "wifi_pass" ) {
    if ( wifi_pass1 != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_pass", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  } */
  if ( cmd == "log_module" ) {
    if ( do_log_module != ( value == "1" ) ) {
      do_log_module = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_log_module", do_log_module);
      preferences.end();
      write2log(LOG_SYSTEM,2,"do_log_module:",do_log_module?"1":"0");
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
  if ( cmd == "log_system" ) {
    if ( do_log_system != ( value == "1" ) ) {
      do_log_system = ( value == "1" );
      preferences.begin("settings",false);
      preferences.putBool("do_log_system", do_log_system);
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
    html_json  = "{\"stat\":\"Ungültiges Komando:";
    html_json += cmd;
    html_json += "\"}";
    write2log(LOG_MODULE,1,html_json.c_str());
    ws.textAll(html_json);
  }
}

