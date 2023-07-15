# ESPNode
## brief description
ESPNode is a firmware for ESP8266 and ESP32 (in future). This firmware is not generic, it has to be configured and compiled for each individual node.

#### Features:

- Implemented RF24 Gateway for my RF24Hub (https://github.com/wilmsn/RF24Hub)
- Implemented MQTT Client
- Web Interface (based on J-Query and Websockets) 
- Written in C++ for Arduino

!["ESPNode Komponenten"](https://wilmsn.github.io/ESPNode/espnode.png)

ESPNode as a part of RF24Hub

!["Einbindung des ESPNode in den RF24Hub"](https://wilmsn.github.io/ESPNode/einbindung.png)

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

* Ausführen der BEGIN Staements aller Sensoren und Switches

* Starten des Messvorgangs aller Sensoren

* Start des MQTT Clients

Danach ist das Programm initialisiert und läuft in einer Endlosschleife. Innerhalb der Schleife finden (zum Teil zeitgesteuert) folgende Aktionen statt:

* RF24 Gateway (falls aktiviert): Prüfen ob neue Datenpakete anliegen, wenn ja dann an den Hub weiterleiten

* MQTT auf Pakete prüfen und veraarbeiten

* loop() Funktion aller Sensoren und Switches bedienen

Zeitabhängige Aktionen:

* Telemetriedaten senden (var: TELEINTERVAL)

* Messungen starten (var: STATINTERVAL)

* Statusdaten und Messwerte senden (var: MESSINTERVAL) 



## Sensormodule
Sensormodule beschreiben bzw erzeugen eigene Objekte. Die Einbindung erfolgt über die Datei **"Node_settings.h"**. Dort müssen die nachfolgenden Funktionen vorhanden und mit Inhalt gefüllt sein. Zur Kompatibilität sind die Funktionen als Leerfunktion im Grundobjekt **"Sensor_Generic"** bereits vorhanden. Alle neuen Objekte für Sensoren und Aktoren werden von **"Sensor_Generic"** abgeleitet. 

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
