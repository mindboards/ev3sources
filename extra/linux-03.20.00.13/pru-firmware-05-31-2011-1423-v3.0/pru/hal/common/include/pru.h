/* --------------------------------------------------------------------------
  FILE        : pru.h                                                   
  PROJECT     : DA8xx/OMAP-L138/C674x PRU Development
  DESC        : PRU Load and Run API Definitions
----------------------------------------------------------------------------- */

#ifndef _PRU_H_
#define _PRU_H_

#include "tistdtypes.h"
#include "csl/cslr_prucore.h"

// Prevent C++ name mangling
#ifdef __cplusplus
extern far "c" {
#endif

#define PRU_NUM0		(0)
#define PRU_NUM1		(1)

/***********************************************************
* Global Macro Declarations                                *
***********************************************************/
// PRU Memory Macros
#define PRU0_DATA_RAM_START  (0x01C30000)
#define PRU0_PROG_RAM_START  (0x01C38000)

#define PRU1_DATA_RAM_START  (0x01C32000)
#define PRU1_PROG_RAM_START  (0x01C3C000)

#define PRU_DATA_RAM_SIZE    (0x200)
#define PRU_PROG_RAM_SIZE    (0x1000)

#define PRU_PRU0_BASE_ADDRESS					0
#define PRU_INTC_BASE_ADDRESS					(PRU_PRU0_BASE_ADDRESS + 0x4000)

#define PRU_INTC_REVID							(PRU_INTC_BASE_ADDRESS + 0)
#define PRU_INTC_CONTROL						(PRU_INTC_BASE_ADDRESS + 0x4)
#define PRU_INTC_GLBLEN							(PRU_INTC_BASE_ADDRESS + 0x10)
#define PRU_INTC_GLBLNSTLVL						(PRU_INTC_BASE_ADDRESS + 0x1C)
#define PRU_INTC_STATIDXSET						(PRU_INTC_BASE_ADDRESS + 0x20)
#define PRU_INTC_STATIDXCLR						(PRU_INTC_BASE_ADDRESS + 0x24)
#define PRU_INTC_ENIDXSET						(PRU_INTC_BASE_ADDRESS + 0x28)
#define PRU_INTC_ENIDXCLR						(PRU_INTC_BASE_ADDRESS + 0x2C)
#define PRU_INTC_HSTINTENIDXSET					(PRU_INTC_BASE_ADDRESS + 0x34)
#define PRU_INTC_HSTINTENIDXCLR					(PRU_INTC_BASE_ADDRESS + 0x38)
#define PRU_INTC_GLBLPRIIDX						(PRU_INTC_BASE_ADDRESS + 0x80)
#define PRU_INTC_STATSETINT0					(PRU_INTC_BASE_ADDRESS + 0x200)
#define PRU_INTC_STATSETINT1					(PRU_INTC_BASE_ADDRESS + 0x204)
#define PRU_INTC_STATCLRINT0					(PRU_INTC_BASE_ADDRESS + 0x280)
#define PRU_INTC_STATCLRINT1					(PRU_INTC_BASE_ADDRESS + 0x284)
#define PRU_INTC_ENABLESET0						(PRU_INTC_BASE_ADDRESS + 0x300)
#define PRU_INTC_ENABLESET1						(PRU_INTC_BASE_ADDRESS + 0x304)
#define PRU_INTC_ENABLECLR0						(PRU_INTC_BASE_ADDRESS + 0x380)
#define PRU_INTC_ENABLECLR1						(PRU_INTC_BASE_ADDRESS + 0x384)
#define PRU_INTC_CHANMAP0						(PRU_INTC_BASE_ADDRESS + 0x400)
#define PRU_INTC_CHANMAP1						(PRU_INTC_BASE_ADDRESS + 0x404)
#define PRU_INTC_CHANMAP2						(PRU_INTC_BASE_ADDRESS + 0x408)
#define PRU_INTC_CHANMAP3						(PRU_INTC_BASE_ADDRESS + 0x40C)
#define PRU_INTC_CHANMAP4						(PRU_INTC_BASE_ADDRESS + 0x410)
#define PRU_INTC_CHANMAP5						(PRU_INTC_BASE_ADDRESS + 0x414)
#define PRU_INTC_CHANMAP6						(PRU_INTC_BASE_ADDRESS + 0x418)
#define PRU_INTC_CHANMAP7						(PRU_INTC_BASE_ADDRESS + 0x41C)
#define PRU_INTC_CHANMAP8						(PRU_INTC_BASE_ADDRESS + 0x420)
#define PRU_INTC_CHANMAP9						(PRU_INTC_BASE_ADDRESS + 0x424)
#define PRU_INTC_CHANMAP10						(PRU_INTC_BASE_ADDRESS + 0x428)
#define PRU_INTC_CHANMAP11						(PRU_INTC_BASE_ADDRESS + 0x42C)
#define PRU_INTC_CHANMAP12						(PRU_INTC_BASE_ADDRESS + 0x430)
#define PRU_INTC_CHANMAP13						(PRU_INTC_BASE_ADDRESS + 0x434)
#define PRU_INTC_CHANMAP14						(PRU_INTC_BASE_ADDRESS + 0x438)
#define PRU_INTC_CHANMAP15						(PRU_INTC_BASE_ADDRESS + 0x43C)
#define PRU_INTC_HOSTMAP0						(PRU_INTC_BASE_ADDRESS + 0x800)
#define PRU_INTC_HOSTMAP1						(PRU_INTC_BASE_ADDRESS + 0x804)
#define PRU_INTC_HOSTMAP2						(PRU_INTC_BASE_ADDRESS + 0x808)
#define PRU_INTC_HOSTINTPRIIDX0					(PRU_INTC_BASE_ADDRESS + 0x900)
#define PRU_INTC_HOSTINTPRIIDX1					(PRU_INTC_BASE_ADDRESS + 0x904)
#define PRU_INTC_HOSTINTPRIIDX2					(PRU_INTC_BASE_ADDRESS + 0x908)
#define PRU_INTC_HOSTINTPRIIDX3					(PRU_INTC_BASE_ADDRESS + 0x90C)
#define PRU_INTC_HOSTINTPRIIDX4					(PRU_INTC_BASE_ADDRESS + 0x910)
#define PRU_INTC_HOSTINTPRIIDX5					(PRU_INTC_BASE_ADDRESS + 0x914)
#define PRU_INTC_HOSTINTPRIIDX6					(PRU_INTC_BASE_ADDRESS + 0x918)
#define PRU_INTC_HOSTINTPRIIDX7					(PRU_INTC_BASE_ADDRESS + 0x91C)
#define PRU_INTC_HOSTINTPRIIDX8					(PRU_INTC_BASE_ADDRESS + 0x920)
#define PRU_INTC_HOSTINTPRIIDX9					(PRU_INTC_BASE_ADDRESS + 0x924)
#define PRU_INTC_POLARITY0						(PRU_INTC_BASE_ADDRESS + 0xD00)
#define PRU_INTC_POLARITY1						(PRU_INTC_BASE_ADDRESS + 0xD04)
#define PRU_INTC_TYPE0							(PRU_INTC_BASE_ADDRESS + 0xD80)
#define PRU_INTC_TYPE1							(PRU_INTC_BASE_ADDRESS + 0xD84)
#define PRU_INTC_HOSTINTNSTLVL0					(PRU_INTC_BASE_ADDRESS + 0x1100)
#define PRU_INTC_HOSTINTNSTLVL1					(PRU_INTC_BASE_ADDRESS + 0x1104)
#define PRU_INTC_HOSTINTNSTLVL2					(PRU_INTC_BASE_ADDRESS + 0x1108)
#define PRU_INTC_HOSTINTNSTLVL3					(PRU_INTC_BASE_ADDRESS + 0x110C)
#define PRU_INTC_HOSTINTNSTLVL4					(PRU_INTC_BASE_ADDRESS + 0x1110)
#define PRU_INTC_HOSTINTNSTLVL5					(PRU_INTC_BASE_ADDRESS + 0x1114)
#define PRU_INTC_HOSTINTNSTLVL6					(PRU_INTC_BASE_ADDRESS + 0x1118)
#define PRU_INTC_HOSTINTNSTLVL7					(PRU_INTC_BASE_ADDRESS + 0x111C)
#define PRU_INTC_HOSTINTNSTLVL8					(PRU_INTC_BASE_ADDRESS + 0x1120)
#define PRU_INTC_HOSTINTNSTLVL9					(PRU_INTC_BASE_ADDRESS + 0x1124)
#define PRU_INTC_HOSTINTEN						(PRU_INTC_BASE_ADDRESS + 0x1500)

/* Macros defining some PRU specific constants. */
#define PRU_INTC_HOSTINTLVL_MAX					9

/*
 *====================
 * Typedef structures
 *====================
 */

typedef struct arm_pru_iomap {
	void *pru_io_addr;
	void *mcasp_io_addr;
	void *psc0_io_addr;
	void *psc1_io_addr;
	void *syscfg_io_addr;
	void *pFifoBufferPhysBase;
	void *pFifoBufferVirtBase;
	unsigned int   pru_clk_freq;
} arm_pru_iomap;

/***********************************************************
* Global Function Declarations                             *
***********************************************************/

extern __FAR__ Uint32 pru_enable(Uint8 pruNum,
				 arm_pru_iomap * pru_arm_iomap);
extern __FAR__ Uint32 pru_load(Uint8 pruNum, Uint32 * pruCode,
				   Uint32 codeSizeInWords,
				   arm_pru_iomap * pru_arm_iomap);
extern __FAR__ Uint32 pru_run(Uint8 pruNum,
				  arm_pru_iomap * pru_arm_iomap);
extern __FAR__ Uint32 pru_waitForHalt(Uint8 pruNum, Int32 timeout,
					  arm_pru_iomap * pru_arm_iomap);
extern __FAR__ Uint32 pru_disable(arm_pru_iomap * pru_arm_iomap);

short pru_ram_write_data(Uint32 u32offset, Uint8 * pu8datatowrite,
			 Uint16 u16wordstowrite,
			 arm_pru_iomap * pru_arm_iomap);
short pru_ram_read_data(Uint32 u32offset, Uint8 * pu8datatoread,
			Uint16 u16wordstoread,
			arm_pru_iomap * pru_arm_iomap);
short pru_ram_read_data_4byte(unsigned int u32offset,
				  unsigned int *pu32datatoread,
				  short u16wordstoread);
short pru_ram_write_data_4byte(unsigned int u32offset,
				   unsigned int *pu32datatoread,
				   short u16wordstoread);

/***********************************************************
* End file                                                 *
***********************************************************/

#ifdef __cplusplus
}
#endif
#endif				// End _PRU_H_
