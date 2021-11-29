#include <Wire.h>
#include "segments1.h"

#define PCF85176_ADDR 0x38
static const uint16_t asciiToSeg[] = {SPACE, BLANK, QUMARK, BLANK, BLANK, PERC, BLANK, APH, OBRAC, CBRAC, STAR, BLANK, BLANK, MINUS, BLANK, BLANK, NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, BLANK, BLANK, BLANK, BLANK, BLANK, QUEST, BLANK,
                                      LETA, LETB, LETC, LETD, LETE, LETF, LETG, LETH, LETI, LETJ, LETK, LETL, LETM, LETN, LETO, LETP, LETQ, LETR, LETS, LETT, LETU, LETV, LETW, LETX, LETY, LETZ
                                     };

int pos = 0;
void setup() {
  Serial.begin(115200);
  Wire.begin();
  writeCommand(PCF85176_ADDR, B01001000);
  //writeData(PCF85176_ADDR, "*BORNA?");
}

void loop() {
  // put your main code here, to run repeatedly:
  scrollText("14-SEGMENTNI BARE GLASS LCD JE COOL JEDNOSTAVAN ISPIS TEXTA TE MALA POTROSNJA STRUJE SVEGA 25UA", &pos);
  delay(330);
}


void scrollText(char* c, int *n)
{
  char _cInt[120];
  char out[9];
  int len = strlen(c);
  if (len > 100) len = 100;

  memset(out, ' ', 8);
  memset(_cInt, ' ', 118);
  memcpy(_cInt + 8, c, len);
  memcpy(out, _cInt + (*n), 8);

  (*n)++;
  if ((*n) > len + 8) *n = 0;
  writeData(PCF85176_ADDR, out);
}

void writeCommand(uint8_t _addr, uint8_t _comm) {
  Wire.beginTransmission(_addr);
  Wire.write(_comm | B10000000);
  Wire.endTransmission();
}

void writeData(uint8_t _addr, char* s) {
  uint16_t segments[8];
  for (int i = 0; i < 8; i++)
  {
    segments[i] = asciiToSeg[s[i] - ' '];
  }

  Wire.beginTransmission(_addr);
  Wire.write(0);
  for (int i = 0; i < 8; i++) {
    Wire.write(segments[i] >> 8);
    Wire.write(segments[i] & 0xff);
  }
  Wire.endTransmission();
}

void drawLetter(uint8_t _addr, uint16_t _l)
{
  Wire.beginTransmission(_addr);
  Wire.write(0);
  Wire.write(_l >> 8);
  Wire.write(_l & 0xff);
  Wire.endTransmission();
}
