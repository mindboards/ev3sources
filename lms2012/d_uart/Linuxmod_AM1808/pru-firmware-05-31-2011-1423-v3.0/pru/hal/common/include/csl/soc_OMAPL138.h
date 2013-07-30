/*  ============================================================================
 *  Copyright (c) Texas Instruments Inc 2002, 2003, 2004, 2005, 2006, 2007
 *
 *  Use of this software is controlled by the terms and conditions found in the
 *  license agreement under which this software has been supplied.
 *  ===========================================================================
 */

#ifndef _SOC_H_
#define _SOC_H_

/* =============================================================================
 *  Revision History
 *  ===============
 *  15-Feb-2007 NS  File created.
 *  Apr 09  2009           IMTIAZ SMA     Updated for OMAPL138 SoC
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "csl/cslr.h"

/**************************************************************************\
* SOC file
\**************************************************************************/

/******************************************************************************\
* Static inline definition
\******************************************************************************/
#ifndef CSL_IDEF_INLINE
#define CSL_IDEF_INLINE static inline
#endif

/******************************************************************************\
* Peripheral Instance count
\******************************************************************************/

/** @brief Number of UPP  instances                                           */
#define CSL_UPP_PER_CNT                     1

/** @brief Number of UHPI instances                                           */
#define CSL_HPI_PER_CNT                     1

/** @brief Number of McASP instances                                          */
#define CSL_MCASP_PER_CNT                   1

/** @brief Number of TIMER instances                                          */
#define CSL_TMR_PER_CNT                     4

/** @brief Number of PSC instances                                            */
#define CSL_PSC_PER_CNT                     2

/** @brief Number of UART instances                                           */
#define CSL_UART_PER_CNT                    3

/** @brief Number of SPI instances                                            */
#define CSL_SPI_PER_CNT                     2

/** @brief Number of I2C instances                                            */
#define CSL_I2C_PER_CNT                     2

/** @brief Number of PLL instances                                            */
#define CSL_PLLC_PER_CNT                    2

/** @brief Number of MMCSD instances                                          */
#define CSL_MMCSD_PER_CNT                   2

/** @brief Number of LCDC instances                                           */
#define CSL_LCDC_PER_CNT                    1

/** @brief Number of Mcbsp instances                                          */
#define CSL_MCBSP_PER_CNT                   2

/** @brief Number of EDMA3 CC instances                                       */
#define CSL_EDMA3CC_CNT                     2

/** @brief Number of EDMA3 TC instances                                       */
#define CSL_EDMA3TC_CNT                     3

/** @brief Number of EMIFA instances                                          */
#define CSL_EMIFA_PER_CNT                   1

/** @brief Number of EMIFB instances                                          */
#define CSL_EMIFB_PER_CNT                   1

/** @brief Number of EMAC instances                                           */
#define CSL_EMAC_PER_CNT                    1

/** @brief Number of MDIO instances                                           */
#define CSL_MDIO_PER_CNT                    1

/** @brief Number of EHRPWM instances                                         */
#define CSL_EHRPWM_PER_CNT                  2

/** @brief Number of ECAP instances                                           */
#define CSL_ECAP_PER_CNT                    3

/** @brief Number of CPGMAC instances                                         */
#define CSL_CPGMACSSR_PER_CNT               1

/** @brief Number of CPPI instances                                           */
#define CSL_CPPI_PER_CNT                    1

/** @brief Number of USB instances                                            */
#define CSL_USB_PER_CNT                     2

/** @brief Number of VPIF instances                                           */
#define CSL_VPIF_PER_CNT                    1

/** @brief Number of INTC instances                                           */
#define CSL_INTC_PER_CNT                    1

/** @brief Number of AINTC instances                                          */
#define CSL_AINTC_PER_CNT                   1

/** @brief Number of SATA instances                                           */
#define CSL_SATA_PER_CNT                    1

/** @brief Number of RTC instances                                            */
#define CSL_RTC_PER_CNT                     1

/** @brief Number of GPIO instances                                           */
#define CSL_GPIO_PER_CNT                    1

/** @brief Number of SYSCFG instances                                         */
#define CSL_SYSCFG_PER_CNT                  2
/******************************************************************************\
* Peripheral Instance definitions.
\******************************************************************************/

/** @brief Peripheral Instances of UHPI instances                             */
#define CSL_HPI                             (0)

/** @brief Peripheral Instances of McASP instances                            */
#define CSL_MCASP_0                         (0)

/** @brief Peripheral Instance of EDMA CC instances                           */
#define CSL_EDMA3CC_0                       (0)
#define CSL_EDMA3CC_1                       (1)

/** @brief Peripheral Instance of EDMA TC instances                           */
#define CSL_EDMA3TC_0                       (0)
#define CSL_EDMA3TC_1                       (1)

