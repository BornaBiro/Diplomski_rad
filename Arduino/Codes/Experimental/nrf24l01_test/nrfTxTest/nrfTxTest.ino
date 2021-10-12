//TX TX TX TX TX TX TX TX TX
//TX TX TX TX TX TX TX TX TX
//TX TX TX TX TX TX TX TX TX

#include "Inkplate.h"
#include <SPI.h>
#include "RF24.h"

Inkplate display(INKPLATE_1BIT);
RF24 radio(14, 15, 2000000);
byte addresses[][6] = {"1Node", "2Node"};

long i = 0;

SPIClass *mySpi = new SPIClass(HSPI);
void setup()
{
  //SPIClass mySpi = display.getSPI();
  Serial.begin(115200);
  Serial.println("Code has started");
  mySpi->begin(14, 12, 13, 15);
  display.begin();
  if (!radio.begin(mySpi, &display))
  {
    Serial.println("Radio init ERROR!");
    while (1);
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(0x65646F4E31);
  radio.stopListening();
  Serial.println("Radio Init OK");
  for (int i = 0; i < 6; i++)
  {
    Serial.print(addresses[0][i], HEX);
  }
  Serial.println();
}

void loop()
{
  radio.write(&i, sizeof(i));
  i++;
  Serial.print("TX: ");
  Serial.println(i, DEC);
  delay(250);
}
