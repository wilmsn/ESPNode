# ESPNode
## Brief Description
ESPNode is a firmware for ESP8266 and ESP32. This firmware is not generic, it has to be configured and compiled for each individual node.

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
Module beschreiben bzw erzeugen eigene Objekte. Die Definition erfolgt in der Datei **"Node_settings.h"**. Die Aktivierung der Konfiguration für einen Node erfolgt in der Datei **"config.h"**

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

###Erstellung eines neuen Modules / einer neuen Klasse###

Für jedes neue Modul werden jeweils eine Headerdatei und eine CPP-Datei angelegt. Innerhalb dieser Dateien wird eine Klasse beschrieben die in den ESPNode eingebaut wird. Diese Klasse kann natürlich Objekte nutzen die wiederum auf anderen Klassen basieren. Es ist darauf zu achten das sich in der Headerdatei nur Deklarationen befnden. Definitionen und Initialisierungen kommen in die CPP Datei.

Die Modulklasse wird direkt oder indirekt von der Klasse "Base_Generic" abgeleitet. Beispiel:

	class Sensor_18B20 : public Base_Generic {
	
	public:
	
	private:
 	
	};

Das hat den Vorteil das alle benötigten Funktionen bereits vorhanden sind und dadurch sichergestellt wird das der Compiler fehlerfrei läuft. Einige der in der Klasse "Base_Generic" definierten Funktionen sind jedoch leer und müssen in der abgeleiteten Funktion mit sinnvollem Inhalt gefüllt werden.

**Wichtiger Hinweis**

Nicht benötigte Module ( wichtig bei Hardwarebezug z.B addressiete Pins ) müssen entweder

a) durch eine passende Precompilerdirektive deaktiviert werden (Beispiel Modul "actor_ledmatrix"):

Datei "actor_ledmatrix.h":

	#ifdef USE_ACTOR_LEDMATRIX
	
	... Programmext ...
	
	#endif
	
Datei "actor_ledmatrix.cpp":

	#include "config.h"
    #ifdef USE_ACTOR_LEDMATRIX
	
	... Programmext ...
	
	#endif
	
Diese Direktive wird dann in der Datei "Node_settings.h" aktiviert:

oder

b) entfernt werden => sehr unpraktisch!!

**Problem:** Besitzt ein Modul einen Hardwarebezug stören sich die Programme gegenseitig auch wenn sie nicht aktiv sind. (Beispiel: 2 Module greifen mit unterschiedlichen Methoden auf GPIO 4 zu, jedoch ist für einen Node jeweils nur ein Modul mit einer Methode aktiviert) 

**Lösung:** Umgesetzt ist die Methode a)

Die benötigte Direktive steht jeweils in der ersten Zeile der Moduldateien *.h bzw. in der zweite Zeile von *.cpp

###Aufgaben
Jedes Modul ist für die Bereitstellung seiner Daten egal ob Aktualisierung der Webseite oder Versand mittels MQTT selbst verantwortlich.

####Web
Hier sind drei Szenarien zu unterscheiden: Die Kommunikation mit der Webseite erfolgt mittels JSON Statements. Ein JSON Statenment besteht immer aus einem Wertepaar: "Schlüssel":"Wert". Mehrere Statements werden durch Komma getrennt.

**1) Öffnen einer Webseite**
Wird eine Webseite geöffnet und das Modul darüber (mittels html_init()) informiert, dann werden in der Variablen "html_json" alle benötigten JSON Statements gespeichert.

**2) Update einer Webseite**
Bei jeder Änderung, die auf der Webseite dargestellt wird, wird durch das Modul eigenständig eine JSON Nachricht mit den neuen Daten an die Webseite geschickt.

**3) Systeminfo Daten**
Hardwaredaten zu diesem Modul können auf der Seite "Systeminfo" angezeigt werden. Dazu sind folgende Maßnahmen erforderliich:

Über den Schalter "html_has_info" wird dem Hauptprogramm mitgeteilt das diese Daten vorhanden sind.

In der Variablen "html_info" werden diese Daten als JSON bereitgestellt.

Beispiel: Es soll ein Sensor mit seinem HW-GPIO angezeigt werden:

Zunächst wird eine neue Abschnittsüberschrift erzeugt. Der Schlüssel dafür lautet `tab_head_<myname>` Wichtig Da jeder Schlüssel im JSON eindeutig sein muss ist bei `<myname>èine eindeutige Bezeichnung zu wählen. Der Wert ist die angezeigte Abschnittsüberschrift. Danach folgen max 5 Zeilen in folgender Syntax:

Schlüssel: tab_lineX_id (mit X =1 ..5) 

Wert: `<Zellelinks>#<Zelle rachts>`

