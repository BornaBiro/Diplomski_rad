#include <Wire.h>
#include "segments.h"
#define PCF85176_ADDR 0x38
static const uint16_t asciiToSeg[] = {SPACE, BLANK, QUMARK, BLANK, BLANK, PERC, BLANK, APH, OBRAC, CBRAC, BLANK, BLANK, BLANK, MINUS, BLANK, BLANK, NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,
                                      LETA, LETB, LETC, LETD, LETE, LETF, LETG, LETH,
                                     };

char strTmp[9];
uint8_t _tempSeg[8] = {0};
uint8_t _dots = 0;
int k = 0;
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  writeCommand(PCF85176_ADDR, B01001011);
  sprintf(strTmp, "1234");
  writeData(PCF85176_ADDR, strTmp);
  update(PCF85176_ADDR);
  delay(1000);
  setDot(PCF85176_ADDR, 7, 1);
  writeArrow(PCF85176_ADDR, 0, 1);
  update(PCF85176_ADDR);
}

void loop()
{

}

void writeArrow(uint8_t _addr, uint8_t _n, uint8_t _en)
{
  _n &= 7;
  if (_en)
  {
    _tempSeg[_n] |= SEGW;
  }
  else
  {
    _tempSeg[_n] &= ~(SEGW);
  }
}

void writeData(uint8_t _addr, char* s) {
  int n = strlen(s);
  for (int i = 0; i < n; i++)
  {
    _tempSeg[i] = asciiToSeg[s[i] - ' '];
  }

  //Wire.beginTransmission(_addr);
  //Wire.write(2 & (~B10000000));
  //Wire.write(0b11111111);
  //Wire.endTransmission();
}

void update(uint8_t _addr)
{
  uint8_t _dotMask = 0;

  //Write segments
  Wire.beginTransmission(_addr);
  Wire.write(0);
  for (int i = 0; i < 8; i++) {
    Wire.write(_tempSeg[i]);
  }
  Wire.endTransmission();

  //Now write dots
  for (int i = 0; i < 8; i++)
  {
    _dotMask = (_dots & (1 << i)) ? 0b00100000 : 0b00000000;
    Wire.beginTransmission(PCF85176_ADDR);
    Wire.write(2 + (3 * i));
    Wire.write(((_tempSeg[i] & 3) << 6) | _dotMask | (_tempSeg[i + 1] >> 3));
    Wire.endTransmission();
  }
}

//Very dirty and ultra slow way of setting dot on LCD
void setDot(uint8_t _addr, uint8_t _dot, uint8_t _en)
{
  _dot &= 7;
  if (_en)
  {
    _dots |= 1 << _dot;
  }
  else
  {
    _dots &= ~(1 << _dot);
  }
}

void writeCommand(uint8_t _addr, uint8_t _comm) {
  Wire.beginTransmission(_addr);
  Wire.write(_comm | B10000000);
  Wire.endTransmission();
}