/** @brief Peripheral Instance of Timer 64 instances                          */
#define CSL_TMR_0                           (0)
#define CSL_TMR_1                           (1)
#define CSL_TMR_2                           (2)
#define CSL_TMR_3                           (3)

/** @brief Peripheral Instances of PSC instances                              */
#define CSL_PSC_0                           (0)
#define CSL_PSC_1                           (1)

/** @brief Peripheral Instances of UART instances                             */
#define CSL_UART_0                          (0)
#define CSL_UART_1                          (1)
#define CSL_UART_2                          (2)

/** @brief Peripheral Instances of SPI instances                              */
#define CSL_SPI_0                           (0)
#define CSL_SPI_1                           (1)

/** @brief Peripheral Instances of I2C instances                              */
#define CSL_I2C_0                           (0)
#define CSL_I2C_1                           (1)

/** @brief Peripheral Instances of MMCSD instances                            */
#define CSL_MMCSD_0                         (0)
#define CSL_MMCSD_1                         (1)

/** @brief Peripheral Instances of LCDC instances                              */
#define CSL_LCDC                            (0)

/** @brief Instance number of PLL controller                                  */
#define CSL_PLLC_0                          (0)
#define CSL_PLLC_1                          (1)

/** @brief Peripheral Instance of EMIFA instances                             */
#define CSL_EMIFA                           (0)

/** @brief Peripheral Instance of EMAC instances                              */
#define CSL_EMAC                            (0)

/** @brief Peripheral Instance of MDIO instances                              */
#define CSL_MDIO                            (0)

/** @brief Peripheral Instance of EHRPWM instances                            */
#define CSL_EHRPWM_0                        (0)
#define CSL_EHRPWM_1                        (1)

/** @brief Peripheral Instance of ECAP instances                              */
#define CSL_ECAP_0                          (0)
#define CSL_ECAP_1                          (1)
#define CSL_ECAP_2                          (2)

/** @brief Peripheral Instance of USB instances                               */
#define CSL_USB_0                           (0)
#define CSL_USB_1                           (1)

/** @brief Peripheral Instance of PRU CORE instances                          */
#define CSL_PRUCORE_0                       (0)
#define CSL_PRUCORE_1                       (1)

/** @brief Peripheral Instance of PRUINTC instances                           */
#define CSL_PRUINTC                         (0)

/** @brief Peripheral Instance of INTC instances                              */
#define CSL_INTC                            (0)

/** @brief Peripheral Instance of AINTC instances                             */
#define CSL_AINTC                           (0)

/** @brief Peripheral Instance of RTC instances                               */
#define CSL_RTC                             (0)

/** @brief Peripheral Instance of GPIO instances                              */
#define CSL_GPIO                            (0)

/** @brief Peripheral Instance of ECTL instances                              */
#define CSL_ECTL                            (0)

/** @brief Peripheral Instance of SYSCFG instances                            */
#define CSL_SYSCFG                          (2)

/*******************************************************************************
* Peripheral Base Address
*******************************************************************************/

/** @brief Base address of INTC memory mapped registers                       */
#define CSL_INTC_0_REGS                     (0x01800000u)

/** @brief Base address of PDC memory mapped registers                        */
#define CSL_PWRDWN_PDC_REGS                 (0x01810000u)

/** @brief Base address of SYS - Security ID register                         */
#define CSL_SYS_0_SECURITY_ID_REGS          (0x01811000u)

/** @brief Base address of SYS - Revision ID register                         */
#define CSL_SYS_0_REV_ID_REGS               (0x01812000u)

/** #brief IDMA Module memory mapped address  */
#define CSL_IDMA_0_REGS                     (0x01820000u)

/** #brief EMC Module memory mapped address                                   */
#define CSL_EMC_0_REGS                      (0x01820000u)

/** #brief Cache Module memory mapped address                                 */
#define CSL_CACHE_0_REGS                    (0x01840000u)

/** @brief Base address of Channel controller  memory mapped registers        */
#define CSL_EDMA30CC_0_REGS                 (0x01C00000u)

/** @brief Base address of Transfer controller  memory mapped registers       */
#define CSL_EDMA30TC_0_REGS                 (0x01C08000u)
#define CSL_EDMA30TC_1_REGS                 (0x01C08400u)

/** @brief Base address of PSC memory mapped registers                        */
#define CSL_PSC_0_REGS                      (0x01C10000u)

/** @brief PLL controller instance o module address                           */
#define CSL_PLLC_0_REGS                     (0x01C11000u)

/** @brief Base address of DEV memory mapped registers                        */
#define CSL_SYSCFG_0_REGS                   (0x01C14000u)

/** @brief Base address of TIMER memory mapped registers                      */
#define CSL_TMR_0_REGS                      (0x01C20000u)
#define CSL_TMR_1_REGS                      (0x01C21000u)

/** @brief Base address of I2C memory mapped registers                        */
#define CSL_I2C_0_REGS                      (0x01C22000u)

