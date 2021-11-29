#include "Wire.h"
#define AS5600_ADDR 0x36
void setup() {
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Code has started");
}

void loop() {
  Serial.print("Angle: ");
  Serial.println(getAngle(), 1);
  delay(50);
}

void readRegs(uint8_t _addr, uint8_t _reg, uint8_t* _buf, uint8_t _n)
{
  Wire.beginTransmission(_addr);
  Wire.write(_reg);
  Wire.endTransmission(false);

  Wire.requestFrom(_addr, _n);
  for (int i = 0; i < _n; i++)
  {
    _buf[i] = Wire.read();
  }
}

uint16_t readReg16(uint8_t _addr, uint8_t _reg)
{
  uint8_t _temp[2];
  readRegs(_addr, _reg, _temp, 2);
  return ((_temp[0] << 8) | _temp[1]);
}

uint8_t readReg8(uint8_t _addr, uint8_t _reg)
{
  uint8_t _temp;
  readRegs(_addr, _reg, &_temp, 1);
  return _temp;
}

float getAngle()
{
  uint16_t angle;
  // Check if there if magnet detected at all
  if(!magnetDetected()) return -1;

  // If it is, read angle of magnet
  angle = readReg16(AS5600_ADDR, 0x0E);
  return ((angle / 4096.0) * 360);
}

uint8_t magnetDetected()
{
  uint8_t _temp = readReg8(AS5600_ADDR, 0x0b);
  return (_temp & 0b00100000) ? 1 : 0;
}
