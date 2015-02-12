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
	int DistFromPosition(enum Position target_pos); //returns distance from the position (positive: distance above, negative: distance below)
	bool AtPosition(enum Position target_pos);

private:
	static constexpr float LIFT_SPEED_UP = -0.3; //speed to go up (negative = up)
	static constexpr float LIFT_SPEED_DOWN = 0.2; //speed to go down

	static const int TOTE_POSITION = 450; //encoder ticks to pick up a tote to be able to pick up another tote
	static const int BIN_POSITION = 600; //encoder ticks to lift up a bin to be able to pick up a tote
	static const int MARGIN_OF_ERROR = 20; //TODO: check these values (I got them by messing with the lifter and some game pieces

	Victor * lifter;
	Encoder * encoder;
	DigitalInput * bottom_switch;
	DigitalInput * top_switch;

	enum Position target;
};

#endif /* LIFTER_H_ */
