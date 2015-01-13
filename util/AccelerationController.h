/*
 * AccelerationController.h
 *
 *  Created on: Jan 12, 2015
 *      Author: ratpack
 */

#ifndef UTIL_ACCELERATIONCONTROLLER_H_
#define UTIL_ACCELERATIONCONTROLLER_H_

class AccelerationController {
public:
	AccelerationController(float time, float start_value = 0.0);
	virtual ~AccelerationController();

	void Set(float target_value, float time_span);
	float Get();
private:
	double time_to_max_speed;
	float value;
};

#endif /* ACCELERATIONCONTROLLER_H_ */
