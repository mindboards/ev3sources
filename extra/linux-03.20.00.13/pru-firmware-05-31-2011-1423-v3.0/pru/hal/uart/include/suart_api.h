/*
 * linux/<file location within the kernel tree>
 *
 * Copyright (C) 2010 Texas Instruments Incorporated
 * Author: Ganeshan N
 *
 * Based on <Give reference of old kernel file from which this file is derived from>
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

#ifndef _SUART_API_H_
#define _SUART_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *====================
 * Includes
 *====================
 */
#include "tistdtypes.h"
#include "pru.h"
/*
 *====================
 * Defines
 *====================
 */
	/* Retrun Values */
#ifndef BIT
#define BIT(x)				(1 << x)
#endif

#define	 SINGLE_PRU		0
#define  BOTH_PRU		1
#define  PRU_ACTIVE		SINGLE_PRU //BOTH_PRU

#define SUART_NUM_OF_CHANNELS_PER_SUART		2
#define SUART_NUM_OF_BYTES_PER_CHANNEL		16

#define SUART_PASS          	0
#define SUART_SUCCESS       	0
#define SUART_FAIL          	1

#define SUART_FALSE         	0
#define SUART_TRUE          	1

#define PRU_TX_INTR 	    	1
#define PRU_RX_INTR 	    	2

#define CHN_TXRX_STATUS_TIMEOUT BIT(6)
#define CHN_TXRX_STATUS_BI      BIT(5)
#define CHN_TXRX_STATUS_FE      BIT(4)
#define CHN_TXRX_STATUS_UNERR   BIT(3)
#define CHN_TXRX_STATUS_OVRNERR BIT(3)	/* UNERR->TX & OVRNERR->RX */
#define CHN_TXRX_STATUS_ERR     BIT(2)
#define CHN_TXRX_STATUS_CMPLT   BIT(1)
#define CHN_TXRX_STATUS_RDY     BIT(0)

#define CHN_RX_IE_MASK_OVRN	 BIT(15)
#define CHN_TXRX_IE_MASK_TIMEOUT BIT(14)
#define CHN_TXRX_IE_MASK_BI      BIT(13)
#define CHN_TXRX_IE_MASK_FE      BIT(12)
#define CHN_TXRX_IE_MASK_CMPLT   BIT(1)

#define SUART_GBL_INTR_ERR_MASK 	BIT(9)
#define SUART_PRU_ID_MASK        0xFF

#define SUART_FIFO_LEN           15

#define SUART_8X_OVRSMPL 	 1
#define SUART_16X_OVRSMPL	 2
#define SUART_DEFAULT_OVRSMPL   SUART_8X_OVRSMPL

#if (SUART_DEFAULT_OVRSMPL == SUART_16X_OVRSMPL)
#define SUART_DEFAULT_BAUD      57600
#else
#define SUART_DEFAULT_BAUD      115200
#endif

#define PRU_MODE_INVALID    	0x00
#define PRU_MODE_TX_ONLY     	0x1
#define PRU_MODE_RX_ONLY     	0x2
#define PRU_MODE_RX_TX_BOTH  	0x3

#if (PRU_ACTIVE == BOTH_PRU)
#define PRU0_MODE    PRU_MODE_RX_ONLY 
#define PRU1_MODE    PRU_MODE_TX_ONLY 
#elif (PRU_ACTIVE ==  SINGLE_PRU)
#define PRU0_MODE    PRU_MODE_RX_TX_BOTH
#define PRU1_MODE    PRU_MODE_INVALID
#else
#define PRU0_MODE    PRU_MODE_INVALID
#define PRU1_MODE    PRU_MODE_INVALID
#endif 

#if !(defined CONFIG_OMAPL_SUART_MCASP) || (CONFIG_OMAPL_SUART_MCASP == 0)
#define MCASP_BASE_OFFSET		(0x0)
#elif (CONFIG_OMAPL_SUART_MCASP == 1)
#define MCASP_BASE_OFFSET		(0x4000)
#elif (CONFIG_OMAPL_SUART_MCASP == 2)
#define MCASP_BASE_OFFSET		(0x8000)
#endif

#define MCASP_XBUF_BASE_ADDR            (0x01d00200 + MCASP_BASE_OFFSET)
#define MCASP_RBUF_BASE_ADDR		(0x01d00280 + MCASP_BASE_OFFSET)
#define MCASP_SRCTL_BASE_ADDR           (0x01d00180 + MCASP_BASE_OFFSET)

