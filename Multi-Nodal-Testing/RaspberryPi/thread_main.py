import threading
from pyrf24 import *
import time

pipes = [0x7878787878, 0xB3B4B5B6F1, 0xB3B4B5B6CD, 0xB3B4B5B6A3, 0xB3B4B5B60F, 0xB3B4B5B605]
node_addresses = [0xF0F0F0F0E1, 0xF0F0F0F0D2, 0xF0F0F0F0C3, 0xF0F0F0F0B4, 0xF0F0F0F0A5, 0xF0F0F0F096]
file_locks = [threading.Lock() for _ in range(len(pipes))]  # Create locks for each file

def receive_data(radio, pipe_num):
    radio.openWritingPipe(pipes[pipe_num])
    radio.openReadingPipe(1, pipes[pipe_num])  # Use pipe_num + 1 for pipe number

    radio.startListening()

    while True:
        if radio.available():
            gotoByte = radio.read(pipe_num)
            print(f"Received data from pipe {pipe_num}: {int.from_bytes(gotoByte, 'little')}")

            # Save the received data to the corresponding file
            with file_locks[pipe_num]:
                with open(f"pipe{pipe_num}_data.txt", "a") as file:
                    file.write(f"{int.from_bytes(gotoByte, 'little')}\n")

# Create an RF24 instance
radio = RF24()
radio.begin(22, 0)  # Set CE and IRQ pins
radio.setPALevel(RF24_PA_HIGH)
radio.setDataRate(RF24_250KBPS)
radio.setChannel(0x4c)

# Create a thread for each pipe
threads = []
for i in range(len(pipes)):
    thread = threading.Thread(target=receive_data, args=(radio, i))
    threads.append(thread)
    thread.start()

# Main thread waits indefinitely
while True:
    pass
