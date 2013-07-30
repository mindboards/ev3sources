/*
 * linux/<file location within the kernel tree>
 *
 * Copyright (C) 2010 Texas Instruments Incorporated
 * Author: Wilfred Felix
 *
 * Based on <Give reference of old kernel file from which this file is derived from>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as  published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

/* *********************************************************************************************************
  FILE        	: 	PRU_CAN_Emulation_APIs.c 				                             	 	        
  PROJECT		: 	DA8xx/OMAP-L138/C674x PRU Development
  DESC        	: 	PRU Load , Run , Can Interface APIs
  VERSION		: 	1.0   
* **********************************************************************************************************/

#include "pru_can_emulation_api.h"
#include "pru.h"
#include "csl/cslr_syscfg0_OMAPL138.h"

can_emulation_drv_inst gstrcanemulationinst[ecanmaxinst];
arm_pru_iomap str_pru_iomap;
u32 gu32canpruiomapaddress;
u32 gu32cansyscfgmapaddress;
u32 gu32canpsc1mapaddress;
u32 gu32canpsc0mapaddress;
u32 gu32pruclock;

/** ********************************************************************************************************
 * \brief    pru_can_enable()		Configure and Enable PRU0 and PRU1 of OMAP L138.
 *
 * This API will be called by the Application to Configure and Enable PRU0 and PRU1
 *
 * \param None
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_enable(void) {
	CSL_PrucoreRegsOvly hPru;
	CSL_SyscfgRegsOvly sysCfg0;
	short s16returncapture;

	/* Set pru to Alternate mode for timer interrupts */
	sysCfg0 = 
		(CSL_SyscfgRegsOvly) gu32cansyscfgmapaddress;

	sysCfg0->KICK0R = 0x83E70B13;
	sysCfg0->KICK1R = 0x95A4F1E0;
	sysCfg0->CFGCHIP3 |= (CSL_SYSCFG_CFGCHIP3_PRUEVTSEL_ALTERNATE << CSL_SYSCFG_CFGCHIP3_PRUEVTSEL_SHIFT);
	sysCfg0->KICK0R = 0x0;
	sysCfg0->KICK1R = 0x0;

	/******* Enable PRU Sub System ******/
	s16returncapture = pru_can_psc_enable();
	if (s16returncapture == -1) {
		return -1;
	}

	/******* Reset PRU 0 ******/
	hPru =
	    (CSL_PrucoreRegsOvly) (gu32canpruiomapaddress | CSL_PRUCANCORE_0_REGS);
	hPru->CONTROL = CSL_PRUCORE_CONTROL_RESETVAL;

	/******* Reset PRU 0 ******/
	hPru =
	    (CSL_PrucoreRegsOvly) (gu32canpruiomapaddress | CSL_PRUCANCORE_1_REGS);
	hPru->CONTROL = CSL_PRUCORE_CONTROL_RESETVAL;

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_disable()		Disable PRU0 and PRU1 of OMAP L138.
 *
 * This API will be called by the Application to disable PRU0 and PRU1
 *
 * \param None
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_disable(void) {
	CSL_PrucoreRegsOvly hPru;
	short s16returncapture;

	/******* DisablePRU 0 ******/
	hPru =
	    (CSL_PrucoreRegsOvly) (gu32canpruiomapaddress | CSL_PRUCANCORE_0_REGS);
	CSL_FINST(hPru->CONTROL, PRUCORE_CONTROL_COUNTENABLE, DISABLE);
	CSL_FINST(hPru->CONTROL, PRUCORE_CONTROL_ENABLE, DISABLE);

	/******* Reset PRU 0 ******/
	hPru->CONTROL = CSL_PRUCORE_CONTROL_RESETVAL;
	/******* Disable PRU 1 ******/
	hPru =
	    (CSL_PrucoreRegsOvly) (gu32canpruiomapaddress | CSL_PRUCANCORE_1_REGS);
	CSL_FINST(hPru->CONTROL, PRUCORE_CONTROL_COUNTENABLE, DISABLE);
	CSL_FINST(hPru->CONTROL, PRUCORE_CONTROL_ENABLE, DISABLE);

	/******* Reset PRU 1 ******/
	hPru->CONTROL = CSL_PRUCORE_CONTROL_RESETVAL;
	/******* Disable PRU Sub System ******/
	s16returncapture = pru_can_psc_disable();
	if (s16returncapture == -1) {
		return -1;
	}

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_run ()	Allows the PRU0 or PRU1 of OMAP L138 to execute the code loaded into its Instruction RAM
 *
 * This API will be called by the Application to execute the instruction in PRU
 *
 * \param 	u8prunum		The PRU number to initiate execution
 *
 * \return   	SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_run(u8 u8prunum) {
	CSL_PrucoreRegsOvly hPru;

	/******* Choose the  PRU to RUN *******/
	if (u8prunum == CSL_PRUCORE_0) {
		hPru =
		    (CSL_PrucoreRegsOvly) (gu32canpruiomapaddress |
					   CSL_PRUCANCORE_0_REGS);
	} else if (u8prunum == CSL_PRUCORE_1) {
		hPru =
		    (CSL_PrucoreRegsOvly) (gu32canpruiomapaddress |
					   CSL_PRUCANCORE_1_REGS);
	} else {
		return -1;
	}

	/******* Enable PRU Clock, *******/
	CSL_FINST(hPru->CONTROL, PRUCORE_CONTROL_COUNTENABLE, ENABLE);
	CSL_FINST(hPru->CONTROL, PRUCORE_CONTROL_ENABLE, ENABLE);

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_psc_enable ()	Enable state transition of PRU
 *
 * This API will be called by the Application for state transition of PRU
 *
 * \param 	None
 *
 * \return   	SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_psc_enable(void) {
	u32 u32timeout;
	bool btransitioncomplete;
	CSL_PscRegsOvly PSC = (CSL_PscRegsOvly) gu32canpsc0mapaddress;

	u32timeout = 0;
	btransitioncomplete = false;

	/** Wait for any outstanding transition to complete ******************/
	while (btransitioncomplete == false) {
		if (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
		    CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) {
			btransitioncomplete = true;
		}
		/** If the process is timed out return failure *******************/
		u32timeout++;
		if (u32timeout == PRU_CAN_INIT_MAX_TIMEOUT) {
			return -1;
		}
	}

	/** If already in that state, just return ******************/
	if (CSL_FEXT(PSC->MDSTAT[CSL_PSC_PRU], PSC_MDSTAT_STATE) ==
	    CSL_PSC_MDSTAT_STATE_ENABLE) {
		return 0;
	}

	/** Perform transition ******************/
	CSL_FINST(PSC->MDCTL[CSL_PSC_PRU], PSC_MDCTL_NEXT, ENABLE);
	CSL_FINST(PSC->PTCMD, PSC_PTCMD_GO0, SET);

	u32timeout = 0;
	btransitioncomplete = false;

	/** Wait for transition to complete ******************/
	while (btransitioncomplete == false) {
		if (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
		    CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) {
			btransitioncomplete = true;
		}
		/** If the process is timed out return failure *******************/
		u32timeout++;
		if (u32timeout == PRU_CAN_INIT_MAX_TIMEOUT) {
			return -1;
		}
	}

	u32timeout = 0;
	btransitioncomplete = false;

	/** Wait and verify the state ******************/
	while (btransitioncomplete == false) {
		if (CSL_FEXT(PSC->MDSTAT[CSL_PSC_PRU], PSC_MDSTAT_STATE) !=
		    CSL_PSC_MDSTAT_STATE_ENABLE) {
			btransitioncomplete = true;
		}
		/** If the process is timed out return failure *******************/
		u32timeout++;
		if (u32timeout == PRU_CAN_INIT_MAX_TIMEOUT) {
			return -1;
		}
	}

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_psc_disable ()	Disable state transition of PRU
 *
 * This API will be called by the Application for disabling state transition of PRU
 *
 * \param 	None
 *
 * \return   	SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_psc_disable(void) {

	u32 u32timeout;
	bool btransitioncomplete;
	CSL_PscRegsOvly PSC = (CSL_PscRegsOvly) gu32canpsc0mapaddress;

	u32timeout = 0;
	btransitioncomplete = false;

	/** Wait for any outstanding transition to complete ******************/
	while (btransitioncomplete == false) {
		if (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
		    CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) {
			btransitioncomplete = true;
		}
		/** If the process is timed out return failure *******************/
		u32timeout++;
		if (u32timeout == PRU_CAN_INIT_MAX_TIMEOUT) {
			return -1;
		}
	}

	/** If already in that state, just return ******************/
	if (CSL_FEXT(PSC->MDSTAT[CSL_PSC_PRU], PSC_MDSTAT_STATE) ==
	    CSL_PSC_MDSTAT_STATE_SYNCRST) {
		return 0;
	}

	/** Perform transition ******************/
	CSL_FINST(PSC->MDCTL[CSL_PSC_PRU], PSC_MDCTL_NEXT, SYNCRST);
	CSL_FINST(PSC->PTCMD, PSC_PTCMD_GO0, SET);

	u32timeout = 0;
	btransitioncomplete = false;

	/** Wait for transition to complete ******************/
	while (btransitioncomplete == false) {
		if (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
		    CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) {
			btransitioncomplete = true;
		}
		/** If the process is timed out return failure *******************/
		u32timeout++;
		if (u32timeout == PRU_CAN_INIT_MAX_TIMEOUT) {
			return -1;
		}
	}

	u32timeout = 0;
	btransitioncomplete = false;

	/** Wait and verify the state ******************/
	while (btransitioncomplete == false) {
		if (CSL_FEXT(PSC->MDSTAT[CSL_PSC_PRU], PSC_MDSTAT_STATE) !=
		    CSL_PSC_MDSTAT_STATE_SYNCRST) {
			btransitioncomplete = true;
		}
		/** If the process is timed out return failure *******************/
		u32timeout++;
		if (u32timeout == PRU_CAN_INIT_MAX_TIMEOUT) {
			return -1;
		}
	}

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_ram_write_data()		Download the data into data RAM of PRU0 or PRU1 of OMAP L138.
 *
 * This API will be called by the Application to download the data into data RAM of PRU0 or PRU1
 *
 * \param 	u32offset			Offset of the data RAM where the data has to be written
 * \param	pu32datatowrite		Pointer to a buffer that holds the data to be written into RAM
* \param	u16wordstowrite		Number of words to be written into that RAM
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_ram_write_data
    (u32 u32offset, u32 * pu32datatowrite, u16 u16wordstowrite) {

	u32 *pu32addresstowrite;
	u16 u16loop;

	pu32addresstowrite = (u32 *) (u32offset);

	for (u16loop = 0; u16loop < u16wordstowrite; u16loop++) {
		*pu32addresstowrite = *pu32datatowrite;
		pu32datatowrite++;
		pu32addresstowrite++;
	}

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_ram_read_data()		Download the data into data RAM of PRU0 or PRU1 of OMAP L138.
 *
 * This API will be called by the Application to read the data from data RAM of PRU0 or PRU1
 *
 * \param 	u32offset			Offset of the data RAM where the data has to be read
 * \param	pu32datatoread		Pointer to a buffer that would hold the data to be read from the RAM
* \param	u16wordstoread		Number of words to be read from RAM
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_ram_read_data
    (u32 u32offset, u32 * pu32datatoread, u16 u16wordstoread) {

	u32 *pu32addresstoread;
	u16 u16loop;

	pu32addresstoread = (u32 *) (u32offset);

	for (u16loop = 0; u16loop < u16wordstoread; u16loop++) {
		*pu32datatoread = *pu32addresstoread;
		pu32datatoread++;
		pu32addresstoread++;
	}

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_download_firmware()		Download the firmware into PRU0 and PRU1 of OMAP L138.
 *
 * This API will be called by the Application to download the code into instruction RAM of PRU0 
 * and PRU1
 *
 * \param 	pstrfirmwaredata		Pointer to structure holding firmware data
 * \param 	u8prunum			The PRU number to download firmware
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_download_firmware
    (pru_can_firmware_structure * pstrfirmwaredata, u8 u8prunum) {

	u32 *pu32pruinstructionram;
	u32 u32codesizeinwords;
	u32 *pu32prucode;
	u32 u32counter;

	pu32pruinstructionram = NULL;
	u32codesizeinwords = 0;

	/** Check for valid input parameter  *******************/
	if (pstrfirmwaredata == NULL) {
		return -1;
	}
	/** Choose the PRU to be loaded with instructions  ********/
	if (u8prunum == CSL_PRUCORE_0) {
		pu32pruinstructionram =
		    (u32 *) (gu32canpruiomapaddress | PRU0_PROG_RAM_START_OFFSET);
		pu32prucode = (u32 *) pstrfirmwaredata->ptr_pru0;
		u32codesizeinwords = pstrfirmwaredata->u32_pru0_code_size;
	} else if (u8prunum == CSL_PRUCORE_1) {
		pu32pruinstructionram =
		    (u32 *) (gu32canpruiomapaddress | PRU1_PROG_RAM_START_OFFSET);
		pu32prucode = (u32 *) pstrfirmwaredata->ptr_pru1;
		u32codesizeinwords = pstrfirmwaredata->u32_pru1_code_size;
	} else {
		return -1;
	}

	/** Copy PRU code to its instruction RAM *****************/
	for (u32counter = 0; u32counter < u32codesizeinwords; u32counter++) {
		pu32pruinstructionram[u32counter] = pu32prucode[u32counter];
	}

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_set_brp()		Updates the  BRP register of PRU0 and PRU1 of OMAP L138.
 *
 * This API will be called by the Application to updtae the BRP register of PRU0 and PRU1
 *
 * \param 	u16bitrateprescaler		The can bus bitrate prescaler value be set
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_set_brp(u16 u16bitrateprescaler) {

	u32 u32offset;

	if (u16bitrateprescaler > 255) {
		return -1;
	}

	u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_CLOCK_BRP_REGISTER);
	pru_can_ram_write_data(u32offset, (u32 *) & u16bitrateprescaler, 1);

	u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_CLOCK_BRP_REGISTER);
	pru_can_ram_write_data(u32offset, (u32 *) & u16bitrateprescaler, 1);

	return 0;

}

/** End of   pru_can_set_brp() ********************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_set_bit_timing()		Updates the  timing register of PRU0 and PRU1 of OMAP L138.
 *
 * This API will be called by the Application to updtae the timing register of PRU0 and PRU1
 *
 * \param 	pstrcanbittiming		Pointer to structure holding the bit timing values for can bus
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_set_bit_timing(can_bit_timing_consts * pstrcanbittiming) {

	u32 u32offset;
	u32 u32serregister;

	u32serregister = 0;

	if (pstrcanbittiming == NULL) {
		return -1;
	}

	if ((pstrcanbittiming->u8syncjumpwidth > PRU_CAN_MAX_SJW) ||
	    (pstrcanbittiming->u8phseg1 > PRU_CAN_MAX_PHSEG1) ||
	    (pstrcanbittiming->u8phseg2 > PRU_CAN_MAX_PHSEG2)) {
		return -1;
	}

	u32serregister = u32serregister |
	    ((pstrcanbittiming->u8syncjumpwidth << 7) |
	     (pstrcanbittiming->u8phseg1 << 3) | (pstrcanbittiming->u8phseg2));

	u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_TIMING_REGISTER);
	pru_can_ram_write_data(u32offset, (u32 *) & u32serregister, 1);

	u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_TIMING_REGISTER);
	pru_can_ram_write_data(u32offset, (u32 *) & u32serregister, 1);

	return 0;
}

/** End of   pru_can_set_bit_timing() ************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_calculatetiming()		Updates the  timing values of PRU0 and PRU1 of OMAP L138.
 *
 * This API will be called by the Application to updtae the timing values of PRU0 and PRU1
 *
 * \param 	u32canbittiming		Bit timing values for can bus
 * \param 	u32bitrateprescaler	Bit Rate Prescaler  for can bus
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_calculatetiming(u32 pru_freq, u32 bit_rate) {
	u16 u16phaseseg1;
	u16 u16phaseseg2;
	u32 u32offset;
	u32 u32TimingValue;
	u32 u32SetupValue;
//	u32TimingValue = (u32bitrateprescaler - 1) / 3;
//	u32TimingValue -= 20; //Adjust the STATIC delay
	u32TimingValue = TIMER_CLK_FREQ/bit_rate;

	u32offset = (gu32canpruiomapaddress | PRU_CAN_TIMING_VAL_TX);
	pru_can_ram_write_data(u32offset, (u32 *) & u32TimingValue, 4);
	pru_can_ram_read_data(u32offset, (u32 *) & u32TimingValue, 4);

	u32SetupValue = (GPIO_SETUP_DELAY*(pru_freq/1000000)/1000)/DELAY_LOOP_LENGTH;
	u32offset = (gu32canpruiomapaddress | (PRU_CAN_TIMING_VAL_TX_SJW)) ;
	pru_can_ram_write_data(u32offset, (u32 *) & u32SetupValue, 4);

	u16phaseseg1 = (u16) (u32TimingValue / 2);
	u16phaseseg2 = u32TimingValue - u16phaseseg1;
	u16phaseseg1 -= TIMER_SETUP_DELAY;
	u16phaseseg2 -= TIMER_SETUP_DELAY;
	u32SetupValue = (u16phaseseg1 << 16) | u16phaseseg2;

	u32offset = (gu32canpruiomapaddress | PRU_CAN_TIMING_VAL_RX);
	pru_can_ram_write_data(u32offset, (u32 *) & u32SetupValue, 4);

	u32offset = (gu32canpruiomapaddress | (PRU_CAN_TIMING_VAL_RX+4)) ;
	pru_can_ram_write_data(u32offset, (u32 *) & u32TimingValue, 4);

	
	return 0;
}

/** End of   pru_can_calculatetiming() ************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_write_data_to_mailbox()		Updates the transmit mailboxes of PRU1 of OMAP L138.
 *
 * This API will be called by the Application to updtae the transmit mailboxes of PRU1
 *
 * \param 	pu16canframedata	Can mailbox data buffer
 * \param 	u8mailboxnum		Mailbox to be updated
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_write_data_to_mailbox(can_emulation_app_hndl * pstrcanemuapphndl) {
	short s16subrtnretval;
	u32 u32offset;

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}

	switch ((u8) pstrcanemuapphndl->ecanmailboxnumber) {
	case 0:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX0);
		break;
	case 1:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX1);
		break;
	case 2:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX2);
		break;
	case 3:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX3);
		break;
	case 4:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX4);
		break;
	case 5:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX5);
		break;
	case 6:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX6);
		break;
	case 7:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_TX_MAILBOX7);
		break;
	default:
		return -1;
	}

	s16subrtnretval =
	    pru_can_ram_write_data(u32offset,
			       (u32 *) & (pstrcanemuapphndl->strcanmailbox), 4);
	if (s16subrtnretval == -1) {
		return -1;
	}
	return 0;
}

/** End of pru_can_write_data_to_mailbox()************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_get_data_from_mailbox()		Receive data from the receive mailboxes of PRU0  of OMAP L138.
 *
 * This API will be called by the Application to get data from the receive mailboxes of PRU0 
 *
 * \param 	pu16canframedata	Can mailbox data buffer
 * \param 	u8mailboxnum		Mailbox to be updated
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_get_data_from_mailbox
    (can_emulation_app_hndl * pstrcanemuapphndl) {

	short s16subrtnretval;
	u32 u32offset;

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}

	switch ((u8) pstrcanemuapphndl->ecanmailboxnumber) {
	case 0:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX0);
		break;
	case 1:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX1);
		break;
	case 2:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX2);
		break;
	case 3:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX3);
		break;
	case 4:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX4);
		break;
	case 5:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX5);
		break;
	case 6:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX6);
		break;
	case 7:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX7);
		break;
	case 8:
		u32offset = (gu32canpruiomapaddress | PRU_CAN_RX_MAILBOX8);
		break;
	default:
		return -1;
	}

	s16subrtnretval =
	    pru_can_ram_read_data(u32offset,
			      (u32 *) & (pstrcanemuapphndl->strcanmailbox), 4);
	if (s16subrtnretval == -1) {
		return -1;
	}
	return 0;
}

/** End of pru_can_get_data_from_mailbox()************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_receive_id_map()		Receive mailboxes ID Mapping of PRU0  of OMAP L138.
 *
 * This API will be called by the Application to map the IDs  to receive mailboxes of PRU0 
 *
 * \param 	u32nodeid			Can node ID
 * \param 	ecanmailboxno		Mailbox to be mapped
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_receive_id_map(u32 u32nodeid, can_mailbox_number ecanmailboxno) {

	pru_can_ram_write_data((gu32canpruiomapaddress |
			    (PRU_CAN_ID_MAP + (((u8) ecanmailboxno) * 4))),
			   (u32 *) & u32nodeid, 1);

	return 0;
}

/** End of pru_can_receive_id_map()***********************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_get_interrupt_status()		Gets the interrupts status register value 
 *
 * This API will be called by the Application  to get the interrupts status register value 
 *
 * \param 	pu32intrstatus			Pointer to a var where interrupt status register value has to be written
 * \param 	u8prunumber			PRU number for which IntStatusReg  has to be read 
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_get_interrupt_status(can_emulation_app_hndl * pstrcanemuapphndl) {
	u32 u32offset;
	short s16subrtnretval = -1;

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}

	if (pstrcanemuapphndl->u8prunumber == CSL_PRUCORE_1) {
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_TX_INTERRUPT_STATUS_REGISTER);
	} else if (pstrcanemuapphndl->u8prunumber == CSL_PRUCORE_0) {
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_RX_INTERRUPT_STATUS_REGISTER);
	} else {
		return -1;
	}

	s16subrtnretval =
	    pru_can_ram_read_data(u32offset,
			      (u32 *) & pstrcanemuapphndl->u32interruptstatus,
			      1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	return 0;
}

/** End of pru_can_get_interrupt_status() ***********************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_get_global_status()		Gets the globalstatus register value 
 *
 * This API will be called by the Application to  get the global status register value 
 *
 * \param 	pu32globalstatus			Pointer to a var where global status register value has to be written
 * \param 	u8prunumber			PRU number for which GlobalStatusReg  has to be read 
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_get_global_status(can_emulation_app_hndl * pstrcanemuapphndl) {
	u32 u32offset;
	int s16subrtnretval = -1;

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}

	if (pstrcanemuapphndl->u8prunumber == CSL_PRUCORE_1) {
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_TX_GLOBAL_STATUS_REGISTER);
	}

	else if (pstrcanemuapphndl->u8prunumber == CSL_PRUCORE_0) {
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_RX_GLOBAL_STATUS_REGISTER);
	}

	else {
		return -1;
	}

	s16subrtnretval =
	    pru_can_ram_read_data(u32offset,
			      (u32 *) & pstrcanemuapphndl->u32globalstatus, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	return 0;
}

/** End of pru_can_get_global_status() ***********************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_get_mailbox_status()		Gets the mailbox status register value 
 *
 * This API will be called by the Application to get the mailbox status register value 
 *
 * \param 	pu32globalstatus			Pointer to a var where global status register value has to be written
 * \param 	u8prunumber			PRU number for which GlobalStatusReg  has to be read 
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_get_mailbox_status(can_emulation_app_hndl * pstrcanemuapphndl) {
	u32 u32offset;
	short s16subrtnretval = -1;

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}

	if (pstrcanemuapphndl->u8prunumber == CSL_PRUCORE_1) {
		switch (pstrcanemuapphndl->ecanmailboxnumber) {
		case 0:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX0_STATUS_REGISTER);
			break;
		case 1:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX1_STATUS_REGISTER);
			break;
		case 2:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX2_STATUS_REGISTER);
			break;
		case 3:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX3_STATUS_REGISTER);
			break;
		case 4:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX4_STATUS_REGISTER);
			break;
		case 5:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX5_STATUS_REGISTER);
			break;
		case 6:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX6_STATUS_REGISTER);
			break;
		case 7:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_TX_MAILBOX7_STATUS_REGISTER);
			break;
		default:
			return -1;
		}
	}

	else if (pstrcanemuapphndl->u8prunumber == CSL_PRUCORE_0) {
		switch (pstrcanemuapphndl->ecanmailboxnumber) {
		case 0:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX0_STATUS_REGISTER);
			break;
		case 1:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX1_STATUS_REGISTER);
			break;
		case 2:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX2_STATUS_REGISTER);
			break;
		case 3:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX3_STATUS_REGISTER);
			break;
		case 4:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX4_STATUS_REGISTER);
			break;
		case 5:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX5_STATUS_REGISTER);
			break;
		case 6:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX6_STATUS_REGISTER);
			break;
		case 7:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX7_STATUS_REGISTER);
			break;
		case 8:
			u32offset =
			    (gu32canpruiomapaddress |
			     PRU_CAN_RX_MAILBOX8_STATUS_REGISTER);
			break;
		default:
			return -1;
		}
	}

	else {
		return -1;
	}

	s16subrtnretval =
	    pru_can_ram_read_data(u32offset,
			      (u32 *) & pstrcanemuapphndl->u32mailboxstatus, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	return 0;
}

/** End of pru_can_get_mailbox_status() ***********************************************************************/

short pru_can_transfer_mode_set
    (bool btransfer_flag, can_transfer_direction ecan_trx) {
	u32 u32offset;
	u32 u32value;

	if (ecan_trx == ecantransmit) {
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_RX_GLOBAL_STATUS_REGISTER);
		pru_can_ram_read_data(u32offset, &u32value, 1);
		if (btransfer_flag == true) {
			u32value &= 0x1F;
			u32value |= 0x80;
		} else {
			u32value &= 0x7F;
		}
		pru_can_ram_write_data(u32offset, &u32value, 1);
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_TX_GLOBAL_STATUS_REGISTER);
		pru_can_ram_write_data(u32offset, &u32value, 1);
	} else if (ecan_trx == ecanreceive) {
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_RX_GLOBAL_STATUS_REGISTER);
		pru_can_ram_read_data(u32offset, &u32value, 1);
		if (btransfer_flag == true) {
			u32value &= 0x1F;
			u32value |= 0x40;
		} else {
			u32value &= 0xBF;
		}
		pru_can_ram_write_data(u32offset, &u32value, 1);
		u32offset =
		    (gu32canpruiomapaddress |
		     PRU_CAN_TX_GLOBAL_STATUS_REGISTER);
		pru_can_ram_write_data(u32offset, &u32value, 1);
	} else
		return -1;

	return 0;
}

/** ********************************************************************************************************
 * \brief    pru_can_configuration_mode_set()		Sets the timing value for data transfer 
 *
 * This API will be called by the Application to set timing valus for data transfer 
 *
 * \param 	bconfigmodeenabledisableflag			set the timing parameters
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_configuration_mode_set(bool bconfigmodeenabledisableflag) {

	u32 u32bitrateprescaler;
	u32 u32canbittiming;

	pru_can_ram_read_data((gu32canpruiomapaddress |
			   PRU_CAN_TX_CLOCK_BRP_REGISTER),
			  (u32 *) & u32bitrateprescaler, 1);
	pru_can_ram_read_data((gu32canpruiomapaddress | PRU_CAN_TX_TIMING_REGISTER),
			  (u32 *) & u32canbittiming, 1);

	if (bconfigmodeenabledisableflag == 1) {
		pru_can_calculatetiming(u32canbittiming, u32bitrateprescaler);
	}

	else {
		pru_can_calculatetiming(0, 0);
	}

	return 0;
}

/** End of pru_can_configuration_mode_set() *****************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_emulation_init()		Initializes the Can Emulation Parameters 
 *
 * This API will be called by the Application to Initialize the Can Emulation Parameters 
 *
 * \param 	pstr_pru_iomap		Pointer to structure holding the io map addresses
 * \param 	u32pruclock			PRU Clock value
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_emulation_init(arm_pru_iomap * pstr_pru_iomap, u32 u32pruclock)
{
	u32 u32offset;
	u32 u32value;
	short s16subrtnretval = -1;
	u8 u8loop;

	for (u8loop = 0; u8loop < (u8) ecanmaxinst; u8loop++) {
		gstrcanemulationinst[u8loop].bcaninststate = (bool) 0;
		gstrcanemulationinst[u8loop].ecantransferdirection =
		    (can_transfer_direction) 0;
		gstrcanemulationinst[u8loop].u32apphandlerptr = 0;
	}


	gu32canpruiomapaddress = (u32) pstr_pru_iomap->pru_io_addr;
	gu32canpsc0mapaddress = (u32) pstr_pru_iomap->psc0_io_addr;
	gu32canpsc1mapaddress = (u32) pstr_pru_iomap->psc1_io_addr;
	gu32cansyscfgmapaddress = (u32) pstr_pru_iomap->syscfg_io_addr;
	gu32pruclock = u32pruclock;


	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_GLOBAL_CONTROL_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_GLOBAL_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000040;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_GLOBAL_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000040;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_INTERRUPT_MASK_REGISTER &
				      0xFFFF);
	u32value = 0x00004000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_INTERRUPT_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX0_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX1_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX2_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX3_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX4_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX5_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX6_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_MAILBOX7_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000001;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_ERROR_COUNTER_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_TIMING_REGISTER & 0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_CLOCK_BRP_REGISTER & 0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_TX_ERROR_COUNTER_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	
	u32offset =
	    gu32canpruiomapaddress | (PRU_INTC_POLARITY0 &
				      0xFFFF);
	u32value = 0xFFFFFFFF;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	u32offset =
	    gu32canpruiomapaddress | (PRU_INTC_POLARITY1 &
				      0xFFFF);
	u32value = 0xFFFFFFFF;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	u32offset =
	    gu32canpruiomapaddress | (PRU_INTC_TYPE0 &
				      0xFFFF);
	u32value = 0x1C000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	u32offset =
	    gu32canpruiomapaddress | (PRU_INTC_TYPE1 &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	
	
	u32offset = gu32canpruiomapaddress | (PRU_INTC_HSTINTENIDXCLR & 0xFFFF);
	u32value = 0x0;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}


	u32offset = gu32canpruiomapaddress | (PRU_INTC_GLBLEN & 0xFFFF);
	u32value = 0x1;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	/* tx intr map arm->pru */
	u32offset = gu32canpruiomapaddress | (PRU_INTC_HSTINTENIDXSET & 0xFFFF);
	u32value = 0x0;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_HOSTMAP0 & 0xFFFF);
	u32value = 0x03020100;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_HOSTMAP1 & 0xFFFF);
	u32value = 0x07060504;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_HOSTMAP2 & 0xFFFF);
	u32value = 0x0000908;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_CHANMAP0 & 0xFFFF);
	u32value = 0;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_CHANMAP8 & 0xFFFF);
	u32value = 0x00020200;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXCLR & 0xFFFF);
	u32value = 32;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
