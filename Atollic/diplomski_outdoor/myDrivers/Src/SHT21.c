#include "SHT21.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t SHT21_Begin()
{
	return SHT21_CheckSensor();
}

uint8_t SHT21_CheckSensor()
{
	return HAL_I2C_Master_Transmit(&hi2c1, SHT21_ADDRESS, NULL, 0, 1000) == HAL_OK? 1:0;
}

int16_t SHT21_ReadHumidity() {
	// Start the measurment and get the data
	uint16_t _rh = SHT21_ReadRegister(SHT21_TRIG_HUM_MEAS_NO_HOLD);

	// Clean last two bits
	_rh &= ~0x0003;

	// Return relative humidity multiplied by ten to avoid using float;
  	return (int16_t)((-6.0 + 125.0/65536 * (float)_rh) * 100);
}

int16_t SHT21_ReadTemperature()
{
	// Start the measurment and get the data
	uint16_t _t = SHT21_ReadRegister(SHT21_TRIG_TEMP_MEAS_NO_HOLD);

	// Clean last two bits
	_t &= ~0x0003;

	// Return relative humidity multiplied by ten to avoid using float;
  	return (int16_t)((-46.85 + 175.72/65536 * (float)_t) * 100);
}

uint16_t SHT21_ReadRegister(uint8_t _reg)
{
	uint8_t _data[3];

	// Use No Hold Master Mode - No Clock Streching!
	HAL_I2C_Master_Transmit(&hi2c1, SHT21_ADDRESS, &_reg, 1, 1000);

	// Wait for measurment to be completed
	HAL_Delay(100);

	// Read the data
	HAL_I2C_Master_Receive(&hi2c1, SHT21_ADDRESS, _data, 3, 1000);

	// Pack it!
	return (uint16_t)(_data[0] << 8) | _data[1];
}

void SHT21_SoftReset()
{

}
