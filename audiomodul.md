#Audiomodul
##Bedienung
###Allgemein:
Das Modul startet im Zustand ausgeshaltet- Das Einschalten erfolgt durch Drehung des Lautstärkereglers in Richtung "lauter", Ausschalten erfolgt durch den Lautstärkeregler durch drehen in Richtung "leiser".
Durch Druck auf den Lautstärkeregler sind weitere Funktionen verfügbar. Dabei wird zwishen kurzem Druck (kleiner 1 Sekunde) und lange Druck (größer 1 Sekunde) unterschieden. Wird der Grundzustand verlassen und ca. 1 Minute nichts gemacht wird automatisch in den Grundzustand zurückgeschaltet.

###Modus
Durch einen langen Druck wird der Modus umgeschaltet zwischen Radio, Mediaplayer, Bluetoothlautsprecher und Einstellungen.

1* kurz drücken: Aktiviert den Modus.

###Einstellungen
Grundzustand: Höhen einstellen

1* kurz drücken: Bässe einstellen

2* kurz drücken: Beenden 

###Radio
Grundzustand: Der aktuelle Sender wird angezeigt.

1* kurz drücken: Senderwahl. Durch kurzen Druck auf den Lautstärkeregler wird auf Senderwahl umgeschaltet. Durch drehen wird der Sender gewechselt.

2* kurz drücken: Rückkehr in den Grundzustand
 
##Mediaplayer
Gndzustand: Wiedergabe, das aktuelle Stück wird angezeigt.

1* kurz drücken: Albenauswahl. Durch drehen wird das gewünschte Album ausgewählt.

2* kurz drücken: Titelwahl. Durch drehen wird der gewünshte Titel bzw. alle Titel ausgewählt

3* kurz drücken: Auswahl bestätigt => Grundzustand.

##Einschränkungen
Das Webradio und/oder der Medienplayer wurden von mir auf einem ESP32-S3 konzipiert und getestet. Alle anderen ESPs sind ungetestet und werden auch teilweise wegen der verwendeten Bibliotheken nicht funktionieren.

##Bauteile und deren Verbindung beim ESP32S3:

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

