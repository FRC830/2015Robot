/*
 * Lifter.h
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#ifndef LIFTER_H_
#define LIFTER_H_

#include "WPILib.h"

class Lifter {
public:
	enum Position {
		kFloor = 0, kBinPickup = 500, kCalibration = 1100, kHoldTote = 700, kStep = 1200, kRollersClear = 1500, kFeederPickup = 1400, kTote = 2350, kBelowStepBin = 2500, kStepBinPickup = 2850, kBin = 4400, kFeederGather = 4500, kMaxHeight = 6000
	};

	Lifter(Victor * lift_motor, Encoder * lift_enc, DigitalInput * bottom_limit_switch, DigitalInput * cal_switch);
	void Update();
	void MoveToPosition(enum Position target_pos);
	int DistFromPosition(enum Position target_pos); //returns distance from the position (positive: distance above, negative: distance below)
	bool AtPosition(enum Position target_pos); //returns true if we are at the target position
	void Calibrate(); //moves to the calibration switch position and recalibrates
	bool Calibrated();
	bool AtBottom();
	bool AtCalibrationPoint();

private:

	static constexpr float LIFT_SPEED_UP = -1.0; //speed to go up (negative = up)
	static constexpr float LIFT_SPEED_DOWN = 1.0; //speed to go down

	static const int MARGIN_OF_ERROR = 100;

	Victor * lifter;
	Encoder * encoder;
	DigitalInput * bottom_switch;
	DigitalInput * calibration_switch;

	enum Position target;
	bool calibrated; //whether we've calibrated
	int offset; //offset for encoder values, depends on where we've calibrated from
	bool above_cal; //let the user say whether we're above or below the calibration switch
};

#endif /* LIFTER_H_ */
