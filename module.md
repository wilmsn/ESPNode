# Module
In diesem Dokument wird die Programmierung der Module und ihre Einbindung ins Hauptprogramm beschrieben.
##Grundlagen
Ein Modul besteht aus einer Klasse, die direkt oder indirekt von der Klasse "Base_Generic" abgeleitet wurde. In der Klasse"Base_Generic" sind bereits alle benötigten Eigenschaften und Methoden vorhanden, teilweise müssen diese aber noch mit Inhalt gefüllt werden.

Inhaltlich ist jedes Modul eigenständig und für sich verantwortlich. Es bestehen Schnittstellen um mit dem Rahmenprogramm zu kommunizieren. Diese müssen bedient werden und sind im weiteren Verlauf beschrieben.
 
Im Hauptprogramm ist die Einbindung von 6 Modulen vorgesehen. Die Modulnummer wird aufsteigend und lückenlos von 1 bis 6 verwendet.

Auf der Weboberfläche sind 4 Schalter vorgesehen. Diese Schalter haben den Namen "sw1" bis "sw4".

Auf der Weboberfläche sind 3 Schieberegler vorgesehen. Diese Schieberegler werden idR. zusammen mit Schaltern verwendet. Siehe dazu "switch_onoff". Die Schieberegler haben die Namen "slider1" bis "slider3".

Auf der Weboberfläche sind 4 Ausgabezeilen vorgesehen. Diese Felder haben den Namen "out1" bis "out4".

Daneben können auf der Webseite noch weitere Objekte angelegt werden, diese werden hier jedoch nicht beschrieben.

**MQTT:**

Für die Datenübertragung über MQTT sind 2 Nachrichten vorgesehen.

* Sammelnachricht im JSON Format

In dieser Nachricht werden alle zu übermittelnden Werte als JSON übermittelt. In jedem Modul wird ein Teil-JSON angelegt.

* Statusnachricht

ZUsätzlich kann ein Wert bestimmt werden, der den Status des Objektes (z.B. in FHEM) anzeigt. Es gibt nur einen Status für den kompletten Node!

Zusätzlich gibt es Zulieferungen zur Systeminfo des Nodes.

##Anforderungen an die Software
Grundsätzlich ist das Modul dafür verantwortlich:
1. Alle Inhalte selbst zu verwalten
2. Sparsam mit der CPU Zeit umzugehen (kein delay()!)
3. GPIOs selbst zu verwalten (initialisieren, setzen)
4. Webinhalte sebst zu verwalten

##Schnittstellen zum Hauptprogramm
Die nachfolgenden Schnittstellen werden durch das Hauptprogramm aufgerufen und müssen vorhanden sein. Wenn ein Modul auf "Base_Generic" als Vaterobjekt aufbaut ist das gewährleistet.

###Funktion "begin()"
Hier wird das Objekt initialisiert dabei ist naturgemäß die Anzahl der Parameter variabel. Jedliche grundlegende Konfiguration muss hier erfolgen. Im weiteren Programm gibt es dazu keine Möglichkeit mehr.
Der Einbau in den ESPNode mittels Precompilerdirektive siehe:
**Einbindung eines Modules**

###Funktion loop(time_t now)
Wärend des Laufs wird die loop Funktion regelmäßig aufgerufen. Alle loop Funktionen aller Module und die im Hauptprogramm aufgerufenen periodischen Funktionen werden nach dem "round Robin" Prinzip nacheinander aufgerufen. Erst wenn wenn sie beendet ist folgt die nächtse. Es gibt keine zeitliche Begrenzung, dies liegt in der Verantwortung des Moduls! Durch Übergabe des Zeitstempels sind Zeitmessungen (z.B. für Wartezeit) möglich.

