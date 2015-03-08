/*
 * SitStill.h
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#ifndef AUTON_SITSTILL_H_
#define AUTON_SITSTILL_H_

#include "AutonProgram.h"

class SitStill: public AutonProgram {
public:
	SitStill(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive);
	void Init();
	void Periodic();
};

#endif /* AUTON_SITSTILL_H_ */
