/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Source: PRU_CAN_Emulation_Rx.p///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///Revision: 1.0/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///Author : Wilfred Felix Raj//////////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
///// Global Macro definitions///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
///// Header Files //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#include "PRU_CAN_Emulation_Rx.hp"

/////////////////////////////////////////////////////////////////////////////////////////
////Assign Registers to predefined mailbox structure///////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////

.assign CAN_MAIL_BOX_STRUCTURE,R4,*,canmailbox
.assign CAN_MAIL_BOX_STRUCTURE,MailboxShiftRegister,*,mailboxShiftreg

/////////////////////////////////////////////////////////////////////////////////////////
////Receive Emulation Entry Point /////////////////////////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////
.setcallreg CRCFrame.w2
.origin 0
.entrypoint CAN_EMULATION_RX_INIT
   
//==================================================================================================================

//***************************Initialize the Registers Associated with CAN receive Emulation*******************

//==================================================================================================================
CAN_EMULATION_RX_INIT:
	  
	// Load CAN Period 
	MOV32  		RegisterOffset, PRU_CAN_BIT_TIMING_VAL_RX
	LBBO   		CanBitPeriodRegister, RegisterOffset, 0, 4 

	// load the CAN Phase Seg Delay
    MOV32  		RegisterOffset, PRU_CAN_PHASE_SEG_TIMING_VAL_RX
    LBBO   		CanPhaseSegRegister, RegisterOffset, 0, 4	
	
	// Reset Error The Error Counter
	XOR 		ErrorCounter,ErrorCounter, ErrorCounter
	MOV32   	RegisterValue, PRU_CAN_RX_ERROR_COUNTER_REGISTER        
	SBBO    	ErrorCounter,RegisterValue,0,4 
		
//==================================================================================================================

//******************************************TIMER INITIALIZATION****************************************************

//==================================================================================================================	
TIMER_CONFIG:
    // Init the register pointer
    MOV32       RegisterOffset, TMR_REGS

    // Place Timer in reset
	XOR  		RegisterValue, RegisterValue, RegisterValue
    SBBO      	RegisterValue, RegisterOffset, TMR_TGCR_OFFSET,  2

    // Reset the Timer counter registers
    SBBO      	RegisterValue, RegisterOffset, TMR_TIM12_OFFSET,  4

    // Setup the TCR register
    SBBO      	RegisterValue, RegisterOffset, TMR_TCR_OFFSET,  2

    // Setting the Timer global control register
    LDI       	RegisterValue.w0, 0x0015
    SBBO      	RegisterValue, RegisterOffset, TMR_TGCR_OFFSET,  2

    // Load the time constant into the period register
    SBBO      	CanBitPeriodRegister, RegisterOffset, TMR_PRD12_OFFSET,  4

    // Clear any pending interrupts,
    MOV     	RegisterValue.w0, 0x00000002
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2

    // Enable the Timer0 1:2 interrupt
    LDI       	RegisterValue.w0, 0x0001
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2

    // Enable the Timer0 in auto reload mode
    LDI       	RegisterValue.w0, TIMER12_MODE
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2
	
	// Clear Timer interrupt
    MOV32     	RegisterValue, (0x00000000 | TIMER_SYS_EVT)
    SBCO      	RegisterValue, CONST_PRUSSINTC, SICR_OFFSET, 4
	
	// Enable Timer 0 system interrupt
    SBCO      	RegisterValue, CONST_PRUSSINTC, EISR_OFFSET, 4	
//==================================================================================================================

//************************************Starting Loop of CAN Receive emulation ***************************************

//==================================================================================================================
CAN_EMULATION_RX_START:
	
	// Wait For Consecutive 11 bit high i.e. do not start, monitoring the bus from in between of a frame
RECIEVE_GPIO_POLL:
    MOV   		FrameLength.w0, 0
	
RECIEVE_GPIO_POLL_LOOP:  
    CALL    	TIMER_EVENT_WAIT
	QBBC    	RECIEVE_GPIO_POLL, r31, GPIO_RX_BIT
	ADD     	FrameLength.w0,FrameLength.w0, 1
	QBEQ    	BUS_OFF_CHECK, FrameLength.w0, 7           // 7 bit high received, check for Bus Off
	JMP			RECIEVE_GPIO_POLL_LOOP

//==================================================================================================================

//************************************Poll for receipt of activity on CAN Bus***************************************

//==================================================================================================================
BUS_OFF_CHECK:
	MOV32      	RegisterOffset, PRU_CAN_TX_GLOBAL_STATUS_REGISTER
    LBBO      	RegisterValue, RegisterOffset, 0,  4
	QBBC        START_OF_FRAME, RegisterValue, CAN_BUS_OFF_ERR_BIT
	
BUS_OFF_CHECK_LOOP:	
	MOV32      	RegisterOffset, PRU_CAN_TX_GLOBAL_STATUS_REGISTER
    LBBO      	RegisterValue, RegisterOffset, 0,  4
	QBBS        BUS_OFF_CHECK_LOOP, RegisterValue, CAN_BUS_OFF_ERR_BIT
	
RESET_BUS_OFF_STATE:	
	// Reset RX Bus off Flag
	MOV32      	RegisterOffset, PRU_CAN_RX_GLOBAL_STATUS_REGISTER
	LBBO		RegisterValue, RegisterOffset, 0, 4
	CLR			RegisterValue, 	CAN_BUS_OFF_ERR_BIT
	CLR			RegisterValue, 	CAN_PASSIVE_ERR_BIT
	SBBO      	RegisterValue, RegisterOffset, 0, 4
	
	// Reset RX Error Counter
	MOV32  		RegisterOffset, PRU_CAN_RX_ERROR_COUNTER_REGISTER
	XOR 		ErrorCounter,ErrorCounter, ErrorCounter
	SBBO   		ErrorCounter,RegisterOffset,0,4	
		
START_OF_FRAME:
	// Reset the variables
	XOR     	BitsReceivedCntr, BitsReceivedCntr, BitsReceivedCntr
	XOR   		FrameLength,FrameLength, FrameLength
	XOR   		CanCurrentState, CanCurrentState, CanCurrentState
    XOR   		BitStuffCounter, BitStuffCounter, BitStuffCounter
	XOR   		CanFrame, CanFrame, CanFrame
	XOR			CRC_TempVar, CRC_TempVar, CRC_TempVar
	XOR 		CanFrameState, CanFrameState, CanFrameState
	XOR   		CRCFrame, CRCFrame, CRCFrame
    XOR   		CRC_Input, CRC_Input, CRC_Input
    
    // Init the register pointer
    MOV32     	RegisterOffset, TMR_REGS
	
	// Disable the Timer0
    XOR  		RegisterValue, RegisterValue, RegisterValue
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2
	
	// Reset the Timer0 counter registers
    SBBO      	RegisterValue, RegisterOffset, TMR_TIM12_OFFSET,  4
	
	// Clear any pending interrupts
    MOV     	RegisterValue.w0, 0x00000002
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2

	//wait for CAN Start of Frame condition
	WBC 	 	r31, GPIO_RX_BIT		  
	
	// load the CAN Phase Seg Delay
    MOV32  		RegisterOffset, PRU_CAN_PHASE_SEG_TIMING_VAL_RX
    LBBO   		CanPhaseSegRegister, RegisterOffset, 0, 4	
	
	// Load CAN Period PRU_CAN_BIT_TIMING_VAL_RX
	ADD  		RegisterOffset, RegisterOffset, 4
	LBBO   		CanBitPeriodRegister, RegisterOffset, 0, 4 
	
	// Load the PHASE SEG 1 DELAY into the period register
    MOV32     	RegisterOffset, TMR_REGS
	
	XOR			RegisterValue, RegisterValue, RegisterValue
	ADD      	RegisterValue.w0, RegisterValue.w0, CanPhaseSegRegister.w2           //phaseSeg 1 delay
    SBBO     	RegisterValue, RegisterOffset, TMR_PRD12_OFFSET,  4			//Bit Delay counter
	
	// Start the Timer in One Shot mode
    LDI       	RegisterValue.w0, TIMER12_MODE_ONESHOT												//Reload the bit delay
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2	
	
	// ENABLE the Timer interrupt and clear status bit for Timer12
	MOV  		RegisterValue.w0, 0x00000003
    SBBO   		RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2
	
	// Increament Stuff Bit Count as Start of 
	ADD    		BitStuffCounter.w0, BitStuffCounter.w0, 1
	
	//Wait for PhaseSeg1 DELAY
	CALL   		TIMER_EVENT_WAIT            
	
