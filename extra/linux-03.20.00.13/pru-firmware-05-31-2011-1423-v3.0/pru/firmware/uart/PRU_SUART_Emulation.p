//******************************************************************************
//+--------------------------------------------------------------------------+**
//|                            ****                                          |**
//|                            ****                                          |**
//|                            ******o***                                    |**
//|                      ********_///_****                                   |**
//|                      ***** /_//_/ ****                                   |**
//|                       ** ** (__/ ****                                    |**
//|                           *********                                      |**
//|                            ****                                          |**
//|                            ***                                           |**
//|                                                                          |**
//|         Copyright (c) 1998-2010 Texas Instruments Incorporated           |**
//|                        ALL RIGHTS RESERVED                               |**
//|                                                                          |**
//| Permission is hereby granted to licensees of Texas Instruments           |**
//| Incorporated (TI) products to use this computer program for the sole     |**
//| purpose of implementing a licensee product based on TI products.         |**
//| No other rights to reproduce, use, or disseminate this computer          |**
//| program, whether in part or in whole, are granted.                       |**
//|                                                                          |**
//| TI makes no representation or warranties with respect to the             |**
//| performance of this computer program, and specifically disclaims         |**
//| any responsibility for any damages, special or consequential,            |**
//| connected with the use of this program.                                  |**
//|                                                                          |**
//+--------------------------------------------------------------------------+**
//*****************************************************************************/

// file:   suart_pru_emu.p
//
// brief:  Soft UART emulation using PRU/McASP0>
//
//
//  (C) Copyright 2010, Texas Instruments, Inc
//
//  author     Jitendra
//
//  version    0.1     Created
//

.origin 0
.entrypoint SUART_EMULATION

#include "PRU_SUART_Emulation.hp"

SUART_EMULATION:

	// Clear the ZERO Register r20
	XOR    pZERO, pZERO, pZERO
        
//--------------------- McASP TX Initialization - Starts ----------------
//activate Clocks,Serializers,state machine and frame sync
tx_asp_init1:
    //Activate the high-transmit clock XHCLKRST
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4
    SET       scratch_reg1, BIT_9  
    SBCO   	  scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4

tx_asp_init2:
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4
    QBBC      tx_asp_init2, scratch_reg1, BIT_9

    //Activate the transmit frequency clock XCLKRST
    SET       scratch_reg1, BIT_8 
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4

tx_asp_init3: 
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4
    QBBC      tx_asp_init3, scratch_reg1, BIT_8

    //Before starting, clear the respective transmitter and receiver status registers by writing 0xffff
    MOV32     scratch_reg2, 0xffff
    SBCO      scratch_reg2, MCASP_CONTROL, MCASP_XSTAT, 2
 
    // Activate serializer, XSRCLR 
    SET       scratch_reg1, BIT_10
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4

tx_asp_init4:
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4
    QBBC      tx_asp_init4, scratch_reg1, BIT_10

    // Till now no serializer is activated for TX, so no need to service all active XBUF
    // to avoid underrun errors to be done

    // Actiavte the McASP state machine
    SET       scratch_reg1, BIT_11
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4

tx_asp_init5: 
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4
    QBBC      tx_asp_init5, scratch_reg1, BIT_11

    // Activate the MCASP Frame sync
    SET       scratch_reg1, BIT_12
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4

tx_asp_init6: 
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_XGBLCTL, 4
    QBBC      tx_asp_init6, scratch_reg1, BIT_12
	
//----------------------- McASP TX Initialization - Ends ------------------

//--------------------- McASP RX Initialization - Starts ----------------

//activate Clocks,Serializers,state machine and frame sync
rx_asp_init1:
    //Activate the high-transmit clock RHCLKRST
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
    SET       scratch_reg1, BIT_1  
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4

rx_asp_init2:
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
    QBBC      rx_asp_init2, scratch_reg1, BIT_1

    //Activate the transmit frequency clock RCLKRST
    SET       scratch_reg1, BIT_0 
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4

rx_asp_init3: 
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
    QBBC      rx_asp_init3, scratch_reg1, BIT_0

	//Clear RSTAT
    LDI       scratch_reg2, 0xffff			
    SBCO      scratch_reg2, MCASP_CONTROL, MCASP_RSTAT, 2

    // Activate serializer, RSRCLR 
    SET       scratch_reg1, BIT_2
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
 
rx_asp_init4:
   LBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
   QBBC      rx_asp_init4, scratch_reg1, BIT_2

    // Actiavte the McASP state machine
    SET       scratch_reg1, BIT_3
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4

rx_asp_init5: 
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
    QBBC      rx_asp_init5, scratch_reg1, BIT_3

    // Activate the MCASP Frame sync
    SET       scratch_reg1, BIT_4
    SBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4

rx_asp_init6: 
    LBCO      scratch_reg1, MCASP_CONTROL, MCASP_RGBLCTL, 4
    QBBC      rx_asp_init6, scratch_reg1, BIT_4			

//----------------------- McASP RX Initialization - Ends ------------------

LOCAL_INIT:
	//Read the PRU ID
	LBBO	scratch_reg2, pZERO, SUART_GBL_PRU_STATUS_ADDR, 4
	MOV 	suart_global.pru_id, scratch_reg2.b0			
	
	// Read the PRU mode
	MOV		suart_global.pru_rx_tx_mode, scratch_reg2.b1

        //PRU Delay Count in CORE_LOOP
        MOV     suart_global.pru_delay_cnt, scratch_reg2.b2
	
	//Clear RSTAT
	LDI     scratch_reg2, 0xffff		
   	SBCO    scratch_reg2, MCASP_CONTROL, MCASP_RSTAT, 4	

	//Clear XSTAT
	LDI     scratch_reg2, 0xffff		
   	SBCO    scratch_reg2, MCASP_CONTROL, MCASP_XSTAT, 4	

	QBEQ   CORE_LOOP, suart_global.pru_rx_tx_mode, PRU_MODE_TX_ONLY	
	
        // This Block the Sampling Point with invalid value in RX Context Area
        LDI    scratch_reg2, 0xFF
	XOR    scratch_reg3, scratch_reg3, scratch_reg3
	
	QBEQ   PRUxxxx_MODE_RX_ONLY, suart_global.pru_rx_tx_mode, PRU_MODE_RX_ONLY	
	
        LDI    scratch_reg1, SUART_CH1_RX_CONTEXT_ADDR 
	LDI    scratch_reg4, RX_CONTEXT_OFFSET 
	LDI    scratch_reg3, 0x1B0
	JMP	   INIT_SAMPLE_PNT	

PRUxxxx_MODE_RX_ONLY:	
	LDI    scratch_reg1, 0x90 
	LDI    scratch_reg4, 0x20
        LDI    scratch_reg3, 0x170	
	
INIT_SAMPLE_PNT:
        SBBO   scratch_reg2, scratch_reg1, SUART_CH_SAMPLING_BIT_POS_OFFSET, 1
	SBBO   pZERO.b0, scratch_reg1, SUART_CH_FALSE_START_FLAG_OFFSET, 1
	
        ADD    scratch_reg1, scratch_reg1, scratch_reg4
        QBGE   INIT_SAMPLE_PNT, scratch_reg1, scratch_reg3


	// JUMP  to CORE_LOOP
	JMP    CORE_LOOP 

//=====================================================================================================================================

//			************************************************************************************************
//									SOFT-UART TRANSMIT ROUTINE : STARTS
//			************************************************************************************************

//=====================================================================================================================================		

//********************************************  TxServiceRequestHndlr Starts *********************************

//=====================================================================================================================================
// This routine perform the baisc intialization and clearing of varoius registers for TX and loads the configuration info
// from PRU RAM to register for serviced TX channel.  It calculates and saves serializer and x_buf address mapped
// to TX channel to TX context area so that each time it is not calculated again and is being directly read from TX Context Area.
//=====================================================================================================================================

TxServiceRequestHndlr:	
	//read interrupt status regsiter
        LBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2	
		
	// clear the channel interrupt status bit
	CLR	    suart_global.intrStatus, suart_global.intrStatus, suart_ch_info.ch_num

	//update interrupt status regsiter
   	SBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2   

	//Clear Service Request
	CLR  	suart_ch_regs.Chn_Cntrl, SUART_CTRL_SR_BIT // initialized by driver  
	SBBO 	suart_ch_regs.Chn_Cntrl,suart_ch_info.curr_ch_base_addr, SUART_CH_CTRL_OFFSET, SIZE (suart_ch_regs.Chn_Cntrl)
	
	// Set the TXRX_READY_BIT
	SET  	suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_READY_BIT  //(Initialized by driver)  
	SBBO 	suart_ch_regs.Chn_TxRxStatus, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXSTATUS_OFFSET, SIZE (suart_ch_regs.Chn_TxRxStatus)
    
        // Set SUART_CH_TXRXCHNSTATUS_BIT bit in channel status to indicate the channel active
        SET     suart_ch_regs.Chn_Status, SUART_CH_TXRXCHNSTATUS_BIT
        SBBO 	suart_ch_regs.Chn_Status, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXCHNSTATUS_OFFSET, SIZE (suart_ch_regs.Chn_Status)
	
	// New Tx Request received initilialize the Channel specific data and save it in memroy
	XOR 	suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBYTESDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBytesDoneCtr)
		
        // Load channel specific serializer, xbuf, srctl register mapped active channel	
	JMP 	LOAD_TX_COMMON_INFO

ENABLE_TX_SERIALIZER:
        //Change the MCASP AXR[n] pin from GPIO mode to MCASP mode of operation
        LBCO    scratch_reg2, MCASP_CONTROL, MCASP_PFUNC, 4
        AND     scratch_reg1, suart_ch_regs.Chn_Cntrl.b1, SUART_CTRL_SERIALIZER_MASK
        CLR     scratch_reg2, scratch_reg1
        SBCO    scratch_reg2, MCASP_CONTROL, MCASP_PFUNC, 4

CLEAR_XSTAT:
        LDI     scratch_reg1, 0xFFFF
        SBCO    scratch_reg1, MCASP_CONTROL, MCASP_XSTAT, 4
        JMP     MCASP_EVENT

//******************************************** TxServiceRequestHndlr Ends ************************************

//=====================================================================================================================================

//******************************************** TxServiceReqHndlLoop Starts ***********************************		

//=====================================================================================================================================
//  This routine reads the formated data to be transmitted from formatted data area region mapped to
//  current serviced TX channel and depending upon prescalar value in config1 register, it jumps to that 
// 	that prescalar label. This is getting called from TX interrupt handler or when is there new service request for TX.
//=====================================================================================================================================		

TxServiceReqHndlLoop:
	// Read the Formated byte to transmitted
	CALL	READ_TX_DATA	 

	XOR 	TX_DATA_reg.w0,TX_DATA_reg.w0,TX_DATA_reg.w0
	
        // Branch According to Pre-Scalar Value
	MOV		scratch_reg1, SUART_CTRL_PRE_SCALAR_MASK
	AND 	scratch_reg1, scratch_reg1, suart_ch_regs.Chn_Config1
	
   	QBEQ 	PRE_SCALAR1, scratch_reg1, PRE_SCALAR_1
        QBEQ 	PRE_SCALAR2, scratch_reg1, PRE_SCALAR_2
	QBEQ 	PRE_SCALAR4, scratch_reg1, PRE_SCALAR_4
	QBEQ 	PRE_SCALAR6, scratch_reg1, PRE_SCALAR_6
	QBEQ 	PRE_SCALAR12, scratch_reg1, PRE_SCALAR_12
	QBEQ 	PRE_SCALAR16, scratch_reg1, PRE_SCALAR_16
	QBLE 	PRE_SCALAR24, scratch_reg1, PRE_SCALAR_24
	
//******************************************** TxServiceReqHndlLoop ENDS *************************************

//======================================================================================================================================	

//******************************************** PRE_SCALAR1 Starts ********************************************
PRE_SCALAR1:
	// copy data to RAM TX_DATA_reg.w0 register from scratch_reg3
	MOV		TX_DATA_reg.w0, scratch_reg3
	
	CALL    TRANSMIT_PRESCALED_DATA
	
	// Increament the Chn_TxRxBytesDoneCtr bye one
	ADD 	suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr, 1
	SBBO 	suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBYTESDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBytesDoneCtr)
	
	JMP     TxInterruptServiceRequestHndlr
//******************************************** PRE_SCALAR1 Ends **********************************************

//======================================================================================================================================

//******************************************** PRE_SCALAR2 Starts ********************************************
PRE_SCALAR2:
	MOV 	scratch_reg1, suart_ch_regs.Chn_TxRxBitsDoneCtr
	QBGT	XMIT_FISRT_8BIT, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	
XMIT_LAST_8BIT:
	LSR     scratch_reg3, scratch_reg3, 8  //Last 8 bits to transmitted
	
	CALL    PRESACLE_TX_DATA
	
	CALL 	TRANSMIT_PRESCALED_DATA
	
	JMP     TX_DONE
	
XMIT_FISRT_8BIT:
	AND 	scratch_reg3, scratch_reg3, 0x00FF
	CALL	PRESACLE_TX_DATA

	CALL    TRANSMIT_PRESCALED_DATA
	
	// Write To RAM number of Bits Transmitted
	ADD		suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 8  	//8 bits transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	// If bit per character less than 8  // added with start and stop bit in bits per channel
	MOV     scratch_reg1, suart_ch_regs.Chn_Config2
	AND     scratch_reg1, scratch_reg1, 0xF
	QBGE  	TX_DONE, scratch_reg1, 0x8    //check  (Chn_Config2.BitsPerChar <= 8) 
	JMP     TxInterruptServiceRequestHndlr
