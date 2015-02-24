/*
 * BinOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "BinOnly.h"
#include "WPILib.h"

BinOnly::BinOnly(Lifter * lift, Roller * roll, MecanumDrive * mec_drive) : AutonProgram(lift, roll, mec_drive) {
	current_state = kCalibrating;
}

void BinOnly::Init() {
	tote_handler->Calibrate();
}

void BinOnly::Periodic(){
	switch (current_state) {
	case kCalibrating:
		if (tote_handler->Calibrated()){
			current_state = kGatheringBin;
			tote_handler->GatherBin();
			timer->Reset();
			timer->Start();
		}
		break;
	case kGatheringBin:
		drive->DriveCartesian(0.0, 0.5, 0.0);
		if (timer->Get() >= TIME_TO_GATHER_BIN || roller->ToteCaptured()){
			timer->Reset();
			tote_handler->PickUpBin();
			current_state = kMovingToAuto;
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(-1.0, 0.0, 0.0); //we start out facing the bin, with the auto zone to our left, so strafe left
		if (timer->Get() >= TIME_TO_MOVE) {
			current_state = kDone;
		}
		break;
	case kDone:
		break;
	}

	tote_handler->Update();



	SmartDashboard::PutString("auton mode", "bin");
}
