#! /bin/bash
#******************************************************************************************************************
#     BUILD U-BOOT
#******************************************************************************************************************
# as normal user on linux pc terminal:

echo
echo -------------------------------------------------------------------------------
echo BUILDING U-BOOT
echo -------------------------------------------------------------------------------
echo
sleep 2

project=~/projects/lms2012/open_first

source ${project}/env_setup

PATH=${AM1808_COMPILER}:$PATH

cd ${AM1808_UBOOT_DIR}

make distclean CROSS_COMPILE=arm-none-linux-gnueabi-

make da850_omapl138_evm_config CROSS_COMPILE=arm-none-linux-gnueabi-

make all CROSS_COMPILE=arm-none-linux-gnueabi-

cp u-boot.bin ${project}/uBoot
