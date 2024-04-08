#include <RF24.h>
#include <RF24Network.h>
#include <SPI.h>
#include <SD.h>

RF24 radio(9, 10); // nRF24L01 (CE,CSN)
RF24Network network(radio); // Include the radio in the network
const uint16_t this_node = 04; // Address of this node in Octal format -- change this for each node 01, 02, ...etc.
const uint16_t node00 = 00;
unsigned long iteration = 0;

// Define the file name to store data on the SD card
const char* filename = "sensor.csv";

struct SensorData {
  unsigned long time; // 4 bytes
  float temperature;  // 4 bytes
  float humidity;     // 4 bytes
  float conductivity; // 4 bytes
  unsigned short battery_level; // 2 bytes
  unsigned long packetNumber;   // 4 bytes
};

SensorData data;

File dataFile; // Declare the File object as a global variable

// Declare pin for soil moisture sensor
const int moistureSensorPin = A3;


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

  // Initialize the SD card
  if (!SD.begin(4)) {
    Serial.println(F("SD card initialization failed!"));
    while (1) {
      // hold in infinite loop
    }
  }

  // Open the file for data writing and keep it open during the program's execution
  dataFile = SD.open(filename, FILE_WRITE);
  if (!dataFile) {
    Serial.println(F("Error opening file for writing!"));
    while (1) {
      // hold in infinite loop
    }
  }

  // Check if the file is empty. If so, write the header.
  if (dataFile.size() == 0) {
    writeHeaderToFile();
  }
}


void loop() {
  network.update();
  RF24NetworkHeader header(node00);

  Serial.print(F("Sending... "));

  data.time = millis();
  data.temperature = 0.0;
  data.humidity = 0.0;
  data.conductivity = readMoistureSensorVoltage();
  data.battery_level = 0.0;
  data.packetNumber = iteration;

  Serial.println();
  printSensorData(data);

  // Write the data to the SD card
  writeDataToFile(data);

  bool ok = network.write(header, &data, sizeof(data)); // Send the data
  Serial.println(ok ? F("ok.") : F("failed."));

  iteration++;
  delay(1000);
}


float readMoistureSensorVoltage() {
  // uses 3300 and 1000 for 3 decimal pt percision...
  //  use 33000 and 10000 for 4 pt, and so on
  float sensorDigi = analogRead(moistureSensorPin);
  sensorDigi = map(sensorDigi, 0, 1023, 0, 3300);
  sensorDigi = sensorDigi / 1000;
  
  return sensorDigi;
}


void writeHeaderToFile() {
  dataFile.println("Time, Temperature, Humidity, Conductivity, Battery Level, Packet Number");
  dataFile.flush(); // Ensure data is written immediately to the card
}

void writeDataToFile(const SensorData& data) {
  if (dataFile) {
    // Write sensor data to the file
    dataFile.print(data.time);
    dataFile.print(", ");
    dataFile.print(data.temperature);
    dataFile.print(", ");
    dataFile.print(data.humidity);
    dataFile.print(", ");
    dataFile.print(data.conductivity);
    dataFile.print(", ");
    dataFile.print(data.battery_level);
    dataFile.print(", ");
    dataFile.println(data.packetNumber);
    dataFile.flush(); // Ensure data is written immediately to the card
  } else {
    Serial.println(F("Error opening file for writing!"));
  }
}

void printSensorData(const SensorData& data) {
  Serial.print(F("Time: "));
  Serial.println(data.time);
  Serial.print(F("Temperature: "));
  Serial.println(data.temperature, 2); // Display float with 2 decimal places
  Serial.print(F("Humidity: "));
  Serial.println(data.humidity, 2); // Display float with 2 decimal places
  Serial.print(F("Conductivity: "));
  Serial.println(data.conductivity, 2); // Display float with 2 decimal places
  Serial.print(F("Battery Level: "));
  Serial.println(data.battery_level);
  Serial.print(F("Packet Number: "));
  Serial.println(data.packetNumber);
}