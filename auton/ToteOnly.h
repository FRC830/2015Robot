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
};

#endif /* AUTON_TOTEONLY_H_ */
