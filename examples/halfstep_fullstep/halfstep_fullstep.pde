#include <EasyStepper.h>

int pins[4] = {9, 10, 11, 12}; // Pins 1, 2, 3 and 4 to the stepper driver
int rpm = 10;
bool fullstep = true;
bool CW = false;

EasyStepper(pins[0], pins[1], pins[2], pins[3]);

void setup() {
  EasyStepper.init();
  EasyStepper.setRPM(rpm);
}

void loop() {

  // This loop turns 1 full revolution clockwise, then 1 counter-clockwise, etc..
  // After two rotations it toggles between full-step and half-step with the same rotational speed
  if (Stepper.getStepsLeft() == 0) {
    CW = !CW;
    if (CW) { 
      fullstep = !fullstep;
      Stepper.setFullStep(fullstep);
    }
    
    Stepper.moveRevolutions(CW, 1);
  }

  Stepper.run();
}
