#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

class Robot;
class Configuration;

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

class Command {
  public:
    byte actID = -1;
    int speed = 0;
    int duration = 0;
    unsigned long start = 0;
    unsigned long stop = 0;
    bool started = false;
};

class CmdQueue {
  public:
    const static int N = 4;
    Command commands[N];
    int lastAdded = -1;
    unsigned long lastStop = 0;
    unsigned long currentStarted = 0;
    bool active = false;
    void push(const Command& cmd);
};

class Scheduler {
  private:
    const static int timeMultiplier = 100; // from byte to ms, for the duration passed in commands
    const static int speedMultiplier = 2;
    Configuration* config = 0;
    Robot* robot = 0;
		CmdQueue* actuators[10] = {0};

  public:
    Scheduler(Robot* robot, Configuration* config);
    ~Scheduler();

    void processSerialInput(byte c);
    void processCommand(string Cmd);
    void update();

    string genActorCommand(byte i, byte s, byte d, byte o);
};

#endif //SCHEDULER_H