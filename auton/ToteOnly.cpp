/*
 * ToteOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "ToteOnly.h"
#include "WPILib.h"

ToteOnly::ToteOnly(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive) : AutonProgram(lift, roll, tote_h, mec_drive) {
	current_state = kCalibrating;
}

void ToteOnly::Init() {
	tote_handler->Calibrate();
	current_state = kTurning;
	timer->Reset();
	timer->Start();
}

void ToteOnly::Periodic(){
	//this assumes a starting position where we start out with the tote already in our rollers
	switch (current_state) {
	//turn clockwise, while rolling in the tote to hold it
	case kTurning:
		//turn 90 degrees to face forwards
		drive->DriveCartesian(0.0, 0.0, 0.5); //eventually this should be based on the gyro or something
		if (timer->Get() >= TIME_TO_ROTATE) {
			current_state = kMovingToAuto;
			timer->Reset();
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 0.7, 0.0);
		if (timer->Get() >= MOVE_TIME) {
			current_state = kDone;
		}
		break;
	case kDone:
		drive->Brake();
		break;
	}

	tote_handler->Update();


	SmartDashboard::PutNumber("current state", (int) current_state);
	SmartDashboard::PutString("auton mode", "tote");
}
