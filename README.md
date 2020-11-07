# GymClock

This repository contains the Arduino code, Blender models, and Kicad projects that I used to build the programmable gym clock at the [Fremont Gymnasium](https://www.fremontgymnasium.com/).

![View of sign interior](Interior.jpg)

## How to update

If you're ok with pulling the sign down, you can flash the esp8266 via the usb port as normal. The sketch also supports wireless flashing of signed binaries. The public key for the signing is the [public.key](public.key) file in this repo. The private key is on my laptop. I'll also store a copy of the private key in a USB drive and put the drive inside the sign itself.

## Errata

I had a bug in the DigitController2 schematic that I didn't catch until after I had the PCBs manufactured. The bug was that the SER and SRCLK lines are swapped. I fixed the bug in the schematic, but did not order new PCBs because I was able to work around the bug by simply swapping the SER and SRCLK wiring lines from the central controller to the main bus.
