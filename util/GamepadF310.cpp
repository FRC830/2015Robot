/*
 * GamepadF310.cpp
 *
 *  Created on: Jan 5, 2015
 *      Author: ratpack
 */

#include "GamepadF310.h"

GamepadF310::GamepadF310(int port) {
	joystick = new Joystick(port);
}

GamepadF310::~GamepadF310() {

}


float GamepadF310::LeftX() {
	return joystick->GetRawAxis(LEFT_X_AXIS_NUM);
}
float GamepadF310::LeftY() {
	//this one is backwards
	return -joystick->GetRawAxis(LEFT_Y_AXIS_NUM);
}
float GamepadF310::LeftTrigger() {
	return joystick->GetRawAxis(LEFT_TRIGGER_NUM);
}
float GamepadF310::RightTrigger() {
	return joystick->GetRawAxis(RIGHT_TRIGGER_NUM);
}
float GamepadF310::RightX() {
	return joystick->GetRawAxis(RIGHT_X_AXIS_NUM);
}
float GamepadF310::RightY() {
	//this one is backwards
	return -joystick->GetRawAxis(RIGHT_Y_AXIS_NUM);
}

float GamepadF310::DPadX() {
	//the joystick class treats the DPad like a POV switch, i.e. a little control stick
	//it returns an angle giving its position clockwise from straight up
	//on the DPad on the angles will be multiples of 45 degrees
	//this is written so that the DPad has the same interface as the sticks
	//which is good because the mode button switches the DPad and left stick
	//(which is also why we use <=, even though the physical DPad won't give in-between values
	float pov = joystick->GetPOV();
	if (45.0 <= pov && pov <= 135.0){
		return 1.0;
	} else if (225.0 <= pov && pov <= 315.0) {
		return -1.0;
	} else {
		return 0.0;
	}
}

float GamepadF310::DPadY() {
	//(see comments on DPadX)

	float pov = joystick->GetPOV();
	if (315.0 <= pov ||(0.0 <= pov && pov <= 45.0) ){
		return 1.0;
	} else if (135.0 <= pov && pov <= 225.0) {
		return -1.0;
	} else {
		return 0.0;
	}
}


bool GamepadF310::Button(int buttonNum) {
	return joystick->GetRawButton(buttonNum);
}

//the stick presses are actually just considered buttons
bool GamepadF310::LeftStickPress() {
	return joystick->GetRawButton(9);
}
bool GamepadF310::RightStickPress() {
	return joystick->GetRawButton(10);
}

void GamepadF310::RightRumble(float rumbleness){
	joystick->SetRumble(Joystick::kRightRumble, rumbleness);
}

void GamepadF310::LeftRumble(float rumbleness){
	joystick->SetRumble(Joystick::kLeftRumble, rumbleness);
}
