/*
 * Roller.h
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#ifndef ROBOT_ROLLER_H_
#define ROBOT_ROLLER_H_

#include "WPILib.h"

class Roller {
public:
	Roller(Victor * intake_motor, DigitalInput * linebreak_sensor);

	void Update();

	void RollIn();
	void RollOut();
	void Stop();

	bool ToteCaptured();

private:
	Victor * intake;
	DigitalInput * line_break;

	static constexpr float SPEED = 0.8;

	enum RollState{
		kRollingIn, kRollingOut, kStopped
	};

	enum RollState state;

};

#endif /* ROBOT_ROLLER_H_ */