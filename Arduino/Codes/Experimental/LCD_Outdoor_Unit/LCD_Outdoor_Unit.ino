#include <Wire.h>
#include "segments.h"

//For temp read
#define VCC 4.8    //Supply voltage
#define R 10000  //R=10KΩ
#define RT0 10000   // Ω
#define B 3977      // K

#define PCF85176_ADDR 0x38
static const uint16_t asciiToSeg[] = {SPACE, BLANK, QUMARK, BLANK, BLANK, PERC, BLANK, APH, OBRAC, CBRAC, BLANK, BLANK, BLANK, MINUS, BLANK, BLANK, NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK, BLANK,
                                      LETA, LETB, LETC, LETD, LETE, LETF, LETG, LETH,
                                     };
void setup()
{
  Wire.begin();
  writeCommand(PCF85176_ADDR, B01001011);
  setDot(PCF85176_ADDR, 4, true);
}

void loop()
{
  char strTmp[9];
  float t = readT(A0);
  sprintf(strTmp, "%5d%d C", int(t), int(abs(t*10))%10);
  writeData(PCF85176_ADDR, strTmp);
  delay(1000);
}

void writeData(uint8_t _addr, char* s) {
  uint16_t segments[8];
  for (int i = 0; i < 8; i++)
  {
    segments[i] = asciiToSeg[s[i] - ' '];
  }

  Wire.beginTransmission(_addr);
  Wire.write(0 & (~B10000000));
  for (int i = 0; i < 8; i++) {
    Wire.write(segments[i] >> 1);
  }
  Wire.endTransmission();

  //Wire.beginTransmission(_addr);
  //Wire.write(2 & (~B10000000));
  //Wire.write(0b11111111);
  //Wire.endTransmission();
}

//Very dirty and ultra slow way of setting dot on LCD
void setDot(uint8_t _addr, uint8_t _n, bool _dot)
{
  Wire.beginTransmission(_addr);
  Wire.write(2 + (2 * _n));
  Wire.endTransmission();

  Wire.requestFrom(_addr, 1);
  char c = Wire.read();

  Wire.beginTransmission(_addr);
  Wire.write(2 + (3 * _n));
  Wire.write(_dot? (c & 0b00100000):(c & ~(0b00100000)));
  Wire.endTransmission();
}

void writeCommand(uint8_t _addr, uint8_t _comm) {
  Wire.beginTransmission(_addr);
  Wire.write(_comm | B10000000);
  Wire.endTransmission();
}

double readT(int _ch)
{
  double RT, VR, ln, TX, T0, VRT;
  T0 = 25 + 273.15;
  VRT = analogRead(_ch);              //Acquisition analog value of VRT
  VRT = (4.8 / 1023.00) * VRT;      //Conversion to voltage
  VR = VCC - VRT;
  RT = VRT / (VR / R);               //Resistance of RT

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temperature from thermistor
  TX = TX - 273.15;                 //Conversion to Celsius
  return TX;
}
