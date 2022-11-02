#include <SoftwareSerial.h> 
#include <SPI.h>

//add the soft serial libray
#define rxpin 2                                                                //set the RX pin to pin 2
#define txpin 13                                                               //set the TX pin to pin 13

//Bit positions in the Arduino UNO WIFI Rev 2 micro controller output

#define PWM2B 3 //For slots M1&M2 on the L293D Motor Shield
#define DIR_CLK 4 //SHCP of the Shift Register 74HC595
#define PWM0B 5 //For slots M3&M4 on the L293D Motor Shield
#define PWM0A 6 //For slots M3&M4 on the L293D Motor Shield
#define DIR_EN 7 //OE of the Shift Register 74HC595
#define DIR_SER 8 //DS of the Shift Register 74HC595
#define PWM1A 9 //For slots SERVO1&SERVO3 on the L293D Motor Shield
#define PWM1B 10 //For slots SERVO1&SERVO3 on the L293D Motor Shield
#define PWM2A 11 //For slots M1&M2 on the L293D Motor Shield
#define DIR_LATCH 12 //STCP of the Shift Register 74HC595

//Bit positions in the 74HCT595 shift register output

#define MOTOR3_A 15 //Q0
#define MOTOR2_A 1 //Q1
#define MOTOR1_A 2 //Q2
#define MOTOR1_B 3 //Q3
#define MOTOR2_B 4 //Q4
#define MOTOR4_A 5 //Q5
#define MOTOR3_B 6 //Q6
#define MOTOR4_B 7 //Q7

SoftwareSerial myserial(rxpin, txpin);

//Motor1 connected to M1&M2 by setting the following entries to HIGH, respectively: Q2=4, Q1=2, Q3=8, Q4=16
//Motor2 connected to M3&M4 by setting the following entries to HIGH, respectively: Q0=1, Q5=32, Q6=64, Q7=128

String commandline = "";
int waittime = 2000;
unsigned long zeitpuffer = 0;

// konstanten fuer impulslast
int dummyloadpin = 17;
long dummyofftime = 1000;
long dummyontime = 50;
bool dummystate = false;
long lastmillis = 0;
long interval = 4000;

//int status = WL_IDLE_STATUS;
bool standby = false;

Robot* robot = 0;

void setup() {  
  Serial.begin(115200);
  myserial.begin(9600); 
  Serial.println("Arduino setup starting");
  
  robot = new Robot();
  //myserial.listen();
  
  pinMode(9, OUTPUT);      // set the LED pin mode
  pinMode(dummyloadpin,OUTPUT); //setze impulslast-pin

  //Pins für Treiberboard initialisieren
  pinMode(DIR_EN, OUTPUT);
  pinMode(DIR_SER, OUTPUT);
  pinMode(DIR_LATCH, OUTPUT);
  pinMode(DIR_CLK, OUTPUT);
  pinMode(PWM0A, OUTPUT);
  pinMode(PWM0B, OUTPUT);
  digitalWrite(PWM0A, HIGH);
  digitalWrite(PWM0B, HIGH);
  pinMode(PWM2A, OUTPUT);
  pinMode(PWM2B, OUTPUT);
  digitalWrite(PWM2A, HIGH);
  digitalWrite(PWM2B, HIGH);

  Serial.println("Arduino setup complete");
  delay(2000); 
  fahrbewegung("S"); 
  delay(2000);
  standby = false;
}

void processCommand(String Cmd) {
  uint8_t speeds = 50;
  char toSend[Cmd.length()+1];
  Cmd.toCharArray(toSend,Cmd.length()+1);
  lastmillis = millis();
  standby = false;
  
  if (Cmd.substring(4,5) == "S") fahrbewegung("S");
  if (Cmd.substring(4,5) == "F") fahrbewegung("D");
  if (Cmd.substring(4,5) == "B") fahrbewegung("L");
  if (Cmd.substring(4,5) == "L") fahrbewegung("R");
  if (Cmd.substring(4,5) == "R") fahrbewegung("B");
  
  Serial.println("-"+Cmd);
}

const int Nbuff = 256;
char buff[Nbuff];
int pointer = 0;

void loop() {
  if (millis()>lastmillis + 500) fahrbewegung("S"); //to kill motors after commands, maybe tune later
  if (myserial.available()){
    
    while (myserial.available() > 0) {
      char c = myserial.read();      // read a byte, then
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
  return;

  if ((millis() > lastmillis + dummyofftime) && (!dummystate)) //anschalten
    {
      lastmillis = millis();
      digitalWrite(dummyloadpin,HIGH);
      dummystate = true;
    }
    
   if ((millis()> lastmillis + dummyontime)  && dummystate) 
    {
      lastmillis = millis();
      digitalWrite(dummyloadpin,LOW);
      dummystate = false;
    }
}

void fahrbewegung(String line) {
  if (standby) return;
  
  if (millis()>zeitpuffer) {
	  if (line == "S") {
		robot->setActuator(0, 0);
		robot->setActuator(1, 0);
		standby = true;
	  } else if (line == "D")) {
		robot->setActuator(0, 1);
		robot->setActuator(1, 1);
	  } else if (line == "L") {
		robot->setActuator(0, 0);
		robot->setActuator(1, 1);
	  } else if (line == "R") {
		robot->setActuator(0, 1);
		robot->setActuator(1, 0);
	  } else if (line == "B") {
		robot->setActuator(0, -1);
		robot->setActuator(1, -1);
	  }
	zeitpuffer = millis()+500;
  }
}