//===============================================================
			// load the Timer with Continuous mode 0x80
//===============================================================
    // Init the register pointer
    MOV32     	RegisterOffset, TMR_REGS
	
	// Disable the Timer
    XOR			RegisterValue, RegisterValue, RegisterValue
    SBBO      	RegisterValue, RegisterOffset, TMR_TCR_OFFSET,  2
	
	// Reset the Timer counter registers	
    SBBO      	RegisterValue, RegisterOffset, TMR_TIM12_OFFSET,  4
	
	// Clear any pending interrupts
    MOV     	RegisterValue.w0, 0x00000002
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2
		
    // Load the BIT Time Constant into the Relaod register
    SBBO      	CanBitPeriodRegister, RegisterOffset, TMR_PRD12_OFFSET,  4	
	
	// Load the Timer with Continuous mode 
    LDI       	RegisterValue.w0, TIMER12_MODE												//Reload the bit delay
    SBBO      	RegisterValue, RegisterOffset, TMR_TCR_OFFSET,  2	
	
	// ENABLE the Timer interrupt and clear status bit for Timer12
	MOV32  		RegisterValue, 0x00000003
    SBBO   		RegisterValue, RegisterOffset, TMR_INTCTL_OFFSET,  2
//====================================================================	
	
CAN_RECIEVE_DATA:
	CALL   		TIMER_EVENT_WAIT                // DELAY For ONE BIT

	// Read the BIT from the CAN BUS
CAN_BIT_RECEIVE:		
    QBBS    	RECIEVE_BIT_ONE, r31, GPIO_RX_BIT
	
	// Debug code to be removed		
	QBNE    	BIT_RECIEVED_ZERO, BitsReceivedCntr.w0, r9   
	JMP 		BIT_RECIEVED_ZERO
		
	// CAN BIT received is ZERO 
BIT_RECIEVED_ZERO:
    QBEQ    	CAL_CRC_BIT0, FrameLength.w0, 0		
    MOV     	RegisterValue, FrameLength.w0
    SUB     	RegisterValue, RegisterValue, 16  //DO not Calcute CRC for CRC Bits
    QBGT    	CRC_SKIP_0, RegisterValue, BitsReceivedCntr.w0	
	
    CAL_CRC_BIT0:
	CALL    	CRC_CALCULATE_0
		
    CRC_SKIP_0:
	// Increament the BitsReceivedCntr & BitStuffCounter.w0(Zero Bit), reset BitStuffCounter.w2 (ONE Bit)
    ADD     	BitsReceivedCntr.w0, BitsReceivedCntr.w0, 1
	ADD     	BitStuffCounter.w0, BitStuffCounter.w0, 1
	XOR     	BitStuffCounter.w2, BitStuffCounter.w2, BitStuffCounter.w2
	
//=================================================================================================================

//**************************Analyse the bits to identify the type of Frame and DLC******************************

//================================================================================================================== 
DATA_ANALYSIS_0:
	QBEQ    	BASE_ID_IDENTIFIED, BitsReceivedCntr.w0, 12
    QBEQ    	EXTD_DATA_ANALYSED_0, CanFrameState.b0, EXTD_DATA_FRAME
	QBEQ    	EXTD_DATA_ANALYSED_0, CanFrameState.b0, EXTD_REMOTE_FRAME
    QBEQ    	DLC_IDENTIFIED, BitsReceivedCntr.w0, 18
    QBEQ    	CHECK_STUFF_BIT_ZERO, CanFrameState.b0, STD_DATA_FRAME
	JMP     	CHECK_STUFF_BIT_ZERO  
		
EXTD_DATA_ANALYSED_0:
    QBEQ    	DLC_EXTD_IDENTIFIED, BitsReceivedCntr.w0, 38
    JMP     	CHECK_STUFF_BIT_ZERO

DLC_IDENTIFIED:
    QBEQ    	DLC_STD_FRAME, CanFrameState.b0, STD_DATA_FRAME
    QBEQ    	DLC_REMOTE_FRAME, CanFrameState.b0, STD_REMOTE_FRAME
	JMP     	CHECK_STUFF_BIT_ZERO

DLC_REMOTE_FRAME:   
	MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28    
    MOV     	FrameLength.w0,0        
    ADD     	FrameLength.w0,FrameLength.w0,33
    JMP     	CHECK_STUFF_BIT_ZERO

DLC_STD_FRAME:
    MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28
    MOV     	FrameLength.w0,0        
    MOV     	FrameLength.w0,DataLength.b0
    LSL     	FrameLength.w0,FrameLength.w0,3
    ADD     	FrameLength.w0,FrameLength.w0,33
    JMP     	CHECK_STUFF_BIT_ZERO 

DLC_EXTD_IDENTIFIED:
    QBEQ    	DLC_EXTD_FRAME,CanFrameState.b0, EXTD_DATA_FRAME
    QBEQ    	DLC_EXTD_REMOTE_FRAME,CanFrameState.b0, EXTD_REMOTE_FRAME
    JMP     	CHECK_STUFF_BIT_ZERO

DLC_EXTD_REMOTE_FRAME:   
	MOV32   	DataLength,0
    MOV     	DataLength.b0, CanFrame.b0
    LSL     	DataLength,DataLength, 28
    LSR     	DataLength, DataLength, 28
    MOV     	FrameLength.w0, 0        
    ADD     	FrameLength.w0, FrameLength.w0, 53
    JMP     	CHECK_STUFF_BIT_ZERO

DLC_EXTD_FRAME:
    MOV32       DataLength, 0
    MOV     	DataLength.b0, CanFrame.b0
    MOV     	FrameLength.w0, 0        
    MOV     	FrameLength.w0, DataLength.b0
    LSL     	DataLength, DataLength, 28
    LSR     	DataLength, DataLength, 28
    LSL     	FrameLength.w0, FrameLength.w0, 3
    ADD     	FrameLength.w0, FrameLength.w0, 53
    JMP     	CHECK_STUFF_BIT_ZERO 
                        
BASE_ID_IDENTIFIED:
    MOV     	CanFrameState.b0, STD_DATA_FRAME

CHECK_STUFF_BIT_ZERO:
    QBEQ    	CHECK_STUFF_BIT1, BitStuffCounter.w0, 5
    
	// if Bits received in multiple of 32, put can frame to PRU RAM
    QBEQ    	ALL_BITS_RECIEVED, BitsReceivedCntr.w0, 32
    QBEQ    	ALL_BITS_RECIEVED, BitsReceivedCntr.w0, 64
    QBEQ    	ALL_BITS_RECIEVED, BitsReceivedCntr.w0, 96
    QBEQ    	ALL_BITS_RECIEVED, BitsReceivedCntr.w0, 128
    QBEQ    	ALL_BITS_RECIEVED, BitsReceivedCntr.w0, 160
	
	// Shift the Bit by one to put current received bit in Can Frame
    LSL     	CanFrame, CanFrame, 1
    JMP     	WAIT_TIMER_EVENT
	
	// Delay for ONE BIT, and check  the sense the CAN Bus for Stuf Bit One	
CHECK_STUFF_BIT1:
	CALL      	TIMER_EVENT_WAIT
    QBBC  	  	CAN_ERROR_FRAME, r31, GPIO_RX_BIT
	
	// NO Error Reset Stuff bit counter, JUMP to CHECK_STUFF_BIT_ZERO
    MOV     	BitStuffCounter.w2, 1  // Increament Stuff bit counter for BIT ONE
    MOV     	BitStuffCounter.w0, 0  // RESET Stuff bit counter for BIT ZERO	
    JMP     	CHECK_STUFF_BIT_ZERO
	
//==========================================================================================================

/// Write the Bits Recieved in CAN Frame to PRU RAM

//==========================================================================================================================
ALL_BITS_RECIEVED:
    MOV32   	RegisterOffset,PRU_CAN_RECEIVE_FRAME
    SBBO    	CanFrame,RegisterOffset,CanCurrentState.b1,4 
    ADD     	CanCurrentState.b1,CanCurrentState.b1,4
    XOR   		CanFrame,CanFrame, CanFrame
	
//==========================================================================================================================
//		Check if Complete CAN Frame is being recieved if yes generate ACK
//==========================================================================================================================
WAIT_TIMER_EVENT:
    MOV     	FrameLength.w2, FrameLength.w0		
    QBEQ    	DATA_RECEIVE_COMPLETED_CRC, BitsReceivedCntr.w0, FrameLength.w0	
	JMP     	CAN_RECIEVE_DATA 
