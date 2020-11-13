#!/bin/bash
/usr/share/arduino/arduino-builder -compile -logger=machine -hardware /usr/share/arduino/hardware -hardware $HOME/.arduino15/packages -tools /usr/share/arduino/tools-builder -tools $HOME/.arduino15/packages -libraries $HOME/Arduino/libraries -fqbn=esp8266:esp8266:nodemcuv2:xtal=80,vt=flash,exception=legacy,ssl=all,eesz=4M2M,led=2,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=115200 -ide-version=10813 -build-path ./build -warnings=none -build-cache ./build-cache -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.xtensa-lx106-elf-gcc.path=$HOME/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 -prefs=runtime.tools.xtensa-lx106-elf-gcc-2.5.0-4-b40a506.path=$HOME/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506 -prefs=runtime.tools.mklittlefs.path=$HOME/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-fe5bb56 -prefs=runtime.tools.mklittlefs-2.5.0-4-fe5bb56.path=$HOME/.arduino15/packages/esp8266/tools/mklittlefs/2.5.0-4-fe5bb56 -prefs=runtime.tools.mkspiffs.path=$HOME/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 -prefs=runtime.tools.mkspiffs-2.5.0-4-b40a506.path=$HOME/.arduino15/packages/esp8266/tools/mkspiffs/2.5.0-4-b40a506 -prefs=runtime.tools.python3.path=$HOME/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 -prefs=runtime.tools.python3-3.7.2-post1.path=$HOME/.arduino15/packages/esp8266/tools/python3/3.7.2-post1 $HOME/GymClock/GymClock.ino