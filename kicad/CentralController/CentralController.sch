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
$Comp
L CentralControllerLibrary:ESP8266 U1
U 1 1 5F9CF011
P 5500 4200
F 0 "U1" H 5500 5165 50  0000 C CNN
F 1 "ESP8266" H 5500 5074 50  0000 C CNN
F 2 "CentralController:ESP8266NodeMCU" H 5500 4050 50  0001 C CNN
F 3 "" H 5500 4050 50  0001 C CNN
	1    5500 4200
	1    0    0    -1  
$EndComp
Text Label 6000 3600 0    50   ~ 0
GND
Text Label 6000 3900 0    50   ~ 0
3V3
Text Label 6000 4000 0    50   ~ 0
GND
Text Label 5000 4400 2    50   ~ 0
3V3
Text Label 5000 4300 2    50   ~ 0
GND
Text Label 5000 3600 2    50   ~ 0
GND
Text Label 5000 3500 2    50   ~ 0
3V3
NoConn ~ 6000 3500
NoConn ~ 6000 3800
NoConn ~ 6000 3700
NoConn ~ 6000 4100
NoConn ~ 6000 4200
NoConn ~ 6000 4300
NoConn ~ 6000 4400
NoConn ~ 6000 4500
NoConn ~ 6000 4600
NoConn ~ 6000 4700
NoConn ~ 6000 4800
NoConn ~ 6000 4900
Text Label 5000 4500 2    50   ~ 0
ADDR0
Text Label 5000 4200 2    50   ~ 0
ADDR1
Text Label 5000 4100 2    50   ~ 0
ADDR2
Text Label 5000 4000 2    50   ~ 0
ADDR3
$Comp
L SamacSys_Parts:OKI-78SR-3.3_1.5-W36E-C PS1
U 1 1 5F9D0E12
P 4950 2350
F 0 "PS1" H 5778 2296 50  0000 L CNN
F 1 "OKI-78SR-3.3_1.5-W36E-C" H 5778 2205 50  0000 L CNN
F 2 "SamacSys_Parts:OKI-78SR_V" H 5800 2450 50  0001 L CNN
F 3 "https://componentsearchengine.com/Datasheets/1/OKI-78SR-3.3_1.5-W36E-C.pdf" H 5800 2350 50  0001 L CNN
F 4 "Non-Isolated DC/DC Converters 7-36Vin 3.3Vout 1.5A VSIP Encapsulated" H 5800 2250 50  0001 L CNN "Description"
F 5 "" H 5800 2150 50  0001 L CNN "Height"
F 6 "580-78SR3.3/1.5W36EC" H 5800 2050 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/Murata-Power-Solutions/OKI-78SR-33-15-W36E-C?qs=AQlKX63v8RtE9SkoXLzDAA%3D%3D" H 5800 1950 50  0001 L CNN "Mouser Price/Stock"
F 8 "Murata Electronics" H 5800 1850 50  0001 L CNN "Manufacturer_Name"
F 9 "OKI-78SR-3.3/1.5-W36E-C" H 5800 1750 50  0001 L CNN "Manufacturer_Part_Number"
	1    4950 2350
	1    0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J1
U 1 1 5F9D1ED7
P 5350 2000
F 0 "J1" H 5120 1950 50  0000 R CNN
F 1 "Barrel_Jack_Switch" H 5120 2041 50  0000 R CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 5400 1960 50  0001 C CNN
F 3 "~" H 5400 1960 50  0001 C CNN
	1    5350 2000
	-1   0    0    1   
$EndComp
Text Label 5050 1900 2    50   ~ 0
GND
Text Label 5050 2000 2    50   ~ 0
GND
Text Label 5050 2100 2    50   ~ 0
15V
Text Label 4950 2350 2    50   ~ 0
15V
Text Label 4950 2450 2    50   ~ 0
GND
Text Label 4950 2550 2    50   ~ 0
3V3
$Comp
L Transistor_BJT:TIP120 Q1
U 1 1 5F9D374B
P 8050 3750
F 0 "Q1" H 8257 3796 50  0000 L CNN
F 1 "TIP120" H 8257 3705 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 8250 3675 50  0001 L CIN
F 3 "http://www.fairchildsemi.com/ds/TI/TIP120.pdf" H 8050 3750 50  0001 L CNN
	1    8050 3750
	1    0    0    -1  
