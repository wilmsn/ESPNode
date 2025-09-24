# Modul Switch OnOff
In diesem Dokument werden Details zum Modul Switch_OnOff beschrieben.
##Grundlagen
Das Modul Switch_OnOff beinhaltet folgende Elemente

- Schalter (ein/aus) ohne Hardwarebezug
- Schalter (ein/aus) mit Hardwarebezug auf 1 GPIO
- Schalter (ein/aus) mit Hardwarebezug auf 2 GPIO
- Schalter (ein/aus) mit Regler ohne Hardwarebezug
- Schalter (ein/aus) mit Regler mit PWM auf 1 GPIO

##Auswahl und Initialisierung

##Neu: Zustandsdiagramm für die letzten 24 Stunden
Auf der HTML Seite wird in einem Diagramm der Zustand aller Schalter für die letzten 24 Stunden angezeigt. 
Auflösung 5 Min: 12 Punkte je Stunde => 24 * 16bit => 48 byte 
Auflösung 10 Min: 6 Punkte je Stunde => 24 * 8bit => 24 byte 
Auflösung 10 Min Ringspeicher: 6 Punkte je Stunde => 132 bit => 17 byte 

VerwendeteMethode Array mit 24 Plätzen und einem gespeicherten Wert alle 10 Minuten. Innerhalb dieser 8 Minuten wird der Wrt auf ON gesetzt sobald der Schalter einmal ein war!


##Neu: Timerfunktion 1...6 Stunden einschalten
Die Timerfunktion schaltet für ein bis sechs Stunden ein, ein timergestütztes Ausschalten (für X Stunden aus , dann wieder ein) ist nicht vorgesehen. Die Timerfunktion kann beliebig oft ausgelöst werden, auch wenn der Timer bereits aktiv ist. In diesem Falle wird der Timer mit dem neuen Wert neu gestartet.

####Starten des Timers 
Über die Set-Funktion: <Name des Elementes>:<Xh> (mit X 0 1 bis 6>
