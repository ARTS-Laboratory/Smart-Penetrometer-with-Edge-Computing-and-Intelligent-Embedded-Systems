#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <SD.h>

RF24 radio(9, 10); // CE, CSN
File SD_Card; // SD Card

const int bufferSize = 256; 


const byte address[6] = "00001";
const uint8_t payloadSize = 128; 



//void setup() {
//  Serial.begin(9600);
//  radio.begin();
//  radio.openReadingPipe(0, address);
//  radio.setPALevel(RF24_PA_MIN);
//  radio.setChannel(0);
//  radio.setPayloadSize(payloadSize);
//  radio.startListening();
//}
//
//void loop() {
//  if (radio.available()) {
//    char text[payloadSize] = "";
//    radio.read(&text, sizeof(text));
//    Serial.println(text);
//  }
//}


const int lineLength = 76;   // Length of each line in characters


void setup() {
  Serial.begin(9600);
  radio.begin();

  sd_card_setup();
  read_data_setup();
//  String tmp = read_lines_between("data.csv", 1, 10);
//  Serial.print(tmp);

}

void loop() {

}

void read_data_setup(){
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(0);
  radio.setPayloadSize(payloadSize);
  radio.startListening();
}



void sd_card_setup(){
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("Initialization FAILED!");
    while (1);
  }
  Serial.println("Initialization SUCCESS!");
}

String read_line(const char* filename, long lineNumber) {
  File file = SD.open(filename, FILE_READ);

  if (!file) {
    Serial.println("Error opening file!");
    return "";
  }

  // Calculate the seek position for the desired line
  long seekPosition = (lineNumber - 1) * (lineLength + 1); // Add 1 for '\n' character
  file.seek(seekPosition);
  Serial.println(seekPosition);
  Serial.println(file.size());
  String line = file.readStringUntil('\n');
  file.close();

  return line;
}

String read_lines_between(const char* filename, long startLine, long endLine) {
  File file = SD.open(filename, FILE_READ);

  String str_buff = "";
  
  if (!file) {
    Serial.println("Error opening file!");
    return;
  }

  // Calculate the seek positions for the start and end lines
  long startSeekPosition = (startLine - 1) * (lineLength + 1); // Add 1 for '\n' character
  long endSeekPosition = endLine * (lineLength + 1);           // Add 1 for '\n' character

  // Seek to the start position
  file.seek(startSeekPosition);

  // Print lines between startLine and endLine
  while (file.position() < endSeekPosition) {
    String line = file.readStringUntil('\n');
    str_buff += line;
    str_buff += '\n';
    
//    Serial.println(line);
  }

  file.close();
  return str_buff;
}