//******************************************** PRE_SCALAR2 ENDs **********************************************	

//======================================================================================================================================

//******************************************** PRE_SCALAR4 Starts ********************************************
PRE_SCALAR4:

	MOV 	scratch_reg1, suart_ch_regs.Chn_TxRxBitsDoneCtr
	QBGT 	XMIT_FIRST_4BIT, scratch_reg1, 1
	
XMIT_NXT_4BIT:      
	AND     scratch_reg2, suart_ch_regs.Chn_Config2, 0xF
	SUB     scratch_reg2, scratch_reg2, suart_ch_regs.Chn_TxRxBitsDoneCtr    //Chn_Config2.BitsPerChar - Chn_TxRxBitsDoneCntr 
	
	QBLT    MORE_DATA4, scratch_reg2, 4   //(Chn_Config2.BitsPerChar - Chn_TxRxBitsDoneCntr) > 4, more bits to be transmitted
	
	//transmit last remaining 4 bits 
	LSR		scratch_reg3, scratch_reg3, scratch_reg1
	AND		scratch_reg3, scratch_reg3, 0xF
	
	CALL	PRESACLE_TX_DATA
        CALL    TRANSMIT_PRESCALED_DATA
	
	JMP 	CHK_TX_DONE
	
MORE_DATA4:			//transmit next 4 bit of present byte being transmitted
	LSR		scratch_reg3, scratch_reg3, scratch_reg1
	AND		scratch_reg3, scratch_reg3, 0xF
	
	CALL	PRESACLE_TX_DATA

 	CALL    TRANSMIT_PRESCALED_DATA	
	
	// Check all bits have been transmitted
CHK_TX_DONE:    
	//Updating number of bits written
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 4   
	
	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	AND 	scratch_reg2, suart_ch_regs.Chn_Config2, 0xF

	// check if all bits have been trasmitted
	QBGE    TX_DONE, scratch_reg2, suart_ch_regs.Chn_TxRxBitsDoneCtr
	JMP     TxInterruptServiceRequestHndlr

XMIT_FIRST_4BIT:   // transmit first 4 bit of formated data
	AND		scratch_reg3, scratch_reg3, 0xF
	CALL	PRESACLE_TX_DATA
   	CALL    TRANSMIT_PRESCALED_DATA
	
	//Updating number of bits written
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 4    
	
	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	JMP     TxInterruptServiceRequestHndlr
//******************************************** PRE_SCALAR4 Ends **********************************************	
	
//======================================================================================================================================
	
//******************************************** PRE_SCALAR6 Starts ********************************************		
PRE_SCALAR6:
	QBGT 	XMIT_FIRST_3BIT, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1   // transmit first 3 bit of formated data	
	
GENERIC_TRANSMIT_BLOCK:
	//initialize the register
	MOV     tx_context.bitsLoaded, 0x0  
	XOR     scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2

LOAD_BITS_LOOP_FOR6:        
	AND 	scratch_reg2, suart_ch_regs.Chn_Config2, 0xF
	
	QBLT 	XMIT_NXT_3BIT, scratch_reg2, suart_ch_regs.Chn_TxRxBitsDoneCtr    // transmit the next bits if (ChnTxRxBitsDoneCntr < Chn_Config2.BitsPerChar)

	// transmit the last remaining bits of the present byte if any and updated counters as below	
XMIT_MORE_BITS: 
	// update the bytes done counter and reset the Chn_TxRxBitsDoneCtr and Chn_TxRxRepeatDoneCtr
	ADD 	suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBYTESDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBytesDoneCtr)
	
	XOR	    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	XOR	    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	
	// set the remaining bits to one, if there are no more bits in formated data to send 
	// and still there is space in TX_DATA_reg.
	RSB     scratch_reg1, tx_context.bitsLoaded, 0x10	   	//16 - bitsLoaded
	XOR     scratch_reg3, scratch_reg3, scratch_reg3		// Load the remaining (16 - bitsLoaded) bits with logic High
	NOT   	scratch_reg3, scratch_reg3

	RSB 	scratch_8bit_reg2, scratch_reg1, 16			//calculate the bit position from where one is to be inserted
	XOR 	scratch_reg2, scratch_reg2, scratch_reg2		//CLR scratch_reg2
	AND     scratch_reg2, scratch_reg3,0x1				// COPY 1 bit to scratch_reg2 from scratch_reg3
	LSL 	scratch_reg2, scratch_reg2, scratch_8bit_reg2
	
	// Now, set the remaining bits to one in TX_DATA_reg
SET_BIT_BIT:
	OR      TX_DATA_reg.w0, TX_DATA_reg.w0, scratch_reg2
	LSL 	scratch_reg2, scratch_reg2, 1
	SUB     scratch_reg1, scratch_reg1, 1
	QBLE    SET_BIT_BIT, scratch_reg1, 1

   	MOV 	tx_context.bitsLoaded, 16
	JMP	    CHK_MORE_PRESCALAR
	JMP     TxInterruptServiceRequestHndlr

XMIT_NXT_3BIT:
	//if the bitsLoaded in TX_DATA_reg is less than 16 load the next bits  
	QBLT 	CHK_MORE_PRESCALAR, tx_context.bitsLoaded, 0x10  //(bitsLoaded < 16) 

BIT_LOAD16:
	// Read Prescalar value
	MOV    	scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK
	AND    	scratch_reg1, scratch_reg2, suart_ch_regs.Chn_Config1
	
	RSB	    scratch_reg2, tx_context.bitsLoaded, 16    //(16 - bitsLoaded)
	SUB	    scratch_reg1, scratch_reg1, suart_ch_regs.Chn_TxRxRepeatDoneCtr //(Chn_Config1.PreScaller - ChnTxRxRepeatDoneCntr)
	
	MIN    	scratch_reg1, scratch_reg1, scratch_reg2  // repeat count = scratch_reg1, min of 

	// Read Next Bit
	CALL	READ_TX_DATA	
	LSR 	scratch_reg3, scratch_reg3, suart_ch_regs.Chn_TxRxBitsDoneCtr

	AND 	scratch_reg2, scratch_reg3, 0x1		// copy bit to transmitted to scratch_reg2
	MOV     scratch_reg4, scratch_reg1         	// move repeat count to scratch_reg4
	LSL 	scratch_reg2, scratch_reg2, tx_context.bitsLoaded   // shift the bit to be transmitted to expected position 

	// prescale the bit to transmitted	
PRESCALE_NXT_BIT:
	OR      TX_DATA_reg.w0, TX_DATA_reg.w0, scratch_reg2
	LSL 	scratch_reg2, scratch_reg2, 1
	SUB     scratch_reg1, scratch_reg1, 1
	QBLE    PRESCALE_NXT_BIT, scratch_reg1, 1
		
	// write back to memory
	ADD 	tx_context.bitsLoaded, tx_context.bitsLoaded, scratch_reg4 
	
	ADD 	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, scratch_reg4 
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	
	// get the prescalar value
	MOV    scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK
	AND    scratch_reg1, scratch_reg2, suart_ch_regs.Chn_Config1
	
	//if bit has been transmitted prescaler times, fall through and updated the Chn_TxRxBitsDoneCtr and Chn_TxRxRepeatDoneCtr
	QBGT    CHK_MORE_PRESCALAR, suart_ch_regs.Chn_TxRxRepeatDoneCtr, scratch_reg1   //if (Chn_Config1.PreScaller == ChnTxRxRepeatDoneCntr)
	
TX_DONE_CNTR6:  // rename to TX_BIT_DONE_CNTR
	// Write Chn_TxRxBitsDoneCtr
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	// Write Chn_TxRxRepeatDoneCtr
	XOR 	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	
CHK_MORE_PRESCALAR:   //rename this label to CHK_TX_DATA_REG_FULL
	QBGT 	LOAD_BITS_LOOP_FOR6, tx_context.bitsLoaded, 0x10  //if (bitsLoaded < 16), next bit can be loaded in TX_DATA_reg
	CALL 	TRANSMIT_PRESCALED_DATA				  // TX_DATA_reg is full, transmit the data
	JMP     TxInterruptServiceRequestHndlr
	
	// transmit the bits from start bit that can be trasmnitted from present character that is to transmitted
XMIT_FIRST_3BIT:
	AND 	scratch_reg3, scratch_reg3, 0x7			// copy the first 3 bits to be transmiited
	MOV 	scratch_reg2, 12        			//number of times the byte loop is to be looped
	XOR 	TX_DATA_reg.w0,TX_DATA_reg.w0,TX_DATA_reg.w0    //Clear TX_DATA_reg.w0
        XOR     scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2
	CALL	BYTE_LOOP

	MOV	    scratch_reg1, 0x4  				// Repeat last bit by 4 times
	
	XOR 	scratch_reg2, scratch_reg2, scratch_reg2	//CLR scratch_reg2
	AND     scratch_reg2, scratch_reg3, 0x1			//copy the third bit to scratch_reg2

	LSL 	scratch_reg2, scratch_reg2, 0xc      	//shift the bit to expected place i.e. bit ps 12	

	// prescale the last bit 4 times 
PRESCALE_LAST_4BIT:
	OR      TX_DATA_reg.w0, TX_DATA_reg.w0, scratch_reg2
	LSL 	scratch_reg2, scratch_reg2, 1
	SUB     scratch_reg1, scratch_reg1, 1
	QBLE    PRESCALE_LAST_4BIT, scratch_reg1, 1
	
	CALL    TRANSMIT_PRESCALED_DATA
 
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 2    //Updating number of bits written
	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	ADD 	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 4 //Updating number of bits written
	// Write To RAM Write Repeat done counter to RAM
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)

	JMP     TxInterruptServiceRequestHndlr
//******************************************** PRE_SCALAR6 ENDs **********************************************		

//======================================================================================================================================

//******************************************** PRE_SCALAR12 Starts *******************************************		
PRE_SCALAR12:
	QBGT 	XMIT_FIRST_2BIT, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	JMP 	GENERIC_TRANSMIT_BLOCK

XMIT_FIRST_2BIT:
	AND 	scratch_reg3, scratch_reg3, 0x3		// copy the first two bit to be loaded in  TX_DATA_reg
	MOV 	scratch_8bit_reg2, 0x0      	 	// To Left shif teach copied data bit
	MOV 	scratch_reg2, 12        		//Keep track of byte_loop loop count
	XOR     scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2	
	CALL	BYTE_LOOP
	
	XOR 	scratch_reg2, scratch_reg2, scratch_reg2		//CLR scratch_reg2
	AND     scratch_reg2, scratch_reg3, 0x1				//copy the next bit to prescaled
	MOV	    scratch_reg1, 4						//counter to prescale second bit by 4
	LSL 	scratch_reg2, scratch_reg2, 0xC				//shift the bit to desired position
PRESCALE_4BIT:
	OR      TX_DATA_reg.w0, TX_DATA_reg.w0, scratch_reg2
	LSL 	scratch_reg2, scratch_reg2, 1
	SUB     scratch_reg1, scratch_reg1, 1
	QBLE    PRESCALE_4BIT, scratch_reg1, 1
	
   	CALL 	TRANSMIT_PRESCALED_DATA	
	
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1    //Updating number of bits written
	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	ADD 	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 4 //Updating number of bits written
	// Write To RAM number of Bits Repeated
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)

	JMP     TxInterruptServiceRequestHndlr
//******************************************** PRE_SCALAR12 ENDs *********************************************			

//======================================================================================================================================

//******************************************** PRE_SCALAR16 Starts *******************************************		
PRE_SCALAR16:
	QBGT 	XMIT_FIRST_16, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	JMP 	GENERIC_TRANSMIT_BLOCK

XMIT_FIRST_16:
	AND 	scratch_reg3, scratch_reg3, 0x2		//// copy the first two bit to be loaded in  TX_DATA_reg
	MOV 	scratch_8bit_reg2, 0x0       		//Left shift each copied data bit
	MOV 	scratch_reg2, 16       			//Keep track of byte_loop loop count
	XOR 	TX_DATA_reg.w0,TX_DATA_reg.w0,TX_DATA_reg.w0    //Clear TX_DATA_reg.w0
        XOR     scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2	
	CALL	BYTE_LOOP
	
	CALL 	TRANSMIT_PRESCALED_DATA	
	
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1   //Updating number of bits written
	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	ADD 	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 0 //Updating number of bits written
	// Write To RAM number of Bits Repeated
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)

	JMP     TxInterruptServiceRequestHndlr
//******************************************** PRE_SCALAR16 ENDs *********************************************		

//======================================================================================================================================

//********************************************* PRE_SCALAR24 Starts ******************************************		
PRE_SCALAR24:
	QBGT 	XMIT_FIRST_24, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	JMP 	GENERIC_TRANSMIT_BLOCK
	
XMIT_FIRST_24:
	MOV    scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK
	AND    scratch_reg1, scratch_reg2, suart_ch_regs.Chn_Config1	
	SUB    scratch_reg1, scratch_reg1, suart_ch_regs.Chn_TxRxRepeatDoneCtr //Chn_TxRxConfig1.PreScaler - ChnTxRxRepeadDoneCnt
	QBLE 	PRESCALE_START_BIT, scratch_reg1, 16		//(Chn_TxRxConfig1.PreScaler - ChnTxRxRepeadDoneCntr >= 16 )
	
