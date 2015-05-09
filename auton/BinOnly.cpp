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
		if (tote_handler->IsCalibrated()){
			current_state = kGatheringBin;
			tote_handler->GatherBin();
			timer->Reset();
		}
		break;
	case kGatheringBin:
		if (lifter->AtPosition(Lifter::kBinPickup)) {
			drive->DriveCartesian(0.0, 0.6, 0.0);
			timer->Start();
		}
		if (timer->Get() >= 1.5){
			timer->Reset();
			tote_handler->PickUpBin();
			current_state = kMovingToAuto;
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(0.0, 0.6, 0.0);
		if (timer->Get() >= 1.0) {
			current_state = kDone;
		}
		break;
	case kDone:
		drive->Stop();
		break;
	}

	tote_handler->Update();



	SmartDashboard::PutString("auton mode", "bin");
}
