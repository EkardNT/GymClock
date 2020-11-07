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
L 74xx:74HC688 U2
U 1 1 5F68A6CE
P 5500 2550
F 0 "U2" H 6050 2600 50  0000 L CNN
F 1 "74HC688 - IDENTITY COMPARATOR" V 5950 1900 50  0000 L CNN
F 2 "Package_DIP:DIP-20_W7.62mm_LongPads" H 5500 2550 50  0001 C CNN
F 3 "https://www.ti.com/lit/ds/symlink/cd54hc688.pdf" H 5500 2550 50  0001 C CNN
	1    5500 2550
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_DIP_x04 SW1
U 1 1 5F68E487
P 3300 1850
F 0 "SW1" H 3300 2317 50  0000 C CNN
F 1 "SW_DIP_x04" H 3300 2226 50  0000 C CNN
F 2 "Button_Switch_THT:SW_DIP_SPSTx04_Slide_9.78x12.34mm_W7.62mm_P2.54mm" H 3300 1850 50  0001 C CNN
F 3 "~" H 3300 1850 50  0001 C CNN
	1    3300 1850
	1    0    0    -1  
$EndComp
Text Label 5500 1350 1    50   ~ 0
VCC
Text Label 3000 1650 2    50   ~ 0
VCC
Text Label 3000 1750 2    50   ~ 0
VCC
Text Label 3000 1850 2    50   ~ 0
VCC
Text Label 3000 1950 2    50   ~ 0
VCC
$Comp
L Device:R R1
U 1 1 5F694CE2
P 3750 2250
F 0 "R1" H 3820 2296 50  0000 L CNN
F 1 "47k" H 3820 2205 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 3680 2250 50  0001 C CNN
F 3 "~" H 3750 2250 50  0001 C CNN
	1    3750 2250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5F6956F7
P 3950 2250
F 0 "R2" H 4020 2296 50  0000 L CNN
F 1 "47k" H 4020 2205 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 3880 2250 50  0001 C CNN
F 3 "~" H 3950 2250 50  0001 C CNN
	1    3950 2250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 5F695840
P 4150 2250
F 0 "R3" H 4220 2296 50  0000 L CNN
F 1 "47k" H 4220 2205 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 4080 2250 50  0001 C CNN
F 3 "~" H 4150 2250 50  0001 C CNN
	1    4150 2250
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 5F6959FF
P 4350 2250
F 0 "R4" H 4420 2296 50  0000 L CNN
F 1 "47k" H 4420 2205 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 4280 2250 50  0001 C CNN
F 3 "~" H 4350 2250 50  0001 C CNN
	1    4350 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 1950 3750 1950
Wire Wire Line
	3750 1950 3750 2100
Connection ~ 3750 1950
Wire Wire Line
	3600 1850 3950 1850
Wire Wire Line
	3600 1750 4150 1750
Wire Wire Line
	3600 1650 4350 1650
Wire Wire Line
	3950 2100 3950 1850
Connection ~ 3950 1850
Wire Wire Line
	4150 2100 4150 1750
Connection ~ 4150 1750
Wire Wire Line
	4350 2100 4350 1650
Connection ~ 4350 1650
Text Label 3750 2400 3    50   ~ 0
GND
Text Label 3950 2400 3    50   ~ 0
GND
Text Label 4150 2400 3    50   ~ 0
GND
Text Label 4350 2400 3    50   ~ 0
GND
Wire Wire Line
	4350 1650 5000 1650
Wire Wire Line
	4150 1750 5000 1750
Wire Wire Line
	3950 1850 5000 1850
Wire Wire Line
	3750 1950 5000 1950
Text Label 5000 2050 2    50   ~ 0
GND
Text Label 5000 2150 2    50   ~ 0
GND
Text Label 5000 2250 2    50   ~ 0
GND
Text Label 5000 2350 2    50   ~ 0
GND
Text Label 5000 2950 2    50   ~ 0
GND
Text Label 5000 3050 2    50   ~ 0
GND
Text Label 5000 3150 2    50   ~ 0
GND
Text Label 5000 3250 2    50   ~ 0
GND
Text Label 5000 2550 2    50   ~ 0
ADDR0_IN
Text Label 5000 2650 2    50   ~ 0
ADDR1_IN
Text Label 5000 2750 2    50   ~ 0
ADDR2_IN
Text Label 5000 2850 2    50   ~ 0
ADDR3_IN
Text Label 6000 1650 0    50   ~ 0
DIGIT_SELECT
Text Label 5000 3450 2    50   ~ 0
GND
Text Label 5500 3750 3    50   ~ 0
GND
Wire Notes Line
	2750 1150 2750 3950
Wire Notes Line
	6550 3950 6550 1150
Text Notes 2750 1100 0    50   ~ 0
Digit Identity Selector\n
Wire Notes Line
	6550 1150 2750 1150
Wire Notes Line
	2750 3950 6550 3950
