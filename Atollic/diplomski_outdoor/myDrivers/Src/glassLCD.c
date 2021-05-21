#include "glassLCD.h"
#include "string.h"

extern I2C_HandleTypeDef hi2c1;

void glassLCD_Begin()
{
	glassLCD_WriteCmd(LCD_CONFIG);
}

void glassLCD_WriteData(char* s)
{
	uint8_t _segments[9] = {0};
	uint8_t _n = strlen(s);

	//Convert ASCII to segment data
	for (int i = 0; i < _n; i++)
	{
	  _segments[i + 1] = asciiToSeg[s[i] - ' '] >> 1;
	}

	// Add command for RAM addresing
	_segments[0] = 0;

	// Push all data to I2C!
	HAL_I2C_Master_Transmit(&hi2c1, PCF85176_ADDR, _segments, 9, 1000);
}

void glassLCD_Clear()
{
	uint8_t _segments[9] = {0};
	_segments[0] = 0;
	HAL_I2C_Master_Transmit(&hi2c1, PCF85176_ADDR, _segments, 9, 1000);
}

void glassLCD_State(uint8_t _state)
{
	glassLCD_WriteCmd((LCD_CONFIG) & ((_state & 1) << 3));
}

void glassLCD_SetDot(uint8_t _n, uint8_t _dot)
{
	uint8_t _data[2];

	_data[0] = 2 + (3 * _n);
	_data[1] = _dot ? (0b00100000) : (0b00000000);
	HAL_I2C_Master_Transmit(&hi2c1, PCF85176_ADDR, _data, 2, 1000);
}

void glassLCD_WriteCmd(uint8_t _comm)
{
	_comm = _comm | 0b10000000;
	HAL_I2C_Master_Transmit(&hi2c1, PCF85176_ADDR, &_comm, 1, 1000);
}
