#include "Wire.h"

#define ADDR_BU21029 0x40

void setup() {
  pinMode(19, INPUT_PULLUP);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Code has started");

  // Do a software reset of the touch IC
  writeReg8(ADDR_BU21029, 0b00000011, 0);
  // Wait a little bit
  delay(10);
  // Set a interval for auto mode and ADC sample time
  writeReg8(ADDR_BU21029, 0b00010000, 0b11110011);
  // Disable I2C clock stretching, 12bit resolution, dual touch off
  writeReg8(ADDR_BU21029, 0b01011000, 0b00100010);
  // Do not use calibration result
  writeReg8(ADDR_BU21029, 0b00000000, 0b00000000);
  // Set a higher voltage for ADC and panel (2V)
  writeReg8(ADDR_BU21029, 0b01100000, 0b01110111);
  // Do a autoscan
  writeRegOnly(ADDR_BU21029, 0b10000000);
  delay(10);
}

void loop() {
  if (!digitalRead(19))
  {
    uint8_t myData[8];
    readRegs(ADDR_BU21029, 0b10000100, myData, 8);
    int x = (myData[0] << 8 | myData[1]) >> 4;
    int y = (myData[2] << 8 | myData[3]) >> 4;
    int z1 = (myData[4] << 8 | myData[5]) >> 4;
    int z2 = (myData[6] << 8 | myData[7]) >> 4;
    Serial.print(x, DEC);
    Serial.print(',');
    Serial.print(y, DEC);
    Serial.print(',');
    Serial.print(z1, DEC);
    Serial.print(',');
    Serial.print(z2, DEC);
    Serial.print(',');
    printBIN(readReg8(ADDR_BU21029, 0b01101000));
    //Serial.println();
    delay(100);
  }
}

void readRegs(uint8_t _addr, uint8_t _reg, uint8_t *_buf, uint8_t _n)
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

uint8_t readReg8(uint8_t _addr, uint8_t _reg)
{
  uint8_t _temp;
  readRegs(_addr, _reg, &_temp, 1);
  return _temp;
}

void writeRegs(uint8_t _addr, uint8_t _reg, uint8_t *_buf, uint8_t _n)
{
  Wire.beginTransmission(_addr);
  Wire.write(_reg);
  Wire.write(_buf, _n);
  Wire.endTransmission();
}

void writeReg8(uint8_t _addr, uint8_t _reg, uint8_t _data)
{
  writeRegs(_addr, _reg, &_data, 1);
}

void writeRegOnly(uint8_t _addr, uint8_t _reg)
{
  Wire.beginTransmission(_addr);
  Wire.write(_reg);
  Wire.endTransmission();
}

void printBIN(uint16_t _b)
{
  for (int i = 15; i >= 0; i--)
  {
    Serial.print(_b & (1UL << i) ? '1' : '0');
  }
  Serial.println();
}
