/*
This code is for gathering data onto microSD card using a geophone with ADC, tds sensor, 
bme280 environmental sensor only. Accelerometer inactive.
*/
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
SdFat SD;

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

RTC_DS3231 rtc;

Adafruit_BME280 bme;
 
  int analogBufferIndex = 0, copyIndex = 0;
  float averageVoltage = 0, temperature = 25;
  
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(A0, INPUT);

  Serial.println("Hello!");

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 // rtc.begin();
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  //Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  //if (!SD.begin(10)) {
  //  Serial.println("Card failed, or not present");
    // don't do anything more:
 //   while (1);
 // }
 // Serial.println("card initialized.");

 // Serial.print("Initializing ADS...");

  //if (!ads.begin()) {
  //  Serial.println("Failed to initialize ADS.");
  //  while (1);
 // }
  //Serial.println("ADS initialized.");

  //bme.begin(0x76); // initiate BME sensor

  //char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  //DateTime now = rtc.now();
  //Serial.print(now.year(), DEC);
  //Serial.print('/');
  //Serial.print(now.month(), DEC);
  //Serial.print('/');
  //Serial.print(now.day(), DEC);
  //Serial.print(" (");
  //Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  //Serial.println(") ");
  //Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  //Serial.println("ADC Range: +/- 0.256V (1 bit = 0.125mV/ADS1115)");
  //Serial.println("time(hr, min, sec), count(Ms), ground velocity(m/s), temperature(*C), humidity(%), pressure(hPa), conductivity(mS/cm)");
  //Serial.println("Opening datalog.txt...");
}


void tdsFunc(float & compensationCoefficient, float & compensationVoltage, float & tdsValue, float & condValue, float & getMedianNum2)
{


  const float VREF = 5.0;      // analog reference voltage(Volt) of the ADC
  const int SCOUNT  = 30;           // sum of sample point
  int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
  int analogBufferTemp[SCOUNT];
  //float tdsValue;

  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U)  //every 40 milliseconds,read the analog value from the ADC
  {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(A0);    //read the analog value and store into the buffer
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }
  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U)
  {
    printTimepoint = millis();
    for (copyIndex = 0; copyIndex < SCOUNT; copyIndex++)
    analogBufferTemp[copyIndex] = analogBuffer[copyIndex];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
    compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    compensationVoltage = averageVoltage / compensationCoefficient; //temperature compensation
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5; //convert voltage value to tds value
    condValue = tdsValue / 640;//conversion from TDS ppm to EC (electrical conductivity) which is the unit used on some conducivity meters and corresponds to milliSiemens per cm.
    getMedianNum2 = getMedianNum(analogBufferTemp, SCOUNT);
  }
}

void loop() {

 // int16_t results;

  /* Be sure to update this value based on the IC and the gain settings! */
  //float multiplier = 0.0078125F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

 // results = ads.readADC_Differential_0_1();

  //float trueResults = (results * multiplier);
  //  Serial.print(results); Serial.print("(");
  //  Serial.print(trueResults); Serial.print(" ");
  //  Serial.println("mV)");
  //float gVelocity = trueResults / 27.50;

  float compensationCoefficient, compensationVoltage, tdsValue, condValue, getMedianNum2;
  tdsFunc (compensationCoefficient, compensationVoltage, tdsValue, condValue, getMedianNum2);
  Serial.println(compensationCoefficient);
  Serial.println(compensationVoltage);
  //float SEALEVELPRESSURE_HPA = 1014.9;


  //File dataFile = SD.open("datalog.txt", FILE_WRITE);

  //if (dataFile) {
    //DateTime now = rtc.now();
    //float time = millis();
    //Serial.print(now.hour(), DEC);
    //Serial.print(':');
    //Serial.print(now.minute(), DEC);
    //Serial.print(':');
    //Serial.print(now.second(), DEC);
    //Serial.print(" ");
    //Serial.print(time/1000000000, 9);
    //Serial.print(" ");
    //if (gVelocity < 0) {
      //Serial.print(gVelocity, 6);
    //}
    //else{
      //Serial.print(gVelocity, 7);
    //}
    //Serial.print(" ");
    //Serial.print(bme.readTemperature());
    //Serial.print(" ");
    //Serial.print(bme.readHumidity());
    //Serial.print(" ");
    //Serial.print(bme.readPressure() / 100.0F);
    //Serial.print(" ");
  Serial.print("TDS Value:");
  Serial.print(tdsValue, 3);
  Serial.println(" ppm");
  Serial.print("Conductivy Value:");
  Serial.print(condValue,3);
  //Serial.print(" \xce\xbc");
  Serial.println(" mS/cm");
  Serial.println("");
  Serial.println(getMedianNum2);
//    dataFile.print(now.hour(), DEC, 2);
//    dataFile.print(now.minute(), DEC, 2);
//    dataFile.print(now.second(), DEC, 2);
//    dataFile.print(" ");
    //dataFile.print(time/1000000000,9);
    //dataFile.print(" ");
    //if (gVelocity < 0) {
      //dataFile.print(gVelocity, 6);
    //}
   // else{
      //dataFile.print(gVelocity, 7);
    //}
    //dataFile.print(" ");
    //dataFile.print(bme.readTemperature(), 5);
    //dataFile.print(" ");
    //dataFile.print(bme.readHumidity(), 5);
    //dataFile.print(" ");
    //dataFile.print(bme.readPressure() / 100.0F, 7);
    //dataFile.print(" ");
    //dataFile.println(condValue, 3);
    //dataFile.close();
  //}
  // if the file isn't open, pop up an error:
  //else {
    //Serial.println("error opening datalog.txt");
  //}
}

int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
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