/** @brief Base address of RTC memory                                         */
#define CSL_RTC_0_REGS                      (0x01C23000u)

/** @brief Base address of MMCSD memory mapped registers                      */
#define CSL_MMCSD_0_REGS                    (0x01C40000u)

/** @brief Base address of SPI memory mapped registers                        */
#define CSL_SPI_0_REGS                      (0x01C41000u)

/** @brief Base address of UART memory mapped registers                       */
#define CSL_UART_0_REGS                     (0x01C42000u)

/** @brief Base address of McASP memory mapped registers                      */
#define CSL_MCASP_0_CTRL_REGS               (0x01D00000u)
#define CSL_MCASP_0_FIFO_REGS               (0x01D01000u)
#define CSL_MCASP_0_DATA_REGS               (0x01D02000u)

/** @brief Base address of UART memory mapped registers                       */
#define CSL_UART_1_REGS                     (0x01D0C000u)
#define CSL_UART_2_REGS                     (0x01D0D000u)

/** @brief Base address of McBSP memory mapped registers                      */
#define CSL_MCBSP_0_CTRL_REGS               (0x01D10000u)
#define CSL_MCBSP_0_FIFO_REGS               (0x01D10800u)
#define CSL_MCBSP_0_DATA_REGS               (0x01F10000u)

/** @brief Base address of McASP memory mapped registers                      */
#define CSL_MCBSP_1_CTRL_REGS               (0x01D11000u)
#define CSL_MCBSP_1_FIFO_REGS               (0x01D11800u)
#define CSL_MCBSP_1_DATA_REGS               (0x01F10000u)

/** @brief Base address of USB memory                                         */
#define CSL_USB_0_REGS                      (0x01E00000u)
#define CSL_USB_1_REGS                      (0x01E25000u)

/** @brief Base address of HPI memory mapped registers                        */
#define CSL_HPI_0_REGS                      (0x01E10000u)

/** @brief Base address of LCDC memory mapped registers                       */
#define CSL_LCDC_0_REGS                     (0x01E13000u)

/** @brief Base address of UPP memory mapped registers                        */
#define CSL_UPP_0_REGS                      (0x01E16000u)

/** @brief Base address of VPIF memory mapped registers                       */
#define CSL_VPIF_0_REGS                     (0x01E17000u)

/** @brief Base address of SATA memory mapped registers                       */
#define CSL_SATA_0_REGS                     (0x01E18000u)

/** @brief PLL controller instance 1 module address                           */
#define CSL_PLLC_1_REGS                     (0X01E1A000u)

/** @brief Base address of MMCSD memory mapped registers                      */
#define CSL_MMCSD_1_REGS                    (0x01E1B000u)

/** @brief Base address of EMAC memory                                        */
#define CSL_EMAC_DSC_CTRL_MOD_RAM           (0x01E20000u)
#define CSL_EMAC_DSC_CTRL_MOD_REG           (0x01E22000u)
#define CSL_EMAC_DSC_CONTROL_REG            (0x01E23000u)
#define CSL_MDIO_0_REGS                     (0x01E24000u)

/** @brief Base address of PRU Core Regsiters                                 */
#define CSL_PRUCORE_0_REGS                  (0x01C37000u)
#define CSL_PRUCORE_1_REGS                  (0x01C37800u)

/** @brief Base address of PRU Interrupt Controller Registers                 */
#define CSL_PRUINTC_0_REGS                  (0x01C34000u)

/** @brief Base address of GPIO memory mapped registers                       */
#define CSL_GPIO_0_REGS                     (0x01E26000u)

/** @brief Base address of PSC memory mapped registers                        */
#define CSL_PSC_1_REGS                      (0x01E27000u)

/** @brief Base address of I2C memory mapped registers                        */
#define CSL_I2C_1_REGS                      (0x01E28000u)

/** @brief Base address of syscfg registers                                   */
#define CSL_SYSCFG_1_REGS                   (0x01E2C000u)

/** @brief Base address of Channel controller  memory mapped registers        */
#define CSL_EDMA31CC_0_REGS                 (0x01E30000u)

/** @brief Base address of Transfer controller  memory mapped registers       */
#define CSL_EDMA31TC_0_REGS                 (0x01E38000u)

/** @brief Base address of EPWM memory mapped registers                       */
#define CSL_EHRPWM_0_REGS                   (0x01F00000u)
#define CSL_EHRPWM_1_REGS                   (0x01F02000u)

/** @brief Base address of EPWM memory mapped registers                       */
#define CSL_HRPWM_0_REGS                    (0x01F01000u)
#define CSL_HRPWM_1_REGS                    (0x01F03000u)

/** @brief Base address of ECAP memory mapped registers                       */
#define CSL_ECAP_0_REGS                     (0x01F06000u)
#define CSL_ECAP_1_REGS                     (0x01F07000u)
#define CSL_ECAP_2_REGS                     (0x01F08000u)

