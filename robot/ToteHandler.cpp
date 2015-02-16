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
}
void ToteHandler::Update(){
	switch (current_state){
	case kGatheringBin:
		if (roller->ToteCaptured()){
			roller->Stop();
			default_position = Lifter::kBin;
			current_state = kDefault;
		}
		break;
	case kGatheringTote:
		if (roller->ToteCaptured()){
			roller->Stop();
			lifter->MoveToPosition(Lifter::kFloor);
			current_state = kPickingUpTote;
		}
		break;
	case kPickingUpTote:
		if (lifter->AtPosition(Lifter::kFloor)){
			default_position = Lifter::kTote;
			current_state = kDefault;
		}
		break;
	case kEjectingToFloor:
		if (lifter->AtPosition(Lifter::kFloor)){
			roller->RollOut();
		}
		break;
	case kEjectingToStep:
		if (lifter->AtPosition(Lifter::kStep)) {
			roller->RollOut();
		}
		break;
	case kDefault:
		lifter->MoveToPosition(default_position);
		roller->Stop();
		break;
	case kFree:
		//let main program control lifter and roller
		break;
	}

	roller->Update();
	lifter->Update();
}
void ToteHandler::PickupBin(){
	if (current_state != kGatheringBin) {
		lifter->MoveToPosition(Lifter::kFloor); //position above bin
		roller->RollIn();
		current_state = kGatheringBin;
	}
}
void ToteHandler::PickupTote(){
	if (current_state != kGatheringTote) {
		lifter->MoveToPosition(default_position);
		roller->RollIn();
		current_state = kGatheringTote;
	}
}
void ToteHandler::EjectToFloor(){
	if (current_state != kEjectingToFloor){
		lifter->MoveToPosition(Lifter::kFloor);
		current_state = kEjectingToFloor;
	}
}

void ToteHandler::EjectToStep() {
	if (current_state != kEjectingToStep){
		lifter->MoveToPosition(Lifter::kStep);
	}
	current_state = kEjectingToStep;
}

void ToteHandler::Override(){
	current_state = kFree;
}
void ToteHandler::Cancel(){
	current_state = kDefault;
}
