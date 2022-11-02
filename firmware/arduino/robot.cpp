#include "robot.h"


void Robot::setup(int N_actuators) {
	for (int i=0; i<min(10, N_actuators); i++) {
		actuators[i] = new Actuator();
		setActuator(i, 0);
	}
}






int Motor1Seq[5]={0,4,2,8,16}; //Sequence for BPM1 for forward 4 steps
int Motor2Seq[5]={0,1,32,64,128}; //Sequence for BPM2 for forward 4 steps
int motor1val;
int motor2val;

void updateActuators() {
  motorval=motor1val+motor2val;
  digitalWrite(DIR_LATCH, LOW);                     //schreiben starten 
  shiftOut(DIR_SER, DIR_CLK, MSBFIRST, motorval); //in register schreiben
  digitalWrite(DIR_LATCH, HIGH);                    //schreiben stoppen, register scharf schalten
}

void setMotor1(int dir) { 
  if (dir ==  0) motor1val = 0;
  if (dir ==  1) motor1val=8+16;
  if (dir == -1) motor1val=2+4;
  updateActuators();
}

void setMotor2(int dir) { 
  if (dir ==  0) motor2val = 0;
  if (dir ==  1) motor2val=1+32;
  if (dir == -1) motor2val=64+128;
  updateActuators();
}

void Robot::setActuator(int i, int dir) {
	if (i == 0) setMotor1(dir);
	if (i == 1) setMotor2(dir);
}