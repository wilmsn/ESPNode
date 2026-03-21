# ESPNode
## Brief Description
ESPNode is a firmware for ESP8266 and ESP32. This firmware is not generic, it has to be configured and compiled for each individual node. If you are looking for a precompiled ready to use Firmware try TASMOTA or similar.

### Features:

- Implemented RF24 Gateway for my RF24Hub (https://github.com/wilmsn/RF24Hub)
- Implemented MQTT Client
- Web Interface (based on J-Query and Websockets) 
- Written in C++ for Arduino

![ESPNode Komponenten](https://wilmsn.github.io/ESPNode/espnode.png)

The main documentation of this projekt is in german language.
 
[See the web version here](http://wilmsn.github.io/ESPNode/index.html) 

## Hauptprogramm

Mehr Details gibt es in der [Onlineprogrammdoku](http://wilmsn.github.io/ESPNode/index.html).

Im Hauptprogramm sind folgende Komponenten vorgesehen:

* Einbau von bis zu 6 Modulen.

Sofern die Module/Objektdefinitionen bereits vorhanden sind, kann das Programm einfach durch Konfiguration angepasst werden. Ohne Konfiguration sind alle Elemente deaktiviert. Details über die Konfiguration sind [hier](#konfiguration) zu finden

Initialisierung des Hauptprogrammes:
Hier werden nacheinander folgende Dinge erledigt:

* Einlesen von Konfigurationsdaten aus den Preferences

* Öffnen des internen Filesystems

* Verbindung mit dem WLAN

* Starten des Webservers

* Starten des RF24 Gateways (falls aktiviert)

* Start des MQTT Clients (falls aktiviert)

* Ausführen der BEGIN Statements aller Module

Danach ist das Programm initialisiert und läuft in einer Endlosschleife. Innerhalb der Schleife finden (zum Teil zeitgesteuert) folgende Aktionen statt:

* RF24 Gateway (falls aktiviert): Prüfen ob neue Datenpakete anliegen, wenn ja dann an den Hub oder Node weiterleiten

* MQTT auf Pakete prüfen und verarbeiten

* loop() Funktion aller Module bedienen

Zeitabhängige Aktionen:

* Telemetriedaten senden (var: TELEINTERVAL)

* Messungen starten (var: MESSINTERVAL)

* Statusdaten und Messwerte senden (var: STATINTERVAL) 

## Module
Module beschreiben bzw erzeugen eigene Objekte. Die Definition erfolgt in der Datei **"Node_settings.h"**. Die Aktivierung der Konfiguration für einen Node erfolgt in der Datei **"config.h"**. Wird die Konfiguration in einer dieser Dateien geändert muss die Firmware neu kompiliert werden.

Details zu den Modulen in der [Modulbeschreibung](module.md)

Für die nachfolgenden Beispiele reichen die mitgelieferten Klassenbibliotheken.

###Beispiel 1: Einfacher Node (ESP8266 oder ESP32) ohne externe Bauteile
Hie kann auf einem ESP8266/ESP32 lediglich die interne LED per Schalter über die Weboberfläche ein und ausgeschaltet werden.

	#ifdef NODESIMPLE
	#define USE_SWITCH_ONOFF
	#include "switch_onoff.h"
	
	#define HOSTNAME               "nodesimple"
	#define HOST_DISCRIPTION       "Ein ESP32 Node ohne externe Elemente"
	#define DEBUG_SERIAL_WEB
	#define DEBUG_SERIAL_MODULE
	#define DEBUG_SERIAL_MQTT
	#define MODULE1_DEFINITION      Switch_OnOff module1;
	#ifdef ESP32
	#ifndef LED_BUILTIN
	#define LED_BUILTIN   2
	#endif
	#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, true, LED_BUILTIN);
	#else
	#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, false, 2);
	#endif
	#define DO_LOG_SYSTEM           true
	#define MAGICNO                 0
	
	#endif
	
###Beispiel 2: Demo für die Witty Cloud
Die Witty Cloud ist ein ESP8266 Modul mit einer RGB LED und einem LDR. In der Weboberfläche gibt es 4 Schalter für die RGB Leds und die interne LED. Zusätzlich kann die Helligkeit der jeweiligen RGB Led mittels SChieberegler geändert werden.

	#ifdef WITTYNODE
	
	#define DEBUG_SERIAL_MODULE
	#define DEBUG_SERIAL_WEB
	
	#define USE_SWITCH_ONOFF
	#include "switch_onoff.h"
	#define USE_SENSOR_LDR
	#include "sensor_ldr.h"
	
	#define WITTY_RGB_RT           15
	#define WITTY_RGB_GN           12
	#define WITTY_RGB_BL           13
	#define WITTY_LED_PIN          2
	
	#define MAGICNO                123
	
	#define HOSTNAME               "wittynode"
	#define HOST_DISCRIPTION       "A Witty Node"
	
	#define MQTT_CLIENT            "WittyNode"
	#define MQTT_TOPICP2           "wittynode"
	
	#define DEBUG_SERIAL_WEB
	#define DEBUG_SERIAL_MODULE
	#define DEBUG_SERIAL_MQTT
	#define DEBUG_SERIAL_SYSTEM
	
	#define DO_LOG_MQTT              true
	#define DO_LOG_MODULE            true
	#define DO_LOG_WEB               true
	#define DO_LOG_SYSTEM            true
	
	#define MODULE1_DEFINITION      Switch_OnOff module1;
	#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, false, WITTY_LED_PIN);
	
	#define MODULE2_DEFINITION      Switch_OnOff module2;
	#define MODULE2_BEGIN_STATEMENT module2.begin("sw2", "RGB rot", "rot", "rot", false, true, false, WITTY_RGB_RT, 100, 100, 1, "Rot-Helligkeit", "rot_sl", "rot_sl");
	
	#define MODULE3_DEFINITION      Switch_OnOff module3;
	#define MODULE3_BEGIN_STATEMENT module3.begin("sw3", "RGB gruen", "gruen", "gruen", false, true, false, WITTY_RGB_GN, 100, 100, 2, "Grün Helligkeit", "gruen_sl", "gruen_sl");
	
	#define MODULE4_DEFINITION      Switch_OnOff module4;
	#define MODULE4_BEGIN_STATEMENT module4.begin("sw4", "RGB blau", "blau", "blau", false, true, false, WITTY_RGB_BL, 100, 100, 3, "Blau Helligkeit", "blau_sl", "blau_sl");
	
	#define MODULE5_DEFINITION      Sensor_LDR module5;
	#define MODULE5_BEGIN_STATEMENT module5.begin("out1", "LDR", 30);
	
	#endif
	
###Funktion der jeweiligen Zeilen:

	#ifdef NODESIMPLE
	#endif
Syntax:
	#ifdef <Name des Nodes>
	#endif
Zwischen diesen beiden Zeilen befindet sich die Definition für den Node "NODESIMPEL". (Zeilen sind verpflichtend)

Diese Konfiguration wird in der Datei **config.h** mit der folgenden Zeile aktiviert.

	#define NODESIMPEL

Syntax:

	#define <Name des Nodes>

**Achtung:** Es darf immer nur eine Konfiguration aktiviert sein !!!	

	#include "switch_onoff.h"

Syntax:

	#include "<Name der Headerdatei>"

Hier wird/werden die Headerdatei(en) für das/die verwendete Modul(e) includiert.
(min. 1 Zeile ist verpflichtend)

	#define HOSTNAME               "nodesimple"

Syntax:

	#define HOSTNAME               <Netzwerkname des Nodes>

Legt den Hostnamen fest. Dieser wird für die Anmeldung im Netz genutzt und in der Weboberfläche angezeigt.
(Zeile ist verpflichtend)

	#define HOST_DISCRIPTION       "Ein ESP8266 Node ohne externe 

Syntax:

	#define HOST_DISCRIPTION       <Beschreibender Text> 

Ein beschreibender Text für die Weboberfläche.
(Zeile ist optional)

	#define MODULE1_DEFINITION      Switch_OnOff module1;
	#define MODULE2_DEFINITION      Sensor_LDR module2;

Syntax:

	#define MODULE[1..6]_DEFINITION      <Name der Objektklasse> module[1...6];

Durch diese Zeile wird aus dem Klassenmodell eine Instanz. Wichtig dabei ist: 
Die Nummern müssen aufsteigend, mit 1 beginnend vergeben werden.

	#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, true, LED_BUILTIN);
	
Syntax:
	#define MODULE[1...6]_BEGIN_STATEMENT module[1...6].begin(<Parameter1>,...,<ParameterN>);
Hier wird die Initialisierung des Objektes durchgeführt. Die Art und Anzahl der übergebenen Parameter hängt von deren Definition in der Klassenbibliothek ab.
(min. 1 Zeile ist verpflichtend)

###Weitere Konfigurationszeilen:
Einschalten und konfigurieren des eingebauten MQTT Clients:

	#define MQTT_CLIENT            "wittynode"
	#define MQTT_TOPICP2           "wittynode"
	
Syntax:

	#define MQTT_CLIENT            <Name des Clients>
	#define MQTT_TOPICP2          <Topic Part 2>

Mit den beiden Zeilen wird der MQTT Client eingeschaltet und konfiguriert. (Wenn MQTT verwendet werden soll sind beide Zeilen verpflichtend)

`<Name des Clients>` ist der Name mit dem sich dieser Client bein MQTT Server anmeldet.

Das Topic des MQTT Server (Vergleichbar mit einem Pfad im Dateisystem) ist im allgemeinen so aufgebaut:

`<Topic Part 1> / <Topic Part 2> / <Topic Part 3>` Topic Part 1 und Topic Part 3 sind fest in der Konfiguration hinterlegt und werden idR. nicht geändert.

Topic 1 und Topic 3 sind fallbezogen fest hinterlegt und können nur im Bedarf global in der Datei config.h geändert werden.

Topic Part 2 kann bei Bedarf wiederum aus mehreren Subtopics bestehen. Denkbar wäre also folgende Konstellation:

	   stat/haus/keller/bastelkeller/temperaturnode/info1

In diesem Fall wäre `<Topic Part 2>` "haus/keller/bastelkeller/temperaturnode"

---
Einschalten und konfigurieren des eingebauten RF24 Gateways. Nähere Infos [RF24Hub auf Github](https://github.com/wilmsn/RF24Hub)

	#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
	#define RF24GW_NO                	   103

Syntax:

	#define RF24GW_HUB_SERVER        <Hostname des RF24 Servers>
	#define RF24GW_NO                         <Nummer des Gateways>

Mit diesen beiden Zeilen wird der RF24 Gateway konfiguriert und aktiviert. Zum RF24 Gateway gibt es noch mehrere Einstellungen
in der Datei "config.h", die einmalig für das komplette System festgelegt werden.
---
Einstellungen zum Debugging (Sollten in der produktiven Umgebung ausgeschaltet sein)

Debuggingausgaben zum Web auf der seriellen Schnittstelle

	#define DEBUG_SERIAL_WEB

Debuggingausgaben zum Modul auf der seriellen Schnittstelle

	#define DEBUG_SERIAL_MODULE

Debuggingausgaben zu MQTT auf der seriellen Schnittstelle

	#define DEBUG_SERIAL_MQTT

Debuggingausgaben zum System auf der seriellen Schnittstelle

	#define DEBUG_SERIAL_SYSTEM

####MQTT
Mittels MQTT werden folgende Arten von Daten bereitgestellt.

**1) Telemetriedaten**

sind von ihrer Natur her eher statisch und verändern sich nicht oder nur langsam während der Laufzeit. Die können z.B. Anschlüsse von Sensoren oder Schaltern sein. Die Daten werden in JSON Format in die Variable "mqtt_info" geschrieben. Um dem Hauptprogramm zu signalisieren das es diese Daten gibt ist der Schalter "mqtt_has_info" au true zu setzen.

**2) Zustandsdaten**

sind z.B Messwerte, Schalterzustände, etc. Zustandsdaten werden als normale Messwerte übertragen. Zusätzlich kann ein Wert den Status des kompletten Nodes darstellen.

Normale Messwerte werden als JSON in die Variable "mqtt_stat" geschrieben. Durch den Schalter `mqtt_has_stat = true`wird dem Hauptprogramm mitgeteilt das dieses Modul Daten zuliefert. Wichtig: "mqtt_stat" ist immer mit den letzten gültigen Daten gefüllt. Werden die Daten aktualisiert wird der Schalter "mqtt_stat_changed" auf true gesetzt. Danach werden sie zeitnah vom Hauptprogramm an den Broker geschickt.

Wird ein Wert als Nodestatus definiert ist folgendes zu beachten: Der Wert wird in die Variable "state" eingetragen. Zusätzlich muss "is_state" auf true gesetzt sein. Achtung: Ein Node kann nur einen State haben!

 
 
