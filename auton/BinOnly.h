/*
 * BinOnly.h
 *
 *	Starting position: directly behind any starting recycling container
 *	Action: robot drives forward while picking up container, ends up in auto zone or just before it
 *	This has been tested at competition and works consistently
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#ifndef AUTON_BINONLY_H_
#define AUTON_BINONLY_H_

#include "AutonProgram.h"

class BinOnly : public AutonProgram {
public:
	BinOnly(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive);
	void Init();
	void Periodic();

private:
	enum State {kCalibrating, kGatheringBin, kMovingToAuto, kDone};
	enum State current_state;

};

#endif /* AUTON_BINONLY_H_ */
