/*
 * Roller.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#include "Roller.h"

Roller::Roller(Victor * intake_motor, DigitalInput * linebreak_sensor) {
	intake = intake_motor;
	line_break = linebreak_sensor;
	state = kStopped;

}
void Roller::Update(){
	switch (state){
	case kStopped:
		intake->Set(0.0);
		break;
	case kRollingIn:
		if (ToteCaptured()){
			state = kStopped;
			intake->Set(0.0);
		}else{
			intake->Set(SPEED);
		}
		break;
	case kRollingOut:
		intake->Set(-SPEED);
	}
}
bool Roller::ToteCaptured(){
	return line_break->Get();
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
