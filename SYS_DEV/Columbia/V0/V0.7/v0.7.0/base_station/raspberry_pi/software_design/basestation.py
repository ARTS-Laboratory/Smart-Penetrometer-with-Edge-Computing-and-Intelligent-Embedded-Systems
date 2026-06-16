"""
Created on Sat Feb 8 01:15:07 2025

@author: Malichi Flemming II
"""

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

# -------------------- FIND ARDUINO --------------------
def find_arduino_nano():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if port:
            return port.device
    return None

# -------------------- BULLETPROOF SERIAL READER --------
def read_serial_line(arduino):
    """
    Accumulates bytes until a full newline-terminated line is received.
    Never returns partial lines.
    """
    buffer = getattr(read_serial_line, "buffer", "")

    try:
        data = arduino.read(arduino.in_waiting or 1).decode("utf-8", errors="ignore")
    except:
        return None

    buffer += data

    if "\n" in buffer:
        line, buffer = buffer.split("\n", 1)
        read_serial_line.buffer = buffer
        return line.strip()

    read_serial_line.buffer = buffer
    return None

# -------------------- UNIVERSAL VALUE EXTRACTOR --------
def extract_values(line):
    # Extract all numbers (ints or floats)
    nums = re.findall(r"-?\d+(?:\.\d+)?", line)

    # Expecting at least 9 values
    if len(nums) < 9:
        return None

    return {
        "node": int(nums[0]),
        "counter": int(nums[1]),
        "time_ms": int(nums[2]),
        "rtdTemp": float(nums[3]),
        "bmeTemp": float(nums[4]),
        "humidity": float(nums[5]),
        "pressure": float(nums[6]),
        "conductivity": float(nums[7]),
        "battery": float(nums[8]),
    }

# -------------------- STORE DATA + DROPBOX UPLOAD ------
def store_data(parsed, datestring):
    global count
    node = parsed["node"]

    if 1 <= node <= 9:
        row = [
            parsed["node"],
            parsed["counter"],
            parsed["time_ms"],
            parsed["rtdTemp"],
            parsed["bmeTemp"],
            parsed["humidity"],
            parsed["pressure"],
            parsed["conductivity"],
            parsed["battery"],
            datetime.now().strftime("%d%H%M%S")
        ]

        print("ROW:", row)

        outpath = f"/home/species/notes/notes/{datestring}/node{node}.csv"
        with open(outpath, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(row)

        # Dropbox upload
        remote_path = f"bs03/{datestring}/node{node}.csv"
        dropbox_cmd = [
            "/home/species/Dropbox-Uploader/dropbox_uploader.sh",
            "upload",
            outpath,
            remote_path
        ]

        process = sp.Popen(dropbox_cmd, stdout=sp.PIPE, stderr=sp.PIPE)
        stdout, stderr = process.communicate()

        if process.returncode == 0:
            print("Dropbox upload successful")
            return True
        else:
            print(f"Dropbox upload failed: {stderr.decode()}")
            return False

    else:
        if count < 6:
            print("Invalid node or corrupt packet")
            count += 1
        else:
            sys.exit()

# -------------------- COLLECT DATA ---------------------
def data_collect(arduino, datestring):
    line = read_serial_line(arduino)
    if not line:
        return

    print("RAW:", repr(line))

    parsed = extract_values(line)
    if parsed:
        store_data(parsed, datestring)
    else:
        print("Ignored non‑data line")

# -------------------- MAIN LOOP ------------------------
def main():
    arduino_port = find_arduino_nano()
    if arduino_port is None:
        print("No Arduino Nano found. Retrying...")
        time.sleep(2)
        return

    arduino = serial.Serial(port=arduino_port, baudrate=115200, timeout=0.1)

    # Allow Arduino reboot time
    time.sleep(1)
    arduino.reset_input_buffer()

    while True:
        try:
            datestring = datetime.now().strftime("%Y%m%d")
            Path(f"/home/species/notes/notes/{datestring}").mkdir(parents=True, exist_ok=True)
            data_collect(arduino, datestring)

        except (serial.SerialException, serial.serialutil.SerialException, OSError, TypeError):
            print("Connection lost. Searching again...")
            try:
                arduino.close()
            except:
                pass
            return False

if __name__ == "__main__":
    while True:
        main()
