/*
 * Roller.h
 *
 *  Created on: Jan 25, 2015
 *      Author: ratpack
 */

#ifndef SRC_ROLLER_H_
#define SRC_ROLLER_H_

#include "WPILib.h"

class Roller {
public:
	Roller(Victor * left_motor, Victor * right_motor, DigitalInput * linebreak_sensor);

	void Update();

	void RollIn();
	void RollOut();
	void Stop();
	void Rotate(float r);
	void Manual(float x, float y);

	bool ToteCaptured();

private:
	Victor * left_side;
	Victor * right_side;
	DigitalInput * line_break;

	bool roller_set;
	float left_turn;
	float right_turn;

	static constexpr float INTAKE_SPEED = 0.8;
	static constexpr float EJECT_SPEED = 0.3;
	static constexpr float MANUAL_SPEED = 0.2;

	enum RollState{
		kRollingIn, kRollingOut, kStopped, kManual
	};

	enum RollState state;

};

#endif /* SRC_ROLLER_H_ */
