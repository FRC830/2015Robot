/*
 * ToteHandler.h
 *
 *  Created on: Feb 5, 2015
 *      Author: ratpack
 */

#ifndef ROBOT_TOTEHANDLER_H_
#define ROBOT_TOTEHANDLER_H_

#include "Lifter.h"
#include "Roller.h"

class ToteHandler {
public:
	ToteHandler(Roller * roll, Lifter * lift);
	void Update();
	void GatherBin();
	void PickUpBin();
	void GatherTote();
	void PickUpTote();
	void PickUp();
	void EjectToFloor();
	void EjectToStep();
	void Calibrate();
	void Override();
	void ReturnToDefault();

	bool Calibrated();

	void IncreaseHeight();
	void DecreaseHeight();
private:
	Roller * roller;
	Lifter * lifter;
	enum Lifter::Position default_position; //depends on whether we're carrying a tote or bin

	bool calibrated;

	enum HandlerState{
		kGatheringBin, kGatheringTote, kPickingUpTote, kEjectingToFloor, kEjectingToStep, kDefault, kCalibrating, kFree
	};
	enum HandlerState current_state;
};

#endif /* ROBOT_TOTEHANDLER_H_ */
