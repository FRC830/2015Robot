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
		kFloor, kTote1, kTote2
	};

	Lifter(Victor * lift_motor, Encoder * lift_enc, DigitalInput * bottom_limit_switch, DigitalInput * top_limit_switch);
	void Update();
	void MoveToPosition(enum Position target_pos);
	int DistFromPosition(enum Position target_pos);
	bool AtPosition(enum Position target_pos);
private:
	static constexpr float LIFT_SPEED = 0.2;

	static const int TOTE_1_POSITION = 0; //enocder ticks to put lifter at height of 1 tote
	static const int TOTE_2_POSITION = 0; //TODO: empirically determine actual values
	static const int MARGIN_OF_ERROR = 5;

	Victor * lifter;
	Encoder * encoder;
	DigitalInput * bottom_switch;
	DigitalInput * top_switch;

	enum Position target;
};

#endif /* LIFTER_H_ */
