//Created on Sat Feb 8 01:15:07 2025
//@author: Malichi Flemming II

// -------------------------------------------------------------
// LIBRARIES
// -------------------------------------------------------------
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <BMx280I2C.h>
#include "printf.h"
#include <Adafruit_ADS1X15.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

// -------------------------------------------------------------
// HARDWARE CONTROL PINS
// -------------------------------------------------------------
#define OE_PIN A0        // SN74LVC8T245 OE pin (active LOW)
#define EN_3V3 5         // 3.3V regulator enable pin (active HIGH)
#define RTC_INT_PIN 2    // DS3231 INT/SQW pin

// -------------------------------------------------------------
// OBJECTS
// -------------------------------------------------------------
RTC_DS3231 rtc;
BMx280I2C bmx280(0x76);
RF24 radio(6, 7);
Adafruit_ADS1115 ads;

// -------------------------------------------------------------
// VARIABLES
// -------------------------------------------------------------
String file = "";
const char* name = "";
float volt = 3.3;
float Temp = 0;
long currTime = 10001;
long prevTime = 0;
float combData[7] = {0};   // 0: node, 1: time, 2: RTD temp, 3: BME T, 4: RH, 5: P, 6: cond
int rec[1] = {5};
volatile bool rtcWake = false;

const byte Address[6][6] = {
  "7node", "8node", "9node", "Anode", "Bnode", "Cnode"
};

// -------------------------------------------------------------
// INTERRUPT HANDLER
// -------------------------------------------------------------
void ISR_RTC() {
  rtcWake = true;
}

// -------------------------------------------------------------
// FORWARD DECLARATIONS
// -------------------------------------------------------------
void enterSleep();
void wakeFromSleep();
void doMeasurementCycle();
void nameFileByTime(String& file);
float interpolate(float x, float x0, float y0, float x1, float y1);
void gTemp(float& Temp);
float rtdTemp(float buff);
float readBatteryVoltage();
bool sendFloat(float value);
bool safeWriteLine(const char* filename, const String& line);
bool ensureSensors();

// -------------------------------------------------------------
// SETUP
// -------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(OE_PIN, OUTPUT);
  pinMode(EN_3V3, OUTPUT);
  pinMode(RTC_INT_PIN, INPUT_PULLUP);

  digitalWrite(EN_3V3, HIGH);
  digitalWrite(OE_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), ISR_RTC, FALLING);

  if (!rtc.begin()) {
    Serial.println("RTC failed.");
    while (1);
  }

  rtc.writeSqwPinMode(DS3231_OFF);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  rtc.setAlarm2(
    rtc.now() + TimeSpan(0,1,0,0),
    DS3231_A2_Minute
  );

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

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

  radio.begin();
  while (!radio.isChipConnected()) {
    Serial.println("radio hardware is not responding!!");
    while (1);
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(90);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();
  radio.openWritingPipe(Address[2]);
  radio.setRetries(15, 15);

  enterSleep();
}

// -------------------------------------------------------------
// MAIN LOOP
// -------------------------------------------------------------
void loop() {
  if (rtcWake) {
    wakeFromSleep();
    doMeasurementCycle();
    enterSleep();
  }
}

// -------------------------------------------------------------
// ENTER SLEEP
// -------------------------------------------------------------
void enterSleep() {
  wdt_disable();
  rtcWake = false;

  digitalWrite(OE_PIN, HIGH);
  digitalWrite(EN_3V3, LOW);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}

// -------------------------------------------------------------
// WAKE FROM SLEEP
// -------------------------------------------------------------
void wakeFromSleep() {
  digitalWrite(EN_3V3, HIGH);
  delay(10);
  digitalWrite(OE_PIN, LOW);

  SPI.begin();
  radio.begin();
  SD.begin(10);

  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(90);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();
  radio.openWritingPipe(Address[2]);
  radio.setRetries(15, 15);

  rtc.clearAlarm(2);
  rtc.setAlarm2(
    rtc.now() + TimeSpan(0,1,0,0),
    DS3231_A2_Minute
  );

  wdt_enable(WDTO_8S);
}

