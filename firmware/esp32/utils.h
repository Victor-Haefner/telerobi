#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"

bool strContains(String str, String substr);

struct SplitString {
  int N = 0;
  String strings[20];

  SplitString();
};

SplitString strSplit(String str, char delim);
SplitString strSplit(String str, char delim, char delim2);

struct Pair {
  String key;
  String value;

  Pair(String k, String v);
  Pair();
};

struct Map {
  Pair data[10];
  int N = 0;

  Map();

  void set(String key, String val);
  String get(String key);
};

struct Settings {
  String serverIPasString;
  String serverPath;
  String botName;
  String ssid;
  String password;
  String botModel;
};

struct State {
  bool needsSSIDConnect;
  bool doCheckWifiStatus;
  bool disconnectingWifi;
  String state;
  bool doStream;
  String connected_ssid;
};

#endif // UTILS_H