PRESCALE_FIRST_DATA_BIT:

	XOR 	scratch_reg3, scratch_reg3, scratch_reg3 	//Clear Scratch reg	
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1   //Updating number of bits written
	
	CALL	READ_TX_DATA	
	
	//get the bits to transmitted
	LSR     scratch_reg3, scratch_reg3, suart_ch_regs.Chn_TxRxBitsDoneCtr
	AND     scratch_reg2, scratch_reg3, 0x1
	
	//shif the bit to desired bit position 
	LSL     scratch_reg2, scratch_reg2, scratch_reg1 
	RSB 	scratch_reg1,  scratch_reg1, 16	
	MOV		suart_ch_regs.Chn_TxRxRepeatDoneCtr, scratch_reg1
	
PRESCALE_FIRST_DAT_BIT:
	OR      TX_DATA_reg.w0, TX_DATA_reg.w0, scratch_reg2
	LSL 	scratch_reg2, scratch_reg2, 1
	SUB     scratch_reg1, scratch_reg1, 1
	QBLE    PRESCALE_FIRST_DAT_BIT, scratch_reg1, 1
	
	
	CALL 	TRANSMIT_PRESCALED_DATA	

	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	// Write To RAM Chn_TxRxRepeatDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	JMP     TxInterruptServiceRequestHndlr
	
PRESCALE_START_BIT:
	MOV 	scratch_reg1, 0x10
	MOV 	scratch_reg2, 0x10
	XOR     scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2       //to Left shif teach copied data bit
	CALL 	BITS_LOOP
	CALL 	TRANSMIT_PRESCALED_DATA	
	
	ADD 	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 16 // Update number of bits written
	// Write To RAM number of Bits Repeated
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	JMP     TxInterruptServiceRequestHndlr
	
//************************************************ PRE_SCALAR24 ENDs *****************************************

//======================================================================================================================================

//======================================================================================================================================

//			************************************************************************************************
//												TX SUB ROUTINE Section 
//			**************************************************************************************************
//======================================================================================================================================

//======================================================================================================================================


//********************************************** LOAD_TX_COMMON_INFO:  Start *********************************

//======================================================================================================================================
// 	Loads The X_BUF, SRCTL address mapped with present TX Channel and format data address which stores formatted data 
// 	obtained from ARM/DSP specific to channel, also formats the data if it is required  
//======================================================================================================================================

LOAD_TX_COMMON_INFO:
	// Load the TX Format Address for the specific channel
	QBEQ    LOAD_TX_FORMAT_ADDRESS_DONE, suart_global.pru_rx_tx_mode, PRU_MODE_TX_ONLY	
	CALL  	LOAD_TX_FORMAT_ADDRESS
	
LOAD_TX_FORMAT_ADDRESS_DONE:
	//  Load the mapped SR and XBUF address mapped to channel
	JMP 	LOCATE_SR_XBUF_SRCTL	
	
LOCATE_SR_XBUF_SRCTL_DONE:
	// Format the data if required
	JMP    	CHK_TX_DATA_FORMAT

CHK_TX_DATA_FORMAT_DONE:
	JMP     ENABLE_TX_SERIALIZER

//****************************************** LOAD_TX_COMMON_INFO: ENDS ***************************************
	
//======================================================================================================================================	
	
//****************************************** TX LOCATE_SR_XBUF_SRCTL : Starts ********************************

//======================================================================================================================================
// 		Subroutine to find channel specific serializer, xbuf, srctl register mapped active channel
//======================================================================================================================================
LOCATE_SR_XBUF_SRCTL:
    	// Calculating Seralizer Mapped to Channel
	AND 	scratch_reg1, suart_ch_regs.Chn_Cntrl.b1, SUART_CTRL_SERIALIZER_MASK  
	LSL 	scratch_reg1, scratch_reg1, SUART_CTRL_SRCTL_BIT_SHIFT
	
	// copy the tx format address to temp regsiter
	MOV   	scratch_reg3, tx_context.buff_addr
	MOV     scratch_reg4, TX_FMT_DATA_TO_TX_CONTEXT_OFFSET
		
	//Calculating the specific SRCTL register offset .
	MOV32  	tx_context.asp_xsrctl_reg, MCASP_SRCTL_BASE
	ADD 	tx_context.asp_xsrctl_reg, tx_context.asp_xsrctl_reg, scratch_reg1
	
	ADD     scratch_reg2, scratch_reg4, SUART_CH_ASP_XSRCTL_REG_OFFSET
	SBBO    tx_context.asp_xsrctl_reg, scratch_reg3, scratch_reg2, SIZE (tx_context.asp_xsrctl_reg)
	
	//Calculating the specific xbuf register offset .
	MOV32	tx_context.asp_xbuf_reg, MCASP_XBUF_BASE
	ADD	    tx_context.asp_xbuf_reg, tx_context.asp_xbuf_reg, scratch_reg1
	
	ADD     scratch_reg2, scratch_reg4, SUART_CH_ASP_XBUF_REG_OFFSET
	SBBO    tx_context.asp_xbuf_reg, scratch_reg3, scratch_reg2, SIZE (tx_context.asp_xbuf_reg)
	
	//Store the data length
	MOV 	tx_context.buff_size, suart_tx_ch.Chn_Config2.b1
	
	ADD     scratch_reg2, scratch_reg4, SUART_CH_BUFF_SIZE_OFFSET
	SBBO    tx_context.buff_size, scratch_reg3, scratch_reg2, SIZE (tx_context.buff_size)
	
	//Store the data Tx FMT Context address
	ADD     scratch_reg2, scratch_reg4, SUART_CH_BUFF_ADDR_OFFSET
	SBBO    tx_context.buff_addr, scratch_reg3, scratch_reg2, SIZE (tx_context.buff_addr)
		
	LDI 	tx_context.bitsLoaded, 0x00         
	ADD     scratch_reg2, scratch_reg4, SUART_CH_BITSLOADED_OFFSET
	SBBO    tx_context.bitsLoaded, scratch_reg3, scratch_reg2, SIZE (tx_context.bitsLoaded)
	
	JMP	LOCATE_SR_XBUF_SRCTL_DONE
//********************************************** TX LOCATE_SR_XBUF_SRCTL: ENDS **************************************	

//======================================================================================================================================

//********************************************** TX CHK_TX_DATA_FORMAT : Starts***************************************	

//======================================================================================================================================
// 	Check For Data Formating, formats the data only if Chn_TxRxRepeatDoneCtr, 
//	Chn_TxRxBitsDoneCtr, Chn_TxRxBytesDoneCtr all are zero, 
//  If All the conditions is satisfied, it jumps to TX_DATA_FORMAT Subroutine  and 
// 	formats data the TX Data obtained from ARM/DSP by adding start and stop bit.
//======================================================================================================================================
CHK_TX_DATA_FORMAT:
	QBEQ    CHK_TX_DATA_FORMAT_BITS, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 0
	JMP		CHK_TX_DATA_FORMAT_DONE
	
CHK_TX_DATA_FORMAT_BITS:
	QBEQ    CHK_TX_DATA_FORMAT_BYTE, suart_ch_regs.Chn_TxRxBitsDoneCtr, 0
	JMP		CHK_TX_DATA_FORMAT_DONE
	
CHK_TX_DATA_FORMAT_BYTE:
	QBEQ    TX_DATA_FORMAT, suart_ch_regs.Chn_TxRxBytesDoneCtr, 0
	JMP		CHK_TX_DATA_FORMAT_DONE
	
TX_DATA_FORMAT:
	// Load the TX Format Address for the specific channel
	XOR 	scratch_reg1, scratch_reg1, scratch_reg1
	NOT 	scratch_reg1, scratch_reg1
	
	SUB 	scratch_reg2, suart_tx_ch.Chn_Config2.b0, 1

	LSL	    scratch_reg1, scratch_reg1, scratch_reg2
	XOR 	scratch_reg2, scratch_reg2, scratch_reg2  // offset from base addr
	
	// to store formated data into DATA RAM
	MOV	    scratch_reg3, tx_context.buff_addr
	
	//Number of Bits Per Character
	AND  	scratch_8bit_reg1, suart_ch_regs.Chn_Config2.b0, 0xF
   	SUB	    scratch_8bit_reg1, scratch_8bit_reg1, 2
	
TX_DATA_FORMAT_LOOP:
	//Load the data from the data pointer
	LD16	TX_DATA_reg, suart_tx_ch.ch_TXRXData
	LSL	    TX_DATA_reg, TX_DATA_reg, 1
	OR	    TX_DATA_reg, TX_DATA_reg, scratch_reg1
	
	//store formated data into DATA RAM
	SBBO	TX_DATA_reg.w0, scratch_reg3, scratch_reg2, 2 

	//Increment the formatted buffer address offset
	ADD 	scratch_reg2, scratch_reg2, 2

	
	QBGE  	INC_ADDR_BY_ONE, scratch_8bit_reg1, SUART_CH_CONFIG2_8BITS_PER_CHAR //0x8
	//Next data buffer pointer 
	ADD		suart_tx_ch.ch_TXRXData, suart_tx_ch.ch_TXRXData, 1
	
// Increamnet the tx buffer data pointer by ONE, if bit per character is less or equal to 8 including start and stop bit
INC_ADDR_BY_ONE:
	//Next data buffer pointer 
	ADD	    suart_tx_ch.ch_TXRXData, suart_tx_ch.ch_TXRXData, 1

	QBEQ	CHK_TX_DATA_FORMAT_DONE, tx_context.buff_size, 0

	//Decrement the data length .i.e no of bytes to send
	SUB	    tx_context.buff_size, tx_context.buff_size, 1

	JMP		TX_DATA_FORMAT_LOOP
	
//******************************************** TX CHK_TX_DATA_FORMAT: ENDS************************************

//======================================================================================================================================

//******************************************** TX READ_TX_DATA: Starts****************************************

//======================================================================================================================================
// 	Reads the 16 bit formatted character to be trasnsmitted from formatted data area corresponding to TX channel
//======================================================================================================================================

READ_TX_DATA:
	// Copy the base address of formated data
	MOV	    scratch_reg3, tx_context.buff_addr
	
	// Calculate the offset of formated data
	LSL 	scratch_reg4, suart_ch_regs.Chn_TxRxBytesDoneCtr, 1       
	
	//LOAD formated data from DATA RAM
	LBBO 	scratch_reg3, scratch_reg3, scratch_reg4, 2
.ret

//********************************************** TX READ_TX_DATA: ENDS ***************************************

//======================================================================================================================================

//********************************************** TX LOAD_TX_FORMAT_ADDRESS : Starts **************************

//======================================================================================================================================
//	Initializes the TX formatted data buffer address which stores the formated data with stop and start bit
//======================================================================================================================================

LOAD_TX_FORMAT_ADDRESS:
	QBEQ	TX_CH0_FMT_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_0
	QBEQ	TX_CH2_FMT_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_2
	QBEQ	TX_CH4_FMT_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_4
	QBEQ	TX_CH6_FMT_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_6
.ret

TX_CH0_FMT_ADDR:	
	LDI 	tx_context.buff_addr, SUART_CH0_TX_FMT_ADDR  
.ret


TX_CH2_FMT_ADDR:	
	LDI 	tx_context.buff_addr, SUART_CH2_TX_FMT_ADDR  
.ret


TX_CH4_FMT_ADDR:	
	LDI 	tx_context.buff_addr, SUART_CH4_TX_FMT_ADDR  
.ret


TX_CH6_FMT_ADDR:	
	LDI 	tx_context.buff_addr, SUART_CH6_TX_FMT_ADDR  
.ret
//******************************************** TX LOAD_TX_FORMAT_ADDRESS Routine: ENDS ***********************

//======================================================================================================================================

//******************************************** PRESACLE_TX_DATA : Starts *************************************

//======================================================================================================================================
// This routine Prescales data bit to be transmitted into the TX_DATA_reg.w0 register
//======================================================================================================================================	

PRESACLE_TX_DATA:
	XOR 	scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2       //to Left shif teach copied data bit
	MOV 	scratch_reg2, 16        						//Xbuf Size: Keep track of byte loop
	XOR 	TX_DATA_reg.w0,TX_DATA_reg.w0,TX_DATA_reg.w0    //Clear TX_DATA_reg.w0
	
BYTE_LOOP:
	MOV 	scratch_reg4, SUART_CTRL_PRE_SCALAR_MASK
	AND		scratch_reg1, scratch_reg4, suart_ch_regs.Chn_Config1
	
BITS_LOOP:
	XOR 	scratch_reg4, scratch_reg4, scratch_reg4			//CLR scratch_reg4
	AND     scratch_reg4, scratch_reg3,0x1					// COPY 1 bit to scratch_reg4 from scratch_reg3
	LSL 	scratch_reg4, scratch_reg4, scratch_8bit_reg2
	OR      TX_DATA_reg.w0, TX_DATA_reg.w0, scratch_reg4
	ADD	    scratch_8bit_reg2, scratch_8bit_reg2, 1
	SUB     scratch_reg2, scratch_reg2, 1  				//INC Bytes loop counter
	SUB     scratch_reg1, scratch_reg1, 1			//INC Bits loop counter
	QBLE    BITS_LOOP, scratch_reg1, 1
	LSR	    scratch_reg3, scratch_reg3, 1
	QBLE    BYTE_LOOP, scratch_reg2, 1
