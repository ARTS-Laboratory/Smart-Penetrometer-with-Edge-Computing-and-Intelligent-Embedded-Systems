EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	4950 2900 4800 2900
Wire Wire Line
	4950 2800 4750 2800
Wire Wire Line
	4950 2700 4700 2700
Wire Wire Line
	4950 2600 4650 2600
Wire Wire Line
	5350 2350 6450 2350
Wire Wire Line
	6450 2350 6450 3100
Wire Wire Line
	5850 4650 5950 4650
Wire Wire Line
	5850 4550 5900 4550
Wire Wire Line
	4700 2700 4700 4650
Wire Wire Line
	4750 4850 4850 4850
Wire Wire Line
	4750 2800 4750 4850
Wire Wire Line
	4700 4650 4850 4650
Wire Wire Line
	4800 4750 4850 4750
Wire Wire Line
	4800 2900 4800 4750
Wire Wire Line
	4650 4550 4850 4550
Wire Wire Line
	4650 2600 4650 3350
Wire Wire Line
	5450 3150 5450 3100
Wire Wire Line
	5450 3100 6050 3100
$Comp
L power:GND #PWR04
U 1 1 60A6136F
P 5450 5150
F 0 "#PWR04" H 5450 4900 50  0001 C CNN
F 1 "GND" H 5500 5000 50  0000 C CNN
F 2 "" H 5450 5150 50  0001 C CNN
F 3 "" H 5450 5150 50  0001 C CNN
	1    5450 5150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 60A617CA
P 5350 5150
F 0 "#PWR03" H 5350 4900 50  0001 C CNN
F 1 "GND" H 5350 5000 50  0000 C CNN
F 2 "" H 5350 5150 50  0001 C CNN
F 3 "" H 5350 5150 50  0001 C CNN
	1    5350 5150
	1    0    0    -1  
$EndComp
NoConn ~ 5850 4750
NoConn ~ 5850 4850
NoConn ~ 5850 4450
NoConn ~ 5850 3950
NoConn ~ 5850 3650
NoConn ~ 5850 3550
NoConn ~ 4850 3550
NoConn ~ 4850 3650
NoConn ~ 4850 3750
NoConn ~ 4850 3850
NoConn ~ 4850 3950
NoConn ~ 4850 4050
NoConn ~ 4850 4150
NoConn ~ 4850 4250
NoConn ~ 4850 4350
NoConn ~ 4850 4450
$Comp
L kitest-rescue:Battery_Input-kitest U3
U 1 1 60A6877D
P 3900 3200
F 0 "U3" H 4158 3234 50  0000 L CNN
F 1 "Battery_Input" H 3700 3400 50  0000 L CNN
F 2 "kitest:Battery_Input" H 3900 3200 50  0001 C CNN
F 3 "" H 3900 3200 50  0001 C CNN
	1    3900 3200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 60A69074
P 3750 3350
F 0 "#PWR06" H 3750 3100 50  0001 C CNN
F 1 "GND" H 3755 3177 50  0000 C CNN
F 2 "" H 3750 3350 50  0001 C CNN
F 3 "" H 3750 3350 50  0001 C CNN
	1    3750 3350
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0101
U 1 1 60A6AB09
P 4100 3350
F 0 "#PWR0101" H 4100 3200 50  0001 C CNN
F 1 "VCC" H 4100 3200 50  0000 C CNN
F 2 "" H 4100 3350 50  0001 C CNN
F 3 "" H 4100 3350 50  0001 C CNN
	1    4100 3350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 3150 4850 3150
Wire Wire Line
	4850 3150 4850 3350
$Comp
L Timer_RTC:DS3231M U2
U 1 1 60DCDB50
P 6750 3950
F 0 "U2" H 7050 3450 50  0000 C CNN
F 1 "DS3231M" H 7050 3550 50  0000 C CNN
F 2 "Package_SO:SOIC-16W_7.5x10.3mm_P1.27mm" H 6750 3350 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS3231.pdf" H 7020 4000 50  0001 C CNN
	1    6750 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 4650 5950 3750
