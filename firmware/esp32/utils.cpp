#include "utils.h"

bool strContains(String str, String substr) {
  int j = 0;
  for (int i = 0; i < str.length(); i++) {
    if (str[i] == substr[j]) {
      j++;
    } else j = 0;
    if (j == substr.length()) return true;
  }
  return false;
}

SplitString::SplitString() {}

SplitString strSplit(String str, char delim) {
  SplitString splitString;
  splitString.N = 0;
  int j = 0;
  String aggregate;
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (c == delim) {
      splitString.strings[j] = aggregate;
      j++;
      splitString.N++;
      aggregate = "";
      if (j >= 20) return splitString; // TODO show error to user
    } else {
      aggregate += c;
    }
  }
  splitString.strings[splitString.N] = aggregate;
  splitString.N++;
  return splitString;
}


SplitString strSplit(String str, char delim, char delim2) {
  SplitString splitString;
  splitString.N = 0;
  int j = 0;
  String aggregate;
  char lc = 0;
  for (int i = 0; i < str.length(); i++) {
    char c = str[i];
    if (lc == delim && c == delim2) {
      splitString.strings[j] = aggregate;
      j++;
      splitString.N++;
      aggregate = "";
      lc = 0;
      if (j >= 20) return splitString; // TODO show error to user
    } else if (c != delim || str[i+1] != delim2) {
      aggregate += c;
    }
    lc = c;
  }
  splitString.strings[splitString.N] = aggregate;
  splitString.N++;
  return splitString;
}


Pair::Pair(String k, String v) : key(k), value(v) {}
Pair::Pair() {}


Map::Map() {}

void Map::set(String key, String val) {
  if (N >= 10) return;
  data[N] = Pair(key, val);
  N++;
}

String Map::get(String key) {
  for (int i=0; i<10; i++) {
    if (data[i].key == key) return data[i].value;
  }
  return "";
}


