/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////Source: PRU_CAN_Emulation_Tx.hp///////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
///Revision: 1.0/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
////PRU_CAN_emulation service Routine assembly constants.//////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
///// Global Macro definitions///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#include "PRU_CAN_Emulation_Tx.hp"

/////////////////////////////////////////////////////////////////////////////////////////
////Assign Registers to predefined mailbox structure///////////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////
.assign CAN_MAIL_BOX_STRUCTURE,R4,*,canmailbox
.setcallreg	r2.w0

.origin 0
.entrypoint CAN_EMULATION_TX_INIT

/////////////////////////////////////////////////////////////////////////////////////////
////Initialize PRU_CAN_emulation Registers to reset value//////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////
CAN_EMULATION_TX_INIT:

	MOV32  		RegisterOffset,PRU_CAN_TX_BUS_AQUIRED			
	XOR  		RegisterValue,RegisterValue, RegisterValue
	SBBO		RegisterValue,RegisterOffset,0,4

	// Set the TX GPIO Line High
	GPIO_SET 	GPIO_TX_BIT
	
CAN_EMULATION_TX_START:	
	MOV			CRCFrame.b0,0
	MOV			CRCFrame.b1,0
	JMP			ARM_DSP_INTR_POLL
	
TRANSMIT_POLL_BUS_OFF:	
	XOR         RegisterValue.w0, RegisterValue.w0,RegisterValue.w0

POLL_BUS_OFF:	
	CALL		TIMER_EVENT_WAIT
	QBBC        TRANSMIT_POLL_BUS_OFF, r31, 30
	ADD			RegisterValue.w0, RegisterValue.w0, 1
	QBGT		POLL_BUS_OFF,  RegisterValue.w0, ELEVEN_CONSECUTIVE_HIGH_BIT
	ADD			RegisterValue.w2, RegisterValue.w2, 1
	QBGT		TRANSMIT_POLL_BUS_OFF, RegisterValue.w2, BUS_OFF_RESET_CNT_128
		
TRANSMIT_BUS_OFF_RESET:
	MOV32  		RegisterOffset,PRU_CAN_TX_ERROR_COUNTER_REGISTER
	XOR         RegisterValue, RegisterValue,RegisterValue
	SBBO   		RegisterValue,RegisterOffset,0,4	

	MOV32  		RegisterOffset,PRU_CAN_TX_GLOBAL_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	CLR			RegisterValue, CAN_PASSIVE_ERR_BIT
	CLR			RegisterValue, CAN_BUS_OFF_ERR_BIT
	SBBO   		RegisterValue,RegisterOffset,0,4
	
/////////////////////////////////////////////////////////////////////////////////////////
///////// Poll for receipt of interrupt on DSP/ARM/RXPRU //////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////
ARM_DSP_INTR_POLL:

	MOV32  		RegisterOffset,PRU_CAN_TX_ERROR_COUNTER_REGISTER
	LBBO   		ShiftRegister,RegisterOffset,0,4	
	XOR         RegisterValue, RegisterValue,RegisterValue
	QBLE		TRANSMIT_POLL_BUS_OFF,ShiftRegister,PRU_CAN_BUS_OFF_ERR_CNT
	
	// Disable Timer
	MOV32     	RegisterOffset, TMR_REGS
    LDI       	RegisterValue.w0, 0x0000												//Reload the bit delay
    SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2
	
	// Place Timer in reset
	SBBO      	RegisterValue.w0, RegisterOffset, TMR_TGCR_OFFSET,  2
	
	TRANSMIT_POLL:	
	MOV32  		RegisterOffset,PRU_CAN_TX_BUS_AQUIRED			
	XOR	        RegisterValue, RegisterValue, RegisterValue
	SBBO		RegisterValue, RegisterOffset,0,4
		
	QBBS    	ARM_DSP_INTERRUPT, EventStatus, 30
	JMP     	TRANSMIT_POLL	
		
ARM_DSP_INTERRUPT:
	MOV    	    RegisterOffset.w2, INTCTL_BASE 
	MOV    	    RegisterOffset.w0, SECR2_OFFSET
	LBBO      	RegisterValue, RegisterOffset, 0, 4
	QBBC      	TRANSMIT_POLL, RegisterValue, 0
	
	// Clear DSP/ARM interrupt
    MOV32     	RegisterValue, (0x00000000 | SYS_EVT_DSPARM_PRU_TX)
    SBCO      	RegisterValue, CONST_PRUSSINTC, SICR_OFFSET, 4
	
/////////////////////////////////////////////////////////////////////////////////////////
///////// Start Transmitting data ==> Initated by DSP/ARM /////////////////////////////// 
/////////////////////////////////////////////////////////////////////////////////////////
PRU_CAN_TRANSMIT:							
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX0_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4		
	QBBC   		MAILBOX0_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX1_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX1_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX2_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX2_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX3_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX3_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX4_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX4_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX5_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX5_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX6_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX6_TRANSMIT,RegisterValue,0

	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX7_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4
	QBBC   		MAILBOX7_TRANSMIT,RegisterValue,0		
	JMP    		TRANSMIT_POLL

MAILBOX0_TRANSMIT:
	MOV32  		CurrentMailbox,0
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX0
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)	
	JMP			CAN_START_TRANSMIT

MAILBOX1_TRANSMIT:
	MOV32  		CurrentMailbox,1
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX1
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)
	JMP			CAN_START_TRANSMIT

MAILBOX2_TRANSMIT:
	MOV32  		CurrentMailbox,2
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX2
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)
	JMP			CAN_START_TRANSMIT

MAILBOX3_TRANSMIT:
	MOV32  		CurrentMailbox,3
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX3
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)
	JMP			CAN_START_TRANSMIT

MAILBOX4_TRANSMIT:
	MOV32  		CurrentMailbox,4
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX4
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)
	JMP			CAN_START_TRANSMIT

MAILBOX5_TRANSMIT:
	MOV32  		CurrentMailbox,5
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX5
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)
	JMP			CAN_START_TRANSMIT

MAILBOX6_TRANSMIT:
	MOV32  		CurrentMailbox,6
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX6
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)
	JMP			CAN_START_TRANSMIT

MAILBOX7_TRANSMIT:
	MOV32  		CurrentMailbox,7
	MOV32  		MailboxShiftRegister,PRU_CAN_TX_MAILBOX7
	LBBO   		canmailbox,MailboxShiftRegister,0,SIZE(canmailbox)

