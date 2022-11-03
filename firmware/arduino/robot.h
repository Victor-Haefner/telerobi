#ifndef ROBOT_H
#define ROBOT_H

#include "config.h"

class Actuator {
	public:		
		Actuator();
		~Actuator();
		
		void runFor(double Tms);
};	

class Robot {
  private:
    Configuration config;
		Actuator* actuators[10] = {0};

    void setupPins();

    void updateActuators();
    void setMotor1(int dir);
    void setMotor2(int dir);

	public:
		Robot(Configuration& config);
		~Robot();
		
		void setup(int N_actuators);
		void setActuator(int i, int dir);
    void drive(String line);
};

#endif //ROBOT_H