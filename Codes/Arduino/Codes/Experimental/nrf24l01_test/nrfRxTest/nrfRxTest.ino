//RX RX RX RX RX RX RX RX RX
//RX RX RX RX RX RX RX RX RX
//RX RX RX RX RX RX RX RX RX

#include "Inkplate.h"
#include <SPI.h>
#include "RF24_Inkplate.h"

Inkplate display(INKPLATE_1BIT);
RF24_Inkplate radio(14, 15, 2000000);
byte addresses[][6] = {"1Node", "2Node"};
//SPIClass *mySpi = new SPIClass(HSPI);
void setup()
{
  SPIClass *mySpi = display.getSPI();
  Serial.begin(115200);
  Serial.println("Code has started");
  display.begin();
  mySpi->begin(14, 12, 13, 15);
  if(!radio.begin(mySpi, &display))
  {
    Serial.println("Radio init ERROR!");
    while(1);
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openReadingPipe(0, 0x65646F4E31);
  radio.startListening();
  Serial.println("Radio Init OK");
}

void loop()
{
  while (radio.available())
  {
    char temp[32];
    radio.read(temp, 32);
    Serial.print("RX: ");
    Serial.println(temp);
  }
}
