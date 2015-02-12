/*
 * ToteOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "ToteOnly.h"
#include "WPILib.h"

ToteOnly::ToteOnly(Lifter * lift, Roller * roll, MecanumDrive * mec_drive) : AutonProgram(lift, roll, mec_drive) {

}

void ToteOnly::Init() {

}

void ToteOnly::Periodic(){
	drive->Brake();
	SmartDashboard::PutString("auton mode", "tote");
}
