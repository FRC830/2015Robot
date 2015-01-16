/*
 * AccelerationController.cpp
 *
 *  Created on: Jan 12, 2015
 *      Author: ratpack
 */

#include "AccelLimitedController.h"
#include "WPILib.h"

AccelLimitedController::AccelLimitedController(SpeedController * con, float accel_time, float cycle_time) {
	controller = con;
	time_to_max_speed = accel_time;
	time_of_cycle = cycle_time;
	value = 0.0;
}

void AccelLimitedController::Set(float speed, uint8_t syncGroup){
	float max_delta = 1.0 / (time_to_max_speed / time_of_cycle); //1.0 represents the maximum value


	if (speed <= DEAD_ZONE && speed >= -DEAD_ZONE){//grounds the input
		speed = 0.0;
	}

	float delta = speed - value;

	if (delta > max_delta){
		value = value + max_delta;
	} else if (delta < -max_delta){
		value = value - max_delta;
	} else {
		value = speed;
	}


	controller->Set(value, syncGroup);
}

void AccelLimitedController::SetRaw(float speed) {
	controller->Set(speed);
}
void AccelLimitedController::PIDWrite(float output){
	SetRaw(output);
}
float AccelLimitedController::Get(){
	return value;
}


void AccelLimitedController::Disable() {
	controller->Disable();
}

AccelLimitedController::~AccelLimitedController() {}