#define MCASP_SRCTL_TX_MODE		(0x000D)
#define MCASP_SRCTL_RX_MODE		(0x000E)

/* Since only PRU0 can work as RX */
#define RX_DEFAULT_DATA_DUMP_ADDR	(0x00001FC)
#define PRU_NUM_OF_CHANNELS		(16)

#define PRU_SUART_UART1                 (1u)
/** UART instance */
#define PRU_SUART_UART2                 (2u)
/** UART instance */
#define PRU_SUART_UART3                 (3u)
/** UART instance */

#define PRU_SUART_UART4                 (4u)
/** UART instance */
#define PRU_SUART_UART5                 (5u)
/** UART instance */
#define PRU_SUART_UART6                 (6u)
/** UART instance */
#define PRU_SUART_UART7                 (7u)
/** UART instance */
#define PRU_SUART_UART8                 (8u)
/** UART instance */

#define PRU_SUART_UARTx_INVALID         (9u)
/** UART instance */

#define PRU_SUART_HALF_TX		(1u)
#define PRU_SUART_HALF_RX		(2u)
#define PRU_SUART_HALF_TX_DISABLED      (4u)
#define PRU_SUART_HALF_RX_DISABLED	(8u)
/** UART RX/TX Enable Flag */
 
/*
 *====================
 * Enumerations
 *====================
 */
 
/**
 * \brief Chanel direction
 *
 *  This enum is used to specify the direction of the channel in UART
 */
    typedef enum {
    SUART_CHN_TX = 1,   /**< Channel configured for Transmission */
    SUART_CHN_RX = 2   /**< Channel configured for receiving */
    } SUART_CHN_DIR;

/**
 * \brief Chanel State
 *
 *  This enum is used to specify the state of the channel in UART. It 
 *  is either enabled or disabled.
 */
    typedef enum {
    SUART_CHN_DISABLED = 0,   /**< Channel is not enabled */
    SUART_CHN_ENABLED = 1    /**< Channel enabled */
    } SUART_CHN_STATE;

 
/**
 * \brief One line description of the enum
 *
 *  Detailed description of the enum
 */
	typedef enum {
		ePRU_SUART_DATA_BITS6 = 8,
				/**< 6 + 2 */
		ePRU_SUART_DATA_BITS7,
				/**< member2 description */
		ePRU_SUART_DATA_BITS8,
				/**< member1 description */
		ePRU_SUART_DATA_BITS9,
				/**< member2 description */
		ePRU_SUART_DATA_BITS10,
				/**< member1 description */
		ePRU_SUART_DATA_BITS11,
				/**< member1 description */
		ePRU_SUART_DATA_BITS12
				/**< member1 description */
	} SUART_EN_BITSPERCHAR;

/**
 * \brief One line description of the enum
 *
 *  Detailed description of the enum
 */
	typedef enum {
		ePRU_SUART_NUM_1 = 1,
			       /**< member1 description */
		ePRU_SUART_NUM_2,
			       /**< member1 description */
		ePRU_SUART_NUM_3,
			       /**< member1 description */
		ePRU_SUART_NUM_4,
			       /**< member1 description */
		ePRU_SUART_NUM_5,
			       /**< member1 description */
		ePRU_SUART_NUM_6,
			       /**< member1 description */
		ePRU_SUART_NUM_7,
			       /**< member1 description */
		ePRU_SUART_NUM_8
			       /**< member1 description */
	} SUART_EN_UARTNUM;

/**
 * \brief One line description of the enum
 *
 *  Detailed description of the enum
 */
	typedef enum {
		ePRU_SUART_HALF_TX = 1,
		ePRU_SUART_HALF_RX,
		ePRU_SUART_FULL_TX_RX,
		ePRU_SUART_HALF_TX_DISABLED = 4,
		ePRU_SUART_HALF_RX_DISABLED = 8
	} SUART_EN_UARTTYPE;

