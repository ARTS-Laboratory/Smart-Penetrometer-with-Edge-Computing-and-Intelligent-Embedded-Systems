from pyrf24 import *
import time

import struct

pipes = [0x7878787878, 0xB3B4B5B6F1, 0xB3B4B5B6CD, 0xB3B4B5B6A3, 0xB3B4B5B60F, 0xB3B4B5B605]
daNumber = 0

radio = RF24()
radio.begin(22, 0) #Set CE and IRQ pins
radio.setPALevel(RF24_PA_HIGH)
radio.setDataRate(RF24_1MBPS)
radio.setChannel(0x4c)

radio.openReadingPipe(0, pipes[0])
radio.openReadingPipe(1, pipes[1])
radio.openReadingPipe(2, pipes[2])
radio.openReadingPipe(3, pipes[3])
radio.openReadingPipe(4, pipes[4])
radio.openReadingPipe(5, pipes[5])

radio.startListening()
radio.printDetails()


while True:

    pipeNum = bytes(0)
    gotoByte = bytes(0)
    
    while radio.available():
        gotoByte = radio.read()
        
#        print(f"Received guess from transmitter: {pipeNum + 1}")
        print(f"{int.from_bytes(gotoByte, 'little')}")
        
#        if daNumber != int.from_bytes(daNumber, "little"):
 #           print("Try Again")
  #      else:
   #         print("Correct")
