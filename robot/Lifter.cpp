/*
 * Lifter.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#include "Lifter.h"
#include <cmath>

Lifter::Lifter(Victor * lift_motor, Encoder * lift_enc, DigitalInput * bottom_limit_switch, DigitalInput * top_limit_switch) {
	lifter = lift_motor;
	encoder = lift_enc;
	bottom_switch = bottom_limit_switch;
	top_switch = top_limit_switch;
	target = kFloor;
}

void Lifter::Update() {
	//use bottom position as reference point for encoder
	if (bottom_switch->Get()) {
		//ignore while switch not wired in
		encoder->Reset();
	}

	//move lifter to the desired posidion
	int dist_to_target = DistFromPosition(target);
	if (dist_to_target > 0) {
		lifter->Set(LIFT_SPEED);
	} else if (dist_to_target < 0) {
		lifter->Set(-LIFT_SPEED);
	} else {
		lifter->Set(0.0);
	}

}
void Lifter::MoveToPosition(enum Position target_pos){
	 target = target_pos;
}
int Lifter::DistFromPosition(enum Position target_pos){
	int dist;
	switch (target_pos){
	case kFloor:
		if (bottom_switch->Get()) {
			return 0;
		} else {
			dist = encoder->Get();
			if (dist < MARGIN_OF_ERROR) {
				dist = MARGIN_OF_ERROR + 1;
			}
		}
		break;
	case kTote1:
		dist = (encoder->Get() - TOTE_1_POSITION);//TODO: Add margin of error
		break;
	case kTote2:
		dist = (encoder->Get() - TOTE_2_POSITION);
		break;
	}
	if (abs(dist) < MARGIN_OF_ERROR){
		return 0;
	} else {
		return dist;
	}
}
bool Lifter::AtPosition(enum Position target_pos){
	return DistFromPosition(target_pos) == 0;
}
