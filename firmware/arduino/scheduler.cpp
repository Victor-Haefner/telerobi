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

/** Command syntax
AiSsDdOo # set Actor i Speed s Duration d Offset o
LiRrGgBb # set LED i RGB (NOT YET IMPLEMENTED)

schema: byte pairs, 1 byte label and 1 byte value
first byte, A for actuator, L for LED
second byte is index 0 to 255
then follow parameters
*/

void Scheduler::processCommand(String Cmd) {
  Cmd = Cmd.substring(4); // remove 'cmd:', maybe dont send it from the esp
  int N = Cmd.length()/2;

  Command command;
  for (int i=0; i<2*N; i+=2) {
    char label = Cmd[i];
    byte param = Cmd[i+1];
    if (label == 'A') command.actID = param;
    if (label == 'S') command.dir = param;
    if (label == 'D') command.duration = param;
    if (label == 'O') command.start = millis() + param;
  }
  command.stop = command.start + command.duration;
  actuators[command.actID]->push(command);
}




