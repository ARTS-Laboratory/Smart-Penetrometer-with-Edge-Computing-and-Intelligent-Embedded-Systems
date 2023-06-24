#include <SPI.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN

const byte address[6] = "ABCD";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setChannel(0);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }
}
