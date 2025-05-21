# V 0.6.2
Sensor node
1. A sensor node that collects data, saves it to local memory, and sends it wirelessly to gateway station.
1. Utilizes solar recharging.
1. 4-layer PCB
2. Incorporates geophone
3. Six solar cells
4. Improve waterproof seals
5. Replaced ADS1118 with ADS1115 due to spi mode incompatibility.
6. RTD, EC, and Geophone sensors all read through ADS1118.
7. Collected data stored on raspberry pi and in a dropbox client.
8. Service script initiates data collection on boot.
9. A new folder is generated each day, and each node's data is stored in a seperate file.

<p align="center">
<img src="https://github.com/user-attachments/assets/f60e092a-56cc-4d42-abb4-71a7e73cc723" alt="image" width="300"/>
</p>

## [Return to V0.6](https://github.com/ARTS-Laboratory/Smart-Penetrometer-with-Edge-Computing-and-Intelligent-Embedded-Systems/blob/main/V0/V0.6)