#if 1
	u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXCLR & 0xFFFF);
	u32value = 19;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_ENIDXSET & 0xFFFF);
	u32value = 19;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXCLR & 0xFFFF);
	u32value = 18;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_ENIDXSET & 0xFFFF);
	u32value = 18;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

		u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXCLR & 0xFFFF);
	u32value = 34;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset = gu32canpruiomapaddress | (PRU_INTC_ENIDXSET & 0xFFFF);
	u32value = 34;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

#endif

	u32offset = gu32canpruiomapaddress | (PRU_INTC_ENIDXSET & 0xFFFF);
	u32value = 32;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}


	u32offset = gu32canpruiomapaddress | (PRU_INTC_HOSTINTEN & 0xFFFF);
	u32value = 0x5;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

/* PRU0 - Rx Internal Registers Initializations */

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_GLOBAL_CONTROL_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_GLOBAL_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000040;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_INTERRUPT_MASK_REGISTER &
				      0xFFFF);
	u32value = 0x00004000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_INTERRUPT_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX0_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX1_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x0000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX2_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX3_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX4_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX5_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX6_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_MAILBOX7_STATUS_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_ERROR_COUNTER_REGISTER &
				      0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_TIMING_REGISTER & 0xFFFF);
	u32value = 0x0000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}

	u32offset =
	    gu32canpruiomapaddress | (PRU_CAN_RX_CLOCK_BRP_REGISTER & 0xFFFF);
	u32value = 0x00000000;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	return 0;
}

