// RECEIVER: multi-node RF24 + CRC + counters + missed packets + per-node SD
// (no ACK payloads / OTA interval in this version)

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SD.h>
#include <stdint.h>

// -------------------- PINS --------------------
#define CE_PIN  6
#define CSN_PIN 7

RF24 radio(CE_PIN, CSN_PIN);

// -------------------- ADDRESSES ---------------
const byte pipes[6][6] = {
  "1node","2node","3node","4node","5node","6node"
};

// -------------------- PACKETS -----------------
struct DataPacket {
  uint8_t  type;
  uint8_t  nodeId;
  uint16_t counter;

  int16_t  rtdTemp;
  int16_t  bmeTemp;
  uint16_t humidity;
  uint16_t pressure;
  uint16_t conductivity;
  uint16_t battery;

  uint32_t time;
  uint32_t crc;
};

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

// -------------------- STATE -------------------
uint32_t lastCounter[7] = {0};
bool     hasLast[7]     = {false};

// -------------------- HELPERS -----------------
void handlePacket(const DataPacket& p, uint8_t pipeNum) {
  uint8_t id = p.nodeId;

  float rtdT  = p.rtdTemp / 100.0f;
  float bmeT  = p.bmeTemp / 100.0f;
  float hum   = p.humidity / 100.0f;
  float pres  = p.pressure;
  float cond  = p.conductivity / 100.0f;
  float batt  = p.battery / 1000.0f;

  Serial.print("Node ");
  Serial.print(int(id));
  Serial.print("  cnt=");
  Serial.print(p.counter);
  Serial.print("  t(ms)=");
  Serial.print(p.time);
  Serial.print("  RTD=");
  Serial.print(rtdT, 4);
  Serial.print("  T=");
  Serial.print(bmeT, 4);
  Serial.print("  RH=");
  Serial.print(hum, 4);
  Serial.print("  P=");
  Serial.print(pres, 4);
  Serial.print("  C=");
  Serial.print(cond, 4);
  Serial.print("  Vbat=");
  Serial.println(batt, 4);

  char fname[16];
  sprintf(fname, "node%d.txt", int(id));
  File f = SD.open(fname, FILE_WRITE);
  if (f) {
    f.print(id); f.print(" ");
    f.print(p.counter); f.print(" ");
    f.print(p.time); f.print(" ");
    f.print(rtdT, 4); f.print(" ");
    f.print(bmeT, 4); f.print(" ");
    f.print(hum, 4); f.print(" ");
    f.print(pres, 4); f.print(" ");
    f.print(cond, 4); f.print(" ");
    f.println(batt, 4);
    f.close();
  }
}

// -------------------- SETUP / LOOP -----------
void setup() {
  Serial.begin(115200);
  // Serial.print("RX DataPacket size = ");
  // Serial.println(sizeof(DataPacket));

  SD.begin(10);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setChannel(108);
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();  
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.setRetries(5, 15);
  for (int i = 0; i < 6; i++) {
    radio.openReadingPipe(i + 1, pipes[i]); // pipes 1..6
  }

  radio.startListening();

  Serial.println("Receiver ready.");
}

void loop() {
  if (radio.available()) {
    uint8_t pipeNum;
    radio.available(&pipeNum);

    DataPacket pkt;
    radio.read(&pkt, sizeof(pkt));

    uint32_t c = crc32_calc(pkt, sizeof(DataPacket) - sizeof(uint32_t));
    if (c != pkt.crc || pkt.type != 0x01) {
      Serial.println("Bad CRC or type");
      return;
    }

    uint8_t id = pkt.nodeId;
    if (id >= 1 && id <= 6) {
      if (hasLast[id]) {
        if (pkt.counter > lastCounter[id] + 1) {
          Serial.print("Node ");
          Serial.print(id);
          Serial.print(" missed ");
          Serial.print(pkt.counter - lastCounter[id] - 1);
          Serial.println(" packets");
        }
      }
      hasLast[id] = true;
      lastCounter[id] = pkt.counter;

      handlePacket(pkt, pipeNum);
    }
  }
}