/*
 * ADXRS450Gyro.h
 *
 *  Created on: Jan 18, 2015
 *      Author: ratpack
 */

#ifndef UTIL_ADXRS450GYRO_H_
#define UTIL_ADXRS450GYRO_H_

#include "WPILib.h"

class ADXRS450Gyro {
public:
	ADXRS450Gyro();
	virtual ~ADXRS450Gyro();
	float GetRate();
	float GetAngle();
	void Reset();
	void Update();
	float Offset();


private:
	int GetData();
	void UpdateData();
	void Calibrate();

	void check_parity(unsigned char * command); //gyro requires odd parity for command
	int bits(unsigned char val); //returns number of on bits in a byte (helper for parity check)

	short assemble_sensor_data(unsigned char * data); //takes the sensor data from the data array and puts it into an int

	static const unsigned char DATA_SIZE = 4; //4 bytes = 32 bits
	static const unsigned char PARITY_BIT = 1; //parity check on first bit
	static const unsigned char FIRST_BYTE_DATA = 0x3; //mask to find sensor data bits on first byte
	static const unsigned char THIRD_BYTE_DATA = 0xFC; //mask to find sensor data bits on third byte
	static const unsigned char READ_COMMAND = 0x20; //0010 0000

	float accumulated_angle;
	Timer * update_timer;
	Timer * calibration_timer;
	float current_rate;
	float accumulated_offset;
	float rate_offset;

	unsigned char command[4];
	unsigned char data[4];
	SPI * spi;
};

#endif /* UTIL_ADXRS450GYRO_H_ */