$Comp
L SamacSys_Parts:SN74HC27N IC1
U 1 1 5F978FCA
P 4800 4850
F 0 "IC1" H 5300 5115 50  0000 C CNN
F 1 "SN74HC27N - NOR" H 5300 5024 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 5650 4950 50  0001 L CNN
F 3 "http://www.ti.com/lit/gpn/sn74hc27" H 5650 4850 50  0001 L CNN
F 4 "Triple 3 i/p NOR gate,SN74HC27N 25pcs" H 5650 4750 50  0001 L CNN "Description"
F 5 "5.08" H 5650 4650 50  0001 L CNN "Height"
F 6 "595-SN74HC27N" H 5650 4550 50  0001 L CNN "Mouser Part Number"
F 7 "https://www.mouser.co.uk/ProductDetail/Texas-Instruments/SN74HC27N?qs=dQbkqP7qjaYcPBClMsy6Og%3D%3D" H 5650 4450 50  0001 L CNN "Mouser Price/Stock"
F 8 "Texas Instruments" H 5650 4350 50  0001 L CNN "Manufacturer_Name"
F 9 "SN74HC27N" H 5650 4250 50  0001 L CNN "Manufacturer_Part_Number"
	1    4800 4850
	1    0    0    -1  
$EndComp
Text Label 4800 4850 2    50   ~ 0
DIGIT_SELECT
Text Label 4800 5050 2    50   ~ 0
DIGIT_SELECT
Text Label 5800 5350 0    50   ~ 0
DIGIT_SELECT
Text Label 5800 4850 0    50   ~ 0
VCC
Text Label 4800 5450 2    50   ~ 0
GND
Text Label 4800 5250 2    50   ~ 0
GND
Text Label 5800 4950 0    50   ~ 0
GND
Text Label 5800 5150 0    50   ~ 0
GND
Text Label 3700 5300 0    50   ~ 0
SER_IN
Text Label 3700 5100 0    50   ~ 0
RCLK_IN
Text Label 3700 4900 0    50   ~ 0
SRCLK_IN
Text Label 3000 5400 2    50   ~ 0
GND
Text Label 3000 5300 2    50   ~ 0
GND
Text Label 3000 5200 2    50   ~ 0
GND
Text Label 3000 5100 2    50   ~ 0
GND
Text Label 3000 5000 2    50   ~ 0
GND
Text Label 3000 4900 2    50   ~ 0
GND
Text Label 3350 4650 1    50   ~ 0
VCC
Text Label 3350 5650 3    50   ~ 0
GND
Wire Wire Line
	3700 5200 4400 5200
Wire Wire Line
	4400 5200 4400 5150
Wire Wire Line
	4400 5150 4800 5150
Wire Wire Line
	5950 5250 5800 5250
$Comp
L DigitController2Symbols:SN74HC04N U1
U 1 1 5F97F005
P 3350 5150
F 0 "U1" H 3350 5950 50  0000 C CNN
F 1 "SN74HC04N - INVERTER" H 3350 5850 50  0000 C CNN
F 2 "Package_DIP:DIP-14_W7.62mm_LongPads" H 3350 5850 50  0001 C CNN
F 3 "https://www.ti.com/lit/ds/symlink/sn74hc04.pdf" H 3350 5900 50  0001 C CNN
	1    3350 5150
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC595 U3
U 1 1 5F982E71
P 6950 5150
F 0 "U3" H 6950 6050 50  0000 C CNN
F 1 "74HC595 - Shift Register" H 6950 5950 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm_LongPads" H 6950 5150 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 6950 5150 50  0001 C CNN
	1    6950 5150
	1    0    0    -1  