// -------------------------------------------------------------
// MEASUREMENT + LOGGING + RADIO TX
// -------------------------------------------------------------
void doMeasurementCycle() {
  if (!ensureSensors()) {
    return;
  }

  volt = ads.computeVolts(ads.readADC_SingleEnded(2)); // also used for RTD calc
  float batteryVoltage = readBatteryVoltage();

  combData[0] = 3;

  float condValue;
  bmx280.measure();
  unsigned long startWait = millis();
  while (!bmx280.hasValue()) {
    if (millis() - startWait > 500) {
      ensureSensors();
      return;
    }
  }

  unsigned long time = millis();
  condValue = ads.computeVolts(ads.readADC_SingleEnded(3));
  condValue = (2.6671 * condValue + 0.0535);
  if (condValue < 0) condValue = 0;

  combData[1] = float(time) / 1000000000.0;
  gTemp(Temp);
  combData[2] = Temp;
  combData[3] = bmx280.getTemperature();
  combData[4] = bmx280.getHumidity();
  combData[5] = bmx280.getPressure64() / 100.0F;
  combData[6] = condValue;

  for (int i = 0; i <= 6; i++) {
    sendFloat(combData[i]);
  }
  sendFloat(batteryVoltage);

  nameFileByTime(file);
  name = file.c_str();

  String line =
    String(combData[0]) + " " +
    String(combData[1], 9) + " " +
    String(combData[2], 4) + " " +
    String(combData[3], 4) + " " +
    String(combData[4], 4) + " " +
    String(combData[5], 4) + " " +
    String(combData[6], 4) + " " +
    String(batteryVoltage, 4);

  if (!safeWriteLine(name, line)) {
    safeWriteLine("backup.txt", line);
  }
}

// -------------------------------------------------------------
// BATTERY VOLTAGE MEASUREMENT
// -------------------------------------------------------------
float readBatteryVoltage() {
  // Assuming ADS channel 2 sees battery through 2:1 divider
  float v = ads.computeVolts(ads.readADC_SingleEnded(2));
  return v * 2.0; // adjust if divider different
}

// -------------------------------------------------------------
// RADIO RETRY WRAPPER
// -------------------------------------------------------------
bool sendFloat(float value) {
  const int maxRetries = 5;
  for (int i = 0; i < maxRetries; i++) {
    if (radio.write(&value, sizeof(value))) {
      return true;
    }
    delay(5);
  }
  return false;
}

// -------------------------------------------------------------
// SD WRITE-FAIL RECOVERY
// -------------------------------------------------------------
bool safeWriteLine(const char* filename, const String& line) {
  File f = SD.open(filename, FILE_WRITE);
  if (!f) return false;
  f.println(line);
  f.close();
  return true;
}

// -------------------------------------------------------------
// I2C SENSOR RECOVERY
// -------------------------------------------------------------
bool ensureSensors() {
  static bool bmeOK = true;
  static bool adsOK = true;

  if (!bmeOK || !adsOK) {
    Wire.end();
    delay(5);
    Wire.begin();
    delay(5);

    adsOK = ads.begin();
    bmeOK = bmx280.begin();

    if (bmeOK) {
      bmx280.resetToDefaults();
      bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
      bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
      bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
    }
  }

  return bmeOK && adsOK;
}

// -------------------------------------------------------------
// SUPPORT FUNCTIONS
// -------------------------------------------------------------
void nameFileByTime(String& file) {
  char daysOfTheWeek[7][12] = {
    "Sunday","Monday","Tuesday","Wednesday",
    "Thursday","Friday","Saturday"
  };
  DateTime now = rtc.now();
  file = String(now.year()) +
         String(now.month()) +
         String(now.day()) +
         daysOfTheWeek[now.dayOfTheWeek()] +
         String(now.hour()) +
         String(now.minute()) +
         String(now.second()) +
         ".txt";
}

float interpolate(float x, float x0, float y0, float x1, float y1) {
  return y0 + (y1 - y0) * ((x - x0) / (x1 - x0));
}

void gTemp(float& Temp) {
  float neW = ads.computeVolts(ads.readADC_SingleEnded(0));
  float r1 = (10000 * (volt - neW)) / neW;
  long ir1 = round(r1 * 100);
  Temp = rtdTemp(ir1);
}

