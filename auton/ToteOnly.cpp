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
}

void ToteOnly::Periodic(){
	switch (current_state) {
	case kCalibrating:
		if (tote_handler->Calibrated()){
			current_state = kMovingToAuto;
			//current_state = kGatheringTote;
			//tote_handler->GatherTote();
		}
		break;
	case kGatheringTote:
		drive->DriveCartesian(0.0, 0.5, 0.0);
		if (roller->ToteCaptured()){
			tote_handler->PickUpTote();
			timer->Reset();
			timer->Start();
			current_state = kMovingToAuto;
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 0.6, 0.0);
		if (timer->Get() >= MOVE_TIME) {
			current_state = kDone;
		}
		break;
	case kDone:
		break;
	}

	tote_handler->Update();



	SmartDashboard::PutString("auton mode", "tote");
}