/** End of pru_can_emulation_init() ************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_emulation_open()		Opens the can emulation for application to use
 *
 * This API will be called by the Application to Open the can emulation for application to use
 *
 * \param 	pstrcanemuapphndl		Pointer to application handler structure
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_emulation_open(can_emulation_app_hndl * pstrcanemuapphndl)
{

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}

	if (gstrcanemulationinst[pstrcanemuapphndl->ecaninstance].
	    bcaninststate == 1) {
		return -1;
	}

	gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].bcaninststate
	    = (bool) 1;
	gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].
	    ecantransferdirection =
	    (can_transfer_direction) (u8) pstrcanemuapphndl->
	    ecantransferdirection;
	gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].
	    u32apphandlerptr = (u32) pstrcanemuapphndl;

	return 0;
}

/** End of pru_can_emulation_open() ************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_emulation_close()		Closes the can emulation for other applications to use
 *
 * This API will be called by the Application to Close the can emulation for other applications to use
 *
 * \param 	pstrcanemuapphndl		Pointer to application handler structure
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_emulation_close(can_emulation_app_hndl * pstrcanemuapphndl)
{

	if (pstrcanemuapphndl == NULL) {
		return -1;
	}
	if (gstrcanemulationinst[pstrcanemuapphndl->ecaninstance].
	    bcaninststate == 0) {
		return -1;
	}
	if ((u32) pstrcanemuapphndl !=
	    gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].
	    u32apphandlerptr) {
		return -1;
	}

	gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].bcaninststate
	    = (bool) 0;
	gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].
	    ecantransferdirection = (can_transfer_direction) 0;
	gstrcanemulationinst[(u8) pstrcanemuapphndl->ecaninstance].
	    u32apphandlerptr = 0;

	return 0;
}

/** End of pru_can_emulation_close() ************************************************************************/

