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
	void RaiseTote();
	void GatherFromFeeder();
	void PickUpFromFeeder();
	void PickUp();
	void GoToStep();
	void GoToFloor();
	void Calibrate(bool from_above);
	void Override();
	void ReturnToDefault();

	void ManualRoller(float x, float y);
	void Eject();

	bool Calibrated();

	void IncreaseHeight();
	void DecreaseHeight();
private:
	Roller * roller;
	Lifter * lifter;
	enum Lifter::Position default_position; //depends on whether we're carrying a tote or bin

	Timer * timer;

	bool ejecting;
	enum HandlerState{
		kGatheringBin, kGatheringTote, kPickingUpTote, kGatheringFromFeeder, kPickingUpFromFeeder, kWaitingToRaise, kDefault, kCalibrating, kOverridden
	};
	enum HandlerState current_state;
};

#endif /* ROBOT_TOTEHANDLER_H_ */
