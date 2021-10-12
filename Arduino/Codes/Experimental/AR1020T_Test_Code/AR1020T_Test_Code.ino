#include <Wire.h>

#define ADDR_AR1020             0x4D

#define AR1020_TOUCHTRESH       0x02
#define AR1020_AFILTERFAST      0x06
#define AR1020_AFILTERSLOW      0x07
#define AR1020_TOUCHMODE        0x0C
#define AR1020_TOUCHOPT         0x0D

#define AR1020_EN_TOUCH_CMD     0x12
#define AR1020_DIS_TOUCH_CMD    0x13
#define AR1020_CAL_MODE_CMD     0x14
#define AR1020_REG_READ_CMD     0x20
#define AR1020_REG_WRITE_CMD    0x21
#define AR1020_REG_ADDR_CMD     0x22
#define AR1020_REG_EEPROM_CMD   0x23
#define AR1020_EEPROM_READ_CMD  0x28
#define AR1020_EEPROM_WRITE_CMD 0x29
#define AR1020_EEPROM_REGS_CMD  0x2B

void setup()
{
  pinMode(19, INPUT_PULLUP);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Code has started!");
  writeReg(ADDR_AR1020, AR1020_TOUCHOPT, 0);
  writeReg(ADDR_AR1020, AR1020_TOUCHTRESH, 224);
  writeReg(ADDR_AR1020, AR1020_AFILTERFAST, 8);
  writeReg(ADDR_AR1020, AR1020_AFILTERSLOW, 8);
  Serial.println(writeCmd(ADDR_AR1020, AR1020_EN_TOUCH_CMD), DEC);
}


void loop()
{
  if (digitalRead(19))
  {
    int x, y;
    getXY(ADDR_AR1020, &x, &y, NULL);
    Serial.print(x, DEC);
    Serial.print(',');
    Serial.println(y, DEC);
    delay(50);
  }
}

uint8_t writeReg(uint8_t _addr, uint8_t _reg, uint8_t _data)
{
  uint8_t _res;
  _res = sendData(_addr, AR1020_REG_ADDR_CMD, &_reg, 1);
  if (_res) return 0;

  _res = sendData(_addr, AR1020_REG_WRITE_CMD, &_data, 1);
  if (_res) return 0;
}

uint8_t sendData(uint8_t _addr, uint8_t _cmd, uint8_t *_data, uint8_t _n)
{
  uint8_t _buf[16];
  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  Wire.write(0x55);
  Wire.write(_n + 1);
  Wire.write(AR1020_REG_ADDR_CMD);
  Wire.write(_data, _n);
  Wire.endTransmission(false);

  Wire.requestFrom(_addr, 4 + _n);
  for (int i = 0; i < 4 + _n; i++)
  {
    _buf[i] = Wire.read();
  }
  return _buf[2];
}

uint8_t writeCmd(uint8_t _addr, uint8_t _cmd)
{
  uint8_t _buf[5];
  Wire.beginTransmission(_addr);
  Wire.write(0x00);
  Wire.write(0x55);
  Wire.write(1);
  Wire.write(_cmd);
  Wire.endTransmission(false);

  Wire.requestFrom(_addr, 5);
  for (int i = 0; i < 5; i++)
  {
    _buf[i] = Wire.read();
  }
  return _buf[2];
}

void getXY(uint8_t _addr, int *_x, int *_y, int *_p)
{
  Wire.requestFrom(_addr, 5);
  int _pen = Wire.read() & 1;
  if (_p != NULL) *_p = _pen;
  *_x = (Wire.read() | (Wire.read() << 7));
  *_y = (Wire.read() | (Wire.read() << 7));
}