CAN_START_TRANSMIT:

	QBBS 		CAN_EXTD_ID_UPDATE,canmailbox.u16CANBaseIdentifier,13
	MOV32		Identifier,CAN_STD_ID
	MOV32  		BitCounter,0x000D0001			
	JMP			CAN_EXTD_ID_UPDATE_COMPLETE

CAN_EXTD_ID_UPDATE:
	MOV32		Identifier,CAN_EXTD_ID
	MOV32  		BitCounter,0x000D0001	

CAN_EXTD_ID_UPDATE_COMPLETE:

	XOR  		BitStuffCounter,CanCurrentState, CanCurrentState
	XOR  		CanFrame,CanCurrentState, CanCurrentState
	XOR  		CanCurrentState,CanCurrentState, CanCurrentState	
	XOR   		BitsToSendCntr,BitsToSendCntr, BitsToSendCntr
	XOR   		CRC_TempVar, CRC_TempVar, CRC_TempVar
	XOR   		CRC_Input,CRC_Input, CRC_Input
	MOV			BitStuffCounter.w0,1
	MOV			r13,canmailbox.u16CANBaseIdentifier
				
CAN_FRAME_BASE_IDENTIFIER_STATE:

	QBEQ    	CAN_TRANS_EXTD_FRM,Identifier,CAN_EXTD_ID
	QBEQ 		CAN_FRAME_RMTRQST_STATE,BitCounter.w2,2	
	JMP			CAN_TRANS_EXTD_FRM_END	

CAN_TRANS_EXTD_FRM:
	QBEQ 		CAN_FRAME_RMTRQST_STATE,BitCounter.w2,0	
	QBLT		SRR_IDE_EXTD_DONE,BitsToSendCntr.w0,16
	QBEQ		SRR_STATE,BitCounter.w2,2
	
SRR_IDE_EXTD_DONE:
				
CAN_TRANS_EXTD_FRM_END:	
	QBGT 		FRAME_STATE_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK	
	
FRAME_STATE_0:
	QBEQ    	CAN_TRANS_EXTD_UPDATE,Identifier,CAN_EXTD_ID
	SUB			BitCounter.w2,BitCounter.w2,1			
	QBBS 		FRAME_BIT_ONE_0,r13,BitCounter.w2
	ADD			BitCounter.w2,BitCounter.w2,1
	JMP			CAN_TRANS_EXTD_UPDATE_END

CAN_TRANS_EXTD_UPDATE:
	QBGE		EXTD_FRAME,BitCounter.w2,16
	SUB			BitCounter.w2,BitCounter.w2,1
	QBBS 		FRAME_BIT_ONE_0,r13,BitCounter.w2
	ADD			BitCounter.w2,BitCounter.w2,1
	JMP			CAN_TRANS_EXTD_UPDATE_END
	
EXTD_FRAME:
	SUB			BitCounter.w2,BitCounter.w2,1
	QBBS 		FRAME_BIT_ONE_0,r13,BitCounter.w2
	ADD			BitCounter.w2,BitCounter.w2,1
	JMP			CAN_TRANS_EXTD_UPDATE_END
	
SRR_STATE:

////////////////////////////////////////////////////////////
///////////START OF SRR / IDE /////////////////////////////
///////////////////////////////////////////////////////////

	MOV			r13,canmailbox.u16CANExtendedIdentifier
	MOV			BitCounter.w2,16

	ADD			BitsToSendCntr,BitsToSendCntr,1				
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0	
	CALL		CRC_CALCULATE_1					
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		SRR_BYTE_OFFSET_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK							
	
SRR_BYTE_OFFSET_0:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_SRR_EXTD,BitStuffCounter.w2,5
	JMP 		SRR_NO_STUFF_0

FRAME_STUFF_ONE_SRR_EXTD:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1								
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		SRR_BYTE_OFFSET_1,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
SRR_BYTE_OFFSET_1:
SRR_NO_STUFF_0:

FRAME_BIT_ONE_IDE_EXTD:

	ADD			BitsToSendCntr,BitsToSendCntr,1			
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1				
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		IDE_BYTE_OFFSET_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
				
IDE_BYTE_OFFSET_0:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_EXTD_IDE,BitStuffCounter.w2,5
	JMP 		IDE_BYTE_OFFSET_1

FRAME_STUFF_ZERO_EXTD_IDE:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1				
	JMP 		IDE_BYTE_OFFSET_1	

	QBGT 		IDE_BYTE_OFFSET_1,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
IDE_BYTE_OFFSET_1:
	QBBS 		FRAME_BIT_ONE_0_SRR,canmailbox.u16CANBaseIdentifier,1

FRAME_BIT_ZER0_0_SRR:
	ADD			BitsToSendCntr,BitsToSendCntr,1					
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_IDENTIFIER_SRR,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_IDENTIFIER_SRR:
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_0,BitStuffCounter.w0,5
	JMP 		SRR_IDE_SECOND_BIT

FRAME_STUFF_ONE_0_SRR:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		SRR_IDE_SECOND_BIT,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		SRR_IDE_SECOND_BIT

FRAME_BIT_ONE_0_SRR:
	ADD			BitsToSendCntr,BitsToSendCntr,1				
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_IDENTIFIER_1_SRR,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_IDENTIFIER_1_SRR:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_0_SRR,BitStuffCounter.w2,5
	JMP 		SRR_IDE_SECOND_BIT

FRAME_STUFF_ZERO_0_SRR:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1	
	QBGT 		SRR_IDE_SECOND_BIT,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK			

SRR_IDE_SECOND_BIT:		
	QBBS 		FRAME_BIT_ONE_0_SRR_IDE,canmailbox.u16CANBaseIdentifier,0

FRAME_BIT_ZER0_0_SRR_IDE:
	ADD			BitsToSendCntr,BitsToSendCntr,1						
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_IDENTIFIER_SRR_IDE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_IDENTIFIER_SRR_IDE:
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_0_SRR_IDE,BitStuffCounter.w0,5
	JMP 		SRR_IDE_EXTD_DONE

FRAME_STUFF_ONE_0_SRR_IDE:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		SRR_IDE_EXTD_DONE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		SRR_IDE_EXTD_DONE

FRAME_BIT_ONE_0_SRR_IDE:
	ADD			BitsToSendCntr,BitsToSendCntr,1				
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_IDENTIFIER_1_SRR_IDE,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_IDENTIFIER_1_SRR_IDE:

	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_0_SRR_IDE,BitStuffCounter.w2,5
	JMP 		SRR_IDE_EXTD_DONE

FRAME_STUFF_ZERO_0_SRR_IDE:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1	
	QBGT 		SRR_IDE_EXTD_DONE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK			
					
/////////////////////////////////////////////////////////////////////////////////////////

