/*
 * ToteOnly.h
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
	ToteOnly(Lifter * lift, Roller * roll, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

	enum State {kCalibrating, kGatheringTote, kMovingToAuto, kDone};
	enum State current_state;

	static constexpr float TIME_TO_GATHER_TOTE = 5.0;
	static constexpr float TIME_TO_MOVE = 10.0;
};

#endif /* AUTON_TOTEONLY_H_ */
