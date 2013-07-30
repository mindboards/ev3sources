/*
 * LEGO® MINDSTORMS EV3
 *
 * Copyright (C) 2010-2013 The LEGO Group
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

/*
 * This UsbDev file is based on and inheritated from
 * the original file (zero.c) and work done by David Brownell
 *
 * >> zero.c -- Gadget Zero, for USB development <<
 *
 * >> Copyright (C) 2003-2008 David Brownell <<
 * >> Copyright (C) 2008 by Nokia Corporation <<
 *
 */

/*! \page UsbdevModule USB device Module
 *
 *
 *-  \subpage UsbdevModuleResources
 */


#ifndef PCASM
#include  <asm/types.h>
#endif

#include  "../../lms2012/source/lms2012.h"
#include  "../../lms2012/source/am1808.h"


#define   MODULE_NAME                   "usbdev_module"
#define   DEVICE1_NAME                  USBDEV_DEVICE



static    int  ModuleInit(void);
static    void ModuleExit(void);

#define   __USE_POSIX

#include  <linux/kernel.h>
#include  <linux/fs.h>

#include  <linux/sched.h>


#ifndef   PCASM
#include  <linux/hrtimer.h>

#include  <linux/mm.h>
#include  <linux/hrtimer.h>

#include  <linux/init.h>
#include  <linux/uaccess.h>
#include  <linux/debugfs.h>

#include  <linux/ioport.h>
#include  <asm/gpio.h>
#include  <asm/io.h>
#include  <linux/module.h>
#include  <linux/miscdevice.h>
#include  <asm/uaccess.h>
#include  <linux/hid.h>
#include  <linux/utsname.h>
#include  <linux/device.h>

#include  "computil.c"                            // The composite framework used as utility file
#include  <../drivers/usb/gadget/gadget_chips.h>
#include  <../drivers/usb/gadget/usbstring.c>
#include  <../drivers/usb/gadget/config.c>
#include  <../drivers/usb/gadget/epautoconf.c>


/*-------------------------------------------------------------------------*/

/*
 * Kbuild is not very cooperative with respect to linking separately
 * compiled library objects into one module.  So for now we won't use
 * separate compilation ... ensuring init/exit sections work to shrink
 * the runtime footprint, and giving us at least some parts of what
 * a "gcc --combine ... part1.c part2.c part3.c ... " build would.
 */

#include  <../drivers/usb/gadget/g_zero.h>
#define MAX_EP_SIZE 1024
#define MAX_FULLSPEED_EP_SIZE 64
unsigned buflen = MAX_EP_SIZE ;
char usb_char_buffer_in[MAX_EP_SIZE];
char usb_full_buffer_in[MAX_FULLSPEED_EP_SIZE];
int usb_char_in_length = 0;
char usb_char_buffer_out[MAX_EP_SIZE];
char usb_full_buffer_out[MAX_FULLSPEED_EP_SIZE];
int usb_char_out_length = 0;

#define     SHM_LENGTH    (sizeof(UsbSpeedDefault))
#define     NPAGES        ((SHM_LENGTH + PAGE_SIZE - 1) / PAGE_SIZE)
static void *kmalloc_ptr;

#include  "usb_function.c"    // Specific USB functionality

/*-------------------------------------------------------------------------*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("The LEGO Group");
MODULE_DESCRIPTION(MODULE_NAME);
MODULE_SUPPORTED_DEVICE(DEVICE1_NAME);

module_init(ModuleInit);
module_exit(ModuleExit);

#else
// Keep Eclipse happy
#endif

// USB main stuff

#define DRIVER_VERSION    "31jan2011->"

#ifndef   PCASM
module_param(buflen, uint, 0);
#else
// Keep Eclipse happy
#endif

static int loopdefault = 0;
#ifndef   PCASM
module_param(loopdefault, bool, S_IRUGO|S_IWUSR);
#else
// Keep Eclipse happy
#endif

#define DRIVER_VENDOR_NUM     0x0694      // LEGO Group
#define DRIVER_PRODUCT_NUM    0x0005      // No. 5 in a row
#define DEFAULT_AUTORESUME    0

/* If the optional "autoresume" mode is enabled, it provides good
 * functional coverage for the "USBCV" test harness from USB-IF.
 * It's always set if OTG mode is enabled.
 */

unsigned autoresume = DEFAULT_AUTORESUME;
module_param(autoresume, uint, S_IRUGO);

#ifndef   PCASM
MODULE_PARM_DESC(autoresume, "zero, or seconds before remote wakeup");
#else
// Keep Eclipse happy
#endif