$EndComp
Text Label 8150 3950 3    50   ~ 0
GND
$Comp
L Device:R R1
U 1 1 5F9D4CC5
P 7250 3600
F 0 "R1" V 7043 3600 50  0000 C CNN
F 1 "10k" V 7134 3600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7180 3600 50  0001 C CNN
F 3 "~" H 7250 3600 50  0001 C CNN
	1    7250 3600
	0    1    1    0   
$EndComp
$Comp
L Device:R_POT RV1
U 1 1 5F9D64C0
P 7550 3600
F 0 "RV1" H 7480 3554 50  0000 R CNN
F 1 "10k" H 7480 3645 50  0000 R CNN
F 2 "Potentiometer_THT:Potentiometer_Piher_T-16H_Single_Horizontal" H 7550 3600 50  0001 C CNN
F 3 "~" H 7550 3600 50  0001 C CNN
	1    7550 3600
	-1   0    0    1   
$EndComp
Wire Wire Line
	7550 3750 7850 3750
NoConn ~ 7550 3450
Text Label 8150 3550 1    50   ~ 0
SPKR_IN
Text Label 7100 3600 2    50   ~ 0
SPKR_CTRL
Text Label 5000 4900 2    50   ~ 0
SPKR_CTRL
Text Label 5000 4800 2    50   ~ 0
SER
Text Label 5000 4600 2    50   ~ 0
SRCLK
Text Label 5000 4700 2    50   ~ 0
RCLK
NoConn ~ 5000 3900
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5F9DAF1A
P 3800 3500
F 0 "J2" H 3718 3175 50  0000 C CNN
F 1 "SerialConn" H 3718 3266 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3800 3500 50  0001 C CNN
F 3 "~" H 3800 3500 50  0001 C CNN
	1    3800 3500
	-1   0    0    1   
$EndComp
Text Label 5000 3700 2    50   ~ 0
TX
Text Label 5000 3800 2    50   ~ 0
RX
Text Label 4000 3400 0    50   ~ 0
TX
Text Label 4000 3500 0    50   ~ 0
RX
Text Label 8950 3900 2    50   ~ 0
15V
Text Label 8950 4000 2    50   ~ 0
SPKR_IN
$Comp
L Connector_Generic:Conn_01x09 J4
U 1 1 5F9DD180
P 3800 4400
F 0 "J4" H 3800 3750 50  0000 C CNN
F 1 "DigitControlConn" H 3800 3850 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x09_P2.54mm_Vertical" H 3800 4400 50  0001 C CNN
F 3 "~" H 3800 4400 50  0001 C CNN
	1    3800 4400
	-1   0    0    1   
$EndComp
Text Label 4000 4300 0    50   ~ 0
ADDR0
Text Label 4000 4200 0    50   ~ 0
ADDR1
Text Label 4000 4100 0    50   ~ 0
ADDR2
Text Label 4000 4000 0    50   ~ 0
ADDR3
Text Label 4000 4600 0    50   ~ 0
SER
Text Label 4000 4400 0    50   ~ 0
SRCLK
Text Label 4000 4500 0    50   ~ 0
RCLK
$Comp
L Device:CP C1
U 1 1 5F9ED5E3
P 4300 2400
F 0 "C1" H 4418 2446 50  0000 L CNN
F 1 "1000uF" H 4418 2355 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 4338 2250 50  0001 C CNN
F 3 "~" H 4300 2400 50  0001 C CNN
	1    4300 2400
	1    0    0    -1  
$EndComp
Text Label 4300 2250 1    50   ~ 0
15V
Text Label 4300 2550 3    50   ~ 0
GND
Text Label 4000 4700 0    50   ~ 0
GND
Text Label 4000 4800 0    50   ~ 0
3V3
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 5F9F00D6
P 3800 5300
F 0 "J5" H 3718 4975 50  0000 C CNN
F 1 "DigitPowerConn" H 3718 5066 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 3800 5300 50  0001 C CNN
F 3 "~" H 3800 5300 50  0001 C CNN
	1    3800 5300
	-1   0    0    1   
$EndComp
Text Label 4000 5200 0    50   ~ 0
15V
Text Label 4000 5300 0    50   ~ 0
GND
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5F9F14D9
P 9150 3900
F 0 "J3" H 9230 3892 50  0000 L CNN
F 1 "SpeakerConn" H 9230 3801 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 9150 3900 50  0001 C CNN
F 3 "~" H 9150 3900 50  0001 C CNN
	1    9150 3900
	1    0    0    -1  
$EndComp
$EndSCHEMATC
