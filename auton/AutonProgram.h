/*
 * AutonProgram.h
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#ifndef AUTON_AUTONPROGRAM_H_
#define AUTON_AUTONPROGRAM_H_

#include "../robot/Lifter.h"
#include "../robot/Roller.h"
#include "../robot/ToteHandler.h"
#include "../robot/MecanumDrive.h"

//abstract class to represent auton programs
class AutonProgram {
public:
	AutonProgram(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive);
	virtual ~AutonProgram();
	virtual void Init() = 0;
	virtual void Periodic() = 0;

protected:
	static constexpr float MOVE_TIME = 2.0; //the time it takes to strafe into the auto zone


	Lifter * lifter;
	Roller * roller;
	ToteHandler * tote_handler;
	MecanumDrive * drive;
	Timer * timer;
};



#endif /* AUTON_AUTONPROGRAM_H_ */
