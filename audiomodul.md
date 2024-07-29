#Audiomodul
##Bedienung
###Allgemein:
Das Modul startet im Zustand ausgeshaltet- Das Einschalten erfolgt durch Drehung des Lautstärkereglers in Richtung "lauter", Ausschalten erfolgt durch den Lautstärkeregler durch drehen in Richtung "leiser", bei Lautstärke 0 wird ausgeschaltet..
Durch Druck auf den Lautstärkeregler sind weitere Funktionen verfügbar. Dabei wird zwischen kurzem Druck (kleiner 1 Sekunde) und langer Druck (größer 1 Sekunde) unterschieden. Wird der Grundzustand verlassen und ca. 1 Minute nichts gemacht wird automatisch in den Grundzustand zurückgeschaltet.

###Modus
Durch einen langen Druck wird der Modus umgeschaltet zwischen Radio, Mediaplayer, Bluetoothlautsprecher und Einstellungen.

1* kurz drücken: Aktiviert den Modus.

###Modus: Einstellungen
Grundzustand: Höhen einstellen

1* kurz drücken: Bässe einstellen

2* kurz drücken: Beenden 

###Modus: Radio
Grundzustand: Der aktuelle Sender wird gespielt und angezeigt.

1* kurz drücken: Senderwahl. Durch kurzen Druck auf den Lautstärkeregler wird auf Senderwahl umgeschaltet. Durch drehen wird der Sender gewechselt.

2* kurz drücken: Rückkehr in den Grundzustand
 
###Modus: Mediaplayer
Gndzustand: Wiedergabe, das aktuelle Stück wird gespielt und angezeigt.

1* kurz drücken: Albenauswahl. Durch drehen wird das gewünschte Album ausgewählt.

2* kurz drücken: Titelwahl. Durch drehen wird der gewünshte Titel ausgewählt

3* kurz drücken: Auswahl bestätigt => Grundzustand.

##Einschränkungen
Das Webradio und/oder der Medienplayer wurden von mir auf einem ESP32-S3 konzipiert und getestet. Alle anderen ESPs sind ungetestet und werden auch teilweise wegen der verwendeten Bibliotheken nicht funktionieren.

##Technik
###Bauteile und deren Verbindung beim ESP32S3:

| ESP32-S3|MAX08357|SD Adapter|Rotary Encoder|Display|Kabelfarbe|
|--|--|--|--|--|--|
| 4 | LRC |  |  |  | braun|
| 5 | BCLK |  |  |  | rot|
| 6 | DIN |  |  |  | gelb|
| 8 |  | CS |  |  | lila |
| 11 |  | MOSI |  |  | orange |
| 12 |  | SCK |  |  | grün |
| 13 |  | MISO |  |  | gelb |
| 35 |  |  | Key |  | lila |
| 36 |  |  | S1 |  | blau |
| 37 |  |  | S2 |  | grau |
| 9 |  |  |  | DC | lila |
| 10 |  |  |  | CS | grau |
| 11 |  |  |  | SDA | orange |
| 12 |  |  |  | SCL | grün |
| RST |  |  |  | RST | blau |
| GND | GND | GND | GND | GND | schwarz |
| 3V3 | VIN |  | 5V | VCC | weiss |
| 5V |  | VCC |  |  | weiss |

![ESP32S3](/home/norbert/projekte/ESPNode_audiomodul/doc/pic/esp32s3.jpg  "ESP32S3")

![MAX98357](/home/norbert/projekte/ESPNode_audiomodul/doc/pic/max98357a.png  "MAX98357")

###Ablaupläne:
Hier wird nur das Audiomodul betrachtet! Alle hier genannten Funktionen sind Funktionen des Audiomoduls.
####Einschalten des Nodes
0. Einschalten des Nodes
1. begin()
2. Initialisierung abgeschlossen alle Applikationen aus.
####Einschalten am Lautstärkeregler
0. Drehen des Lautstärkereglers
1. Rotarymodul erkennt Drehung und setzt rotary.changed()
2. loop() => Klickstream wertet die Veränderungen aus.
	* modus = off und rotary.value >5
		* audio_set_modus(last_modus)
3. Rotary entsprechend der App. setzen
4. audio_all_apps_off();
5. audio_[APP]_on();
	* connecttohost();
6. audio_media_disp_init();
7. audio_media_web_init();
####Aufruf der Webseite(alle App. aus)
0. Browser verbindet sich
1. html_create_json_part();
2. 


