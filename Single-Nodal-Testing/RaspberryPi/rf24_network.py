#!/usr/bin/env python
"""
Simplest possible example of using RF24Network

RECEIVER NODE
Listens for messages from the transmitter and prints them out.
"""
import struct
from pyrf24 import RF24, RF24Network


radio = RF24(22, 0)
network = RF24Network(radio)

# Address of our node in Octal format (01, 021, etc)
THIS_NODE = 0o0  # make this node behave like the network master node

# initialize the nRF24L01 on the spi bus
if not radio.begin():
    raise OSError("nRF24L01 hardware isn't responding")

radio.channel = 90
network.begin(THIS_NODE)
radio.print_pretty_details()

EXPECTED_SIZE = struct.calcsize("<LfffL")

try:
    while True:
        network.update()
        while network.available():
            header, payload = network.read()
            print("payload length ", len(payload))
            time, temperature, humidity, conductivity, packetNumber = struct.unpack("<LfffL", payload[:EXPECTED_SIZE])
            print("Time:", time)
            print("Temperature:", temperature)
            print("Humidity:", humidity)
            print("Conductivity:", conductivity)
            print("Packet Number:", packetNumber)

            print("================================================")

except KeyboardInterrupt:
    print("powering down radio and exiting.")
    radio.power = False