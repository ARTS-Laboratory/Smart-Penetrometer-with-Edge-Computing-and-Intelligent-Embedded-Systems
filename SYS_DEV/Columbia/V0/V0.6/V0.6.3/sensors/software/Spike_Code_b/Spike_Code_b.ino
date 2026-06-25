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

extern int __heap_start, *__brkval;
int freeRam() {
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
// -------------------- RADIO ADDRESSES --------
const byte Address[6][6] = {
  "7node", "8node", "9node", "Anode", "Bnode", "Cnode"
};

// -------------------- PACKETS -----------------
// 32-byte packed data packet
struct DataPacket {
  uint8_t type;      // 1
  uint8_t nodeId;    // 1
  uint16_t counter;  // 2

  int16_t rtdTemp;        // 2  (°C × 100)
  int16_t bmeTemp;        // 2  (°C × 100)
  uint16_t humidity;      // 2  (% × 100)
  uint16_t pressure;      // 2  (hPa)
  uint16_t conductivity;  // 2  (scaled ×100)
  uint16_t battery;       // 2  (V × 1000)

  uint32_t time;  // 4  (ms)
  uint32_t crc;   // 4  (CRC32 over first 28 bytes)
};                // TOTAL = 32 bytes

// -------------------- CRC32 -------------------
uint32_t crc32_update(uint32_t crc, uint8_t data) {
  crc ^= data;
  for (uint8_t i = 0; i < 8; i++) {
    if (crc & 1) crc = (crc >> 1) ^ 0xEDB88320UL;
    else crc >>= 1;
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
uint8_t wakeIntervalMin = 1;  // default 1 minute
uint16_t pktCounter = 0;
volatile uint8_t wdtCounter = 0;

// -------------------- ISR ---------------------
void ISR_RTC() {
  rtcWake = true;
}

ISR(WDT_vect) {
  wdtCounter++;
}
void forceReset() {
  cli();                  // disable interrupts
  wdt_enable(WDTO_15MS);  // shortest timeout
  while (1) {}            // wait for watchdog to reset MCU
}

// -------------------- HELPERS -----------------
uint8_t decToBcd(uint8_t v) {
  return ((v / 10) << 4) | (v % 10);
}
uint8_t bcdToDec(uint8_t v) {
  return (v >> 4) * 10 + (v & 0x0F);
}
void setAlarm1_60sec() {
  DateTime now = rtc.now();
  DateTime next = now + TimeSpan(60);  // EXACT 60 seconds later

  // Program Alarm1 for exact second match
  Wire.beginTransmission(0x68);
  Wire.write(0x07);  // Alarm1 registers start

  Wire.write(decToBcd(next.second()));  // A1M1 = 0
  Wire.write(decToBcd(next.minute()));  // A1M2 = 0
  Wire.write(decToBcd(next.hour()));    // A1M3 = 0
  Wire.write(decToBcd(next.day()));     // A1M4 = 0 (date match)

  Wire.endTransmission();

  // Enable Alarm1 interrupt
  Wire.beginTransmission(0x68);
  Wire.write(0x0E);        // control register
  Wire.write(0b00000101);  // INTCN=1, A1IE=1
  Wire.endTransmission();
}


void loadWakeInterval() {
  uint8_t v = EEPROM.read(0);
  if (v == 0xFF || v == 0) v = 1;
  wakeIntervalMin = v;
}

float rtdTempFromR(float R) {
  const float R0 = 1000.0;
  const float A = 3.9083e-3;
  const float B = -5.775e-7;
  float ratio = R / R0;
  float disc = A * A - 4 * B * (1 - ratio);
  if (disc < 0) return -999;
  return (-A + sqrt(disc)) / (2 * B);
}

float readBatteryVoltage() {
  float v = ads.computeVolts(ads.readADC_SingleEnded(2));
  return v * 2.0;
}

void enterSleep() {
  cli();
  // Enable watchdog interrupt mode (not reset mode)
  WDTCSR = (1 << WDCE) | (1 << WDE);   // enable config
  WDTCSR = (1 << WDIE) | (1 << WDP3);  // interrupt mode, ~8s timeout
  sei();                               // re-enable interrupts BEFORE sleeping
  delay(10);
  rtcWake = false;
  digitalWrite(OE_PIN, HIGH);
  digitalWrite(EN_3V3, LOW);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_cpu();
  sleep_disable();
}

void wakeFromSleep() {
  wdtCounter = 0;  // RTC woke successfully

  // Power sensors and peripherals
  // Serial.println("WAKE_FROM_SLEEP");
  Serial.flush();
  digitalWrite(EN_3V3, HIGH);
  delay(200);
  // Serial.println("a");
  Wire.begin();
  delay(10);
  // Serial.println("b");
  SPI.begin();
  delay(5);
  // Serial.println("c");
  radio.powerUp();
  delay(5);
  // Serial.println("d");
  SD.begin(10);
  delay(10);
  // Serial.println("e");
  // if (!ads.begin()) {
  //   Serial.println("ADS1115 FAILED");
  // } else {
  //   delay(10);
  // }

  if (!bmx280.begin()) {
    Serial.println("BME280 FAILED");
  } else {
    bmx280.resetToDefaults();
    bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
    bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
    delay(10);
  }
  // Serial.println("f");
  // Clear alarm flags properly
  Wire.beginTransmission(0x68);
  Wire.write(0x0F);
  Wire.endTransmission();

  Wire.requestFrom(0x68, 1);
  uint8_t status = Wire.read();

  status &= ~0b00000001;  // clear A1F only

  Wire.beginTransmission(0x68);
  Wire.write(0x0F);
  Wire.write(status);
  Wire.endTransmission();
  setAlarm1_60sec();

  // Serial.println("g");
  wdt_enable(WDTO_8S);
}

void doMeasurementCycle() {
  wdt_reset();
  // Serial.println("measure");
  if (!bmx280.begin()) {
    bmx280.resetToDefaults();
    bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
    bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
    bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
  }

  float vref = ads.computeVolts(ads.readADC_SingleEnded(2));

  float vRTD = ads.computeVolts(ads.readADC_SingleEnded(0));
  float r1 = (10000.0f * (vref - vRTD)) / vRTD;
  float rtdT = rtdTempFromR(r1);

  bmx280.measure();
  unsigned long startWait = millis();
  while (!bmx280.hasValue()) {
    if (millis() - startWait > 500) break;
  }

  float bmeT = bmx280.getTemperature();
  float hum = bmx280.getHumidity();
  float pres = bmx280.getPressure64() / 100.0F;

  float condV = ads.computeVolts(ads.readADC_SingleEnded(3));
  float cond = 2.6671f * condV + 0.0535f;
  if (cond < 0) cond = 0;

  float batt = readBatteryVoltage();
  uint32_t tms = millis();

  DataPacket pkt;
  pkt.type = 0x01;
  pkt.nodeId = NODE_ID;
  pkt.counter = ++pktCounter;

  pkt.rtdTemp = (int16_t)(rtdT * 100.0f);
  pkt.bmeTemp = (int16_t)(bmeT * 100.0f);
  pkt.humidity = (uint16_t)(hum * 100.0f);
  pkt.pressure = (uint16_t)(pres);
  pkt.conductivity = (uint16_t)(cond * 100.0f);
  pkt.battery = (uint16_t)(batt * 1000.0f);

  pkt.time = tms;

  pkt.crc = crc32_calc(pkt, sizeof(DataPacket) - sizeof(uint32_t));

  // Serial.print("Sending size = ");
  // Serial.println(sizeof(pkt));
  // Serial.println("SENDING NOW");

  bool ok = radio.write(&pkt, sizeof(pkt));
  delay(5);

  // Serial.print("radio.write ok = ");
  // Serial.println(ok ? "true" : "false");
  // delay(5);
  // Local SD logging (decoded back to floats for readability)
  File f = SD.open("node3.txt", FILE_WRITE);
  if (f) {
    float rtdT_f = pkt.rtdTemp / 100.0f;
    float bmeT_f = pkt.bmeTemp / 100.0f;
    float hum_f = pkt.humidity / 100.0f;
    float pres_f = pkt.pressure;
    float cond_f = pkt.conductivity / 100.0f;
    float batt_f = pkt.battery / 1000.0f;

    f.print(pkt.nodeId);
    f.print(" ");
    f.print(pkt.counter);
    f.print(" ");
    f.print(pkt.time);
    f.print(" ");
    f.print(rtdT_f, 4);
    f.print(" ");
    f.print(bmeT_f, 4);
    f.print(" ");
    f.print(hum_f, 4);
    f.print(" ");
    f.print(pres_f, 4);
    f.print(" ");
    f.print(cond_f, 4);
    f.print(" ");
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
    while (1)
      ;
  }

  rtc.clearAlarm(1);
  rtc.clearAlarm(2);

  if (!ads.begin()) {
    Serial.println("ADS failed");
    while (1)
      ;
  }

  if (!bmx280.begin()) {
    Serial.println("BME failed");
    while (1)
      ;
  }
  bmx280.resetToDefaults();
  bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
  bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
  bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);

  SD.begin(10);

  radio.begin();
  delay(100);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.disableDynamicPayloads();
  radio.disableAckPayload();
  radio.setAutoAck(true);
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.stopListening();
  radio.openWritingPipe(Address[NODE_ID - 1]);
  radio.setRetries(5, 15);
  setAlarm1_60sec();


  wdt_enable(WDTO_8S);
}

void loop() {
  // If watchdog fired too many times without RTC wake → RTC FAILED
  if (!rtcWake) {
    Serial.println(wdtCounter);
    if (wdtCounter >= 14) {  // 9 × 8s = 72seconds
      Serial.println("RTC FAILED — FORCING RESET");
      forceReset();
    }
  }

  delay(200);
  if (rtcWake) {
    // Serial.print("RAM at loop start: ");
    // Serial.println(freeRam());
    wakeFromSleep();
    doMeasurementCycle();
    enterSleep();
    // wdt_reset();
  }
}