/*
 * MoveForward.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#include "MoveForward.h"

MoveForward::MoveForward(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive) : AutonProgram(lift, roll, tote_h, mec_drive) {
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
		drive->DriveCartesian(0.0, 0.5, 0.0); //assumes we start out facing forwards
		if (timer->Get() >= MOVE_TIME) {
			current_state = kDone;
		}
		break;
	case kDone:
		drive->Brake();
		break;
	}

	tote_handler->Update();
}

