from pyrf24 import *
import time

import struct

#pipes = [ 0x52, 0x78, 0x41, 0x41, 0x41 ] 
#pipesbytes = bytearray(pipes)

pipesbytes = 0x7878787878

radio = RF24()
radio.begin(22, 0) #Set CE and IRQ pins
radio.setPALevel(RF24_PA_HIGH)
radio.setDataRate(RF24_250KBPS)
radio.setChannel(0x4c)
radio.openReadingPipe(1, pipesbytes)
radio.startListening()
radio.printDetails()

#radio.powerUp()
cont=0

prev_val = -1

while True:
  while radio.available():

    pipe = [1]    

    recv_buffer = bytearray([])
    recv_buffer = radio.read()
    #  print(recv_buffer)

    int_val = int.from_bytes(recv_buffer, "little")

    if prev_val != int_val-1:
      print('*********************DATALOSS**************')
    # printing int equivalent
    print(int_val)
    prev_val = int_val
