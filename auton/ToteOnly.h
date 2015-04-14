/*
 * ToteOnly.h
 *
 *	Starting position: holding any yellow tote between the rollers
 *	Action: robot turns 90 degrees while holding tote, then drives forward into auto zone (without picking up the tote)
 *  This auton has been tested at competitions and works consistently
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#ifndef AUTON_TOTEONLY_H_
#define AUTON_TOTEONLY_H_

#include "AutonProgram.h"

#include "../robot/Lifter.h"
#include "../robot/Roller.h"
#include "../robot/MecanumDrive.h"

class ToteOnly : public AutonProgram {
public:
	ToteOnly(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

	enum State {kTurning, kMovingToAuto, kDone};
	enum State current_state;
};

#endif /* AUTON_TOTEONLY_H_ */
