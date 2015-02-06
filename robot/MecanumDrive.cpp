/*
 * MecanumDrive.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: ratpack
 */

#include "MecanumDrive.h"

MecanumDrive::MecanumDrive(RobotDrive * robot_drive, float accel_time) {

	drive = robot_drive;
	time_to_max_speed = accel_time;
	current_x = 0.0;
	current_y = 0.0;

}

void MecanumDrive::DriveCartesian(float x, float y, float rotation, float gyro_angle){
	//square inputs for greater precision
	curve_accel(&current_x, x*x);
	curve_accel(&current_y, y*y);
	drive->MecanumDrive_Cartesian(current_x, -current_y, rotation*rotation, gyro_angle);
}

void MecanumDrive::TestAll() {
	drive->ArcadeDrive(0.0, 0.3, false);
}

float MecanumDrive::curve_accel(float * current, float target){

	if (target<= DEAD_ZONE && target>= -DEAD_ZONE){
		target= 0.0;
	}

	float max_delta = 1.0 / (time_to_max_speed / SECS_PER_CYCLE); //1.0 represents the maximum value

	float delta = target - *current;

	if (delta > max_delta){
		*current = *current + max_delta;
	} else if (delta < -max_delta){
		*current = *current - max_delta;
	} else {
		*current = target;
	}
	//if controller is in full acceleration in either direction, set speed to zero instantly when reversed
	if ((*current > 0 && target < 0) || (*current < 0 && target > 0)) {
		*current = 0;
	}
	return *current;
}
MecanumDrive::~MecanumDrive() {
	// TODO Auto-generated destructor stub
}

