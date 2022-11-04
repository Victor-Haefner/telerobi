#include "config.h"

Configuration* getConfig(String botType) {
  Configuration* c = new Configuration();

  if (botType == "Telerobi") {
    c->type = TELEROBI;
    c->N_actuators = 2;

    c->LOAD = 17;

    c->PWM2B = 3; //For slots M1&M2 on the L293D Motor Shield
    c->DIR_CLK = 4; //SHCP of the Shift Register 74HC595
    c->PWM0B = 5; //For slots M3&M4 on the L293D Motor Shield
    c->PWM0A = 6; //For slots M3&M4 on the L293D Motor Shield
    c->DIR_EN = 7; //OE of the Shift Register 74HC595
    c->DIR_SER = 8; //DS of the Shift Register 74HC595
    c->PWM1A = 9; //For slots SERVO1&SERVO3 on the L293D Motor Shield
    c->PWM1B = 10; //For slots SERVO1&SERVO3 on the L293D Motor Shield
    c->PWM2A = 11; //For slots M1&M2 on the L293D Motor Shield
    c->DIR_LATCH = 12; //STCP of the Shift Register 74HC595

    c->MOTOR3_A = 15; //Q0
    c->MOTOR2_A = 1; //Q1
    c->MOTOR1_A = 2; //Q2
    c->MOTOR1_B = 3; //Q3
    c->MOTOR2_B = 4; //Q4
    c->MOTOR4_A = 5; //Q5
    c->MOTOR3_B = 6; //Q6
    c->MOTOR4_B = 7; //Q7
  }

  if (botType == "Elegoo") { // TODO
    c->type = ELEGOO;
    c->N_actuators = 4;

    c->LOAD = 17;
    
    c->PWM2B = 3; //For slots M1&M2 on the L293D Motor Shield
    c->DIR_CLK = 4; //SHCP of the Shift Register 74HC595
    c->PWM0B = 5; //For slots M3&M4 on the L293D Motor Shield
    c->PWM0A = 6; //For slots M3&M4 on the L293D Motor Shield
    c->DIR_EN = 7; //OE of the Shift Register 74HC595
    c->DIR_SER = 8; //DS of the Shift Register 74HC595
    c->PWM1A = 9; //For slots SERVO1&SERVO3 on the L293D Motor Shield
    c->PWM1B = 10; //For slots SERVO1&SERVO3 on the L293D Motor Shield
    c->PWM2A = 11; //For slots M1&M2 on the L293D Motor Shield
    c->DIR_LATCH = 12; //STCP of the Shift Register 74HC595

    c->MOTOR3_A = 15; //Q0
    c->MOTOR2_A = 1; //Q1
    c->MOTOR1_A = 2; //Q2
    c->MOTOR1_B = 3; //Q3
    c->MOTOR2_B = 4; //Q4
    c->MOTOR4_A = 5; //Q5
    c->MOTOR3_B = 6; //Q6
    c->MOTOR4_B = 7; //Q7
  }

  return c;
}