/*
 * pru/hal/uart/src/suart_utils.c
 *
 * Copyright (C) 2010 Texas Instruments Incorporated
 * Author: Jitendra Kumar <jitendra@mistralsolutions.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as  published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any kind,
 * whether express or implied; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

/*
 *====================
 * Includes
 *====================
 */

#include "suart_pru_regs.h"
#include "omapl_suart_board.h"
#include "suart_api.h"
#include "suart_utils.h"
#include "suart_err.h"
#include "pru.h"

#include "csl/soc_OMAPL138.h"
#include "csl/cslr_dspintc.h"
#include "csl/cslr_mcasp.h"
#include "csl/cslr_psc_OMAPL138.h"

#define SUART_TRX_DIV_CONF_SZ	4

static short suart_mcasp_tx_baud_set(unsigned int txBaudValue,
			      arm_pru_iomap * pru_arm_iomap);
static short suart_mcasp_rx_baud_set(unsigned int rxBaudValue,
			      unsigned int oversampling,
			      arm_pru_iomap * pru_arm_iomap);

/* 
 * Lookup table for TX baud rate 
 * The divisor value is calculated using the formula
 *	
 *	ACLKX = (AUXCLK)/(CLKXDIV * HCLKXDIV)
 *
 * Where
 *		CLKXDIV takes values from 1-32
 *		HCLKXDIV takes values from 1-4096
 * Here
 *		AUXCLK = 24MHz
 */

unsigned int lt_tx_baud_rate[][SUART_TRX_DIV_CONF_SZ] = {
	/*BaudRate,     Divisor, CLKXDIV, HCLKXDIV */
	{300, 80000, 24, 3200},
	{600, 40000, 15, 2500},
	{1800, 13333, 10, 1212},
	{2400, 10000, 4, 2000},
	{4800, 5000, 1, 2500},
	{7200, 3333, 0, 3333},
	{9600, 2500, 0, 2500},
	{14400, 1666, 0, 1666},
	{19200, 1250, 0, 1250},
	{38400, 625, 0, 625},
	{57600, 416, 0, 416},
	{115200, 208, 0, 208},
	{230400, 104, 0, 104}
};

/* 
 * Lookup table for RX baud rate for 8 bit oversampling 
 * The divisor value is calculated using the formula
 *	
 *	ACLKR = (AUXCLK)/(CLKRDIV * HCLKRDIV) * Oversampling
 *
 * Where
 *		CLKRDIV takes values from 1-32
 *		HCLKRDIV takes values from 1-4096
 * Here
 *		AUXCLK = 24MHz
 */
unsigned int lt_rx_8x_baud_rate[][SUART_TRX_DIV_CONF_SZ] = {
/*BaudRate, Divisor, CLKXDIV, HCLKXDIV */
	{300, 10000, 4, 2000},
	{600, 5000, 1, 2500},
	{1800, 1667, 0, 1667},
	{2400, 1250, 0, 1250},
	{7200, 417, 0, 417},
	{4800, 625, 0, 625},
	{9600, 312, 0, 312},
	{14400, 208, 0, 208},
	{19200, 156, 0, 156},
	{38400, 78, 0, 78},
	{57600, 52, 0, 52},
	{115200, 26, 0, 26},
	{230400, 13, 0, 13}
};

/* 
 * Lookup table for RX baud rate for 16 bit oversampling 
 * The divisor value is calculated using the formula
 *	
 *	ACLKR = (AUXCLK)/(CLKRDIV * HCLKRDIV) * Oversampling
 *
 * Where
 *		CLKRDIV takes values from 1-32
 *		HCLKRDIV takes values from 1-4096
 * Here
 *		AUXCLK = 24MHz
 */
unsigned int lt_rx_16x_baud_rate[][SUART_TRX_DIV_CONF_SZ] = {
/*BaudRate, Divisor, CLKXDIV, HCLKXDIV */
	{300, 5000, 1, 2500},
	{600, 2500, 0, 2500},
	{1800, 833, 0, 833},
	{2400, 625, 0, 625},
	{4800, 312, 0, 312},
	{7200, 208, 0, 208},
	{9600, 156, 0, 156},
	{14400, 104, 0, 104},
	{19200, 78, 0, 78},
	{38400, 39, 0, 39},
	{57600, 26, 0, 26},
	{115200, 13, 0, 13},
	{230400, 6, 0, 6}
};