.ret  // Return to PRESACLE_TX_DATA

//********************************************  PRESACLE_TX_DATA : ENDs **************************************

//======================================================================================================================================

//******************************************** TRANSMIT_PRESCALED_DATA : Starts ******************************

//======================================================================================================================================
// This routine Transmits the prescaled data by writing to mcasp x_buf register maaped to this serializer
//======================================================================================================================================	
 
TRANSMIT_PRESCALED_DATA:
	// Clear the under run error
	LBCO	scratch_reg1, MCASP_CONTROL, MCASP_XSTAT, 4
	QBBC	WRITE_TO_XBUF, scratch_reg1, 0x8
	
	LDI		scratch_reg1, 0xFFFF
	SBCO	scratch_reg1, MCASP_CONTROL, MCASP_XSTAT, 4

WRITE_TO_XBUF:
	// Write Byte to X_BUF 
	SBBO  	TX_DATA_reg.w0, tx_context.asp_xbuf_reg, #00, 4
.ret   // return from Transmit Prescaled Data	

//******************************************** TRANSMIT_PRESCALED_DATA : ENDs ********************************

//======================================================================================================================================	

//******************************************** TX_DONE : Starts **********************************************

//======================================================================================================================================	
// This routine the cleanup after one character has been transmitted successfully
//======================================================================================================================================	

TX_DONE:
	XOR    	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr
	// Write To RAM number of Bits Transmitted
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	ADD 	suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBYTESDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBytesDoneCtr)

	JMP     TxInterruptServiceRequestHndlr
	
//******************************************** TX_DONE : ENDs ************************************************

//======================================================================================================================================

//******************************************** TxInterruptServiceRequestHndlr: Starts ************************

//=======================================================================================================================================
// This routine is called when there is mcasp TX event, it searches for the active TX serializer by scanning the 
// serializer registers and if detected, finds the corresponding Tx channel and finally loads the context info for the 
// channel, otherwise, if it fails any condition jumps to core loop.
// This routine also causes the PRU to ARM interrupt to be raised if transfer of requested character is finished other wise
// call the routine TxServiceReqHndlLoop to transfer remaining  characters.
// It also, causes the dummy transfer when there is no character to send
//=======================================================================================================================================

TxInterruptServiceRequestHndlr:
	//Retrieve the channel number and load the context base;
	MOV 	suart_ch_info.curr_ch_base_addr, SUART_CH_BASE_ADDRESS 
	MOV  	suart_ch_info.curr_ch_offset, SUART_CH_BASE_ADDRESS
	MOV	suart_ch_info.ch_num, 0x00
	MOV     tx_context.buff_addr, 0x90
	
SERCH_MAPPED_TX_CHN:
        ADD     scratch_reg1, suart_ch_info.curr_ch_offset, SUART_CH_TXRXCHNSTATUS_OFFSET
        LBBO    suart_ch_regs.Chn_Status, suart_ch_info.curr_ch_base_addr, scratch_reg1, SIZE (suart_ch_regs.Chn_Status)  //Load the Channel Cntrl info from Memory to Register
        QBBC    NEXT_TX_CHN, suart_ch_regs.Chn_Status, SUART_CH_TXRXCHNSTATUS_BIT

        ADD     scratch_reg1, tx_context.buff_addr, TX_FMT_DATA_TO_TX_CONTEXT_OFFSET
        LBBO    scratch_reg2, scratch_reg1, SUART_CH_ASP_XSRCTL_REG_OFFSET, 4
        LBBO    scratch_reg1, scratch_reg2, #00, 4
        QBBS    MAPPED_TX_CHN_FOUND, scratch_reg1, ASP_SRCTL_XRDY_BIT

NEXT_TX_CHN:	
        QBEQ    PRU_TX_ONLY_MODE, suart_global.pru_rx_tx_mode, PRU_MODE_TX_ONLY
    
        //TX & RX together. So channel nunbers are 0, 2, 4, 6
        ADD    suart_ch_info.curr_ch_offset, suart_ch_info.curr_ch_offset, SUART_CH_REGS_SIZE2
        ADD    suart_ch_info.ch_num, suart_ch_info.ch_num, 0x02
        ADD    tx_context.buff_addr, tx_context.buff_addr, SUART_TX_FMT_OFFSET
        QBGE   SERCH_MAPPED_TX_CHN, suart_ch_info.ch_num, NUM_OF_CHANNELS
        JMP    CORE_LOOP

PRU_TX_ONLY_MODE:
        //TX Only ...So channel nunbers are contigous
        ADD    suart_ch_info.curr_ch_offset, suart_ch_info.curr_ch_offset, SUART_CH_REGS_SIZE
        ADD    suart_ch_info.ch_num, suart_ch_info.ch_num, 0x01
        ADD	   tx_context.buff_addr, tx_context.buff_addr, 0x2C
        QBGE   SERCH_MAPPED_TX_CHN, suart_ch_info.ch_num, NUM_OF_CHANNELS
        JMP    CORE_LOOP
	
MAPPED_TX_CHN_FOUND:

	LBBO 	suart_ch_regs, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset, SIZE (suart_ch_regs) 
        ADD     suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset

	QBEQ     PRUx_MODE_TX_ONLY, suart_global.pru_rx_tx_mode, PRU_MODE_TX_ONLY
	
	QBEQ     CORE_LOOP, suart_global.pru_rx_tx_mode, PRU_MODE_INVALID
	
PRUx_MODE_TX_ONLY:	
	ADD     scratch_reg1, tx_context.buff_addr, TX_FMT_DATA_TO_TX_CONTEXT_OFFSET
	LBBO    tx_context,   scratch_reg1, #00, SIZE (tx_context) 
	
	// JMP TO TxServiceReqHndlLoop Chn_TxRxBytesDoneCtr is less than Data length
	LSR 	scratch_reg1, suart_ch_regs.Chn_Config2, SUART_CH_CONFIG2_DATALEN_SHIFT // 0x8
	AND	    scratch_reg1, scratch_reg1, SUART_CH_CONFIG2_DATALEN_MASK	//0x0F
	ADD	    scratch_reg1, scratch_reg1, 0x01
	QBLT 	TxServiceReqHndlLoop, scratch_reg1, suart_ch_regs.Chn_TxRxBytesDoneCtr
	
	QBBS    DECLARE_COMPLETE, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_READY_BIT
	NOT	    scratch_reg1, pZERO   
  	SBBO    scratch_reg1, tx_context.asp_xbuf_reg, 00, 4
	
	JMP     CORE_LOOP	
DECLARE_COMPLETE:	

	// Set the status in the context area
	SET    suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_COMPLETE_BIT
   	CLR    suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_READY_BIT
        SBBO   suart_ch_regs.Chn_TxRxStatus, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXSTATUS_OFFSET, SIZE (suart_ch_regs.Chn_TxRxStatus)
	
	// Generate the interrupt to the ARM/DSP about the completion
	LBBO    suart_global.intrMask, pZERO, SUART_GBL_INT_MASK_ADDR, 2	
        QBBC    CORE_LOOP, suart_global.intrMask, suart_ch_info.ch_num
	
	LBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2		
	SET	suart_global.intrStatus, suart_global.intrStatus, suart_ch_info.ch_num
	SBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2       
	JMP     PRU_TO_HOST_INTERRUPT			

//******************************************** TxInterruptServiceRequestHndlr: ENDs **************************	

//======================================================================================================================================

//			************************************************************************************************
//									SOFT-UART TRANSMIT ROUTINE : ENDS
//			************************************************************************************************
	
//======================================================================================================================================

//======================================================================================================================================

//******************************************** CORE LOOP: Starts *********************************************    

//========================================================================================================================================
//  CORE LOOP where the PRU polls for System Interrupts i.e ARM_DSP_EVENT and MCASP_EVENT
//  when there is ARM_DSP_EVENT DSP event it jumps to Channel search to acknowledge the service request from ARM/DSP,
// otherwise if MCASP_EVENT (TX/RX) occurs it goest to corresponding interrupt handler (TX/RX) to service the interrupt
// if there is no event, it loops in the core loop and waits for interrupt form ARM or DSP.
//========================================================================================================================================

CORE_LOOP:
	QBEQ    CORE_LOOP, suart_global.pru_rx_tx_mode, PRU_MODE_INVALID
	
ARM_DSP_EVENT:
	QBEQ    CORE_LOOP_PRU1, suart_global.pru_id, 1
	
CORE_LOOP_PRU0:	
        // wait for the hostEventStatus to get set. Loop till then
        WBS     hostEventStatus, ARM_TO_PRU0_INT

        // Read the PRUINTC register to know if the event is from ARM/DSP. If yes, then branch
        MOV32   scratch_reg2, SRSR2_OFFSET
        LBCO    scratch_reg1, CONST_PRUSSINTC, scratch_reg2, 4
        QBBS    CHN_SEARCH, scratch_reg1, 0
    
        // Else it is McASP Event. So before proceeding, clear it
        MOV32   scratch_reg1, MCASP_TXRX_EVENT
        SBCO    scratch_reg1, CONST_PRUSSINTC, SICR_OFFSET, 4
    
        JMP	MCASP_EVENT
	
CORE_LOOP_PRU1:
        // wait for the hostEventStatus to get set. Loop till then
        WBS     hostEventStatus, ARM_TO_PRU1_INT
	LBCO 	scratch_reg1, MCASP_CONTROL, MCASP_XSTAT, 4   
	QBBC    CHN_SEARCH, scratch_reg1, BIT_5

	// Clear the event here and go to Transmit processing
        MOV32   scratch_reg1, PRU0_TO_PRU1_EVENT
        SBCO    scratch_reg1, CONST_PRUSSINTC, SICR_OFFSET, 4
        JMP     TxInterruptServiceRequestHndlr

MCASP_EVENT:
        // Check for RX interrrupt first
        // If TX only PRU Skip RSTAT Check
	QBEQ    MCASP_TX_EVNT, suart_global.pru_rx_tx_mode, PRU_MODE_TX_ONLY
	
	// if the PRU is RX only mode, then check if the XSTAT is set. If so, raise event to PRU1 and proceed
	QBNE    RX_TX_PROCESS, suart_global.pru_rx_tx_mode, PRU_MODE_RX_ONLY
	LBCO 	scratch_reg1, MCASP_CONTROL, MCASP_XSTAT, 4
	QBBC    RX_TX_PROCESS, scratch_reg1, BIT_5
        MOV32   scratch_reg1, PRU0_TO_PRU1_EVENT
        SBCO    scratch_reg1, CONST_PRUSSINTC, SISR_OFFSET, 4
	
RX_TX_PROCESS:	
	LBCO 	scratch_reg1, MCASP_CONTROL, MCASP_RSTAT, 4  
	QBBS    RxInterruptServiceRequestHndlr, scratch_reg1, BIT_5

        // Skip the check for XSTAT if we are not Rx/Tx PRU.
	// We don't want the PRU to spin in a tight loop around the McASP register to introduce a delay
	QBNE    CORE_LOOP, suart_global.pru_rx_tx_mode, PRU_MODE_RX_TX_BOTH
	
MCASP_TX_EVNT:
	LBCO 	scratch_reg1, MCASP_CONTROL, MCASP_XSTAT, 4   
	QBBS    TxInterruptServiceRequestHndlr, scratch_reg1, BIT_5
	
	// If PRU is both TX/RX, then go back to Core-loop. Else delay to avoid McASP Spins
	QBEQ    CORE_LOOP, suart_global.pru_rx_tx_mode, PRU_MODE_RX_TX_BOTH
	
	
//******************************************** CORE LOOP: Ends ***********************************************

//========================================================================================================================================

//******************************************** CHN_SEARCH: Starts ********************************************

//========================================================================================================================================
//	Retrieve the active channel number that has requested for serviced and 
//	load the context base info for that channel
//========================================================================================================================================
CHN_SEARCH:        	
        MOV32   regVal, 0x00000001
        LSL     regVal, regVal, suart_global.pru_id
	MOV32   scratch_reg1, SECR2_OFFSET
	SBCO    regVal, CONST_PRUSSINTC, scratch_reg1, 4

	//Read Global control register
	LBBO	suart_global, pZERO, SUART_GBL_CTRL_ADDR, SIZE(suart_global)   
	
	//Retrieve the channel number and load the context base;
	LDI 	suart_ch_info.curr_ch_base_addr, SUART_CH_BASE_ADDRESS 		
	LDI  	suart_ch_info.curr_ch_offset, SUART_CH_BASE_ADDRESS		
	LDI	    suart_ch_info.ch_num, 0x00			
	XOR		tx_context.buff_addr, tx_context.buff_addr, tx_context.buff_addr
	XOR     suart_ch_info.rx_context_addr, suart_ch_info.rx_context_addr, suart_ch_info.rx_context_addr
	
	MOV		tx_context.buff_addr, 0x90
	MOV     suart_ch_info.rx_context_addr, 0x90 
	