/** @brief Base address of TIMER memory mapped registers                      */
#define CSL_TMR_2_REGS                      (0x01F0C000u)
#define CSL_TMR_3_REGS                      (0x01F0D000u)

/** @brief Base address of SPI memory mapped registers                        */
#define CSL_SPI_1_REGS                      (0x01F0E000u)

/** @brief Base address of EMIFA memory mapped registers                      */
#define CSL_EMIFA_0_REGS                    (0x68000000u)

/** @brief Base address of EMIFA_CS0 memory                                   */
#define CSL_EMIFA_CS0_ADDR                  (0x40000000u)

/** @brief Base address of EMIFA_CS2 memory                                   */
#define CSL_EMIFA_CS2_ADDR                  (0x60000000u)

/** @brief Base address of EMIFA_CS3 memory                                   */
#define CSL_EMIFA_CS3_ADDR                  (0x62000000u)

/** @brief Base address of EMIFA_CS4 memory                                   */
#define CSL_EMIFA_CS4_ADDR                  (0x64000000u)

/** @brief Base address of EMIFA_CS5 memory                                   */
#define CSL_EMIFA_CS5_ADDR                  (0x66000000u)

/** @brief Base address of DDR memory mapped registers                        */
#define CSL_DDR2_0_CTRL_REGS                (0xB0000000u)
#define CSL_DDR2_0_DATA_REGS                (0xC0000000u)

/** @brief Base address of AINTC memory mapped registers                      */
#define CSL_AINTC_0_REGS                    (0xFFFEE000u)

/** @brief Base address of UMC Memory protection registers                    */
#define CSL_MEMPROT_L2_REGS                 (0x00800000u)

/** @brief Base address of PMC memory Protection registers                    */
#define CSL_MEMPROT_L1P_REGS                (0x00E00000u)

/** @brief Base address of DMC memory protection registers                    */
#define CSL_MEMPROT_L1D_REGS                (0x00F00000u)

/******************************************************************************\
* Interrupt Event IDs
\******************************************************************************/

/*
 * @brief   Interrupt Event IDs
 */
/* Output of event combiner 0, for events 1 to 31                             */
#define    CSL_INTC_EVENTID_EVT0            (0)
/* Output of event combiner 0, for events 32 to 63                            */
#define    CSL_INTC_EVENTID_EVT1            (1)
/* Output of event combiner 0, for events 64 to 95                            */
#define    CSL_INTC_EVENTID_EVT2            (2)
/* Output of event combiner 0, for events 96 to 127                           */
#define    CSL_INTC_EVENTID_EVT3            (3)

/* Timer 64P0 lower counter interrupt                                         */
#define    CSL_INTC_EVENTID_T64P0_TINT12    (4)

/* SYSCFG CHIPSIG Register Interrupt                                          */
#define    CSL_INTC_EVENTID_BOOTCFGINT0     (5)

#define    CSL_INTC_EVENTID_EHRPWM0         (7)

/* CC Completion Interrupt for region 1                                       */
#define    CSL_INTC_EVENTID_EDMA3CC_INT1    (8)
/* EMU interrupt                                                              */
#define    CSL_INTC_EVENTID_EMU_DTDMA       (9)

/* HiResTimer Trip Zone Interrupt                                             */
#define    CSL_INTC_EVENTID_EHRPWM0TZ       (10)

/* EMU real time data exchange receive complete                               */
#define    CSL_INTC_EVENTID_EMU_RTDXRX      (11)
/* EMU RTDX transmit complete                                                 */
#define    CSL_INTC_EVENTID_EMU_RTDXTX      (12)

/* IDMA Channel 0 Interrupt                                                   */
#define    CSL_INTC_EVENTID_IDMA0           (13)
/* IDMA Channel 1 Interrupt                                                   */
#define    CSL_INTC_EVENTID_IDMA1           (14)

/* mmcsd0 mmcsd interrupt                                                     */
#define    CSL_INTC_EVENTID_MMCSD0_INT0     (15)
/* mmcsd0 SDIO interrupt                                                      */
#define    CSL_INTC_EVENTID_MMCSD0_INT1     (16)

#define    CSL_INTC_EVENTID_EHRPWM1         (18)

/* USB Interrupt                                                              */
#define    CSL_INTC_EVENTID_USBINT0         (19)
#define    CSL_INTC_EVENTID_USB1_HCINT      (20)
#define    CSL_INTC_EVENTID_USB1_RWAKEUP    (21)

#define    CSL_INTC_EVENTID_EHRPWM1TZ       (23)

/* SATA interrupt                                                             */
#define    CSL_INTC_EVENTID_SATAINT         (24)

/* Timer 64P2 COMBINED counter interrupt                                      */
#define    CSL_INTC_EVENTID_T64P2_TINTALL   (25)

