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
	Roller(Victor * left_motor, Victor * right_motor, DigitalInput * linebreak_sensor);

	void Update();

	void RollIn();
	void RollOut();
	void Stop();

	void SetSpeed(float new_speed);
	void SetRotation(float rotate_val); //run the motors at different speeds to pull the tote into a better position

	bool ToteCaptured();

private:
	Victor * left_side;
	Victor * right_side;
	DigitalInput * line_break;

	float speed;
	float left_side_coefficient;
	float right_side_coefficient;

	enum RollState{
		kRollingIn, kRollingOut, kStopped
	};

	enum RollState state;

};

#endif /* ROBOT_ROLLER_H_ */
