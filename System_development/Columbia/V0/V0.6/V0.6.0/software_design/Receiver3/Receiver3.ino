// SimpleRx - the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 6
#define CSN_PIN 7

const byte thisAddress[][6] = { "node01", "node09", "node03", "node04", "node05", "node07" };

RF24 radio(CE_PIN, CSN_PIN);

int i = 0;
float dataReceived;  // this must match dataToSend in the TX
bool newData = false;
float combData[7];
//===========

void setup() {

  Serial.begin(115200);
  delay(100);
  // Serial.println("Node |      Time    | aTemp | gTemp |  Hum  |  Press  |  Cond  |");
  radio.begin();
  if (!radio.isChipConnected()) {
    Serial.println("radio hardware is not responding!!");
    while (1) {}
  }
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(0, thisAddress[0]);
  radio.openReadingPipe(1, thisAddress[1]);
  radio.openReadingPipe(2, thisAddress[2]); 
  radio.openReadingPipe(3, thisAddress[3]);
  radio.openReadingPipe(4, thisAddress[4]);
  radio.openReadingPipe(5, thisAddress[5]);    
  radio.startListening();
}

//=============

void loop() {
  getData();
  showData();
} 

//==============

void getData() {
  if (radio.available()) {
    radio.read(&dataReceived, sizeof(dataReceived));
    newData = true;
  }
}

void showData() {
  if (newData == true) {
    combData[i] = dataReceived;
    // Serial.print("Data received ");
    // Serial.println(dataReceived, 9);
    newData = false;
    i += 1;
    if (i == 7) {
      Serial.print("  ");      
      Serial.print(int(combData[0]));
      Serial.print(",  ");
      Serial.print(combData[1],11);
      Serial.print(",  ");
      Serial.print(combData[2]);
      Serial.print(",  ");
      Serial.print(combData[3]);
      Serial.print(",  ");
      Serial.print(combData[4]);
      Serial.print(",  ");
      Serial.print(combData[5]);
      Serial.print(",   ");
      Serial.println(combData[6]);
      i = 0;
    }
  }
}