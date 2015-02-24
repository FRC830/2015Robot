/*
 * BinAndTote.h
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#ifndef AUTON_BINANDTOTE_H_
#define AUTON_BINANDTOTE_H_

#include "AutonProgram.h"

class BinAndTote: public AutonProgram {
public:
	BinAndTote(Lifter * lift, Roller * roll, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

private:
	enum State {kCalibrating, kGatheringBin, kRaisingBin, kGatheringTote, kMovingToAuto, kDone};
	enum State current_state;
	static constexpr float TIME_TO_GATHER_BIN = 5.0;
	static constexpr float TIME_TO_RAISE_BIN = 1.0;
	static constexpr float TIME_TO_MOVE = 8.0;
};

#endif /* AUTON_BINANDTOTE_H_ */
