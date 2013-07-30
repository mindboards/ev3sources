#! /bin/bash
#******************************************************************************************************************
#     COLLECT OS PACKAGE
#******************************************************************************************************************
# as normal user on linux pc terminal:

echo
echo -------------------------------------------------------------------------------
echo BUILDING OS PACKAGE
echo -------------------------------------------------------------------------------
echo

DATE=`/bin/date +%Y%m%d`
project=~/projects/lms2012/open_first
source ${project}/env_setup

cd ${project}

if [ -d os_temp ]
then
   rm -R ${project}/os_temp
   mkdir os_temp
else
   mkdir os_temp
fi

echo "Compressing Kernel Source..."
cd ${AM1808_KERNEL}/../
tar zcf ${project}/os_temp/${AM1808_LINUX}_${DATE}.tgz ${AM1808_LINUX}

echo "Compressing uBoot Source..."
cd ${AM1808_UBOOT_DIR}/../
tar jcf ${project}/os_temp/${AM1808_UBOOT}_${DATE}.tar.bz2 ${AM1808_UBOOT}

echo "Compressing linux-devkit..."
cd ~/am1808/${AM1808_DISTRO}
tar zcf ${project}/os_temp/linux-devkit.tar.gz linux-devkit

echo "Compressing files to EV3os.tar.bz2..."
cd ${project}/os_temp
#tar zcf ../EV3os_${DATE}.tar.bz2 ./*
tar jcf ../EV3os.tar.bz2 ./*

