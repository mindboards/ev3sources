#ifndef __D_IIC_H
#define __D_IIC_H



// Defines and Structures for internal and user space usage --------------------------------------------

enum TransferState {
  TRANSFER_IDLE,
  TRANSFER_START,
  TRANSFER_START2,
  TRANSFER_ADDR,
  TRANSFER_DATA,
  TRANSFER_WRITE,
  TRANSFER_READ,
  TRANSFER_WBIT,
  TRANSFER_RBIT,
  TRANSFER_WACK,
  TRANSFER_RACK,
  TRANSFER_STOP,
  TRANSFER_STOP2,
  TRANSFER_STOP3,
  TRANSFER_RESTART,
  TRANSFER_WAIT,
  TRANSFER_TESTING
};


struct IIC_data_package {
  __u8 addr;
  __u8 data[32];
  __u8 read_length;
  __u8 write_length;
  __u8 clock_state;
  __u8 bit;
  __u8 byte;
  __u8 data_byte;
  __u8 transfer_state;
  __u8 read_start_send;
  __u8 nacked;
  __u8 port;
  __u8 wait_cycles;
};

#define IIC_MAGIC 'i'                                // defines the magic number
#define IIC_PORT_CONFIG_IOC _IOW(IIC_MAGIC,0, int)   // defines our ioctl call.
#define IIC_READ_WRITE_IOC  _IOWR(IIC_MAGIC,1, struct IIC_data_package)  // defines our ioctl call.


// Defines and structures for internal usage --------------------------------------------------

#define IIC_INPUTS 4

struct IIC_control_t {
  // For communication to FIQ and timer functions
  struct IIC_data_package data_package[IIC_INPUTS];
  __u8  port_enabled;
  __u8  transfers_active;
};

enum      IicPortPins
{
  IIC_PORT_BUFFER_CTRL,
  IIC_PORT_CLOCK,
  IIC_PORT_DATA,
  IIC_PORT_PINS,
};

#define   PINInput(port,pin)            { (*IicPortPin[port][pin].pGpio).dir |=  IicPortPin[port][pin].Mask; }
#define   PINOutput(port,pin)           { (*IicPortPin[port][pin].pGpio).dir &= ~IicPortPin[port][pin].Mask; }
#define   PINRead(port,pin)             (((*IicPortPin[port][pin].pGpio).in_data & IicPortPin[port][pin].Mask)?1:0)
#define   PINHigh(port,pin)             { (*IicPortPin[port][pin].pGpio).set_data  =  IicPortPin[port][pin].Mask; }
#define   PINLow(port,pin)              { (*IicPortPin[port][pin].pGpio).clr_data  =  IicPortPin[port][pin].Mask; }

#endif
