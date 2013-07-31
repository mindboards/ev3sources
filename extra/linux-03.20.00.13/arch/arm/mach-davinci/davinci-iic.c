#include  <asm/types.h>

// Placeholder for the two symbolic include files - start
#include "mach/da8xx.h"

#define FINAL 1
#define HARDWARE FINAL
#define INPUTS 4

enum
  {
    GP0_0,GP0_1,GP0_2,GP0_3,GP0_4,GP0_5,GP0_6,GP0_7,GP0_8,GP0_9,GP0_10,GP0_11,GP0_12,GP0_13,GP0_14,GP0_15,
    GP1_0,GP1_1,GP1_2,GP1_3,GP1_4,GP1_5,GP1_6,GP1_7,GP1_8,GP1_9,GP1_10,GP1_11,GP1_12,GP1_13,GP1_14,GP1_15,
    GP2_0,GP2_1,GP2_2,GP2_3,GP2_4,GP2_5,GP2_6,GP2_7,GP2_8,GP2_9,GP2_10,GP2_11,GP2_12,GP2_13,GP2_14,GP2_15,
    GP3_0,GP3_1,GP3_2,GP3_3,GP3_4,GP3_5,GP3_6,GP3_7,GP3_8,GP3_9,GP3_10,GP3_11,GP3_12,GP3_13,GP3_14,GP3_15,
    GP4_0,GP4_1,GP4_2,GP4_3,GP4_4,GP4_5,GP4_6,GP4_7,GP4_8,GP4_9,GP4_10,GP4_11,GP4_12,GP4_13,GP4_14,GP4_15,
    GP5_0,GP5_1,GP5_2,GP5_3,GP5_4,GP5_5,GP5_6,GP5_7,GP5_8,GP5_9,GP5_10,GP5_11,GP5_12,GP5_13,GP5_14,GP5_15,
    GP6_0,GP6_1,GP6_2,GP6_3,GP6_4,GP6_5,GP6_6,GP6_7,GP6_8,GP6_9,GP6_10,GP6_11,GP6_12,GP6_13,GP6_14,GP6_15,
    GP7_0,GP7_1,GP7_2,GP7_3,GP7_4,GP7_5,GP7_6,GP7_7,GP7_8,GP7_9,GP7_10,GP7_11,GP7_12,GP7_13,GP7_14,GP7_15,
    GP8_0,GP8_1,GP8_2,GP8_3,GP8_4,GP8_5,GP8_6,GP8_7,GP8_8,GP8_9,GP8_10,GP8_11,GP8_12,GP8_13,GP8_14,GP8_15,
    NO_OF_GPIOS,
    UART0_TXD,UART0_RXD,UART1_TXD,UART1_RXD,
    SPI1_MOSI,SPI1_MISO,SPI1_SCL,SPI1_CS,
    EPWM1A,EPWM1B,APWM0,APWM1,EPWM0B
  };

typedef   struct gpio_controller *__iomem     GPIOC;

typedef struct
{
  int     Pin;      // GPIO pin number
  GPIOC   pGpio;    // GPIO bank base address
  u32     Mask;     // GPIO pin mask
} INPIN;
// Placeholder for the two symbolic include files - stop


#define   __USE_POSIX

#include  <linux/kernel.h>
#include  <linux/init.h>
#include  <linux/ioport.h>
#include  <linux/delay.h>
#include  <asm/gpio.h>
#include  <asm/io.h>
#include  <linux/circ_buf.h>
#include  "d_iic.h"

#include <asm/fiq.h>
#include <mach/irqs.h>
#include <linux/err.h>

int iic_fiq_handler(void *data);
void davinci_fiq_disable(void);

void cp_intc_ack_irq(unsigned int irq);
void cp_intc_fiq_enable(bool enable, unsigned int irq);
void cp_intc_set_channel(unsigned int channel, unsigned int irq);

#define BUFFER_SIZE 256
struct IIC_control_t IicCtrl;


//*****************************************************************************
#if       (HARDWARE == FINAL)
//*****************************************************************************
//*****************************************************************************

