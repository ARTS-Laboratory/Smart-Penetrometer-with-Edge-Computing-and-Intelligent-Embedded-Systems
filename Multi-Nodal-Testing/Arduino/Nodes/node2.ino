#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN pins
const uint64_t pipe = 0xB3B4B5B6F1;  // Receiver pipe address

void setup() {
  radio.begin();
  radio.openWritingPipe(pipe);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(0x4C);
  radio.printDetails();
  Serial.begin(9600);
}

int i = 0;
void loop() {
  if(i%100 == 0){
    i++;
  }
  radio.write(&i, sizeof(i));
  Serial.println(i);
  delay(1);  // Delay between transmissions
  i++;
}