/**
 * \brief One line description of the enum
 *
 *  Detailed description of the enum
 */
	typedef enum {
		ePRU_SUART_TX_CH0 = 0,
				 /**< member1 description */
		ePRU_SUART_TX_CH1,
				/**< member1 description */
		ePRU_SUART_TX_CH2,
				/**< member1 description */
		ePRU_SUART_TX_CH3,
				/**< member1 description */
		ePRU_SUART_TX_CH4,
				/**< member1 description */
		ePRU_SUART_TX_CH5,
				/**< member1 description */
		ePRU_SUART_TX_CH6,
				/**< member1 description */
		ePRU_SUART_TX_CH7
				/**< member1 description */
	} SUART_EN_TXCHANNEL;

/**
 * \brief One line description of the enum
 *
 *  Detailed description of the enum
 */
	typedef enum {
		ePRU_SUART_RX_CH0 = 0,
				 /**< member1 description */
		ePRU_SUART_RX_CH1,
				/**< member1 description */
		ePRU_SUART_RX_CH2,
				/**< member1 description */
		ePRU_SUART_RX_CH3,
				/**< member1 description */
		ePRU_SUART_RX_CH4,
				/**< member1 description */
		ePRU_SUART_RX_CH5,
				/**< member1 description */
		ePRU_SUART_RX_CH6,
				/**< member1 description */
		ePRU_SUART_RX_CH7
				/**< member1 description */
	} SUART_EN_RXCHANNEL;

/**
 * \brief One line description of the enum
 *
 *  Detailed description of the enum
 */
	typedef enum {
		ePRU_SUART_UART_FREE = 0,
				    /**< member1 description */
		ePRU_SUART_UART_IN_USE
				    /**< member1 description */
	} SUART_EN_UART_STATUS;

/*
 *====================
 * Structures
 *====================
 */

typedef struct 
{
    unsigned int mode:2;
    unsigned int service_req:1;
    unsigned int asp_id:2;
    unsigned int reserved1:3;
    unsigned int serializer_num:4;
    unsigned int reserved2:4;
    unsigned int presacler:10;
    unsigned int over_sampling:2;
    unsigned int framing_mask:1;
    unsigned int break_mask:1;
    unsigned int timeout_mask:1;
    unsigned int reserved3:1;
} pru_suart_chn_cntrl_config1;

typedef struct 
{
    unsigned int bits_per_char:4;
    unsigned int reserved1:4;
    unsigned int data_len:4;
    unsigned int reserved2:4;
    unsigned int txrx_ready:1;
    unsigned int txrx_complete:1;
    unsigned int txrx_error:1;
    unsigned int txrx_underrun:1;
    unsigned int framing_error:1;
    unsigned int break_error:1;
    unsigned int timeout_error:1;
    unsigned int reserved3:8;
    unsigned int chn_state:1;
} pru_suart_chn_config2_status;

typedef struct {
    pru_suart_chn_cntrl_config1 CH_Ctrl_Config1;
			/**< PRU SUART control & configuration1 register */
    pru_suart_chn_config2_status CH_Config2_TXRXStatus;
			/**< PRU SUART configuration2 & TX/RX status register */
    unsigned int CH_TXRXData;
		      /**< PRU SUART TX/RX data register */
    unsigned int Reserved1;
		    /**< Reserved 1  */
} pru_suart_regs;

typedef volatile pru_suart_regs *PRU_SUART_RegsOvly;

typedef struct {
    unsigned int asp_xsrctl_base;
    unsigned int asp_xbuf_base;
    unsigned short buff_addr;
    unsigned char buff_size;
    unsigned char bits_loaded;
} pru_suart_tx_cntx_priv;
typedef pru_suart_tx_cntx_priv* ppru_suart_tx_cntx_priv;

typedef struct {
    unsigned int asp_rbuf_base;
    unsigned int asp_rsrctl_base;
    unsigned int reserved1;
    unsigned int reserved2;
    unsigned int reserved3;
    unsigned int reserved4;
} pru_suart_rx_cntx_priv;
typedef pru_suart_rx_cntx_priv* ppru_suart_rx_cntx_priv;

/**
 * \brief One line description of the structure
 *
 *  Detailed description of the structure
 */
