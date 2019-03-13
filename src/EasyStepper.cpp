/*
 * EasyStepper.cpp
 *
 * Created and maintained by Paul Miggiels <paul@paulmiggiels.nl>
 * https://github.com/paulmiggiels/EasyStepper
 *
 * License: MIT License
 * Copyright (c) Paul Miggiels
 *
 */

#include "EasyStepper.h"
#include "Arduino.h"


// PUBLIC
//	These can be called to set or read stepper info
//

/* EasyStepper(...)************************************************************
 * 	Constructor to initialise members with the right pin numbers
 *
 * 	@param	pin1-4	output pins for the stepper driver
 *****************************************************************************/
EasyStepper::EasyStepper(uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4) : pins{pin1, pin2, pin3, pin4} {
	// make sure that stepTime is set to non-zero (default RPM is 10)
	setRPM(RPM);
}


EasyStepper::~EasyStepper() {
	// empty
}


/* init()**********************************************************************
 * 	Initialises the pins
 * 	Needs to be called in the setup()
******************************************************************************/
void EasyStepper::init() {
	for (uint8_t i = 0; i < 4; i++) {
		pinMode(pins[i], OUTPUT);
	}
	// Initialise pin sequence to zeros
	memset(pinSequence, 0, sizeof(pinSequence));
}


/* run()***********************************************************************
 * 	Checks if the step time has been exceed to make the next step sequence
 * 	Call this function continuously in the loop()
******************************************************************************/
void EasyStepper::run() {
	if (stepsLeft > 0 && (micros() - lastStepTime > stepTime)) {
		lastStepTime = micros();
		//Serial.println(stepsLeft);
		makeStep();
	}
}


/* setRPM(..)******************************************************************
 * 	Set the RPM by calculating the step time in microseconds
 *
 * 	@param 	rpm		RPM to set
******************************************************************************/
void EasyStepper::setRPM(uint8_t rpm) {
	RPM = rpm;
	stepTime = usPerMin / (stepsPerRotation * RPM);
}

/* moveSteps(..)***************************************************************
 * 	Set number of sequence steps to move
 *
 * 	@param 	steps	number of steps to move
******************************************************************************/
void EasyStepper::moveSteps(bool CW, uint32_t steps) {
	stepsLeft = steps;
	setDirection(CW);
}


/* moveRevolutions(..)*********************************************************
 * 	Set number of full revolutions to move
 * 	Only takes integers as argument. Use moveSteps() or moveDegrees() for move precise
 * 	moves
 *
 * 	@param 	turns	Revolutions to turn
******************************************************************************/
void EasyStepper::moveRotations(bool CW, uint8_t turns) {
	// Force 32-bit integer here, other wise the 16-bit from stepsPerRotation is used
	// This would result in a maximum of 16 revolutions before overflow occurs
	stepsLeft = (uint32_t) turns * stepsPerRotation;
	setDirection(CW);
}


/* moveDegrees(..)*************************************************************
 * 	Set degrees to rotate the stepper
 * 	Takes integers (whole degrees) as argument. For more precise control, use
 * 	moveSteps()
 *
 * 	@param 	degrees	Degrees to rotate
******************************************************************************/
void EasyStepper::moveDegrees(bool CW, uint16_t degrees) {
	stepsLeft = degreesToSteps(degrees);
	setDirection(CW);
}


/* release()*******************************************************************
 * 	Release the hold current on the coils
******************************************************************************/
void EasyStepper::release() {
	for (uint8_t i = 0; i < NPins; i++) {
		digitalWrite(pins[i], 0);
	}
}


/* setFullStep(..)*************************************************************
 * 	Enables/disables full step
 * 	Disabled means two-one on half-step mode is used (default)
 * 	When full step is enabled, the number of steps per rotation is halved
 * 	This is restored when full-step is disabled again
 * 	Recalculates the RPM after adjusting steps per rotation
 *
 * 	@param 	fullstep	True to set full stepping mode
******************************************************************************/
void EasyStepper::setFullStep(bool fullstep) {
	if (halfstep == fullstep) {
		halfstep = !fullstep;
		stepsPerRotation = (halfstep) ? stepsPerRotation * 2 : stepsPerRotation / 2;
		setRPM(RPM);
	}
}


/* setSteps(..)****************************************************************
 * 	Set number of steps per rotation of the stepper
 * 	Default is 4096 for the 28-BYJ-48 stepper.
 * 	NOTE: Be cautious with half-step and full-step mode!
 * 	Recalculates the RPM after adjusting steps per rotation
 *
 * 	@param 	steps	Steps per rotation
******************************************************************************/
void EasyStepper::setSteps(uint16_t steps) {
	stepsPerRotation = steps;
	setRPM(RPM);
}


/* setAutoRelease()************************************************************
 * 	Enables/disables auto release of the hold current after finishing a move
 *
 * 	@param	release	True to enable auto release
******************************************************************************/
void EasyStepper::setAutoRelease(bool release) {
	autoRelease = release;
}



// 	PRIVATES
//	Don't touch them
//


/* setDirection(..)************************************************************
 * 	Change the direction of movement: CW or CCW
 * 	Step direction is positive for CW, negative for CCW
 *
 * 	@param 	bool CW		True for clockwise movement
******************************************************************************/
void EasyStepper::setDirection(bool CW) {
	direction = (CW) ? 1 : -1;
}


/* degreesToSteps(..)**********************************************************
 * 	Convert degrees to rotate into steps to move
 *
 * 	@param 	degrees	Degrees to rotate
 * 	@return	steps	Steps to move
******************************************************************************/
uint32_t EasyStepper::degreesToSteps(uint16_t degrees) {
	// Force 32-bit again to prevent overflow
	uint32_t steps = (uint32_t) (degrees/360) * stepsPerRotation;
	return steps;
}


/* makeStep()******************************************************************
 * 	This is where the magic happens. Generates a pin-sequence for
 * 	full-stepping or one-two on half-stepping mode
 * 	Sequence is generated on-the-spot for any number of pins (coils), and
 * 	automatically takes direction into account
 *
 * 	Full-step sequence sets the current pin LOW and next pin HIGH
 * 	Half-step uses a wait-variable to decide whether to set the next pin HIGH
 * 	or to set the current pin LOW
******************************************************************************/
void EasyStepper::makeStep() {

	// Update the wait boolean
	wait = !wait;

	// Half-step and wait step? Reset the last pin to 0
	if (halfstep && wait) {
		pinSequence[lastStep] = 0;
	}
	// Full-step or no wait? Set the next pin to 1
	// For full-step, the last pin is also set to 0
	else {
		lastStep = step;
		step = step + direction;
		// Rotate pins
		if (step >= NPins) step = 0;
		if (step < 0) step = NPins-1;

		pinSequence[step] = 1;
		pinSequence[lastStep] = (halfstep) ? 1 : 0;
	}

	// Write pins and update steps left
	for (uint8_t i = 0; i < NPins; i++) {
		digitalWrite(pins[i], pinSequence[i]);
	}
	stepsLeft--;

	// If auto-release enabled and move finished, release the hold current
	// The delay is a quick-and-dirty way to ensure that the move is fully finished
	// Occurs only seldom (if at all), so blocking effect is negligible
	if (autoRelease && stepsLeft == 0) {
		delayMicroseconds(stepTime);
		release();
	}
}