Wire Wire Line
	5950 3750 6150 3750
Wire Wire Line
	5900 4550 5900 3850
Wire Wire Line
	5900 3850 6050 3850
$Comp
L power:GND #PWR0102
U 1 1 60DD225C
P 6750 4350
F 0 "#PWR0102" H 6750 4100 50  0001 C CNN
F 1 "GND" H 6755 4177 50  0000 C CNN
F 2 "" H 6750 4350 50  0001 C CNN
F 3 "" H 6750 4350 50  0001 C CNN
	1    6750 4350
	1    0    0    -1  
$EndComp
NoConn ~ 7250 4050
NoConn ~ 7250 3750
NoConn ~ 6250 4150
Wire Wire Line
	6050 4150 5850 4150
Wire Wire Line
	7400 5400 7400 3100
Wire Wire Line
	7400 3100 6450 3100
Connection ~ 6450 3100
Wire Wire Line
	4550 3350 4650 3350
Connection ~ 4650 3350
Wire Wire Line
	4650 3350 4650 4550
Wire Wire Line
	4650 3350 4850 3350
NoConn ~ 5850 4350
$Comp
L Connector:XLR3_Switched J1
U 1 1 60DFB15F
P 3950 4050
F 0 "J1" H 3950 4415 50  0000 C CNN
F 1 "XLR3_Switched" H 3950 4324 50  0000 C CNN
F 2 "kitest:grounded_switch" H 3950 4150 50  0001 C CNN
F 3 " ~" H 3950 4150 50  0001 C CNN
	1    3950 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 3350 4250 3350
Wire Wire Line
	4250 3350 4250 4050
Connection ~ 4100 3350
Wire Wire Line
	4550 4350 3950 4350
Wire Wire Line
	4550 3350 4550 4350
NoConn ~ 3650 4050
$Comp
L Connector:Conn_01x03_Female J2
U 1 1 60E1E395
P 6400 5100
F 0 "J2" H 6428 5126 50  0000 L CNN
F 1 "Conn_01x03_Female" H 6428 5035 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x03_P2.54mm_Horizontal" H 6400 5100 50  0001 C CNN
F 3 "~" H 6400 5100 50  0001 C CNN
	1    6400 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	6100 5400 6100 5100
Wire Wire Line
	6100 5100 6200 5100
Wire Wire Line
	6100 5400 7400 5400
$Comp
L power:GND #PWR0103
U 1 1 60E27F4F
P 6200 5000
F 0 "#PWR0103" H 6200 4750 50  0001 C CNN
F 1 "GND" H 6205 4827 50  0000 C CNN
F 2 "" H 6200 5000 50  0001 C CNN
F 3 "" H 6200 5000 50  0001 C CNN
	1    6200 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 5200 6200 5200
Wire Wire Line
	6050 4150 6050 5200
$Comp
L 4xxx:4023 U4
U 3 1 615601EA
P 7950 4750
F 0 "U4" H 7950 5075 50  0000 C CNN
F 1 "4023" H 7950 4984 50  0000 C CNN
F 2 "kitest:TDS_module" H 7950 4750 50  0001 C CNN
F 3 "http://www.intersil.com/content/dam/Intersil/documents/cd40/cd4011bms-12bms-23bms.pdf" H 7950 4750 50  0001 C CNN
F 4 "G" H 8050 4750 50  0000 C CNN "4"
F 5 "SCL" H 7900 4850 50  0000 C CNN "2"
F 6 "SDA" H 7900 4750 50  0000 C CNN "1"
F 7 "V" H 7850 4650 50  0000 C CNN "3"
	3    7950 4750
	1    0    0    -1  
