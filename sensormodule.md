##Module##
###Grundlagen###
Im Hauptprogramm ist die Einbindung von 6 Modulen vorgesehen. Die Modulnummer wird aufsteigend von 1 bis 6 verwendet.

Auf der Weboberfläche sind 4 Schalter vorgesehen. Diese Schalter haben den Namen "sw1" bis "sw4".

Auf der Weboberfläche sind 3 Schieberegler vorgesehen. Diese Schieberegler werden idR. zusammen mit Schaltern verwendet. Siehe dazu "switch_onoff". Die Schieberegler haben die Nummern "1" bis "4".

Auf der Weboberfläche sind 4 Ausgabezeilen vorgesehen. Diese Felder haben den Namen "out1" bis "out4".

Daneben können auf der Webseite noch weitere Objekte angelegt werden, diese werden hier jedoch nicht beschrieben.

###Einbindung eines Modules###
Module beschreiben bzw erzeugen eigene Objekte. Sie werden von der Klasse "Base_Generic" abgeletet, dort sind alle grundlegenden Funktionen bereits vorhanden, viele jedoch als Leerfunktionen. Diese müssen in der abgeleiteten Klasse mit Inhalt gefüllt werden.

Diese Module werden über die Datei **"Node_settings.h"** eingebunden.

Zur Einbindung innerhalb der Datei "Node_settings.h" gilt folgende Syntax:

**Erzeugung einer Instanz**
Für jede Klasse muss eine oder mehrere Instanzen erzeugt werden. Diese Instanzen haben festgelegte Namen (module1 ... module6). Es ist wichtig diese Namen einzuhalten, da im Hauptprogramm auf die Instanzen mit diese Namen zugegriffen wird. Die Instanziierung erfolgt in der Datei  **"Node_settings.h"** über eine Precompiler Direktive. Beispiel:

	//Allgemein:
	#define MODULE<Modulnummer>_DEFINITION       <Klassenname> module<Modulnummer>;
	//Beispiele:
	#define MODULE1_DEFINITION       Switch_OnOff module1;
	#define MODULE2_DEFINITION       Sensor_18B20 module2;

**Initialisierung der Instanz**
Zur Initialisierung der Instanz wird die Funktion "begin();" aufgerufe. Diese Funktion kann je nach Einsatz und Aufgabe unterschiedliche Parameter erfordern. Damit die Funktion universell eingebaut werden kann, erfolgt ihr "Einbau" in der Datei  **"Node_settings.h"** über eine Precompiler Direktive. Beispiel:

	//Allgemein:
	#define MODULE<Modulnummer>_BEGIN_STATEMENT   module<Modulnummer>.begin(Parameter1...N);
	//Beispiele:
	#define MODULE1_BEGIN_STATEMENT  module1.begin("sw1", "int. LED", "led", "led", false, false, 2);
	#define MODULE2_BEGIN_STATEMENT  module2.begin("out1","Temperatur","Temperatur");


###Erstellung eines neuen Modules / einer neuen Klasse###

Für jedes neue Modul werden jeweils eine Headerdatei und eine CPP-Datei angelegt. Innerhalb dieser Dateien wird eine Klasse beschrieben die in den ESPNode eingebaut wird. Diese Klasse kann natürlich Objekte nutzen die wiederum auf anderen Klassen basieren. Es ist darauf zu achten das sich in der Headerdatei nur Deklarationen befnden. Definitionen und Initialisierungen kommen in die CPP Datei.

Die Modulklasse wird direkt oder indirekt von der Klasse "Base_Generic" abgeleitet. Beispiel:

	class Sensor_18B20 : public Base_Generic {
	
	public:
	
	private:
 	
	};

Das hat den Vorteil das alle benötigten Funktionen bereits vorhanden sind und dadurch sichergestellt wird das der Compiler feherfrei läuft. Einige der in der Klasse "Base_Generic" definierten Funktionen sind jedoch lerr und üssen in der abgeleiteten Funktion mit sinnvollem Inhalt gefüllt werden.

####Wichtiger Hinweis####
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

###Funktionen und Aufgaben###

####Funktion "begin()"####
Hier wird das Objekt initialisiert dabei ist naturgemäß die Anzahl der Parameter variabel. Jedliche grundlegende Konfiguration muss ier erfolgen. Im weiteren Programm gibt es dazu keine Möglichkeit mehr.
Der Einbau in den ESPNode mittels Precompilerdirektive wurde bereits behandelt.

###Funktion "start_measure()"###

Innerhalb dieser Funktion muss bei einem Sensor zwingend folgendes Programmiert werden:

1) Alles nötige zum Auslesen des oder der Messwerte.

2) Befüllung folgender Variablen (vordefiniert in "Sensor_Generic")

2.a) **"obj_html_stat_json"**
Hier muss ein kompletter gültiger JSON eingetragen mit dem/den aktuellen Messwert(en) eingetragen werden.

Syntax: **{"Einbauplatz"  : "Anzuzeigender Schriftzug"}**

Beispiel: **{"out1" : "Temperatur 21 °C"}**

2.b) **"obj_mqtt_json"**
Hier muss ein Teil-JSON für alle Messwerte zur Übertragung mittels MQTT eingetragen werden. **Achtung:** Sollen die Messwerte anschliessend in eine Datenbank eingetragen werden, dann sind Einheiten (z.B. °C) sehr hinderlich!

Syntax: **{"Messwertlabel"  : "Wert"}**

Beispiel: **{"Temperatur" : 21.2 }**


###Funktion set( keyword, value)###
Diese Funktion muss für jedes Modul gefüllt werden. Innerhalb des Hauptprogrammes werden alle Befehle (Format Item=value) durch jede set funktion der eingebauten Objekte geschleust. Die Objekte prüfen ob das Item für sie ein keyword ist und sie handeln müssen. Die benötigte Funktion für diese Prüfing ist im generischen Basisobjekt als Funktion **keyword_match** hinterlegt. Nach Beendigung der Funktion müssen auch hier die Variablen **"obj_html_stat_json"** und  **"obj_mqtt_json"** gefüllt sein.

###Funktion "html_create_json_part(json)"###
Stellt alle Konfigurationsdaten (als JSON Teilstring) bereit die für dieses Modul beim Aufruf der Webseite benötigt werden
Ein Modul aktiviert seine Anzeige selbst indem der anzuzeigende Wert in das passende HTML Feld geschrieben wird. Dazu muss die Funktion "html_create_json_part" gefüllt werden. Durch das hier erzeugte JSON wird festgelgt was wie auf der HTML Seite angezeigt wird. Dabei gibt es folgende Möglichkeit:

a) Reiner Ein-Aus Schalter

Codesegment: **"sw1 : 0 , sw1_label : Lichtschalter , sw1_format : x"**

b) Schalter mit Regler

Codesegment: **"sw1 : 0 , sw1_label : Lichtschalter , sw1_format : x, slider1 : 1 , slider1label : Helligkeit , slider1val : 54"**

c) Jede andere Kombination die nach Änderung der HTML Seite benötigt wird

Beispiele dazu in der Datei switch_onoff.h/cpp




###Variablen und deren Inhalte###

In die Variable **obj_sensorinfo** wird ein Teil-JSON mit allenunter Sysinfo anzuzeigenden Sensorinfos angelegt.

Syntax: **"sensorinfo1 : Sensordetails linke Spalte # Sensordetails rechte Spalte"**

In der Webseite ist Platz für **"sensorinfo1"** bis **"sensorinfo5"** vorgesehen










