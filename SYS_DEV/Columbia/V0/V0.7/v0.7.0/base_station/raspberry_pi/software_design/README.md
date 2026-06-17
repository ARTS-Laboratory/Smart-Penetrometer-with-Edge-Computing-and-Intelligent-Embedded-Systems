# Raspberry Pi Data Logger Setup  
This guide explains how to set up a Raspberry Pi to:

- Read sensor data from an Arduino over USB  
- Store the data in daily CSV files  
- Upload the CSV files to Dropbox  
- Run the logger automatically as a systemd service  

This setup is designed for long‑term, unattended operation.

---

## 1. Update the Raspberry Pi
```bash
sudo apt update
sudo apt upgrade -y
```

## 2. Install Required Packages
```bash
sudo apt install -y python3 python3-pip git curl
pip3 install pyserial
```

## 3. Create Folder Structure
```bash
mkdir -p /home/species/notes/notes
sudo chown -R species:species /home/species/notes
```

## 4. Create a Dropbox App (Required for Dropbox-Uploader)
Dropbox-Uploader cannot work until you create a Dropbox App and generate an access token.

Steps:
Go to:
https://www.dropbox.com/developers/apps

Click Create App

Choose:

Scoped Access

App Folder (recommended)

Name your app (e.g., SpeciesDataLogger)

After creation, go to the Permissions tab:

Enable:

files.metadata.read

files.content.read

files.content.write

Go to the Settings tab:

Scroll to OAuth 2

Click Generate Access Token

Copy the token — you will need it during Dropbox-Uploader setup.

## 5. Install Dropbox-Uploader (as the correct user)
```bash
cd /home/species
git clone https://github.com/andreafabrizi/Dropbox-Uploader.git
cd Dropbox-Uploader
chmod +x dropbox_uploader.sh
Run the setup:
```
```bash
./dropbox_uploader.sh
```
When prompted:

Paste the access token you generated earlier

Confirm the setup

This creates the config file:

/home/species/.dropbox_uploader

## 6. Place Basestaion.py Script
Save script here:

/home/species/basestaion.py
Make it executable:

```bash
chmod +x /home/species/basestation.py
```

## 7. Create the systemd Service
Create the service file:

```bash
sudo nano /etc/systemd/system/basestation.service
```
Paste basestation.service file
Save and exit.

## 8. Enable and Start the Service
```bash
sudo systemctl daemon-reload
sudo systemctl enable basestation.service
sudo systemctl start basestation.service
```

## 9. View Logs
```bash
journalctl -u basestation.service -f
```