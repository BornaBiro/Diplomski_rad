//RX RX RX RX RX RX RX RX RX
//RX RX RX RX RX RX RX RX RX
//RX RX RX RX RX RX RX RX RX

#include "Inkplate.h"
#include <SPI.h>
#include "RF24_Inkplate.h"

Inkplate display(INKPLATE_1BIT);
RF24_Inkplate radio(14, 15, 2000000);
byte addresses[][6] = {"1Node", "2Node"};
uint64_t addr[2] = {0x65646F4E31, 0x65646F4E32};
void setup()
{
  SPIClass *mySpi = display.getSPI();
  Serial.begin(115200);
  Serial.println("Code has started");
  display.begin();
  mySpi->begin(14, 12, 13, 15);
  if (!radio.begin(mySpi, &display))
  {
    Serial.println("Radio init ERROR!");
    while (1);
  }
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setChannel(0);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX, 1);
  radio.openWritingPipe(addr[1]);
  radio.openReadingPipe(1, addr[0]);
  radio.startListening();
  Serial.println("Radio Init OK");
}

char myAck[7] = {"ACK-OK"};

void loop()
{
  if (radio.available())
  {
    char temp[32];
    while (radio.available())
    {
      radio.read(temp, 32);
      radio.writeAckPayload(1, myAck, sizeof(myAck));
    }
    Serial.print("RX: ");
    Serial.println(temp);
  }
}