/*-------------------------------------------------------------------------*/

static struct usb_device_descriptor device_desc = {
  .bLength =    sizeof device_desc,
  .bDescriptorType =  USB_DT_DEVICE,

  .bcdUSB =   cpu_to_le16(0x0200),
  .bDeviceClass =   0,
  .bDeviceSubClass =  0,
  .bDeviceProtocol =  0,
  /*.bMaxPacketSize0 = f(hardware) */
  .idVendor =   cpu_to_le16(DRIVER_VENDOR_NUM),
  .idProduct =    cpu_to_le16(DRIVER_PRODUCT_NUM),

  .bNumConfigurations = 1,
};

#ifdef CONFIG_USB_OTG
struct usb_otg_descriptor otg_descriptor = {
  .bLength =    sizeof otg_descriptor,
  .bDescriptorType =  USB_DT_OTG,

  /* REVISIT SRP-only hardware is possible, although
   * it would not be called "OTG" ...
   */
  .bmAttributes =   USB_OTG_SRP | USB_OTG_HNP,
};

const struct usb_descriptor_header *otg_desc[] = {
  (struct usb_descriptor_header *) &otg_descriptor,
  NULL,
};
#endif

/* string IDs are assigned dynamically */

#define STRING_MANUFACTURER_IDX   0
#define STRING_PRODUCT_IDX    1
#define STRING_SERIAL_IDX   2

static char manufacturer[] = "LEGO Group";
static char serial[] = "123456789ABC ";
static char longname[] = "EV3 brick    ";

static struct usb_string strings_dev[3] = {
  [STRING_MANUFACTURER_IDX].s = manufacturer,
  [STRING_PRODUCT_IDX].s = longname,
  [STRING_SERIAL_IDX].s = serial
};


static struct usb_gadget_strings stringtab_dev = {
  .language = 0x0409, /* en-us */
  .strings  = strings_dev,
};

static struct usb_gadget_strings *dev_strings[] = {
  &stringtab_dev,
  NULL,
};

/*-------------------------------------------------------------------------*/

struct usb_request *alloc_ep_req(struct usb_ep *ep)
{
  struct usb_request  *req;

  req = usb_ep_alloc_request(ep, GFP_ATOMIC);
  if (req) {
    req->length = buflen;
    req->buf = kmalloc(buflen, GFP_ATOMIC);
    if (!req->buf) {
      usb_ep_free_request(ep, req);
      req = NULL;
    }
  }
  return req;
}

void free_ep_req(struct usb_ep *ep, struct usb_request *req)
{
  kfree(req->buf);
  usb_ep_free_request(ep, req);
}

static void disable_ep(struct usb_composite_dev *cdev, struct usb_ep *ep)
{
  int     value;

  if (ep->driver_data) {
    value = usb_ep_disable(ep);
    if (value < 0)
      DBG(cdev, "disable %s --> %d\n",
          ep->name, value);
    ep->driver_data = NULL;
  }
}

void disable_endpoints(struct usb_composite_dev *cdev,
    struct usb_ep *in, struct usb_ep *out)
{
  disable_ep(cdev, in);
  disable_ep(cdev, out);
}

/*-------------------------------------------------------------------------*/

static struct timer_list  autoresume_timer;

static void zero_autoresume(unsigned long _c)
{
  struct usb_composite_dev  *cdev = (void *)_c;
  struct usb_gadget   *g = cdev->gadget;

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("zero_autoresume\n\r");
  #endif

  /* unconfigured devices can't issue wakeups */
  if (!cdev->config)
    return;

  /* Normally the host would be woken up for something
   * more significant than just a timer firing; likely
   * because of some direct user request.
   */
  if (g->speed != USB_SPEED_UNKNOWN) {
    int status = usb_gadget_wakeup(g);
    INFO(cdev, "%s --> %d\n", __func__, status);
  }
}

static void zero_suspend(struct usb_composite_dev *cdev)
{
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("zero_suspend\n\r");
  #endif

  if (cdev->gadget->speed == USB_SPEED_UNKNOWN)
    return;

  if (autoresume) {
    mod_timer(&autoresume_timer, jiffies + (HZ * autoresume));
    DBG(cdev, "suspend, wakeup in %d seconds\n", autoresume);
  } else
    DBG(cdev, "%s\n", __func__);
}

static void zero_resume(struct usb_composite_dev *cdev)
{
  DBG(cdev, "%s\n", __func__);

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("zero_resume\n\r");
  #endif

  del_timer(&autoresume_timer);
}

