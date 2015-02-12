/*
 * BinOnly.cpp
 *
 *  Created on: Feb 11, 2015
 *      Author: ratpack
 */

#include "BinOnly.h"
#include "WPILib.h"

BinOnly::BinOnly(Lifter * lift, Roller * roll, MecanumDrive * mec_drive) : AutonProgram(lift, roll, mec_drive) {

}

void BinOnly::Init() {

}

void BinOnly::Periodic(){
	drive->Brake();
	SmartDashboard::PutString("auton mode", "bin");
}
