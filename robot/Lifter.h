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
		kFloor, kTote, kBin
	};

	Lifter(Victor * lift_motor, Encoder * lift_enc, DigitalInput * bottom_limit_switch, DigitalInput * top_limit_switch);
	void Update();
	void MoveToPosition(enum Position target_pos);
	int DistFromPosition(enum Position target_pos);
	bool AtPosition(enum Position target_pos);

private:
	static constexpr float LIFT_SPEED = 0.3;

	static const int TOTE_POSITION = 1000; //encoder ticks to pick up a tote to be able to pick up another tote
	static const int BIN_POSITION = 2000; //encoder ticks to lift up a bin to be able to pick up a tote
	static const int MARGIN_OF_ERROR = 50; //TODO: empirically determine actual values

	Victor * lifter;
	Encoder * encoder;
	DigitalInput * bottom_switch;
	DigitalInput * top_switch;

	enum Position target;
};

#endif /* LIFTER_H_ */