/*
 *====================
 * API implementations
 *====================
 */
/*
 * McASP configuration routine 
 */

void suart_mcasp_reset (arm_pru_iomap * pru_arm_iomap)
{

        CSL_McaspRegsOvly mcasp0Regs = (CSL_McaspRegsOvly) pru_arm_iomap->mcasp_io_addr;;  //CSL_MCASP_0_CTRL_REGS;

        // reset mcasp.
        mcasp0Regs->GBLCTL = 0;
        mcasp0Regs->RGBLCTL = 0;
        mcasp0Regs->XGBLCTL = 0;
        mcasp0Regs->XSTAT = 0x0000FFFF; // Clear all
        mcasp0Regs->RSTAT = 0x0000FFFF; // Clear all

}

void suart_mcasp_config(unsigned int mcasp_addr,
			unsigned int txBaudValue,
			unsigned int rxBaudValue,
			unsigned int oversampling,
			arm_pru_iomap * pru_arm_iomap)
{
	CSL_McaspRegsOvly mcasp0Regs = (CSL_McaspRegsOvly) mcasp_addr;	//CSL_MCASP_0_CTRL_REGS;

	// reset mcasp.
	mcasp0Regs->GBLCTL = 0;
	mcasp0Regs->RGBLCTL = 0;
	mcasp0Regs->XGBLCTL = 0;
	
	// configure receive registers.
	if ((SUART_8X_OVRSMPL == oversampling) || (0 == oversampling)){
		mcasp0Regs->RMASK = 0x000000FF; 
		mcasp0Regs->RFMT = 0x0000A038;	//slot size 8 bits  , RPAD = 1.
	}
	if(SUART_16X_OVRSMPL == oversampling){
		mcasp0Regs->RMASK = 0x0000FFFF;
		mcasp0Regs->RFMT = 0x0000A078;
	}
		
	mcasp0Regs->AFSRCTL = 0x00000002;	//burst mode
	mcasp0Regs->ACLKRCTL = 0x000000A0;
	mcasp0Regs->AHCLKRCTL = 0x00008000;
	suart_mcasp_rx_baud_set(rxBaudValue, oversampling, pru_arm_iomap);

	mcasp0Regs->RTDM = 0x00000001;
	mcasp0Regs->RINTCTL = 0x00000002;
	mcasp0Regs->RCLKCHK = 0x00FF0008;

	// configure transmit registers.
	mcasp0Regs->XMASK = 0x0000FFFF;
	mcasp0Regs->XFMT = 0x00002078;
	mcasp0Regs->AFSXCTL = 0x0000002;  // Burst mode
	mcasp0Regs->ACLKXCTL = 0x000000E0;
	mcasp0Regs->AHCLKXCTL = 0x00008000;

	suart_mcasp_tx_baud_set(txBaudValue, pru_arm_iomap);

	mcasp0Regs->XTDM = 0x00000001;
	mcasp0Regs->XINTCTL = 0x00000002;
	mcasp0Regs->XCLKCHK = 0x00FF0008;

	//Serializer as a transmitter
	mcasp0Regs->SRCTL0 = 0x000c;
	mcasp0Regs->SRCTL1 = 0x000c;
	mcasp0Regs->SRCTL2 = 0x000c;
	mcasp0Regs->SRCTL3 = 0x000c;
	mcasp0Regs->SRCTL4 = 0x000c;
	mcasp0Regs->SRCTL5 = 0x000c;
	mcasp0Regs->SRCTL6 = 0x000c;
	mcasp0Regs->SRCTL7 = 0x000c;
	mcasp0Regs->SRCTL8 = 0x000c;
	mcasp0Regs->SRCTL9 = 0x000c;
	mcasp0Regs->SRCTL10 = 0x000c;
	mcasp0Regs->SRCTL11 = 0x000c;
	mcasp0Regs->SRCTL12 = 0x000c;
	mcasp0Regs->SRCTL13 = 0x000c;
	mcasp0Regs->SRCTL14 = 0x000c;
	mcasp0Regs->SRCTL15 = 0x000c;

	//Configure all AXR[n] as McASP pins 
	
	/*
 	 *  Setting  all TX MCASP AXR[n] Pin mapped to Even Serializer number (0,2,4,6,8,10,12,14) 
	 *  to GPIO Mode by default. During setting the serializer to TX mode in PRU assembly code, the 
	 *  MCASP AXR[n] Pin would get configured to MCASP mode of operation, before Actual Data Transfer. 
	 */
		
    	//Setting  all TX Pin to GPIO Mode by default
	mcasp0Regs->PFUNC = (CSL_MCASP_PFUNC_RESETVAL) |
			(1 << PRU_SUART1_CONFIG_TX_SER)|(1 << PRU_SUART2_CONFIG_TX_SER)|
                        (1 << PRU_SUART3_CONFIG_TX_SER)|(1 << PRU_SUART4_CONFIG_TX_SER)|
                        (1 << PRU_SUART5_CONFIG_TX_SER)|(1 << PRU_SUART6_CONFIG_TX_SER)|
                        (1 << PRU_SUART7_CONFIG_TX_SER)|(1 << PRU_SUART8_CONFIG_TX_SER);

	mcasp0Regs->PDOUT = 0xFFFF;

	// config pin function and direction.
	mcasp0Regs->PDIR = 0x00000000;
        mcasp0Regs->PDIR = (1 << PRU_SUART1_CONFIG_TX_SER)|(1 << PRU_SUART2_CONFIG_TX_SER)|
			   (1 << PRU_SUART3_CONFIG_TX_SER)|(1 << PRU_SUART4_CONFIG_TX_SER)|
			   (1 << PRU_SUART5_CONFIG_TX_SER)|(1 << PRU_SUART6_CONFIG_TX_SER)|
			   (1 << PRU_SUART7_CONFIG_TX_SER)|(1 << PRU_SUART8_CONFIG_TX_SER)|
			   (MCASP_PDIR_VAL);

	mcasp0Regs->PDOUT = 0xFFFF;

	mcasp0Regs->DITCTL = 0x00000000;
	mcasp0Regs->DLBCTL = 0x00000000;
	mcasp0Regs->AMUTE = 0x00000000;

	mcasp0Regs->XSTAT = 0x0000FFFF;	// Clear all
	mcasp0Regs->RSTAT = 0x0000FFFF;	// Clear all
}

