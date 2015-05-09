/*
 * BinAndTote.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#include "BinAndTote.h"

BinAndTote::BinAndTote(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive) : AutonProgram(lift, roll, tote_h, mec_drive) {
	current_state = kCalibrating;
}

void BinAndTote::Init() {
	tote_handler->Calibrate();
	current_state = kCalibrating;
}

void BinAndTote::Periodic() {
	switch (current_state) {
	case kCalibrating:
		if (tote_handler->IsCalibrated()){
			current_state = kGatheringBin;
			tote_handler->GatherBin();
			timer->Reset();
			timer->Start();
		}
		break;
	case kGatheringBin:
		drive->DriveCartesian(0.0, 0.2, 0.0);
		if (timer->Get() >= 1.2){
			timer->Reset();
			tote_handler->PickUpBin();
			current_state = kRaisingBin;
		}
		break;
	case kRaisingBin:
		if (lifter->AtPosition(Lifter::kBin)) {
			tote_handler->GatherTote();
			current_state = kGatheringTote;
		}
		break;
	case kGatheringTote:
		drive->DriveCartesian(0.0, 0.4, 0.0);
		if (roller->ToteCaptured()) {
			tote_handler->PickUpTote();
			timer->Reset();
			current_state = kMovingToAuto;
		}
		break;
	case kMovingToAuto:
		drive->DriveCartesian(-0.5, 0.0, 0.0); //we start out facing the bin, with the auto zone to our left, so strafe left
		if (timer->Get() >= 2.0) {
			current_state = kDone;
		}
		break;
	case kDone:
		break;
	}

	tote_handler->Update();
}

