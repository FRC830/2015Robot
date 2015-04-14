/*
 * BinAndTote.h
 *
 *	This auton has not been tested in any capacity, and should be substantially redesigned if we want to do this
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#ifndef AUTON_BINANDTOTE_H_
#define AUTON_BINANDTOTE_H_

#include "AutonProgram.h"

class BinAndTote: public AutonProgram {
public:
	BinAndTote(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

private:
	enum State {kCalibrating, kGatheringBin, kRaisingBin, kGatheringTote, kMovingToAuto, kDone};
	enum State current_state;
};

#endif /* AUTON_BINANDTOTE_H_ */