###Funktion set( keyword, value)
Diese Funktion ist die Schnittstelle in das Modul hinein. Innerhalb des Hauptprogrammes werden alle Befehle (Format Item=value) durch jede set funktion der eingebauten Module geschleust. Die Module prüfen ob das Item für sie ein keyword ist und sie handeln müssen. Die benötigte Funktion für diese Prüfing ist im generischen Basisobjekt als Funktion **keyword_match** hinterlegt. 

###Funktion html_create()
Diese Funktion wird vom Hauptrogramm aus aufgerufen wenn ein neuer Webclient sich verbindet. In diesem Fall muss der Client mit aktuellen Daten (idR. mittels Websocket) versorgt werden.

###Funktion html_sysinfo()
Diese Funktion wird augerufen wenn im HTML Client die Systeminfo Seite aufgerufen wird. Hier werden nähere Infos zur Hardware angezeigt (falls gewünsscht). 

###mqtt_json_part()
Diese Funktion liefert einen Teil-JSON zurück der vom Hauptprogramm zu einer MQTT-Nachricht zusammengebaut wird:
**stat/TOPIC2/data JSON-Statement**

###mqtt_has_state()
Ein Schalter der "WAHR" ist wenn das Modul den Statuswert beinhaltet.
Der Satus ist in **obj_mqtt_has_state** gespeichert. Defaultwert wurde in "Base_Generic" auf "false" gesetzt.

###mqtt_state()
Gibt den Status für MQTT als String zurück. Der Wert wird aus **obj_mqtt_state** genommen. 

##interne Funktionen

###keyword_match()



##Interne Variablen


##Einbindung eines Modules
Module beschreiben bzw erzeugen eigene Objekte. Sie werden von der Klasse "Base_Generic" abgeletet, dort sind alle grundlegenden Funktionen bereits vorhanden, viele jedoch als Leerfunktionen. Diese müssen in der abgeleiteten Klasse mit Inhalt gefüllt werden.

Über die Datei "node_settings.h" werden die Module ins Hauptprogramm eingebunden.

Beispiel:

	#ifdef ESP8266SIMPLE
	#define USE_SWITCH_ONOFF
	#include "switch_onoff.h"
	#define HOSTNAME              "nodesimple"
	#define HOST_DISCRIPTION       "Ein ESP8266 Node ohne externe Elemente"
	#define DEBUG_SERIAL_HTML
	#define DEBUG_SERIAL_SENSOR
	#define DEBUG_SERIAL_MQTT
	#define MODULE1_DEFINITION      Switch_OnOff module1;
	#define MODULE1_BEGIN_STATEMENT module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, 2);
	#endif

In der Datei "config.h" wird dann genau ein zu erzeugender Node aktiviert

Auszug:

	// Hier wird der zu erzeugende Node aktiviert
	// Achtung: Es darf nur ein Node ausgewählt werden!
	//#define NODE_AUDIO
	#define ESP8266SIMPLE
	//#define ESP32SIMPLE

## Ausdrücke(Precompilermakros) für "node_settings.h"
1) Vergabe eines Nodenamens (als logischer Einschalter für diesen Node) imBeispiel oben: "ESP8266SIMPLE"

2) Setzen von Hostname und Beschreibung

	#define HOSTNAME                  "nodesimple"
	#define HOST_DISCRIPTION      "Ein ESP8266 Node ohne externe 

3) Einbindung des/der Module

	#include "switch_onoff.h"
	#define MODULE1_DEFINITION                 Switch_OnOff module1;
	#define MODULE1_BEGIN_STATEMENT     module1.begin("sw1", "interne LED", "int_led", "int_led", false, false, 2);
 
Zeile 1: Einbindung der Headerdatei des Modules

Zeile 2: Erzeugung einer Instanz. 

Wichtig: Es wird der Klassenname aus der Headerdatei genommen. Die Instanz muss zwingend mit module1 bis modue6 benannt werden. Die Nummern müssen aufsteigend und lückenlos verwendet werden.

Zeile 3: Das Begin Statement passend zur Deinition des Modules

4) Setzen der Magic Number

	#define MAGICNO                  0