//==========================================================================================================================	
	                
////////////////////////////////////////////////////////////////////
//**** Check for Error and Generate Appropriate Error Frame ******//
////////////////////////////////////////////////////////////////////

CAN_ERROR_FRAME:
	//Tx is transmitting so don't generate error frame
	MOV32       RegisterValue,PRU_CAN_TX_BUS_AQUIRED
	LBBO        RegisterOffset,RegisterValue,0,4  
	QBEQ		CAN_EMULATION_RX_START,RegisterOffset,1
	
	// Read the Rx Error Counter, increament Error Counter
	MOV32       RegisterValue,PRU_CAN_RX_ERROR_COUNTER_REGISTER
    LBBO        ErrorCounter,RegisterValue,0,4
    ADD         ErrorCounter,ErrorCounter, 1
    SBBO        ErrorCounter,RegisterValue,0,4

	// Load the Error Frame Length
	MOV         BitsReceivedCntr.w0, ERR_FRAME_LENGTH
	
	// Change The Pin Muxing
	MOV32       RegisterValue, KICKR0
    MOV32       RegisterOffset,KICK0R_UNLOCK
    SBBO        RegisterOffset,RegisterValue,0,4 

	MOV32       RegisterValue, KICKR1
    MOV32       RegisterOffset,KICK1R_UNLOCK
    SBBO        RegisterOffset,RegisterValue,0,4 
	
	// Configure the PRU1-TX line to GPIO 5.7 mode for driving ACK
    MOV32       RegisterValue,  PINMUX12REG
	LBBO		RegisterOffset, RegisterValue,0,4
	LSR			RegisterOffset, RegisterOffset, 4
	LSL			RegisterOffset, RegisterOffset, 4
	SET			RegisterOffset, PINMUX12REG_GP5_7_VAL
	SBBO        RegisterOffset,RegisterValue,0,4              
	
	// Set  GPIO DIR as O/p
	MOV32       RegisterValue, GPIOBANK_45_DIR_REG 
    MOV32       RegisterOffset,0x000000
    SBBO        RegisterOffset,RegisterValue,0,4
	
	CALL        TIMER_EVENT_WAIT 
	
	MOV32      	RegisterOffset, PRU_CAN_TX_GLOBAL_STATUS_REGISTER
	LBBO		RegisterValue, RegisterOffset, 0, 4
	QBBS        CAN_GENERATE_PASSIVE_ERROR, RegisterValue, CAN_PASSIVE_ERR_BIT
	
    QBGT        CAN_GENERATE_ACTIVE_ERROR, ErrorCounter,PRU_CAN_ERROR_ACTIVE
	
	// Drive the Can Bus for 6 Bit duration high to Generate Passive Error Frame
CAN_GENERATE_PASSIVE_ERROR:	
	MOV32      	RegisterOffset, PRU_CAN_RX_GLOBAL_STATUS_REGISTER
	LBBO		RegisterValue, RegisterOffset, 0, 4
	SET			RegisterValue, 	CAN_PASSIVE_ERR_BIT
	SBBO      	RegisterValue, RegisterOffset, 0, 4
	
	QBEQ		CAN_GENERATE_ACTIVE_ERROR, ErrorCounter, PRU_CAN_ERROR_ACTIVE
	MOV32       RegisterValue,  GPIOBANK_45_SET_DATA_REG
    MOV32       RegisterOffset, GPIOBANK_45_GPIO5_7
	
CAN_GENERATE_PASSIVE_ERROR_FRAME:
    SBBO        RegisterOffset,	RegisterValue,0,4
	CALL      	TIMER_EVENT_WAIT
    SUB         BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
    QBNE        CAN_GENERATE_PASSIVE_ERROR_FRAME,BitsReceivedCntr.w0,0
	JMP         ERROR_EXIT	
	
    // Drive the Can Bus for 6 bit duration low to Generate Active Error Frame                                
CAN_GENERATE_ACTIVE_ERROR:
//	MOV32       RegisterValue, GPIOBANK_45_CLR_DATA_REG
//    MOV32       RegisterOffset,GPIOBANK_45_GPIO5_7

	//tEmp CODE
	MOV32       RegisterValue,  GPIOBANK_45_SET_DATA_REG
    MOV32       RegisterOffset, GPIOBANK_45_GPIO5_7
	
CAN_GENERATE_ACTIVE_ERROR_FRAME:	
    SBBO        RegisterOffset,RegisterValue,0,4 
	CALL      	TIMER_EVENT_WAIT
	SUB         BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
    QBNE        CAN_GENERATE_ACTIVE_ERROR_FRAME,BitsReceivedCntr.w0,0
	
	// Generate Error 8 Bit Duration Error Delimiter
ERROR_EXIT:
	MOV32       RegisterValue,  GPIOBANK_45_SET_DATA_REG
    MOV32       RegisterOffset, GPIOBANK_45_GPIO5_7	
	SBBO        RegisterOffset,	RegisterValue,0,4
	
	// Configure the pin back to PRU1-TX line 
    MOV32       RegisterValue, PINMUX12REG
	LBBO		RegisterOffset, RegisterValue,0,4
	LSR			RegisterOffset, RegisterOffset, 4
	LSL			RegisterOffset, RegisterOffset, 4
	SET			RegisterOffset, PINMUX12REG_PRU_VAL
	SBBO        RegisterOffset,RegisterValue,0,4              

	MOV32       RegisterValue, KICKR0
    MOV32       RegisterOffset,KICK0R_LOCK
    SBBO        RegisterOffset,RegisterValue,0,4 

	MOV32       RegisterValue, KICKR1
    MOV32       RegisterOffset,KICK1R_LOCK
    SBBO        RegisterOffset,RegisterValue,0,4 
	
	//dEBUG	
	XOR	RegisterValue, RegisterValue,RegisterValue
	SET    RegisterValue, RegisterValue, 21
	XOR	   r30, r30,RegisterValue	

///==================================================
	// Generate 8 bit High Error Frame
ERROR_DELIMITER:
//	XOR     	ErrorCounter.w2, ErrorCounter.w2, ErrorCounter.w2			
	
ERROR_DELIMITER_FRAME:	
	MOV     	RegisterValue.w0, 0
	XOR     	ErrorCounter.w0, ErrorCounter.w0, ErrorCounter.w0
	
ERROR_DELIMITER_FRAME_LOOP:
	XOR       	CRC_Input.w0, CRC_Input.w0, CRC_Input.w0
	CALL		POLL_CAN_BUS_STATUS
	QBEQ      	NO_DELIMITER_ERR, CRC_Input.w0, 0x00
	ADD			ErrorCounter.w0, ErrorCounter.w0, 1
	
NO_DELIMITER_ERR:
	ADD     	RegisterValue, RegisterValue, 1
	QBGT    	ERROR_DELIMITER_FRAME_LOOP, RegisterValue.w0, ERR_DELMTR_LEN
//	QBEQ		ERROR_DELIMITER_FRAME_EXIT, ErrorCounter.w0, 0	
//	ADD			ErrorCounter.w2, ErrorCounter.w2, 8
//	QBGT		ERROR_DELIMITER_FRAME_EXIT, ErrorCounter.w0, 8
//	QBEQ		ERROR_DELIMITER_FRAME, ErrorCounter.w0, 8
	
ERROR_DELIMITER_FRAME_EXIT:	
	MOV32  		RegisterOffset,PRU_CAN_RX_ERROR_COUNTER_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	ADD			RegisterValue,RegisterValue, ErrorCounter.w2
	SBBO   		RegisterValue,RegisterOffset,0,4	

	//dEBUG	
	XOR	RegisterValue, RegisterValue,RegisterValue
	SET    RegisterValue, RegisterValue, 21
	XOR	   r30, r30,RegisterValue	

	// IFS
	CALL      	TIMER_EVENT_WAIT
	CALL      	TIMER_EVENT_WAIT
	CALL      	TIMER_EVENT_WAIT
//===========================================================	

//==============================================================================	

	MOV32		TempVar,0      
    JMP     	CAN_EMULATION_RX_START
	
//========================================================================================================
    ///////// Receive Data Bit as One ////////////////////////////////////
