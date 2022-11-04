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
  Serial.println("Arduino setup starting");

  String bot = "Telerobi";
  
  config = getConfig(bot);
  robot = new Robot(config);
  robot->setup(config->N_actuators);
  scheduler = new Scheduler(robot, config);

  Serial.println("Arduino setup complete");
  delay(2000);
}

void loop() {
  scheduler->update();

  if (!espInput.available()) return;
  while (espInput.available() > 0) {
    char c = espInput.read();      // read a byte, then
    scheduler->processSerialInput(c);
  }
}
