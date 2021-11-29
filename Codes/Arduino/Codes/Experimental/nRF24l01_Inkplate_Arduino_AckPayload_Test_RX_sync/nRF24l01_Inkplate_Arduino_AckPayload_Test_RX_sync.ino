//RX RX RX RX RX RX RX RX RX
//RX RX RX RX RX RX RX RX RX
//RX RX RX RX RX RX RX RX RX

#include "Inkplate.h"
#include <SPI.h>
#include "RF24_Inkplate.h"

#define SYNC_HEADER     0b00110101

Inkplate display(INKPLATE_1BIT);
RF24_Inkplate radio(14, 15, 2000000);
byte addresses[][6] = {"1Node", "2Node"};
uint64_t addr[2] = {0x65646F4E31, 0x65646F4E32};

struct syncStructHandle{
  uint8_t header;
  uint32_t myEpoch;
  uint32_t readInterval;
  uint32_t sendInterval;
}syncStruct = {SYNC_HEADER};

void setup()
{
  SPIClass *mySpi = display.getSPIptr();
  Serial.begin(115200);
  Serial.println("Code has started");
  display.begin();
  display.display();
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



void loop()
{
  if (radio.available())
  {
    char temp[32];
    while (radio.available())
    {
      radio.read(temp, 32);
    }
    if (temp[0] == SYNC_HEADER)
    {
      memcpy(&syncStruct, temp, sizeof(syncStruct));
      Serial.println(syncStruct.myEpoch, DEC);
      Serial.println(syncStruct.readInterval, DEC);
      Serial.println(syncStruct.sendInterval, DEC);
      syncStruct.myEpoch = 1630788084;
      radio.writeAckPayload(1, &syncStruct, sizeof(syncStruct));
    }
    Serial.print("RX: ");
    Serial.println(temp);
  }
}
