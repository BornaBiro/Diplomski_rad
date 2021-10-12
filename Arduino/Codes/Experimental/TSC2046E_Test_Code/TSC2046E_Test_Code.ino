#include <SPI.h>

#define X_PLATE_RES   650.0
#define IRQ_PIN       19
#define SAMPLES       16

#define X_MIN 862
#define Y_MIN 3517
#define X_MAX 3398
#define Y_MAX 418

SPIClass *mySpi = new SPIClass(HSPI);
void setup() {
  pinMode(15, OUTPUT);
  pinMode(IRQ_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Code has started!");
  mySpi->begin(14, 12, 13, 15);
}

void loop() {
  int x, y;
  if (tsAvailable(&x, &y, NULL))
  {
    //x = map(x, X_MIN, X_MAX, 0, 800);
    //y = map(y, Y_MIN, Y_MAX, 0, 600);
    Serial.print(x, DEC);
    Serial.print(',');
    Serial.println(y, DEC);
    delay(50);
  }
}

uint16_t getX()
{
  digitalWrite(15, LOW);
  // 2MHz clock -> 2MHz/16 = 125kHz Sample Rate
  mySpi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  // 0b1, 101 - x measure, 0 - 12 bits,  0 - differential meas., 00 - low power between meas.
  mySpi->transfer(0b11010000);
  delayMicroseconds(50);
  int _x = ((mySpi->transfer(0x00) << 8 | mySpi->transfer(0x00)) >> 3);
  mySpi->endTransaction();
  digitalWrite(15, HIGH);
  return _x;
}

uint16_t getY()
{
  digitalWrite(15, LOW);
  // 2MHz clock -> 2MHz/16 = 125kHz Sample Rate
  mySpi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  // 0b1, 001 - y measure, 0 - 12 bits,  0 - differential meas., 00 - low power between meas.
  mySpi->transfer(0b10010000);
  delayMicroseconds(50);
  int _x = ((mySpi->transfer(0x00) << 8 | mySpi->transfer(0x00)) >> 3);
  mySpi->endTransaction();
  digitalWrite(15, HIGH);
  return _x;
}

uint16_t getZ1()
{
  digitalWrite(15, LOW);
  // 2MHz clock -> 2MHz/16 = 125kHz Sample Rate
  mySpi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  // 0b1, 011 - z1 measure, 0 - 12 bits,  0 - differential meas., 00 - low power between meas.
  mySpi->transfer(0b10110000);
  delayMicroseconds(50);
  int _x = ((mySpi->transfer(0x00) << 8 | mySpi->transfer(0x00)) >> 3);
  mySpi->endTransaction();
  digitalWrite(15, HIGH);
  return _x;
}

uint16_t getZ2()
{
  digitalWrite(15, LOW);
  // 2MHz clock -> 2MHz/16 = 125kHz Sample Rate
  mySpi->beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  // 0b1, 100 - z2 measure, 0 - 12 bits,  0 - differential meas., 00 - low power between meas.
  mySpi->transfer(0b11000000);
  delayMicroseconds(50);
  int _x = ((mySpi->transfer(0x00) << 8 | mySpi->transfer(0x00)) >> 3);
  mySpi->endTransaction();
  digitalWrite(15, HIGH);
  return _x;
}

uint16_t getP(uint16_t _x)
{
  uint32_t _z1 = 0;
  uint32_t _z2 = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    _z1 += getZ1();
    _z2 += getZ2();
  }
  _z1 /= SAMPLES;
  _z2 /= SAMPLES;
  delay(1);
  return (uint16_t)(X_PLATE_RES * (_x / 4096.0) * (((_z2 * 1.0) / _z1) - 1));
}

uint8_t tsAvailable(int *_x, int *_y, int *_p)
{
  if (digitalRead(IRQ_PIN)) return 0;
  delay(1);
  uint32_t _tempX = 0;
  uint32_t _tempY = 0;
  for (int i = 0; i < SAMPLES; i++)
  {
    _tempX += getX();
    _tempY += getY();
  }
  _tempX /= SAMPLES;
  _tempY /= SAMPLES;
  int _tempP = getP(_tempX);
  if (_tempP < 700)
  {
    *_x = (uint16_t)_tempX;
    *_y = (uint16_t)_tempY;
    if (_p != NULL) *_p = _tempP;
    return 1;
  }
  return 0;
}
