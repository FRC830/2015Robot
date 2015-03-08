/*
 * ToteHandler.cpp
 *
 *  Created on: Feb 5, 2015
 *      Author: ratpack
 */

#include "ToteHandler.h"
#include "Lifter.h"
#include "Roller.h"

ToteHandler::ToteHandler(Roller * roll, Lifter * lift) {
	roller = roll;
	lifter = lift;
	current_state = kDefault;
	default_position = Lifter::kTote;

	timer = new Timer();
	calibrated = false;
	ejecting = false;

}
void ToteHandler::Update(){
	switch (current_state){
	case kGatheringBin:
		if (roller->ToteCaptured()){
			PickUpBin();
		}
		break;
	case kGatheringTote:
		/*
		if (roller->ToteCaptured()){
			PickUpTote();
		}
		*/
		break;
	case kPickingUpTote:
		if (lifter->AtPosition(Lifter::kFloor)){
			timer->Start();
			timer->Reset();
			current_state = kWaitingToRaise;
			//roller->RollIn();
		}
		break;
	case kWaitingToRaise:
		if (timer->Get() >= 0.5) {
			default_position = Lifter::kHoldTote;
			timer->Stop();
			timer->Reset();
			ReturnToDefault();
		}
		break;
	case kCalibrating:
		if (lifter->AtPosition(Lifter::kFloor)) {
			ReturnToDefault();
		}
		break;
	case kDefault:
		if (ejecting) {
			roller->RollOut();
			ejecting = false;
		} else {
			roller->Stop();
		}
		break;
	case kOverridden:
		//let main program control lifter and roller
		break;
	}

	if (lifter->AtPosition(Lifter::kFloor)) {
		calibrated = true;
	}

	if (current_state == kOverridden) {
		current_state = kDefault;
	} else {
		lifter->Update();
		roller->Update();
	}
}
void ToteHandler::GatherBin(){
	if (current_state != kGatheringBin) {
		lifter->MoveToPosition(Lifter::kBinPickup);
		roller->RollIn();
		current_state = kGatheringBin;
	}
}

void ToteHandler::PickUpBin() {
	roller->Stop();
	default_position = Lifter::kBin;
	ReturnToDefault();
}

void ToteHandler::GatherTote(){
	if (current_state != kGatheringTote) {
		if (default_position != Lifter::kBin) {
			default_position = Lifter::kTote;
		}
		lifter->MoveToPosition(default_position);
		roller->RollIn();
		current_state = kGatheringTote;
	}
}

void ToteHandler::PickUpTote() {
	if (current_state != kPickingUpTote && roller->ToteCaptured()) {
		roller->Stop();
		lifter->MoveToPosition(Lifter::kFloor);
		current_state = kPickingUpTote;
	}
}

void ToteHandler::RaiseTote() {
	if (current_state == kPickingUpTote && lifter->AtPosition(Lifter::kFloor)){
		default_position = Lifter::kHoldTote;
		ReturnToDefault();
	}
}

void ToteHandler::PickUp() {
	if (current_state == kGatheringBin) {
		PickUpBin();
	} else {
		PickUpTote();
	}
}

void ToteHandler::Eject(){
	ejecting = true;
}

void ToteHandler::GoToStep() {
	if (current_state == kDefault) {
		default_position = Lifter::kStep;
		lifter->MoveToPosition(default_position);
	}
}

void ToteHandler::GoToFloor() {
	if (current_state == kDefault) {
		default_position = Lifter::kFloor;
		lifter->MoveToPosition(default_position);
	}
}

void ToteHandler::Calibrate() {
	if (current_state != kCalibrating){
		roller->Stop();
		lifter->MoveToPosition(Lifter::kFloor);
	}
	current_state = kCalibrating;
}

void ToteHandler::Override(){
	current_state = kOverridden;
}
void ToteHandler::ReturnToDefault(){
	lifter->MoveToPosition(default_position);
	roller->Stop();
	current_state = kDefault;
}

void ToteHandler::ManualRoller(float x, float y) {
	if (current_state == kPickingUpTote && lifter->AtPosition(Lifter::kFloor)) {
		roller->Manual(x, y);
	}
}

bool ToteHandler::Calibrated() {
	return calibrated;
}

void ToteHandler::IncreaseHeight() {
	if (current_state == kDefault) {
		switch (default_position) {
		case Lifter::kHoldTote: default_position = Lifter::kTote; break;
		case Lifter::kTote: default_position = Lifter::kBin; break;
		case Lifter::kBin: default_position = Lifter::kMaxHeight; break;
		default: return;
		}
	lifter->MoveToPosition(default_position);
	}
}

void ToteHandler::DecreaseHeight() {
	if (current_state == kDefault) {
	switch (default_position) {
		case Lifter::kMaxHeight: default_position = Lifter::kBin; break;
		case Lifter::kBin: default_position = Lifter::kTote; break;
		default: return;
		}
	}
	lifter->MoveToPosition(default_position);
}