CHN_ACTIVE:
	LBBO 	suart_ch_regs.Chn_Cntrl, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset, SIZE (suart_ch_regs.Chn_Cntrl) 
	QBBS 	CHN_SERACH_RTN, suart_ch_regs.Chn_Cntrl, SUART_CTRL_SR_BIT
	ADD  	suart_ch_info.curr_ch_offset, suart_ch_info.curr_ch_offset, SUART_CH_REGS_SIZE
	ADD  	suart_ch_info.ch_num, suart_ch_info.ch_num, 0x01
	ADD		tx_context.buff_addr, tx_context.buff_addr, 0x2C
	ADD		suart_ch_info.rx_context_addr, suart_ch_info.rx_context_addr, 0x20
	
	//None of the channel has service request, go back to MainLoop
	QBLT 	MCASP_EVENT, suart_ch_info.ch_num, NUM_OF_CHANNELS  //check to be verified to boundary condition
	JMP 	CHN_ACTIVE
	
CHN_SERACH_RTN:
	LBBO 	suart_ch_regs, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset, SIZE (suart_ch_regs) 
        ADD     suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset

	AND     scratch_reg1.w0, suart_ch_regs.Chn_Cntrl, SUART_CTRL_MODE_MASK	
	QBEQ    TxServiceRequestHndlr, scratch_reg1.w0, SUART_CH_TX_MODE		
	QBEQ    RxServiceRequestHndlr, scratch_reg1.w0, SUART_CH_RX_MODE		
	JMP     CORE_LOOP
	
//******************************************** CHN_SEARCH: Ends **********************************************

//======================================================================================================================================

//******************************************** PRU_TO_HOST_INTERRUPT[SINGLE_PRU] starts ********************************

//========================================================================================================================================
// 	This routine raises the interrupt to ARM/DSP once the serivce request given by ARM/DSP is serviced and
// 	Jump to Core loop. The interrupt has been configured on per channel basis i.e. each RX and TX channel will
//	raise separate event to PRU INTC and that will raise an interrup to ARM/DSP. Depending upon PRU0/PRU1, 
//      PRU raises the system events to ARM/DSP. The Mapping for PRU system event on per channel basis is given below.	
	// PRU0
	// PRU0 - SYS_EVT34 (host2 - ch2) - SUART Channel 0 	        (SUART0: TX)
	// PRU0 - SYS_EVT35 (host2 - ch3) - SUART Channel 1		(SUART0: RX)
	// PRU0 - SYS_EVT36 (host3 - ch4) - SUART Channel 2		(SUART1: TX)
	// PRU0 - SYS_EVT37 (host3 - ch5) - SUART Channel 3		(SUART1: RX)
	// PRU0 - SYS_EVT38 (host4 - ch2) - SUART Channel 4		(SUART2: Tx)
	// PRU0 - SYS_EVT39 (host4 - ch3) - SUART Channel 5		(SUART2: RX)
	// PRU0 - SYS_EVT40 (host5 - ch4) - SUART Channel 6		(SUART3: TX)
	// PRU0 - SYS_EVT41 (host5 - ch5) - SUART Channel 7		(SUART3: RX)
	
	// PRU1
	// PRU1 - SYS_EVT42 (host6 - ch6) - SUART Channel 8		(SUART4: TX)
	// PRU1 - SYS_EVT43 (host6 - ch7) - SUART Channel 9		(SUART4: RX)
	// PRU1 - SYS_EVT44 (host7 - ch8) - SUART Channel 10	        (SUART5: TX)
	// PRU1 - SYS_EVT45 (host7 - ch9) - SUART Channel 11	        (SUART5: RX)
	// PRU1 - SYS_EVT46 (host8 - ch10) - SUART Channel 12	        (SUART6: TX)
	// PRU1 - SYS_EVT47 (host8 - ch11) - SUART Channel 13	        (SUART6: RX)
	// PRU1 - SYS_EVT48 (host9 - ch12) - SUART Channel 14	        (SUART7: TX)
	// PRU1 - SYS_EVT49 (host9 - ch13) - SUART Channel 15	        (SUART7: RX)
//======================================================================================================================================
PRU_TO_HOST_INTERRUPT:		
	LDI	scratch_reg1, 0		
	
	QBEQ	EVTOUT_PRU0_EVENTS, suart_global.pru_id, 0
	QBEQ	EVTOUT_PRU1_EVENTS, suart_global.pru_id, 1

EVTOUT_PRU0_EVENTS:
	//storing the counter value		
	ADD	scratch_reg1, scratch_reg1, SYS_EVT_34
	JMP	EVTOUT_SYSEVT_INIT
	
EVTOUT_PRU1_EVENTS:
	ADD	scratch_reg1, scratch_reg1, SYS_EVT_42
	
EVTOUT_SYSEVT_INIT:
	ADD     scratch_reg1, scratch_reg1, suart_ch_info.ch_num

EVTOUT_GEN:            
   	//Clear SYS_EVTn
	SBCO    scratch_reg1, CONST_PRUSSINTC, SICR_OFFSET, 4
	
	// Enable SYS_EVTn system interrupt
	SBCO    scratch_reg1, CONST_PRUSSINTC, EISR_OFFSET,  4
	
        //Generate SYS_EVTn by event out mapping
   	MOV 	r31.w0, scratch_reg1.w0
	
	JMP  	MCASP_EVENT
	
//******************************************** PRU_TO_HOST_INTERRUPT : ENDS **********************************

//======================================================================================================================================
	
//=====================================================================================================================================

//			************************************************************************************************
//									SOFT-UART RECEIVE ROUTINE : STARTS
//			************************************************************************************************

//=====================================================================================================================================	

//======================================================================================================================================

//******************************************** RxServiceRequestHndlr: Starts *********************************

//========================================================================================================================================
// 	This routine performs the baisc intialization and clearing of varoius registers for RX and loads the configuration info
// 	from PRU RAM to registers for RX channel that is being serviced. It calculates and saves serializer and r_buf address mapped
// 	to RX channel to RX context area so that each time it is not calculated again and is being directly read from TX Context Area 
//	and activates the RX serilizer if it is diabled.
//========================================================================================================================================

RxServiceRequestHndlr:	
	// load the max RX TRIES before time out
	LBBO    MAX_RX_TIMEOUT_TRIES, pZERO, MAX_RX_TIMEOUT_TRIES_OFFSET, 2

	//read interrupt status regsiter
        LBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2	

	CLR	    suart_global.intrStatus, suart_global.intrStatus, suart_ch_info.ch_num

	//write interrupt status regsiter
   	SBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2   

	//Clear Service Request
	CLR  	suart_ch_regs.Chn_Cntrl, SUART_CTRL_SR_BIT
	SBBO 	suart_ch_regs.Chn_Cntrl,suart_ch_info.curr_ch_base_addr, SUART_CH_CTRL_OFFSET, SIZE (suart_ch_regs.Chn_Cntrl)
	
	// clear timeout flag
	CLR    suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_RX_TIMEOUT_BIT
	
	// Set the TXRX_READY_BIT
	SET  	suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_READY_BIT ;

	// update the RX Status Register
	SBBO 	suart_ch_regs.Chn_TxRxStatus, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXSTATUS_OFFSET, SIZE (suart_ch_regs.Chn_TxRxStatus)

        // Set the SUART_CH_TXRXCHNSTATUS_BIT to indicate the channel being active
        SET     suart_ch_regs.Chn_Status, SUART_CH_TXRXCHNSTATUS_BIT
        SBBO 	suart_ch_regs.Chn_Status, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXCHNSTATUS_OFFSET, SIZE (suart_ch_regs.Chn_Status)
		
RX_CONTEXT_INIT:

	QBEQ     PRUxxx_MODE_RX_ONLY, suart_global.pru_rx_tx_mode, PRU_MODE_RX_ONLY
	
        // Load RX Context Base Address corresponding to Active RX Channel
	CALL 	LOAD_RX_CONTEXT_ADDRESS
	
PRUxxx_MODE_RX_ONLY:		
	//Calculating the specific SRCTL and R_BUF register offset.
	AND 	scratch_reg1, suart_ch_regs.Chn_Cntrl.b1, SUART_CTRL_SERIALIZER_MASK  
	LSL 	scratch_reg1, scratch_reg1, SUART_CTRL_SRCTL_BIT_SHIFT
	
	// Storing SRCTL register address in RX Context Area Region
	MOV32  	rx_context.asp_rsrctl_reg, MCASP_SRCTL_BASE
	ADD 	rx_context.asp_rsrctl_reg, rx_context.asp_rsrctl_reg, scratch_reg1
	
	//storing asp_rsrctl_reg in RX Context Address Region
	SBBO    rx_context.asp_rsrctl_reg, suart_ch_info.rx_context_addr, SUART_CH_ASP_RSRCTL_REG, SIZE (rx_context.asp_rsrctl_reg)  
	
	//Store RBuf Address in RX Context Region
	MOV32  	rx_context.asp_rbuf_reg, MCASP_RBUF_BASE
	ADD 	rx_context.asp_rbuf_reg, rx_context.asp_rbuf_reg, scratch_reg1
	
	// storing asp_rbuf_reg in RX context  adress region
	SBBO    rx_context.asp_rbuf_reg, suart_ch_info.rx_context_addr, SUART_CH_ASP_RBUF_REG, SIZE (rx_context.asp_rbuf_reg)
	
	// Load the Context info specific to Current RX channel from memory to registers 
//	LBBO   	rx_context,  suart_ch_info.rx_context_addr, #00, SIZE (rx_context)
	
	// Clear the RX timeout counter
	XOR     rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr
	SBBO    rx_context.rx_timeout_cntr, suart_ch_info.rx_context_addr, SUART_CH_RX_TIMEOUT_CNTR_OFFSET, SIZE(rx_context.rx_timeout_cntr)
	
	// Activate RX serializer,  
	LBBO	scratch_reg2, rx_context.asp_rsrctl_reg, #00, 4
	AND     scratch_reg2, scratch_reg2, 0x3
	QBEQ    CLR_RSTAT, scratch_reg2, 0x2           // Check if Serializer is Already Active as Rx if ,yes skip activation
	MOV32   scratch_reg2, 0x000E 					  //  Activate serializer as Receiver
	SBBO 	scratch_reg2, rx_context.asp_rsrctl_reg, #00, 4
	
CLR_RSTAT:	
	//Clear the RSTAT  (Overrun, etc)
	MOV32 	scratch_reg1, 0xFFFF 
	SBCO  	scratch_reg1, MCASP_CONTROL, MCASP_RSTAT, 4	
	
	JMP     MCASP_EVENT 			

//******************************************** RxServiceRequestHndlr: ENDS ***********************************
	
//========================================================================================================================================

//******************************************** RxInterruptServiceRequestHndlr: Starts ************************

//========================================================================================================================================
//	RxInterruptServiceRequestHndlr is called when there is MCASP RX event, scans the active RX serializer,
//	once the active serializer is found, scans for corresponding RX channel, if it also is found, it loads 
//	the context info for RX channel and proceeds for reading the frame tramsmitted by sender.
//========================================================================================================================================

RxInterruptServiceRequestHndlr:
	//Retrieve the channel number and load the RX context base info corressponding to serializer address in scratch_reg1 and serializer number  in scratch_reg4
	LDI 	suart_ch_info.curr_ch_base_addr, SUART_CH_BASE_ADDRESS 			//Load the SUART CHANNEL BASE ADDRESS
	
	QBEQ     PRUx_MODE_RX_ONLY, suart_global.pru_rx_tx_mode, PRU_MODE_RX_ONLY
		
	//Since the Rx Channel are 1,3,5,7 Load the suart_ch_regs for Rx channel 1 as it is first channel
	ADD     suart_ch_info.curr_ch_offset, pZERO.w0, SUART_CH_REGS_SIZE	
	
	LDI	    suart_ch_info.ch_num, 0x01						
	
	// Load the RX channel 1 context address to Ch_info register's rx_context_addr field
	LDI 	suart_ch_info.rx_context_addr, SUART_CH1_RX_CONTEXT_ADDR 		
	JMP     SERCH_ACTIVE_RX_CHN_RX
	

PRUx_MODE_RX_ONLY:		
	//Since the Rx Channel are 1,3,5,7 Load the suart_ch_regs for Rx channel 1 as it is first channel
	LDI     suart_ch_info.curr_ch_offset, 0x00

	LDI	    suart_ch_info.ch_num, 0x00						
	
	// Load the RX channel 1 context address to Ch_info register's rx_context_addr field
	LDI 	suart_ch_info.rx_context_addr, 0x90 		

	// Search for the channel corresponding to serilizer number in scratch_reg4
SERCH_ACTIVE_RX_CHN_RX:
        ADD     scratch_reg1, suart_ch_info.curr_ch_offset, SUART_CH_TXRXCHNSTATUS_OFFSET
	LBBO 	suart_ch_regs.Chn_Status, suart_ch_info.curr_ch_base_addr, scratch_reg1, SIZE (suart_ch_regs.Chn_Status)  //Load the Channel Cntrl info from Memory to Register
        QBBC    NEXT_RX_CHN, suart_ch_regs.Chn_Status, SUART_CH_TXRXCHNSTATUS_BIT

        LBBO    scratch_reg1, suart_ch_info.rx_context_addr, SUART_CH_ASP_RSRCTL_REG, 4
        LBBO    scratch_reg2, scratch_reg1, #0, 4
	QBBS    ACTIVE_RX_CHN_FOUND, scratch_reg2, ASP_SRCTL_RRDY_BIT  
      
