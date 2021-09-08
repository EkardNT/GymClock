#!/bin/bash
/usr/share/arduino/arduino-builder -compile -logger=machine -hardware /usr/share/arduino/hardware -hardware $HOME/.arduino15/packages -tools /usr/share/arduino/tools-builder -tools $HOME/.arduino15/packages -libraries $HOME/Arduino/libraries -fqbn=esp32:esp32:nodemcu-32s:FlashFreq=80,UploadSpeed=921600 -vid-pid=10C4_EA60 -ide-version=10815 -build-path ./build -warnings=none -build-cache ./build-cache -prefs=build.warn_data_percentage=75 -prefs=runtime.tools.riscv32-esp-elf-gcc.path=$HOME/.arduino15/packages/esp32/tools/riscv32-esp-elf-gcc/gcc8_4_0-esp-2021r1 -prefs=runtime.tools.riscv32-esp-elf-gcc-gcc8_4_0-esp-2021r1.path=$HOME/.arduino15/packages/esp32/tools/riscv32-esp-elf-gcc/gcc8_4_0-esp-2021r1 -prefs=runtime.tools.xtensa-esp32s2-elf-gcc.path=$HOME/.arduino15/packages/esp32/tools/xtensa-esp32s2-elf-gcc/gcc8_4_0-esp-2021r1 -prefs=runtime.tools.xtensa-esp32s2-elf-gcc-gcc8_4_0-esp-2021r1.path=$HOME/.arduino15/packages/esp32/tools/xtensa-esp32s2-elf-gcc/gcc8_4_0-esp-2021r1 -prefs=runtime.tools.esptool_py.path=$HOME/.arduino15/packages/esp32/tools/esptool_py/3.1.0 -prefs=runtime.tools.esptool_py-3.1.0.path=$HOME/.arduino15/packages/esp32/tools/esptool_py/3.1.0 -prefs=runtime.tools.xtensa-esp32-elf-gcc.path=$HOME/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r1 -prefs=runtime.tools.xtensa-esp32-elf-gcc-gcc8_4_0-esp-2021r1.path=$HOME/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r1 -prefs=runtime.tools.mkspiffs.path=$HOME/.arduino15/packages/esp32/tools/mkspiffs/0.2.3 -prefs=runtime.tools.mkspiffs-0.2.3.path=$HOME/.arduino15/packages/esp32/tools/mkspiffs/0.2.3 -prefs=runtime.tools.mklittlefs.path=$HOME/.arduino15/packages/esp32/tools/mklittlefs/3.0.0-gnu12-dc7f933 -prefs=runtime.tools.mklittlefs-3.0.0-gnu12-dc7f933.path=$HOME/.arduino15/packages/esp32/tools/mklittlefs/3.0.0-gnu12-dc7f933 -verbose $HOME/GymClock/esp32-experiments/WiFiAccessPoint/WiFiAccessPoint.ino