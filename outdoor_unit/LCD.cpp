#include "LCD.h"

Lcd::Lcd()
{
  
}


void Lcd::begin(TwoWire *_w)
{
  _wire = _w;
}
