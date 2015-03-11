/*
 * BinOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "BinOnly.h"
#include "WPILib.h"

BinOnly::BinOnly(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive) : AutonProgram(lift, roll, tote_h, mec_drive) {
	current_state = kCalibrating;
}

void BinOnly::Init() {
	tote_handler->Calibrate();
	current_state = kCalibrating;
}

void BinOnly::Periodic(){
	switch (current_state) {
	case kCalibrating:
		if (tote_handler->Calibrated()){
			current_state = kGatheringBin;
			tote_handler->GatherBin();
			timer->Reset();
		}
		SmartDashboard::PutString("auton state", "calibrating");
		break;
	case kGatheringBin:
		if (lifter->AtPosition(Lifter::kBinPickup)) {
			drive->DriveCartesian(0.0, 0.5, 0.0);
			timer->Start();
		}
		if (timer->Get() >= BIN_TIME || roller->ToteCaptured()){
			timer->Reset();
			tote_handler->PickUpBin();
			current_state = kMovingToAuto;
		}
		SmartDashboard::PutString("auton state", "gathering bin");
		break;
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 0.5, 0.0); //we start out facing the bin, with the auto zone to our left, so strafe left
		if (timer->Get() >= (MOVE_TIME - BIN_TIME)) {
			current_state = kDone;
		}
		SmartDashboard::PutString("auton state", "moving");
		break;
	case kDone:
		SmartDashboard::PutString("auton state", "done");
		break;
	}

	tote_handler->Update();



	SmartDashboard::PutString("auton mode", "bin");
}
