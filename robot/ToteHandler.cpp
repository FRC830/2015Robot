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
	current_state = kCalibrating;
	default_position = Lifter::kTote;
	calibrated = false;
}
void ToteHandler::Update(){
	switch (current_state){
	case kGatheringBin:
		if (roller->ToteCaptured()){
			PickUpBin();
		}
		break;
	case kGatheringTote:
		if (roller->ToteCaptured()){
			PickUpTote();
		}
		break;
	case kPickingUpTote:
		if (lifter->AtPosition(Lifter::kFloor)){
			default_position = Lifter::kTote;
			ReturnToDefault();
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
	case kCalibrating:
		if (lifter->AtPosition(Lifter::kFloor)) {
			calibrated = true;
			default_position = Lifter::kTote;
			ReturnToDefault();
		}
		break;
	case kDefault:
		break;
	case kFree:
		//let main program control lifter and roller
		break;
	}

	roller->Update();
	lifter->Update();
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
		lifter->MoveToPosition(default_position);
		roller->RollIn();
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

void ToteHandler::Calibrate() {
	if (current_state != kCalibrating){
		lifter->MoveToPosition(Lifter::kFloor);
	}
	current_state = kCalibrating;
}

void ToteHandler::Override(){
	current_state = kFree;
}
void ToteHandler::ReturnToDefault(){
	if (current_state != kDefault) {
		lifter->MoveToPosition(default_position);
		roller->Stop();
		current_state = kDefault;
	}
}

bool ToteHandler::Calibrated() {
	return calibrated;
}

void ToteHandler::IncreaseHeight() {
	switch (default_position) {
	case Lifter::kFloor: default_position = Lifter::kTote; break;
	case Lifter::kTote: default_position = Lifter::kBin; break;
	case Lifter::kBin: default_position = Lifter::kMaxHeight; break;
	default: return;
	}
}

void ToteHandler::DecreaseHeight() {
	switch (default_position) {
	case Lifter::kMaxHeight: default_position = Lifter::kBin; break;
	case Lifter::kBin: default_position = Lifter::kTote; break;
	case Lifter::kTote: default_position = Lifter::kFloor; break;
	default: return;
	}
}
