//@author: Malichi Flemming II

// TRANSMITTER: struct-based RF24 + CRC + counter + watchdog + sleep

#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <BMx280I2C.h>
#include <Adafruit_ADS1X15.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <math.h>
#include <stdint.h>

// -------------------- PINS --------------------
#define OE_PIN      A0
#define EN_3V3      5
#define RTC_INT_PIN 3
#define NODE_ID     3

// -------------------- OBJECTS -----------------
RTC_DS3231 rtc;
BMx280I2C bmx280(0x76);
RF24 radio(6, 7);
Adafruit_ADS1115 ads;

// -------------------- RADIO ADDRESSES --------
const byte Address[6][6] = {
  "1node","2node","3node","4node","5node","6node"
};

// -------------------- PACKETS -----------------
// 32-byte packed data packet
struct DataPacket {
  uint8_t  type;          // 1
  uint8_t  nodeId;        // 1
  uint16_t counter;       // 2

  int16_t  rtdTemp;       // 2  (°C × 100)
  int16_t  bmeTemp;       // 2  (°C × 100)
  uint16_t humidity;      // 2  (% × 100)
  uint16_t pressure;      // 2  (hPa)
  uint16_t conductivity;  // 2  (scaled ×100)
  uint16_t battery;       // 2  (V × 1000)

  uint32_t time;          // 4  (ms)
  uint32_t crc;           // 4  (CRC32 over first 28 bytes)
}; // TOTAL = 32 bytes

// -------------------- CRC32 -------------------
uint32_t crc32_update(uint32_t crc, uint8_t data) {
  crc ^= data;
  for (uint8_t i = 0; i < 8; i++) {
    if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320UL;
    else         crc >>= 1;
  }
  return crc;
}

template<typename T>
uint32_t crc32_calc(const T& obj, size_t lenWithoutCRC) {
  const uint8_t* p = (const uint8_t*)&obj;
  uint32_t crc = 0xFFFFFFFFUL;
  for (size_t i = 0; i < lenWithoutCRC; i++) {
    crc = crc32_update(crc, p[i]);
  }
  return ~crc;
}

// -------------------- GLOBALS -----------------
volatile bool rtcWake = true;
uint8_t  wakeIntervalMin = 1;   // default 1 minute
uint16_t pktCounter      = 0;

// -------------------- ISR ---------------------
void ISR_RTC() {
  rtcWake = true;
}

// -------------------- HELPERS -----------------
void loadWakeInterval() {
  uint8_t v = EEPROM.read(0);
  if (v == 0xFF || v == 0) v = 1;
  wakeIntervalMin = v;
}

float rtdTempFromR(float R) {
  const float R0 = 1000.0;
  const float A  = 3.9083e-3;
  const float B  = -5.775e-7;
  float ratio = R / R0;
  float disc  = A*A - 4*B*(1 - ratio);
  if (disc < 0) return -999;
  return (-A + sqrt(disc)) / (2*B);
}

float readBatteryVoltage() {
  float v = ads.computeVolts(ads.readADC_SingleEnded(2));
  return v * 2.0;
}

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