$EndComp
Connection ~ 7400 3100
$Comp
L power:GND #PWR09
U 1 1 61566C7E
P 8250 4750
F 0 "#PWR09" H 8250 4500 50  0001 C CNN
F 1 "GND" H 8255 4577 50  0000 C CNN
F 2 "" H 8250 4750 50  0001 C CNN
F 3 "" H 8250 4750 50  0001 C CNN
	1    8250 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 4650 5950 4650
Connection ~ 5950 4650
Wire Wire Line
	7650 4750 5900 4750
Wire Wire Line
	5900 4750 5900 4550
Connection ~ 5900 4550
$Comp
L Amplifier_Operational:LMV324 U1
U 1 1 61571006
P 3750 5100
F 0 "U1" H 3750 5467 50  0000 C CNN
F 1 "LMV324" H 3750 5376 50  0000 C CNN
F 2 "kitest:LMV324IDR" H 3700 5200 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lmv324.pdf" H 3800 5300 50  0001 C CNN
	1    3750 5100
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:LMV324 U1
U 2 1 61574368
P 3750 5600
F 0 "U1" H 3750 5967 50  0000 C CNN
F 1 "LMV324" H 3750 5876 50  0000 C CNN
F 2 "kitest:LMV324IDR" H 3700 5700 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lmv324.pdf" H 3800 5800 50  0001 C CNN
	2    3750 5600
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:LMV324 U1
U 3 1 6157728F
P 3750 6100
F 0 "U1" H 3750 6467 50  0000 C CNN
F 1 "LMV324" H 3750 6376 50  0000 C CNN
F 2 "kitest:LMV324IDR" H 3700 6200 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lmv324.pdf" H 3800 6300 50  0001 C CNN
	3    3750 6100
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:LMV324 U1
U 4 1 6157825D
P 3750 6600
F 0 "U1" H 3750 6967 50  0000 C CNN
F 1 "LMV324" H 3750 6876 50  0000 C CNN
F 2 "kitest:LMV324IDR" H 3700 6700 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lmv324.pdf" H 3800 6800 50  0001 C CNN
	4    3750 6600
	1    0    0    -1  
$EndComp
$Comp
L Amplifier_Operational:LMV324 U1
U 5 1 6157A7E5
P 3700 7200
F 0 "U1" H 3658 7246 50  0000 L CNN
F 1 "LMV324" H 3658 7155 50  0000 L CNN
F 2 "kitest:LMV324IDR" H 3650 7300 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lmv324.pdf" H 3750 7400 50  0001 C CNN
	5    3700 7200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 4250 6000 4250
$Comp
L Device:R_Small_US R3
U 1 1 61580B06
P 4500 5200
F 0 "R3" H 4568 5246 50  0000 L CNN
F 1 "R_Small_US" H 4568 5155 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 4500 5200 50  0001 C CNN
F 3 "~" H 4500 5200 50  0001 C CNN
	1    4500 5200
	1    0    0    -1  
$EndComp
Connection ~ 4500 5100
Wire Wire Line
	4500 5100 4050 5100
$Comp
L Device:R_Small_US R4
U 1 1 6158148E
P 4500 5400
F 0 "R4" H 4568 5446 50  0000 L CNN
F 1 "R_Small_US" H 4568 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 4500 5400 50  0001 C CNN
F 3 "~" H 4500 5400 50  0001 C CNN
	1    4500 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R1
U 1 1 61581EDE
P 2900 5400
F 0 "R1" H 2968 5446 50  0000 L CNN
F 1 "R_Small_US" H 2968 5355 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 2900 5400 50  0001 C CNN
F 3 "~" H 2900 5400 50  0001 C CNN
	1    2900 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small_US R2
U 1 1 61582741
P 2900 5600
F 0 "R2" H 2968 5646 50  0000 L CNN
F 1 "R_Small_US" H 2968 5555 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 2900 5600 50  0001 C CNN
F 3 "~" H 2900 5600 50  0001 C CNN
	1    2900 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 5200 3450 5300
Wire Wire Line
	3450 5300 4500 5300
