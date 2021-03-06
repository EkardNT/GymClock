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
Text Label 4150 3200 0    50   ~ 0
5V
Text Label 4150 3100 0    50   ~ 0
GND
Text Label 4150 3400 0    50   ~ 0
SDA
Text Label 4150 3300 0    50   ~ 0
SCL
$Comp
L Device:Crystal Y1
U 1 1 5F56ED99
P 3450 4600
F 0 "Y1" H 3450 4868 50  0000 C CNN
F 1 "Crystal" H 3450 4777 50  0000 C CNN
F 2 "Crystal:Resonator-2Pin_W10.0mm_H5.0mm" H 3450 4600 50  0001 C CNN
F 3 "~" H 3450 4600 50  0001 C CNN
	1    3450 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5F56F2F5
P 3750 4850
F 0 "C4" H 3800 4750 50  0000 L CNN
F 1 "22pF" H 3450 4800 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 3788 4700 50  0001 C CNN
F 3 "~" H 3750 4850 50  0001 C CNN
	1    3750 4850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5F56FA1E
P 3150 4850
F 0 "C3" H 3000 4750 50  0000 L CNN
F 1 "22pF" H 3300 4900 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 3188 4700 50  0001 C CNN
F 3 "~" H 3150 4850 50  0001 C CNN
	1    3150 4850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 4600 3150 4600
Wire Wire Line
	3150 4600 3150 4700
Wire Wire Line
	3150 5000 3450 5000
Wire Wire Line
	3450 5000 3450 5100
Connection ~ 3450 5000
Wire Wire Line
	3450 5000 3750 5000
Wire Wire Line
	3750 4600 3750 4700
Wire Wire Line
	3600 4600 3750 4600
Text Label 3450 5100 3    50   ~ 0
GND
Wire Wire Line
	3150 4600 3150 4450
Connection ~ 3150 4600
Wire Wire Line
	3750 4600 3750 4450
Connection ~ 3750 4600
Text Label 3750 4450 1    50   ~ 0
XTAL2
Text Label 3150 4450 1    50   ~ 0
XTAL1
Text Label 8200 3450 0    50   ~ 0
XTAL1
Text Label 8200 3550 0    50   ~ 0
XTAL2
Text Label 7600 2550 1    50   ~ 0
5V
Text Label 7600 5550 3    50   ~ 0
GND
Wire Notes Line
	3950 4150 3950 5350
Wire Notes Line
	3950 5350 2950 5350
Wire Notes Line
	2950 5350 2950 4150
Wire Notes Line
	2950 4150 3950 4150
Text Notes 2950 4100 0    50   ~ 0
Crystal
Text Label 4400 4450 1    50   ~ 0
RESET
Text Label 8200 4350 0    50   ~ 0
RESET
Text Label 4200 4850 3    50   ~ 0
5V
NoConn ~ 7700 2550
NoConn ~ 7000 2850
$Comp
L Device:CP C2
U 1 1 5F57712C
P 4100 3750
F 0 "C2" V 4250 3800 50  0000 L CNN
F 1 "10uF" V 4250 3600 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 4138 3600 50  0001 C CNN
F 3 "~" H 4100 3750 50  0001 C CNN
	1    4100 3750
	0    -1   -1   0   
$EndComp
Text Label 3950 3750 2    50   ~ 0
5V
Text Label 4250 3750 0    50   ~ 0
GND
Text Label 4750 3600 0    50   ~ 0
FTDI_DTR
Text Label 4750 3500 0    50   ~ 0
FTDI_RX
Text Label 4750 3400 0    50   ~ 0
FTDI_TX
NoConn ~ 4750 3200
$Comp
L Device:C C1
U 1 1 5F57B263
P 4600 4700
F 0 "C1" H 4715 4746 50  0000 L CNN
F 1 "100nF" H 4715 4655 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.42x1.75mm_HandSolder" H 4638 4550 50  0001 C CNN
F 3 "~" H 4600 4700 50  0001 C CNN
	1    4600 4700
	1    0    0    -1  
$EndComp
Text Label 4600 4850 3    50   ~ 0
FTDI_DTR
Wire Wire Line
	4200 4550 4200 4500
Wire Wire Line
	4200 4500 4400 4500
