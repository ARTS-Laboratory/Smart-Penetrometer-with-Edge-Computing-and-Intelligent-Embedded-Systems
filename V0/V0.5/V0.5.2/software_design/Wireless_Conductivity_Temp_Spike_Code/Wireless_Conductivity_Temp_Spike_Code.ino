// SimpleTx - the master or the transmitter
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <BMx280I2C.h>

RTC_DS3231 rtc;
BMx280I2C bmx280(0x76);
int count = 0;
float buffer = 0;
int Temp = 0;
int n = 1;
int dcount = 0;
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, temperature = 25;
String file = "";
const char* name = "";
bool dataCollected = false;
float combData[6];
const byte slaveAddress[5] = { 'R', 'x', 'A', 'A', 'A' };

RF24 radio(6, 7);  // Create a Radio

float dataToSend;
char txNum = '0';

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 1000;  // send once per second

void setup() {

  Serial.begin(115200);
  Wire.begin();
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  Serial.println("SimpleTx Starting");
  if (!rtc.begin()) {
    Serial.println("RTC failed.");
    while (1)
      ;
  }
  if (!bmx280.begin()) {
    Serial.println("BME failed.");
    while (1)
      ;
  }
  bmx280.resetToDefaults();
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
  bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  radio.begin();
  if (!radio.isChipConnected()) {
    Serial.println("radio hardware is not responding!!");
    while (1) {}  // hold in infinite loop
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(3, 5);  // delay, count
  radio.openWritingPipe(slaveAddress);
}

//====================

void loop() {
  if (!dataCollected) {
    collect();
    dataToSend = combData[dcount];
  } else {
    send();
    updateMessage();
  }
}

//====================

void nameFileByTime(String& file) {
  char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  DateTime now = rtc.now();
  int year = now.year();
  int month = now.month();
  int day = now.day();
  String dow = daysOfTheWeek[now.dayOfTheWeek()];
  int hour = now.hour();
  int min = now.minute();
  int sec = now.second();
  file = (String(year) + String(month) + String(day) + dow + String(hour) + String(min) + String(sec) + ".txt");
}

void gTemp(int& Temp) {
  count = count + 1;
  float sensorValue = analogRead(A1);
  float neW = (sensorValue * 4.72 / 1024) + 0.02;
  float r1 = (10000 * (4.72 - neW)) / neW;
  buffer = buffer + r1;
  if (count == 10) {
    buffer = buffer / count;
    buffer = round(buffer * 100);
    int buff = buffer;
    Temp = rtdTemp(buff);
    count = 0;
    buffer = 0;
  }
}

int rtdTemp(int buff) {
  switch (buff) {
    case 100000 ... 103902:
      switch (buff) {
        case 100000 ... 100390:
          Temp = 0;
          break;
        case 100391 ... 100780:
          Temp = 1;
          break;
        case 100781 ... 101171:
          Temp = 2;
          break;
        case 101172 ... 101561:
          Temp = 3;
          break;
        case 101562 ... 101952:
          Temp = 4;
          break;
        case 101953 ... 102342:
          Temp = 5;
          break;
        case 102343 ... 102732:
          Temp = 6;
          break;
        case 102733 ... 103122:
          Temp = 7;
          break;
        case 103123 ... 103512:
          Temp = 8;
          break;
        case 103513 ... 103902:
          Temp = 9;
          break;
        default:
          break;
      }
      break;
    case 103903 ... 107793:
      switch (buff) {
        case 103903 ... 104291:
          Temp = 10;
          break;
        case 104292 ... 104681:
          Temp = 11;
          break;
        case 104682 ... 105070:
          Temp = 12;
          break;
        case 105071 ... 105459:
          Temp = 13;
          break;
        case 105460 ... 105849:
          Temp = 14;
          break;
        case 105850 ... 106238:
          Temp = 15;
          break;
        case 106239 ... 106626:
          Temp = 16;
          break;
        case 106627 ... 107015:
          Temp = 17;
          break;
        case 107016 ... 107404:
          Temp = 18;
          break;
        case 107405 ... 107793:
          Temp = 19;
          break;
        default:
          break;
      }
      break;
    case 107794 ... 111672:
      switch (buff) {
        case 107794 ... 108181:
          Temp = 20;
          break;
        case 108182 ... 108569:
          Temp = 21;
          break;
        case 108570 ... 108958:
          Temp = 22;
          break;
        case 108959 ... 109346:
          Temp = 23;
          break;
        case 109347 ... 109734:
          Temp = 24;
          break;
        case 109735 ... 110122:
          Temp = 25;
          break;
        case 110123 ... 110509:
          Temp = 26;
          break;
        case 110510 ... 110897:
          Temp = 27;
          break;
        case 110898 ... 111285:
          Temp = 28;
          break;
        case 111286 ... 111672:
          Temp = 29;
          break;

        default:
          break;
      }
      break;
    case 111673 ... 115540:
      switch (buff) {
        case 111673 ... 112059:
          Temp = 30;
          break;
        case 112060 ... 112446:
          Temp = 31;
          break;
        case 112447 ... 112834:
          Temp = 32;
          break;
        case 112835 ... 113221:
          Temp = 33;
          break;
        case 113222 ... 113607:
          Temp = 34;
          break;
        case 113608 ... 113994:
          Temp = 35;
          break;
        case 113995 ... 114381:
          Temp = 36;
          break;
        case 114382 ... 114767:
          Temp = 37;
          break;
        case 114768 ... 115154:
          Temp = 38;
          break;
        case 115155 ... 115540:
          Temp = 39;
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
void tdsFunc(float& condValue) {
  const float VREF = 3.3;
  const int SCOUNT = 30;
  int analogBuffer[SCOUNT];
  int analogBufferTemp[SCOUNT];
  float tdsValue;
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(A0);
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
      analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * VREF / 1024.0;                                                                                                          // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                                //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVoltage = averageVoltage / compensationCoefficient;                                                                                                             //temperature compensation
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;  //convert voltage value to tds value
    condValue = .5135 * tdsValue / .670;
  }
}

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void collect() {
  float condValue;
  bmx280.measure();
  while (!bmx280.hasValue())
    ;
  unsigned long time = millis();
  combData[0] = float(time) / 1000000000;
  combData[1] = bmx280.getTemperature();
  gTemp(Temp);
  combData[2] = Temp;
  combData[3] = bmx280.getHumidity();
  combData[4] = bmx280.getPressure64() / 100.0F;
  tdsFunc(condValue);
  combData[5] = condValue;
  dataCollected = true;
}

void send() {
  // currentMillis = millis();
  // if (currentMillis - prevMillis >= txIntervalMillis) {

  bool rslt;
  rslt = radio.write(&dataToSend, sizeof(dataToSend));
  // Always use sizeof() as it gives the size as the number of bytes.
  // For example if dataToSend was an int sizeof() would correctly return 2

  Serial.println(dataToSend, 9);

  if (rslt) {
    Serial.println("  Acknowledge received");

  } else {
  }
  //     prevMillis = millis();
  //   }
  // Serial.println("seee");
}

//================

void updateMessage() {
  // so you can see that new data is being sent
  dcount++;
  dataToSend = combData[dcount];
  if (dcount == 6) {
    dcount = 0;
    dataCollected = false;
    Serial.println("Data Sent ");
    delay(10000);
  } else {
    dcount = dcount;
  }
}