Connection ~ 4500 5300
Wire Wire Line
	4050 5600 4500 5600
Wire Wire Line
	4500 5600 4500 5500
Wire Wire Line
	3450 5700 3450 5850
Wire Wire Line
	3450 5850 4050 5850
Wire Wire Line
	4050 5850 4050 5600
Connection ~ 4050 5600
Wire Wire Line
	3450 5500 2900 5500
$Comp
L power:GND #PWR08
U 1 1 61589198
P 3600 7500
F 0 "#PWR08" H 3600 7250 50  0001 C CNN
F 1 "GND" V 3605 7372 50  0000 R CNN
F 2 "" H 3600 7500 50  0001 C CNN
F 3 "" H 3600 7500 50  0001 C CNN
	1    3600 7500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 61589B68
P 2900 5700
F 0 "#PWR07" H 2900 5450 50  0001 C CNN
F 1 "GND" H 2905 5527 50  0000 C CNN
F 2 "" H 2900 5700 50  0001 C CNN
F 3 "" H 2900 5700 50  0001 C CNN
	1    2900 5700
	1    0    0    -1  
$EndComp
Connection ~ 2900 5500
$Comp
L Device:C_Small C1
U 1 1 6158E5F2
P 2500 5400
F 0 "C1" H 2592 5446 50  0000 L CNN
F 1 "C_Small" H 2592 5355 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 2500 5400 50  0001 C CNN
F 3 "~" H 2500 5400 50  0001 C CNN
	1    2500 5400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C2
U 1 1 6158F79D
P 2650 5600
F 0 "C2" H 2742 5646 50  0000 L CNN
F 1 "C_Small" H 2742 5555 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 2650 5600 50  0001 C CNN
F 3 "~" H 2650 5600 50  0001 C CNN
	1    2650 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2900 5300 2500 5300
Wire Wire Line
	2900 5500 2650 5500
$Comp
L power:GND #PWR05
U 1 1 61592C20
P 2650 5700
F 0 "#PWR05" H 2650 5450 50  0001 C CNN
F 1 "GND" H 2655 5527 50  0000 C CNN
F 2 "" H 2650 5700 50  0001 C CNN
F 3 "" H 2650 5700 50  0001 C CNN
	1    2650 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 615934B1
P 2500 5500
F 0 "#PWR02" H 2500 5250 50  0001 C CNN
F 1 "GND" H 2505 5327 50  0000 C CNN
F 2 "" H 2500 5500 50  0001 C CNN
F 3 "" H 2500 5500 50  0001 C CNN
	1    2500 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3100 2900 3100
Wire Wire Line
	2900 3100 2900 5300
Connection ~ 5450 3100
Connection ~ 2900 5300
Wire Wire Line
	2900 5300 3200 5300
$Comp
L Connector:Conn_01x02_Female J3
U 1 1 6159743F
P 2550 5100
F 0 "J3" H 2442 4775 50  0000 C CNN
F 1 "Conn_01x02_Female" H 2442 4866 50  0000 C CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x02_P2.54mm_Horizontal" H 2550 5100 50  0001 C CNN
F 3 "~" H 2550 5100 50  0001 C CNN
	1    2550 5100
	-1   0    0    1   
$EndComp
Wire Wire Line
	2750 5000 3450 5000
Wire Wire Line
	2750 5100 3300 5100
Wire Wire Line
	3300 5100 3300 5350
Wire Wire Line
	3300 5350 4350 5350
Wire Wire Line
	4350 5350 4350 5500
Wire Wire Line
	4350 5500 4500 5500
Connection ~ 4500 5500
NoConn ~ 3450 6000
NoConn ~ 3450 6200
NoConn ~ 3450 6500
NoConn ~ 3450 6700
NoConn ~ 4050 6600
Wire Wire Line
	3200 6900 3600 6900
Wire Wire Line
	3200 5300 3200 6900
