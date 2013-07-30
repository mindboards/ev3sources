#! /bin/bash

path="sys"
app="tst"

#*** GENERIC CONSTANTS *******************************************************

gext=".rgf"
bext=".rbf"
am1808="../../lms2012/Linux_AM1808"
x86="../../lms2012/Linux_X86"

#*** COMPILE *****************************************************************

echo
echo -------------------------------------------------------------------------
echo Compiling ../${path}/${app}
echo

rm "${app}"/*"${bext}"

lms="tstlog"
../adk/asm "${lms}"
cp "${lms}""${bext}" "${app}"/
rm "${lms}""${bext}"


#*** COPY IN PLACE ***********************************************************

cp "${app}"/"${lms}".r?f "${am1808}"/"${path}"/ui/
cp "${app}"/"${lms}".r?f "${x86}"/"${path}"/ui/
cp "${app}"/"${lms}".r?f ../../../../am1808/DaVinci-PSP-SDK-03.20.00.13/filesys/home/root/lms2012/"${path}"/ui/




