/*
 * BinOnly.h
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#ifndef AUTON_BINONLY_H_
#define AUTON_BINONLY_H_

#include "AutonProgram.h"

#include "../robot/Lifter.h"
#include "../robot/Roller.h"
#include "../robot/MecanumDrive.h"

class BinOnly : public AutonProgram {
public:
	BinOnly(Lifter * lift, Roller * roll, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

private:
	enum State {kCalibrating, kGatheringBin, kMovingToAuto, kDone};
	enum State current_state;
	static constexpr float TIME_TO_GATHER_BIN = 5.0;
	static constexpr float TIME_TO_MOVE = 10.0;

};

#endif /* AUTON_BINONLY_H_ */
