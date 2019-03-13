/*
 * EasyStepper.h
 *
 * Created and maintained by Paul Miggiels <paul@paulmiggiels.nl>
 * https://github.com/paulmiggiels/EasyStepper
 *
 * License: MIT License
 * Copyright (c) Paul Miggiels
 *
 *
 * Changelog:
 * v1.0.0	Initial release
 * v1.0.1	Cleanup & updated commenting; license added
 *
 */

#ifndef EASYSTEPPER_H_
#define EASYSTEPPER_H_

#include <Arduino.h>

const uint32_t usPerMin = 60000000; // microseconds per minute
const uint8_t NPins = 4;			// Adjust this value for controllers with different amount of pins (coils)


class EasyStepper {

public:
	EasyStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4);
	virtual ~EasyStepper();

	void init();
	void run();
	void setRPM(uint8_t rpm);
	void moveSteps(bool CW, uint32_t steps);
	void moveRotations(bool CW, uint8_t turns);
	void moveDegrees(bool CW, uint16_t degrees);
	void setFullStep(bool fullstep);
	void setSteps(uint16_t steps);

	// getStepsLeft() is kind of dirty: by converting to signed int the range is halved
	// but even then it can still hold the equivalent of 524,288 rotations - should be plenty
	int32_t getStepsLeft() { return (int32_t) direction * stepsLeft; }
	uint8_t getRPM() { return RPM; }
	uint16_t getStepTime() { return stepTime; }
	uint16_t getStepsPerRotation() { return stepsPerRotation; }

private:

	void makeStep();
	void setDirection(bool CW);
	uint32_t degreesToSteps(uint16_t degrees);

	// Stepper settings
	uint8_t pins[NPins];
	uint16_t stepsPerRotation = 4096;
	uint8_t RPM = 10;
	int8_t direction = 1;
	uint32_t stepsLeft = 0;
	uint16_t stepTime;
	uint32_t lastStepTime = 0;

	// Stepper sequence variables
	uint8_t pinSequence[NPins];
	int8_t step = 0;
	int8_t lastStep = 0;
	bool halfstep = true;
	bool wait = true;
};

#endif /* EASYSTEPPER_H_ */