$EndComp
Text Label 7200 2150 0    50   ~ 0
ADDR3_IN
Text Label 7200 2250 0    50   ~ 0
ADDR2_IN
Text Label 7200 2350 0    50   ~ 0
ADDR1_IN
Text Label 7200 2450 0    50   ~ 0
ADDR0_IN
Text Label 7200 2550 0    50   ~ 0
SRCLK_IN
Text Label 7200 2650 0    50   ~ 0
RCLK_IN
Text Label 7200 2750 0    50   ~ 0
SER_IN
Text Label 8000 2150 0    50   ~ 0
SEG_H
Text Label 8000 2300 0    50   ~ 0
SEG_G
Text Label 8000 2450 0    50   ~ 0
SEG_F
Text Label 8000 2600 0    50   ~ 0
SEG_E
Text Label 8000 2750 0    50   ~ 0
SEG_D
Text Label 8000 2900 0    50   ~ 0
SEG_C
Text Label 8000 3050 0    50   ~ 0
SEG_B
Text Label 8000 3200 0    50   ~ 0
SEG_A
$Comp
L Connector_Generic:Conn_01x01 J9
U 1 1 5F67B2A4
P 7800 3200
F 0 "J9" H 7718 2975 50  0000 C CNN
F 1 "Conn_SegA" H 7718 3066 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 3200 50  0001 C CNN
F 3 "~" H 7800 3200 50  0001 C CNN
	1    7800 3200
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J8
U 1 1 5F67B11C
P 7800 3050
F 0 "J8" H 7718 2825 50  0000 C CNN
F 1 "Conn_SegB" H 7718 2916 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 3050 50  0001 C CNN
F 3 "~" H 7800 3050 50  0001 C CNN
	1    7800 3050
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 5F67B028
P 7800 2900
F 0 "J7" H 7718 2675 50  0000 C CNN
F 1 "Conn_SegC" H 7718 2766 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 2900 50  0001 C CNN
F 3 "~" H 7800 2900 50  0001 C CNN
	1    7800 2900
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J6
U 1 1 5F67AED1
P 7800 2750
F 0 "J6" H 7718 2525 50  0000 C CNN
F 1 "Conn_SegD" H 7718 2616 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 2750 50  0001 C CNN
F 3 "~" H 7800 2750 50  0001 C CNN
	1    7800 2750
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 5F67AD8E
P 7800 2600
F 0 "J5" H 7718 2375 50  0000 C CNN
F 1 "Conn_SegE" H 7718 2466 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 2600 50  0001 C CNN
F 3 "~" H 7800 2600 50  0001 C CNN
	1    7800 2600
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 5F67ABA9
P 7800 2450
F 0 "J3" H 7718 2225 50  0000 C CNN
F 1 "Conn_SegF" H 7718 2316 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 2450 50  0001 C CNN
F 3 "~" H 7800 2450 50  0001 C CNN
	1    7800 2450
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 5F67A915
P 7800 2300
F 0 "J2" H 7718 2075 50  0000 C CNN
F 1 "Conn_SegG" H 7718 2166 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 2300 50  0001 C CNN
F 3 "~" H 7800 2300 50  0001 C CNN
	1    7800 2300
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J1
U 1 1 5F67A2EC
P 7800 2150
F 0 "J1" H 7718 1925 50  0000 C CNN
F 1 "Conn_SegH" H 7718 2016 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 7800 2150 50  0001 C CNN
F 3 "~" H 7800 2150 50  0001 C CNN
	1    7800 2150
	-1   0    0    1   
$EndComp
Text Label 7200 2850 0    50   ~ 0
GND
Text Label 7200 2950 0    50   ~ 0
VCC
$Comp
L Connector_Generic:Conn_01x09 J4
U 1 1 5F679515
P 7000 2550
F 0 "J4" H 6918 1925 50  0000 C CNN
F 1 "Conn_Inputs" H 6918 2016 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x09_P2.54mm_Vertical" H 7000 2550 50  0001 C CNN
F 3 "~" H 7000 2550 50  0001 C CNN
	1    7000 2550
	-1   0    0    1   
$EndComp
Wire Wire Line
	5800 5050 6100 5050
Wire Wire Line
	6100 5050 6100 4750
Wire Wire Line
	6100 4750 6550 4750
Text Label 6550 5350 2    50   ~ 0
GND
Text Label 6550 5050 2    50   ~ 0
VCC
Text Label 6950 5850 3    50   ~ 0
GND
Text Label 6950 4550 1    50   ~ 0
VCC
Wire Wire Line
	4800 5350 4550 5350
Wire Wire Line
	4550 5350 4550 5600
Wire Wire Line
	4550 5600 6350 5600
Wire Wire Line
	6350 5600 6350 5250
Wire Wire Line
	6350 5250 6550 5250
Wire Wire Line
	6550 4950 6300 4950
Wire Wire Line
	6300 4950 6300 5450
Wire Wire Line
	6300 5450 5800 5450
NoConn ~ 7350 5650
Text Label 7350 4750 0    50   ~ 0
SEG_A
Text Label 7350 4850 0    50   ~ 0
SEG_B
Text Label 7350 4950 0    50   ~ 0
SEG_C
Text Label 7350 5050 0    50   ~ 0
SEG_D
Text Label 7350 5150 0    50   ~ 0
SEG_E
Text Label 7350 5250 0    50   ~ 0
SEG_F
Text Label 7350 5350 0    50   ~ 0
SEG_G
Text Label 7350 5450 0    50   ~ 0
SEG_H
Wire Notes Line
	7700 4150 7700 6100
Wire Notes Line
	7700 6100 2750 6100
Wire Notes Line
	2750 6100 2750 4150
Wire Notes Line
	2750 4150 7700 4150
Text Notes 2750 4100 0    50   ~ 0
Select-Controlled Shift Register
Wire Wire Line
	5950 5250 5950 5700
Wire Wire Line
	5950 5700 4100 5700
Wire Wire Line
	4100 5700 4100 5000
Wire Wire Line
	4100 5000 3700 5000
Wire Wire Line
	3700 5400 4200 5400
Wire Wire Line
	4200 5400 4200 4950
Wire Wire Line
	4200 4950 4800 4950
$EndSCHEMATC