/** ********************************************************************************************************
 * \brief    pru_can_emulation_exit()		Diables all the PRUs
 *
 * This API will be called by the Application to disable all PRUs
 *
 * \param 	None
 *
 * \return   SUCCESS or FAILURE
 *
 ***********************************************************************************************************/
short pru_can_emulation_exit(void
    ) {
	short s16subrtnretval;

	s16subrtnretval = pru_can_disable();
	if (s16subrtnretval == -1) {
		return -1;
	}

	return 0;
}

/** End of pru_can_emulation_exit() ************************************************************************/

short pru_can_emulation_sreset(void) {
	return 0;
}

short pru_can_transfer(u8 u8mailboxnumber, u8 u8prunumber) {
	u32 u32offset = 0;
	u32 u32value = 0;
	short s16subrtnretval = -1;

	if (CSL_PRUCORE_1 == u8prunumber) {
		switch (u8mailboxnumber) {
		case 0:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX0_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 1:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX1_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 2:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX2_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 3:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX3_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 4:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX4_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 5:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX5_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 6:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX6_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 7:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_TX_MAILBOX7_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000080;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		default:
			return -1;
		}
	} else {

		u32offset =
		    gu32canpruiomapaddress |
		    (PRU_CAN_RX_INTERRUPT_STATUS_REGISTER & 0xFFFF);
		u32value = 0x00000000;
		s16subrtnretval =
		    pru_can_ram_read_data(u32offset, (u32 *) & u32value, 1);
		if (s16subrtnretval == -1) {
			return -1;
		}
		u32value = u32value & ~(1 << u8mailboxnumber);
		s16subrtnretval =
		    pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
		if (s16subrtnretval == -1) {
			return -1;
		}

		switch (u8mailboxnumber) {
		case 0:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX0_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 1:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX1_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 2:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX2_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 3:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX3_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 4:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX4_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 5:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX5_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 6:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX6_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		case 7:
			u32offset =
			    gu32canpruiomapaddress |
			    (PRU_CAN_RX_MAILBOX7_STATUS_REGISTER & 0xFFFF);
			u32value = 0x00000000;
			s16subrtnretval =
			    pru_can_ram_write_data(u32offset, (u32 *) & u32value,
					       1);
			if (s16subrtnretval == -1) {
				return -1;
			}
			break;
		default:
			return -1;
		}
	}
	return 0;
}

