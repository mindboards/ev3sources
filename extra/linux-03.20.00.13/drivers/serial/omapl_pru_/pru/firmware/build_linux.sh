#!/bin/bash

../utils/pru-as/linux/pasm -b  -DMCASP0 uart/PRU_SUART_Emulation.p bin/PRU_SUART_Emulation
if [ "$?" -ne 0 ];then
   echo "Build failed for uart/PRU_SUART_Emulation.p"
   exit
fi
../utils/pru-as/linux/pasm -b can/PRU_CAN_Emulation_Rx.p bin/PRU_CAN_Emulation_Rx
if [ "$?" -ne 0 ];then
   echo "Build failed for can/PRU_CAN_Emulation_Rx.p"
   exit
fi
../utils/pru-as/linux/pasm -b can/PRU_CAN_Emulation_Tx.p bin/PRU_CAN_Emulation_Tx
if [ "$?" -ne 0 ];then
   echo "Build failed for can/PRU_CAN_Emulation_Tx.p"
   exit
fi
