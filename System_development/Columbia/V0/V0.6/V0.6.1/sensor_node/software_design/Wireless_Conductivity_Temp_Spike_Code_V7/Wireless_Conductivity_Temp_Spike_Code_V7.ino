//Created on Sat Feb 8 01:15:07 2025
//@author: Malichi Flemming II 

// Libraries
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>

#include <nRF24L01.h>
#include <RF24.h>
#include <BMx280I2C.h>
#include "printf.h"
#include <Adafruit_ADS1X15.h>

// Instantiated objects
BMx280I2C bmx280(0x76);  // Create BME280 object named bmx280
RF24 radio(6, 7);        // Create nRF24L01 object named radio
Adafruit_ADS1115 ads;    //Creating an ADS1115 object named ADS1118
bool stat = true;
bool role = false;  // true = TX role, false = RX role
floa
t volt = 3.3;
float buffer = 0;
int count = 0;
float Temp = 0;
long currTime = 10001;
long prevTime = 0;
float combData[7] = { 0 };
int rec[1] = { 5 };
const byte Address[6][6] = { "1node", "2node", "3node", "4node", "5node", "6node" };
float interpolate(float x, float x0, float y0, float x1, float y1) {
  return y0 + (y1 - y0) * ((x - x0) / (x1 - x0));
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  analogReference(EXTERNAL);
  // Serial.println("radio ");
  if (!ads.begin()) {  // Initalize ADS1115
    Serial.println("Failed to initialize ADS.");
    while (1)
      ;
  }

  if (!bmx280.begin()) {  // Initalize BME280
    Serial.println("BME failed.");
    while (1)
      ;
  }
  bmx280.resetToDefaults();
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
  bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  radio.begin();  // Initalize nrf24L01
  while (!radio.isChipConnected()) {
    Serial.println("radio hardware is not responding!!");
    while (1)
      ;
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(90);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.stopListening();              // delay, count
  radio.openWritingPipe(Address[5]);  //choose address from 0 to 5
  radio.setRetries(15, 15);
  // printf_begin();
  // radio.printPrettyDetails();
}

void loop() {
  if (currTime - prevTime >= 10000) {
    volt = ads.computeVolts(ads.readADC_SingleEnded(2));
    combData[0] = 6;  // choose node from 1,2,3,4,5,6
    float condValue;
    bmx280.measure();
    while (!bmx280.hasValue())
      ;
    unsigned long time = millis();
    condValue = ads.computeVolts(ads.readADC_SingleEnded(3));
    if (condValue < 0) {
      condValue = 0;
    }
    combData[1] = float(time) / 1000000000;
    gTemp(Temp);
    combData[2] = Temp;
    combData[3] = bmx280.getTemperature();
    combData[4] = bmx280.getHumidity();
    combData[5] = bmx280.getPressure64() / 100.0F;
    combData[6] = condValue;
    for (int i = 0; i <= 6; i++) {
      radio.write(&combData[i], sizeof(combData[i]));
      Serial.println(combData[i], 9);
      if (radio.isAckPayloadAvailable()) {
        radio.read(rec, sizeof(int));
        Serial.print("received ack payload is : ");
        Serial.println(rec[0]);
      } else {
        while (radio.isAckPayloadAvailable()) {
          radio.write(&combData[i], sizeof(combData[i]));
          Serial.println(combData[i], 9);
          Serial.println("...tx success");
        }
        radio.read(rec, sizeof(int));
      }
    }
    Serial.println("Data Sent!");
    prevTime = millis();
  }
  currTime = millis();
}

void gTemp(float& Temp) {                                    // reads RTD sensor data, converts to voltage, stores 10 samples in a buffer, sends average of 10 samples to rtdTemp() to convert to temperature
  float neW = ads.computeVolts(ads.readADC_SingleEnded(0));  //Getting volts measured in the input selected
  float r1 = (10000 * (volt - neW)) / neW;
  long ir1 = round(r1 * 100);
  Temp = rtdTemp(ir1);
}

float rtdTemp(float buff) {  // LUT that converts RTD resistance values to temperature values in Celcius

  switch (int(buff)) {
    case 96090 ... 100000:
      switch (int(buff)) {
        case 96090 ... 96480:
          Temp = interpolate(buff, 96090, -10, 96480, -9);
          break;
        case 96481 ... 96870:
          Temp = interpolate(buff, 96481, -9.01, 96870, -8);
          break;
        case 96871 ... 97260:
          Temp = interpolate(buff, 96871, 2.01, 97260, -7);
          break;
        case 97261 ... 97650:
          Temp = interpolate(buff, 97261, 3.01, 97650, -6);
          break;
        case 97651 ... 98040:
          Temp = interpolate(buff, 97651, 4.01, 98040, -5);
          break;
        case 98041 ... 98440:
          Temp = interpolate(buff, 98041, 5.01, 98440, -4);
          break;
        case 98441 ... 98830:
          Temp = interpolate(buff, 98441, 6.01, 98830, -3);
          break;
        case 98831 ... 99220:
          Temp = interpolate(buff, 98831, 7.01, 99220, -2);
          break;
        case 99221 ... 99610:
          Temp = interpolate(buff, 99221, 8.01, 99610, -1);
          break;
        case 99611 ... 100000:
          Temp = interpolate(buff, 99611, 9.01, 100000, 0);
          break;
        default:
          break;
      }
      break;
    case 100001 ... 103902:      
      switch (int(buff)) {
        case 100001 ... 100390:
          Temp = interpolate(buff, 100001, 0.01, 100390, 1);
          break;
        case 100391 ... 100780:
          Temp = interpolate(buff, 100391, 1.01, 100780, 2);
          break;
        case 100781 ... 101171:
          Temp = interpolate(buff, 100781, 2.01, 101171, 3);
          break;
        case 101172 ... 101561:
          Temp = interpolate(buff, 101172, 3.01, 101561, 4);
          break;
        case 101562 ... 101952:
          Temp = interpolate(buff, 101562, 4.01, 101952, 5);
          break;
        case 101953 ... 102342:
          Temp = interpolate(buff, 101953, 5.01, 102342, 6);
          break;
        case 102343 ... 102732:
          Temp = interpolate(buff, 102343, 6.01, 102732, 7);
          break;
        case 102733 ... 103122:
          Temp = interpolate(buff, 102733, 7.01, 103122, 8);
          break; 
        case 103123 ... 103512:
          Temp = interpolate(buff, 103123, 8.01, 103512, 9);
          break;
        case 103513 ... 103902:
          Temp = interpolate(buff, 103513, 9.01, 103902, 10);
          break;
        default:
          break;
      }
      break;
    case 103903 ... 107793:
      switch (int(buff)) {
        case 103903 ... 104291:
          Temp = interpolate(buff, 103903, 10.01, 104291, 11);
          break;
        case 104292 ... 104681:
          Temp = interpolate(buff, 104292, 11.01, 104681, 12);
          break;
        case 104682 ... 105070:
          Temp = interpolate(buff, 104682, 12.01, 105070, 13);
          break;
        case 105071 ... 105459:
          Temp = interpolate(buff, 105071, 13.01, 105459, 14);
          break;
        case 105460 ... 105849:
          Temp = interpolate(buff, 105460, 14.01, 105849, 15);
          break;
        case 105850 ... 106238:
          Temp = interpolate(buff, 105850, 15.01, 106238, 16);
          break;
        case 106239 ... 106626:
          Temp = interpolate(buff, 106239, 16.01, 106626, 17);
          break;
        case 106627 ... 107015:
          Temp = interpolate(buff, 106627, 17.01, 107015, 18);
          break;
        case 107016 ... 107404:
          Temp = interpolate(buff, 107016, 18.01, 107404, 19);
          break;
        case 107405 ... 107793:
          Temp = interpolate(buff, 107405, 19.01, 107793, 20);
          break;
        default:
          break;
      }
      break;
    case 107794 ... 111672:
      switch (int(buff)) {
        case 107794 ... 108181:
          Temp = interpolate(buff, 107794, 20.01, 108181, 21);
          break;
        case 108182 ... 108569:
          Temp = interpolate(buff, 108182, 21.01, 108569, 22);
          break;
        case 108570 ... 108958:
          Temp = interpolate(buff, 108570, 22.01, 108958, 23);
          break;
        case 108959 ... 109346:
          Temp = interpolate(buff, 108959, 23.01, 109346, 24);
          break;
        case 109347 ... 109734:
          Temp = interpolate(buff, 109347, 24.01, 109734, 25);
          break;
        case 109735 ... 110122:
          Temp = interpolate(buff, 109735, 25.01, 110122, 26);
          break;
        case 110123 ... 110509:
          Temp = interpolate(buff, 110123, 26.01, 110509, 27);
          break;
        case 110510 ... 110897:
          Temp = interpolate(buff, 110510, 27.01, 110897, 28);
          break;
        case 110898 ... 111285:
          Temp = interpolate(buff, 110898, 28.01, 111285, 29);
          break;
        case 111286 ... 111672:
          Temp = interpolate(buff, 111286, 29.01, 111672, 30);
          break;

        default:
          break;
      }
      break;
    case 111673 ... 115540:
      switch (int(buff)) {
        case 111673 ... 112059:
          Temp = interpolate(buff, 111673, 30.01, 112059, 31);
          break;
        case 112060 ... 112446:
          Temp = interpolate(buff, 112060, 31.01, 112446, 32);
          break;
        case 112447 ... 112834:
          Temp = interpolate(buff, 112447, 32.01, 112834, 33);
          break;
        case 112835 ... 113221:
          Temp = interpolate(buff, 112835, 33.01, 113221, 34);
          break;
        case 113222 ... 113607:
          Temp = interpolate(buff, 113222, 34.01, 113607, 35);
          break;
        case 113608 ... 113994:
          Temp = interpolate(buff, 113608, 35.01, 113994, 36);
          break;
        case 113995 ... 114381:
          Temp = interpolate(buff, 113995, 36.01, 114381, 37);
          break;
        case 114382 ... 114767:
          Temp = interpolate(buff, 114382, 37.01, 114767, 38);
          break;
        case 114768 ... 115154:
          Temp = interpolate(buff, 114768, 38.01, 115154, 39);
          break;
        case 115155 ... 115540:
          Temp = interpolate(buff, 115155, 39.01, 115540, 40);
          break;
        default:
          break;
      }
      break;
    default:
      Temp = 0;
      break;
  }
  return Temp;
}