/*-------------------------------------------------------------------------*/

static int zero_bind(struct usb_composite_dev *cdev)
{
  int     gcnum;
  struct usb_gadget *gadget = cdev->gadget;
  int     id;

  /* Allocate string descriptor numbers ... note that string
   * contents can be overridden by the composite_dev glue.
   */
  id = usb_string_id(cdev);
  if (id < 0)
    return id;

  strings_dev[STRING_MANUFACTURER_IDX].id = id;
  device_desc.iManufacturer = id;

  id = usb_string_id(cdev);
  if (id < 0)
    return id;


  strings_dev[STRING_PRODUCT_IDX].id = id;
  device_desc.iProduct = id;

  id = usb_string_id(cdev);
  if (id < 0)
    return id;


  strings_dev[STRING_SERIAL_IDX].id = id;
  device_desc.iSerialNumber = id;

  setup_timer(&autoresume_timer, zero_autoresume, (unsigned long) cdev);

  rudolf_add(cdev, autoresume != 0);

  gcnum = usb_gadget_controller_number(gadget);
  if (gcnum >= 0)
    device_desc.bcdDevice = cpu_to_le16(0x0200 + gcnum);
  else {
    /* gadget zero is so simple (for now, no altsettings) that
     * it SHOULD NOT have problems with bulk-capable hardware.
     * so just warn about unrcognized controllers -- don't panic.
     *
     * things like configuration and altsetting numbering
     * can need hardware-specific attention though.
     */
    pr_warning("%s: controller '%s' not recognized\n",
      longname, gadget->name);
    device_desc.bcdDevice = cpu_to_le16(0x9999);
  }
  return 0;
}

static int zero_unbind(struct usb_composite_dev *cdev)
{
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("zero_unbind\n\r");
  #endif

  del_timer_sync(&autoresume_timer);
  return 0;
}

static struct usb_composite_driver zero_driver = {
  .name   = "zero",
  .dev    = &device_desc,
  .strings  = dev_strings,
  .bind   = zero_bind,
  .unbind   = zero_unbind,
  .suspend  = zero_suspend,
  .resume   = zero_resume,
};

static int dUsbInit(void)
{
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("dUsbInit\n\r");
  #endif

  UsbSpeed.Speed = FULL_SPEED;          // default to FULL_SPEED if not connected to a HIGH-SPEED
  (*pUsbSpeed).Speed = FULL_SPEED;      // HOST. If not connected to HIGH-SPEED we assume we're
                                        // wanting (or at least doing) Daisy Chain
  return usb_composite_register(&zero_driver);
}

static void dUsbExit(void)
{
  usb_composite_unregister(&zero_driver);
}

// DEVICE1 char device stuff ********************************************************************

static ssize_t Device1Write(struct file *File,const char *Buffer,size_t Count,loff_t *Data)
{
  // Write data for the HOST to poll - Stuff sent to the HOST

  int BytesWritten = 0;

  #undef DEBUG
  //#define DEBUG
  #ifdef DEBUG
    printk("Device1Write - usb_char_in_length = %d\n", usb_char_in_length);
  #endif

  if (usb_char_in_length == 0)  // ready for more
  {                             // else wait in USER layer
      BytesWritten = Count;
      copy_from_user(usb_char_buffer_in, Buffer, BytesWritten);
      usb_char_in_length = BytesWritten;

	  //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      	  printk("WR = %d, %d -- ", usb_char_buffer_in[2], usb_char_buffer_in[3]);
	  #endif

      if(USB_DATA_PENDING == input_state)
      {
        // Already we've a failed tx (HOST part starwing??

        input_state = USB_DATA_READY;
        #undef DEBUG
        //#define DEBUG
        #ifdef DEBUG
          printk("DATA_PENDING SECOND time and reset!! in Device1Write\n\r");
        #endif
      }

      if(USB_DATA_READY == input_state)
      {
        #undef DEBUG
        //#define DEBUG
        #ifdef DEBUG
          printk("USB_DATA_READY in Device1Write\n\r");
        #endif

        input_state = USB_DATA_BUSY;
        write_data_to_the_host(save_in_ep, save_in_req);
        usb_req_arm(save_in_ep, save_in_req); // new request
      }
      else
      {
        input_state = USB_DATA_PENDING;

        #undef DEBUG
        //#define DEBUG
        #ifdef DEBUG
          printk("DATA_PENDING in Device1Write\n\r");
        #endif
      }
  }

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("usbdev %d written\n\r", BytesWritten);
  #endif
 
  return (BytesWritten); // Zero means USB was not ready yet
}

