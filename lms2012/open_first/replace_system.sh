#! /bin/bash
#******************************************************************************************************************
#     REPLACE U-BOOT, KERNEL AND FILESYSTEM
#******************************************************************************************************************
# as normal user on linux pc terminal:

echo
echo -------------------------------------------------------------------------------
echo REPLACING U-BOOT, KERNEL AND FILESYSTEM
echo -------------------------------------------------------------------------------
echo
sleep 1

echo "  ...."deleting.filesystem
cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13
sudo rm -r filesys

echo "  ...."deleting.uboot.source
cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13/src
rm -r u-boot

echo "  ...."deleting.kernel.source
cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13/src
rm -r kernel



echo "  ...."unpacking.EV3os.package
cd ~
tar -jxf EV3os.tar.bz2



echo "  ...."making.new.filesystem
cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13
sudo mkdir filesys
cd filesys
sudo cp ~/projects/lms2012/open_first/lmsfs*.tar.bz2 ./
sudo tar -jxf lmsfs*.tar.bz2
sudo rm lmsfs*.tar.bz2


cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13/filesys/home/root
sudo rm -r lms2012
sudo mkdir lms2012
sudo chmod 777 lms2012
cd ~


echo "  ...."installing.new.uboot.source
cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13/src
mkdir u-boot
cd u-boot
cp ~/uboot-03.20.00.13_*.tar.bz2 ./
tar -jxf uboot-03.20.00.13_*.tar.bz2
rm uboot-03.20.00.13_*.tar.bz2

echo "  ...."installing.new.kernel.source
cd ~/am1808/DaVinci-PSP-SDK-03.20.00.13/src
mkdir kernel
cd kernel
cp ~/linux-03.20.00.13_*.tgz ./
tar zxf linux-03.20.00.13_*.tgz
rm linux-03.20.00.13_*.tgz



echo "  ...."deleting.packages
cd ~
rm linux-03.20.00.13_*.tgz
rm linux-devkit.tar.gz
rm uboot-03.20.00.13_*.tar.bz2



echo "  ...."compiling.uboot
cd ~/projects/lms2012/open_first
./uboot_build.sh

echo "  ...."compiling.kernel
cd ~/projects/lms2012/open_first
./build_kernel.sh





