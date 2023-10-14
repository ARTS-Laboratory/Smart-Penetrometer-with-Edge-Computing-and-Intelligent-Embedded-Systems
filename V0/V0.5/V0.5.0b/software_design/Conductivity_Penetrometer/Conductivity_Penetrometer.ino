/*
Written by Malichi Flemming II
*/
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SdFat.h>
#include <BMx280I2C.h>

SdFat SD;
RTC_DS3231 rtc;
BMx280I2C bmx280(0x76);

int n = 1;
int analogBufferIndex = 0, copyIndex = 0;
float averageVoltage = 0, temperature = 25;
String file = "";
const char *name = "";

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(A0, INPUT);

  if (!rtc.begin()) {
    Serial.println("RTC failed.");
    while (1);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //uncomment line, upload sketch, and run sketch to adjust time then comment line and upload sketch to set time.
  
	if (!bmx280.begin()) {
    Serial.println("BME failed.");
    while (1);
  }
	bmx280.resetToDefaults();
	bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
	bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
  bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    while (1);
  }  
}

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

void tdsFunc(float& condValue) {
  const float VREF = 5;     
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
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0;                                                                                                   // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);                                                                                                                //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVoltage = averageVoltage / compensationCoefficient;                                                                                                             //temperature compensation
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5;  //convert voltage value to tds value
    condValue = .667 * tdsValue / .640;
  }
}

void logData() {
  float condValue;
  bmx280.measure();
	while (!bmx280.hasValue());  
  temperature = bmx280.getTemperature();
  tdsFunc(condValue);
  File dataFile = SD.open(name, FILE_WRITE);
  if (dataFile) {
    unsigned long time = millis();
    // Serial.print(float(time) / 1000000000, 9);
    // Serial.print(" ");
    // Serial.print(bmx280.getTemperature());
    // Serial.print(" ");
    // Serial.print(bmx280.getHumidity());
    // Serial.print(" ");
    // Serial.print(bmx280.getPressure64() / 100.0F);
    // Serial.print(" ");
    // Serial.println(condValue, 3);

    dataFile.print(float(time) / 1000000000, 9);
    dataFile.print(" ");
    dataFile.print(bmx280.getTemperature(), 2);
    dataFile.print(" ");
    dataFile.print(bmx280.getHumidity(), 2);
    dataFile.print(" ");
    dataFile.print(bmx280.getPressure64() / 100.0F, 2);
    dataFile.print(" ");
    dataFile.println(condValue, 3);
    dataFile.close();
  }
  else {
    Serial.println("error opening .txt file");
  }
}

void loop() {
  if (n == 1) {
    nameFileByTime(file);
    name = file.c_str();
    n = n + 1;
  }
  else {
  }
  logData();
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