short pru_can_start_or_abort_transmission(bool bcantransmitabortflag) {
	u32 u32offset;
	u32 u32value;
	short s16subrtnretval;
	u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXCLR & 0xFFFF);
	u32value = 32;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);

	u32offset = gu32canpruiomapaddress | (PRU_INTC_ENIDXSET & 0xFFFF);
	u32value = 32;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);

	u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXSET & 0xFFFF);
	u32value = 32;
	s16subrtnretval = pru_can_ram_write_data(u32offset, (u32 *) & u32value, 1);
	if (s16subrtnretval == -1) {
		return -1;
	}
	return 0;
}

short pru_can_check_init_status(void) {
	return 0;
}

short pru_can_mask_ints(u32 int_mask) {
	return 0;
}

int pru_can_get_error_cnt(u8 u8prunumber) {
	return 0;
}

int pru_can_intc_status_get(void)
{
    u32 u32offset = 0;
    u32 u32getvalue = 0;
    u32 u32clrvalue = 0;

	u32offset = gu32canpruiomapaddress | (PRU_INTC_STATCLRINT1 & 0xFFFF);
	pru_ram_read_data_4byte(u32offset, (u32 *)&u32getvalue, 1);

	if (u32getvalue & 4)
		u32clrvalue = 34; //CLR Event 34

	if (u32getvalue & 2)
		u32clrvalue = 33; //CLR Event 33

	if (u32clrvalue){
		u32offset = gu32canpruiomapaddress | (PRU_INTC_STATIDXCLR & 0xFFFF);
    	pru_ram_write_data_4byte(u32offset, &u32clrvalue, 1);
	} else
		return -1;

	return u32getvalue;
}
