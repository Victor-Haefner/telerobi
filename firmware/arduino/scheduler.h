#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

class Robot;

class Command {
  public:
    int actID = -1;
    int dir = 0;
    long start = 0;
    long stop = 0;
    bool started = false;
};

class CmdQueue {
  public:
    Command commands[10];
    int lastAdded = -1;
    long lastStop = 0;
    long currentStarted = 0;
    bool active = false;
    void push(const Command& cmd);
};

class Scheduler {
  private:
    Robot* robot = 0;
		CmdQueue* actuators[10] = {0};

  public:
    Scheduler(Robot* robot);
    ~Scheduler();

    void processSerialInput(char c);
    void processCommand(String Cmd);
    void update();
};

#endif //SCHEDULER_H