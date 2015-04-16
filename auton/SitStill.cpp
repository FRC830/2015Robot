/*
 * SitStill.cpp
 *
 *  Created on: Feb 23, 2015
 *      Author: ratpack
 */

#include "SitStill.h"

SitStill::SitStill(Lifter * lift, Roller * roll, ToteHandler * tote_h, MecanumDrive * mec_drive) : AutonProgram(lift, roll, tote_h, mec_drive) {

}

void SitStill::Init() {
	tote_handler->Calibrate();
}

void SitStill::Periodic() {
	drive->Stop(); //yaaaay
	tote_handler->Update();
}

