cmd_drivers/serial/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/serial/built-in.o drivers/serial/serial_core.o drivers/serial/8250.o drivers/serial/8250_early.o 