NoConn ~ 4050 6100
$Comp
L power:GND #PWR01
U 1 1 60A59E22
P 5350 3150
F 0 "#PWR01" H 5350 2900 50  0001 C CNN
F 1 "GND" V 5350 2950 50  0000 C CNN
F 2 "" H 5350 3150 50  0001 C CNN
F 3 "" H 5350 3150 50  0001 C CNN
	1    5350 3150
	1    0    0    -1  
$EndComp
$Comp
L kitest-rescue:Arduino_Nano_v3.x-MCU_Module A2
U 1 1 60A40C30
P 5350 4150
F 0 "A2" H 5350 2800 50  0000 C CNN
F 1 "Arduino_Nano_v3.x" H 5350 2900 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 5500 3200 50  0001 L CNN
F 3 "http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf" H 5350 3150 50  0001 C CNN
	1    5350 4150
	1    0    0    -1  
$EndComp
$Comp
L kitest-rescue:SD_Card_Module-Charleslabs_Parts A1
U 1 1 60A4A816
P 5350 2750
F 0 "A1" H 5680 2796 50  0000 L CNN
F 1 "SD_Card_Module" H 5680 2705 50  0000 L CNN
F 2 "kitest:SD Module" H 6300 2800 50  0001 C CNN
F 3 "" H 5050 3100 50  0001 C CNN
	1    5350 2750
	1    0    0    -1  
$EndComp
$Comp
L Analog_ADC:ADS1115IDGS U5
U 1 1 615B81A7
P 5150 6300
F 0 "U5" H 5150 6981 50  0000 C CNN
F 1 "ADS1115IDGS" H 5150 6890 50  0000 C CNN
F 2 "Package_SO:TSSOP-10_3x3mm_P0.5mm" H 5150 5800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/ads1113.pdf" H 5100 5400 50  0001 C CNN
	1    5150 6300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 5800 5150 5100
Wire Wire Line
	5150 5100 4500 5100
Wire Wire Line
	6000 5750 4750 5750
Wire Wire Line
	4750 5750 4750 6200
Wire Wire Line
	6000 4250 6000 5750
Wire Wire Line
	5550 6300 5950 6300
Wire Wire Line
	5950 6300 5950 4650
Wire Wire Line
	5550 6400 5900 6400
Wire Wire Line
	5900 6400 5900 4750
Connection ~ 5900 4750
$Comp
L power:GNDS #PWR0104
U 1 1 615CA2F2
P 5150 6700
F 0 "#PWR0104" H 5150 6450 50  0001 C CNN
F 1 "GNDS" H 5155 6527 50  0000 C CNN
F 2 "" H 5150 6700 50  0001 C CNN
F 3 "" H 5150 6700 50  0001 C CNN
	1    5150 6700
	1    0    0    -1  
$EndComp
NoConn ~ 5550 6500
NoConn ~ 5550 6100
NoConn ~ 4750 6500
NoConn ~ 4750 6400
NoConn ~ 4750 6300
$Comp
L Device:R_Small_US R5
U 1 1 6162D30F
P 6050 3200
F 0 "R5" H 6118 3246 50  0000 L CNN
F 1 "R_Small_US" H 6118 3155 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 6050 3200 50  0001 C CNN
F 3 "~" H 6050 3200 50  0001 C CNN
	1    6050 3200
	1    0    0    -1  
$EndComp
Connection ~ 6050 3100
Wire Wire Line
	6050 3100 6150 3100
$Comp
L Device:R_Small_US R6
U 1 1 6162E2E0
P 6150 3200
F 0 "R6" H 6218 3246 50  0000 L CNN
F 1 "R_Small_US" H 6218 3155 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric" H 6150 3200 50  0001 C CNN
F 3 "~" H 6150 3200 50  0001 C CNN
	1    6150 3200
	1    0    0    -1  
$EndComp
Connection ~ 6150 3100
Wire Wire Line
	6150 3100 6300 3100
Wire Wire Line
	6050 3300 6050 3850
