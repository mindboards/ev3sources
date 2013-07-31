cmd_drivers/leds/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/leds/built-in.o drivers/leds/led-core.o drivers/leds/led-class.o drivers/leds/led-triggers.o 