//========================================================================================================
RECIEVE_BIT_ONE:
	//debug code
	QBNE    DATA_ANALYSIS_1abc, BitsReceivedCntr.w0, r9  // to be removed
	JMP 	DATA_ANALYSIS_1abc
DATA_ANALYSIS_1abc:	
    SET     	CanFrame,0      
    QBEQ    	CAL_CRC_BIT1, FrameLength.w0, 0
    MOV     	RegisterValue, FrameLength.w0
    SUB     	RegisterValue, RegisterValue, CAN_CRC_LEN
    QBGT    	CRC_SKIP_BIT1, RegisterValue, BitsReceivedCntr.w0
	
CAL_CRC_BIT1:
    CALL    	CRC_CALCULATE_1
	
CRC_SKIP_BIT1:
	// Increament the BitsReceivedCntr & BitStuffCounter.w2(ONE Bit), reset BitStuffCounter.w0 (Zero Bit)
	ADD     	BitsReceivedCntr.w0, BitsReceivedCntr.w0, 1        
    ADD     	BitStuffCounter.w2,  BitStuffCounter.w2, 1
	XOR     	BitStuffCounter.w0,  BitStuffCounter.w0, BitStuffCounter.w0
	
//*********************************************************************************************************	
    /////////// Analyse Data bit State : 1 /////////////////////////                
//*********************************************************************************************************

DATA_ANALYSIS_1:  
    QBEQ    	REMOTE_STD_FRAME_IDENTIFIED, BitsReceivedCntr.w0, 12	 
    QBEQ    	EXTD_ID_IDENTIFIED,BitsReceivedCntr.w0,13
    QBEQ    	EXTD_DATA_ANALYSED_1, CanFrameState.b0, EXTD_DATA_FRAME
	QBEQ    	EXTD_DATA_ANALYSED_1, CanFrameState.b0, EXTD_REMOTE_FRAME
    QBEQ    	DLC_IDENTIFIED_1, BitsReceivedCntr.w0, 18
    QBEQ    	STUFF_BIT1_CHECK, CanFrameState.b0, STD_DATA_FRAME	 
	JMP     	STUFF_BIT1_CHECK
	
EXTD_DATA_ANALYSED_1:
    QBEQ    	REMOTE_EXTD_VERIFY,BitsReceivedCntr.w0,32
    QBEQ    	DLC_IDENTIFIED_1,BitsReceivedCntr.w0,38
    JMP     	STUFF_BIT1_CHECK

REMOTE_STD_FRAME_IDENTIFIED:
    MOV     	CanFrameState.b0,STD_REMOTE_FRAME	
    JMP     	STUFF_BIT1_CHECK
        
EXTD_ID_IDENTIFIED:
    MOV     	CanFrameState.b0,EXTD_DATA_FRAME
    JMP     	STUFF_BIT1_CHECK

REMOTE_EXTD_VERIFY:
    QBEQ    	REMOTE_EXTD_FRAME_IDENTIFIED,CanFrameState.b0,EXTD_DATA_FRAME
	QBEQ    	STUFF_BIT1_CHECK, CanFrameState.b0,STD_REMOTE_FRAME
    MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    MOV     	FrameLength.w0,0        
    MOV     	FrameLength.w0,DataLength.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28
    LSL     	FrameLength.w0,FrameLength.w0,3
    ADD     	FrameLength.w0,FrameLength.w0,33        
    JMP     	STUFF_BIT1_CHECK
            
REMOTE_EXTD_FRAME_IDENTIFIED:
    MOV     	CanFrameState.b0,EXTD_REMOTE_FRAME  
    JMP     	STUFF_BIT1_CHECK

DLC_IDENTIFIED_1:
    QBEQ    	DLC_STD_FRAME_1,CanFrameState.b0,STD_DATA_FRAME
    QBEQ    	DLC_EXTD_FRAME_1,CanFrameState.b0,EXTD_DATA_FRAME
    QBEQ    	DLC_EXTD_REMOTE_FRAME_1,CanFrameState.b0,EXTD_REMOTE_FRAME
	QBEQ    	DLC_REMOTE_FRAME_1,CanFrameState.b0,STD_REMOTE_FRAME
    JMP     	STUFF_BIT1_CHECK

DLC_REMOTE_FRAME_1:   
	MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28   
    MOV     	FrameLength.w0,0        
    ADD     	FrameLength.w0,FrameLength.w0,33
    JMP     	STUFF_BIT1_CHECK

DLC_STD_FRAME_1:
    MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28
    MOV     	FrameLength.w0,0        
    MOV     	FrameLength.w0,DataLength.b0
    LSL     	FrameLength.w0,FrameLength.w0,3
    ADD     	FrameLength.w0,FrameLength.w0,33
    JMP     	STUFF_BIT1_CHECK 

DLC_EXTD_REMOTE_FRAME_1:
    MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28
    MOV     	FrameLength.w0,0            
    ADD     	FrameLength.w0,FrameLength.w0,53
    JMP     	STUFF_BIT1_CHECK 

DLC_EXTD_FRAME_1:
    MOV32   	DataLength,0
    MOV     	DataLength.b0,CanFrame.b0
    LSL     	DataLength,DataLength,28
    LSR     	DataLength,DataLength,28
    MOV     	FrameLength.w0,0        
    MOV     	FrameLength.w0,DataLength.b0
    LSL     	FrameLength.w0,FrameLength.w0,3
    ADD     	FrameLength.w0,FrameLength.w0,53
	
//*********************************************************************************************************	    
STUFF_BIT1_CHECK:	
	//Check For Stuff Error
    QBEQ    	CHECK_STUFF_BIT0,BitStuffCounter.w2,5       	   
	
	// if Bits received in multiple of 32, put can frame to PRU RAM	
    QBEQ   		ALL_BITS_RECIEVED,BitsReceivedCntr.w0,32
    QBEQ   		ALL_BITS_RECIEVED,BitsReceivedCntr.w0,64
    QBEQ   		ALL_BITS_RECIEVED,BitsReceivedCntr.w0,96
    QBEQ   		ALL_BITS_RECIEVED,BitsReceivedCntr.w0,128
    QBEQ   		ALL_BITS_RECIEVED,BitsReceivedCntr.w0,160
	
	// Shift the Bit by one to put current received bit in Can Frame
    LSL    		CanFrame,CanFrame,1
    JMP    		WAIT_TIMER_EVENT

    // Delay for ONE BIT, and the sense the CAN Bus for Stuff Bit ZERO	    
CHECK_STUFF_BIT0:
	CALL      	TIMER_EVENT_WAIT		
    QBBS  		CAN_ERROR_FRAME, r31, GPIO_RX_BIT
	
    // NO Error Reset Stuff bit counter, JUMP to STUFF_BIT1_CHECK
    MOV     	BitStuffCounter.w0,1      //Increament the bit stuff counter for BIT ZERO
    MOV     	BitStuffCounter.w2,0      // Reset the bit stuff counter for BIT ONE
    JMP     	STUFF_BIT1_CHECK
//=========================================================================================================
//////////////////////////////////////////////////////////////////////////////////////////
/////////// Full Frame has been received and data is ready for CRC Check /////////////////
//////////////////////////////////////////////////////////////////////////////////////////

DATA_RECEIVE_COMPLETED_CRC:        
    LSR   		TempVar,BitsReceivedCntr.w0,5
    LSL   		TempVar,TempVar,5
    SUB   		BitsReceivedCntr.w0,BitsReceivedCntr.w0,TempVar
    MOV32   	TempVar,32
    SUB     	BitsReceivedCntr.w0,TempVar,BitsReceivedCntr.w0
    SUB     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
    LSL     	CanFrame,CanFrame,BitsReceivedCntr.w0
        
    ////////////////////////////////////////////////////////////////
    //////// Write the Last piece of frame received into Mem ///////
    ////////////////////////////////////////////////////////////////

    MOV32   	RegisterOffset,PRU_CAN_RECEIVE_FRAME
    SBBO    	CanFrame,RegisterOffset,CanCurrentState.b1,4 
    ADD     	CanCurrentState.b1,CanCurrentState.b1,4
    XOR   		CanFrame,CanFrame, CanFrame			 
	
	// Read the CAN Frame storing the CAN ID
    LBBO    	RegisterValue,RegisterOffset, 0, 4
                
    QBBS    	EXTENDED_ID, RegisterValue, 18
    LSR     	CanCurrentState, RegisterValue, 21
    MOV32   	TempVar, 0
    JMP     	CRC_CHECK
                
