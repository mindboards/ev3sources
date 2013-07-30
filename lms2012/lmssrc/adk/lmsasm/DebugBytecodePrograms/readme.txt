GET THE LINUX PROMPT ON THE EV3
-------------------------------

1.  EV3 is off

2.  Connect a "USB to ETHERNET" adapter in the EV3 USB host connector

3.  Connect the ethernet adaptor to the router (DHCP server)

4.  Power up the EV3 (FW 1.02 and newer)

5.  Get EV3 IP address (xxx.xxx.xxx.xxx) from the bottom of "Brick Info"

6.  Open a telnet connection on a PC console to the EV3 IP address (xxx.xxx.xxx.xxx)

7.  EV3 will asks for login

8.  Type "root" as login and the EV3 should give a prompt like:

root@EV3:~#




STOP THE INITIAL RUNNING EV3 FIRMWARE
-------------------------------------

1.  At EV3 prompt: Type command below:

killall -9 lms2012

2.  Wait until EV3 leds goes out




START SSH SERVER ON EV3
-----------------------

1.  At EV3 prompt: Type command below:

/etc/init.d/dropbear start




REMOVE EV3 KEY FROM KNOWN_HOSTS
-------------------------------

1.  At PC prompt: Type command below:

ssh-keygen -R xxx.xxx.xxx.xxx




START THE EV3 FIRMWARE FOR DEBUGGING
------------------------------------

1.  At EV3 prompt: Type command below:

cd ~/lms2012/sys
./run

2.  Run the "Debug" app on the EV3




HOW TO DEBUG A VM DISASSEMBLED BYTE CODE PROGRAM
------------------------------------------------

1.  Place source code in "~/projects/lms2012/lmssrc/adk/lmsasm/DebugBytecodePrograms/Bytecode.lms" file

2.  At PC prompt: Type command below:

cd ~/projects/lms2012/lmssrc/adk/lmsasm/DebugBytecodePrograms

./compile.rc Bytecode xxx.xxx.xxx.xxx

3.  On EV3 start "Bytecode" in "BrkProg_SAVE"





