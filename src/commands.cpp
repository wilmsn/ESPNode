#include "common.h"
#include "commands.h"

void show_settings() {
  String tmpstr;
  tmpstr = String("{\"stat\":\"looptimealarm: ") + String(loop_time_alarm) + String(" ms\"");
  tmpstr += ",\"stat\":\"ENDE\"}";
  write2log(LOG_WEB,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str() );
}


void console_help() {
  String tmpstr;
  tmpstr = String("{\"stat\":\"settings >>> Zeigt aktuelle Einstellungen\"") +
           String("{\"stat\":\"looptimealarm=<Maximalzeit in ms>\"}");
  write2log(LOG_WEB,1,tmpstr.c_str());
  ws.textAll(tmpstr.c_str());
}

// Kommentiert in main.h
void prozess_cmd(const String cmd, const String value)  {
  String tmpstr;
  write2log(LOG_SYSTEM,4,"prozess_cmd Cmd:",cmd.c_str(),"Val:",value.c_str());
  cmd_valid = false;
//  tmpstr = String("{\"alert\":\"") + cmd + String("-") + value + String("\"");
//  tmpstr = ",\"statclear\":1}";
//  write2log(LOG_WEB,1,tmpstr.c_str());
//  ws.textAll(tmpstr.c_str());
#if defined(MODULE1)
  if ( module1.set( cmd, value ) ) {
    cmd_valid = true;
  }
#endif
#if defined(MODULE2)
  if ( module2.set( cmd, value ) ) {
    cmd_valid = true;
  }
#endif
#if defined(MODULE3)
  if ( module3.set( cmd, value ) ) {
    cmd_valid = true;
  }
#endif
#if defined(MODULE4)
  if ( module4.set( cmd, value ) ) {
    cmd_valid = true;
  }
#endif
#if defined(MODULE5)
  if ( module5.set( cmd, value ) ) {
    cmd_valid = true;
  }
#endif
#if defined(MODULE6)
  if ( module6.set( cmd, value ) ) {
    cmd_valid = true;
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
    tmpstr = "{\"statclear\":1"; 
    tmpstr += String(",\"stat\":\"looptimealarm: set to ") + String(loop_time_alarm) + String("\"}");
    write2log(LOG_MODULE,1,tmpstr.c_str());
    ws.textAll(tmpstr);
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
#ifdef ESP32
  if ( cmd == "wifi_ssid1" ) {
    if ( wifi_ssid1 != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_ssid1", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "wifi_pass1" ) {
    if ( wifi_pass1 != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_pass1", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "wifi_ssid2" ) {
    if ( wifi_ssid2 != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_ssid2", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
  if ( cmd == "wifi_pass2" ) {
    if ( wifi_pass2 != value ) {
      preferences.begin("settings",false);
      preferences.putString("wifi_pass2", value);
      preferences.end();
      rebootflag = true;
    }
    cmd_valid = true;
    cmd_no++;
  }
#endif
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
    tmpstr  = String("{\"stat\":\"Ungültiges Kommando:") + cmd + String("\"}");
    write2log(LOG_MODULE,1,tmpstr.c_str());
    ws.textAll(tmpstr);
  }
}

