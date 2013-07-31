cmd_fs/fat/fat.o := arm-none-linux-gnueabi-ld -EL    -r -o fs/fat/fat.o fs/fat/cache.o fs/fat/dir.o fs/fat/fatent.o fs/fat/file.o fs/fat/inode.o fs/fat/misc.o 