JMP	SRR_IDE_EXTD_DONE

//////////////////////////////////////////////////////
/////////END of SRR / IDE////////////////////////////
////////////////////////////////////////////////////

CAN_TRANS_EXTD_UPDATE_END:

FRAME_BIT_ZER0_0:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1		
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_IDENTIFIER,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_IDENTIFIER:
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_0,BitStuffCounter.w0,5
	JMP 		CAN_FRAME_BASE_IDENTIFIER_STATE

FRAME_STUFF_ONE_0:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		CAN_FRAME_BASE_IDENTIFIER_STATE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		CAN_FRAME_BASE_IDENTIFIER_STATE

FRAME_BIT_ONE_0:
	ADD			BitsToSendCntr,BitsToSendCntr,1				
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_IDENTIFIER_1,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_IDENTIFIER_1:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_0,BitStuffCounter.w2,5
	JMP 		CAN_FRAME_BASE_IDENTIFIER_STATE

FRAME_STUFF_ZERO_0:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1	
	QBGT 		CAN_FRAME_BASE_IDENTIFIER_STATE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK			
	JMP 		CAN_FRAME_BASE_IDENTIFIER_STATE			

CAN_FRAME_RMTRQST_STATE:

	QBBS 		FRAME_BIT_ONE_1,canmailbox.u16CANBaseIdentifier,14

FRAME_BIT_ZER0_1:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_RR_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				

BYTE_OFFSET_RR_0:				
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_1,BitStuffCounter.w0,5
	JMP 		FRAME_RTR_DONE

FRAME_STUFF_ONE_1:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		FRAME_RTR_DONE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		FRAME_RTR_DONE

FRAME_BIT_ONE_1:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_RR_1,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK							
	
BYTE_OFFSET_RR_1:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_1,BitStuffCounter.w2,5
	JMP 		FRAME_RTR_DONE

FRAME_STUFF_ZERO_1:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1				
	
FRAME_RTR_DONE:

	QBGT 		CAN_FRAME_IDE_STATE,BitCounter.w0,32			
	CALL		PUSH_TO_FRAME_STACK

CAN_FRAME_IDE_STATE:
	QBEQ    	CAN_TRANS_EXTD_RO_R1,Identifier,CAN_EXTD_ID			
	QBBS 		FRAME_BIT_ONE_2,canmailbox.u16CANBaseIdentifier,13

FRAME_BIT_ZER0_2:					
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_STD_IDE_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_STD_IDE_0:				
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_2,BitStuffCounter.w0,5
	JMP 		FRAME_IDE_DONE

FRAME_STUFF_ONE_2:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		FRAME_IDE_DONE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		FRAME_IDE_DONE

FRAME_BIT_ONE_2:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_STD_IDE_1,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK				

BYTE_OFFSET_STD_IDE_1:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_2,BitStuffCounter.w2,5
	JMP 		FRAME_IDE_DONE

FRAME_STUFF_ZERO_2:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1				
	
FRAME_IDE_DONE:
	QBGT 		CAN_FRAME_R0_STATE,BitCounter.w0,32			
	CALL		PUSH_TO_FRAME_STACK

CAN_FRAME_R0_STATE:

FRAME_BIT_ZER0_3:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_STD_R0,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK		
	
BYTE_OFFSET_STD_R0:				
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_3,BitStuffCounter.w0,5
	JMP 		FRAME_R0_DONE

FRAME_STUFF_ONE_3:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		FRAME_R0_DONE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		FRAME_R0_DONE

FRAME_R0_DONE:

	mov 		BitCounter.w2,0x0004
	QBGT 		CAN_FRAME_DLC_STATE,BitCounter.w0,32
	ADD			BitsToSendCntr,BitsToSendCntr,1
	CALL		PUSH_TO_FRAME_STACK
	JMP			CAN_FRAME_DLC_STATE

//////////////////////////////////////////////////////////////
/////*************** R0 and R1 State **************//////////
/////////////////////////////////////////////////////////////

CAN_TRANS_EXTD_RO_R1:

FRAME_BIT_ZER0_RO_R1:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_R0_R1,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK		
	
BYTE_OFFSET_R0_R1:				
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_RO_R1,BitStuffCounter.w0,5
	JMP 		FRAME_BIT_ZER0_R1_R0

FRAME_STUFF_ONE_RO_R1:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	
	QBGT 		FRAME_BIT_ZER0_R1_R0,BitCounter.w0,32				
	CALL		PUSH_TO_FRAME_STACK				
					
FRAME_BIT_ZER0_R1_R0:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_R1_R0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK		
	
	BYTE_OFFSET_R1_R0:				
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_R1_R0,BitStuffCounter.w0,5
	JMP 		FRAME_R0_R1_DONE

FRAME_STUFF_ONE_R1_R0:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
		
FRAME_R0_R1_DONE:	
	mov 		BitCounter.w2,0x0004
	QBGT 		CAN_FRAME_DLC_STATE,BitCounter.w0,32
	ADD			BitsToSendCntr,BitsToSendCntr,1
	CALL		PUSH_TO_FRAME_STACK	

/////////////////////////////////////////////////////////////////
////////////**************** Data Length state ***********///////
/////////////////////////////////////////////////////////////////

CAN_FRAME_DLC_STATE:
	MOV			DataCounter,0
	QBEQ 		CAN_FRAME_DATA_STATE,BitCounter.w2,0			
	SUB			DataCounter.w2,BitCounter.w2,1
	QBBS 		FRAME_BIT_ONE_4,canmailbox.u16CANdataLength,DataCounter.w2

FRAME_BIT_ZER0_4:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1					
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_DLC_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK	
	
BYTE_OFFSET_DLC_0:			
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_4,BitStuffCounter.w0,5
	JMP 		FRAME_DLC_DONE

FRAME_STUFF_ONE_4:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		FRAME_DLC_DONE,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK	
	JMP 		FRAME_DLC_DONE

FRAME_BIT_ONE_4:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_DLC_1,BitCounter.w0,32			
	CALL		PUSH_TO_FRAME_STACK					
	
BYTE_OFFSET_DLC_1:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_4,BitStuffCounter.w2,5
	JMP 		FRAME_DLC_DONE

FRAME_STUFF_ZERO_4:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1				
		
FRAME_DLC_DONE:
	QBGT 		CAN_FRAME_DLC_STATE,BitCounter.w0,32			
	CALL		PUSH_TO_FRAME_STACK	
	JMP			CAN_FRAME_DLC_STATE