void suart_mcasp_tx_serialzier_set(unsigned int serializerNum,
			      arm_pru_iomap * pru_arm_iomap)
{
	CSL_McaspRegsOvly mcasp0Regs =
	    (CSL_McaspRegsOvly) pru_arm_iomap->mcasp_io_addr;
	
	mcasp0Regs->PFUNC |= (0x1 << serializerNum);
}

/*
 * mcasp TX buard rate setting routine 
 */
short suart_mcasp_tx_baud_set(unsigned int txBaudValue,
			      arm_pru_iomap * pru_arm_iomap)
{
	unsigned int clkDivVal;
	unsigned int loopCnt;
	short status = SUART_SUCCESS;
	short foundVal = SUART_FALSE;

	CSL_McaspRegsOvly mcasp0Regs =
	    (CSL_McaspRegsOvly) pru_arm_iomap->mcasp_io_addr;

	/* Search the supported baud rate in the table */
	for (loopCnt = 0; loopCnt < SUART_NUM_OF_BAUDS_SUPPORTED; loopCnt++) {
		if (txBaudValue == lt_tx_baud_rate[loopCnt][0]) {
			foundVal = SUART_TRUE;
			break;
		}
	}
	if (foundVal == SUART_TRUE) {
		clkDivVal = lt_tx_baud_rate[loopCnt][2];

		mcasp0Regs->ACLKXCTL |=
		    clkDivVal << CSL_MCASP_ACLKXCTL_CLKXDIV_SHIFT;
		clkDivVal = lt_tx_baud_rate[loopCnt][3];	/* starts from 0 */
		mcasp0Regs->AHCLKXCTL |=
		    clkDivVal << CSL_MCASP_AHCLKXCTL_HCLKXDIV_SHIFT;
	} else {
		return SUART_INVALID_TX_BAUD;
	}
	return (status);
}

