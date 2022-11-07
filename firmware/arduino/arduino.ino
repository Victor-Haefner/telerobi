#include <SoftwareSerial.h> 
#include <SPI.h>

#include "config.h"
#include "robot.h"
#include "scheduler.h"

// add the soft serial libray
#define rxpin 2                                                                //set the RX pin to pin 2
#define txpin 13                                                               //set the TX pin to pin 13
SoftwareSerial espInput(rxpin, txpin);

Configuration* config = 0;
Robot* robot = 0;
Scheduler* scheduler = 0;

void setup() {
  Serial.begin(115200);
  espInput.begin(9600); 

  // TODO: config! ..or maybe identify the board??
  String bot = "Elegoo";
  //String bot = "Telerobi";
  
  config = getConfig(bot);
  robot = new Robot(config);
  robot->setup(config->N_actuators);
  scheduler = new Scheduler(robot, config);

  Serial.println("Uno started");
  //delay(2000);

  /*string cmd = scheduler->genActorCommand(1, 180, 10, 0); // set Actor i, Speed s, Duration d, Offset o
  //scheduler->processCommand(cmd); 
  for (int i=0; i<cmd.length; i++) scheduler->processSerialInput(cmd[i]);
  scheduler->processSerialInput(0);
  scheduler->processSerialInput('\n');
  cmd = scheduler->genActorCommand(1, 220, 10, 10); // set Actor i, Speed s, Duration d, Offset o
  //scheduler->processCommand(cmd); 
  for (int i=0; i<cmd.length; i++) scheduler->processSerialInput(cmd[i]);
  scheduler->processSerialInput(0);
  scheduler->processSerialInput('\n');
  cmd = scheduler->genActorCommand(1, 250, 10, 20); // set Actor i, Speed s, Duration d, Offset o
  //scheduler->processCommand(cmd);
  for (int i=0; i<cmd.length; i++) scheduler->processSerialInput(cmd[i]);
  scheduler->processSerialInput(0);
  scheduler->processSerialInput('\n');*/
}

void loop() {
  scheduler->update();

  if (!espInput.available()) return;
  while (espInput.available() > 0) {
    char c = espInput.read();      // read a byte, then
    scheduler->processSerialInput(c);
  }
}