Die Magic Number legt fest wie mit Änderungen in den Preferences beim Neustart umgegangen wird:

Magic Number = 0: Änderungen werden beim Neustart verworfen

Magic Number > 0: Änderungen werden beim Neustart bebehalten.

<b style="color:red">Wichtiger Hinweis</b>Auch bei einem Programmupate bleiben die Preferences erhalten wenn sich dies Nummer nicht ändert!

5) Debuggingeinstellungen (optional)

5.1) Serielles Debugging

	#define DEBUG_SERIAL_HTML
	
Gibt auf der Seriellen Schnittstelle Debuginfos zu HTML Inhalten aus

	#define DEBUG_SERIAL_MODULE
	
Reserviert für Debugausgaben innerhalb der Module

	#define DEBUG_SERIAL_MQTT

Gibt auf der Seriellen Schnittstelle Debuginfos zu MQTT Inhalten aus

	#define DEBUG_SERIAL_WEB

Gibt auf der Seriellen Schnittstelle Debuginfos zu Web Inhalten aus

5.2) Voreinstellungen für das eingebaute Logging

	#define DO_LOG_CRITICAL          true
	
Schaltet das Logging für kritische Ereignisse ein

	#define DO_LOG_SYSTEM              true
	
Schaltet das Systemlogging ein

	#define DO_LOG_RF24              true
	
Schaltet das RF24 Logging ein

	#define DO_LOG_MQTT              true

Schaltet das MQTT Logging ein

	#define DO_LOG_WEB              true

Schaltet das WEB Logging ein

	#define DO_LOG_MODULE              true

Schaltet das MODULE Logging ein

6) MQTT Einstellungen (optional)

	#define MQTT_CLIENT              "wohnzimmernode"
	#define MQTT_TOPICP2             "wohnzimmernode"

Sind diese beiden Zeilen gesetzt wird MQTT eingeschaltet.

Der Name des Mqtt-Clients muss innerhalb der Mqtt Umgebung eindeutig sein.

Das Topics 1 ist in Der Datei "config.h" definiert.
Topic 3 ist der MQTT-Name eines Objektes innerhalb des Nodes.

7) RF24 Einstellungen (optional)

	#define RF24GW_HUB_SERVER        "rpi1.fritz.box"
	#define RF24GW_NO                        102

Sind diese beiden Zeilen gesetzt wird Der RF24 Gateway eingeschaltet.

## Ableitung eines Modules von der Urklasse
Alle Module werden von der Urklasse "Base_Generic" abgeleitet. Diese Vorgehensweise sorgt dafür das in dem abgeleiteten Modul bereits alle Methoden vorhanden sind. Die Methoden müssen jetzt nur noch mit den entsprechenden Inhalten gefüllt werden.

<b style="color:red">Wichtiger Hinweis</b>
Nicht benötigte Module ( wichtig bei Hardwarebezug z.B addressiete Pins ) müssen entweder

a) durch eine passende Precompilerdirektive deaktiviert werden (Beispiel Modul "actor_ledmatrix"):

b) entfernt werden => sehr unpraktisch!!

Umgesetzt wurde Option a)

Jedes Modul bekommt eine Precompilerdirektive als Schlüsselwort zum Einschalten des Moduls (im Beispiel "USE_MYMODUL").

Datei: mymodul.h

	#ifdef USE_MYMODUL
	#include "base_generic.h"

	class MyModul : public Base_Generic {
	public:
	private:
	}

Datei mymodul.cpp

	#include "config.h"
	#ifdef USE_MYMODUL









###Variablen und deren Inhalte###

In die Variable **obj_sensorinfo** wird ein Teil-JSON mit allenunter Sysinfo anzuzeigenden Sensorinfos angelegt.

Syntax: **"sensorinfo1 : Sensordetails linke Spalte # Sensordetails rechte Spalte"**

In der Webseite ist Platz für **"sensorinfo1"** bis **"sensorinfo5"** vorgesehen


