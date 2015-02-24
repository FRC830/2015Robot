/*
 * MoveForward.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#include "MoveForward.h"

MoveForward::MoveForward(Lifter * lift, Roller * roll, MecanumDrive * mec_drive) : AutonProgram(lift, roll, mec_drive) {
	current_state = kMovingToAuto;
}

void MoveForward::Init() {
	tote_handler->Calibrate();
	timer->Reset();
	timer->Start();
}

void MoveForward::Periodic() {
	switch (current_state) {
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 1.0, 0.0); //assumes we start out facing forwards
		if (timer->Get() >= TIME_TO_MOVE) {
			current_state = kDone;
		}
		break;
	case kDone:
		drive->Brake();
		break;
	}

	tote_handler->Update();
}