/* EMAC Interrupt                                                             */
#define     CSL_INTC_EVENTID_EMAC_RXTHRHC0  (26)
#define     CSL_INTC_EVENTID_EMAC_RXC0      (27)
#define     CSL_INTC_EVENTID_EMAC_TXC0      (28)
#define     CSL_INTC_EVENTID_EMAC_MISCC0    (29)
#define     CSL_INTC_EVENTID_EMAC_RXTHRHC1  (30)
#define     CSL_INTC_EVENTID_EMAC_RXC1      (31)
#define     CSL_INTC_EVENTID_EMAC_TXC1      (32)
#define     CSL_INTC_EVENTID_EMAC_MISCC1    (33)

/* UHPI DSPI Interrupt                                                        */
#define     CSL_INTC_EVENTID_UHPI_DSPINT    (34)

/* I2C interrupt                                                              */
#define    CSL_INTC_EVENTID_I2CINT0         (36)

/* SPI interrupt                                                              */
#define    CSL_INTC_EVENTID_SPIINT0         (37)

/* UART interrupt                                                             */
#define    CSL_INTC_EVENTID_UARTINT0        (38)

/* Timer 64P1 lower counter interrupt                                         */
#define    CSL_INTC_EVENTID_T64P1_TINT12    (40)

/* GPIO BANK 1 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK1_INT   (41)

#define    CSL_INTC_EVENTID_I2CINT1         (42)

/* SPI interrupt                                                              */
#define    CSL_INTC_EVENTID_SPIINT1         (43)

/* ECAP0 interrupt                                                            */
#define    CSL_INTC_EVENTID_ECAPINT0        (45)

/* UART INTERRUPT                                                             */
#define    CSL_INTC_EVENTID_UARTINT1        (46)

/* ECAP1 interrupt                                                            */
#define    CSL_INTC_EVENTID_ECAPINT1        (47)

/* Timer 64P1 higher counter interrupt                                        */
#define    CSL_INTC_EVENTID_T64P1_TINT34    (48)

/* GPIO BANK 2 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK2_INT   (49)

/* ECAP2 interrupt                                                            */
#define    CSL_INTC_EVENTID_ECAPINT2        (51)

/* GPIO BANK 3 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK3_INT   (52)

/* mmcsd1 SDIO interrupt                                                      */
#define    CSL_INTC_EVENTID_MMCSD1_INT1     (53)

/* GPIO BANK 4 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK4_INT   (54)

/* EMIFA Error Interrupt                                                      */
#define    CSL_INTC_EVENTID_EMIFAINT        (55)

/* EDMA3 CC Interrupt                                                         */
#define    CSL_INTC_EVENTID_TPCC0_ERRINT    (56)
/* EDMA3 TC Interrupt                                                         */
#define    CSL_INTC_EVENTID_TPTC_ERRINT0    (57)
#define    CSL_INTC_EVENTID_TPTC_ERRINT1    (58)

/* GPIO BANK 5 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK5_INT   (59)

/* DDR2 memory err interrupt                                                  */
#define    CSL_INTC_EVENTID_DDR2_MEMERR     (60)

/* McASP combined interrupt                                                   */
#define    CSL_INTC_EVENTID_MCASP0INT       (61)

/* GPIO BANK 6 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK6_INT   (62)

/* RTC Combined Interrupt                                                     */
#define    CSL_INTC_EVENTID_RTC_IRQS        (63)

/* Timer 64P0 higher counter interrupt                                        */
#define    CSL_INTC_EVENTID_T64P0_TINT34    (64)

/* GPIO BANK 0 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK0_INT   (65)

/* SYSCFG_CHIPSIG Register                                                    */
#define    CSL_INTC_EVENTID_SYSCFG_CHIPINT3 (67)

/* MMCSD 1 interrupt                                                          */
#define    CSL_INTC_EVENTID_MMCSD1_INT0     (68)

/* uart2 interrupt                                                            */
#define    CSL_INTC_EVENTID_UARTINT2        (69)
/* PSC interrupt                                                              */
#define    CSL_INTC_EVENTID_PSC0_ALLINT     (70)
#define    CSL_INTC_EVENTID_PSC1_ALLINT     (71)

/* GPIO bank 7 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK7_INT   (72)

/* LCDC interrupt                                                             */
#define    CSL_INTC_EVENTID_LCDC_INT0       (73)

/* SYSCFG Protection Shared Interrupt                                         */
#define    CSL_INTC_EVENTID_PROTERR         (74)

/* GPIO bank 8 interrupt                                                      */
#define    CSL_INTC_EVENTID_GPIO_BNK8_INT   (75)

