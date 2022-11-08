#include "string.h"

string::string(const char* d, int N) {
  if (!d || N == 0) return;
  if (N == -1) for (const char* i=d; *i != 0; i++) length++;
  else length = N;
  data = new byte[length];
  if (data == 0) Serial.println("Mem fail 1");
  memcpy(data, d, length);
}

string::string(const string& other) {
  copy(other);
}

string::~string() {
  if (data) delete[] data;
}

string& string::operator=(const string& other) {
    copy(other);
    return *this;
}

void string::copy(const string& other) {
    length = other.length;
    data = new byte[length];
    if (data == 0) Serial.println("Mem fail 2");
    memcpy(data, other.data, length);
}

byte string::operator [](int i) const {return data[i];}
byte & string::operator [](int i) {return data[i];}

void string::print() {
  for (int i=0; i<length; i++) Serial.print(char(data[i]));
  Serial.println("");
}