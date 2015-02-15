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
	void BinPickup();
	void TotePickup();
	void Eject();
	void Override();
	void Cancel();
private:
	Roller * roller;
	Lifter * lifter;
	enum Lifter::Position default_position; //depends on whether we're carrying a tote or bin


	enum HandlerState{
		kGatheringBin, kGatheringTote, kEjecting, kDefault, kFree
	};
	enum HandlerState current_state;
};

#endif /* ROBOT_TOTEHANDLER_H_ */
