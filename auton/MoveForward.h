/*
 * MoveForward.h
 *
 *	Starting position: anywhere facing towards auto zone
 *	Action: robot drives forward into auto zone
 *	This auton has been tested at competitions and works consistenly
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#ifndef AUTON_MOVEFORWARD_H_
#define AUTON_MOVEFORWARD_H_

#include "AutonProgram.h"

class MoveForward: public AutonProgram {
public:
	MoveForward(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

private:
	enum State {kMovingToAuto, kDone};
	enum State current_state;
};

#endif /* AUTON_MOVEFORWARD_H_ */
