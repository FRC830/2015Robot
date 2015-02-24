/*
 * ToteOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "ToteOnly.h"
#include "WPILib.h"

ToteOnly::ToteOnly(Lifter * lift, Roller * roll, MecanumDrive * mec_drive) : AutonProgram(lift, roll, mec_drive) {
	current_state = kCalibrating;
}

void ToteOnly::Init() {
	tote_handler->Calibrate();
}

void ToteOnly::Periodic(){
	switch (current_state) {
	case kCalibrating:
		if (tote_handler->Calibrated()){
			current_state = kGatheringTote;
			tote_handler->PickUpTote();
		}
		break;
	case kGatheringTote:
		drive->DriveCartesian(0.0, 0.5, 0.0);
		if (roller->ToteCaptured()){
			timer->Reset();
			timer->Start();
			current_state = kMovingToAuto;
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(1.0, 0.0, 0.0); //we start out facing the tote, with the auto zone to our right, so strafe right
		if (timer->Get() >= TIME_TO_MOVE) {
			current_state = kDone;
			tote_handler->EjectToFloor(); //get rid of the yellow tote
		}
		break;
	case kDone:
		break;
	}

	tote_handler->Update();



	SmartDashboard::PutString("auton mode", "tote");
}
