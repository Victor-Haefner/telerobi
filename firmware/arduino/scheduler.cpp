#include "scheduler.h"
#include "robot.h"
#include "config.h"

void CmdQueue::push(const Command& cmd) {
  lastAdded++;
  lastAdded = lastAdded%CmdQueue::N;
  commands[lastAdded] = cmd;
  lastStop = max(lastStop, cmd.stop);
  active = true;
}

// ---- scheduler ----

Scheduler::Scheduler(Robot* b, Configuration* c) : config(c), robot(b) {}
Scheduler::~Scheduler();

void Scheduler::update() {
  unsigned long t = millis();

  for (int i=0; i<10; i++) { // for each actuator command queue
    CmdQueue* queue = actuators[i];
    Serial.println(String(i)+", "+String(int(queue)));
    if (queue == 0) continue;
    if (!queue->active) continue;

    if (queue->lastStop <= t) {
      queue->active = false;
      robot->setActuator(i, 0); // safeguard stop
    }

    for (int j=0; j<CmdQueue::N; j++) { // for each command
      Command& cmd = queue->commands[j];
      Serial.println(String(i)+", "+String(j)+", "+String(cmd.actID));
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
      Serial.println("Start cmd!");
      robot->setActuator(i, cmd.speed);
      queue->currentStarted = cmd.start;
      cmd.started = true;
    }
  }
}

const int Nbuff = 64;
char buff[Nbuff];
int pointer = 0;

void Scheduler::processSerialInput(char c) { // TODO!!!!!!!
  //Serial.write(c);               // print it out the serial monitor
  buff[pointer] = c;
  pointer++;
      
  if (c == '\n') {                // if the byte is a newline character
    buff[pointer - 1] = 0;
    String data = String(buff);
    //processCommand(data);
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

string::string(const char* d, int N) {
  if (!d || N == 0) return;
  if (N == -1) for (const char* i=d; *i != 0; i++) length++;
  else length = N;
  data = new byte[length];
  memcpy(data, d, length);
}

string::~string() {
  if (data) delete data;
}

string& string::operator=(const string& other) {
    length = other.length;
    data = new byte[length];
    memcpy(data, other.data, length);
    return *this;
}

byte string::operator [](int i) const {return data[i];}
byte & string::operator [](int i) {return data[i];}

void string::print() {
  for (int i=0; i<length; i++) Serial.print(char(data[i]));
  Serial.println("");
}

string Scheduler::genActorCommand(byte i, byte s, byte d, byte o) {
  string cmd("cmd:AiSsDdOo");
  cmd[5] = char(i);
  cmd[7] = char(s);
  cmd[9] = char(d);
  cmd[11] = char(o);
  return cmd;
}

void Scheduler::processCommand(string Cmd) {
  int N = Cmd.length/2;

  Command command;
  for (int i=4; i<2*N; i+=2) { // start at 2 to skip 'cmd:' at the beginning!
    char label = Cmd[i];
    byte param = Cmd[i+1];

    if (label == 'A') command.actID = param;
    if (label == 'S') command.speed = param;
    if (label == 'D') command.duration = param;
    if (label == 'O') command.start = millis() + param;
  }
  command.stop = command.start + int(command.duration);

  int caID = command.actID;
  if (caID < 0 || caID > 10) return;
  actuators[command.actID]->push(command);
}