CAN_FRAME_DATA_STATE:
	MOV  		RegisterOffset.w0,canmailbox.u16CANBaseIdentifier
	QBBS		CAN_FRAME_CRC_STATE,RegisterOffset.w0,14
	MOV 		DataCounter.b0,canmailbox.u16CANdataLength
	ADD			DataCounter.b0,DataCounter.b0,1
	
CAN_RESET_BYTE:
	SUB 		DataCounter.b0,DataCounter.b0,1
	QBEQ 		CAN_FRAME_CRC_STATE,DataCounter.b0,0
	MOV			BitCounter.w2,0x0007
	MOV			RegisterOffset.w2,BitCounter.w2
	
CAN_DATA_START:
	QBGT 		CAN_DATA_CNT,BitCounter.w0,32
	MOV			BitCounter.w0,0					
	MOV32  		RegisterOffset,PRU_CAN_TRANSMIT_FRAME	
	SBBO   		CanFrame,RegisterOffset,CanCurrentState.b1,4 
	ADD			CanCurrentState.b1,CanCurrentState.b1,4
	MOV32  		CanFrame,0
						
CAN_DATA_CNT:
	MOV			RegisterOffset,BitCounter
	ADD			RegisterOffset.w2,RegisterOffset.w2,1	
	QBEQ 		CAN_RESET_BYTE,RegisterOffset.w2,0							
	QBEQ 		FRAME_DATA_BYTE_0,DataCounter.b0,1
	QBEQ 		FRAME_DATA_BYTE_1,DataCounter.b0,2
	QBEQ 		FRAME_DATA_BYTE_2,DataCounter.b0,3
	QBEQ 		FRAME_DATA_BYTE_3,DataCounter.b0,4
	QBEQ 		FRAME_DATA_BYTE_4,DataCounter.b0,5
	QBEQ 		FRAME_DATA_BYTE_5,DataCounter.b0,6
	QBEQ 		FRAME_DATA_BYTE_6,DataCounter.b0,7
	QBEQ 		FRAME_DATA_BYTE_7,DataCounter.b0,8
	JMP			CAN_FRAME_CRC_STATE
	
FRAME_DATA_BYTE_0:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata0,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata0,BitCounter.w2

FRAME_DATA_BYTE_1:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata1,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata1,BitCounter.w2

FRAME_DATA_BYTE_2:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata2,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata2,BitCounter.w2

FRAME_DATA_BYTE_3:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata3,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata3,BitCounter.w2

FRAME_DATA_BYTE_4:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata4,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata4,BitCounter.w2

FRAME_DATA_BYTE_5:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata5,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata5,BitCounter.w2

FRAME_DATA_BYTE_6:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata6,BitCounter.w2
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata6,BitCounter.w2

FRAME_DATA_BYTE_7:			
	QBBS 		FRAME_BIT_ONE_5,canmailbox.u8CANdata7,BitCounter.w2			
	QBBC 		FRAME_BIT_ZERO_5,canmailbox.u8CANdata7,BitCounter.w2

FRAME_BIT_ZERO_5:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1		
	LSL			CanFrame,CanFrame,1	
	CALL		CRC_CALCULATE_0
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_DATA_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK					
	
BYTE_OFFSET_DATA_0:
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_5,BitStuffCounter.w0,5
	JMP 		CAN_DATA_START

FRAME_STUFF_ONE_5:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		CAN_DATA_START,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK	
	JMP			CAN_DATA_START

FRAME_BIT_ONE_5:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	CALL		CRC_CALCULATE_1
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_DATA_1,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK	
	
BYTE_OFFSET_DATA_1:				
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_5,BitStuffCounter.w2,5
	JMP 		CAN_DATA_START

FRAME_STUFF_ZERO_5:
	ADD			BitsToSendCntr,BitsToSendCntr,1 
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		CAN_DATA_START,BitCounter.w0,32	
	CALL		PUSH_TO_FRAME_STACK				
	JMP 		CAN_DATA_START	

//////////////////////////////////////////////////////////////////////////////////////////
////////////// PUSH Data frame to stack //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////	

PUSH_TO_FRAME_STACK:
	MOV			BitCounter.w0,0					
	MOV32  		RegisterOffset,PRU_CAN_TRANSMIT_FRAME	
	SBBO   		CanFrame,RegisterOffset,CanCurrentState.b1,4 
	ADD			CanCurrentState.b1,CanCurrentState.b1,4
	XOR  		CanFrame,CanFrame, CanFrame
	RET

///CRC///////////////////CRC///////////////////CRC/////////////////////CRC/////////CRC///
///////////////****** START ******** ///////// CRC Calculate State Machine //////////////////
///CRC//////////////////CRC////////////////////CRC/////////////////////CRC/////////CRC///
CRC_CALCULATE_0:
	MOV			CRC_Input.w0,0
	LSR			CRC_TempVar.w0,CRC_TempVar.w2,14
	AND			CRC_TempVar.w0,CRC_TempVar.w0,1
	XOR			CRC_TempVar.w0,CRC_TempVar.w0,CRC_Input.w0
	LSL			CRC_TempVar.w2,CRC_TempVar.w2,2
	LSR			CRC_TempVar.w2,CRC_TempVar.w2,1
	QBBC		NO_EXOR_WITH_POLYNOMIAL_R0R1,CRC_TempVar.w0,0				
	MOV			CRC_Input.w2,0x4599
	XOR			CRC_TempVar.w2,CRC_TempVar.w2,CRC_Input.w2
	NO_EXOR_WITH_POLYNOMIAL_R0R1:
	RET
		
///CRC///////////////////CRC///////////////////CRC/////////////////////CRC/////////CRC///
///////////////****** END ******** ///////// CRC Calculate State Machine //////////////////
///CRC//////////////////CRC////////////////////CRC/////////////////////CRC/////////CRC///

///CRC///////////////////CRC///////////////////CRC/////////////////////CRC/////////CRC///
///////////////****** START ******** ///////// CRC Calculate State Machine //////////////////
///CRC//////////////////CRC////////////////////CRC/////////////////////CRC/////////CRC///
CRC_CALCULATE_1:
	MOV			CRC_Input.w0,1
	LSR			CRC_TempVar.w0,CRC_TempVar.w2,14
	AND			CRC_TempVar.w0,CRC_TempVar.w0,1
	XOR			CRC_TempVar.w0,CRC_TempVar.w0,CRC_Input.w0
	LSL			CRC_TempVar.w2,CRC_TempVar.w2,2
	LSR			CRC_TempVar.w2,CRC_TempVar.w2,1
	QBBC		NO_EXOR_WITH_POLYNOMIAL,CRC_TempVar.w0,0				
	MOV			CRC_Input.w2,0x4599
	XOR			CRC_TempVar.w2,CRC_TempVar.w2,CRC_Input.w2
	NO_EXOR_WITH_POLYNOMIAL:
	RET
		