/*! \page IicModuleResources Gpios and Resources used for Module
 *
 *  Describes use of gpio and resources\n
 *
 *  \verbatim
 */

INPIN     IicPortPin[][IIC_PORT_PINS] =
  {
    { // Input port 1
      { GP8_11 , NULL, 0 }, // Buffer disable
      { GP0_2  , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
      { GP0_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
    },
    { // Input port 2
      { GP8_14 , NULL, 0 }, // Buffer disable
      { GP0_14 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
      { GP0_13 , NULL, 0 }, // Pin 6  - Digital output  // DATA
    },
    { // Input port 3
      { GP7_9  , NULL, 0 }, // Buffer disable
      { GP0_12 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
      { GP1_14 , NULL, 0 }, // Pin 6  - Digital output  // DATA
    },
    { // Input port 4
      { GP7_10 , NULL, 0 }, // Buffer disable
      { GP1_13 , NULL, 0 }, // Pin 5  - Digital input   // CLOCK
      { GP1_15 , NULL, 0 }, // Pin 6  - Digital output  // DATA
    },
  };

/*  \endverbatim
 *  \n
 */

#else
#error Structs only defined for HARDWARE == FINAL
#endif

int t_index, first[512], last[512];

// Uncomment these defines to time FiQ response time
//#define TIMING_TOTAL 1
//#define TIMING_CHANNEL 1
//#define TIMING_OVERHEAD 1
//#define TIMING_COLLECT_TIMES 1
//#define TIMING_COLLECT_TIMES_VERBOSE 1

u64 read_cycles_none(void);

void davinci_fiq_handler(void)
{
  int keep_running = 0;
#if defined(TIMING_TOTAL) || defined (TIMING_CHANNEL) 
  PINOutput(2,IIC_PORT_DATA);
#endif

#ifdef TIMING_TOTAL
  PINHigh(2,IIC_PORT_DATA);
#endif

#ifdef TIMING_COLLECT_TIMES
  first[t_index] = read_cycles_none();
#endif

  /* disable further timer interrupts if nobody has any work                                                    
   * or adjust rate according to who still has work
   *
   * CAUTION: it means forground code must disable FIQ around 
   * its own non-atomic INTMSK changes... not common 
   * thankfully and taken care of by the fiq-basis patch                                                        
   */

  keep_running = iic_fiq_handler(NULL);

  if (!keep_running) {
    /* Disable fiq */
    davinci_fiq_disable();
  }

  // Indicate to interrupt controller that fiq has been handled
  cp_intc_ack_irq(IRQ_DA8XX_TINT34_1);
#ifdef TIMING_OVERHEAD
  PINLow(2,IIC_PORT_DATA);
  PINHigh(2,IIC_PORT_DATA);
  PINLow(2,IIC_PORT_DATA);
  PINHigh(2,IIC_PORT_DATA);
#endif
#ifdef TIMING_TOTAL
  PINLow(2,IIC_PORT_DATA);
#endif

#ifdef TIMING_COLLECT_TIMES
  last[t_index++] = read_cycles_none();
#endif
}

int davinci_fiq_enable(void)
{
  /* Disable fiq and set interrupt handler  */
  local_fiq_disable();
  set_fiq_c_handler(davinci_fiq_handler);

  /* Enable fiq  */
  cp_intc_set_channel(0, IRQ_DA8XX_TINT34_1);
  cp_intc_fiq_enable(true, IRQ_DA8XX_TINT34_1);
  local_fiq_enable();
  return 0;
}

void davinci_fiq_disable(void)
{
  /* Disable fiq  */
  local_fiq_disable();
  cp_intc_fiq_enable(false, IRQ_DA8XX_TINT34_1);
}




int iic_fiq_handler(void *data)
{
  u8 Port;
  struct IIC_data_package *iic;
  u8 restart_timer = false;

  for (Port=0; Port < INPUTS; Port++) {
#ifdef TIMING_CHANNEL
    if (Port & 1) PINHigh(2,IIC_PORT_DATA)
    else PINLow(2,IIC_PORT_DATA);
#endif
    iic = &IicCtrl.data_package[Port];
    if (iic->transfer_state != TRANSFER_IDLE) {
      restart_timer = true;
      
      // Check set high in TRANSFER_STOP2 state
      if (iic->clock_state)
	PINHigh(Port,IIC_PORT_CLOCK)
      else
	PINLow(Port,IIC_PORT_CLOCK);
    }

    switch (iic->transfer_state) {
      case TRANSFER_START  : // Make sure to SYNC into Timer settings 
	                     // to ensure first bit time having full length 
                             iic->transfer_state = TRANSFER_START2;
	                     break;

      case TRANSFER_START2 : // Generate start condition
                             PINLow(Port,IIC_PORT_DATA);
			     PINOutput(Port,IIC_PORT_DATA);
                             iic->clock_state = 0;
                             iic->nacked = false;
                             iic->transfer_state = TRANSFER_ADDR;
                             break;
			     
      case TRANSFER_ADDR  :  iic->data_byte = (iic->addr << 1);
                             // Check if write_lenght = 0? In given case we are to do a read
                             if (iic->write_length)
                               iic->read_start_send = false;
                             else
                               iic->data_byte |= 1;

                             iic->byte = 0;       // Byte is offset one as it's increased by writing address
                             iic->write_length++; // Need to transfer Address as well

      case TRANSFER_DATA :   if (iic->transfer_state == TRANSFER_DATA)
                               iic->data_byte = iic->data[iic->byte-1];
                             iic->transfer_state = TRANSFER_WBIT;
                             iic->bit  = 0x80;
			     PINOutput(Port,IIC_PORT_DATA);

      case TRANSFER_WBIT:  if (!iic->clock_state) {
                               if (iic->data_byte & iic->bit)
                                 PINHigh(Port,IIC_PORT_DATA)
                               else
                                 PINLow(Port,IIC_PORT_DATA);
                               iic->bit >>= 1;
                             }

                             if ((!iic->bit) && (iic->clock_state))
                             {
                               iic->byte++;
                               iic->write_length--;
                               iic->transfer_state = TRANSFER_RACK;
                             }
                             iic->clock_state ^= 1;
                             break;

      case TRANSFER_READ  :  PINInput(Port,IIC_PORT_DATA);
                             iic->transfer_state = TRANSFER_RBIT;
                             iic->bit  = 0x80;
                             iic->data_byte = 0;

      case TRANSFER_RBIT :   if (iic->clock_state) {
                               iic->data_byte |= PINRead(Port, IIC_PORT_DATA)? iic->bit:0;
                               iic->bit >>= 1;

                               if (!iic->bit)
                               {
                                 iic->data[iic->byte-1] = iic->data_byte;
                                 iic->byte++;
                                 iic->read_length--;
                                 iic->transfer_state = TRANSFER_WACK;
                               }
                             }
                             iic->clock_state ^= 1;
                             break;

      case TRANSFER_RACK  :  if (!iic->clock_state)
                               PINInput(Port,IIC_PORT_DATA)
                             else {

                               if (PINRead(Port, IIC_PORT_DATA) == 0) {
                                 if (iic->write_length) {
				   iic->wait_cycles = 4;
                                   iic->transfer_state = TRANSFER_WAIT;
				 }
                                 else if (iic->read_start_send) {
				   iic->wait_cycles = 4;
                                   iic->transfer_state = TRANSFER_WAIT;
				 }
                                 else
                                   iic->transfer_state = TRANSFER_STOP;
                               }
                               else
                               {
                                 iic->nacked = true;
                                 iic->transfer_state = TRANSFER_STOP;
                               }
                             }

                             iic->clock_state ^= 1;
                             break;

      case TRANSFER_WACK  :  if (!iic->clock_state) {
                               if (iic->read_length)
                                 PINLow(Port,IIC_PORT_DATA)  // ACK
                               else
                                 PINHigh(Port,IIC_PORT_DATA);  // NACK the last one
			       PINOutput(Port,IIC_PORT_DATA);
                             } else {
                               if (iic->read_length) {
				 iic->wait_cycles = 2;
				 iic->transfer_state = TRANSFER_WAIT;
			       }
                               else
                                   iic->transfer_state = TRANSFER_STOP;
                             }
                             iic->clock_state ^= 1;
                             break;

      case TRANSFER_WAIT  :  if (iic->wait_cycles--) {}
	                     else if (iic->write_length)
			       iic->transfer_state = TRANSFER_DATA;
                             else if (iic->read_length)
			       iic->transfer_state = TRANSFER_READ;
	                     break;

      case TRANSFER_STOP  :  if (iic->read_length) {
                               PINLow(Port,IIC_PORT_DATA);
 			       PINOutput(Port,IIC_PORT_DATA);
			     }
                             else
                               PINInput(Port,IIC_PORT_DATA);

                             if (iic->clock_state)
                               iic->transfer_state = TRANSFER_STOP2;
                             iic->clock_state = 1;
                             break;

      case TRANSFER_STOP2 :  //PINHigh(Port,IIC_PORT_DATA);
                             if (iic->nacked)
                                 iic->transfer_state = TRANSFER_STOP3;
                             else if (iic->read_length) {
                               PINHigh(Port,IIC_PORT_DATA);
  			       PINOutput(Port,IIC_PORT_DATA);
                               iic->clock_state ^= 1;
                               if  (iic->clock_state) {
                                 iic->transfer_state = TRANSFER_RESTART;
                                 iic->read_start_send = true;
                               }
                             }
                             else
                               iic->transfer_state = TRANSFER_STOP3;
                             break;

      case TRANSFER_RESTART: iic->transfer_state = TRANSFER_START2;
                             break;

      case TRANSFER_STOP3 :  PINInput(Port,IIC_PORT_DATA);
	                     //PINHigh(Port,IIC_PORT_DATA);
	                     //PINOutput(Port,IIC_PORT_DATA);
			     iic->transfer_state = TRANSFER_IDLE;
      case TRANSFER_IDLE:

      default             :  break;
    }
  }

  return (restart_timer);
}


int fiq_register_callback(int (*func)(void *arg), void *data);
int fiq_set_time(int time);

void iic_fiq_start_transfer(unsigned int time, bool fiq_nirq)
{
#ifdef TIMING_COLLECT_TIMES
  int i=0, fiq_cycles=0, fiq_cycles_total=0, cycles_total=0;

  // Print statistics from last FIQ transfer
#ifdef TIMING_COLLECT_TIMES_VERBOSE
  if (first[0]) printk("UsedInFiqLast: ");
#endif

  while(first[i]) {
    fiq_cycles = last[i] - first[i];
#ifdef TIMING_COLLECT_TIMES_VERBOSE
    printk("%d ", fiq_cycles);
#endif
    fiq_cycles_total += fiq_cycles;
    i++;
  }
#ifdef TIMING_COLLECT_TIMES_VERBOSE
  if (first[0]) printk("\n");
#endif

  if (first[0]) {
    cycles_total = last[i-1] - first[0];
    printk("UsedInFiqTotal = %d, CyclesTotal=%d, Percentage=%d\n",
	   fiq_cycles_total, cycles_total, fiq_cycles_total*100/cycles_total);
  }
  // Prepare collecting data in next transfer 
  t_index = 0;
  memset(first,0, sizeof(first));
  memset(last, 0, sizeof(last));
#endif

  // Set FiQ period time
  fiq_set_time(time);
  
  // Decide to run FiQ functionality as FIQ or in ordinary IRQ
  if (fiq_nirq) {
    davinci_fiq_enable();
  } else {
    fiq_register_callback(&iic_fiq_handler, NULL);
    cp_intc_set_channel(2, IRQ_DA8XX_TINT34_1);
    cp_intc_fiq_enable(true, IRQ_DA8XX_TINT34_1);
  }
}

EXPORT_SYMBOL(iic_fiq_start_transfer);
EXPORT_SYMBOL(IicPortPin);
EXPORT_SYMBOL(IicCtrl);
