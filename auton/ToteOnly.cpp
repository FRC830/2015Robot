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
	current_state = kCalibrating;
}

void ToteOnly::Periodic(){
	switch (current_state) {
	case kCalibrating:
		if (tote_handler->Calibrated()){
			current_state = kGatheringTote;
			roller->RollIn();
			timer->Reset();
			timer->Start();
		}
		break;
	case kGatheringTote:
		if (roller->ToteCaptured() && timer->Get() >= TIME_TO_GATHER_TOTE){
			tote_handler->PickUpTote();
			timer->Reset();
			timer->Start();
			current_state = kTurning;
		}
		break;
	case kTurning:
		//turn 90 degrees to face forwards
		drive->DriveCartesian(0.0, 0.0, 0.3); //eventually this should be based on the gyro or something
		if (timer->Get() >= TIME_TO_ROTATE) {
			current_state = kMovingToAuto;
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 0.5, 0.0);
		if (timer->Get() >= MOVE_TIME) {
			current_state = kDone;
		}
		break;
	case kDone:
		drive->Brake();
		break;
	}

	tote_handler->Update();



	SmartDashboard::PutString("auton mode", "tote");
}