/*
 * mcasp RX buard rate setting routine 
 */
short suart_mcasp_rx_baud_set(unsigned int rxBaudValue,
			      unsigned int oversampling,
			      arm_pru_iomap * pru_arm_iomap)
{
	unsigned int clkDivVal;
	unsigned int loopCnt;
	short status = SUART_SUCCESS;
	short foundVal = SUART_FALSE;

	CSL_McaspRegsOvly mcasp0Regs =
	    (CSL_McaspRegsOvly) pru_arm_iomap->mcasp_io_addr;

	if (oversampling == SUART_8X_OVRSMPL) {
		for (loopCnt = 0; loopCnt < SUART_NUM_OF_BAUDS_SUPPORTED;
		     loopCnt++) {
			if (rxBaudValue == lt_rx_8x_baud_rate[loopCnt][0]) {
				clkDivVal = lt_rx_8x_baud_rate[loopCnt][2];
				mcasp0Regs->ACLKRCTL |=
				    clkDivVal <<
				    CSL_MCASP_ACLKXCTL_CLKXDIV_SHIFT;
				clkDivVal = lt_rx_8x_baud_rate[loopCnt][3] - 1;	/* starts from 0 */
				mcasp0Regs->AHCLKRCTL |=
				    clkDivVal <<
				    CSL_MCASP_AHCLKXCTL_HCLKXDIV_SHIFT;
				foundVal = SUART_TRUE;
				break;
			}
		}
	} else if (oversampling == SUART_16X_OVRSMPL) {
		for (loopCnt = 0; loopCnt < SUART_NUM_OF_BAUDS_SUPPORTED;
		     loopCnt++) {
			if (rxBaudValue == lt_rx_16x_baud_rate[loopCnt][0]) {
				clkDivVal = lt_rx_16x_baud_rate[loopCnt][2];
				mcasp0Regs->ACLKRCTL |=
				    clkDivVal <<
				    CSL_MCASP_ACLKXCTL_CLKXDIV_SHIFT;
				clkDivVal = lt_rx_16x_baud_rate[loopCnt][3] - 1; /* starts from 0 */
				mcasp0Regs->AHCLKRCTL |=
				    clkDivVal <<
				    CSL_MCASP_AHCLKXCTL_HCLKXDIV_SHIFT;
				foundVal = SUART_TRUE;
				break;
			}
		}
	} else if (oversampling == 0) {
		for (loopCnt = 0; loopCnt < SUART_NUM_OF_BAUDS_SUPPORTED;
		     loopCnt++) {
			if (rxBaudValue == lt_tx_baud_rate[loopCnt][0]) {
				clkDivVal = lt_tx_baud_rate[loopCnt][2];
				mcasp0Regs->ACLKRCTL |=
				    clkDivVal <<
				    CSL_MCASP_ACLKXCTL_CLKXDIV_SHIFT;
				clkDivVal = lt_tx_baud_rate[loopCnt][3];
				mcasp0Regs->AHCLKRCTL |=
				    clkDivVal <<
				    CSL_MCASP_AHCLKXCTL_HCLKXDIV_SHIFT;
				foundVal = SUART_TRUE;
				break;
			}
		}
	} else {
		return SUART_INVALID_OVERSAMPLING;
	}

	if (foundVal != SUART_TRUE) {
		return SUART_INVALID_RX_BAUD;
	}

	return (status);

}

/*
 * mcasp buard rate setting routine 
 */
short suart_asp_baud_set(unsigned int txBaudValue,
			 unsigned int rxBaudValue,
			 unsigned int oversampling,
			 arm_pru_iomap * pru_arm_iomap)
{
	short status = SUART_SUCCESS;

	status = suart_mcasp_tx_baud_set(txBaudValue, pru_arm_iomap);
	status =
	    suart_mcasp_rx_baud_set(rxBaudValue, oversampling, pru_arm_iomap);

	return (status);

}

