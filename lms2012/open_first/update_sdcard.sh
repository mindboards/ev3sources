#! /bin/bash
echo
echo -------------------------------------------------------------------------------
echo UPDATE SDCARD WITH NEWEST KERNEL, FILESYSTEM AND APPLICATION          TCP120709
echo -------------------------------------------------------------------------------
echo
sudo -v
echo
echo "  ...."checking.sdcard
sleep 10

current=${PWD}

if [ -d /media/LMS2012 ]
then

    if [ -d /media/LMS2012_EXT ]
    then

        echo "  ...."erasing.sdcard
        sudo rm -r /media/LMS2012/*
        sudo rm -r /media/LMS2012_EXT/*
        sync

        echo "  ...."copying.kernel.to.sdcard
        sudo cp uImage /media/LMS2012/uImage
        sync

        echo "  ...."copying.filesystem.to.sdcard
	sudo cp lmsfs.tar.bz2 /media/LMS2012_EXT
	cd /media/LMS2012_EXT
	sudo tar -jxf lmsfs.tar.bz2 
	sudo rm lmsfs.tar.bz2
	cd ${current}
        sync

        echo "  ...."copying.application.to.sdcard
        sudo cp -r ~/projects/lms2012/lms2012/Linux_AM1808/* /media/LMS2012_EXT/home/root/lms2012

        echo "  ...."copying.testprograms.to.sdcard
        sudo cp -r ~/projects/lms2012/lmssrc/Test/Test /media/LMS2012_EXT/home/root/lms2012/prjs/

        echo "  ...."writing.to.sdcard
        sync

        echo
        echo REMOVE sdcard

    else

        echo
        echo SDCARD NOT PROPERLY FORMATTED !!!

    fi

else

    echo
    echo SDCARD NOT PROPERLY FORMATTED !!!

fi
echo
echo -------------------------------------------------------------------------------
echo

