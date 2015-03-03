/*
 * Roller.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#include "Roller.h"

Roller::Roller(Victor * left_motor, Victor * right_motor, DigitalInput * linebreak_sensor) {
	left_side = left_motor;
	right_side = right_motor;
	line_break = linebreak_sensor;
	state = kStopped;

	roller_set = false;

}
void Roller::Update(){
	switch (state){
	case kStopped:
		left_side->Set(0.0);
		right_side->Set(0.0);
		break;
	case kRollingIn:
		left_side->Set(INTAKE_SPEED);
		right_side->Set(-INTAKE_SPEED);
		break;
	case kRollingOut:
		left_side->Set(-EJECT_SPEED);
		right_side->Set(EJECT_SPEED);
		break;
	case kManual:
		if (!roller_set) {
			left_side->Set(0.0);
			right_side->Set(0.0);
		}
		roller_set = false;
	}
}
bool Roller::ToteCaptured(){
	return !line_break->Get(); //linebreak returns 0 when broken
}
void Roller::RollIn(){
	state = kRollingIn;
}
void Roller::RollOut(){
	state = kRollingOut;
}
void Roller::Stop(){
	state = kStopped;
}
void Roller::Manual(float x, float y) {
	state = kManual;
	if (y >= 0.4) {
		left_side->Set(-MANUAL_SPEED);
		right_side->Set(MANUAL_SPEED);
	} else if (y <= -0.4) {
		left_side->Set(MANUAL_SPEED);
		right_side->Set(-MANUAL_SPEED);
	} else if (x >= 0.4) {
		left_side->Set(MANUAL_SPEED);
		right_side->Set(MANUAL_SPEED);
	} else if (x <= -0.4) {
		left_side->Set(-MANUAL_SPEED);
		right_side->Set(-MANUAL_SPEED);
	} else {
		left_side->Set(0.0);
		right_side->Set(0.0);
	}

	roller_set = true;
}
