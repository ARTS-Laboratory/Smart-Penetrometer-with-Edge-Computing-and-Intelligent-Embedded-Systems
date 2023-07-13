// Node02
#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <limits.h>

RF24 radio(9, 10); // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network
const uint16_t this_node = 02; // Address of this node in Octal format
const uint16_t node00 = 00;
unsigned long iteration = 0;

struct SensorData {
  unsigned long time; // 4 bytes
  float temperature;  // 4 bytes
  float humidity;     // 4 bytes
  float conductivity; // 4 bytes
  unsigned short battery_level; //2 bytes
  unsigned long packetNumber;   // 4 bytes
};

SensorData data;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  
  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(this_node); //(channel, node address)
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);

}


void loop() {
  network.update();
  RF24NetworkHeader header(node00);

  Serial.print(F("Sending... "));

  data.time = millis();
  data.temperature = random(0, 100) / 10.0;     // Random float value between 0 and 10
  data.humidity = random(0, 100) / 10.0;        // Random float value between 0 and 10
  data.conductivity = random(0, 100) / 10.0;    // Random float value between 0 and 10
  data.battery_level = random(501);    // Random float value between 0 and 500
  data.packetNumber = iteration;
//  printSensorData(data);
  
  bool ok = network.write(header, &data, sizeof(data)); // Send the data
  Serial.println(ok ? F("ok.") : F("failed."));
  
  iteration++;
  delay(1000);
}

void printSensorData(const SensorData& data) {
  Serial.print("Time: ");
  Serial.println(data.time);
  Serial.print("Temperature: ");
  Serial.println(data.temperature);
  Serial.print("Humidity: ");
  Serial.println(data.humidity);
  Serial.print("Conductivity: ");
  Serial.println(data.conductivity);
  Serial.print("Battery Level: ");
  Serial.println(data.battery_level);
  Serial.print("Packet Number: ");
  Serial.println(data.packetNumber);
}