static ssize_t Device1Read(struct file *File,char *Buffer,size_t Count,loff_t *Offset)
{
  // Read the bits'n'bytes from the HOST
  int     BytesRead     = 0;

  if (usb_char_out_length > 0)     // Something to look at
  {
    #undef DEBUG
    //#define DEBUG
	  #ifdef DEBUG
	    printk("Some bytes to READ?\n\r");
    #endif

	  copy_to_user(Buffer, usb_char_buffer_out, Count);
    BytesRead = usb_char_out_length;
    usb_char_out_length = 0;
  }
  return (BytesRead);
}

static int Device1Mmap(struct file *filp, struct vm_area_struct *vma)
{
   int ret;

   ret = remap_pfn_range(vma,vma->vm_start,virt_to_phys((void*)((unsigned long)pUsbSpeed)) >> PAGE_SHIFT,vma->vm_end-vma->vm_start,PAGE_SHARED);

   if (ret != 0)
   {
     ret  =  -EAGAIN;
   }

   return (ret);
}
static    const struct file_operations Device1Entries =
{
  .owner        = THIS_MODULE,
  .read         = Device1Read,
  .write        = Device1Write,
  .mmap         = Device1Mmap
};


static    struct miscdevice Device1 =
{
  MISC_DYNAMIC_MINOR,
  DEVICE1_NAME,
  &Device1Entries
};


static int Device1Init(void)
{
  int     Result = -1;
  UWORD   *pTemp;
  int i;

  Result  =  misc_register(&Device1);
  if (Result)
  {
    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("  %s device register failed\n",DEVICE1_NAME);
    #endif
  }
  else
  {
    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("  %s device register OK\n",DEVICE1_NAME);
    #endif

    // allocate kernel shared memory for DaisyChain Speed info

    if ((kmalloc_ptr = kmalloc((NPAGES + 2) * PAGE_SIZE, GFP_KERNEL)) != NULL)
    {

      pTemp = (UWORD*)((((unsigned long)kmalloc_ptr) + PAGE_SIZE - 1) & PAGE_MASK);

      for (i = 0; i < NPAGES * PAGE_SIZE; i += PAGE_SIZE)
      {
        SetPageReserved(virt_to_page(((unsigned long)pTemp) + i));
      }

      pUsbSpeed = (USB_SPEED*)pTemp;
    }

    dUsbInit();
  }

  return (Result);
}

static void Device1Exit(void)
{
  int i;
  UWORD *pTemp = (UWORD*)pUsbSpeed;

  dUsbExit();

  pUsbSpeed = &UsbSpeedDefault;

  for (i = 0; i < NPAGES * PAGE_SIZE; i+= PAGE_SIZE)
  {
    ClearPageReserved(virt_to_page(((unsigned long)pTemp) + i));

    //#define DEBUG
    #undef DEBUG
    #ifdef DEBUG
      printk("  %s memory page %d unmapped\n",DEVICE1_NAME,i);
    #endif
  }

  kfree(kmalloc_ptr);

  misc_deregister(&Device1);

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("  %s device unregistered\n",DEVICE1_NAME);
  #endif
}


// MODULE *********************************************************************

char *HostStr;    // Used for HostName - or NOT used at all
char *SerialStr;  // Used for Serial number (I.e. BT number)

module_param (HostStr, charp, 0);
module_param (SerialStr, charp, 0);

static int ModuleInit(void)
{

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("%s Module init started\r\n",MODULE_NAME);
  #endif

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("This is DEFAULT NAME: %s\n\r", longname);
  #endif

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("\n\rThis is the HostStr: %s\n\r", HostStr);
  #endif

  strcpy(longname, HostStr);

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("\n\rThis is the INSMODed NAME: %s\n\r", longname);
  #endif

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("\n\rThis is the DEFAULT SerialNumber: %s\n\r", serial);
  #endif

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("\n\rThis is the SerialStr: %s\n\r", SerialStr);
  #endif

  strcpy(serial, SerialStr);

  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("\n\rThis is the INSMODed SerialNumber (BT mac): %s\n\r", serial);
  #endif

  Device1Init();

  return (0);
}

static void ModuleExit(void)
{
  //#define DEBUG
  #undef DEBUG
  #ifdef DEBUG
    printk("%s exit started\n",MODULE_NAME);
  #endif

  Device1Exit();

}

