#include "scheduler.h"
#include "robot.h"

void CmdQueue::push(const Command& cmd) {
  lastAdded++;
  lastAdded = lastAdded%10;
  commands[lastAdded] = cmd;
  lastStop = max(lastStop, cmd.stop);
  active = true;
}

// ---- scheduler ----

Scheduler::Scheduler(Robot* b) : robot(b) {}
Scheduler::~Scheduler();

void Scheduler::update() {
  long t = millis();

  for (int i=0; i<10; i++) { // for each actuator command queue
    CmdQueue* queue = actuators[i];
    if (queue == 0) continue;
    if (!queue->active) continue;

    if (queue->lastStop <= t) {
      queue->active = false;
      robot->setActuator(i, 0); // safeguard stop
    }

    for (int j=0; j<10; j++) { // for each command
      Command& cmd = queue->commands[j];
      if (cmd.actID == -1) continue;
      if (cmd.start > t) continue; // not yet ment to start

      if (cmd.start < queue->currentStarted) { // another command has taken precedence, disable this one
        cmd.actID = -1;
        continue;
      }

      if (cmd.started) { // has been activated
        if (cmd.start != queue->currentStarted) cmd.actID = -1; // another command has taken over, disable this one
          
        if (cmd.stop <= t) { // needs to stop!
          robot->setActuator(i, 0); 
          cmd.actID = -1;
        }
        continue;
      }

      if (cmd.stop <= t) { // too late to start that one
        cmd.actID = -1;
        continue;
      }

      // start command
      robot->setActuator(i, cmd.dir);
      queue->currentStarted = cmd.start;
      cmd.started = true;
    }
  }
}

void Scheduler::processCommand(String Cmd) { // TODO
  char toSend[Cmd.length()+1];
  Cmd.toCharArray(toSend,Cmd.length()+1);
  
  if (Cmd.substring(4,5) == "S") robot->drive("S");
  if (Cmd.substring(4,5) == "F") robot->drive("D");
  if (Cmd.substring(4,5) == "B") robot->drive("L");
  if (Cmd.substring(4,5) == "L") robot->drive("R");
  if (Cmd.substring(4,5) == "R") robot->drive("B");
}

const int Nbuff = 1024;
char buff[Nbuff];
int pointer = 0;

void Scheduler::processSerialInput(char c) {
  //Serial.write(c);               // print it out the serial monitor
  buff[pointer] = c;
  pointer++;
      
  if (c == '\n') {                // if the byte is a newline character
    buff[pointer - 1] = 0;
    String data = String(buff);
    processCommand(data);
    pointer = 0;
  } else {
    pointer = pointer % (Nbuff - 1);
  }
}