NEXT_RX_CHN:
	QBEQ    PRUxx_MODE_RX_ONLY, suart_global.pru_rx_tx_mode, PRU_MODE_RX_ONLY		
	
	ADD  	suart_ch_info.curr_ch_offset, suart_ch_info.curr_ch_offset, SUART_CH_REGS_SIZE2 //offset of RX suart_ch_regs 
	ADD  	suart_ch_info.ch_num, suart_ch_info.ch_num, 0x2			// Increamnet to Next Rx ChanneL number
	
	ADD     suart_ch_info.rx_context_addr, suart_ch_info.rx_context_addr, RX_CONTEXT_OFFSET  // Increamnet rx_context_addr by RX_CONTEXT_OFFSET i.e. to next RX channel context address
	QBGE 	SERCH_ACTIVE_RX_CHN_RX, suart_ch_info.ch_num, NUM_OF_CHANNELS
	JMP     CORE_LOOP  			//JMP to CORE_LOOP to if no Active RX channel found 
	
PRUxx_MODE_RX_ONLY:
	ADD  	suart_ch_info.curr_ch_offset, suart_ch_info.curr_ch_offset, SUART_CH_REGS_SIZE //offset of RX suart_ch_regs 
	ADD  	suart_ch_info.ch_num, suart_ch_info.ch_num, 0x1			// Increamnet to Next Rx ChanneL number
	ADD     suart_ch_info.rx_context_addr, suart_ch_info.rx_context_addr, 0x20  // Increamnet rx_context_addr by RX_CONTEXT_OFFSET i.e. to next RX channel context address
	QBGE 	SERCH_ACTIVE_RX_CHN_RX, suart_ch_info.ch_num, NUM_OF_CHANNELS
	JMP     CORE_LOOP  			//JMP to CORE_LOOP to if no Active RX channel found 

ACTIVE_RX_CHN_FOUND:
	LBBO 	suart_ch_regs, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset, SIZE (suart_ch_regs)  //Load the suart_ch_regs from Memory to Register

        // Load the Context info specific to current RX Channel from memory to registers 
	LBBO   	rx_context,  suart_ch_info.rx_context_addr, #00, SIZE (rx_context)

        ADD    suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_base_addr, suart_ch_info.curr_ch_offset

	//Clear the RSTAT  (Overrun, etc) for Errors
	LBCO  	scratch_reg1, MCASP_CONTROL, MCASP_RSTAT, 4			
	QBBC    RX_PROCESSING_INIT, scratch_reg1, ASP_RSTAT_RERR_BIT		
	
	MOV32 	scratch_reg1, 0xFFFF 
	SBCO  	scratch_reg1, MCASP_CONTROL, MCASP_RSTAT, 4		

    
//  Start receving DATA from MAC_ASP's R-Buf corresponding to channel	
RX_PROCESSING_INIT:
        XOR    mcasp_rbuf_val, mcasp_rbuf_val, mcasp_rbuf_val
  	// Read the content of RBUF
	LBBO	scratch_reg1, rx_context.asp_rbuf_reg, #00, 4
        OR      mcasp_rbuf_val, mcasp_rbuf_val, scratch_reg1


	// If start condition is already received then go to reading next bit  otherwise look for start condition
	QBLT 	READ_CURRENT, suart_ch_regs.Chn_TxRxBitsDoneCtr, 0  
	
	//check Chn_TxRxRepeatDoneCtr, if it is not zero, jump to READ_CURRENT to prescale the start condition
	QBLT 	READ_CURRENT, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 0 
	
	// If sampling point i.e. sampling_bit_pos is equal to greater than 16 (INVALID_SAMPLING_POINT),
	// start bit transition edge is being detected, fall through to calculate sampling point, 
	// otherwise, sampling point is already calculated JUMP to READ_CURRENT
        QBGE    READ_CURRENT, rx_context.sampling_bit_pos, INVALID_SAMPLING_POINT     // 
	
        // Extract timing information by detecting start transition (first left most zero)
        LMBD    scratch_reg4, scratch_reg1, 0
        QBGT    START_BIT_TRANSITION, scratch_reg4, ZERO_BIT_NOT_DETECTED // branch if zero: start bit transition detected 
        MOV	    rx_context.sampling_bit_pos, 0xff
        SBBO    rx_context.sampling_bit_pos,  suart_ch_info.rx_context_addr, SUART_CH_SAMPLING_BIT_POS_OFFSET, SIZE (rx_context.sampling_bit_pos)

	// RX time out logic 
	QBBC   RxInterruptServiceRequestHndlr, suart_ch_regs.Chn_Config1, RX_TIMEOUT_INTR_MASK
	QBBC   RxInterruptServiceRequestHndlr, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_READY_BIT	  
        QBEQ   RxInterruptServiceRequestHndlr, suart_ch_regs.Chn_TxRxBytesDoneCtr, 0
	
	// Read the request count to be received 
	LSR 	scratch_reg1, suart_ch_regs.Chn_Config2, SUART_CH_CONFIG2_DATALEN_SHIFT //0x8		
	AND	    scratch_reg1, scratch_reg1, SUART_CH_CONFIG2_DATALEN_MASK	//0x0F
	ADD	    scratch_reg1, scratch_reg1, 0x01			//Since fifo size is 16
	QBEQ   RxInterruptServiceRequestHndlr, suart_ch_regs.Chn_TxRxBytesDoneCtr, scratch_reg1
	
	// check if time-out is enabled, if yes increament the timeout counter and check if count is equal to MAX_RX_TIMEOUT_TRIES
	// if yes raise the interrupt for time out.
	ADD    rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr, 1
	SBBO   rx_context.rx_timeout_cntr, suart_ch_info.rx_context_addr, SUART_CH_RX_TIMEOUT_CNTR_OFFSET, SIZE(rx_context.rx_timeout_cntr)
	QBGE   RxInterruptServiceRequestHndlr, rx_context.rx_timeout_cntr, MAX_RX_TIMEOUT_TRIES
	SET    suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_RX_TIMEOUT_BIT
	CLR    suart_ch_regs.Chn_Config1, suart_ch_regs.Chn_Config1, RX_TIMEOUT_INTR_MASK
	SBBO   suart_ch_regs.Chn_Config1, suart_ch_info.curr_ch_base_addr, SUART_CH_CONFIG1_OFFSET, SIZE (suart_ch_regs.Chn_Config1)
	
	// Clear the RX timeout counter
	XOR     rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr
	SBBO    rx_context.rx_timeout_cntr, suart_ch_info.rx_context_addr, SUART_CH_RX_TIMEOUT_CNTR_OFFSET, SIZE(rx_context.rx_timeout_cntr)
	JMP    RX_CHN_INTR

	// Calculate the sampling bit position based on the start bit position
	// center = oversampling / 2
	// sampling bit position = start bit possition - center

START_BIT_TRANSITION:	
    // clear the rx time out counter
	XOR     rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr, rx_context.rx_timeout_cntr
	SBBO    rx_context.rx_timeout_cntr, suart_ch_info.rx_context_addr, SUART_CH_RX_TIMEOUT_CNTR_OFFSET, SIZE(rx_context.rx_timeout_cntr)
	
	// determine the over-sampling rate
	LSR     scratch_reg2, suart_ch_regs.Chn_Config1, SUART_CH_CONFIG1_OVS_BIT_SHIFT //0xA            //Right Shift by 10
	AND     scratch_reg2, scratch_reg2, SUART_CH_CONFIG1_OVS_BIT_MASK  //0x3			   	

	//OVER_SAMPLE
	QBEQ	OVER_SAMPLE_SIZE8BIT, scratch_reg2, OVER_SAMPLING_NONE		// No over Sampling 
	QBEQ	OVER_SAMPLE_SIZE8BIT, scratch_reg2, OVER_SAMPLING_8BIT  	// 8 bit over sampling
	QBEQ	OVER_SAMPLE_SIZE16BIT, scratch_reg2, OVER_SAMPLING_16BIT	// 16 bit over sampling

	// Calaulate sampling bit position for 8 bit over sampling	
OVER_SAMPLE_SIZE8BIT:
	SUB 	rx_context.sampling_bit_pos, scratch_reg4, OVR_SAMPL_8BIT_MID_DATA_BIT  //4						//start bit possition - center 
	AND	rx_context.sampling_bit_pos, rx_context.sampling_bit_pos, SAMPING_MASK_8_BIT_OVRSAMPLNG	// sampling point
	SBBO    rx_context.sampling_bit_pos,  suart_ch_info.rx_context_addr, SUART_CH_SAMPLING_BIT_POS_OFFSET, SIZE (rx_context.sampling_bit_pos)
	QBLE	READ_CURRENT, scratch_reg4, OVR_SAMPL_8BIT_MID_DATA_BIT //4         //if Start bit position is eqaul to/greater than centre, sample the start bit in current read, otherwise in next read
	JMP	RxInterruptServiceRequestHndlr

	// Calaulate sampling bit position for 16 bit over sampling
OVER_SAMPLE_SIZE16BIT:
	SUB	rx_context.sampling_bit_pos, scratch_reg4, OVR_SAMPL_16BIT_MID_DATA_BIT // start bit possition - center
	AND	rx_context.sampling_bit_pos,  rx_context.sampling_bit_pos, SAMPING_MASK_16_BIT_OVRSAMPLNG		//samplimg point  
	SBBO    rx_context.sampling_bit_pos,  suart_ch_info.rx_context_addr, SUART_CH_SAMPLING_BIT_POS_OFFSET, SIZE (rx_context.sampling_bit_pos)
	QBLE	READ_CURRENT, scratch_reg4, OVR_SAMPL_16BIT_MID_DATA_BIT  //if Start bit position is eqaul to/greater than centre, sample the start bit in current read, otherwise in next read
	JMP	RxInterruptServiceRequestHndlr

READ_CURRENT:	
	// scratch_8bit_reg2 holds the information if bit detected is zero if scratch_8bit_reg2= 0, or one if scratch_8bit_reg2 = 1
	XOR	scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2		 		
	QBBC  	READ_ZERO, scratch_reg1, rx_context.sampling_bit_pos	 //if bit at sampling point is zero jump to READ_ZERO
	ADD	scratch_8bit_reg2, scratch_8bit_reg2, 1	                // otherwise increament scratch_8bit_reg2 by one as bit detected is one
    
READ_ZERO:
	// We have read the data bit here...
	// If start bit is being received already, then skip the start condition processing.
	QBLT 	RX_BIT_RECVD, suart_ch_regs.Chn_TxRxBitsDoneCtr, 0  
	
	//(Chn_TxRxBitsDoneCtr == 0)            //No bit is being Recieved, check if it is start bit
	// if DataBit == 0, i.e. scratch_8bit_reg2 == 0, Jump to Start Condition, else error fall through
	QBEQ	START_CONDITION, scratch_8bit_reg2, 0	
	
	QBEQ 	START_CONDITION, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 0   // to be checked with new logic if required or not
	
	// Broken start condition or false alarm, Reset repeat counter		//if DataBit == 1, instead of zero
	XOR     suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	JMP     RxInterruptServiceRequestHndlr	
	
START_CONDITION:              //else part for NO_REPEAT_DONE  DataBit == 0
	// Increament Repeat Done Counter by One, write back to memory
	ADD	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr) 
	
	// Read Pre Scaler
	MOV     scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK
	AND     scratch_reg2, scratch_reg2, suart_ch_regs.Chn_Config1

	// if Repeat Done count is greater than or equal to prescaler, start bit is received, jump to START_BIT_RECIVED, 
	QBGE 	START_BIT_RECIVED, scratch_reg2, suart_ch_regs.Chn_TxRxRepeatDoneCtr  
	JMP	RxInterruptServiceRequestHndlr
	
	// Start bit is condition Detected properly
START_BIT_RECIVED: 
   	 // Increament Bit Count by One, and write it to memory
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	// Reset Repeat Counter, and write it to memory
	XOR     suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	JMP	RxInterruptServiceRequestHndlr
	
	// Start Bit has been detected Already, Now the data bit is being received	
RX_BIT_RECVD:
	// Now scratch_reg1 holds the info whether the data bit in scratch_8bit_reg2, is zero or one
	XOR     scratch_reg1, scratch_reg1, scratch_reg1
	
	//if scratch_8bit_reg2 = 0, i.e data bit is Zero Jump to RX_DATA_BIT_ZERO 
	// else Data bit is one fall through, data bit is ONE
	QBEQ    RX_DATA_BIT_ZERO, scratch_8bit_reg2, 0    
	OR      scratch_reg1, scratch_reg1, 0x1			// bit received is one, scratch_reg1 = 1

RX_DATA_BIT_ZERO:              
	//if (Chn_TxRxRepeatDoneCntr < 32), check if reapeat done counter is less than 32, if yes Jump to RX_REPEAT_DONE_CNTR_LT_32
	QBGE   RX_REPEAT_DONE_CNTR_LT_32, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 0x20
	
	// repeat done counter is Greater than 32, Read Chn_RxDataBitsHoldRegHigh reg, Copy the Received bit to Chn_RxDataBitsHoldRegHigh register