/* Timer 64P2 Compare Interrupt                                               */
#define    CSL_INTC_EVENTID_T64P2_CMPINT0   (78)
#define    CSL_INTC_EVENTID_T64P2_CMPINT1   (79)
#define    CSL_INTC_EVENTID_T64P2_CMPINT2   (80)
#define    CSL_INTC_EVENTID_T64P2_CMPINT3   (81)
#define    CSL_INTC_EVENTID_T64P2_CMPINT4   (82)
#define    CSL_INTC_EVENTID_T64P2_CMPINT5   (83)
#define    CSL_INTC_EVENTID_T64P2_CMPINT6   (84)
#define    CSL_INTC_EVENTID_T64P2_CMPINT7   (85)
/* Timer 64P3 Compare Interrupt                                               */
#define    CSL_INTC_EVENTID_T64P3_TINTALL   (86)

/* McBSP0 RX interrupt                                                        */
#define    CSL_INTC_EVENTID_MCBSP0_RXINT    (87)
/* McBSP0 TX interrupt                                                        */
#define    CSL_INTC_EVENTID_MCBSP0_TXINT    (88)
/* McBSP1 RX interrupt                                                        */
#define    CSL_INTC_EVENTID_MCBSP1_RXINT    (89)
/* McBSP1 TX interrupt                                                        */
#define    CSL_INTC_EVENTID_MCBSP1_TXINT    (90)

/* CC1 region 1 interrupt                                                     */
#define    CSL_INTC_EVENTID_EDMA3CC1_INT1   (91)

/* CC1 Error Interrupt                                                        */
#define    CSL_INTC_EVENTID_EDMA3CC1_ERRINT (92)

/* TC2 Error Interrupt                                                        */
#define    CSL_INTC_EVENTID_EDMA3TC2_ERRINT (93)

/* UPP interrupt event                                                        */
#define    CSL_INTC_EVENTID_UPPINT          (94)
/* VPIF INTERRUPT EVENT                                                       */
#define    CSL_INTC_EVENTID_VPIFINT         (95)
/* Dropped CPU interrupt event                                                */
#define    CSL_INTC_EVENTID_INTERR          (96)
/* EMC Invalid IDMA parameters                                                */
#define    CSL_INTC_EVENTID_EMC_IDMAERR     (97)

/* PMC Single bit error detected during DMA read                              */
#define    CSL_INTC_EVENTID_PMC_ED          (113)

/* UMC single bit error detected                                              */
#define    CSL_INTC_EVENTID_UMC_ED1         (116)
/* UMC two bit error detected                                                 */
#define    CSL_INTC_EVENTID_UMC_ED2         (117)
/* Power Down sleep interrupt                                                 */
#define    CSL_INTC_EVENTID_PDC_INT         (118)

/* SYS CMPA CPU memory protection fault                                       */
#define    CSL_INTC_EVENTID_SYS_CMPA        (119)

/* PMC CPU memory protection fault                                            */
#define    CSL_INTC_EVENTID_PMC_CMPA        (120)
/* PMC DMA memory protection fault                                            */
#define    CSL_INTC_EVENTID_PMC_DMPA        (121)
/* DMC CPU memory protection fault                                            */
#define    CSL_INTC_EVENTID_DMC_CMPA        (122)
/* DMC DMA memory protection fault                                            */
#define    CSL_INTC_EVENTID_DMC_DMPA        (123)
/* UMC CPU memory protection fault                                            */
#define    CSL_INTC_EVENTID_UMC_CMPA        (124)
/* UMC DMA memory protection fault                                            */
#define    CSL_INTC_EVENTID_UMC_DMPA        (125)
/* IDMA CPU memory protection fault                                           */
#define    CSL_INTC_EVENTID_EMC_CMPA        (126)
/* IDMA Bus error interrupt                                                   */
#define    CSL_INTC_EVENTID_EMC_BUSERR      (127)

/*************************** EDMA RELATED DEFINES  ****************************/

/******************************************************************************\
*  Parameterizable Configuration:- These are fed directly from the RTL
*  parameters for the given SOC
\******************************************************************************/

#define CSL_EDMA3_NUM_DMACH                 32
#define CSL_EDMA3_NUM_QDMACH                8
#define CSL_EDMA3_NUM_PARAMSETS             128
#define CSL_EDMA3_NUM_EVQUE                 2
#define CSL_EDMA3_CHMAPEXIST                0
#define CSL_EDMA3_NUM_REGIONS               4
#define CSL_EDMA3_MEMPROTECT                0

/******************************************************************************\
* Channel Instance count
\******************************************************************************/
#define CSL_EDMA3_CHA_CNT                   (CSL_EDMA3_NUM_DMACH + \
                                             CSL_EDMA3_NUM_QDMACH)

/* EDMA channel synchronization events                                        */

