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

Alle Datenübertragungen wurden als Websockets realisiert.

##Aufgaben und Schnittstellen der Module
Grundsätzlich ist das Modul dafür verantwortlich:

- Alle Inhalte selbst zu verwalten

- Sparsam mit der CPU Zeit umzugehen (kein delay()!)

- GPIOs selbst zu verwalten (initialisieren, setzen)

- Webinhalte selbst zu verwalten

Zum Hauptprogramm gibt es definierte Schnittstellen. Da jedes Modul von der Klasse "Base_Generic" abgeleitet werden sollte, sind diese Schnittstellen schon als leere Hüllen implementiert und müssen in der abgeleiteten Klasse mit Inhalt gefüllt werden.
###Initialisierung des Modules
Es gibt nur eine Schnittstelle in der alle spezifischen Einstellungen für dieses Modul geschehen müssen: Die Funktion begin(). Diese Funktion hat je Modul einen individuellen Satz von Parametern. 
###Regelmäßige Aufgaben des Modules
**Allgemein**

Benötigt ein Modul Rechenzeit, dann kann diese nur in der Funktion "loop(time_t now)" realisiert werden. Da diese Funktion den aktuellen Zeitstempel übergeben bekommt sind auch zeitlich gesteuerte oder periodische Aktionen realisierbar. 

**Umsetzung**

Während des Laufs wird die loop Funktion regelmäßig aufgerufen. Alle loop Funktionen aller Module und die im Hauptprogramm aufgerufenen periodischen Funktionen werden nach dem "round Robin" Prinzip nacheinander aufgerufen. Erst wenn eine loop Funktion beendet ist folgt die nächste. Es gibt keine zeitliche Begrenzung, dies liegt in der Verantwortung des Moduls! Durch Übergabe des Zeitstempels sind Zeitmessungen (z.B. für Wartezeit) möglich.
###Ausgabe mittels Webseite
Die komplette Webseite ist vordefiniert und wird beim Öffnen der Webseite eines Nodes geladen. Alle inhaltlichen Elemente sind beim Start ausgeblendet und müssen bei Nutzung durch das Modul eingeblendet und mit Inhalt befüllt werden. Dies geschieht mittels JSON Statement das per Websocket transportiert wird.
####Initialisierung der Webseite
**Allgemein**

Wird die Webseite eines Nodes geöffnet muss der Inhalt der Webseite komplett neu aufgebaut werden. Dazu muss jedes Modul seine Initialisierungsdaten liefern, die vom Hauptprogramm über die Funktion "html_init()" abgefragt werden wenn die Variable "html_init_set" auf true gesetzt ist. Das geschieht jedes mal wenn ein neuer Webclient sich verbindet. In diesem Fall muss der Client mit aktuellen Daten (mittels Websocket) versorgt werden.

**Umsetzung**

Es kann je Modul entschieden werden ob es sinnvoller ist die Websocketnachricht jeweils neu aufzubauen oder sie in Form eines Strings abzulegen. 

####Update der Webseite
**Allgemein**

Ein Update einzelner Elemente der Webseite wird durch das Modul veranlasst. Wird z.B ein Schalter betätigt oder ein neuer Meßwert eingelesen dann schickt das Modul eigenständig die Daten an die Weboberfläche.

**Umsetzung**

Das Modul setzt die Variable "html_update_set" auf "true". Danach wird vom Hauptprogramm die Funktion "html_update(String& _html_update)" abgefragt. Innerhlab der aufgerufenen Funktion wird der String "_html_update" um einen Teil-JSON erweitert. Die Rücksetzung von "html_update_set" auf "false" erfolgt durch das Hauptprogramm.

####Zulieferung zu den Systeminfos
**Allgemein**

Es besteht die Möglichkeit im HTML Client die Systeminfo Seite mit Infos z.B. zur verwendeten Hardware, genutzter Ports, etc zu befüllen. Dazu muss die Funktion "html_info()" gefüllt werden. Zusätlich muss die Variable "html_info_set" auf "true" gesetzt werden.

**Umsetzung**

Die Systeminfoseite wird auf der Webseite dynamisch mittels Javascript erstellt. Um für ein Modul einen oder mehrere Eintäge auszugeben muss zunächst eine Headerzeile erzeugt werden. Dies geschieht durch das item "tab_head_xyz" (wobei xyz durch einen modulspeziefischen Ausdruck ersetzt wird) und einen value der in der Systeminfotabelle angezeigt wird.

Danach folgt für jede Zeile ein item "tab_lineX_xyz" (Dabei ist "X" durch die Zeilennummer zu ersetzen und "xyz" durch den modulspeziefischen Ausdruck) und ein value. Das Value besteht aus einem Tabellenlabel (linke Spalte), dem Trennzeichen "#" und einem dazugehörigen Wert (rechte Spalte)

