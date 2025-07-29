
echo off
..\avr-objcopy -O binary -R .eeprom -R .fuse -R lock %3 %2\%1_vsc.bin
..\avr-objdump -h -S %3 > %2\%1.lss
echo ******** build %2\%1.bin *************