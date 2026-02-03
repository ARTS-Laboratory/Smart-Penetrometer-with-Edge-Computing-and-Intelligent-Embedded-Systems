// SimpleRx - the receiver

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "printf.h"

#define CE_PIN 6
#define CSN_PIN 7

const byte thisAddress[6][6] = { "1node", "1node", "3node","4node", "5node", "6node" };

RF24 radio(CE_PIN, CSN_PIN);
int Reset = 4;
int rec[1] = { 2 };
int i = 0;
float dataReceived;  // this must match dataToSend in the TX
bool newData = false;
float combData[7];

//===========

void setup() {

  Serial.begin(115200);
  digitalWrite(Reset, HIGH);
  delay(200); 
  pinMode(Reset, OUTPUT);
  // Serial.println("Node |      Time    | aTemp | gTemp |  Hum  |  Press  |  Cond  |");
  radio.begin();
  if (!radio.isChipConnected()) {
    Serial.println("radio hardware is not responding!!");
    while (1) {}
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(90);
  radio.setAutoAck(true);  
  radio.enableAckPayload();  
  radio.enableDynamicPayloads();
  radio.openReadingPipe(0, thisAddress[0]);
  radio.openReadingPipe(1, thisAddress[1]);
  radio.openReadingPipe(2, thisAddress[2]); 
  radio.openReadingPipe(3, thisAddress[3]);
  radio.openReadingPipe(4, thisAddress[4]);
  radio.openReadingPipe(5, thisAddress[5]);    
  radio.startListening();
  radio.setRetries(15, 15);  
  // printf_begin(); 
  // radio.printPrettyDetails();  
}

//=============

void loop() {
  getData();
  showData();
} 

//==============

void getData() {
  if (radio.available()) {
    radio.writeAckPayload(1, rec, sizeof(rec)); 
    radio.read(&dataReceived, sizeof(dataReceived));
    rec[0] += 2;   
    newData = true;
  }
}

void showData() {
  if (newData == true) {
    combData[i] = dataReceived;
    newData = false;
    i += 1;
    if (i == 7) {
      if (combData[0] != 1.0 and combData[0] != 2.0 and combData[0] != 3.0 and combData[0] != 4.0 and combData[0] != 5.0) {
        resetReceiver();
      }
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

void resetReceiver() {
  Serial.println("Reset");
  delay(100);
  digitalWrite(Reset, LOW);
}