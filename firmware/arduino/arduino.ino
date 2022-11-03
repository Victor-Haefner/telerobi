#include <SoftwareSerial.h> 
#include <SPI.h>

#include "config.h"
#include "robot.h"


//add the soft serial libray
#define rxpin 2                                                                //set the RX pin to pin 2
#define txpin 13                                                               //set the TX pin to pin 13

SoftwareSerial espInput(rxpin, txpin);

String commandline = "";
int waittime = 2000;

long lastmillis = 0;
long interval = 4000;


Configuration config;
Robot* robot = 0;

void setup() {  
  config = getConfig("Telerobi");

  Serial.begin(115200);
  espInput.begin(9600); 
  Serial.println("Arduino setup starting");
  
  robot = new Robot(config);

  Serial.println("Arduino setup complete");
  delay(2000); 
  robot->drive("S"); 
  delay(2000);
}

void processCommand(String Cmd) {
  uint8_t speeds = 50;
  char toSend[Cmd.length()+1];
  Cmd.toCharArray(toSend,Cmd.length()+1);
  lastmillis = millis();
  
  if (Cmd.substring(4,5) == "S") robot->drive("S");
  if (Cmd.substring(4,5) == "F") robot->drive("D");
  if (Cmd.substring(4,5) == "B") robot->drive("L");
  if (Cmd.substring(4,5) == "L") robot->drive("R");
  if (Cmd.substring(4,5) == "R") robot->drive("B");
  
  Serial.println("-"+Cmd);
}

const int Nbuff = 1024;
char buff[Nbuff];
int pointer = 0;

void loop() {
  if (millis() > lastmillis + 500) {
    robot->drive("S"); //to kill motors after commands, maybe tune later
    lastmillis = millis();
  }

  if (!espInput.available()) return;
    
  while (espInput.available() > 0) {
    char c = espInput.read();      // read a byte, then
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
}
