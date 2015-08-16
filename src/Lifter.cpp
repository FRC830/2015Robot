/*
 * Lifter.cpp
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#include "../src/Lifter.h"

#include "WPILib.h"
#include <cmath>

Lifter::Lifter(Victor * lift_motor, Encoder * lift_enc, DigitalInput * bottom_limit_switch, DigitalInput * top_limit_switch) {
	lifter = lift_motor;
	encoder = lift_enc;
	bottom_switch = bottom_limit_switch;
	calibration_switch = top_limit_switch;
	target = kCalibration;
	calibrated = false;
	offset = 0;
	above_cal = true;
	//SmartDashboard::PutBoolean("above cal switch", true);
}

void Lifter::Update() {
	//use bottom position as reference point for encoder
	if (AtBottom()) {
		encoder->Reset();
		calibrated = true;
		offset = 0;
	}

	//calibrate using linebreak if necessary
	if (AtCalibrationPoint() && !calibrated) {
		encoder->Reset();
		calibrated = true;
		offset = (int) kCalibration;
	}

	//move lifter to the desired position
	int dist_to_target = DistFromPosition(target);
	if (dist_to_target > 0) {
		lifter->Set(LIFT_SPEED_DOWN);
	} else if (dist_to_target < 0) {
		lifter->Set(LIFT_SPEED_UP);
	} else {
		lifter->Set(0.0);
	}

}
void Lifter::MoveToPosition(enum Position target_pos){
	 target = target_pos;
}
int Lifter::DistFromPosition(enum Position target_pos){
	int dist;
	int encoder_val = encoder->Get() + offset;
	if (target_pos == kFloor){
		if (AtBottom()) {
			return 0;
		} else {
			dist = encoder_val; //since encoder measures from the bottom, encoder->Get() gives our distance from the bottom position
			if (dist > MARGIN_OF_ERROR) {
				return dist;
			} else {
				return MARGIN_OF_ERROR;
			}
		}
	} else if (target_pos == kCalibration && !calibrated) {
		if (AtCalibrationPoint()) {
			return 0;
		} else if (above_cal) {
				return MARGIN_OF_ERROR;
		} else {
			return -MARGIN_OF_ERROR;
		}
	} else {
		dist = encoder_val - ((int) target_pos);
		if (abs(dist) < MARGIN_OF_ERROR){
			return 0;
		} else {
			return dist;
		}
	}
}
bool Lifter::AtPosition(enum Position target_pos){
	return DistFromPosition(target_pos) == 0;
}

void Lifter::Calibrate() {
	calibrated = false;
	//above_cal = SmartDashboard::GetBoolean("above cal switch");
	above_cal = true; //ignore the smart dashboard switch for now
	MoveToPosition(kCalibration);
}

bool Lifter::Calibrated() {
	return calibrated;
}

bool Lifter::AtBottom() {
	return !bottom_switch->Get(); //switch returns false when pressed
}

bool Lifter::AtCalibrationPoint() {
	return calibration_switch->Get();
}
