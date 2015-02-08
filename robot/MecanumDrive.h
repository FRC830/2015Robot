/*
 * MecanumDrive.h
 *
 *  Created on: Jan 18, 2015
 *      Author: ratpack
 */

#ifndef ROBOT_MECANUMDRIVE_H_
#define ROBOT_MECANUMDRIVE_H_

#include "WPILib.h"

class MecanumDrive {
public:
	MecanumDrive(RobotDrive * robot_drive, float accel_time);
	void DriveCartesian(float x, float y, float rotation, float gyro_angle = 0.0);
	void Brake();
	void TestAll();

private:
	float curve_accel(float * current, float target);
	static float square_input(float input);

	RobotDrive * drive;

	static constexpr float SECS_PER_CYCLE = 1 / 50.0; //approximate time for one rrio cycle
	static constexpr float DEAD_ZONE = 0.05;
	float time_to_max_speed;
	float current_x;
	float current_y;
};

#endif /* ROBOT_MECANUMDRIVE_H_ */