EXTENDED_ID:
	MOV32		TempVar, 0xFFE00000
    AND     	CanCurrentState, RegisterValue, TempVar
	LSR     	CanCurrentState, CanCurrentState, 3
	LSR     	RegisterValue,RegisterValue,1
	MOV32   	TempVar, 0x3FFFF
    AND     	RegisterValue,RegisterValue,TempVar
    OR      	CanCurrentState,CanCurrentState,RegisterValue
    MOV32   	TempVar, 0

//=================================================================================================================

//****************************************Compare the CRC Received and Calculated CRC*******************************

//================================================================================================================== 
CRC_CHECK:               
    MOV32   	RegisterValue,0
    MOV32   	BitStuffCounter,0
    MOV     	Can_Bits_for_CRC.w2,FrameLength.w2
    LSR     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,5
    LSL     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,2       
    MOV32   	CanFrame,PRU_CAN_RECEIVE_FRAME
    LBBO    	CanFrame,CanFrame,Can_Bits_for_CRC.w2,4
    LSR     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,2
    LSL     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,5
    MOV     	Can_Bits_for_CRC.w0,Can_Bits_for_CRC.w2
    SUB     	Can_Bits_for_CRC.w0,FrameLength.w2,Can_Bits_for_CRC.w0

CRC_SHIFT_LOOP:
    MOV32   	BitStuffCounter,32
    SUB     	BitStuffCounter,BitStuffCounter,Can_Bits_for_CRC.w0     
    LSR     	CanFrame,CanFrame,BitStuffCounter
    MOV     	BitStuffCounter.w2,32
    SUB     	BitStuffCounter,BitStuffCounter.w2,BitStuffCounter.w0
    QBLT    	CRC_CHECK_PROCESS,Can_Bits_for_CRC.w0,16
    LSR     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,5       
    SUB     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,1
    LSL     	Can_Bits_for_CRC.w2,Can_Bits_for_CRC.w2,2
    MOV32   	RegisterValue,PRU_CAN_RECEIVE_FRAME
    LBBO    	RegisterValue,RegisterValue,Can_Bits_for_CRC.w2,4       
    LSL     	RegisterValue,RegisterValue,BitStuffCounter
    OR      	CanFrame,CanFrame,RegisterValue     
	
    CRC_CHECK_PROCESS:
	MOV			RegisterValue.w0, 0x7FFF
	AND			CanFrame.w0, CanFrame.w0, RegisterValue.w0
    QBEQ    	DLC_STD_FRAME_CRC, CanFrameState.b0, STD_DATA_FRAME   
        
    DLC_STD_FRAME_CRC:
    QBEQ    	SEND_ACKNOWLEDGEMENT, CRC_TempVar.w2, CanFrame.w0     

//////////////////////////////////////////////////////////////////////////////////////
////////////// Do not Acknowledge since ID does not match ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//if CRC do not match then Ignore the 6 Bits to syncronize with next TX Frame and Jump to
DO_NOT_ACKNOWLEDGE:   
    MOV32    	TempVar,6  
	
D_ACK_LOOP:
	CALL  		TIMER_EVENT_WAIT
	SUB	 		TempVar, TempVar, 1
	QBEQ		CAN_EMULATION_RX_START, TempVar, 0
	JMP	    	D_ACK_LOOP    
	
//=================================================================================================================

//****************************************Compare the CRC Received and Calculated CRC*******************************

CAN_EMULATION_RX_START_TEMP:
	JMP     	CAN_EMULATION_RX_START
//================================================================================================================== 

//////////////////////////////////////////////////////////////////////////////////////
////////////// Send the Acknowledgement since ID matches /////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

SEND_ACKNOWLEDGEMENT:  

	// check PRU is sender
	MOV32       RegisterValue, PRU_CAN_TX_BUS_AQUIRED
	LBBO        RegisterOffset,RegisterValue, 0, 4  
	QBEQ		CAN_EMULATION_RX_START, RegisterOffset,1
	
    // Init the register pointer
    MOV32     	RegisterOffset, TMR_REGS
	
	CALL  		TIMER_EVENT_WAIT
	
	// Disable the Timer0
    XOR			RegisterValue, RegisterValue, RegisterValue
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2

	// Reset the Timer counter registers
    SBBO      	RegisterValue, RegisterOffset, TMR_TIM12_OFFSET,  4
	
	// Clear any pending interrupts
    MOV     	RegisterValue.w0, 0x00000002
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2
	
	// Load the time constant into the period register; phaseSeg 2 delay
    ADD       	RegisterValue.w0, RegisterValue.w0, CanPhaseSegRegister.w0            
	
    // Load the time constant into the Reload register
    SBBO      	RegisterValue, RegisterOffset, TMR_PRD12_OFFSET,  4

	// Load The timer with One Shot Mode
    LDI       	RegisterValue.w0, TIMER12_MODE_ONESHOT
    SBBO      	RegisterValue, RegisterOffset, TMR_TCR_OFFSET,  2	
	
	// Enable the Timer Intr
	MOV32     	RegisterValue, 0x00000003
    SBBO      	RegisterValue, RegisterOffset, TMR_INTCTL_OFFSET,  2
	
	CALL  		TIMER_EVENT_WAIT   // Phase seg2 delay

//===============================================================
    // Init the register pointer
    MOV32     	RegisterOffset, TMR_REGS
	
	// Disable the Timer
    XOR			RegisterValue, RegisterValue, RegisterValue
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2
	
	// Reset the Timer counter registers
    SBBO      	RegisterValue, RegisterOffset, TMR_TIM12_OFFSET,  4
	
	// Clear any pending interrupts
    MOV     	RegisterValue.w0, 0x00000002
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2
		
    // Load the BIT Time Constant into the Relaod register
    SBBO      	CanBitPeriodRegister, RegisterOffset, TMR_PRD12_OFFSET,  4	

	// Load the Timer with Continuous mode
    LDI       	RegisterValue.w0, TIMER12_MODE												//Reload the bit delay
    SBBO      	RegisterValue, RegisterOffset, TMR_TCR_OFFSET,  2	
	
	// ENABLE the Timer interrupt and clear status bit for Timer12
	MOV32  		RegisterValue, 0x00000003
    SBBO   		RegisterValue, RegisterOffset, TMR_INTCTL_OFFSET,  2
//====================================================================	
		
	// Change the Pin Mix mode to TX to send ACK
	MOV32       RegisterValue, KICKR0
    MOV32       RegisterOffset,KICK0R_UNLOCK
    SBBO        RegisterOffset,RegisterValue,0,4 

	MOV32       RegisterValue, KICKR1
    MOV32       RegisterOffset,KICK1R_UNLOCK
    SBBO        RegisterOffset,RegisterValue,0,4 
	
	// Configure the PRU1-TX line to GPIO 5.7 mode for driving ACK
    MOV32       RegisterValue, PINMUX12REG
	LBBO		RegisterOffset, RegisterValue,0,4
	LSR			RegisterOffset, RegisterOffset, 4
	LSL			RegisterOffset, RegisterOffset, 4
	SET			RegisterOffset, PINMUX12REG_GP5_7_VAL
	SBBO        RegisterOffset,RegisterValue,0,4              
                       
	// set the GPIO5.7 to Output mode				   
    MOV32       RegisterValue, GPIOBANK_45_DIR_REG
    MOV32       RegisterOffset,0x000000
    SBBO        RegisterOffset,RegisterValue,0,4 
                        
	// drive the GPIO5.7 to LOW					
    MOV32       RegisterValue, GPIOBANK_45_CLR_DATA_REG
    MOV32       RegisterOffset,GPIOBANK_45_GPIO5_7
    SBBO        RegisterOffset,RegisterValue,0,4 
        

    //////////////////////////////////////////////////////
    ////////////////// Check ACK Receipt /////////////////
    //////////////////////////////////////////////////////
	
	// Wait for Timer Event TIMER
	CALL        TIMER_EVENT_WAIT

	// drive GPIO 5.7 to output HIGH
	MOV32       RegisterValue, GPIOBANK_45_SET_DATA_REG
    MOV32       RegisterOffset,GPIOBANK_45_GPIO5_7
    SBBO        RegisterOffset,RegisterValue,0,4 

	// Reset the pin-mux to convert the GPIO 5.7 back to PRU1-TX
    MOV32       RegisterValue, PINMUX12REG
	LBBO		RegisterOffset, RegisterValue,0,4
	LSR			RegisterOffset, RegisterOffset, 4
	LSL			RegisterOffset, RegisterOffset, 4
    SET       	RegisterOffset, PINMUX12REG_PRU_VAL
    SBBO        RegisterOffset,RegisterValue,0,4

	MOV32       RegisterValue, KICKR0
    MOV32       RegisterOffset,KICK0R_LOCK
    SBBO        RegisterOffset,RegisterValue,0,4 

	MOV32       RegisterValue, KICKR1
    MOV32       RegisterOffset,KICK1R_LOCK
    SBBO        RegisterOffset,RegisterValue,0,4 	
    MOV32       TempVar, 0               
	
	
	// Read the Rx Error Counter, Decreament Error Counter
	MOV32       RegisterValue,PRU_CAN_RX_ERROR_COUNTER_REGISTER
    LBBO        ErrorCounter,RegisterValue,0,4
	QBEQ        COMPARE_CAN_ID,ErrorCounter,0
	
    SUB         ErrorCounter,ErrorCounter,1
    SBBO        ErrorCounter,RegisterValue,0,4
	QBNE        COMPARE_CAN_ID,ErrorCounter, (PRU_CAN_ERROR_ACTIVE -1)
	
	MOV32       RegisterValue,PRU_CAN_TX_ERROR_COUNTER_REGISTER
    LBBO        ErrorCounter,RegisterValue,0,4	
	QBLT        COMPARE_CAN_ID,ErrorCounter,(PRU_CAN_ERROR_ACTIVE -1)
	
	MOV32      	RegisterOffset, PRU_CAN_RX_GLOBAL_STATUS_REGISTER
	LBBO		RegisterValue, RegisterOffset, 0, 4
	CLR			RegisterValue, 	CAN_PASSIVE_ERR_BIT
	SBBO      	RegisterValue, RegisterOffset, 0, 4
		