///CRC///////////////////CRC///////////////////CRC/////////////////////CRC/////////CRC///
///////////////****** END ******** ///////// CRC Calculate State Machine ////////////////
///CRC//////////////////CRC////////////////////CRC/////////////////////CRC/////////CRC///	

///CRC///////////////////CRC///////////////////CRC/////////////////////CRC/////////CRC///
///////////////****** START ******** ///////// CRC Frame and Send State Machine /////////
///CRC//////////////////CRC////////////////////CRC/////////////////////CRC/////////CRC///	
		
CAN_FRAME_CRC_STATE:

	MOV 		BitCounter.w2,16
	MOV			RegisterValue,CRC_TempVar.w2
	LSL			RegisterValue,RegisterValue,1
	ADD			RegisterValue,RegisterValue,1

CAN_CRC_START:

	QBEQ 		STUFF_BITS_DONE,BitCounter.w2,0
	JMP			STUFF_BITS_CONTINUE

STUFF_BITS_DONE:
	MOV32		BitStuffCounter,0

STUFF_BITS_CONTINUE:
	QBEQ 		CAN_FRAME_SEND_STATE,BitCounter.w2,0
	MOV  		CanCurrentState.b0,6

	QBGT 		CAN_CRC_CNT,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK

CAN_CRC_CNT:
	SUB			BitCounter.w2,BitCounter.w2,1
	QBBS 		FRAME_BIT_ONE_6,RegisterValue,BitCounter.w2
	ADD			BitCounter.w2,BitCounter.w2,1

FRAME_BIT_ZERO_6:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1		
	LSL			CanFrame,CanFrame,1	
	ADD 		BitCounter.w0,BitCounter.w0,1

	QBGT 		BYTE_OFFSET_CRC_0,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK		
	
BYTE_OFFSET_CRC_0:
	ADD 		BitStuffCounter.w0,BitStuffCounter.w0,1
	MOV32  		ShiftRegister,0x0000FFFF
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ONE_6,BitStuffCounter.w0,5
	JMP 		CAN_CRC_START

FRAME_STUFF_ONE_6:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x00010000
	LSL			CanFrame,CanFrame,1
	SET  		CanFrame,CanFrame,0				
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		CAN_CRC_START,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		CAN_CRC_START

FRAME_BIT_ONE_6:
	ADD			BitCounter.w2,BitCounter.w2,1
	ADD			BitsToSendCntr,BitsToSendCntr,1
	SUB 		BitCounter.w2,BitCounter.w2,1
	LSL			CanFrame,CanFrame,1
	SET 		CanFrame,CanFrame,0
	ADD 		BitCounter.w0,BitCounter.w0,1	

	QBGT 		BYTE_OFFSET_CRC_1,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK				
	
BYTE_OFFSET_CRC_1:
	ADD 		BitStuffCounter.w2,BitStuffCounter.w2,1
	MOV32  		ShiftRegister,0xFFFF0000
	AND 		BitStuffCounter,BitStuffCounter,ShiftRegister
	QBEQ   		FRAME_STUFF_ZERO_6,BitStuffCounter.w2,5
	JMP 		CAN_CRC_START

FRAME_STUFF_ZERO_6:
	ADD			BitsToSendCntr,BitsToSendCntr,1
	MOV32 		BitStuffCounter,0x1	
	LSL			CanFrame,CanFrame,1			
	ADD 		BitCounter.w0,BitCounter.w0,1
	QBGT 		CAN_CRC_START,BitCounter.w0,32
	CALL		PUSH_TO_FRAME_STACK
	JMP 		CAN_CRC_START	
//=============================================================================
CAN_FRAME_SEND_STATE:
	QBEQ 		CAN_FRAME_RDY_TO_SEND,BitCounter.w0,0							
	
CAN_FRAME_DUMMY:
   // if BitCounter.w0 is less than 32, reset the non data bit 
	QBLE 		CAN_FRAME_RDY_TO_SEND,BitCounter.w0,32
	LSL			CanFrame,CanFrame,1	
	ADD 		BitCounter.w0,BitCounter.w0,1
	JMP			CAN_FRAME_DUMMY
			
CAN_FRAME_RDY_TO_SEND:
	// Write the current can frame to RAM and Reset CanFrame Reg
	MOV32  		RegisterOffset,PRU_CAN_TRANSMIT_FRAME
	SBBO   		CanFrame,RegisterOffset,CanCurrentState.b1,4 
	ADD			CanCurrentState.b1,CanCurrentState.b1,4
	XOR  		CanFrame,CanFrame, CanFrame
	
	// Load the Timer counter value
	MOV32  		RegisterOffset,PRU_CAN_TIMING_VAL_TX			
	LBBO		DelayCntr,RegisterOffset,0,4
//==================================================================================================================

//******************************************TIMER INITIALIZATION****************************************************

//==================================================================================================================	
TIMER_CONFIG:
	// Init the register pointer
	MOV32     	RegisterOffset, TMR_REGS

	// reset Register to zero
	XOR     	RegisterValue, RegisterValue, RegisterValue
			
	// Reset the Timer counter registers
	SBBO      	RegisterValue, RegisterOffset, TMR_TIM12_OFFSET,  4

	// Setting the Timer global control register, take time out of reset
	LDI       	RegisterValue.w0, 0x0015
	SBBO      	RegisterValue.w0, RegisterOffset, TMR_TGCR_OFFSET,  2
	
	// Load the time constant into the period register
	SBBO      	DelayCntr, RegisterOffset, TMR_PRD12_OFFSET,  4

	// Clear any pending interrupts,
	MOV     	RegisterValue.w0, 0x0002
	SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2

	// Enable the Timer in auto reload mode
	LDI       	RegisterValue.w0, TIMER12_MODE
	SBBO      	RegisterValue.w0, RegisterOffset, TMR_TCR_OFFSET,  2
			
	// Enable the Timer 1:2 interrupt
	LDI       	RegisterValue.w0, 0x0001
	SBBO      	RegisterValue.w0, RegisterOffset, TMR_INTCTL_OFFSET,  2
					
	// Clear Timer interrupt
	MOV32     	RegisterValue, (0x00000000 | TIMER_SYS_EVT)
	SBCO      	RegisterValue, CONST_PRUSSINTC, SICR_OFFSET, 4
			
	// Enable Timer 0 system interrupt
	SBCO      	RegisterValue, CONST_PRUSSINTC, EISR_OFFSET, 4			
			
	// Read BIT setup delay 
	MOV32     	RegisterOffset, PRU_CAN_TIMING_VAL_TX_SJW
	LBBO	  	DelayCntr, RegisterOffset, 0, 4	
	
	// load the frame Address where can frame is located	
	MOV32  		ShiftRegister,PRU_CAN_TRANSMIT_FRAME
	MOV  		BitStuffCounter.w2,0
				
