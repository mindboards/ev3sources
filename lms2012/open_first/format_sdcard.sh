#! /bin/bash

source ${PWD}/env_setup
curdir=${PWD}
sudo

clear
echo 
echo "*** FORMAT SDCARD ********************************** TCP120706 ***"
echo
echo BE SURE SDCARD IS REMOVED
echo -n and press enter or Ctrl-C to skip" ? "
read

clear
echo 
echo "*** FORMAT SDCARD ********************************** TCP120706 ***"
echo
sleep 2
cd /dev
ls sd?
cd ${curdir}
echo
echo
echo CHECK LIST - INSERT SDCARD
echo -n and press enter or Ctrl-C to skip" ? "
read

clear
echo 
echo "*** FORMAT SDCARD ********************************** TCP120706 ***"
echo
sleep 2
cd /dev
ls sd?
cd ${curdir}
echo
echo
echo CHECK LIST CHANGES - Type the added partition name \(e.c. sdb\) 
echo -n and press enter or Ctrl-C to skip" ? "
read drive

if [ -n "$drive" ]
then

if [ "$drive" != "sda" ]
then

while true
do
clear
echo 
echo "*** FORMAT SDCARD ********************************** TCP120706 ***"
echo
echo
echo "!!! ALL DATA ON DRIVE WILL BE LOST !!!"
echo
echo FORMAT "\""${drive}"\"" 
echo -n press enter or Ctrl-C to skip" ? "
read

echo
echo "  ...."formatting.sdcard

sleep 5

sudo umount /dev/${drive}1 &> /dev/null
sudo umount /dev/${drive}2 &> /dev/null

sudo fdisk /dev/${drive} < fdisk.cmd &>> sdcard.err

sleep 2

echo
echo "  ...."making.kernel.partition
sudo mkfs.msdos -n LMS2012 /dev/${drive}1 &>> sdcard.err

sleep 2

echo
echo "  ...."making.filesystem.partition
sudo mkfs.ext3 -L LMS2012_EXT /dev/${drive}2 &>> sdcard.err

echo
echo "  ...."checking.partitions
sync

if [ -e /dev/${drive}1 ]
then

    if [ -e /dev/${drive}2 ]
    then

        echo
        echo SUCCESS

    else

        echo
echo "******************************************************************"
	cat sdcard.err
echo "******************************************************************"
        echo
        echo SDCARD NOT FORMATTED PROPERLY !!!

    fi

else

    echo
echo "******************************************************************"
    cat sdcard.err
echo "******************************************************************"
    echo
    echo SDCARD NOT FORMATTED PROPERLY !!!

fi

echo
echo REMOVE sdcard
echo 
echo "******************************************************************"
echo

echo
echo FORMAT ANOTHER ONE
echo -n press enter or Ctrl-C to skip" ? "
read

echo
echo
echo INSERT SDCARD
echo -n and press enter or Ctrl-C to skip" ? "
read
sleep 5
done

else

echo
echo YOU MUST NOT SPECIFY "sda" !!!
echo 
echo "******************************************************************"
echo
fi

else

echo
echo YOU MUST SPECIFY A DRIVE !!!
echo 
echo "******************************************************************"
echo
fi





















