#ifndef _AUDIODISPLAY_H_
#define _AUDIODISPLAY_H_

/**
 * @brief Generische Anzeige für das Audiomodul, nur als Elternklasse verwenden
 */
class AudioDisplay {
public:

  uint8_t num_lines = 0;
  String displaystr[3];
  uint8_t split4display(String& in_str);
  // int splitStr(const char* inStr, int startPos, int maxLen, char* resultStr);
  //void show_text(String& in_text, int posx, int posy, uint16_t color);
  //void show_text_s2(const char* mytext, int posx, int posy, uint16_t color);
  //void showRadioStation();
  String replaceNonAscii(String inputString);

  /// @brief Teilt einen String in Teilstrings auf
  /// Bei der Aufteilung des Strings gibt es folgende Regeln:
  /// Der String wird - wenn möglich - bei einem Leerzeichen aufgetrennt.
  /// Das Ergebnis hat eine Länge von mindestens <minLen>
  /// Das Ergebnis hat maximal eine Länge von <maxLen>
  /// Das Ergebnis startet an Zeichen <StartAT>
  /// @param data Der Quellstring
  /// @param startAt Die Position des ersten Zeichens vom Ergebnisstring im Quellstring
  /// @param minLen Minimale Länge des Ergebnisstrings
  /// @param maxLen Maximale Länge des Ergebnisstrings
  /// @return Die Position des letzten benutzten Zeichens im Quellstring
  int getPartStringEnd(String data, int startAt, int minLen, int maxLen);

  /// @brief Die Loop Funktion wird regelmäßig aufgerufen
  /// @param now Der Unix Zeitstempel
  void loop(time_t now);


};

#endif // _AUDIODISPLAY_H_
