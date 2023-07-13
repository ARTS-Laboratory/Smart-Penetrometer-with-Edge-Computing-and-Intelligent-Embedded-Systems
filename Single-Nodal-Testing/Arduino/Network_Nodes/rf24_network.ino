#include <SPI.h>
#include <RF24.h>
#include <RF24Network.h>

RF24 radio(9, 10);  // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);  // Network uses that radio

const uint16_t this_node = 01;   // Address of our node in Octal format
const uint16_t other_node = 00;  // Address of the other node in Octal format

const unsigned long interval = 1000;  // How often (in ms) to send 'hello world' to the other unit

unsigned long last_sent;     // When did we last send?
unsigned long packets_sent;  // How many have we sent already

unsigned long iteration = 0;


struct payload_t {  // Structure of our payload
  unsigned long time; // 4 bytes
  float temperature;  // 4 bytes
  float humidity;     // 4 bytes
  float conductivity; // 4 bytes
  unsigned long packetNumber;   // 4 bytes
};

void setup(void) {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need this because of native USB capability
  }
  Serial.println(F("RF24Network/examples/helloworld_tx/"));

  if (!radio.begin()) {
    Serial.println(F("Radio hardware not responding!"));
    while (1) {
      // hold in infinite loop
    }
  }
  radio.setChannel(90);
  network.begin(/*node address*/ this_node);
}

void loop() {

  network.update();  // Check the network regularly

  unsigned long now = millis();

  // If it's time to send a message, send it!
  if (now - last_sent >= interval) {
    last_sent = now;

    Serial.print(F("Sending... "));
    
    RF24NetworkHeader header(/*to node*/ other_node);
    payload_t payload;

    payload.time = millis();
    payload.temperature = random(0, 100) / 10.0;
    payload.humidity = random(0, 100) / 10.0;
    payload.conductivity = random(0, 100) / 10.0; 
    payload.packetNumber = iteration;
    
    
    bool ok = network.write(header, &payload, sizeof(payload));
    Serial.println(ok ? F("ok.") : F("failed."));
    iteration++;
  }
}