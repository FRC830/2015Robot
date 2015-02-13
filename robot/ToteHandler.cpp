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
	holding_bin = false;
}
void ToteHandler::Update(){
	switch (current_state){
	case kGatheringBin:
		if (roller->ToteCaptured()){
			holding_bin = true;
			current_state = kDefault;
		}
		break;
	case kGatheringTote:
		if (roller->ToteCaptured()){
			roller->Stop();
			lifter->MoveToPosition(Lifter::kFloor);
			if (lifter->AtPosition(Lifter::kFloor)){
				holding_bin = false;
				current_state = kDefault;
			}
		}
		break;
	case kEjecting:
		//ejecting, may need to do something here later
		break;
	case kDefault:
		if (holding_bin){
			lifter->MoveToPosition(Lifter::kBin);
		} else {
			lifter->MoveToPosition(Lifter::kTote);
		}
		roller->Stop();
		break;
	case kFree:
		//let main program control lifter and roller
		break;
	}

	roller->Update();
	lifter->Update();
}
void ToteHandler::BinPickup(){
	if (current_state != kGatheringBin) {
		lifter->MoveToPosition(Lifter::kBinPickup);
		roller->RollIn();
		current_state = kGatheringBin;
	}
}
void ToteHandler::TotePickup(){
	if (current_state != kGatheringTote) {
		lifter->MoveToPosition(Lifter::kTote);
		roller->RollIn();
		current_state = kGatheringTote;
	}
}
void ToteHandler::Eject(){
	if (current_state != kEjecting){
		roller->RollOut();
		current_state = kEjecting;
	}
}
void ToteHandler::Override(){
	current_state = kFree;
}
void ToteHandler::Cancel(){
	current_state = kDefault;
}
