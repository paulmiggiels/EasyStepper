/*
 * EasyStepper.cpp
 *
 *  Created on: 1 mrt. 2019
 *      Author: miggielsalw
 */

#include "EasyStepper.h"
#include "Arduino.h"

/* PUBLIC *************************
*	These can be called to set or read stepper info
*/
EasyStepper::EasyStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4) : pins{pin1, pin2, pin3, pin4} {
	setRPM(RPM);
}

EasyStepper::~EasyStepper() {
	// empty
}

void EasyStepper::init() {
	for (uint8_t i = 0; i < 4; i++) {
		pinMode(pins[i], OUTPUT);
	}
	// Initialise pin sequence to zeros
	memset(pinSequence, 0, sizeof(pinSequence));
}
void EasyStepper::run() {
	if (stepsLeft > 0 && (micros() - lastStepTime > delay)) {
		lastStepTime = micros();
		//Serial.println(stepsLeft);
		makeStep();
	}
}
void EasyStepper::setRPM(uint8_t rpm) {
	RPM = rpm;
	delay = usPerMin / (stepsPerRotation * RPM);
}
void EasyStepper::moveSteps(bool CW, uint32_t steps) {
	stepsLeft = steps;
	setDirection(CW);
}
void EasyStepper::moveRevolutions(bool CW, uint8_t turns) {
	stepsLeft = turns * stepsPerRotation;
	setDirection(CW);
}
void EasyStepper::moveDegrees(bool CW, uint16_t degrees) {
	stepsLeft = degreesToSteps(degrees);
	setDirection(CW);
}
void EasyStepper::setFullStep(bool fullstep) {
	if (halfstep == fullstep) {
		halfstep = !fullstep;
		stepsPerRotation = (halfstep) ? stepsPerRotation * 2 : stepsPerRotation / 2;
		setRPM(RPM);
	}
}
void EasyStepper::setSteps(uint16_t steps) {
	stepsPerRotation = steps;
}

/* 	Privates ***********************************
*	Don't touch them
************************************************/
void EasyStepper::setDirection(bool CW) {
	direction = (CW) ? 1 : -1;
}

uint32_t EasyStepper::degreesToSteps(uint16_t degrees) {
	uint32_t steps = (degrees/360) * stepsPerRotation;
	return steps;
}

// This is where the magic happens: generates a pin-sequence for full-stepping and
// One-two phase on - Half Step sequence.
void EasyStepper::makeStep() {

	wait = !wait;
	// Half-step and wait step? Only reset the previous step to 0
	if (halfstep && wait) {
		pinSequence[lastStep] = 0;
	}
	// Full-step or no wait? Increment the ON-position one pin further
	// Previous pin is only reset in full-step mode
	else {
		lastStep = step;
		step = step + direction;
		// Rotate pins
		if (step >= NPins) step = 0;
		if (step < 0) step = NPins-1;

		pinSequence[step] = 1;
		pinSequence[lastStep] = (halfstep) ? 1 : 0;
	}

	// Write pins, update steps left
	for (uint8_t i = 0; i < NPins; i++) {
		digitalWrite(pins[i], pinSequence[i]);
	}
	stepsLeft--;
}
