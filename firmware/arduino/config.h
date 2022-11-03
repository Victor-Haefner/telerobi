#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

class Configuration {
  public:
    int LOAD = 17;

    //Bit positions in the Arduino UNO WIFI Rev 2 micro controller output
    int PWM2B = 3; //For slots M1&M2 on the L293D Motor Shield
    int DIR_CLK = 4; //SHCP of the Shift Register 74HC595
    int PWM0B = 5; //For slots M3&M4 on the L293D Motor Shield
    int PWM0A = 6; //For slots M3&M4 on the L293D Motor Shield
    int DIR_EN = 7; //OE of the Shift Register 74HC595
    int DIR_SER = 8; //DS of the Shift Register 74HC595
    int PWM1A = 9; //For slots SERVO1&SERVO3 on the L293D Motor Shield
    int PWM1B = 10; //For slots SERVO1&SERVO3 on the L293D Motor Shield
    int PWM2A = 11; //For slots M1&M2 on the L293D Motor Shield
    int DIR_LATCH = 12; //STCP of the Shift Register 74HC595

    //Bit positions in the 74HCT595 shift register output
    int MOTOR3_A = 15; //Q0
    int MOTOR2_A = 1; //Q1
    int MOTOR1_A = 2; //Q2
    int MOTOR1_B = 3; //Q3
    int MOTOR2_B = 4; //Q4
    int MOTOR4_A = 5; //Q5
    int MOTOR3_B = 6; //Q6
    int MOTOR4_B = 7; //Q7
};

Configuration getConfig(String botType);

#endif //CONFIG_H