RX_REPEAT_DONE_CNTR_GT_32:	
	 //else part : (Chn_TxRxRepeatDoneCntr is Greater than or equal to 32 )
	RSB     scratch_reg2, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 0x20     		// Calculate the offset for bit in Chn_RxDataBitsHoldRegHigh regsiter
	LSL     scratch_reg1, scratch_reg1, scratch_reg2        // Shift Received bit by above calculated of set i.e Chn_TxRxRepeatDoneCntr - 20
	LBBO    rx_context.Chn_RxDataBitsHoldRegHigh, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGHIGH_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegHigh)
	OR      rx_context.Chn_RxDataBitsHoldRegHigh, scratch_reg1, rx_context.Chn_RxDataBitsHoldRegHigh
	SBBO    rx_context.Chn_RxDataBitsHoldRegHigh, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGHIGH_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegHigh)
	JMP	RX_REPEAT_COUNT_LESS_THAN_PRESCALR
	
	// repeat done counter is less than OR equal to 32, Read the Chn_RxDataBitsHoldRegLow, Copy the Received bit to Chn_RxDataBitsHoldRegLow register
	// write it back to memory
RX_REPEAT_DONE_CNTR_LT_32:   //if for (Chn_TxRxRepeatDoneCntr < 32)
	LSL    scratch_reg1, scratch_reg1, suart_ch_regs.Chn_TxRxRepeatDoneCtr   	// Shift Received bit by Repeat Done Counter
	LBBO    rx_context.Chn_RxDataBitsHoldRegLow, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGLOW_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegLow)
	OR      rx_context.Chn_RxDataBitsHoldRegLow, scratch_reg1, rx_context.Chn_RxDataBitsHoldRegLow    
	SBBO    rx_context.Chn_RxDataBitsHoldRegLow, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGLOW_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegLow)
	
	// Increamnet Chn_TxRxRepeatDoneCntr by one and Check if Repeat Done Counter is equal to Prescalar, 
	// if yes jump to PROCESS_RX_DATA_BIT, otherewise again sample RBuf for same bit
RX_REPEAT_COUNT_LESS_THAN_PRESCALR:   
	ADD     suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	
	// Read Pre Scaler
	MOV     scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK, 
	AND     scratch_reg2, scratch_reg2, suart_ch_regs.Chn_Config1
	
	// check if number of bits sampled (Chn_TxRxRepeatDoneCtr) is equal to prescaler (scratch_reg2), if yes jump to PROCESS_RX_DATA_BIT
	QBGE 	PROCESS_RX_DATA_BIT, scratch_reg2, suart_ch_regs.Chn_TxRxRepeatDoneCtr  //(Chn_TxRxRepeatDoneCntr >= Chn_Config1.PreScaller)
	JMP	RxInterruptServiceRequestHndlr
	
	// Scan Chn_RxDataBitsHoldRegLow, Chn_RxDataBitsHoldRegHigh, to check if BIT received is one or zero and write to Chn_RxDataHoldReg
PROCESS_RX_DATA_BIT:   //(Chn_TxRxRepeatDoneCntr >= Chn_Config1.PreScaller) if part	
	
	// Get the Presaclar
	MOV 	scratch_reg3, SUART_CTRL_PRE_SCALAR_MASK
	AND     scratch_reg3, scratch_reg3, suart_ch_regs.Chn_Config1   //scratch_reg3 hold prescalar
	
	// Initialize the register to zero required for copying data bit received in rxdata_buf 
	XOR     scratch_8bit_reg1, scratch_8bit_reg1, scratch_8bit_reg1  	// keep count of number of ONE scanned 
	XOR     scratch_8bit_reg2, scratch_8bit_reg2, scratch_8bit_reg2		// keep count of number of ZERO scanned
	
	// used to store count of number of bits scanned in Chn_RxDataBitsHoldRegLow, & Chn_RxDataBitsHoldRegHigh
	XOR     scratch_reg2, scratch_reg2,scratch_reg2    
	
	//points to location taken as start point in Chn_RxDataBitsHoldRegLow for scannig bit received
        XOR     scratch_reg1, scratch_reg1, scratch_reg1,  

	// scratch_reg4 holds the data from Chn_RxDataBitsHoldRegLow	
	LBBO    scratch_reg4, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGLOW_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegLow) 	
	QBGE    BIT_CHK_LOOP, scratch_reg3, 0x20       // if Pre Scalar is less than or equal to 32, JMP to BIT_CHK_LOOP

	//pre scalar is greater 32, check if it is greater that 48 then set scratch_reg3 = 48, scan bit upto this count only.
PRE_SCALR_GT_32:
	OR      scratch_reg1, scratch_reg1, 0x10        // start checking bit from bit position 0x10 
	QBGT    BIT_CHK_LOOP, scratch_reg3, 0x30         //if Pre Scalar is less than 48
	
	//pre scalar is greater 48,  set scratch_reg3 = 48, scan bit upto this count only.
PRE_SCALR_GT_48:
        MOV     scratch_reg3, 0x30   
	
	//Scan the Chn_RxDataBitsHoldRegLow, and Chn_RxDataBitsHoldRegHigh registers to know received bit is ZERO or ONE
BIT_CHK_LOOP:
	QBBC    BIT_RECVD_ZERO, scratch_reg4, scratch_reg1     // if bit is cleared, Jump to BIT_RECVD_ZERO
	ADD     scratch_8bit_reg1, scratch_8bit_reg1, 1        // else BIT prerscaled is one
	ADD     scratch_reg1, scratch_reg1, 1			// Increament scratch_reg1 by one so that it points to next bit to scanned
	ADD     scratch_reg2, scratch_reg2, 1			// Increament scratch_reg2 holding bits scanned count
	QBLT    LOAD_RXDATABITS_HOLDREGHIGH, scratch_reg2, 0x20   // if Prescaler is greater than 32, and scratch_reg2 is equal to 32, load Chn_RxDataBitsHoldRegHigh in scratch_reg4
	QBGT    BIT_CHK_LOOP, scratch_reg2, scratch_reg3,         // scan untill all the bits are scanned
	JMP  	COPY_BIT_RECVD
	
	// Load the Chn_RxDataBitsHoldRegHigh to scratch_reg4
LOAD_RXDATABITS_HOLDREGHIGH:
	LBBO    scratch_reg4, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGHIGH_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegHigh) 
	XOR	    scratch_reg1, scratch_reg1, scratch_reg1 		// Reset the scratch_reg1, so that starts from bit 0 for Chn_RxDataBitsHoldRegHigh
	XOR	    scratch_reg2, scratch_reg2, scratch_reg2 		// Reset the scratch_reg2, so that only jump to label LOAD_RXDATABITS_HOLDREGHIGH done one's only
	SUB     scratch_reg3, scratch_reg3, 0x20         		// Decreament Total loop count by 32, since it has been already checked in Chn_RxDataBitsHoldRegLow
	JMP     BIT_CHK_LOOP

	// Current sacnned Bit in Chn_RxDataBitsHoldRegHigh or Chn_RxDataBitsHoldRegLow  is zero
BIT_RECVD_ZERO: 
        ADD     scratch_8bit_reg2, scratch_8bit_reg2, 1   	//for Zeor
	ADD     scratch_reg1,scratch_reg1, 1                   
	ADD     scratch_reg2, scratch_reg2, 1
	QBGT    BIT_CHK_LOOP, scratch_reg2, scratch_reg3,
	
	// Copy the Received bit to Chn_RxDataHoldReg, scratch_reg1, now store the info if bit received is zero or one
COPY_BIT_RECVD:
	XOR     scratch_reg1, scratch_reg1, scratch_reg1
	
	// scratch_8bit_reg1= Bit is ONE, scratch_8bit_reg2 = Bit is Zero, if scratch_8bit_reg2 > scratch_8bit_reg1,
	// jump to WRITE_RCVD_BIT_TO_RX_DATAHOLDREG as data bit is ZERO
	QBGE    WRITE_RCVD_BIT_TO_RX_DATAHOLDREG, scratch_8bit_reg1, scratch_8bit_reg2   
	OR      scratch_reg1, scratch_reg1, 0x1    		//Bit Received is One,  write to Chn_RxDataHoldReg

	
	// Write the Received Data bit (in scratch_reg1) to Chn_RxDataHoldReg
WRITE_RCVD_BIT_TO_RX_DATAHOLDREG:
	LSL    	scratch_reg1, scratch_reg1, suart_ch_regs.Chn_TxRxBitsDoneCtr   // Shit the bit received by Chn_TxRxBitsDoneCtr
	
	//Read the Chn_RxDataHoldReg from Memory
	LBBO   	rx_context.Chn_RxDataHoldReg, suart_ch_info.rx_context_addr, SUART_CH_RXDATAHOLDREG_OFFSET, SIZE (rx_context.Chn_RxDataHoldReg)
	
	//Write the bit received to Chn_RxDataHoldReg
	OR     	rx_context.Chn_RxDataHoldReg, rx_context.Chn_RxDataHoldReg, scratch_reg1
	
	// Write updated Chn_RxDataHoldReg to memroy
	SBBO   	rx_context.Chn_RxDataHoldReg, suart_ch_info.rx_context_addr, SUART_CH_RXDATAHOLDREG_OFFSET, SIZE (rx_context.Chn_RxDataHoldReg)

	 // Increment the Data bit Counter
	ADD 	suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)
	
	// Reset the Repeat Done Counter
	XOR	suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
	
	// initialize Chn_RxDataBitsHoldRegLow
	XOR 	rx_context.Chn_RxDataBitsHoldRegLow, rx_context.Chn_RxDataBitsHoldRegLow, rx_context.Chn_RxDataBitsHoldRegLow
	SBBO    rx_context.Chn_RxDataBitsHoldRegLow, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGLOW_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegLow)
	
	// initialize Chn_RxDataBitsHoldRegHigh
	XOR 	rx_context.Chn_RxDataBitsHoldRegHigh, rx_context.Chn_RxDataBitsHoldRegHigh, rx_context.Chn_RxDataBitsHoldRegHigh
	SBBO    rx_context.Chn_RxDataBitsHoldRegHigh, suart_ch_info.rx_context_addr, SUART_CH_RXDATABITSHOLDREGHIGH_OFFSET, SIZE (rx_context.Chn_RxDataBitsHoldRegHigh)
	
	// Read Bit Per Charater
	AND    	scratch_reg2, suart_ch_regs.Chn_Config2, SUART_CH_CONFIG2_BITS_PER_CHAR_MASK //0xF
	
	// check is (N-1) bit is being received for current data frame, if yes jump to CHK_RECVD_DATA_FRAME
        // if all N bits has been Received Jump to RESET_BITS_CNTR, otherwise receive remaining bits.
        QBGE    RESET_BITS_CNTR, scratch_reg2, suart_ch_regs.Chn_TxRxBitsDoneCtr        //(Chn_TxRxBitsDoneCntr >= Chn_Config2.BitsPerChar)
        SUB     scratch_reg2, scratch_reg2, 1
        QBEQ    CHK_RECVD_DATA_FRAME, scratch_reg2, suart_ch_regs.Chn_TxRxBitsDoneCtr
	JMP	RxInterruptServiceRequestHndlr
	
	// if all bits received, verify the Received data frame
CHK_RECVD_DATA_FRAME:
	//Zero the (16 - Chn_TxRxBitsDoneCntr) Most significant bits in the Chn_RxDataHoldReg.
	RSB     scratch_reg2, scratch_reg2, 16
	ADD     scratch_reg2, scratch_reg2, 0x10           // load the count to for number of zero to inserted
	NOT     scratch_reg1, pZERO  			// Used to Insert  Zero in MSB
	
REGHOLD_MSB_ZERO: 
	// Prepare the MASK with  ZERO's in bits that do not corresponds to data bits	
	LSR     scratch_reg1, scratch_reg1, scratch_reg2

	// Read the Data hold Reg
	LBBO    rx_context.Chn_RxDataHoldReg, suart_ch_info.rx_context_addr, SUART_CH_RXDATAHOLDREG_OFFSET, SIZE (rx_context.Chn_RxDataHoldReg)
	// Insert the ZERO's in bits that do  not  corresponds to Data Bits
	AND  	rx_context.Chn_RxDataHoldReg, rx_context.Chn_RxDataHoldReg, scratch_reg1
	SBBO    rx_context.Chn_RxDataHoldReg, suart_ch_info.rx_context_addr, SUART_CH_RXDATAHOLDREG_OFFSET, SIZE (rx_context.Chn_RxDataHoldReg)
   
        // removing start bit
        LSR     rx_context.Chn_RxDataHoldReg, rx_context.Chn_RxDataHoldReg, 1  // removing start bit

        // load the arm memory lacation address where data is to be written
        LBBO    suart_ch_regs.ch_TxRxData, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXDATA_OFFSET, SIZE (suart_ch_regs.ch_TxRxData)

        // Read Bits Per Charater
        AND     scratch_reg4, suart_ch_regs.Chn_Config2, SUART_CH_CONFIG2_BITS_PER_CHAR_MASK //0xF
        SUB     scratch_reg4, scratch_reg4, 2

        // Load the Bytes Done counter
        MOV     scratch_reg1, suart_ch_regs.Chn_TxRxBytesDoneCtr

        // check, if two byte offset is required (bits per character greater than 8)
        QBGE    WRITE_RX_CHAR_TO_MEM, scratch_reg4, SUART_CH_CONFIG2_8BITS_PER_CHAR //8

        // calculate the offset in memory where received character is to be written
        LSL     scratch_reg1, scratch_reg1, 1

