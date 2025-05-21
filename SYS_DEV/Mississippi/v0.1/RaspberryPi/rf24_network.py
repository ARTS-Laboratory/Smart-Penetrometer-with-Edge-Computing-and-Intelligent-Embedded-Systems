#!/usr/bin/env python
"""
Simplest possible example of using RF24Network

RECEIVER NODE
Listens for messages from the transmitter and prints them out.
"""
import struct
import time
import csv
from pyrf24 import RF24, RF24Network, RF24NetworkHeader, RF24_2MBPS, RF24_PA_HIGH

radio = RF24(22, 0)
network = RF24Network(radio)

# Address of our node in Octal format (01,021, etc)
THIS_NODE = 0o0

# Address of the other node
OTHER_NODES = [0o1, 0o2, 0o3, 0o4, 0o5]

# milliseconds - How long to wait before sending the next message
INTERVAL = 1000

# milliseconds - How long to wait before skipping connection
SKIP_INTERVAL = 5000

# initialize the nRF24L01 on the spi bus
if not radio.begin():
    raise OSError("nRF24L01 hardware isn't responding")

radio.channel = 90
network.begin(THIS_NODE)
radio.setDataRate(RF24_2MBPS)
radio.setPALevel(RF24_PA_HIGH)
radio.print_pretty_details()


COMMAND = 1
LAST_SENT = time.monotonic_ns() / 1000000

EXPECTED_SIZE = struct.calcsize("<LfffHL")
 
column_labels = ['Payload Length', 'From', 'To', 'ID', 'Time', 'Temperature', 'Humidity', 'Conductivity', 'Battery Level', 'Packet Number']


try:
	while True:
		for NODE in OTHER_NODES:
			#time.sleep(0.5)
			await_flag = True
			START_TIME = int(time.monotonic_ns() / 1000000)

			while (await_flag):
				network.update()
				# Receiving the payload from the node
				while network.available():
					#sitt += 1
					header, payload = network.read()
				    
					if (header.from_node == NODE):
						print("================================================")
						print("payload length:", len(payload))

						from_node, to_node, id = header.from_node, header.to_node, header.id
						print('From:', from_node)
						print('To:', to_node)
						print("Id:", id)


						recv_time, temperature, humidity, conductivity, battery_level, packetNumber = struct.unpack("<LfffHL", payload[:EXPECTED_SIZE])
						print("Time:", recv_time)
						print("Temperature:", temperature)
						print("Humidity:", humidity)
						print("Conductivity:", conductivity)
						print("Battery Level:", battery_level)
						print("Packet Number:", packetNumber)
						
						filename = f"data/data_{NODE}.csv"
						current_time = time.strftime("%Y-%m-%d %H:%M:%S")
						print("Current Time:", current_time)

						# Open the file in 'append' mode and create a CSV writer
						with open(filename, 'a', newline='') as file:
						    writer = csv.writer(file)
						    # writer.writerow(column_labels)

						    # Write the data to the CSV file
                            writer.writerow([len(payload), from_node, to_node, id, recv_time, temperature, humidity, conductivity, battery_level, packetNumber,current_time])
						
                        await_flag = False
                    else:
                        print(f"AWAITING DATA FROM {NODE}...")
						
						now = int(time.monotonic_ns() / 1000000)
						if (now - START_TIME >= SKIP_INTERVAL):
							print(f"Node {NODE} is unreachable...")
							await_flag = False
	


except KeyboardInterrupt:
    print("powering down radio and exiting.")
    radio.power = False