Beispiel:

	void MyModule::begin() {
	...
	  html_info_set = true;
	...
	}
	
	void MyModule::html_info(String& _html_info) {
	  _html_info +=  String("\"tab_head_18b20\":\"Sensor\"")+
	  _html_info += String(",\"tab_line1_18b20\":\"HW 18B20:#GPIO: ")+String(PIN_18B20)+String("\"")+
	  _html_info += String(",\"tab_line2_18b20\":\"Resolution:# ")+String(RESOLUTION)+String("\"")
	}
	
####Zulieferung zum eingebauten Hilfesystem
**Allgemein**

Das eingebaute Hilfesystem kann auf der Webseite des Nodes im Reiter Konsole gestartet werden. Dazu wird in der Befehlszeile "?" oder "help" eingegeben.
Jedes Modul kann optional einen Hilfetext anbieten.

**Umsetzung**

Die Implementierung erfolgt über die "set()" Funktion. Hier wird auf das Kommando "?" und "help" reagiert.

	if ( _cmnd == String("?") || _cmnd == String("help")) {

Für jede Ausgabezeile muß zunächst die Funktion

	json_stat_header(stat_str);

aufgerufen werden. Danach folgt der auszugebende Text.
Zum Abschluß <b style="color:red">MUSS</b> hier der Rückgabewert auf "false" gesetzt werden.

	      retval = false;
	    }

Das komplette Beispiel aus der Klasse "Switch_OnOff":

	    if ( _cmnd == String("?") || _cmnd == String("help")) {
	      json_stat_header(stat_str);
	      stat_str += String("\"Schalten:\"");
	      json_stat_header(stat_str);
	      stat_str += String("\"<") + keyword + String(">:<1|ein|0|aus|2|umschalten|<1..6>h>\"");
	      if (slider_used) {
	        json_stat_header(stat_str);
	        stat_str += String("\"Dimmen:\"");
	        json_stat_header(stat_str);
	        stat_str += String("\"") + slider_keyword + String(":<0 ... 100>\"");
	      }
	      retval = false;
	    }

###Ausgabe mittels MQTT
Für die Datenübertragung über MQTT sind 2 Nachrichtentypen vorgesehen.

* Sammelnachricht im JSON Format

In dieser Nachricht werden alle zu übermittelnden Werte als JSON übermittelt. In jedem Modul wird ein Teil-JSON angelegt.

* Statusnachricht

Zusätzlich kann ein Wert bestimmt werden, der den Status des Objektes (z.B. in FHEM) anzeigt. Es gibt nur einen Status für den kompletten Node!
###MQTT
MQTT ist implementiert, muß aber auf Nodeebene eingeschaltet werden.
Die verwendeten Topics sind an TASMOTA und FHEM angelehnt. Die Struktur sieht dabei wie folgt aus:

	tele/my/individual/topic/thisnode

In der Software sind diese als 
####MQTT Telemetriedaten
**Allgemein**

todo
####MQTT Statusdaten
todo
###Eingehende Kommandos
**Allgemein**

Kommandos können über unterschiedliche Kanäle (MQTT, Web, Websocket,...) eingehen, werden jedoch immer gleich behandelt. Jedes Modul besitzt einen Kommandointerpreter in Form der Funktion "set()". Hier wird durch das Modul entschieden ob ein Kommando für dieses Modul relevant ist und welche Maßnahmen dann ausgeführt werden.

**Umsetzung**

Diese Funktion ist die Schnittstelle in das Modul hinein. Innerhalb des Hauptprogrammes werden alle Befehle (Format Item=value) durch jede set funktion aller eingebauten Module geschleust. Jedes Modul prüft eigenverantwortlich ob das Item für dieses Modul ein Keyword ist und ob das Modul handeln muss. Die benötigte Funktion für diese Prüfing ist im generischen Basisobjekt als Funktion **keyword_match** hinterlegt. Die Funktion:

	bool set(const String& _cmnd, const String& _val);

wird vom Hauptprogramm nacheinander in allen Modulen mit dem gesendeten Kommando ("_cmnd") und dem dazugehörigen Wert ("_val") aufgerufen. Das Modul prüft ob das Kommando durch dieses Modul verarbeitet wird. Falls eine Verarbeitung erfolgt gibt die Funktion "set" den Wert "true" zurück, andernfalls wird "false" zurückgegeben.
 

**Rückgabewert:**

"false" wenn das "item" nicht in diesem Modul ausgewertet wird, sonst "true"

####Kommandos per Webseite
todo
####Kommandos per MQTT
todo


##Programmierhinweise
