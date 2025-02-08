import serial
import time
import csv
import serial.tools.list_ports
from datetime import datetime
import re
from pathlib import Path
import subprocess as sp
import sys

count = 0
# Function to find Arduino Nano from available ports
def find_arduino_nano():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if port:
            return port.device
    return None

# Function to read data from the Arduino
def read_data(arduino):
    time.sleep(0.05)
    data = arduino.readline().decode().replace("\r\n", "")
    return data

# Function to store data in a CSV file
def store_data(node, value, datestring):
    global count
    if 1 <= node <= 9:
        chunks = value.split(',')
        print(chunks)  # printing the value
        with open(f'/home/species/notes/notes/{datestring}/node{node}.csv', 'a', newline='') as file:
            writer = csv.writer(file)
            writer.writerows([chunks])
        command = ["rclone", "copy", "/home/species/notes", "Species:"]
        command.extend(["--config", "/home/species/.config/rclone/rclone.conf"])
        process = sp.Popen(command, stdout=sp.PIPE, stderr=sp.PIPE)
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            print("Upload successful")
            return True
        else:
            print(f"Upload failed with error: {stderr.decode()}")
            return False

    else:
        if count < 6:
            print("no packet or packet corrupt")
            count += 1
        else:
            sys.exit()

# Function to collect data from the Arduino
def data_collect(arduino, datestring):
#    time.sleep(2)
    if arduino.in_waiting:
        value = read_data(arduino)
        value = value + ', ' + datetime.now().strftime("%d%H%M%S")
        node = re.search(r'\d+', value)
        print(value)
        store_data(int(node.group()), value, datestring)

def main():
    arduino_port = find_arduino_nano()
    if arduino_port is None:
        print("No Arduino Nano found. Searching again...")
        time.sleep(2)  # Wait for 2 seconds before retrying
        return

    arduino = serial.Serial(port=arduino_port, baudrate=115200, timeout=0.1)
#    print(f"Connected to Arduino Nano on port {arduino_port}")

    while True:
        try:
            datestring = datetime.now().strftime("%Y%m%d")
            Path(f'/home/species/notes/notes/{datestring}').mkdir(parents=True, exist_ok=True)  # Create folder
            data_collect(arduino, datestring)
        except serial.SerialException:
            print("Connection to Arduino Nano lost. Searching again...")
            arduino.close()
            return False
        except OSError:
            print("Connection to Arduino Nano lost. Searching again...")
            arduino.close()
            return False
        except serial.serialutil.SerialException:
            print("Connection to Arduino Nano lost. Searching again...")
            arduino.close()
            return False
        except TypeError as  te:
            print("Connection to Arduino Nano lost. Searching again...")
            arduino.close()
            return False

if __name__ == "__main__":
    while True:
        main()
