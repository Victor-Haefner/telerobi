#include "robot.h"
#include <Arduino.h>

// konstanten fuer impulslast
long dummyofftime = 1000;
long dummyontime = 50;
bool dummystate = false;


void Robot::setupPins() {
  pinMode(9, OUTPUT);      // set the LED pin mode
  pinMode(config.LOAD,OUTPUT); //setze impulslast-pin

  //Pins für Treiberboard initialisieren
  pinMode(config.DIR_EN, OUTPUT);
  pinMode(config.DIR_SER, OUTPUT);
  pinMode(config.DIR_LATCH, OUTPUT);
  pinMode(config.DIR_CLK, OUTPUT);
  pinMode(config.PWM0A, OUTPUT);
  pinMode(config.PWM0B, OUTPUT);
  digitalWrite(config.PWM0A, HIGH);
  digitalWrite(config.PWM0B, HIGH);
  pinMode(config.PWM2A, OUTPUT);
  pinMode(config.PWM2B, OUTPUT);
  digitalWrite(config.PWM2A, HIGH);
  digitalWrite(config.PWM2B, HIGH);
}


Robot::Robot(Configuration& c) : config(c) {}
Robot::~Robot() {}

void Robot::setup(int N_actuators) {
  setupPins();

	for (int i=0; i<min(10, N_actuators); i++) {
		actuators[i] = new Actuator();
		setActuator(i, 0);
	}
}

int Motor1Seq[5]={0,4,2,8,16}; //Sequence for BPM1 for forward 4 steps
int Motor2Seq[5]={0,1,32,64,128}; //Sequence for BPM2 for forward 4 steps
int motor1val;
int motor2val;

void Robot::updateActuators() {
  digitalWrite(config.DIR_LATCH, LOW);                     //schreiben starten 
  shiftOut(config.DIR_SER, config.DIR_CLK, MSBFIRST, motor1val+motor2val); //in register schreiben
  digitalWrite(config.DIR_LATCH, HIGH);                    //schreiben stoppen, register scharf schalten
}

//Motor1 connected to M1&M2 by setting the following entries to HIGH, respectively: Q2=4, Q1=2, Q3=8, Q4=16
//Motor2 connected to M3&M4 by setting the following entries to HIGH, respectively: Q0=1, Q5=32, Q6=64, Q7=128

void Robot::setMotor1(int dir) { 
  if (dir ==  0) motor1val = 0;
  if (dir ==  1) motor1val = 8+16;
  if (dir == -1) motor1val = 2+4;
  updateActuators();
}

void Robot::setMotor2(int dir) { 
  if (dir ==  0) motor2val = 0;
  if (dir ==  1) motor2val = 1+32;
  if (dir == -1) motor2val = 64+128;
  updateActuators();
}

void Robot::setActuator(int i, int dir) {
	if (i == 0) setMotor1(dir);
	if (i == 1) setMotor2(dir);
}

void Robot::drive(String line) {
  if (line == "S") {
    setActuator(0, 0);
    setActuator(1, 0);
  } else if (line == "D") {
    setActuator(0, 1);
    setActuator(1, 1);
  } else if (line == "L") {
    setActuator(0, 0);
    setActuator(1, 1);
  } else if (line == "R") {
    setActuator(0, 1);
    setActuator(1, 0);
  } else if (line == "B") {
    setActuator(0, -1);
    setActuator(1, -1);
  }
}