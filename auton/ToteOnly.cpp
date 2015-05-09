/*
 * ToteOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "ToteOnly.h"
#include "WPILib.h"

ToteOnly::ToteOnly(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive) : AutonProgram(lift, roll, tote_h, mec_drive) {
	current_state = kTurning;
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
		drive->DriveCartesian(0.0, 0.0, 0.6); //eventually this should be based on the gyro or something
		if (timer->Get() >= 1.2) {
			current_state = kMovingToAuto;
			timer->Reset();
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 0.7, 0.0);
		if (timer->Get() >= 1.7) {
			current_state = kDone;
		}
		break;
	case kDone:
		drive->Stop();
		break;
	}

	tote_handler->Update();

	SmartDashboard::PutString("auton mode", "tote");
}
