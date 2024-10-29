#Technische Dokumentation
Dieses Dokument bezieht sich auf das Hauptprogramm (die Firmware), zu den Modulen gibt es ein gesondertes Dokument.
##Include/header Dateien
###Inhalte

###Reihenfolge
Im folgenden werden nur die Headerdateien des Projektes betrachtet, keine Systemheaderdateien.

|Ebene 0|Ebene1|Ebene 2|Ebene 3|Ebene 4|
|--|--|--|--|--|
|main.h|common.h|config.h|Node_settings.h|[Modul].h|
|  |Systen Header|  |  |  |
|  |secrets.h|  |  |  |
|mqtt.h|common.h|  |  |  |  |

##Webseite
Die Web-Applikation des ESPNodes ist ein Onepager, d.h. die Webseite mit allen logischen Blöcken wird beim Start geladen. Danach werden alle Blöcke, die gerade nicht benötigt werden, ausgeblendet.
Für die Inhaltliche Befüllung gibt es 2 Methoden:

1) Nachladen von Daten per HTTP Request

	 $.getJSON( "sysinfo1", function( data ) {
	    $.each( data, function( key, val ) {
	      prozessJS( key, val );
	    });
	  });

Diese Methode wird nur für die Systeminfo verwendet. 
Auf Serverseite implementiert in "main.cpp" in den Funktionen "mk_sysinfo1" bis "mk_sysinfo3".

Auf der Clientseite implementiert in "index.html". 

2) Websockets


###Logische Blöcke
Innerhalb der Applikation gibt es folgende logishe Blöcke:
####Systeminfo
Nutzen: Zeigt Informationen zur Systemumgebung an.
Vorgehensweise: Bei der Anzeige dieses Blocks werden die Daten zum System dynamisch als JSON nachgeladen. 

