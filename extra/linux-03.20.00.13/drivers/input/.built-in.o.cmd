cmd_drivers/input/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/input/built-in.o drivers/input/input-core.o drivers/input/evdev.o drivers/input/keyboard/built-in.o 
