#include "glassLCD.h"

static const uint16_t asciiToSeg[] = {SPACE, BLANK, QUMARK, BLANK, BLANK, PERC, BLANK, APH, OBRAC, CBRAC, BLANK, BLANK, BLANK, MINUS, BLANK, BLANK, NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,
                                      LETA, LETB, LETC, LETD, LETE, LETF, LETG, LETH,
                                     };
Lcd::Lcd()
{

}

void Lcd::begin(TwoWire *_w)
{
  _wire = _w;
  writeCommand(LCD_CONFIG);
  clear();
}

void Lcd::writeData(char* s)
{
  uint8_t segments[8];
  uint8_t _n = strlen(s);
  for (int i = 0; i < _n; i++)
  {
    segments[i] = asciiToSeg[s[i] - ' '] >> 1;
  }

  _wire->beginTransmission(PCF85176_ADDR);
  _wire->write(0 & (~B10000000));
  _wire->write(segments, _n);
  _wire->endTransmission();
}

void Lcd::clear()
{
  _wire->beginTransmission(PCF85176_ADDR);
  _wire->write(0 & (~B10000000));
  for (int i = 0; i < 8; i++)
  {
    _wire->write(0);
  }
  _wire->endTransmission();
}

void Lcd::lcdState(bool _state)
{
  writeCommand(LCD_CONFIG & (_state << 3));
}

void Lcd::setDot(uint8_t _n, bool _dot)
{
  _wire->beginTransmission(PCF85176_ADDR);
  _wire->write(2 + (2 * _n));
  _wire->endTransmission();

  _wire->requestFrom(PCF85176_ADDR, 1);
  char c = _wire->read();

  _wire->beginTransmission(PCF85176_ADDR);
  _wire->write(2 + (3 * _n));
  _wire->write(_dot ? (c & 0b00100000) : (c & ~(0b00100000)));
  _wire->endTransmission();
}

void Lcd::writeCommand(uint8_t _comm) {
  _wire->beginTransmission(PCF85176_ADDR);
  _wire->write(_comm | B10000000);
  _wire->endTransmission();
}
