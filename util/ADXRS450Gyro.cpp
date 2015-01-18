/*
 * ADXRS450Gyro.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: ratpack
 */

#include "ADXRS450Gyro.h"

ADXRS450Gyro::ADXRS450Gyro() {
	spi = new SPI(SPI::kOnboardCS0);
	spi->SetClockRate(4000000); //4 MHz (rRIO max, gyro can go high)
	spi->SetClockActiveHigh();
	spi->SetChipSelectActiveLow();

	command[0] = READ_COMMAND;
	command[1] = 0;
	command[2] = 0;
	command[3] = 0;
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;

	accumulated_angle = 0.0;
	timer = new Timer();

}

ADXRS450Gyro::~ADXRS450Gyro() {
	// TODO Auto-generated destructor stub
}

float ADXRS450Gyro::update() {
	check_parity(command);
	spi->Transaction(command, data, DATA_SIZE);

	//return (data[0] & 0x0C) >> 2; //return the "status" bits
	int sensor_data = assemble_sensor_data(data);
	int rate = ((float) sensor_data) / 80.0;
	timer->Start();
	accumulated_angle += timer->Get() * rate;
	timer->Reset();

	return accumulated_angle;
}

short ADXRS450Gyro::assemble_sensor_data(unsigned char * data){
	//cast to int to make space for shifts
	return ((short) (data[0] & FIRST_BYTE_DATA)) << 14
		| ((short) data[1]) << 6
		| ((short) (data[2] & THIRD_BYTE_DATA)) >> 2;
}

void ADXRS450Gyro::check_parity(unsigned char * command) {
	int num_bits = bits(command[0]) + bits(command[1]) + bits(command[2]) + bits(command[3]);
	if (num_bits % 2 == 0) {
		command[3] |= PARITY_BIT;
	}
}

int ADXRS450Gyro::bits(unsigned char val) {
	int n = 0;
	while (val) {
		val &= val-1;
		n += 1;
	}
	return n;
}