Connection ~ 6050 3850
Wire Wire Line
	6050 3850 6250 3850
Wire Wire Line
	6150 3300 6150 3750
Connection ~ 6150 3750
Wire Wire Line
	6150 3750 6250 3750
$Comp
L Device:C_Small C3
U 1 1 6163336C
P 6400 3300
F 0 "C3" V 6171 3300 50  0000 C CNN
F 1 "C_Small" V 6262 3300 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 6400 3300 50  0001 C CNN
F 3 "~" H 6400 3300 50  0001 C CNN
	1    6400 3300
	0    1    1    0   
$EndComp
Wire Wire Line
	6300 3300 6300 3100
Connection ~ 6300 3100
Wire Wire Line
	6300 3100 6450 3100
Wire Wire Line
	6300 3300 6300 3550
Wire Wire Line
	6300 3550 6650 3550
Connection ~ 6300 3300
$Comp
L power:GNDS #PWR010
U 1 1 6163B2DA
P 6500 3300
F 0 "#PWR010" H 6500 3050 50  0001 C CNN
F 1 "GNDS" V 6505 3172 50  0000 R CNN
F 2 "" H 6500 3300 50  0001 C CNN
F 3 "" H 6500 3300 50  0001 C CNN
	1    6500 3300
	0    -1   -1   0   
$EndComp
$Comp
L Device:C_Small C4
U 1 1 6163E9E6
P 6850 3350
F 0 "C4" V 6621 3350 50  0000 C CNN
F 1 "C_Small" V 6712 3350 50  0000 C CNN
F 2 "Capacitor_SMD:C_1206_3216Metric" H 6850 3350 50  0001 C CNN
F 3 "~" H 6850 3350 50  0001 C CNN
	1    6850 3350
	0    1    1    0   
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 6163F1F7
P 6950 3200
F 0 "BT1" V 7205 3250 50  0000 C CNN
F 1 "Battery_Cell" V 7114 3250 50  0000 C CNN
F 2 "kitest:battery_holder_10mm" V 6950 3260 50  0001 C CNN
F 3 "~" V 6950 3260 50  0001 C CNN
	1    6950 3200
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDS #PWR011
U 1 1 61640E8C
P 6950 3350
F 0 "#PWR011" H 6950 3100 50  0001 C CNN
F 1 "GNDS" V 6955 3222 50  0000 R CNN
F 2 "" H 6950 3350 50  0001 C CNN
F 3 "" H 6950 3350 50  0001 C CNN
	1    6950 3350
	0    -1   -1   0   
$EndComp
$Comp
L power:GNDS #PWR012
U 1 1 6164141C
P 7050 3200
F 0 "#PWR012" H 7050 2950 50  0001 C CNN
F 1 "GNDS" V 7055 3072 50  0000 R CNN
F 2 "" H 7050 3200 50  0001 C CNN
F 3 "" H 7050 3200 50  0001 C CNN
	1    7050 3200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6750 3200 6750 3350
Connection ~ 6750 3350
Wire Wire Line
	6750 3350 6750 3550
NoConn ~ 5550 3150
Wire Wire Line
	7650 4850 7650 4750
Wire Wire Line
	7400 3100 7650 3100
Connection ~ 7650 4650
Wire Wire Line
	7650 4650 7650 3100
Connection ~ 7650 4750
Wire Wire Line
	7650 4750 7650 4650
$Comp
L Graphic:SYM_Arrow45_Large #SYM1
U 1 1 6163AD97
P 6550 5750
F 0 "#SYM1" H 6550 5910 50  0001 C CNN
F 1 "SYM_Arrow45_Large" H 6550 5610 50  0001 C CNN
F 2 "kitest:uofsc_logo_0_75in" H 6550 5750 50  0001 C CNN
F 3 "~" H 6550 5750 50  0001 C CNN
	1    6550 5750
	1    0    0    -1  
$EndComp
$EndSCHEMATC
