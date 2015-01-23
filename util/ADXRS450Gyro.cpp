/*
 * ADXRS450Gyro.cpp
 *
 *  Created on: Jan 18, 2015
 *      Author: ratpack
 */

#include "ADXRS450Gyro.h"

ADXRS450Gyro::ADXRS450Gyro() {
	spi = new SPI(SPI::kOnboardCS0);
	spi->SetClockRate(1000000); //4 MHz (rRIO max, gyro can go high)
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
	current_rate = 0.0;
	accumulated_offset = 0.0;
	rate_offset = 0.0;
	update_timer = new Timer();
	calibration_timer = new Timer();

}

ADXRS450Gyro::~ADXRS450Gyro() {
	// TODO Auto-generated destructor stub
}


int ADXRS450Gyro::GetData() {
	check_parity(command);
	spi->Transaction(command, data, DATA_SIZE); //perform transaction, get error code

	//int status = (data[0] & 0x0C) >> 2; //status bits
	/*
	if (err != 0 || status == 0x00) { //there was an error somewhere
		return 1;
	}
	*/
	return assemble_sensor_data(data);
}

void ADXRS450Gyro::Update() {

	calibration_timer->Start();

	if (calibration_timer->Get() < 3.0){
		Calibrate();
	} else {
		UpdateData();
	}
}
void ADXRS450Gyro::UpdateData() {
	int sensor_data = GetData();
	float rate = ((float) sensor_data) / 80.0;

	current_rate = rate;
	current_rate -= rate_offset;
	update_timer->Start();
	accumulated_angle += update_timer->Get() * current_rate;
	update_timer->Reset();
}

void ADXRS450Gyro::Calibrate() {
	int sensor_data = GetData();
	float rate = ((float) sensor_data) / 80.0;
	update_timer->Start();
	calibration_timer->Start();
	accumulated_offset += rate * update_timer->Get();
	rate_offset = accumulated_offset / calibration_timer->Get();
	update_timer->Reset();
}



float ADXRS450Gyro::GetRate() {
	return current_rate;
}

float ADXRS450Gyro::GetAngle() {
	return accumulated_angle;
}

float ADXRS450Gyro::Offset() {
	return rate_offset;
}

void ADXRS450Gyro::Reset() {
	accumulated_angle = 0.0;
	rate_offset = 0.0;
	accumulated_offset = 0.0;
	calibration_timer->Reset();
	update_timer->Stop();
	update_timer->Reset();
}

short ADXRS450Gyro::assemble_sensor_data(unsigned char * data){
	//cast to short to make space for shifts
	//the 16 bits from the gyro are a 2's complement short
	//so just casting it to a C++ short gets it right
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
