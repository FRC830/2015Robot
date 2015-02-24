/*
 * AutonProgram.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "AutonProgram.h"

AutonProgram::AutonProgram(Lifter * lift, Roller * roll, MecanumDrive * mec_drive){
	lifter = lift;
	roller = roll;
	tote_handler = new ToteHandler(roller, lifter);
	drive = mec_drive;
	timer = new Timer();
}

AutonProgram::~AutonProgram() {

}



