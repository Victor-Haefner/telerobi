#include "robot.h"
#include "config.h"
#include <Arduino.h>

//Motor1 connected to M1&M2 by setting the following entries to HIGH, respectively: Q2=4, Q1=2, Q3=8, Q4=16
//Motor2 connected to M3&M4 by setting the following entries to HIGH, respectively: Q0=1, Q5=32, Q6=64, Q7=128

int motor1val;
int motor2val;

void Actuator::setSpeed(int speed) {
  if (config->type == TELEROBI) {  
    if (ID == 0) { 
      if (speed == 0) motor1val = 0;
      if (speed >  0) motor1val = 8+16;
      if (speed <  0) motor1val = 2+4;    
    }

    if (ID == 1) {
      if (speed == 0) motor2val = 0;
      if (speed >  0) motor2val = 1+32;
      if (speed <  0) motor2val = 64+128;
    }

    digitalWrite(config->DIR_LATCH, LOW);                     //schreiben starten 
    shiftOut(config->DIR_SER, config->DIR_CLK, MSBFIRST, motor1val+motor2val); //in register schreiben
    digitalWrite(config->DIR_LATCH, HIGH);                    //schreiben stoppen, register scharf schalten
  }

  if (config->type == ELEGOO) { // TODO
    ;
  }
}

Actuator::Actuator(int i, Configuration* c) : ID(i), config(c) {}
Actuator::~Actuator() {}

void Robot::setupPins() {
  pinMode(9, OUTPUT);      // set the LED pin mode
  pinMode(config->LOAD,OUTPUT); //setze impulslast-pin

  //Pins für Treiberboard initialisieren
  pinMode(config->DIR_EN, OUTPUT);
  pinMode(config->DIR_SER, OUTPUT);
  pinMode(config->DIR_LATCH, OUTPUT);
  pinMode(config->DIR_CLK, OUTPUT);
  pinMode(config->PWM0A, OUTPUT);
  pinMode(config->PWM0B, OUTPUT);
  digitalWrite(config->PWM0A, HIGH);
  digitalWrite(config->PWM0B, HIGH);
  pinMode(config->PWM2A, OUTPUT);
  pinMode(config->PWM2B, OUTPUT);
  digitalWrite(config->PWM2A, HIGH);
  digitalWrite(config->PWM2B, HIGH);
}

Robot::Robot(Configuration* c) : config(c) {}
Robot::~Robot() {}

void Robot::setup(int N_actuators) {
  setupPins();

  int N = min(10, N_actuators);
	for (int i=0; i<N; i++) {
		actuators[i] = new Actuator(i, config);
		setActuator(i, 0);
	}
}

void Robot::setActuator(int i, int speed) {
  if (i < 0 || i > 10) return;
  Actuator* a = actuators[i];
  if (a) a->setSpeed(speed);
}