CHECK_CAN_BUS_IDLE:
	MOV			RegisterValue.w0, 0	
			
CHECK_CAN_BUS_IDLE_LOOP:			
	QBBC		CHECK_CAN_BUS_IDLE, r31, GPIO_TX_MIRROR_BIT	
	ADD  		RegisterValue.w0,RegisterValue.w0,1
	CALL		TIMER_EVENT_WAIT
	QBNE		CHECK_CAN_BUS_IDLE_LOOP, RegisterValue.w0,11 //Wait for 11 bit high
		
	// Set the Flag that PRU is TX so the PRU RX do not ACK	
CAN_BUS_IDLE:		
	MOV32  		RegisterOffset,PRU_CAN_TX_BUS_AQUIRED			
	MOV32  		RegisterValue,1
	SBBO		RegisterValue,RegisterOffset,0,4

	XOR         CRC_TempVar, CRC_TempVar, CRC_TempVar  
	MOV		  	CRC_Input.w2, 0		// counter: DATA bit trasmitted excluding stuff bit

CAN_FRAME_NEXT_DATA:
	MOV  		BitStuffCounter.w0, 0
	LBBO		RegisterValue,ShiftRegister,BitStuffCounter.w2,4
	ADD			BitStuffCounter.w2,BitStuffCounter.w2,4				
								
CAN_FRAME_CURR_DATA:
	QBEQ		CAN_FRAME_ACK_STATE,BitsToSendCntr,0
	QBBS 		FRAME_BIT_ONE_SEND,RegisterValue,31

	GPIO_CLEAR	GPIO_TX_BIT	
						
	ADD			BitStuffCounter.w0,BitStuffCounter.w0,1
	SUB			BitsToSendCntr,BitsToSendCntr,1
	LSL			RegisterValue,RegisterValue,1			
	
	// Check if current bit is Stuff bit
	QBEQ        STUFF_BIT0,  CRC_TempVar.w2, 5
	ADD         CRC_Input.w2, CRC_Input.w2, 1   
	
STUFF_BIT0:				
	ADD      	CRC_TempVar.w0, CRC_TempVar.w0, 1   // Increament Stuff bit count for ZERO 
	MOV      	CRC_TempVar.w2, 0					// Zero The Stuff bit count
				
	CALL		TIMER_EVENT_WAIT
				
	QBEQ		CAN_FRAME_NEXT_DATA,BitStuffCounter.w0,32			
	JMP 		CAN_FRAME_CURR_DATA				
				
FRAME_BIT_ONE_SEND:
	GPIO_SET 	GPIO_TX_BIT

	ADD			BitStuffCounter.w0,BitStuffCounter.w0,1
	SUB			BitsToSendCntr,BitsToSendCntr,1
	LSL			RegisterValue,RegisterValue,1
	
	// Check if current bit is Stuff bit	
	QBEQ        STUFF_BIT1, CRC_TempVar.w0, 5				
	ADD         CRC_Input.w2, CRC_Input.w2, 1
STUFF_BIT1:				
	ADD      	CRC_TempVar.w2, CRC_TempVar.w2, 1
	MOV      	CRC_TempVar.w0, 0
				
	MOV			CRC_Input.w0, DelayCntr.w0
BIT_ONE_SETUP_DELAY:
	SUB       	CRC_Input.w0, CRC_Input.w0, 1
	QBLT	  	BIT_ONE_SETUP_DELAY, CRC_Input.w0, 0
	
	// If BitsToSendCntr is zero, Jump to wait for ACK
	QBEQ	  	CAN_ONE_LOOP_EXIT, BitsToSendCntr, 0
	
	// Moniter the CAN Bus, if it being driven low,
	// i.e. CRC_Input.w0=0xff -> Error Cond.
	XOR       	CRC_Input.w0, CRC_Input.w0, CRC_Input.w0
	CALL      	POLL_CAN_BUS_STATUS
	QBEQ      	CAN_FRAME_EXIT_ANALYSIS, CRC_Input.w0, 0xff
				
	CAN_ONE_LOOP_EXIT:						
	QBEQ	 	CAN_FRAME_NEXT_DATA,BitStuffCounter.w0,32	
	JMP 		CAN_FRAME_CURR_DATA
	
CAN_FRAME_ACK_STATE:				
	 
	GPIO_SET	GPIO_TX_BIT			// Set the Line high CRC Delimiter*******	

	CALL        TIMER_EVENT_WAIT
	MOV			CRCFrame.b1, 2      // Wait for ACK Bit for Two bit durations
	
	// Moniter the CAN Bus during ACK & ACK DELIMITER SLOT,
	//	if it being driven low, CRC_Input.w0=0 ->No Ack
	XOR     	CRC_Input.w0, CRC_Input.w0, CRC_Input.w0
TIMER_ACK_POLL:													
	CALL    	POLL_CAN_BUS_STATUS 
	SUB			CRCFrame.b1, CRCFrame.b1, 1
	QBLT		TIMER_ACK_POLL, CRCFrame.b1, 0 
	QBEQ      	CAN_FRAME_EXIT_ANALYSIS_PASSIVE, CRC_Input.w0, 0		
	
	// Ack bit received; update status reg & Generate 7 bit EOF