//////////////////////////////////////////////////////////////////
/////// Check ID match for the node //////////////////////////////
//////  If node ID matches check for mailbox to be free //////////
//////////////////////////////////////////////////////////////////

COMPARE_CAN_ID:
    MOV32   	RegisterOffset, PRU_CAN_ID_MAP
    LBBO    	ShiftRegister, RegisterOffset, TempVar,4      
    QBEQ    	CAN_ID_MATCH, CanCurrentState, ShiftRegister
    ADD     	TempVar, TempVar, 4
    QBEQ    	CAN_EMULATION_RX_START_TEMP, TempVar, 32
    JMP     	COMPARE_CAN_ID

CAN_ID_MATCH:
	QBEQ     	MAILBOX_FREE, CanFrameState.b0,STD_REMOTE_FRAME
	QBEQ     	MAILBOX_FREE, CanFrameState.b0,EXTD_REMOTE_FRAME

    LSR     	TempVar, TempVar, 2       
    MOV32   	RegisterOffset, PRU_CAN_RX_INTERRUPT_STATUS_REGISTER
	
    MOV32   	MailboxNumber, 0
    OR     	 	MailboxNumber, TempVar, MailboxNumber 
    LBBO    	RegisterOffset, RegisterOffset, 0, 4
    QBBC    	MAILBOX_FREE, RegisterOffset, TempVar
	
    LSL     	TempVar,TempVar,2
    ADD     	TempVar,TempVar,4
    QBEQ    	CAN_EMULATION_RX_START_TEMP, TempVar, 32
    JMP     	COMPARE_CAN_ID
	
//===================================================================================================
MAILBOX_FREE:
    MOV32       MailboxShiftRegister,0              
    QBEQ        STD_FRAME_WRITE_MAILBOX,CanFrameState.b0,STD_DATA_FRAME
    QBEQ        STD_REMOTE_FRAME_WRITE_MAILBOX,CanFrameState.b0,STD_REMOTE_FRAME
    QBEQ        EXTD_FRAME_WRITE_MAILBOX,CanFrameState.b0,EXTD_DATA_FRAME
    QBEQ        EXTD_REMOTE_FRAME_WRITE_MAILBOX,CanFrameState.b0,EXTD_REMOTE_FRAME
        
//////////////////////////////////////////////////////////////////////////////////////////
///START/////// Parse the Exetended data frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

EXTD_FRAME_WRITE_MAILBOX:

	///////////////////////////
	/// Move the Identifier////
	///////////////////////////

	MOV32   	RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO    	RegisterValue,RegisterOffset,0,4
	MOV     	RegisterOffset,RegisterValue

	LSR     	RegisterOffset.w2,RegisterOffset.w2,5   
	LSL     	RegisterOffset.w2,RegisterOffset.w2,2   
	SET     	RegisterOffset.w2,RegisterOffset.w2,13
	CLR     	RegisterOffset.w2,RegisterOffset.w2,14

	LSR     	RegisterValue, RegisterValue, 1
	AND     	RegisterValue.w2, RegisterValue.w2, 0x3		
	OR      	RegisterOffset.w2,RegisterValue.w2,RegisterOffset.w2        
	MOV     	mailboxShiftreg.u16CANBaseIdentifier,RegisterOffset.w2      
	MOV     	mailboxShiftreg.u16CANExtendedIdentifier,RegisterValue.w0

	///////////////////////////
	/// Move the DLC //////////
	///////////////////////////
	
	MOV32   	RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO    	RegisterValue,RegisterOffset,4,4
	MOV     	RegisterOffset,RegisterValue

	LSL     	RegisterOffset,RegisterValue,2
	LSR     	RegisterOffset,RegisterOffset,28
	MOV     	mailboxShiftreg.u16CANdataLength,RegisterOffset.w0

	///////////////////////////
	/// Move the DATA /////////
	/////////////////////////// 

	MOV     	BitsReceivedCntr.w0,0
	MOV     	BitsReceivedCntr.w0,mailboxShiftreg.u16CANdataLength    
	LSL     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,3

	LSL     	RegisterValue,RegisterValue,6
	MOV     	BitsReceivedCntr.w2,26
	MOV32   	TempVar,8
	MOV32   	CanFrame,0  
	MOV32   	ErrorCounter,0x00010000

	EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP:     

	QBEQ    	EXTD_DATA_FRAME_MAILBOX_WRITE_DONE,BitsReceivedCntr.w0,0
	QBEQ    	EXTD_DATA_FRAME_MAILBOX_WRITE_RESET,BitsReceivedCntr.w2,0

EXTD_DATA_FRAME_MAILBOX_WRITE_RESET_DONE:

	LSL     	CanFrame,CanFrame,1
	
	QBBS    	EXTD_DATA_FRAME_MAILBOX_WRITE_ONE,RegisterValue,31      
	LSL     	RegisterValue,RegisterValue,1
	SUB     	BitsReceivedCntr.w2,BitsReceivedCntr.w2,1
	SUB     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
	ADD     	ErrorCounter.w0,ErrorCounter.w0,1       
	QBNE    	EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP,ErrorCounter.w0,32

	QBEQ    	EXTD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO,ErrorCounter.w2,0

	MOV     	mailboxShiftreg.u8CANdata3,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata2,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata1,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata0,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP

EXTD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO:

	MOV     	mailboxShiftreg.u8CANdata7,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata6,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata5,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata4,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP

EXTD_DATA_FRAME_MAILBOX_WRITE_ONE:

	SET     	CanFrame,CanFrame,0     
	LSL     	RegisterValue,RegisterValue,1
	SUB     	BitsReceivedCntr.w2,BitsReceivedCntr.w2,1
	SUB     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
	ADD     	ErrorCounter.w0,ErrorCounter.w0,1
	QBNE        EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP,ErrorCounter.w0,32

	QBEQ        EXTD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET,ErrorCounter.w2,0

	MOV     	mailboxShiftreg.u8CANdata3,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata2,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata1,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata0,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP

EXTD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET:

	MOV     	mailboxShiftreg.u8CANdata7,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata6,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata5,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata4,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	EXTD_DATA_FRAME_MAILBOX_WRITE_LOOP

EXTD_DATA_FRAME_MAILBOX_WRITE_RESET:

	MOV     	BitsReceivedCntr.w2,32
	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,TempVar,4
	MOV32       RegisterOffset,0
	ADD     	TempVar,TempVar,4

	JMP     	EXTD_DATA_FRAME_MAILBOX_WRITE_RESET_DONE

