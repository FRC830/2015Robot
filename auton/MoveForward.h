/*
 * MoveForward.h
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#ifndef AUTON_MOVEFORWARD_H_
#define AUTON_MOVEFORWARD_H_

#include "AutonProgram.h"

class MoveForward: public AutonProgram {
public:
	MoveForward(Lifter * lift, Roller * roll, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

private:
	enum State {kMovingToAuto, kDone};
	enum State current_state;
	static constexpr float TIME_TO_MOVE = 10.0;
};

#endif /* AUTON_MOVEFORWARD_H_ */
