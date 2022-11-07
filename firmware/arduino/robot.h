#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>

class Configuration;

class Actuator {
	public:
    int ID = -1;
    Configuration* config = 0;

		Actuator(int ID, Configuration* config);
		~Actuator();
		
		void setSpeed(int speed);
};	

class Robot {
  private:
    Configuration* config = 0;
		Actuator* actuators[10] = {0};

    void setupPins();

	public:
		Robot(Configuration* config);
		~Robot();
		
		void setup(int N_actuators);
		void setActuator(int i, int speed);
};

#endif //ROBOT_H