Beispiel:
 
	  "tab_head_ldr":"Sensor","tab_line1_ldr":"LDR:#GPIO: A0"
	

####MQTT
Mittels MQTT werden folgende Arten von Daten bereitgestellt.

**1) Telemetriedaten**

sind von ihrer Natur her statisch und verändern sich nicht während der Laufzeit. Die können z.B. Anschlüsse von Serńsoren oder Schaltern sein. Die Daten werden inḿ JSON Format in die Variable "mqtt_info" geschrieben. Um dem Hauptprogramm zu signalisieren das es diese Daten gibt ist der Schalter "mqtt_has_info" au true zu setzen.

**2) Zustandsdaten**

sind z.B Messwerte, Schalterzustände, etc. Zustandsdaten werden als normale Messwerte übertragen. Zusätzlich kann ein Wert den Status des kompletten Nodes darstellen.

Normale Messwerte werden als JSON in die Variable "mqtt_stat" geschrieben. Durch den Schalter `mqtt_has_stat = true`wird dem Hauptprogramm mitgeteilt das dieses Modul Daten zuliefert. Wichtig: "mqtt_stat" ist immer mit den letzten gültigen Daten gefüllt. Werden die Daten aktualisiert wird der Schalter "mqtt_stat_changed" auf true gesetzt. Danach werden sie zeitnah vom Hauptprogramm an den Broker geschickt.

Wird ein Wert als Nodestatus definiert ist folgendes zu beachten: Der Wert wird in die Variable "state" eingetragen. Zusätzlich muss "is_state" auf true gesetzt sein. Achtung: Ein Node kann nur einen State haben!

###Funktionen und Variablen
Hier werden nur Funktionen und Variablen beschrieben die zwingend erforderlich sind. Diese Funktionen sind alle in der Klasse "Base_Generic" definiert, es handelt ich jedoch teilweise nur um Funktionen ohne Inhalt.

####Funktion "begin()"####
Hier wird das Objekt initialisiert dabei ist naturgemäß die Anzahl der Parameter variabel. Jedliche grundlegende Konfiguration muss hier erfolgen. Im weiteren Programm gibt es dazu keine Möglichkeit mehr.
Der Einbau in den ESPNode mittels Precompilerdirektive wurde bereits behandelt.

####Funktion set( _cmnd, _val)####
Hier werden alle Anweisungen für dieses Modul umgesetzt. Diese Funktion muss bei Bedarf im abgeleiteten Modul gefüllt werden.
Innerhalb des Hauptprogrammes werden alle Befehle (Format "comand"="value") durch jede set funktion der eingebauten Module geschleust. Die Module prüfen innerhalb der **"set"** Funktion ob das "comand" für sie ein keyword ist und sie handeln müssen. Die benötigte Funktion für diese Prüfung ist im generischen Basisobjekt als Funktion **keyword_match** hinterlegt. Alle nötigen Handlungen für einen "Keyword Match" sind hier zu hinterlegen.

####Funktion "html_init()"####
Stellt alle Konfigurationsdaten (als JSON Teilstring) bereit, die für dieses Modul beim Aufruf der Webseite benötigt werden. Die Daten werden in die Variable "html_json" geschrieben. Um dem Hauptprogramm mitzuteilen das Daten zur Verfügung stehen wird der Schalter "html_json_filled" auf "true" gesetzt.

####Funktion loop(time_t now)####
In dieser Funktion steht dem Modul Rechenzeit zur Verfügung. Hier werden alle Aktionen durchgeführt die nicht durch ein "comand" veranlasst worden sind z.B. regelmäßige Messungen mit Sensoren.
ZU beachten ist folgendes: Rechenzeit ist ein knappes Gut. Es findet seitens des Hauptprogrammes keine Kontrolle/Zeitbegrenzung statt. Die Loop Funktionen aller Module wird nah dem "Round Robin" Prinzip angesteuert. Verwendung von "delay()" ist unbedingt zu vermeiden. Um eine Wartefunktion zu realisieren gbt es den übergebenen Zeitstempel.
 
 
(Modul Switch On Off)[./modul_switch_onoff.md]
 
(technische Doku)[./technischedoku.md]
 
 
