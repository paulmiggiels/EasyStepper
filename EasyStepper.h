/*
 * easyStepper.h
 *
 *  Created on: 1 mrt. 2019
 *      Author: miggielsalw
 */

#ifndef EASYSTEPPER_H_
#define EASYSTEPPER_H_

#include <Arduino.h>

const uint32_t usPerMin = 60000000;
const uint8_t NPins = 4;	// Adjust this value for controllers with different amount of pins


class EasyStepper {

public:
	EasyStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);
	virtual ~EasyStepper();

	void init();
	void run();
	void setRPM(uint8_t rpm);
	void moveSteps(bool CW, uint32_t steps);
	void moveRevolutions(bool CW, uint8_t turns);
	void moveDegrees(bool CW, uint16_t degrees);
	void setFullStep(bool fullstep);
	void setSteps(uint16_t steps);

	int32_t getStepsLeft() { return (int32_t) direction * stepsLeft; }
	uint8_t getRPM() { return RPM; }
	uint16_t getDelay() { return delay; }

private:

	void makeStep();
	void setDirection(bool CW);
	uint32_t degreesToSteps(uint16_t degrees);

	uint8_t lookup[8] = {B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001};

	//
	uint8_t pins[NPins];
	uint16_t stepsPerRotation = 4096;
	uint8_t RPM = 10;
	int8_t direction = 1;
	uint16_t delay;
	uint32_t stepsLeft = 0;
	uint32_t lastStepTime = 0;

	// Stepper sequence variables
	uint8_t pinSequence[NPins];
	int8_t step = 0;
	int8_t lastStep = 0;
	bool halfstep = true;
	bool wait = true;
};

#endif /* EASYSTEPPER_H_ */