void wakeFromSleep() {
  // Power sensors and peripherals
  Serial.println("WAKE_FROM_SLEEP");
  Serial.flush();
  digitalWrite(EN_3V3, HIGH);
  delay(200);

  Wire.begin();
  delay(10);

  SPI.begin();
  delay(5);

  // Radio in fixed 32-byte mode, no dynamic/ACK
  radio.begin();
  delay(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();
  radio.openWritingPipe(Address[NODE_ID - 1]);

  SD.begin(10);
  delay(10);

  if (!ads.begin()) {
    Serial.println("ADS1115 FAILED");
  } else {
    delay(10);
  }

  if (!bmx280.begin()) {
    Serial.println("BME280 FAILED");
  } else {
    bmx280.resetToDefaults();
    bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
    bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
    delay(10);
  }

  rtc.disableAlarm(1);
  rtc.disableAlarm(2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  rtc.writeSqwPinMode(DS3231_OFF);
  DateTime now = rtc.now();
  DateTime next(
    now.year(),
    now.month(),
    now.day(),
    now.hour(),
    now.minute() + wakeIntervalMin,
    0   // <-- seconds MUST be zero
  );

  rtc.setAlarm2(next, DS3231_A2_Minute);

  wdt_enable(WDTO_8S);
}

void doMeasurementCycle() {
  wdt_reset();

  if (!bmx280.begin()) {
    bmx280.resetToDefaults();
    bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
    bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
  }

  float vref = ads.computeVolts(ads.readADC_SingleEnded(2));

  float vRTD = ads.computeVolts(ads.readADC_SingleEnded(0));
  float r1   = (10000.0f * (vref - vRTD)) / vRTD;
  float rtdT = rtdTempFromR(r1);

  bmx280.measure();
  unsigned long startWait = millis();
  while (!bmx280.hasValue()) {
    if (millis() - startWait > 500) break;
  }

  float bmeT  = bmx280.getTemperature();
  float hum   = bmx280.getHumidity();
  float pres  = bmx280.getPressure64() / 100.0F;

  float condV = ads.computeVolts(ads.readADC_SingleEnded(3));
  float cond  = 2.6671f * condV + 0.0535f;
  if (cond < 0) cond = 0;

  float batt  = readBatteryVoltage();
  uint32_t tms = millis();

  DataPacket pkt;
  pkt.type    = 0x01;
  pkt.nodeId  = NODE_ID;
  pkt.counter = ++pktCounter;

  pkt.rtdTemp      = (int16_t)(rtdT * 100.0f);
  pkt.bmeTemp      = (int16_t)(bmeT * 100.0f);
  pkt.humidity     = (uint16_t)(hum * 100.0f);
  pkt.pressure     = (uint16_t)(pres);
  pkt.conductivity = (uint16_t)(cond * 100.0f);
  pkt.battery      = (uint16_t)(batt * 1000.0f);

  pkt.time = tms;

  pkt.crc  = crc32_calc(pkt, sizeof(DataPacket) - sizeof(uint32_t));

  Serial.print("Sending size = ");
  Serial.println(sizeof(pkt));
  Serial.println("SENDING NOW");

  bool ok = radio.write(&pkt, sizeof(pkt));
  Serial.print("radio.write ok = ");
  Serial.println(ok ? "true" : "false");

  // Local SD logging (decoded back to floats for readability)
  File f = SD.open("node3.txt", FILE_WRITE);
  if (f) {
    float rtdT_f  = pkt.rtdTemp / 100.0f;
    float bmeT_f  = pkt.bmeTemp / 100.0f;
    float hum_f   = pkt.humidity / 100.0f;
    float pres_f  = pkt.pressure;
    float cond_f  = pkt.conductivity / 100.0f;
    float batt_f  = pkt.battery / 1000.0f;

    f.print(pkt.nodeId); f.print(" ");
    f.print(pkt.counter); f.print(" ");
    f.print(pkt.time); f.print(" ");
    f.print(rtdT_f, 4); f.print(" ");
    f.print(bmeT_f, 4); f.print(" ");
    f.print(hum_f, 4); f.print(" ");
    f.print(pres_f, 4); f.print(" ");
    f.print(cond_f, 4); f.print(" ");
    f.println(batt_f, 4);
    f.close();
  }

  wdt_reset();
}

// -------------------- SETUP / LOOP -----------
void setup() {
  MCUSR = 0;
  wdt_disable();

  Serial.begin(115200);
  Serial.print("TX DataPacket size = ");
  Serial.println(sizeof(DataPacket));
  Wire.begin();

  pinMode(OE_PIN, OUTPUT);
  pinMode(EN_3V3, OUTPUT);
  pinMode(RTC_INT_PIN, INPUT_PULLUP);

  digitalWrite(EN_3V3, HIGH);
  digitalWrite(OE_PIN, LOW);

  attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), ISR_RTC, FALLING);

  if (!rtc.begin()) {
    Serial.println("RTC failed");
    while (1);
  }

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  if (!ads.begin()) {
    Serial.println("ADS failed");
    while (1);
  }

  if (!bmx280.begin()) {
    Serial.println("BME failed");
    while (1);
  }
  bmx280.resetToDefaults();
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
  bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  SD.begin(10);

  radio.begin();
  delay(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(90);
  radio.setAutoAck(true);
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();
  radio.openWritingPipe(Address[NODE_ID - 1]);
  radio.setRetries(5,15);

  loadWakeInterval();
  DateTime now = rtc.now();
  DateTime next(
    now.year(),
    now.month(),
    now.day(),
    now.hour(),
    now.minute() + wakeIntervalMin,
    0   // <-- seconds MUST be zero
  );

  rtc.setAlarm2(next, DS3231_A2_Minute);

  wdt_enable(WDTO_8S);
}

void loop() {
  Serial.print("INT pin = ");
  Serial.println(digitalRead(RTC_INT_PIN));
  delay(200);
  if (rtcWake) {
    wakeFromSleep();
    doMeasurementCycle();
    enterSleep();
  }
  wdt_reset();
}