# -*- coding: utf-8 -*-
"""
Created on Tue Apr  4 22:47:41 2023

@author: Malichi Flemming II
"""

# Importing Libraries
import serial
import time
import csv
import serial.tools.list_ports; 
from datetime import datetime
import re
from pathlib import Path

datestring = datetime.now().strftime("%Y%m%d%H%M%S")  #Get date to the desired string format
arduino = serial.Serial(port="COM17", baudrate=115200, timeout=.1 )
# print([comport.device for comport in serial.tools.list_ports.comports()])
    
def read_data(x):
    time.sleep(0.05)
    data = arduino.readline().decode().replace("\r\n", "")
    return data

def store_data(node, value):
    if (node >= 1 or node <= 9): 
        chunks = value.split(',')
        print(chunks) # printing the value
        with open('SensorData/' + datestring + f'/node{node}.csv', 'a', newline='') as file:
            writer = csv.writer(file)
            writer.writerows([chunks])    

def data_collect():
    Path('SensorData/' + datestring).mkdir(parents=True, exist_ok=True)   # Create folder    
    if (arduino.in_waiting):
        value = read_data("")
        value = value + ', ' + datetime.now().strftime("%d%H%M%S")
        node = re.search(r'\d+', value)
        store_data(int(node.group()), value)   

if __name__ == "__main__":
    while True:
        data_collect()