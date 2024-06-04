#Webradio und Medienplayer

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

