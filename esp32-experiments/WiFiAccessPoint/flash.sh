python $HOME/.arduino15/packages/esp32/tools/esptool_py/3.1.0/esptool.py --chip esp32 elf2image --flash_mode dio --flash_freq 80m --flash_size 4MB -o ./build/WiFiAccessPoint.ino.bin ./build/WiFiAccessPoint.ino.elf

python $HOME/.arduino15/packages/esp32/hardware/esp32/2.0.0/tools/gen_esp32part.py -q ./build/partitions.csv ./build/WiFiAccessPoint.ino.partitions.bin

$HOME/.arduino15/packages/esp32/tools/xtensa-esp32-elf-gcc/gcc8_4_0-esp-2021r1/bin/xtensa-esp32-elf-size -A ./build/WiFiAccessPoint.ino.elf