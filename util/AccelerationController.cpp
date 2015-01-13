/*
 * AccelerationController.cpp
 *
 *  Created on: Jan 12, 2015
 *      Author: ratpack
 */

#include "AccelerationController.h"

AccelerationController::AccelerationController(float time, float start_value) {
	time_to_max_speed = time;
	value = start_value;
}

void AccelerationController::Set(float target_value, float time_span){
	float max_delta = 1.0 / (time_to_max_speed / time_span); //1.0 represents the maximum value

	float delta = target_value - value;

	if (delta > max_delta){
		value = value + max_delta;
	} else if (delta < -max_delta){
		value = value - max_delta;
	} else {
		value = target_value;
	}
}
float AccelerationController::Get(){
	return value;
}


AccelerationController::~AccelerationController() {}