Wire Wire Line
	4600 4500 4600 4550
Wire Wire Line
	4400 4500 4400 4450
Connection ~ 4400 4500
Wire Wire Line
	4400 4500 4600 4500
Wire Notes Line
	4050 4150 4050 5350
Text Notes 4050 4100 0    50   ~ 0
Reset
Text Label 8200 4550 0    50   ~ 0
FTDI_TX
Text Label 8200 4650 0    50   ~ 0
FTDI_RX
$Comp
L Connector_Generic:Conn_01x06 J2
U 1 1 5F578FCE
P 4550 3400
F 0 "J2" H 4468 2875 50  0000 C CNN
F 1 "FTDI_Conn" H 4468 2966 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 4550 3400 50  0001 C CNN
F 3 "~" H 4550 3400 50  0001 C CNN
	1    4550 3400
	-1   0    0    1   
$EndComp
Text Label 3350 3000 0    50   ~ 0
SEG_A
Text Label 3350 3100 0    50   ~ 0
SEG_B
Text Label 3350 3500 0    50   ~ 0
SEG_C
Text Label 3350 3600 0    50   ~ 0
SEG_D
Text Label 3350 3350 0    50   ~ 0
SEG_E
Text Label 3350 3250 0    50   ~ 0
SEG_F
Text Label 3350 3700 0    50   ~ 0
SEG_G
Text Label 3350 3800 0    50   ~ 0
SEG_H
Text Notes 2950 2700 0    50   ~ 0
Connectors
Text Label 8200 4950 0    50   ~ 0
SEG_A
Text Label 8200 4850 0    50   ~ 0
SEG_B
Text Label 8200 3050 0    50   ~ 0
SEG_C
Text Label 8200 3150 0    50   ~ 0
SEG_D
$Comp
L Connector_Generic:Conn_01x04 J1
U 1 1 5F58D5FD
P 3950 3300
F 0 "J1" H 3868 2875 50  0000 C CNN
F 1 "Master_Conn" H 3868 2966 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3950 3300 50  0001 C CNN
F 3 "~" H 3950 3300 50  0001 C CNN
	1    3950 3300
	-1   0    0    1   
$EndComp
Wire Notes Line
	2950 2750 2950 3950
Wire Notes Line
	2950 2750 5250 2750
Wire Notes Line
	4050 5350 5050 5350
$Comp
L Device:R R1
U 1 1 5F575564
P 4200 4700
F 0 "R1" H 4270 4746 50  0000 L CNN
F 1 "10kOhm" V 4300 4350 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 4130 4700 50  0001 C CNN
F 3 "~" H 4200 4700 50  0001 C CNN
	1    4200 4700
	1    0    0    -1  
$EndComp
Text Label 4750 3300 0    50   ~ 0
5V
Text Label 4750 3100 0    50   ~ 0
GND
NoConn ~ 8200 5250
NoConn ~ 8200 5150
NoConn ~ 7000 3050
NoConn ~ 7000 3150
Text Label 8200 4150 0    50   ~ 0
SDA
Text Label 8200 4250 0    50   ~ 0
SCL
Text Label 8200 2950 0    50   ~ 0
SEG_E
Text Label 8200 2850 0    50   ~ 0
SEG_F
Text Label 8200 3250 0    50   ~ 0
SEG_G
Text Label 8200 3350 0    50   ~ 0
SEG_H
Text Label 8200 3850 0    50   ~ 0
LED
Text Label 5550 3050 1    50   ~ 0
LED
$Comp
L Device:LED D1
U 1 1 5F5A347E
P 5550 3200
F 0 "D1" V 5589 3082 50  0000 R CNN
F 1 "LED" V 5498 3082 50  0000 R CNN
F 2 "SevenSegment:Kingbright_AM2520SURCK03" H 5550 3200 50  0001 C CNN
F 3 "~" H 5550 3200 50  0001 C CNN
	1    5550 3200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 5F5A41A0
P 5550 3500
F 0 "R2" H 5650 3550 50  0000 L CNN
F 1 "220Ohm" H 5650 3450 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 5480 3500 50  0001 C CNN
F 3 "~" H 5550 3500 50  0001 C CNN
	1    5550 3500
	1    0    0    -1  
