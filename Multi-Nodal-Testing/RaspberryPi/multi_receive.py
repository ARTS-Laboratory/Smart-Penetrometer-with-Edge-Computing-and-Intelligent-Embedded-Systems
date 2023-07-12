from pyrf24 import *
import time

pipes = [0x7878787878, 0xB3B4B5B6F1, 0xB3B4B5B6CD, 0xB3B4B5B6A3, 0xB3B4B5B60F, 0xB3B4B5B605]
write_pipe = 0xF0F0F0F0D2

file_handles = []
gotobyte = bytearray([])

def initialize_files():
    for i in range(len(pipes)):
        file_handle = open(f"data/pipe{i}_data.txt", "a")
        file_handles.append(file_handle)
        

def main():
    initialize_files()
    
    radio = RF24()
    radio.begin(22, 0)  # Set CE and IRQ pins
    radio.setPALevel(RF24_PA_HIGH)
    radio.setDataRate(RF24_2MBPS)
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
        while radio.available():
            pipe_num = radio.available_pipe()
            pipe_num_update = [list(pipe_num)[0], list(pipe_num)[1]+1]
            gotoByte = radio.read()
            print(f"Received data from pipe {pipe_num_update}: {int.from_bytes(gotoByte, 'little')}")



if __name__ == "__main__":
    main()
