#include <EasyStepper.h>

int pins[4] = {9, 10, 11, 12}; // Pins 1, 2, 3 and 4 to the stepper driver
int rpm = 10;
bool CW = false;

EasyStepper(pins[0], pins[1], pins[2], pins[3]);

void setup() {
  EasyStepper.init();
  EasyStepper.setRPM(rpm);
}

void loop() {

  // This loop turns 1 full revolution clockwise, then 1 counter-clockwise, etc..
  // Commented moveSteps() and moveDegrees do the same
  // Uncomment them (leave only one uncommmented) to try those
  if (Stepper.getStepsLeft() == 0) {
    CW = !CW;
    Stepper.moveRevolutions(CW, 1);
	//Stepper.moveSteps(CW, 4096);		// The 28BYJ-48 has 4096 steps in half-step mode
	//Stepper.moveDegrees(CW, 360);
  }

  Stepper.run();
}
