#ifndef __SHT21_H__
#define __SHT21_H__

#include "stdint.h"
#include "stm32l0xx_hal.h"

#define SHT21_ADDRESS					0x80
#define SHT21_TRIG_TEMP_MEAS_NO_HOLD	0b11110011
#define SHT21_TRIG_HUM_MEAS_NO_HOLD		0b11110101

uint8_t SHT21_Begin();
uint8_t SHT21_CheckSensor();
uint16_t SHT21_ReadRegister(uint8_t _reg);
int16_t SHT21_ReadHumidity();
int16_t SHT21_ReadTemperature();

#endif