/*
 * mcasp deactivate the selected serializer 
 */
short suart_asp_serializer_deactivate (unsigned short u16srNum,
			 arm_pru_iomap * pru_arm_iomap)
{
	short status = SUART_SUCCESS;
	CSL_McaspRegsOvly mcasp0Regs =
	    (CSL_McaspRegsOvly) pru_arm_iomap->mcasp_io_addr;
	unsigned int * pu32SrCtlAddr = NULL;
		
	if (u16srNum > 15)
	{
		status = SUART_INVALID_SR_NUM ;
	}
	else
	{
		pu32SrCtlAddr = (unsigned int *)&(mcasp0Regs->SRCTL0);
		pu32SrCtlAddr += u16srNum;
		* (pu32SrCtlAddr) = 0x000C;
	}

	return (status);

}

#if !(defined CONFIG_OMAPL_SUART_MCASP) || (CONFIG_OMAPL_SUART_MCASP == 0)
#define MCASP_PSC_OFFSET	(CSL_PSC_MCASP0)
#elif (CONFIG_OMAPL_SUART_MCASP == 1)
#define MCASP_PSC_OFFSET	(CSL_PSC_MCASP0 + 1)
#elif (CONFIG_OMAPL_SUART_MCASP == 2)
#define MCASP_PSC_OFFSET	(CSL_PSC_MCASP0 + 2)
#endif

/*
 * mcasp psc enable routine 
 */
void suart_mcasp_psc_enable(unsigned int psc1_addr)
{
	CSL_PscRegsOvly PSC = (CSL_PscRegsOvly) psc1_addr;	// CSL_PSC_1_REGS;

	// Wait for any outstanding transition to complete
	while (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
	       CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) ;

	// If we are already in that state, just return
	if (CSL_FEXT(PSC->MDSTAT[MCASP_PSC_OFFSET], PSC_MDSTAT_STATE) ==
	    CSL_PSC_MDSTAT_STATE_ENABLE)
		return;

	// Perform transition
	CSL_FINST(PSC->MDCTL[MCASP_PSC_OFFSET], PSC_MDCTL_NEXT, ENABLE);
	CSL_FINST(PSC->PTCMD, PSC_PTCMD_GO0, SET);

	// Wait for transition to complete
	while (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
	       CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) ;

	// Wait and verify the state
	while (CSL_FEXT(PSC->MDSTAT[MCASP_PSC_OFFSET], PSC_MDSTAT_STATE) !=
	       CSL_PSC_MDSTAT_STATE_ENABLE) ;
}

/*
 * mcasp psc disable routine 
 */
void suart_mcasp_psc_disable(unsigned int psc1_addr)
{
	CSL_PscRegsOvly PSC = (CSL_PscRegsOvly) psc1_addr;	// CSL_PSC_1_REGS;

	// Wait for any outstanding transition to complete
	while (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
	       CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) ;

	// If we are already in that state, just return
	if (CSL_FEXT(PSC->MDSTAT[MCASP_PSC_OFFSET], PSC_MDSTAT_STATE) ==
	    CSL_PSC_MDSTAT_STATE_SYNCRST)
		return;

	// Perform transition
	CSL_FINST(PSC->MDCTL[MCASP_PSC_OFFSET], PSC_MDCTL_NEXT, SYNCRST);
	CSL_FINST(PSC->PTCMD, PSC_PTCMD_GO0, SET);

	// Wait for transition to complete
	while (CSL_FEXT(PSC->PTSTAT, PSC_PTSTAT_GOSTAT0) ==
	       CSL_PSC_PTSTAT_GOSTAT0_IN_TRANSITION) ;

	// Wait and verify the state
	while (CSL_FEXT(PSC->MDSTAT[MCASP_PSC_OFFSET], PSC_MDSTAT_STATE) !=
	       CSL_PSC_MDSTAT_STATE_SYNCRST) ;
}
/* End of file */
