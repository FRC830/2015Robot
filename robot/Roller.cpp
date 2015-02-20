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
	speed = 0.8;
	left_side_coefficient = 1.0;
	right_side_coefficient = 1.0;
	state = kStopped;

}
void Roller::Update(){
	switch (state){
	case kStopped:
		SetRotation(0.0);
		left_side->Set(0.0);
		right_side->Set(0.0);
		break;
	case kRollingIn:
		if (ToteCaptured()){
			state = kStopped;
			left_side->Set(0.0);
			right_side->Set(0.0);
		}else{
			left_side->Set(speed * left_side_coefficient);
			right_side->Set(-speed * right_side_coefficient);
			SmartDashboard::PutNumber("roller left", left_side_coefficient);
			SmartDashboard::PutNumber("roller right", right_side_coefficient);
		}
		break;
	case kRollingOut:
		SetRotation(0.0);
		left_side->Set(-speed);
		right_side->Set(speed);
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

void Roller::SetSpeed(float new_speed){
	speed = new_speed;
}

void Roller::SetRotation(float rotate_val){
	float left_side = 0.5 + rotate_val;
	float right_side = 0.5 - rotate_val;
	if (left_side > right_side){
		right_side_coefficient = right_side / left_side;
		left_side_coefficient = 1.0;
	} else {
		left_side_coefficient = left_side / right_side;
		right_side_coefficient = 1.0;
	}
}
