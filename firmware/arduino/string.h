#ifndef STRING_H
#define STRING_H

#include <Arduino.h>

class string {
  public:
    int length = 0;
    byte* data = 0;

    string(const char* d, int N = -1); 
    string(const string& other);
    ~string(); 

    //string& operator=(string&& other) noexcept;
    void copy(const string& other);
    string& operator=(const string& other);
    byte operator [](int i) const;
    byte & operator [](int i);

    void print();
};

#endif //STRING_H