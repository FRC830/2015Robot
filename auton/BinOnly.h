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

};

#endif /* AUTON_BINONLY_H_ */
