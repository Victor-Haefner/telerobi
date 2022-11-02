
class Actuator {
	public:		
		Actuator();
		~Arctuator();
		
		void runFor(double Tms);
}	

class Robot {
	public:
		Actuator* actuators[10] = {0};
	
		Robot();
		~Robot();
		
		void setup(int N_actuators);
		void setActuator(int i, int dir);
}