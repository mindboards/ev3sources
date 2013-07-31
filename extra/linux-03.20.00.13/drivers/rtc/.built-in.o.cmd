cmd_drivers/rtc/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/rtc/built-in.o drivers/rtc/rtc-lib.o drivers/rtc/hctosys.o drivers/rtc/rtc-core.o drivers/rtc/rtc-omap.o 
