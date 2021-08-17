#ifndef __GLASSLCD_H__
#define __GLASSLCD_H__

#include <Wire.h>
#include "stm32_def.h"
#include "segments.h"

#define PCF85176_ADDR       0x38
#define PCF85176_DISP_ON    0b00001000
#define PCF85176_DISP_OFF   0b00000000
#define PCF85176_1_3_BIAS   0b00000000
#define PCF85176_1_2_BIAS   0b00000100
#define PCF85176_STATIC     0b00000001
#define PCF85176_1_2_MPX    0b00000010
#define PCF85176_1_3_MPX    0b00000011
#define PCF85176_1_4_MPX    0b00000000

// 1:3 LCD Multiplex, 1/3 bias, LCD Enabled
#define LCD_CONFIG    0b01000000 | PCF85176_DISP_ON | PCF85176_1_3_BIAS | PCF85176_1_3_MPX

class Lcd
{
  public:
  Lcd();
  void begin(TwoWire *_w);
  void writeData(char* s);
  void clear();
  void lcdState(bool _state);
  void setDot(uint8_t _n, bool _dot);

  private:
  TwoWire *_wire = NULL;
  void writeCommand(uint8_t _comm);
};

#endif