FRAME_ACK_SUCCESS:				
	GPIO_SET	GPIO_TX_BIT
	MOV  		BitStuffCounter.w0, END_OF_FRAME_CNT
	
	// Reset the ERR bit in TX INTR Status Reg
	MOV32  		RegisterOffset,PRU_CAN_TX_INTERRUPT_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	CLR    		RegisterValue, RegisterValue, CAN_INTR_ERR_BIT
    SBBO   		RegisterValue,RegisterOffset,0,4

	// Decreament Tx Err Cnt, only if not zero, otherwise JMP to CAN_FRAME_EOF_STATE
	MOV32  		RegisterOffset,PRU_CAN_TX_ERROR_COUNTER_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	
	QBEQ		CAN_FRAME_EOF_STATE,RegisterValue,0
	
	SUB			RegisterValue,RegisterValue,1   
	SBBO   		RegisterValue,RegisterOffset,0,4

	// Check if both TX and RX Err count not equal to 127, 
	// reset them to Active ERR
	QBNE        CAN_FRAME_EOF_STATE,RegisterValue,(PRU_CAN_ERROR_ACTIVE -1)
	
	MOV32       RegisterOffset,PRU_CAN_RX_ERROR_COUNTER_REGISTER
    LBBO        RegisterValue,RegisterOffset,0,4	
	
	// chek if RX Err count is less than 128 Reset to Active State
	QBLT        CAN_FRAME_EOF_STATE, RegisterValue, (PRU_CAN_ERROR_ACTIVE -1)
		
	MOV32  		RegisterOffset,PRU_CAN_TX_GLOBAL_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	CLR			RegisterValue, CAN_PASSIVE_ERR_BIT
	SBBO   		RegisterValue,RegisterOffset,0,4
	
	// Generate EOF Loop			
	CAN_FRAME_EOF_STATE:								
	CALL		TIMER_EVENT_WAIT
	SUB			BitStuffCounter.w0,BitStuffCounter.w0,1
	QBLT		CAN_FRAME_EOF_STATE,BitStuffCounter.w0,0
	MOV			CRCFrame.b0,0
	JMP			TRANSMIT_DONE
	
CAN_FRAME_EXIT_ANALYSIS_PASSIVE:
//	MOV32  		ShiftRegister,PRU_CAN_TX_GLOBAL_STATUS_REGISTER
//	LBBO   		RegisterValue,ShiftRegister,0,4	
//	QBBS        ERR_PASSIVE,RegisterValue, 3  
	
CAN_FRAME_EXIT_ANALYSIS:
	QBEQ    	CAN_TRANS_EXTD_FRM_EXIT,Identifier,CAN_EXTD_ID
	
	// STD Can Frame subtract the Start of Frame bit,check for ARBTR_FAILED
	SUB         CRC_Input.w2,CRC_Input.w2,1            
	QBGE		CAN_ARBTR_FAILED,CRC_Input.w2, CAN_STD_FRAME_ID			
	
	// No ARBTR Failure, bit error
	MOV32		CanCurrentState,CAN_BIT_ERROR
	JMP			CAN_FRAME_EXIT
	
	// EXTD Can Frame subtract the SOF+SRR+EDI bit from Count,check for ARBTR_FAILED	
CAN_TRANS_EXTD_FRM_EXIT:
	SUB         CRC_Input.w2,CRC_Input.w2,3    
	QBGE		CAN_ARBTR_FAILED,CRC_Input.w2,CAN_EXTD_FRAME_ID
	
	// No ARBTR Failure, bit error	
	MOV32		CanCurrentState,CAN_BIT_ERROR
	JMP			CAN_FRAME_EXIT

	// CAN ARBTR FAILED, reset PRU_CAN_TX_BUS_AQUIRED flag, retrycnt (3) is less than 3,jmp CHECK_CAN_BUS_IDLE 
CAN_ARBTR_FAILED:									
	MOV32  		RegisterOffset,PRU_CAN_TX_BUS_AQUIRED			
	XOR  		RegisterValue,RegisterValue, RegisterValue
	SBBO		RegisterValue,RegisterOffset,0,4
				
	MOV32		CanCurrentState,CAN_ARBTR_FAIL
	ADD			CRCFrame.b0,CRCFrame.b0,1
	QBGE		CHECK_CAN_BUS_IDLE,CRCFrame.b0,ARBTR_RETRY_CNT
	
	// Arbitration Failure interrupt check in mail box  reg
	XOR			RegisterValue, RegisterValue, RegisterValue
	SET			RegisterValue,	RegisterValue, CAN_ARBTR_ERR_BIT
	JMP			UPDATE_ARM_DSP 

	// Update the Errror Counter, Increamnet TX Err count by 8
CAN_FRAME_EXIT:	
	MOV			CRCFrame.b0,0
	MOV32  		RegisterOffset,PRU_CAN_TX_ERROR_COUNTER_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	ADD			RegisterValue,RegisterValue, 8  
	SBBO   		RegisterValue,RegisterOffset,0,4
	
	QBLE		BUS_OFF,RegisterValue, PRU_CAN_BUS_OFF_ERR_CNT
	QBLE		ERR_PASSIVE,RegisterValue, PRU_CAN_ERROR_ACTIVE

	MOV32      	RegisterOffset, PRU_CAN_RX_GLOBAL_STATUS_REGISTER
	LBBO		RegisterValue, RegisterOffset, 0, 4
	QBBS		ERR_PASSIVE, RegisterValue, CAN_PASSIVE_ERR_BIT
	JMP			ERR_ACTIVE_FRAME

	// Bus off Error Update the RX PRU Flag to start recover logic  
BUS_OFF:
	MOV32  		RegisterOffset,PRU_CAN_TX_GLOBAL_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	SET			RegisterValue,CAN_BUS_OFF_ERR_BIT
	CLR			RegisterValue,CAN_PASSIVE_ERR_BIT
	SBBO   		RegisterValue,RegisterOffset,0,4
	JMP			EXIT_ERROR
	
	// Passive Error State of Can Node
ERR_PASSIVE:
	
	MOV32  		RegisterOffset,PRU_CAN_TX_GLOBAL_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	SET			RegisterValue,CAN_PASSIVE_ERR_BIT
	SBBO   		RegisterValue,RegisterOffset,0,4
	
	// Reset The Flag That PRU is Transmitter
	MOV32  		RegisterOffset,PRU_CAN_TX_BUS_AQUIRED			
	XOR  		RegisterValue,RegisterValue, RegisterValue
	SBBO		RegisterValue,RegisterOffset,0,4	
	
	// Generate Passive Error Frame
	MOV     	RegisterValue.w0, 0
TX_PASSIVE_ERR_FRAME:			
	GPIO_SET	GPIO_TX_BIT   
	CALL		TIMER_EVENT_WAIT	
	ADD			RegisterValue.w0, RegisterValue.w0, 1
	QBGT    	TX_PASSIVE_ERR_FRAME, RegisterValue.w0, ERR_FRAME_LENGTH
	JMP			ERROR_DELIMITER
			
	// Active Error State of Can Node
ERR_ACTIVE_FRAME:
	
	// Generate Active Error Frame
	MOV     	RegisterValue.w0, 0
TX_ACTIVE_ERR_FRAME:
	GPIO_CLEAR	GPIO_TX_BIT   
	CALL		TIMER_EVENT_WAIT	
	ADD     	RegisterValue.w0, RegisterValue.w0, 1
	QBGT    	TX_ACTIVE_ERR_FRAME, RegisterValue.w0, ERR_FRAME_LENGTH

	// Generate 8 bit High Error Frame
