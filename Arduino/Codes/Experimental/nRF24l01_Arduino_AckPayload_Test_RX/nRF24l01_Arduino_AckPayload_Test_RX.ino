// RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX
// RX RX RX RX RX RX RX RX RX RX

#include <SPI.h>
#include "RF24.h"

RF24 radio(9, 10);

byte addresses[][6] = {"1Node", "2Node"};

void setup() {
  Serial.begin(115200);
  Serial.println("Code has started");
  radio.begin();
  radio.setAutoAck(1);
  radio.enableAckPayload();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
}

char myAck[7] = {"ACK-OK"};
int i = 0;
void loop() {
  if (radio.available())
  {
    while (radio.available())
    {
      radio.read(&i, sizeof(i));
      radio.writeAckPayload(1, myAck, sizeof(myAck));
    }
    Serial.print("RX: ");
    Serial.println(i, DEC);
  }
}
