# ESPNode
## Brief Description
ESPNode is a firmware for ESP8266 and ESP32. This firmware is not generic, it has to be configured and compiled for each individual node.

#### Features:

- Implemented RF24 Gateway for my RF24Hub (https://github.com/wilmsn/RF24Hub)
- Implemented MQTT Client
- Web Interface (based on J-Query and Websockets) 
- Written in C++ for Arduino

![ESPNode Komponenten](https://wilmsn.github.io/ESPNode/espnode.png)

ESPNode as a part of RF24Hub

![Einbindung des ESPNode in den RF24Hub](https://wilmsn.github.io/ESPNode/einbindung.png)

The main documentation of this projekt is in german language.
 
[Programmdoku auf wilmsn.github.io/ESPNode](http://wilmsn.github.io/ESPNode/index.html) 

## Hauptprogramm

Details zum Hauptprogramm gibt es in der [Programmdoku](http://wilmsn.github.io/ESPNode/index.html), hier wird nur das zugrundeliegende Konzept beschrieben.
Im Hauptprogramm sind folgende Komponenten vorgesehen:

* Einbau von bis zu 4 Schaltern

* Einbau von bis zu 2 Sensoren (Hardware) mit bis zu 4 Messwerten in Summe

Sofern die Module/Objektdefinitionen bereits vorhanden sind, kann das Programm einfach durch Konfiguration angepasst werden. Ohne Konfiguration sind alle Elemente deaktiviert. Details über die Konfiguration sind [hier](#konfiguration) zu finden

Initialisierung des Hauptprogrammes:
Hier werden nacheinander folgende Dinge erledigt:

* Verbindung mit dem WLAN (es werden die 2 konfigurierten Netze getestet)

* Öffnen des internen Filesystems

* Starten des Webservers

* Starten des RF24 Gateways (falls aktiviert)

* Ausführen der BEGIN Statements aller Sensoren und Switches

* Start des Messvorgangs aller Sensoren

* Start des MQTT Clients

Danach ist das Programm initialisiert und läuft in einer Endlosschleife. Innerhalb der Schleife finden (zum Teil zeitgesteuert) folgende Aktionen statt:

* RF24 Gateway (falls aktiviert): Prüfen ob neue Datenpakete anliegen, wenn ja dann an den Hub oder Node weiterleiten

* MQTT auf Pakete prüfen und veraarbeiten

* loop() Funktion aller Sensoren und Switches bedienen

Zeitabhängige Aktionen:

* Telemetriedaten senden (var: TELEINTERVAL)

* Messungen starten (var: STATINTERVAL)

* Statusdaten und Messwerte senden (var: MESSINTERVAL) 



## Sensormodule
Sensormodule beschreiben bzw erzeugen eigene Objekte. Die Definition erfolgt in der Datei **"Node_settings.h"**. Für die nachfolgenden Beispiele reichen die mitgelieferten Klassenbibliotheken.

###Beispiel 1: Einfacher ESP8266 Node ohne externe Bauteile
Hie kann auf einem ESP8266 lediglich die interne LED per Schalter über die Weboberfläche ein und ausgeschaltet werden.

	#if defined(NODESIMPLE)
	#include "switch_onoff.h"
	#define HOSTNAME               "nodesimple"
	#define HOST_DISCRIPTION       "Ein ESP8266 Node ohne externe Elemente"
	#define MQTT_CLIENT            "espmini"
	#define MQTT_SERVER            "rpi1.fritz.box"
	#define DEBUG_SERIAL_HTML
	#define DEBUG_SERIAL_SENSOR
	#define DEBUG_SERIAL_MQTT
	#define SWITCH1_DEFINITION      Switch_OnOff switch1;
	#define SWITCH1_BEGIN_STATEMENT switch1.begin("sw1", "interne LED", "int_led", "int_led", 2, false, false);
	#endif

###Beispiel 2: Demo für die Witty Cloud
Die Witty Cloud ist ein ESP8266 Modul mit einer RGB LED und einem LDR. In der Weboberfläche gibt es 4 Schalter für die RGB Leds und die interne LED. Zusätzlich kann die Helligkeit der jeweiligen RGB Led mittels SChieberegler geändert werden.

	#if defined(WITTYNODE1)
	#define WITTY_RGB_RT           15
	#define WITTY_RGB_GN           12
	#define WITTY_RGB_BL           13
	#define WITTY_LED_PIN          2
	#include "switch_onoff.h"
	#include "sensor_ldr.h"
	#define MAGICNO                479
	#define HOSTNAME               "wittynode"
	#define HOST_DISCRIPTION       "A Witty Node"
	#define MQTT_CLIENT            "wittynode"
	#define MQTT_TOPICP2           "wittynode"
	#define SWITCH1_DEFINITION      Switch_OnOff switch1;
	#define SWITCH1_BEGIN_STATEMENT switch1.begin("sw1", "interne LED", "int_led", "int_led", WITTY_LED_PIN, false, false);
	#define SWITCH2_DEFINITION      Switch_OnOff switch2;
	#define SWITCH2_BEGIN_STATEMENT switch2.begin("sw2", "RGB rot", "rot", "rot", WITTY_RGB_RT, false, true);
	#define SWITCH3_DEFINITION      Switch_OnOff switch3;
	#define SWITCH3_BEGIN_STATEMENT switch3.begin("sw3", "RGB gruen", "gruen", "gruen", WITTY_RGB_GN, false, true);
	#define SWITCH4_DEFINITION      Switch_OnOff switch4;
	#define SWITCH4_BEGIN_STATEMENT switch4.begin("sw4", "RGB blau", "blau", "blau", WITTY_RGB_BL, false, true);
	#define SENSOR1_DEFINITION      Sensor_LDR sensor1;
	#define SENSOR1_BEGIN_STATEMENT sensor1.begin("sens1", "ldr");
	#endif

###Funktion der jeweiligen Zeilen:

	#if defined(NODESIMPLE)
	#endif
Syntax:
	#if defined(<Name des Nodes>)
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

	#define SWITCH1_DEFINITION      Switch_OnOff switch1;
	#define SENSOR1_DEFINITION      Sensor_LDR sensor1;
Syntax:
	#define SWITCH[1..4]_DEFINITION      <Name der Objektklasse> switch[1...4];
	#define SENSOR[1...2]_DEFINITION      <Name der Objektklasse> sensor[1...2];
Durch diese Zeile wird aus dem Klassenmodell eine Instanz. Wichtig dabei ist: 
Ein Switch heisst immer switch[1...4], ein Sensor heisst immer sensor[1...2].
Die Nummern müssen aufsteigend, mit 1 beginnend vergeben werden.

	#define SWITCH1_BEGIN_STATEMENT switch1.begin("sw1", "interne LED", "int_led", "int_led", 2, false, false);
	#define SENSOR1_BEGIN_STATEMENT sensor1.begin("sens1", "ldr");
Syntax:
	#define SWITCH[1...4]_BEGIN_STATEMENT switch[1...4].begin(<Parameter1>,...,<ParameterN>);
	#define SENSOR[1...2]_BEGIN_STATEMENT sensor[1...2].begin(<Parameter1>,...,<ParameterN>);
Hier wird die Initialisierung des Objektes durchgeführt. Die Art und Anzahl der übergebenen Parameter hängt von deren Definition in der Klassenbibliothek ab.
(min. 1 Zeile ist verpflichtend)

###Weitere Konfigurationszeilen:
Einschalten und konfigurieren des eingebauten MQTT Clients:

	#define MQTT_CLIENT            "wittynode"
	#define MQTT_TOPICP2           "wittynode"
Syntax:
	#define MQTT_CLIENT            <Name des Clients>
	#define MQTT_TOPICP2          <Name des Topicp2>
Mit den beiden Zeilen wird der MQTT Client eingeschaltet und konfiguriert.
<Name des Clients> ist der Name mit dem sich dieser Client bein MQTT Server anmeldet.
Das Topic des MQTT Server (Vergleichbar mit einem Pfad im Dateisystem) ist im allgemeinen so aufgebaut:
<Topic Part 1>/<Topic Part 2>/<Topic Part3> Topic Part 1 und Topic Part 3 sind fest in der Konfiguration hinterlegt und werden idR. nicht geändert.
Topic Part 2 kann bei Bedarf wiederum aus mehreren Subtopics bestehen. Denkbar wäre also folgende Konstellation:
   stat/haus/keller/bastelkeller/temperaturnode/info1
In diesem Fall wäre <Name des Topicp2> "haus/keller/bastelkeller/temperaturnode"
(Wenn MQTT verwendet werden soll sind beide Zeilen verpflichtend)

Einschalten und konfigurieren des eingebauten RF24 Gateways:

	#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
	#define RF24GW_NO                	   103
Syntax:
	#define RF24GW_HUB_SERVER        <Hostname des RF24 Servers>
	#define RF24GW_NO                         <Nummer des Gateways>
Mit diesen beiden Zeilen wird der RF24 Gateway konfiguriert und aktiviert. Zum RF24 Gateway gibt es noch mehrere Einstellungen
in der Datei "config.h", die einmalig für das komplette System festgelegt werden.

Einstellungen zum Debugging (Sollten in der produktiven Umgebung ausgeschaltet sein)
	
	
	
-------------------------------------------------------------------------
 Dort müssen die nachfolgenden Funktionen vorhanden und mit Inhalt gefüllt sein. Zur Kompatibilität sind die Funktionen als Leerfunktion im Grundobjekt **"Sensor_Generic"** bereits vorhanden. Alle neuen Objekte für Sensoren und Aktoren werden von **"Sensor_Generic"** abgeleitet. 

### Funktion "begin()"
Dies Funktion kann je nach Einsatz und Aufgabe unterschiedliche Parameter erfordern. Damit die Funktion universell eingebaut werden kann, erfolgt ihr "Einbau" in der Datei  **"Node_settings.h"** über eine Precompiler Direktive. Beispiel:

	#include "sensor_18B20.h"
	#define SENSOR1_DEFINITION       Sensor_18B20 sensor1;
	#define SENSOR1_BEGIN_STATEMENT  sensor1.begin("sens1","Temperatur","Temperatur");

In der ersten Zeile wird die Headerdatei für die Objektdefinition eingebunden. In der zweiten Zeile das Objekt definiert. Die Direktive **"SENSOR1_DEFINITION"** MUSS für den ersten Sensor verwendet werden. Gibt es weitere Sensoren, gibt es im Programm eine Vorbereitung für 3 Sensoren (SENSOR1 bis SENSOR3) und 4 Aktoren (SWITCH1 bis SWITCH4).
Liefert eine Hardware mehrere Messwerte, wird diese als ein Sensor implementiert (siehe "sensor_bosch.h/cpp". Die Objektdefinition ist hier "Sensor_18B20". Dieser Ausdruck ist variabel und  entspricht dem der Definition. Das erzeugte Objekt heist "sensor1".

In die Variable **obj_sensorinfo** wird ein Teil-JSON mit allen unter Sysinfo anzuzeigenden Sensorinfos angelegt.

Syntax: **"sensorinfo1 : Sensordetails linke Spalte # Sensordetails rechte Spalte"**

In der Webseite ist Platz für **"sensorinfo1"** bis **"sensorinfo5"** vorgesehen

Innerhalb des Programmes wird in diesem Schritt alles nötige getan um das Objekt zu initalisieren und alle Werte auf Startwerte gesetzt.

Die Implementierung erfolgt wiederum über eine Precompilerdirektive (Zeile3):
Der erste Teil "SENSOR1_BEGIN_STATEMENT" ist statisch. Der zweite Teil wird entsprechend der benötigten Objektinitialisierung angepasst.

### Funktion "start_measure()"
Innerhalb dieser Funktion  muss bei einem Sensor zwingend folgendes Programmiert werden:

1) Alles nötige zum Auslesen des oder der Messwerte.

2) Befüllung folgender Variablen (vordefiniert in "Sensor_Generic")

2.a) **"obj_html_stat_json"**
Hier muss ein kompletter gültiger JSON eingetragen mit dem/den aktuellen Messwert(en) eingetragen werden.

Syntax: **{"Einbauplatz"  : "Anzuzeigender Schriftzug"}**

Beispiel: **{"sens1" : "Temperatur 21 °C"}**

2.b) **"obj_mqtt_json"**
Hier muss ein Teil-JSON für alle Messwerte zur Übertragung mittels MQTT eingetragen werden.

Syntax: **{"Messwertlabel"  : "Wert"}**

Beispiel: **{"Temperatur" : 21.2 }**


### Funktion set( keyword, value)
Diese Funktion muss für jeden Aktor/Switch gefüllt werden. Innerhalb des Hauptprogrammes werden alle Befehle (Format Item=value) durch jede set funktion der eingebauten Objekte geschleust. Die Objekte prüfen ob das Item für sie ein keyword ist und sie handeln müssen. Die benötigte Funktion für diese Prüfing ist im generischen Basisobjekt als Funktion **keyword_match** hinterlegt. Nach Beendigung der Funktion müssen auch hier die Variablen **"obj_html_stat_json"** und  **"obj_mqtt_json"** gefüllt sein.

### Funktion "html_create_json_part(json)"
Ein Sensor aktiviert seine Anzeige selbst indem der anzuzeigende Wert in das passende HTML Feld geschrieben wird. Bei einem Aktor/Switch muss dazu die Funktion "html_create_json_part" gefüllt werden. Durch das hier erzeugte JSON wird festgelgt was wie auf der HTML Seite angezeigt wird. Dabei gibt es folgende Möglichkeit:

a) Reiner Ein-Aus Schalter

Codesegment: **"sw1 : 0 , sw1_label : Lichtschalter , sw1_format : x"**

b) Schalter mit Regler

Codesegment: **"sw1 : 0 , sw1_label : Lichtschalter , sw1_format : x, slider1 : 1 , slider1label : Helligkeit , slider1val : 54"**

c) Jede andere Kombination die nach Änderung der HTML Seite benötigt wird

Beispiele dazu in der Datei switch_onoff.h/cpp

## Konfiguration
