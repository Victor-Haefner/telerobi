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

int Motor1Seq[5]={0,4,2,8,16}; //Sequence for BPM1 for forward 4 steps
int Motor2Seq[5]={0,1,32,64,128}; //Sequence for BPM2 for forward 4 steps
int motorval;
int motor1val;
int motor2val;
int fahrtrichtung = 0;
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

void setup() {  
  Serial.begin(115200);
  myserial.begin(9600); 
  Serial.println("Arduino setup starting");
  //myserial.listen();

  Motor1OFF();
  Motor2OFF();
  
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
  fahrbewegung("/fahrtrichtung/0"); 
  delay(2000);
  standby = false;
}

void processCommand(String Cmd) {
  uint8_t speeds = 50;
  char toSend[Cmd.length()+1];
  Cmd.toCharArray(toSend,Cmd.length()+1);
  lastmillis = millis();
  standby = false;
  
  if (Cmd.substring(4,5) == "S") fahrbewegung("/fahrtrichtung/0");
  if (Cmd.substring(4,5) == "F") fahrbewegung("/fahrtrichtung/1");
  if (Cmd.substring(4,5) == "B") fahrbewegung("/fahrtrichtung/4");
  if (Cmd.substring(4,5) == "L") fahrbewegung("/fahrtrichtung/2");
  if (Cmd.substring(4,5) == "R") fahrbewegung("/fahrtrichtung/3");
  
  Serial.println("-"+Cmd);
}

const int Nbuff = 256;
char buff[Nbuff];
int pointer = 0;

void loop() {
  if (millis()>lastmillis + 500) fahrbewegung("/fahrtrichtung/0"); //to kill motors after commands, maybe tune later
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

//
void fahrbewegung(String line) {
  
  
  if (standby) return;
  /*
  if (line.endsWith("/fahrtrichtung/0")) {
    Serial.print("Fahrtrichtung:");
    Serial.println(zeitpuffer);
    Serial.println(millis());
    Serial.println(fahrtrichtung);
  }*/

  if (line.endsWith("/fahrtrichtung/0")&&(millis()>zeitpuffer)) {
    fahrtrichtung = 0;
    Motor1OFF();
    Motor2OFF();
    zeitpuffer = millis()+500;
    Serial.print("Fahrtrichtung:");
    Serial.println(fahrtrichtung);
    standby = true;
  } else if (line.endsWith("/fahrtrichtung/1")&&(millis()>zeitpuffer)) {
    fahrtrichtung = 1;
    Motor1FWD();
    Motor2FWD();
    zeitpuffer = millis()+500;
    Serial.print("Fahrtrichtung:");
    Serial.println(fahrtrichtung);
  } else if (line.endsWith("/fahrtrichtung/2")&&(millis()>zeitpuffer)) {
    fahrtrichtung = 2;
    Motor1OFF();
    Motor2FWD();
    zeitpuffer = millis()+500;
    Serial.print("Fahrtrichtung:");
    Serial.println(fahrtrichtung);
  } else if (line.endsWith("/fahrtrichtung/3")&&(millis()>zeitpuffer)) {
    fahrtrichtung = 3;
    Motor1FWD();
    Motor2OFF();
    zeitpuffer = millis()+500;
    Serial.print("Fahrtrichtung:");
    Serial.println(fahrtrichtung);
  } else if (line.endsWith("/fahrtrichtung/4")&&(millis()>zeitpuffer)) {
    fahrtrichtung = 4;
    Motor1REV();
    Motor2REV();
    zeitpuffer = millis()+500;
    Serial.print("Fahrtrichtung:");
    Serial.println(fahrtrichtung);
  }
}



//Motoren
//Motor 1
void Motor1FWD() { 
  motor1val=Motor1Seq[3]+Motor1Seq[4];        //output-pins (kurzgeschlossen)
  motorval=motor1val+motor2val;               //nichts an motor 2 aendern
  digitalWrite(DIR_LATCH, LOW);                     //schreiben starten 
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval); //in register schreiben
  digitalWrite(DIR_LATCH, HIGH);                    //schreiben stoppen, register scharf schalten
}

void Motor1REV() {
  motor1val=Motor1Seq[1]+Motor1Seq[2];
  motorval=motor1val+motor2val;
  digitalWrite(DIR_LATCH, LOW);
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval);
  digitalWrite(DIR_LATCH, HIGH);
}

void Motor2FWD() {
  motor2val=Motor2Seq[2]+Motor2Seq[1];
  motorval=motor1val+motor2val;
  digitalWrite(DIR_LATCH, LOW);
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval);
  digitalWrite(DIR_LATCH, HIGH);
}


//Motor 2 
void Motor1OFF() { 
  motor1val=Motor1Seq[0];
  motorval=motor1val+motor2val;
  digitalWrite(DIR_LATCH, LOW);
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval);
  digitalWrite(DIR_LATCH, HIGH);
}

void Motor2REV() {
  motor2val=Motor2Seq[4]+Motor2Seq[3];
  motorval=motor1val+motor2val;
  digitalWrite(DIR_LATCH, LOW);
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval);
  digitalWrite(DIR_LATCH, HIGH);
}

void Motor2OFF() {
  motor2val=Motor2Seq[0];
  motorval=motor1val+motor2val;
  digitalWrite(DIR_LATCH, LOW);
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval);
  digitalWrite(DIR_LATCH, HIGH);
}