EXTD_DATA_FRAME_MAILBOX_WRITE_DONE:

	QBEQ        EXTD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO_1,ErrorCounter.w2,0
	QBLT        MAILBOX_REGISTER_UPDATE,ErrorCounter.w2,1
	
	MOV     	mailboxShiftreg.u8CANdata3,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata2,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata1,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata0,CanFrame.b0
	
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	MAILBOX_REGISTER_UPDATE

EXTD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO_1:
	
	MOV     	mailboxShiftreg.u8CANdata7,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata6,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata5,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata4,CanFrame.b0
	
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	MAILBOX_REGISTER_UPDATE 

//////////////////////////////////////////////////////////////////////////////////////////
///END///////// Parse the Exetended data frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////
    
//////////////////////////////////////////////////////////////////////////////////////////
///START/////// Parse the Standard data frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////


STD_FRAME_WRITE_MAILBOX:

	///////////////////////////
	/// Move the Identifier////
	///////////////////////////

	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,0,4
	MOV     	RegisterOffset,RegisterValue

	LSR     	RegisterOffset.w2,RegisterOffset.w2,5   
	LSL     	RegisterOffset.w2,RegisterOffset.w2,2                   
	MOV     	mailboxShiftreg.u16CANBaseIdentifier,RegisterOffset.w2      
	MOV     	mailboxShiftreg.u16CANExtendedIdentifier,0

	///////////////////////////
	/// Move the DLC //////////
	///////////////////////////
	
	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,0,4
	MOV     	RegisterOffset,RegisterValue

	LSL     	RegisterOffset,RegisterValue,14
	LSR     	RegisterOffset,RegisterOffset,28
	MOV     	mailboxShiftreg.u16CANdataLength,RegisterOffset.w0

	///////////////////////////
	/// Move the DATA /////////
	/////////////////////////// 

	MOV     	BitsReceivedCntr.w0,0
	MOV     	BitsReceivedCntr.w0,mailboxShiftreg.u16CANdataLength    
	LSL     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,3

	LSL     	RegisterValue,RegisterValue,18
	MOV     	BitsReceivedCntr.w2,14
	MOV32       TempVar,4
	MOV32       CanFrame,0  
	MOV32       ErrorCounter,0x00010000

STD_DATA_FRAME_MAILBOX_WRITE_LOOP:      

	QBEQ        STD_DATA_FRAME_MAILBOX_WRITE_DONE,BitsReceivedCntr.w0,0
	QBEQ        STD_DATA_FRAME_MAILBOX_WRITE_RESET,BitsReceivedCntr.w2,0

STD_DATA_FRAME_MAILBOX_WRITE_RESET_DONE:

	LSL     	CanFrame,CanFrame,1
	
	QBBS        STD_DATA_FRAME_MAILBOX_WRITE_ONE,RegisterValue,31       
	LSL     	RegisterValue,RegisterValue,1
	SUB     	BitsReceivedCntr.w2,BitsReceivedCntr.w2,1
	SUB     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
	ADD     	ErrorCounter.w0,ErrorCounter.w0,1       
	QBNE        STD_DATA_FRAME_MAILBOX_WRITE_LOOP,ErrorCounter.w0,32

	QBEQ        STD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO,ErrorCounter.w2,0

	MOV     	mailboxShiftreg.u8CANdata3,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata2,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata1,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata0,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	STD_DATA_FRAME_MAILBOX_WRITE_LOOP

STD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO:

	MOV     	mailboxShiftreg.u8CANdata7,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata6,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata5,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata4,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	STD_DATA_FRAME_MAILBOX_WRITE_LOOP

STD_DATA_FRAME_MAILBOX_WRITE_ONE:

	SET     	CanFrame,CanFrame,0     
	LSL     	RegisterValue,RegisterValue,1
	SUB     	BitsReceivedCntr.w2,BitsReceivedCntr.w2,1
	SUB     	BitsReceivedCntr.w0,BitsReceivedCntr.w0,1
	ADD     	ErrorCounter.w0,ErrorCounter.w0,1
	QBNE        STD_DATA_FRAME_MAILBOX_WRITE_LOOP,ErrorCounter.w0,32

	QBEQ        STD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET,ErrorCounter.w2,0

	MOV     	mailboxShiftreg.u8CANdata3,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata2,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata1,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata0,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	STD_DATA_FRAME_MAILBOX_WRITE_LOOP

STD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET:

	MOV     	mailboxShiftreg.u8CANdata7,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata6,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata5,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata4,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	STD_DATA_FRAME_MAILBOX_WRITE_LOOP

STD_DATA_FRAME_MAILBOX_WRITE_RESET:

	MOV     	BitsReceivedCntr.w2,32
	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,TempVar,4
	MOV32       RegisterOffset,0
	ADD     	TempVar,TempVar,4

	JMP     	STD_DATA_FRAME_MAILBOX_WRITE_RESET_DONE

STD_DATA_FRAME_MAILBOX_WRITE_DONE:

	QBEQ        STD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO_1,ErrorCounter.w2,0
	QBLT        MAILBOX_REGISTER_UPDATE,ErrorCounter.w2,1

	MOV     	mailboxShiftreg.u8CANdata3,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata2,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata1,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata0,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	MAILBOX_REGISTER_UPDATE

STD_DATA_FRAME_MAILBOX_WRITE_SECOND_SET_ZERO_1:

	MOV     	mailboxShiftreg.u8CANdata7,CanFrame.b3
	MOV     	mailboxShiftreg.u8CANdata6,CanFrame.b2
	MOV     	mailboxShiftreg.u8CANdata5,CanFrame.b1
	MOV     	mailboxShiftreg.u8CANdata4,CanFrame.b0
	SUB     	ErrorCounter.w2,ErrorCounter.w2,1
	MOV     	ErrorCounter.w0,0
	MOV32       CanFrame,0
	JMP     	MAILBOX_REGISTER_UPDATE 
//////////////////////////////////////////////////////////////////////////////////////////
///END/////// Parse the Standard data frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
///START/////// Parse the Standard Remote frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

STD_REMOTE_FRAME_WRITE_MAILBOX:

	///////////////////////////
	/// Move the Identifier////
	///////////////////////////

	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,0,4
	MOV     	RegisterOffset,RegisterValue

	
	LSR     	RegisterOffset.w2,RegisterOffset.w2,5   
	LSL     	RegisterOffset.w2,RegisterOffset.w2,2   
	SET     	RegisterOffset.w2,RegisterOffset.w2,14              
	MOV     	mailboxShiftreg.u16CANBaseIdentifier,RegisterOffset.w2      
	MOV     	mailboxShiftreg.u16CANExtendedIdentifier,0

	///////////////////////////
	/// Move the DLC //////////
	///////////////////////////
	
	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,0,4
	MOV     	RegisterOffset,RegisterValue

	LSL     	RegisterOffset,RegisterValue,14
	LSR     	RegisterOffset,RegisterOffset,28
	MOV     	mailboxShiftreg.u16CANdataLength,RegisterOffset.w0
	MOV			MailboxNumber,8
	JMP     	MAILBOX_REGISTER_UPDATE 

//////////////////////////////////////////////////////////////////////////////////////////
///END/////// Parse the Standard remote frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
///START/////// Parse the Exetended remote frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

EXTD_REMOTE_FRAME_WRITE_MAILBOX:

	///////////////////////////
	/// Move the Identifier////
	///////////////////////////

	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,0,4
	MOV     	RegisterOffset,RegisterValue

	LSR     	RegisterOffset.w2,RegisterOffset.w2,5   
	LSL     	RegisterOffset.w2,RegisterOffset.w2,2   
	SET     	RegisterOffset.w2,RegisterOffset.w2,13
	SET     	RegisterOffset.w2,RegisterOffset.w2,14

	LSR     	RegisterValue, RegisterValue, 1
	AND     	RegisterValue.w2, RegisterValue.w2, 0x3
	OR      	RegisterOffset.w2,RegisterValue.w2,RegisterOffset.w2        

	MOV     	mailboxShiftreg.u16CANBaseIdentifier,RegisterOffset.w2      
	MOV     	mailboxShiftreg.u16CANExtendedIdentifier,RegisterValue.w0

	///////////////////////////
	/// Move the DLC //////////
	///////////////////////////
	
	MOV32       RegisterOffset,PRU_CAN_RECEIVE_FRAME
	LBBO        RegisterValue,RegisterOffset,4,4
	MOV     	RegisterOffset,RegisterValue

	LSL     	RegisterOffset,RegisterValue,2
	LSR     	RegisterOffset,RegisterOffset,28
	MOV     	mailboxShiftreg.u16CANdataLength,RegisterOffset.w0
	MOV			MailboxNumber,8