WRITE_RX_CHAR_TO_MEM:
        // Write the actual data to ARM Memory
        SBBO    rx_context.Chn_RxDataHoldReg, suart_ch_regs.ch_TxRxData, scratch_reg1, SIZE (rx_context.Chn_RxDataHoldReg)
 
        JMP     RxInterruptServiceRequestHndlr    

RESET_BITS_CNTR:
	// Check for Framing Error Framing Error
	SUB     scratch_reg3, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1

	// Reset bits done counter	
	XOR     suart_ch_regs.Chn_TxRxBitsDoneCtr,suart_ch_regs.Chn_TxRxBitsDoneCtr,suart_ch_regs.Chn_TxRxBitsDoneCtr
	SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr) 

	// Get the Presaclar
	MOV 	scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK
	AND     scratch_reg2, scratch_reg2, suart_ch_regs.Chn_Config1   //scratch_reg2 hold prescalar

        // Extract timing information by detecting start transition (first left most zero)
        LMBD    scratch_reg4, mcasp_rbuf_val, 0
        QBEQ    INVALID_SAMPLING_PNT, scratch_reg4, ZERO_BIT_NOT_DETECTED   // branch if zero start bit transition detected     
        // determine the over-sampling rate
        LSR     scratch_reg1, suart_ch_regs.Chn_Config1, SUART_CH_CONFIG1_OVS_BIT_SHIFT //0xA            //Right Shift by 10
        AND     scratch_reg1, scratch_reg1, SUART_CH_CONFIG1_OVS_BIT_MASK  //0x3                                //OVER_SAMPLE
        QBEQ    NXT_FRAME_SAMPLING_16BIT_OVS, scratch_reg1, OVER_SAMPLING_16BIT // 16 bit over sampling

    // Calaulate sampling bit position for 8 bit over sampling  
NXT_FRAME_SAMPLING_8BIT_OVS:
        QBLT    INVALID_SAMPLING_PNT, scratch_reg4, 4 // to correct  bit timing error used 4

CAL_SAMPL_PNT8:
        SUB     rx_context.sampling_bit_pos, scratch_reg4, OVR_SAMPL_8BIT_MID_DATA_BIT    //start bit position - center 
        AND     rx_context.sampling_bit_pos, rx_context.sampling_bit_pos, SAMPING_MASK_8_BIT_OVRSAMPLNG // sampling point
        QBGT    UPDATE_SAMPLING_PNT, scratch_reg4, 4
        JMP     NXT_FRAME_SAMPLING_PNT
    
    // Calaulate sampling bit position for 16 bit over sampling
NXT_FRAME_SAMPLING_16BIT_OVS:
        QBLT    INVALID_SAMPLING_PNT, scratch_reg4, 8 // to correct  bit timing error used 4

CAL_SAMPL_PNT16:
        SUB     rx_context.sampling_bit_pos, scratch_reg4, OVR_SAMPL_16BIT_MID_DATA_BIT     //start bit position - center
        AND     rx_context.sampling_bit_pos,  rx_context.sampling_bit_pos, SAMPING_MASK_16_BIT_OVRSAMPLNG  //samplimg point  
        QBGT    UPDATE_SAMPLING_PNT, scratch_reg4, 8

NXT_FRAME_SAMPLING_PNT: 
        // Increament Repeat Done Counter by One, write back to memory
        ADD     suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, 1
        SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)

        // Read Pre Scaler
        MOV     scratch_reg2, SUART_CTRL_PRE_SCALAR_MASK
        AND     scratch_reg2, scratch_reg2, suart_ch_regs.Chn_Config1

        // if Repeat Done count is greater than or equal to prescaler, start bit is received, jump to START_BIT
        QBLT   UPDATE_SAMPLING_PNT, scratch_reg2, suart_ch_regs.Chn_TxRxRepeatDoneCtr

        // Start bit is condition Detected properly
START_BIT:
        // Increament Bit Count by One, and write it to memory
        ADD     suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_regs.Chn_TxRxBitsDoneCtr, 1
        SBBO    suart_ch_regs.Chn_TxRxBitsDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBITSDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBitsDoneCtr)

        XOR     suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_regs.Chn_TxRxRepeatDoneCtr
        SBBO    suart_ch_regs.Chn_TxRxRepeatDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXREPEATDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxRepeatDoneCtr)
        JMP     UPDATE_SAMPLING_PNT

INVALID_SAMPLING_PNT:
	// Reset the Sampling Point 
	MOV     rx_context.sampling_bit_pos, 0xff 

UPDATE_SAMPLING_PNT:
        SBBO    rx_context.sampling_bit_pos,  suart_ch_info.rx_context_addr, SUART_CH_SAMPLING_BIT_POS_OFFSET, SIZE (rx_context.sampling_bit_pos)

	//read interrupt mask regsiter
   	LBBO    suart_global.intrMask, pZERO, SUART_GBL_INT_MASK_ADDR,  2	
	
	//read interrupt status regsiter
        LBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2	
	
	// check for error in received data frame		
	// Check for Break Condiotion Error
	QBGE    RX_DATA_ZERO, rx_context.Chn_RxDataHoldReg, 0			

	// Framing Error: Check if the Bit at Chn_TxRxBitsDoneCtr Bit Position in the Chn_RxDataHoldReg is set;
   	QBBC    BIT_CLEARD, rx_context.Chn_RxDataHoldReg, scratch_reg3

	//increament Chn_TxRxBytesDoneCtr by one,  
	ADD 	suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr, 1
	SBBO    suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBYTESDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBytesDoneCtr)
	
	// Reset the Data Hold Reg	
	XOR     rx_context.Chn_RxDataHoldReg, rx_context.Chn_RxDataHoldReg, rx_context.Chn_RxDataHoldReg
	SBBO    rx_context.Chn_RxDataHoldReg, suart_ch_info.rx_context_addr, SUART_CH_RXDATAHOLDREG_OFFSET, SIZE (rx_context.Chn_RxDataHoldReg)	
	// Read the request count to be received 
	LSR 	scratch_reg1, suart_ch_regs.Chn_Config2, SUART_CH_CONFIG2_DATALEN_SHIFT // 0x8
	AND     scratch_reg1, scratch_reg1, SUART_CH_CONFIG2_DATALEN_MASK //0x0F
	ADD     scratch_reg1, scratch_reg1, 0x01
	
	// Read the bytes done counter
	MOV     scratch_reg2, suart_ch_regs.Chn_TxRxBytesDoneCtr
	
	// check if bytes done counter is less than or equal to data len, 
	// if yes go to CHK_RX_CMPL_INT and check for raise RX complete intr
	QBGE    CHK_RX_CMPL_INT, suart_ch_regs.Chn_TxRxBytesDoneCtr, scratch_reg1
     
	// if bytes done counter is greater than data len subtract data len from it and 
	// check if differnce is data len, if yes raise RX complete intr
	SUB    scratch_reg2, suart_ch_regs.Chn_TxRxBytesDoneCtr, scratch_reg1
	
CHK_RX_CMPL_INT:	
	// check if all data frame received or not, if RX request if complete, else receive next data frame
	QBLT   	RxInterruptServiceRequestHndlr, scratch_reg1, scratch_reg2
	
	// All requested frame received raise interrupt to ARM/DSP, set SUART_RX_FIFO_INDX_BIT, clear SUART_TXRX_READY_BIT
RX_COMPLETE: 
	// RX Data is in lower half of Fifo, if bytes done counter is equal to data len
	CLR    suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_RX_FIFO_INDX_BIT
	
	// Raise the RX interrupt if Chn_TxRxBytesDoneCtr is equal to data len otherwise reset Chn_TxRxBytesDoneCtr and raise Rx interrupt
	QBEQ   CHK_RX_OVERRUN, suart_ch_regs.Chn_TxRxBytesDoneCtr, scratch_reg1
	
	// reset Chn_TxRxBytesDoneCtr if Chn_TxRxBytesDoneCtr is equal to twice the data len
	XOR    suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_regs.Chn_TxRxBytesDoneCtr
	SBBO   suart_ch_regs.Chn_TxRxBytesDoneCtr, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXBYTESDONECTR_OFFSET, SIZE (suart_ch_regs.Chn_TxRxBytesDoneCtr)
	
	// RX data is in upper half of Fifo,if bytes done counter is equal to twice the data len
	SET    suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_RX_FIFO_INDX_BIT
CHK_RX_OVERRUN:
        MOV32   scratch_reg2, SECR2_OFFSET
        LBCO    scratch_reg1, CONST_PRUSSINTC, scratch_reg2, 4
        ADD     scratch_reg2,  suart_ch_info.ch_num, 2
        ADD     scratch_reg2,  suart_ch_info.ch_num, 2

OVER_RUN_ERR:
        QBBC    CHK_RX_READY_BIT, scratch_reg1, scratch_reg2
	QBBC    CHK_RX_READY_BIT, suart_ch_regs.Chn_Config1, RX_OVER_RUN_MASK
	SET     suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_RUNER_BIT
	SET     suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_ERROR_BIT

CHK_RX_READY_BIT:	
	// If the receive is not activated from the host, then don't dump the data 
	QBBC   RxInterruptServiceRequestHndlr, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_READY_BIT
        JMP    RX_CHN_INTR

        // Framing Error Detected, interrupt are masked go to DEACTIVATE_SERIALIZER, other wise update status reg
BIT_CLEARD: 
	SET     suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_RX_FE_BIT
        JMP     SET_RX_ERR_STAT

        //Break Condiotion Error detected, interrupt are masked go to DEACTIVATE_SERIALIZER, other wise update status reg
RX_DATA_ZERO:
	SET     suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_RX_BI_BIT
	
        // Update the Global and Channel Error Status Registers
SET_RX_ERR_STAT:
	SET	    suart_global.intrStatus, suart_global.intrStatus, GLOBAL_ERR_INTR
	SET     suart_ch_regs.Chn_TxRxStatus, suart_ch_regs.Chn_TxRxStatus, SUART_TXRX_ERROR_BIT

        // if global Error interrupt is clear do not raise interrupt
	QBBC    RxInterruptServiceRequestHndlr, suart_global.intrMask, GLOBAL_ERR_INTR
    
        // if Framing error status bit for channel is clear look for Break Condiotion Error
        QBBC    BREAK_COND_ERR, suart_ch_regs.Chn_TxRxStatus, SUART_RX_FE_BIT
    
        // Framming Error Occurred, if Framing Error mask is not set jum to RxInterruptServiceRequestHndlr
FRAMING_ERR:
        QBBS    RX_CHN_INTR, suart_ch_regs.Chn_Config1, FE_ERR_INTR_MASK
        JMP     RxInterruptServiceRequestHndlr

        // if Beark Error Mask not set jump to RxInterruptServiceRequestHndlr
BREAK_COND_ERR:
        QBBC    RxInterruptServiceRequestHndlr, suart_ch_regs.Chn_Config1, BI_ERR_INTR_MASK

	// Set the Global interrupt status register
RX_CHN_INTR:
	SET	    suart_global.intrStatus, suart_global.intrStatus, suart_ch_info.ch_num
	
	//write interrupt status regsiter
   	SBBO    suart_global.intrStatus, pZERO, SUART_GBL_INT_STATUS_ADDR, 2  
	
	//Update tx rx status regsiter status 
	SBBO    suart_ch_regs.Chn_TxRxStatus, suart_ch_info.curr_ch_base_addr, SUART_CH_TXRXSTATUS_OFFSET, SIZE (suart_ch_regs.Chn_TxRxStatus)	
	
	// if interrupt are masked for channel then go to RxInterruptServiceRequestHndlr, otherwise raise the interrupt
	QBBC    RxInterruptServiceRequestHndlr, suart_global.intrMask, suart_ch_info.ch_num
	
	// Raise the interrupt to ARM/DSP
	JMP     PRU_TO_HOST_INTERRUPT			

//******************************************** RxInterruptServiceRequestHndlr: ENDs **************************
	
//========================================================================================================================================
	
//******************************************** LOAD_RX_CONTEXT_ADDRESS: Start*********************************
LOAD_RX_CONTEXT_ADDRESS:
	QBEQ	RX_CONTEXT_CH1_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_1
	QBEQ	RX_CONTEXT_CH3_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_3
	QBEQ	RX_CONTEXT_CH5_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_5
	QBEQ	RX_CONTEXT_CH7_ADDR, suart_ch_info.ch_num, SUART_CHANNEL_7
.ret

RX_CONTEXT_CH1_ADDR:	
	LDI 	suart_ch_info.rx_context_addr, SUART_CH1_RX_CONTEXT_ADDR  			
.ret
	
RX_CONTEXT_CH3_ADDR:	
	LDI 	suart_ch_info.rx_context_addr, SUART_CH3_RX_CONTEXT_ADDR  			
.ret

RX_CONTEXT_CH5_ADDR:	
	LDI 	suart_ch_info.rx_context_addr, SUART_CH5_RX_CONTEXT_ADDR  			
.ret

RX_CONTEXT_CH7_ADDR:	
	LDI 	suart_ch_info.rx_context_addr, SUART_CH7_RX_CONTEXT_ADDR  			
.ret
//******************************************** LOAD_RX_CONTEXT_ADDRESS Ends ***********************************

//=====================================================================================================================================

//			************************************************************************************************
//									SOFT-UART RECEIVE ROUTINE : ENDS
//			************************************************************************************************

//=====================================================================================================================================	
