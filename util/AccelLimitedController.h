/*
 * AccelLimitedController.h
 *
 * Our custom SpeedController class. It limits the acceleration of the motor.
 * Since it extends SpeedController, it can be used for e.g. RobotDrive objects.
 * It wraps another SpeedController. Any motor controller object can be used: Victor, Talon, etc.
 *
 * Created on: Jan 12, 2015
 * Author: ratpack
 */

#ifndef UTIL_ACCELLIMITEDCONTROLLER_H_
#define UTIL_ACCELLIMITEDCONTROLLER_H_

#include "WPILib.h"

class AccelLimitedController : public SpeedController {
public:
	explicit AccelLimitedController(SpeedController * controller, float accel_time, float cycle_time=SECS_PER_CYCLE);
	virtual ~AccelLimitedController();

	virtual void Set(float speed, uint8_t syncGroup = 0 ); //sets value, incorporating acceleration control and deadzone
	virtual void PIDWrite(float output); //identical to SetRaw
	virtual void SetRaw(float speed); //sets the value of the wrapped motor controller directly
	virtual float Get();
	virtual void Disable();

private:
	static constexpr float SECS_PER_CYCLE = 1 / 50.0; //approximate time for one rrio cycle
	static constexpr float DEAD_ZONE = 0.05;
	SpeedController * controller;
	float time_to_max_speed;
	float time_of_cycle;
	float value;

};

#endif /* ACCELERATIONCONTROLLER_H_ */