/* McASP0 Receive Event                                                       */
#define CSL_EDMA3_CHA_MCASP0_RX             0
/* McASP0 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MCASP0_TX             1
/* McBSP0 Receive Event                                                       */
#define CSL_EDMA3_CHA_MCBSP0_RX             2
/* McBSP1 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MCBSP0_TX             3
/* McBSP1 Receive Event                                                       */
#define CSL_EDMA3_CHA_MCBSP1_RX             4
/* McBSP1 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MCBSP1_TX             5

/* GPIO Bank0 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT0          6
/* GPIO Bank1 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT1          7
/* GPIO Bank2 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT2          22
/* GPIO Bank3 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT3          23
/* GPIO Bank4 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT4          28
/* GPIO Bank5 event                                                           */
#define CSL_EDMA3_CHA_GPIO_BNKINT5          29
/* GPIO Bank6 event    (TPCC1)                                                */
#define CSL_EDMA3_CHA_GPIO_BNKINT6          16
/* GPIO Bank7 event    (TPCC1)                                                */
#define CSL_EDMA3_CHA_GPIO_BNKINT7          17
/* GPIO Bank8 event    (TPCC1)                                                */
#define CSL_EDMA3_CHA_GPIO_BNKINT8          18

/* UART0 Receive Event                                                        */
#define CSL_EDMA3_CHA_UART0_RX              8
/* UART0 Transmit Event                                                       */
#define CSL_EDMA3_CHA_UART0_TX              9
/* UART1 Receive Event                                                        */
#define CSL_EDMA3_CHA_UART1_RX              12
/* UART1 Transmit Event                                                       */
#define CSL_EDMA3_CHA_UART1_TX              13
/* UART2 Receive Event                                                        */
#define CSL_EDMA3_CHA_UART2_RX              30
/* UART2 Transmit Event                                                       */
#define CSL_EDMA3_CHA_UART2_TX              31

/* Timer 64P0 Event Out 12                                                    */
#define CSL_EDMA3_CHA_TIMER64P0_EVT12       10
/* Timer 64P0 Event Out 34                                                    */
#define CSL_EDMA3_CHA_TIMER64P0_EVT34       11

/* Timer 64P2 Event Out 12   (TPCC1)                                          */
#define CSL_EDMA3_CHA_TIMER64P2_EVT12       24
/* Timer 64P2 Event Out 34   (TPCC1)                                          */
#define CSL_EDMA3_CHA_TIMER64P2_EVT34       25

/* Timer 64P3 Event Out 12   (TPCC1)                                          */
#define CSL_EDMA3_CHA_TIMER64P3_EVT12       26
/* Timer 64P3 Event Out 34   (TPCC1)                                          */
#define CSL_EDMA3_CHA_TIMER64P3_EVT34       27

/* SPI0 Receive Event                                                         */
#define CSL_EDMA3_CHA_SPI0_RX               14
/* SPI0 Transmit Event                                                        */
#define CSL_EDMA3_CHA_SPI0_TX               15
/* SPI1 Receive Event                                                         */
#define CSL_EDMA3_CHA_SPI1_RX               18
/* SPI1 Transmit Event                                                        */
#define CSL_EDMA3_CHA_SPI1_TX               19

/* MMCSD0 Receive Event                                                       */
#define CSL_EDMA3_CHA_MMCSD0_RX             16
/* MMCSD0 Transmit Event                                                      */
#define CSL_EDMA3_CHA_MMCSD0_TX             17

/* MMCSD1 Receive Event    (TPCC1)                                            */
#define CSL_EDMA3_CHA_MMCSD1_RX             28
/* MMCSD1 Transmit Event   (TPCC1)                                            */
#define CSL_EDMA3_CHA_MMCSD1_TX             29

/* I2C0 Receive Event                                                         */
#define CSL_EDMA3_CHA_I2C0_RX               24
/* I2C0 Transmit Event                                                        */
#define CSL_EDMA3_CHA_I2C0_TX               25
/* I2C1 Receive Event                                                         */
#define CSL_EDMA3_CHA_I2C1_RX               26
/* I2C1 Transmit Event                                                        */
#define CSL_EDMA3_CHA_I2C1_TX               27

/* Timer 2 compare event0  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT0         00
/* Timer 2 compare event1  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT1         01
/* Timer 2 compare event2  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT2         02
/* Timer 2 compare event3  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT3         03
/* Timer 2 compare event4  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT4         04
/* Timer 2 compare event5  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT5         05
/* Timer 2 compare event6  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT6         06
/* Timer 2 compare event7  (TPCC1)                                            */
#define CSL_EDMA3_TIMER2_T12CMPEVT7         07

/* Timer 3 compare event0  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT0         08
/* Timer 3 compare event1  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT1         09
/* Timer 3 compare event2  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT2         10
/* Timer 3 compare event3  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT3         11
/* Timer 3 compare event4  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT4         12
/* Timer 3 compare event5  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT5         13
/* Timer 3 compare event6  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT6         14
/* Timer 3 compare event7  (TPCC1)                                            */
#define CSL_EDMA3_TIMER3_T12CMPEVT7         15

