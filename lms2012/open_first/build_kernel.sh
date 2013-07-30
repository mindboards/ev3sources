#! /bin/bash
#******************************************************************************************************************
#     COMPILE KERNEL
#******************************************************************************************************************
# as normal user on linux pc terminal:

echo
echo -------------------------------------------------------------------------------
echo BUILDING KERNEL
echo -------------------------------------------------------------------------------
echo
sleep 1

project=~/projects/lms2012/open_first

source ${project}/env_setup

PATH=${AM1808_COMPILER}:$PATH
PATH=${AM1808_UBOOT_DIR}/tools:$PATH

cd ${AM1808_KERNEL}

make distclean ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

cp ${AM1808_KERNEL}/pru-firmware-05-31-2011-1423-v3.0/PRU_SUART_Emulation.bin ${AM1808_KERNEL}/PRU/

cp ${project}/LEGOBoard.config ${AM1808_KERNEL}/.config
make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

make -j4 uImage ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

cp ${AM1808_KERNEL}/arch/arm/boot/uImage ${project}/uImage

#make modules ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-
#sudo make modules_install INSTALL_MOD_PATH=${_PATH} ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi-

