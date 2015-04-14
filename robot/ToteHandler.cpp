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
	ejecting = false;

}
void ToteHandler::Update(){
	switch (current_state){
	case kGatheringBin:
		break;
	case kGatheringTote:
		//don't run rollers until the tote is clear
		//so the totes will line up better
		if (lifter->DistFromPosition(Lifter::kRollersClear) >= 0) {
			roller->RollIn();
		}
		break;
	case kPickingUpTote:
		if (lifter->AtPosition(Lifter::kFloor)){
			timer->Start();
			timer->Reset();
			current_state = kWaitingToRaise;
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
	case kGatheringFromFeeder: break;
	case kPickingUpFromFeeder:
		if (lifter->AtPosition(Lifter::kFeederPickup)) {
			default_position = Lifter::kTote;
			ReturnToDefault();
		}
		break;
	case kCalibrating:
		if (lifter->IsCalibrated()) {
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

	//overriding must be continuously refreshed, otherwise control returns to the tote handler
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
		//we don't run the rollers here, because the update loop will start them when we've lifted the tote stack out of the rollers
		current_state = kGatheringTote;
	}
}

void ToteHandler::PickUpTote() {
	if (current_state != kPickingUpTote) {
		roller->Stop();
		lifter->MoveToPosition(Lifter::kFloor);
		current_state = kPickingUpTote;
	}
}

void ToteHandler::GatherFromFeeder() {
	if (current_state != kGatheringFromFeeder) {
		default_position = Lifter::kFeederGather;
		lifter->MoveToPosition(default_position);
		roller->RollIn();
		current_state = kGatheringFromFeeder;
	}
}

void ToteHandler::PickUpFromFeeder() {
	if (current_state != kPickingUpFromFeeder) {
		roller->Stop();
		lifter->MoveToPosition(Lifter::kFeederPickup);
		current_state = kPickingUpFromFeeder;
	}
}

void ToteHandler::PickUp() {
	//call the pick up function appropriate to what we're picking up
	//this has no default case, as in such a case it's impossible to know for sure if we're picking up a bin or tote
	//and performing the wrong pick up operation is bad
	if (current_state == kGatheringBin) {
		PickUpBin();
	} else if (current_state == kGatheringFromFeeder) {
		PickUpFromFeeder();
	} else if (current_state == kGatheringTote){
		PickUpTote();
	}
}

void ToteHandler::Eject(){
	ejecting = true; //ejecting isn't a separate state but an action that can be performed in default mode
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
		lifter->Calibrate();
	}
	current_state = kCalibrating;
}

void ToteHandler::Override(){
	current_state = kOverridden;
}
void ToteHandler::ReturnToDefault(){
	if (current_state != kDefault || !lifter->AtPosition(default_position)) {
		lifter->MoveToPosition(default_position);
		roller->Stop();
		current_state = kDefault;
	}
}

void ToteHandler::ManualRoller(float x, float y) {
	if (current_state == kPickingUpTote && lifter->AtPosition(Lifter::kFloor)) {
		roller->Manual(x, y);
	}
}

bool ToteHandler::IsCalibrated() {
	return lifter->IsCalibrated();
}

//allow the user to cycle upwards through useful heights
//this is most important for picking up bins from the step
void ToteHandler::IncreaseHeight() {
	if (current_state == kDefault) {
		switch (default_position) {
		case Lifter::kFloor: default_position = Lifter::kStep; break;
		case Lifter::kHoldTote: default_position = Lifter::kStep; break;
		case Lifter::kStep:
		case Lifter::kTote: default_position = Lifter::kBelowStepBin; break; //it's not possible to reach with this control; this is because it's very close to kStepBinPickup and the copilot can't tell which position he's at
		case Lifter::kBelowStepBin: default_position = Lifter::kStepBinPickup; break;
		case Lifter::kStepBinPickup: default_position = Lifter::kBin; break;
		case Lifter::kBin: default_position = Lifter::kMaxHeight; break;
		default: return;
		}
	lifter->MoveToPosition(default_position);
	}
}


//...and cycle downwards
void ToteHandler::DecreaseHeight() {
	if (current_state == kDefault) {
	switch (default_position) {
		case Lifter::kMaxHeight: default_position = Lifter::kBin; break;
		case Lifter::kBin: default_position = Lifter::kStepBinPickup; break;
		case Lifter::kStepBinPickup: default_position = Lifter::kBelowStepBin; break;
		case Lifter::kTote:
		case Lifter::kBelowStepBin: default_position = Lifter::kStep; break;
		case Lifter::kStep: default_position = Lifter::kHoldTote; break;
		case Lifter::kHoldTote: default_position = Lifter::kFloor; break;
		default: return;
		}
	}
	lifter->MoveToPosition(default_position);
}