/* QDMA channels                                                              */
#define    CSL_EDMA3_QCHA_BASE              CSL_EDMA3_NUM_DMACH	/* QDMA Channel Base */
#define    CSL_EDMA3_QCHA_0                 (CSL_EDMA3_QCHA_BASE + 0)	/* QDMA Channel 0 */
#define    CSL_EDMA3_QCHA_1                 (CSL_EDMA3_QCHA_BASE + 1)	/* QDMA Channel 1 */
#define    CSL_EDMA3_QCHA_2                 (CSL_EDMA3_QCHA_BASE + 2)	/* QDMA Channel 2 */
#define    CSL_EDMA3_QCHA_3                 (CSL_EDMA3_QCHA_BASE + 3)	/* QDMA Channel 3 */
#define    CSL_EDMA3_QCHA_4                 (CSL_EDMA3_QCHA_BASE + 4)	/* QDMA Channel 4 */
#define    CSL_EDMA3_QCHA_5                 (CSL_EDMA3_QCHA_BASE + 5)	/* QDMA Channel 5 */
#define    CSL_EDMA3_QCHA_6                 (CSL_EDMA3_QCHA_BASE + 6)	/* QDMA Channel 6 */
#define    CSL_EDMA3_QCHA_7                 (CSL_EDMA3_QCHA_BASE + 7)	/* QDMA Channel 7 */

/* Enumerations for EDMA Controlleres */
	typedef enum {
		CSL_EDMACC_ANY = -1,	/* Any instance of EDMACC module */
		CSL_EDMACC_0 = 0	/* EDMACC Instance 0            */
	} CSL_EdmaccNum;

/* Enumerations for EDMA Event Queues */
	typedef enum {
		CSL_EDMA3_QUE_0 = 0,	/* Queue 0                     */
		CSL_EDMA3_QUE_1 = 1	/* Queue 1                     */
	} CSL_Edma3Que;

/* Enumerations for EDMA Transfer Controllers
 *
 * There are 2 Transfer Controllers. Typically a one to one mapping exists
 * between Event Queues and Transfer Controllers.
 *
 */
	typedef enum {
		CSL_EDMATC_ANY = -1,	/* Any instance of EDMATC                */
		CSL_EDMATC_0 = 0,	/* EDMATC Instance 0                     */
		CSL_EDMATC_1 = 1	/* EDMATC Instance 1                     */
	} CSL_EdmatcNum;

#define CSL_EDMA3_REGION_GLOBAL            -1
#define CSL_EDMA3_REGION_0                  0
#define CSL_EDMA3_REGION_1                  1
#define CSL_EDMA3_REGION_2                  2
#define CSL_EDMA3_REGION_3                  3

/******************************* DAT RELATED DEFINES **************************/

/******************************************************************************\
*  Parameterizable Configuration:- These are fed directly from the RTL
*  parameters for the given SOC
\******************************************************************************/
/******************************************************************************\
* Channel Instance count
\******************************************************************************/
/** @brief Number of Generic Channel instances                                */

/** @brief Enumerations for EDMA channels
*
*  There are 8 QDMA channels -
*
*/

#define    CSL_DAT_QCHA_0                   0 /**<   QDMA Channel 0           */
#define    CSL_DAT_QCHA_1                   1 /**<   QDMA Channel 1           */
#define    CSL_DAT_QCHA_2                   2 /**<   QDMA Channel 2           */
#define    CSL_DAT_QCHA_3                   3 /**<   QDMA Channel 3           */
#define    CSL_DAT_QCHA_4                   4 /**<   QDMA Channel 4           */
#define    CSL_DAT_QCHA_5                   5 /**<   QDMA Channel 5           */
#define    CSL_DAT_QCHA_6                   6 /**<   QDMA Channel 6           */
#define    CSL_DAT_QCHA_7                   7 /**<   QDMA Channel 7           */

/** @brief Enumerations for EDMA Event Queues
*
*  There are two Event Queues. Q0 is the highest priority and Q1 is the least
*  priority
*
*/
	typedef enum {
		CSL_DAT_PRI_DEFAULT = 0,	/* Queue 0 is default          */
		CSL_DAT_PRI_0 = 0,	/* Queue 0                     */
		CSL_DAT_PRI_1 = 1	/* Queue 1                     */
	} CSL_DatPriority;

/** @brief Enumeration for EDMA Regions
*
*
*/

#define CSL_DAT_REGION_GLOBAL              -1	/* Global Region        */
#define CSL_DAT_REGION_0                    0	/* EDMA Region 0        */
#define CSL_DAT_REGION_1                    1	/* EDMA Region 1        */
#define CSL_DAT_REGION_2                    2	/* EDMA Region 2        */
#define CSL_DAT_REGION_3                    3	/* EDMA Region 3        */

#ifdef __cplusplus
}
#endif
#endif				/* _SOC_H_ */