ERROR_DELIMITER:
	XOR     	ShiftRegister.w2, ShiftRegister.w2, ShiftRegister.w2			
ERROR_DELIMITER_FRAME:	
	MOV     	RegisterValue.w0, 0
	XOR     	ShiftRegister.w0, ShiftRegister.w0, ShiftRegister.w0
	
	// Generate Error  Delimiter Frame
ERROR_DELIMITER_FRAME_LOOP:
	GPIO_SET	GPIO_TX_BIT   
	XOR       	CRC_Input.w0, CRC_Input.w0, CRC_Input.w0
	CALL		POLL_CAN_BUS_STATUS
	QBEQ      	NO_DELIMITER_ERR, CRC_Input.w0, 0x00
	ADD			ShiftRegister.w0, ShiftRegister.w0, 1
	
NO_DELIMITER_ERR:
	ADD     	RegisterValue.w0, RegisterValue.w0, 1
	QBGT    	ERROR_DELIMITER_FRAME_LOOP, RegisterValue.w0, ERR_DELMTR_LEN		
//	QBEQ		ERROR_DELIMITER_FRAME_EXIT, ShiftRegister.w0, 0	
//	ADD			ShiftRegister.w2, ShiftRegister.w2, 8
//	QBGT		ERROR_DELIMITER_FRAME_EXIT, ShiftRegister.w0, 8
//	QBEQ		ERROR_DELIMITER_FRAME, ShiftRegister.w0, 8
	
ERROR_DELIMITER_FRAME_EXIT:	
	MOV32  		RegisterOffset,PRU_CAN_TX_ERROR_COUNTER_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	ADD			RegisterValue,RegisterValue, ShiftRegister.w2
	SBBO   		RegisterValue,RegisterOffset,0,4	
EXIT_ERROR:
	MOV32  		RegisterOffset,PRU_CAN_TX_INTERRUPT_STATUS_REGISTER
	LBBO   		RegisterValue,RegisterOffset,0,4	
	
	// Set the Error Flag 
	SET    		RegisterValue, RegisterValue, CAN_INTR_ERR_BIT
	SET			RegisterValue, CurrentMailbox
    SBBO   		RegisterValue, RegisterOffset,0,4
	
	MOV			RegisterValue,0x21	
	JMP			IFS_FRAME
	
TRANSMIT_DONE:	
	MOV32  		RegisterOffset,PRU_CAN_TX_INTERRUPT_STATUS_REGISTER
	LBBO		RegisterValue,RegisterOffset,0,4
	SET			RegisterValue,CurrentMailbox	
	SBBO		RegisterValue,RegisterOffset,0,4		
	MOV			RegisterValue,0x41	

IFS_FRAME:
	// Generate 3 Bit Duaration High State on CAN BUS as IFS
	GPIO_SET	GPIO_TX_BIT
	
	// Reset The Flag That PRU is Transmitter
	MOV32  		RegisterOffset,PRU_CAN_TX_BUS_AQUIRED			
	XOR  		ShiftRegister,ShiftRegister, ShiftRegister
	SBBO		ShiftRegister,RegisterOffset,0,4	

	CALL    	TIMER_EVENT_WAIT
	CALL    	TIMER_EVENT_WAIT
	CALL    	TIMER_EVENT_WAIT
			
UPDATE_ARM_DSP:
	QBEQ		MAILBOX0_DONE,CurrentMailbox,0
	QBEQ		MAILBOX1_DONE,CurrentMailbox,1
	QBEQ		MAILBOX2_DONE,CurrentMailbox,2
	QBEQ		MAILBOX3_DONE,CurrentMailbox,3
	QBEQ		MAILBOX4_DONE,CurrentMailbox,4
	QBEQ		MAILBOX5_DONE,CurrentMailbox,5
	QBEQ		MAILBOX6_DONE,CurrentMailbox,6
	QBEQ		MAILBOX7_DONE,CurrentMailbox,7
	JMP			CAN_EMULATION_TX_START

MAILBOX0_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX0_STATUS_REGISTER				
	SBBO		RegisterValue,RegisterOffset,0,4				
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX1_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX1_STATUS_REGISTER					
	SBBO		RegisterValue,RegisterOffset,0,4
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX2_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX2_STATUS_REGISTER					
	SBBO		RegisterValue,RegisterOffset,0,4
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX3_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX3_STATUS_REGISTER				
	SBBO		RegisterValue,RegisterOffset,0,4
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX4_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX4_STATUS_REGISTER					
	SBBO		RegisterValue,RegisterOffset,0,4
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX5_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX5_STATUS_REGISTER					
	SBBO		RegisterValue,RegisterOffset,0,4
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX6_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX6_STATUS_REGISTER				
	SBBO		RegisterValue,RegisterOffset,0,4
	JMP			PRU_TX_CMPLT_GEN_INTR

MAILBOX7_DONE:
	MOV32  		RegisterOffset,PRU_CAN_TX_MAILBOX7_STATUS_REGISTER				
	SBBO		RegisterValue,RegisterOffset,0,4
	
PRU_TX_CMPLT_GEN_INTR:

    // Make sure the SYS_EVT32 system interrupt is cleared
    MOV32     	RegisterValue, 34
    SBCO      	RegisterValue, CONST_PRUSSINTC, SICR_OFFSET, 4

    // Enable SYS_EVT32 system interrupt
    SBCO      	RegisterValue, CONST_PRUSSINTC, EISR_OFFSET,  4

    //Generate SYS_EVT32 by event out mapping
    MOV32     	r31, 34   
											
	JMP       	CAN_EMULATION_TX_START
			
.end

//=================================================================================================================

//**********************************************POLL for TIMER INTERRUPT*******************************************

//=================================================================================================================

POLL_CAN_BUS_STATUS:
	MOV	     	RegisterOffset.w2, INTCTL_BASE 
	MOV		 	RegisterOffset.w0, SECR1_OFFSET											
	
CAN_BUS_POLL:	
	QBBS	 	CAN_BUS_SET, r31, GPIO_TX_MIRROR_BIT
	MOV		 	CRC_Input.w0, 0xff
				
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
	
	// Enable Timer 0 system interrupt
	SBCO      	TempRegister, CONST_PRUSSINTC, EISR_OFFSET, 4
	RET

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
	
	// Enable Timer  system interrupt
	SBCO      	TempRegister, CONST_PRUSSINTC, EISR_OFFSET, 4
	RET
.end
