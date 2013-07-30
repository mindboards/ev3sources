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

1.  Place source code in "~/projects/lms2012/lmssrc/adk/lmststasm/DebugBytecodePrograms/Bytecode.src" file

2.  At PC prompt: Type command below:

cd ~/projects/lms2012/lmssrc/adk/lmststasm/DebugBytecodePrograms

./assemble.rc Bytecode xxx.xxx.xxx.xxx

3.  On EV3 start "Bytecode" in "BrkProg_SAVE"




MIXED COMMANDS
--------------

ssh root@10.131.145.5

scp fdisk.cmd root@10.131.145.5:/mnt/ramdisk/.

http://rcc.its.psu.edu/user_guides/remote_connectivity/ssh/

scp StartSSH.rbf root@10.131.145.5:/media/card/TEST/StartSSH.rbf

ssh user@machine 'bash -s' < local_script.sh

ssh user@machine "remote command to run" 

ssh-keygen -R 10.131.145.5

telnet 10.131.145.5

ifconfig eth0

/sbin/ifconfig eth0 | grep "inet addr" | awk -F: '{print $2}' | awk '{print $1}' > ip.rtf

netstat

scp root@10.131.145.5:/mnt/ramdisk/settings/dropbear_rsa_host_key .

ssh-keygen -t rsa

cat .ssh/id_rsa.pub | ssh root@10.131.145.5 'cat >> /mnt/ramdisk/settings/dropbear_rsa_host_key'