$EndComp
Text Label 5550 3650 3    50   ~ 0
GND
Wire Notes Line
	6050 2750 5350 2750
Text Notes 5350 2700 0    50   ~ 0
Indicator
$Comp
L Mechanical:MountingHole_Pad H1
U 1 1 5F5AD102
P 5450 4450
F 0 "H1" V 5404 4600 50  0000 L CNN
F 1 "MountingHole_Pad" V 5495 4600 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5450 4450 50  0001 C CNN
F 3 "~" H 5450 4450 50  0001 C CNN
	1    5450 4450
	0    1    1    0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H2
U 1 1 5F5AE75B
P 5450 4650
F 0 "H2" V 5404 4800 50  0000 L CNN
F 1 "MountingHole_Pad" V 5495 4800 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5450 4650 50  0001 C CNN
F 3 "~" H 5450 4650 50  0001 C CNN
	1    5450 4650
	0    1    1    0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H3
U 1 1 5F5AE9E0
P 5450 4850
F 0 "H3" V 5404 5000 50  0000 L CNN
F 1 "MountingHole_Pad" V 5495 5000 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5450 4850 50  0001 C CNN
F 3 "~" H 5450 4850 50  0001 C CNN
	1    5450 4850
	0    1    1    0   
$EndComp
$Comp
L Mechanical:MountingHole_Pad H4
U 1 1 5F5AEB37
P 5450 5050
F 0 "H4" V 5404 5200 50  0000 L CNN
F 1 "MountingHole_Pad" V 5495 5200 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.5mm_Pad_Via" H 5450 5050 50  0001 C CNN
F 3 "~" H 5450 5050 50  0001 C CNN
	1    5450 5050
	0    1    1    0   
$EndComp
Wire Wire Line
	5350 4450 5350 4650
Connection ~ 5350 4650
Wire Wire Line
	5350 4650 5350 4750
Connection ~ 5350 4850
Wire Wire Line
	5350 4850 5350 5050
Wire Wire Line
	5350 4750 5250 4750
Connection ~ 5350 4750
Wire Wire Line
	5350 4750 5350 4850
Text Label 5250 4750 2    50   ~ 0
GND
Wire Notes Line
	5350 2750 5350 3950
Wire Notes Line
	6050 3950 6050 2750
Wire Notes Line
	5350 3950 6050 3950
Wire Notes Line
	2950 3950 5250 3950
Wire Notes Line
	4050 4150 5050 4150
Wire Notes Line
	5250 3950 5250 2750
Wire Notes Line
	5050 5350 5050 4150
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5F5C1271
P 3150 3100
F 0 "J3" H 2650 3000 50  0000 C CNN
F 1 "MConn_AB" H 3068 2866 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3150 3100 50  0001 C CNN
F 3 "~" H 3150 3100 50  0001 C CNN
	1    3150 3100
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 5F5C1AF1
P 3150 3350
F 0 "J4" H 2650 3250 50  0000 C CNN
F 1 "MConn_FE" V 3250 3200 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3150 3350 50  0001 C CNN
F 3 "~" H 3150 3350 50  0001 C CNN
	1    3150 3350
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J5
U 1 1 5F5C1DD7
P 3150 3700
F 0 "J5" H 2650 3650 50  0000 C CNN
F 1 "MConn_CDGH" V 3250 3650 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 3150 3700 50  0001 C CNN
F 3 "~" H 3150 3700 50  0001 C CNN
	1    3150 3700
	-1   0    0    1   
$EndComp
NoConn ~ 8200 4750
$Comp
L MCU_Microchip_ATmega:ATmega328P-AU U1
U 1 1 5F56BBD9
P 7600 4050
F 0 "U1" H 7000 4050 50  0000 C CNN
F 1 "ATmega328P-AU" H 6750 3950 50  0000 C CNN
F 2 "Package_QFP:TQFP-32_7x7mm_P0.8mm" H 7600 4050 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 7600 4050 50  0001 C CNN
	1    7600 4050
	1    0    0    -1  
$EndComp
NoConn ~ 8200 3950
NoConn ~ 8200 4050
NoConn ~ 8200 5050
NoConn ~ 8200 3750
$EndSCHEMATC
