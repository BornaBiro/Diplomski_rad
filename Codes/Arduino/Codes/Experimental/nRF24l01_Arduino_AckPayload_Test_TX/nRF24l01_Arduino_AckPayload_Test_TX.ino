// TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX
// TX TX TX TX TX TX TX TX TX TX

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
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.stopListening();
}

char myAck[7];
int i = 0;
void loop() {
  int s = radio.write(&i, sizeof(i));
  Serial.print("TX: ");
  Serial.print(i, DEC);
  Serial.print(" Sent?");
  Serial.println(s?"OK":"Fail");
  if (radio.isAckPayloadAvailable())
  {
    while (radio.available())
    {
      radio.read(myAck, sizeof(myAck));
      Serial.print("Payload ACK:");
      Serial.println(myAck);
    }
  }
  i++;
  delay(250);
}