// -------------------------------------------------------------
// RTD LUT (unchanged from your original)
// -------------------------------------------------------------
float rtdTemp(float buff) {
  switch (int(buff)) {
    case 96090 ... 100000:
      switch (int(buff)) {
        case 96090 ... 96480:
          Temp = interpolate(buff, 96090, -10, 96480, -9); break;
        case 96481 ... 96870:
          Temp = interpolate(buff, 96481, -9.01, 96870, -8); break;
        case 96871 ... 97260:
          Temp = interpolate(buff, 96871, 2.01, 97260, -7); break;
        case 97261 ... 97650:
          Temp = interpolate(buff, 97261, 3.01, 97650, -6); break;
        case 97651 ... 98040:
          Temp = interpolate(buff, 97651, 4.01, 98040, -5); break;
        case 98041 ... 98440:
          Temp = interpolate(buff, 98041, 5.01, 98440, -4); break;
        case 98441 ... 98830:
          Temp = interpolate(buff, 98441, 6.01, 98830, -3); break;
        case 98831 ... 99220:
          Temp = interpolate(buff, 98831, 7.01, 99220, -2); break;
        case 99221 ... 99610:
          Temp = interpolate(buff, 99221, 8.01, 99610, -1); break;
        case 99611 ... 100000:
          Temp = interpolate(buff, 99611, 9.01, 100000, 0); break;
        default: break;
      }
      break;
    case 100001 ... 103902:
      switch (int(buff)) {
        case 100001 ... 100390:
          Temp = interpolate(buff, 100001, 0.01, 100390, 1); break;
        case 100391 ... 100780:
          Temp = interpolate(buff, 100391, 1.01, 100780, 2); break;
        case 100781 ... 101171:
          Temp = interpolate(buff, 100781, 2.01, 101171, 3); break;
        case 101172 ... 101561:
          Temp = interpolate(buff, 101172, 3.01, 101561, 4); break;
        case 101562 ... 101952:
          Temp = interpolate(buff, 101562, 4.01, 101952, 5); break;
        case 101953 ... 102342:
          Temp = interpolate(buff, 101953, 5.01, 102342, 6); break;
        case 102343 ... 102732:
          Temp = interpolate(buff, 102343, 6.01, 102732, 7); break;
        case 102733 ... 103122:
          Temp = interpolate(buff, 102733, 7.01, 103122, 8); break;
        case 103123 ... 103512:
          Temp = interpolate(buff, 103123, 8.01, 103512, 9); break;
        case 103513 ... 103902:
          Temp = interpolate(buff, 103513, 9.01, 103902, 10); break;
        default: break;
      }
      break;
    case 103903 ... 107793:
      switch (int(buff)) {
        case 103903 ... 104291:
          Temp = interpolate(buff, 103903, 10.01, 104291, 11); break;
        case 104292 ... 104681:
          Temp = interpolate(buff, 104292, 11.01, 104681, 12); break;
        case 104682 ... 105070:
          Temp = interpolate(buff, 104682, 12.01, 105070, 13); break;
        case 105071 ... 105459:
          Temp = interpolate(buff, 105071, 13.01, 105459, 14); break;
        case 105460 ... 105849:
          Temp = interpolate(buff, 105460, 14.01, 105849, 15); break;
        case 105850 ... 106238:
          Temp = interpolate(buff, 105850, 15.01, 106238, 16); break;
        case 106239 ... 106626:
          Temp = interpolate(buff, 106239, 16.01, 106626, 17); break;
        case 106627 ... 107015:
          Temp = interpolate(buff, 106627, 17.01, 107015, 18); break;
        case 107016 ... 107404:
          Temp = interpolate(buff, 107016, 18.01, 107404, 19); break;
        case 107405 ... 107793:
          Temp = interpolate(buff, 107405, 19.01, 107793, 20); break;
        default: break;
      }
      break;
    case 107794 ... 111672:
      switch (int(buff)) {
        case 107794 ... 108181:
          Temp = interpolate(buff, 107794, 20.01, 108181, 21); break;
        case 108182 ... 108569:
          Temp = interpolate(buff, 108182, 21.01, 108569, 22); break;
        case 108570 ... 108958:
          Temp = interpolate(buff, 108570, 22.01, 108958, 23); break;
        case 108959 ... 109346:
          Temp = interpolate(buff, 108959, 23.01, 109346, 24); break;
        case 109347 ... 109734:
          Temp = interpolate(buff, 109347, 24.01, 109734, 25); break;
        case 109735 ... 110122:
          Temp = interpolate(buff, 109735, 25.01, 110122, 26); break;
        case 110123 ... 110509:
          Temp = interpolate(buff, 110123, 26.01, 110509, 27); break;
        case 110510 ... 110897:
          Temp = interpolate(buff, 110510, 27.01, 110897, 28); break;
        case 110898 ... 111285:
          Temp = interpolate(buff, 110898, 28.01, 111285, 29); break;
        case 111286 ... 111672:
          Temp = interpolate(buff, 111286, 29.01, 111672, 30); break;
        default: break;
      }
      break;
    case 111673 ... 115540:
      switch (int(buff)) {
        case 111673 ... 112059:
          Temp = interpolate(buff, 111673, 30.01, 112059, 31); break;
        case 112060 ... 112446:
          Temp = interpolate(buff, 112060, 31.01, 112446, 32); break;
        case 112447 ... 112834:
          Temp = interpolate(buff, 112447, 32.01, 112834, 33); break;
        case 112835 ... 113221:
          Temp = interpolate(buff, 112835, 33.01, 113221, 34); break;
        case 113222 ... 113607:
          Temp = interpolate(buff, 113222, 34.01, 113607, 35); break;
        case 113608 ... 113994:
          Temp = interpolate(buff, 113608, 35.01, 113994, 36); break;
        case 113995 ... 114381:
          Temp = interpolate(buff, 113995, 36.01, 114381, 37); break;
        case 114382 ... 114767:
          Temp = interpolate(buff, 114382, 37.01, 114767, 38); break;
        case 114768 ... 115154:
          Temp = interpolate(buff, 114768, 38.01, 115154, 39); break;
        case 115155 ... 115540:
          Temp = interpolate(buff, 115155, 39.01, 115540, 40); break;
        default: break;
      }
      break;
    default:
      Temp = 0;
      break;
  }
  return Temp;
}