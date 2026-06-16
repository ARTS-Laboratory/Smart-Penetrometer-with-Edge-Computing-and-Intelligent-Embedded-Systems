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

# -------------------- READ LINE ------------------------
def read_data(arduino):
    time.sleep(0.02)
    try:
        return arduino.readline().decode(errors="ignore").strip()
    except:
        return ""

# -------------------- PARSE RF24 RECEIVER LINE ---------
rf24_regex = re.compile(
    r"Node\s+(\d+)\s+cnt=(\d+)\s+t\(ms\)=(\d+)\s+RTD=([-0-9.]+)\s+T=([-0-9.]+)\s+RH=([-0-9.]+)\s+P=([-0-9.]+)\s+C=([-0-9.]+)\s+Vbat=([-0-9.]+)"
)

def parse_rf24_line(line):
    m = rf24_regex.search(line)
    if not m:
        return None

    return {
        "node": int(m.group(1)),
        "counter": int(m.group(2)),
        "time_ms": int(m.group(3)),
        "rtdTemp": float(m.group(4)),
        "bmeTemp": float(m.group(5)),
        "humidity": float(m.group(6)),
        "pressure": float(m.group(7)),
        "conductivity": float(m.group(8)),
        "battery": float(m.group(9)),
    }

# -------------------- STORE DATA -----------------------
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

        print(row)

        # Local CSV path
        outpath = f"/home/artslab/notes/notes/{datestring}/node{node}.csv"

        # Write row to CSV
        with open(outpath, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(row)

        # -----------------------------
        # Dropbox-Uploader upload
        # -----------------------------
        # Remote Dropbox path (inside App Folder)
        remote_path = f"bs03/{datestring}/node{node}.csv"

        dropbox_cmd = [
            "/Dropbox-Uploader/dropbox_uploader.sh",
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
    if arduino.in_waiting:
        line = read_data(arduino)
        if not line:
            return

        print("RAW:", line)

        parsed = parse_rf24_line(line)
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

    while True:
        try:
            datestring = datetime.now().strftime("%Y%m%d")
            Path(f"/home/artslab/notes/notes/{datestring}").mkdir(parents=True, exist_ok=True)
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
