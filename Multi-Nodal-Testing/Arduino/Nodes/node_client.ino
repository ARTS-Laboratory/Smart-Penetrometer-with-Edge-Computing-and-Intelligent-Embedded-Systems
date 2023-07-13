#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);  // CE, CSN pins
const uint64_t w_pipe = 0xB3B4B5B6F1;
const uint64_t r_pipe = 0xF0F0F0F0D2;

char recv_data[32];

struct SensorData {
  unsigned long time;
  float temperature;
  float humidity;
  float conductivity;
  int packetNumber;
};

void setup() {
  radio.begin();
  radio.openWritingPipe(w_pipe);
  radio.openReadingPipe(1, r_pipe);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.setChannel(0x4C);
  radio.printDetails();
  radio.startListening();
  Serial.begin(9600);
}

int i = 0;
void loop() {
  send_num(i);
  Serial.println(i);
  delay(1);  // Delay between transmissions
  i++;
}
const uint8_t payloadSize = 128;


//void loop() {
////  if (radio.available()) {
////  char text[payloadSize] = "";
////  radio.read(&text, sizeof(text));
////  Serial.println(text);
//
//  send_num(i);
//  Serial.println(i);
//  i++;
////  }
//}




void send_num(int num) {
  radio.stopListening();
  radio.write(&num, sizeof(num));
  radio.startListening();
}

char*  get_data() {
  char text[payloadSize] = "";
  radio.read(&text, sizeof(text));
  //  Serial.println(text);
  return text;
}