
echo off
set p=%3
set nw=elf
call set p=%%p:hex=%nw%%%
echo "path: "%2\%p%
..\avr-objcopy -O binary -R .eeprom -R .fuse -R lock %2\%p% %2\%1.bin
echo ******** build %2\%1.bin *************