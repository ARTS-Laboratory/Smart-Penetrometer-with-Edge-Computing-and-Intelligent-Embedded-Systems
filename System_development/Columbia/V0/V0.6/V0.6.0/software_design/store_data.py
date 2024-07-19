# -*- coding: utf-8 -*-
"""
Created on Tue Apr  4 22:47:41 2023

@author: malichi
"""

# Importing Libraries
import serial
import time
import csv
import serial.tools.list_ports; 
from datetime import datetime
import re
from pathlib import Path

today = datetime.now()   # Get date
# print([comport.device for comport in serial.tools.list_ports.comports()])
arduino = serial.Serial(port='COM19', baudrate=115200, timeout=.1)
def write_read(x):
    # arduino.write(bytes(x, 'utf-8'))
    time.sleep(0.05)
    data = arduino.readline().decode().replace("\r\n", "")
    return data

while True:
    datestring = today.strftime("%Y%m%d%H%M%S")  # Date to the desired string format
    Path('SensorData/' + datestring).mkdir(parents=True, exist_ok=True)   # Create folder
    if (arduino.in_waiting):
        value = write_read("")
        value = value + ', ' + datetime.now().strftime("%d%H%M%S")
        node = re.search(r'\d+', value)
        
        chunks = value.split(',')
        print(chunks) # printing the value
        with open('SensorData/' + datestring + f'/node{node.group()}.csv', 'a', newline='') as file:
            writer = csv.writer(file)
            writer.writerows([chunks])
