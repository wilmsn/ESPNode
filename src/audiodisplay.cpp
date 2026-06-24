#include "config.h"
#include "common.h"
#include "audiodisplay.h"


void AudioDisplay::loop(time_t now) {}

uint8_t AudioDisplay::split4display(String& in_str) {
  uint8_t startAt = 0;
  uint8_t splitAt = 0;
  uint8_t lineNo = 0;
  uint8_t strLen = in_str.length();
  uint8_t retval;
//  Serial.println(String("split4display: #") + String(in_str) + String("# ") + String(strLen));
  displaystr[0] = "";
  displaystr[1] = "";
  displaystr[2] = "";
  if (strLen < FONT1_MAX_CHAR_PER_LINE) {
    displaystr[0] = in_str;
    retval = 1;
  } else {
    while (splitAt < strLen && lineNo < 3) {
      splitAt = getPartStringEnd(in_str, startAt, FONT2_MIN_CHAR_PER_LINE, FONT2_MAX_CHAR_PER_LINE);
      displaystr[lineNo] = in_str.substring(startAt,splitAt);
      startAt = splitAt + 1;
      if (lineNo < 2) {
        retval = 2;
      } else{
        retval = lineNo+1;
      } 
      lineNo++;
    }
  }
  return retval;
}

String AudioDisplay::replaceNonAscii(String inputString) {
  String result = "";
  for (size_t i = 0; i < inputString.length(); i++) {
    char c = inputString.charAt(i);
    if (c >= 0 && c <= 127) {
      result += c;
    } else {
      // Hier können Ersetzungen für bestimmte nicht-ASCII-Zeichen vorgenommen werden
      String cmpstr;
      bool replaced = false;
      cmpstr = String("ä");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ae");
        replaced = true;
      } 
      cmpstr = String("ü");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ue");
        replaced = true;
      } 
      cmpstr = String("ö");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("oe");
        replaced = true;
      } 
      cmpstr = String("Ä");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("Ae");
        replaced = true;
      } 
      cmpstr = String("Ü");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ue");
        replaced = true;
      } 
      cmpstr = String("Ö");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("oe");
        replaced = true;
      } 
      cmpstr = String("ß");
      if (inputString.charAt(i) == cmpstr.charAt(0) && inputString.charAt(i+1) == cmpstr.charAt(1)) {
        i++;
        result += String("ss");
        replaced = true;
      } 
      // Füge weitere Ersetzungen hinzu, falls nötig
      if ( ! replaced ) result += '?'; // Oder ein anderes Ersatzzeichen
    }
  }
  return result;
}

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
int AudioDisplay::getPartStringEnd(String data, int startAt, int minLen, int maxLen) {
  int retval = -1;
  int maxLenData = data.length()-1;
  for ( int i = startAt + minLen; i <= startAt + maxLen && i < maxLenData; i++ ) {
    if ( data.charAt(i) == ' ' ) {
      retval = i;
    }
  }
  if (retval == -1) retval = startAt + maxLen - 1;
  return retval;
}
