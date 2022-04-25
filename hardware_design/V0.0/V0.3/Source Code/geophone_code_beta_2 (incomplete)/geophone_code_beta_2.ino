/*

*/
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include<Wire.h>
#include<ADXL345_WE.h>
#define ADXL345_I2CADDR 0x53
SdFat SD;

volatile bool freeFall = false;
ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

RTC_DS3231 rtc;

Adafruit_BME280 bme;

void setup() {
  Wire.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(2, INPUT);
  Serial.println("Hello!");

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }

/* Insert your data from ADXL345_calibration.ino and uncomment for more precise results */
  myAcc.setCorrFactors(-256.0, 266.0, -285.0, 239.0, -243.0, 287.0);

  myAcc.setDataRate(ADXL345_DATA_RATE_25);
  myAcc.setRange(ADXL345_RANGE_2G);
  myAcc.setFreeFallThresholds(0.4, 100);
  myAcc.setInterrupt(ADXL345_FREEFALL, INT_PIN_2);
  
  attachInterrupt(digitalPinToInterrupt(2), freeFallISR, RISING); 
  freeFall=false;
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  Serial.print("Initializing ADS...");

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }
  Serial.println("ADS initialized.");

  bme.begin(0x76);


  char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.println(") ");
  Serial.println("Getting differential reading from AIN0 (P) and AIN1 (N)");
  Serial.println("ADC Range: +/- 0.256V (1 bit = 0.125mV/ADS1115)");
  Serial.println("time(hr, min, sec), count(ms), ground velocity(m/s), temperature(*C), humidity(%), pressure(hPa), conductivity(uohm)");
  Serial.println("Opening datalog.txt...");
}

void tdsFunc(float & tdsValue, float & condValue)
{
  const float VREF = 3.3;      // analog reference voltage(Volt) of the ADC
  const int SCOUNT  = 30;           // sum of sample point
  int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
  int analogBufferTemp[SCOUNT];
  int analogBufferIndex = 0, copyIndex = 0;
  float averageVoltage = 0, temperature = 25;

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
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0); //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
    float compensationVoltage = averageVoltage / compensationCoefficient; //temperature compensation
    tdsValue = (133.42 * compensationVoltage * compensationVoltage * compensationVoltage - 255.86 * compensationVoltage * compensationVoltage + 857.39 * compensationVoltage) * 0.5; //convert voltage value to tds value
    condValue = tdsValue / 0.5;
  }
}
void freefall() {
  File dataFile = SD.open("datalog2.txt", FILE_WRITE);

  //   if the file is available, write to it:
  if (dataFile) {
    xyzFloat raw = myAcc.getRawValues();
    xyzFloat g = myAcc.getGValues();
    
    Serial.print(raw.z);
    Serial.println(g.z);
    
    dataFile.print(raw.z);
    dataFile.println(g.z);
    dataFile.close();
    delay(100);
    freeFall = false;
    myAcc.readAndClearInterrupts();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}

void logData() {

  int16_t results;

  /* Be sure to update this value based on the IC and the gain settings! */
  float multiplier = 0.0078125F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

  results = ads.readADC_Differential_0_1();

  float trueResults = (results * multiplier);
  //  Serial.print(results); Serial.print("(");
  //  Serial.print(trueResults); Serial.print(" ");
  //  Serial.println("mV)");
  float gVelocity = trueResults / 2.750;

  float tdsValue, condValue;
  //tdsFunc (tdsValue, condValue);
 
  float SEALEVELPRESSURE_HPA = 1014.9;

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  
  File dataFile = SD.open("datalog2.txt", FILE_WRITE);

  //   if the file is available, write to it:
  if (dataFile) {
    DateTime now = rtc.now();
    unsigned long time = millis();
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.print(" ");
    Serial.print(time);
    Serial.print(" ");
    Serial.print(gVelocity, 7);
    Serial.print(" ");
    Serial.print(bme.readTemperature());
    Serial.print(" ");
    Serial.print(bme.readHumidity());
    Serial.print(" ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.print(" ");
    Serial.println(condValue);

    dataFile.print(now.hour(), DEC);
    dataFile.print(':');
    dataFile.print(now.minute(), DEC);
    dataFile.print(':');
    dataFile.print(now.second(), DEC);
    dataFile.print(" ");
    dataFile.print(time);
    dataFile.print(" ");
    dataFile.print(gVelocity, 7);
    dataFile.print(" ");
    dataFile.print(bme.readTemperature());
    dataFile.print(" ");
    dataFile.print(bme.readHumidity());
    dataFile.print(" ");
    dataFile.print(bme.readPressure() / 100.0F);
    dataFile.print(" ");
    dataFile.println(condValue);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
}

void loop() {
  if (freeFall==true)
    freefall();
  else 
    logData();
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
void freeFallISR(){
  freeFall = true;
}
