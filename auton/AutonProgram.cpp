/*
 * AutonProgram.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "AutonProgram.h"

AutonProgram::AutonProgram(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive){
	lifter = lift;
	roller = roll;
	tote_handler = tote_h;
	drive = mec_drive;
	timer = new Timer();
}

AutonProgram::~AutonProgram() {

}



