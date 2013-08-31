#/bin/sh

#if [ ! -f /usr/bin/mksquashfs ]; 
#then     
#   echo "File not found - /usr/bin/mksquashfs"
#   sudo apt-get install squashfs-tools
#   exit 0
#fi 

#if [ -d /mnt/tmpdrv_fs ];
#then
#   rm -R /mnt/tmpdrv_fs
#   rm -R /mnt/tmpdrv_ud
#fi

CURRENTPATH=${PWD}
PATH1=~/projects/lms2012
PATH2=open_first
PATH3=lms2012/Linux_AM1808
PATH4=/mnt/tmpdrv_fs/home/root/lms2012
PATH5=/mnt/tmpdrv_fs/etc
PATH6=/mnt/tmpdrv_ud/

mkdir /mnt/tmpdrv_fs/
mkdir /mnt/tmpdrv_ud/

tar jxf $PATH1/$PATH2/lmsfs.tar.bz2 -C /mnt/tmpdrv_fs/

cp -r  $PATH1/$PATH3/* $PATH4/

#rm -r $PATH4/apps/Brick\ Datalog/
cp -r $PATH1/lmssrc/Brick\ Datalog/Brick\ Datalog $PATH4/apps/

cp $PATH1/$PATH2/fstab /mnt/tmpdrv_fs/etc/
mkdir /mnt/tmpdrv_fs/media/card
mkdir /mnt/tmpdrv_fs/media/usb

mv $PATH4/apps /mnt/tmpdrv_ud/
mv $PATH4/prjs /mnt/tmpdrv_ud/
mv $PATH4/sys/settings /mnt/tmpdrv_ud/
mkdir /mnt/tmpdrv_ud/bluetooth

find '/mnt/tmpdrv_fs/' -type d -name CVS -exec rm -rf {} +
find '/mnt/tmpdrv_ud/' -type d -name CVS -exec rm -rf {} +
find '/mnt/tmpdrv_ud/apps/' -type d | xargs -i  mkdir {}/CVS
rm -r /mnt/tmpdrv_ud/apps/CVS
mkdir /mnt/tmpdrv_ud/prjs/BrkProg_SAVE/CVS
mkdir /mnt/tmpdrv_ud/prjs/BrkDL_SAVE
mkdir /mnt/tmpdrv_ud/prjs/BrkDL_SAVE/CVS

#rm $PATH5/rc5.d/S99lms
ln -s /mnt/ramdisk/apps/ $PATH4/apps
ln -s /mnt/ramdisk/prjs/ $PATH4/prjs
ln -s /mnt/ramdisk/settings/ $PATH4/sys/settings

rm $PATH4/../.ash_history
rm $PATH4/lms2012
rm $PATH4/Makefile
rm $PATH4/*.o
rm $PATH4/*.d
rm $PATH4/sys/10-lms.rules
rm $PATH4/sys/gdbinit
rm $PATH4/sys/lib/dummy

sed -i "s/\#\//\//g" $PATH4/sys/exit
echo "EV3" > $PATH5/hostname

chmod 755 $PATH4
chmod 755 $PATH4/../lms2012
find $PATH4/tools/* -type f -name '*.*' -exec chmod 644 {} \;
find $PATH4/sys/mod/* -type f -name '*.*' -exec chmod 644 {} \;
find $PATH4/sys/ui/* -type f -name '*.*' -exec chmod 644 {} \;
find /mnt/tmpdrv_ud/apps/* -type f -name '*.*' -exec chmod 644 {} \;
find /mnt/tmpdrv_ud/prjs/* -type f -name '*.*' -exec chmod 644 {} \;
find /mnt/tmpdrv_ud/settings/* -type f -name '*.*' -exec chmod 644 {} \;
chmod 644 /mnt/tmpdrv_ud/settings/BrickName
#chmod 644 /mnt/tmpdrv_ud/settings/caldata
#chmod 644 /mnt/tmpdrv_ud/settings/lastrun

if [ -f $PATH5/def_sett.tgz ];
then
   rm $PATH5/def_sett.tgz
   echo "Remove Old Files\t: [ DONE ]"
fi
cd /mnt/tmpdrv_ud/
tar zcf $PATH5/def_sett.tgz ./
echo "New DefSettFile Created\t: [ DONE ]"

mkfs.cramfs /mnt/tmpdrv_fs/ $PATH1/$PATH2/EV3.cramfs.bin
echo "CRAMFS File Created\t: [ DONE ]"

#rm $PATH1/$PATH2/EV3.squashfs.bin
#mksquashfs /mnt/tmpdrv_fs/ $PATH1/$PATH2/EV3.squashfs.bin
#echo "SQUASHFS File Created\t: [ DONE ]"

cd $CURRENTPATH
rm -R /mnt/tmpdrv_fs/
rm -R /mnt/tmpdrv_ud/
