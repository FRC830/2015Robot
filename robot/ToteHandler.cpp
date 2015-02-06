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
}
void ToteHandler::Update(){
	switch (current_state){
	case kGatheringBin:
		if (roller->ToteCaptured()){
			current_state = kDefault;
		}
		break;
	case kGatheringTote:
		if (roller->ToteCaptured()){
			roller->Stop();
			lifter->MoveToPosition(Lifter::kFloor);
			if (lifter->AtPosition(Lifter::kFloor)){
				current_state = kDefault;
			}
		}
		break;
	case kEjecting:
		//ejecting, may need to do something here later
		break;
	case kDefault:
		lifter->MoveToPosition(Lifter::kTote1);
		roller->Stop();
		break;
	case kFree:
		//let main program controll lifter and roller
		current_state = kDefault;
		break;
	}

	roller->Update();
	lifter->Update();
}
void ToteHandler::BinPickup(){
	lifter->MoveToPosition(Lifter::kFloor);
	roller->RollIn();
	current_state = kGatheringBin;

}
void ToteHandler::TotePickup(){
	lifter->MoveToPosition(Lifter::kTote1);
	roller->RollIn();
	current_state = kGatheringTote;
}
void ToteHandler::Eject(){
	roller->RollOut();
	current_state = kEjecting;
}
void ToteHandler::Override(){
	current_state = kFree;
}
void ToteHandler::Cancel(){
	current_state = kDefault;
}