typedef struct {
    unsigned char TXSerializer;
	              /**< It takes the value of the serializer. It takes the value 0-15 range */
    unsigned char RXSerializer;
	             /**< It takes the value of the serializer. It takes the value 0-15 range */
    unsigned short txClkDivisor;	/**< Divisor (CLKXDIV* HCLKXDIV) value to generate the appropriate baud rate */
    unsigned short rxClkDivisor;	/**< Divisor (CLKXDIV* HCLKXDIV) value to generate the appropriate baud rate */
    unsigned char txBitsPerChar;	/**< Bits per Character (Range: 6 to 12) */
    unsigned char rxBitsPerChar;	/**< Bits per Character (Range: 6 to 12) */
    unsigned char Oversampling;	/**< Oversampling rate */
    unsigned char BIIntrMask;	/**< Break Indicator Interrupt Mask Bit */
    unsigned char FEIntrMask;	/**< Framing Error Interrupt Mask Bit */
} suart_config;

/**
 * \brief One line description of the structure
 *
 *  Detailed description of the structure
 */
	typedef struct {
		unsigned short uartNum;/**< UART number (Range 1 to 16) */
		unsigned short uartType;
				       /**< Type of the UART i.e., Full UART, Half UART */
		unsigned short uartTxChannel;
					/**< Soft UART Channel for Transmission */
		unsigned short uartRxChannel;
					/**< Soft UART Channel for Reception */
		unsigned short uartStatus;
					/**< Status of the UART */
	} suart_struct_handle;

	typedef suart_struct_handle *suart_handle;

/*
 *====================
 * API declarations
 *====================
 */

	short pru_softuart_init(unsigned int txBaudValue,
				unsigned int rxBaudValue,
				unsigned int oversampling,
				unsigned char *pru_suart_emu_code,
				unsigned int fw_size,
				arm_pru_iomap * pru_arm_iomap1);
	int pru_intr_set_mask(unsigned short uartNum,
			      unsigned int txrxmode, unsigned int intrmask);
	short pru_softuart_reset(unsigned int txBaudValue,
				 unsigned int rxBaudValue,
				 unsigned int oversampling);

	short pru_softuart_open(suart_handle hSuart);

	short pru_softuart_close(suart_handle hUart);

	short pru_softuart_setbaud
	    (suart_handle hUart,
	     unsigned short txClkDivisor, unsigned short rxClkDivisor);

	short pru_softuart_setdatabits
	    (suart_handle hUart,
	     unsigned short txDataBits, unsigned short rxDataBits);

	short pru_softuart_setconfig
	    (suart_handle hUart, suart_config * configUart);

	short pru_softuart_getconfig
	    (suart_handle hUart, suart_config * configUart);

	int pru_softuart_pending_tx_request (void);

	short pru_softuart_write
	    (suart_handle hUart,
	     unsigned int *ptTxDataBuf, unsigned short dataLen);

	short pru_softuart_read
	    (suart_handle hUart,
	     unsigned int *ptDataBuf, unsigned short dataLen);

	int suart_intr_clrmask(unsigned short uartNum,
			       unsigned int txrxmode, unsigned int intrmask);

	short pru_softuart_clrTxStatus(suart_handle hUart);

	short pru_softuart_getTxStatus(suart_handle hUart);

	short pru_softuart_clrRxStatus(suart_handle hUart);

	short pru_softuart_getRxStatus(suart_handle hUart);

	short pru_softuart_get_isrstatus(unsigned short uartNum,
					 unsigned short *txrxFlag);

	int pru_intr_clr_isrstatus(unsigned short uartNum,  unsigned int txrxmode);

	int suart_intr_getmask(unsigned short uartNum,
			       unsigned int txrxmode, unsigned int intrmask);
	int suart_intr_setmask(unsigned short uartNum,
			       unsigned int txrxmode, unsigned int intrmask);

	short pru_softuart_getTxDataLen(suart_handle hUart);

	short pru_softuart_getRxDataLen(suart_handle hUart);
	short suart_arm_to_pru_intr(unsigned short uartNum);

	short arm_to_pru_intr_init(void);
	
	void pru_mcasp_deinit (void);
	short pru_softuart_deinit(void);
	
	short pru_softuart_clrRxFifo(suart_handle hUart);

	short pru_softuart_read_data (suart_handle hUart, Uint8 * pDataBuffer, 
		                      Int32 s32MaxLen, Uint32 * pu32DataRead);

	short pru_softuart_stopReceive (suart_handle hUart);
	
	int suart_pru_to_host_intr_enable (unsigned short uartNum, 
				           unsigned int txrxmode, int s32Flag);
	void pru_set_fifo_timeout(Uint32 timeout);


#ifdef __cplusplus
}				/* End of extern C */
#endif				/* #ifdef __cplusplus */
#endif
