cmd_drivers/spi/built-in.o :=  arm-none-linux-gnueabi-ld -EL    -r -o drivers/spi/built-in.o drivers/spi/spi.o drivers/spi/spi_bitbang.o drivers/spi/davinci_spi.o 
