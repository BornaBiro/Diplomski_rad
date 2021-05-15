#ifndef __LCD_H__
#define __LCD_H__

#include <Wire.h>

class Lcd
{
  public:
  Lcd();
  void begin(TwoWire *_w);

  private:
  TwoWire *_wire = NULL;
  
};

#endif