//////////////////////////////////////////////////////////////////////////////////////////
///END/////// Parse the Exetended remote frame and write to shift register ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
///// END ///////////**** Populate the Temperory Mailbox with Received data ***///////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
///START/////// Update the Mailbox Register ////////////////////////////// ///////////////
//////////////////////////////////////////////////////////////////////////////////////////

MAILBOX_REGISTER_UPDATE:
        
    MOV32   	RegisterOffset,PRU_CAN_RX_INTERRUPT_STATUS_REGISTER 
    QBEQ    	INTR_REG_REMOTE_UPDATE,MailboxNumber,8
    MOV32   	RegisterValue,0x1
    LSL     	RegisterValue,RegisterValue,MailboxNumber
    SBBO    	RegisterValue,RegisterOffset,0,4 
    JMP     	INTR_UPDATED_CNTD
        
INTR_REG_REMOTE_UPDATE:
    MOV32   	RegisterValue,0x100   
    SBBO    	RegisterValue,RegisterOffset,0,4 
	MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX8_STATUS_REGISTER 
	MOV32   	RegisterValue,0x91              
    SBBO    	RegisterValue,RegisterOffset,0,4 

INTR_UPDATED_CNTD:
    ///////////////////////////////////////////////////////
    ////////////// Mailbox Register update ////////////////
    ///////////////////////////////////////////////////////
    MOV32       RegisterValue,0x81
    QBEQ        MAILBOX_RECEIVE_0,MailboxNumber,0
    QBEQ        MAILBOX_RECEIVE_1,MailboxNumber,1
    QBEQ        MAILBOX_RECEIVE_2,MailboxNumber,2
    QBEQ        MAILBOX_RECEIVE_3,MailboxNumber,3
    QBEQ        MAILBOX_RECEIVE_4,MailboxNumber,4
    QBEQ        MAILBOX_RECEIVE_5,MailboxNumber,5
    QBEQ        MAILBOX_RECEIVE_6,MailboxNumber,6
    QBEQ        MAILBOX_RECEIVE_7,MailboxNumber,7
    QBEQ        MAILBOX_RECEIVE_8,MailboxNumber,8
    JMP         DO_NOT_ACKNOWLEDGE

MAILBOX_RECEIVE_0:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX0_STATUS_REGISTER              
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX0     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_1:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX1_STATUS_REGISTER            
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX1     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_2:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX2_STATUS_REGISTER            
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX2     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_3:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX3_STATUS_REGISTER         
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX3     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_4:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX4_STATUS_REGISTER           
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX4     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_5:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX5_STATUS_REGISTER          
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX5     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_6:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX6_STATUS_REGISTER         
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX6     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_7:
    MOV32   	RegisterOffset,PRU_CAN_RX_MAILBOX7_STATUS_REGISTER         
    SBBO    	RegisterValue,RegisterOffset,0,4 
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX7     
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_8:  
    MOV32   	RegisterOffset, PRU_CAN_RX_MAILBOX8
    JMP     	MAILBOX_RECEIVE_SELECTED

MAILBOX_RECEIVE_SELECTED:
    SBBO    	MailboxShiftRegister,RegisterOffset,0,16
	
//=================================================================================================================

//**********************************CAN DATA FRAME RECIEVED: Inform to DSMP/ARM ***********************************

//=================================================================================================================                
RECIEVE_DONE:
	// Make sure the SYS_EVT32 system interrupt is cleared
	MOV32     	RegisterValue, SYS_EVT
	SBCO      	RegisterValue, CONST_PRUSSINTC, SICR_OFFSET, 4

	// Enable SYS_EVT32 system interrupt
	SBCO   		RegisterValue, CONST_PRUSSINTC, EISR_OFFSET,  4

	//Generate SYS_EVT32 by event out mapping
	MOV32  		r31, SYS_EVT
	
	JMP       	CAN_EMULATION_RX_START

//=================================================================================================================

//******************************************* CALCULATE CRC *******************************************************

//=================================================================================================================  
CRC_CALCULATE_0:
	MOV     	CRC_Input.w0,0
	LSR     	CRC_TempVar.w0,CRC_TempVar.w2,14
	AND     	CRC_TempVar.w0,CRC_TempVar.w0,1
	XOR     	CRC_TempVar.w0,CRC_TempVar.w0,CRC_Input.w0
	LSL     	CRC_TempVar.w2,CRC_TempVar.w2,2
	LSR     	CRC_TempVar.w2,CRC_TempVar.w2,1
	QBBC    	NO_EXOR_WITH_POLYNOMIAL_0,CRC_TempVar.w0,0              
	MOV     	CRC_Input.w2,0x4599
	XOR     	CRC_TempVar.w2,CRC_TempVar.w2,CRC_Input.w2

	NO_EXOR_WITH_POLYNOMIAL_0:
	RET
        
CRC_CALCULATE_1:
	MOV     	CRC_Input.w0,1
	LSR     	CRC_TempVar.w0,CRC_TempVar.w2,14
	AND     	CRC_TempVar.w0,CRC_TempVar.w0,1
	XOR     	CRC_TempVar.w0,CRC_TempVar.w0,CRC_Input.w0
	LSL     	CRC_TempVar.w2,CRC_TempVar.w2,2
	LSR     	CRC_TempVar.w2,CRC_TempVar.w2,1
	
	QBBC    	NO_EXOR_WITH_POLYNOMIAL,CRC_TempVar.w0,0                
	MOV     	CRC_Input.w2,0x4599
	XOR     	CRC_TempVar.w2,CRC_TempVar.w2,CRC_Input.w2
	
NO_EXOR_WITH_POLYNOMIAL:
    RET
        
//=================================================================================================================

//**********************************************POLL for TIMER INTERRUPT*******************************************

//=================================================================================================================


TIMER_EVENT_WAIT:
	MOV	     	RegisterOffset.w2, INTCTL_BASE 
	MOV		 	RegisterOffset.w0, SECR1_OFFSET
	
TIMER_EVENT_WAIT_LOOP:
	QBBS   	 	TIMER_EVENT_HANDLER, r31, #30
    JMP    	 	TIMER_EVENT_WAIT_LOOP
	
TIMER_EVENT_HANDLER:
	LBBO     	TempRegister, RegisterOffset, 0, 4
	QBBC	 	TIMER_EVENT_WAIT_LOOP, TempRegister, TIMER_SYS_EVT
	MOV32    	RegisterOffset, TMR_REGS	

	MOV      	TempRegister, 0x00000003
    SBBO     	TempRegister, RegisterOffset, TMR_INTCTL_OFFSET,  2	
	
	// Clear Timer interrupt
    MOV32     	TempRegister, (0x00000000 | TIMER_SYS_EVT)
    SBCO      	TempRegister, CONST_PRUSSINTC, SICR_OFFSET, 4
	
	// Enable Timer 0 system interrupt
    SBCO      	TempRegister, CONST_PRUSSINTC, EISR_OFFSET, 4	
	RET	
//===========================================================	
POLL_CAN_BUS_STATUS:
	MOV	     	RegisterOffset.w2, INTCTL_BASE 
	MOV		 	RegisterOffset.w0, SECR1_OFFSET											
	
CAN_BUS_POLL:	
	QBBS    	CAN_BUS_SET, r31, GPIO_RX_BIT
	MOV		 	TempVar.w0, 0xff
				
CAN_BUS_SET:	
	QBBS  	 	TIMER_EXPIERED, r31, #30
	JMP   	 	CAN_BUS_POLL
					
TIMER_EXPIERED:	
	LBBO     	TempRegister, RegisterOffset, 0, 4
	QBBC	 	CAN_BUS_POLL, TempRegister, TIMER_SYS_EVT
				
	MOV32    	RegisterOffset, TMR_REGS
	MOV     	TempRegister, 0x00000003
	SBBO    	TempRegister, RegisterOffset, TMR_INTCTL_OFFSET,  2
				
	// Clear Timer interrupt
	MOV32     	TempRegister, (0x00000000 | TIMER_SYS_EVT)
	SBCO      	TempRegister, CONST_PRUSSINTC, SICR_OFFSET, 4
	
	// Enable Timer system interrupt
	SBCO      	TempRegister, CONST_PRUSSINTC, EISR_OFFSET, 4
	RET
.end


