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
 * This UsbFunction file is based on and inheritated from
 * the original file (f_sourcesink.c) and work done by
 * David Brownell
 *
 * >> f_sourcesink.c - USB peripheral source/sink configuration driver <<
 *
 * >> Copyright (C) 2003-2008 David Brownell <<
 * >> Copyright (C) 2008 by Nokia Corporation <<
 *
 */

/*
 *
 * HID IN/OUT Interrupt transfer FUNCTION ...
 *
 */

struct f_rudolf {
  struct usb_function function;

  struct usb_ep   *in_ep;
  struct usb_ep   *out_ep;
};

enum            // Used for signaling the IN stuff USB-state
{               // Data from the Brick to the HOST
  USB_DATA_IDLE,         //
  USB_DATA_BUSY,         // Ongoing USB request
  USB_DATA_PENDING,      // Data ready for X-fer, but USB busy
  USB_DATA_READY,         // Initial setting
};

int input_state = USB_DATA_IDLE;
struct usb_ep *save_in_ep;
struct usb_request *save_in_req;

#ifndef   PCASM
static inline struct f_rudolf *func_to_rudolf(struct usb_function *f)
{
  return container_of(f, struct f_rudolf, function);
}
#else
// Keep Eclipse happy
#endif

static struct usb_interface_descriptor rudolf_intf = {
  .bLength                    =     sizeof rudolf_intf,
  .bDescriptorType            =     USB_DT_INTERFACE,
  .bInterfaceNumber           =     0,
  .bAlternateSetting          =     0,
  .bNumEndpoints              =     2,                // Just plain in and out
  .bInterfaceClass            =     USB_CLASS_HID,    // We go for NONE custom-driver
  .bInterfaceSubClass         =     0,
  .bInterfaceProtocol         =     0,
  /* .iInterface              =     DYNAMIC */
};

static struct hid_descriptor hs_hid_rudolf_desc = {
  .bLength                    =     sizeof hs_hid_rudolf_desc,
  .bDescriptorType            =     HID_DT_HID,
  .bcdHID                     =     cpu_to_le16(0x0110),
  .bCountryCode               =     0x00,
  .bNumDescriptors            =     0x01,             // "The one and only"
  .desc[0].bDescriptorType    =     0x22,             // Report Descriptor Type - 0x22 = HID
  .desc[0].wDescriptorLength  =     sizeof hs_hid_report_descriptor,
  /*.desc[0].bDescriptorType  =     DYNAMIC */
  /*.desc[0].wDescriptorLenght=     DYNAMIC */
};

static struct hid_descriptor fs_hid_rudolf_desc = {
  .bLength                    =     sizeof fs_hid_rudolf_desc,
  .bDescriptorType            =     HID_DT_HID,
  .bcdHID                     =     cpu_to_le16(0x0110),
  .bCountryCode               =     0x00,
  .bNumDescriptors            =     0x01,             // "The one and only"
  .desc[0].bDescriptorType    =     0x22,             // Report Descriptor Type - 0x22 = HID
  .desc[0].wDescriptorLength  =     sizeof fs_hid_report_descriptor,

};

/* full speed support: */

static struct usb_endpoint_descriptor rudolf_out_fs_desc = {
  .bLength                    =     USB_DT_ENDPOINT_SIZE,
  .bDescriptorType            =     USB_DT_ENDPOINT,
  .bEndpointAddress           =     USB_DIR_OUT,
  .bmAttributes               =     USB_ENDPOINT_XFER_INT,
  .wMaxPacketSize             =     cpu_to_le16(64),
  .bInterval                  =     1, /* 1 = 1 mSec POLL rate for FS */
};

static struct usb_endpoint_descriptor rudolf_in_fs_desc = {
  .bLength                    =     USB_DT_ENDPOINT_SIZE,
  .bDescriptorType            =     USB_DT_ENDPOINT,
  .bEndpointAddress           =     USB_DIR_IN,
  .bmAttributes               =     USB_ENDPOINT_XFER_INT,
  .wMaxPacketSize             =     cpu_to_le16(64),
  .bInterval                  =     1, /* 1 = 1 mSec POLL rate for FS */
};

/* high speed support: */

static struct usb_endpoint_descriptor rudolf_in_hs_desc = {
  .bLength                    =     USB_DT_ENDPOINT_SIZE,
  .bDescriptorType            =     USB_DT_ENDPOINT,
  .bEndpointAddress           =     USB_DIR_IN,
  .bmAttributes               =     USB_ENDPOINT_XFER_INT,
  .wMaxPacketSize             =     cpu_to_le16(1024),
  .bInterval                  =     4, /* Calculated as :
                                        * 2^(value-1) * 125uS
                                        * i.e. value 1: 2^(1-1) * 125 uSec = 125 uSec
                                        *      -     4: 2^(4-1) * 125 uSec = 1 mSec
                                        */
};

static struct usb_endpoint_descriptor rudolf_out_hs_desc = {
  .bLength                    =     USB_DT_ENDPOINT_SIZE,
  .bDescriptorType            =     USB_DT_ENDPOINT,
  .bEndpointAddress           =     USB_DIR_OUT,
  .bmAttributes               =     USB_ENDPOINT_XFER_INT,
  .wMaxPacketSize             =     cpu_to_le16(1024),
  .bInterval                  =     4,  /* Calculated as :
                                         * 2^(value-1) * 125uS
                                         * i.e. value 1: 2^(1-1) * 125 uSec = 125 uSec
                                         *      -     4: 2^(4-1) * 125 uSec = 1 mSec
                                         */
};

static struct usb_descriptor_header *hs_rudolf_descs[] = {
  (struct usb_descriptor_header *) &rudolf_intf,
  (struct usb_descriptor_header *) &hs_hid_rudolf_desc,
  (struct usb_descriptor_header *) &rudolf_in_hs_desc,
  (struct usb_descriptor_header *) &rudolf_out_hs_desc,
  NULL,
};

static struct usb_descriptor_header *fs_rudolf_descs[] = {
  (struct usb_descriptor_header *) &rudolf_intf,
  (struct usb_descriptor_header *) &fs_hid_rudolf_desc,
  (struct usb_descriptor_header *) &rudolf_in_fs_desc,
  (struct usb_descriptor_header *) &rudolf_out_fs_desc,
  NULL,
};

/* function-specific strings: */

static struct usb_string strings_rudolf[] = {
  [0].s = "Xfer data to and from EV3 brick",
  {  }      /* end of list */
};

static struct usb_gadget_strings stringtab_rudolf = {
  .language = 0x0409, /* en-us */
  .strings  = strings_rudolf,
};

static struct usb_gadget_strings *rudolf_strings[] = {
  &stringtab_rudolf,
  NULL,
};

/*-------------------------------------------------------------------------*/

static int
f_rudolf_bind(struct usb_configuration *c, struct usb_function *f)
{
  struct usb_composite_dev  *cdev = c->cdev;
  struct f_rudolf     *rudolf = func_to_rudolf(f);
  int       id;

  /* allocate interface ID(s) */
  id = usb_interface_id(c, f);
  if (id < 0)
    return id;
  rudolf_intf.bInterfaceNumber = id;

  /* allocate endpoints */
  rudolf->in_ep = usb_ep_autoconfig(cdev->gadget, &rudolf_in_fs_desc);
  if (!rudolf->in_ep) {
autoconf_fail:
    ERROR(cdev, "%s: can't autoconfigure on %s\n",
      f->name, cdev->gadget->name);
    return -ENODEV;
  }
  rudolf->in_ep->driver_data = cdev;  /* claim */

  rudolf->out_ep = usb_ep_autoconfig(cdev->gadget, &rudolf_out_fs_desc);
  if (!rudolf->out_ep)
    goto autoconf_fail;
  rudolf->out_ep->driver_data = cdev; /* claim */


  /* support high speed hardware */
  if (gadget_is_dualspeed(c->cdev->gadget)) {

    rudolf_in_hs_desc.bEndpointAddress =
        rudolf_in_fs_desc.bEndpointAddress;
    rudolf_out_hs_desc.bEndpointAddress =
        rudolf_out_fs_desc.bEndpointAddress;
    f->hs_descriptors = hs_rudolf_descs;
    f->descriptors = fs_rudolf_descs;
  }

  DBG(cdev, "%s speed %s: IN/%s, OUT/%s\n",
      gadget_is_dualspeed(c->cdev->gadget) ? "dual" : "full",
      f->name, rudolf->in_ep->name, rudolf->out_ep->name);
  return 0;
}

static void
f_rudolf_unbind(struct usb_configuration *c, struct usb_function *f)
{
  kfree(func_to_rudolf(f));
}

static void usb_req_arm(struct usb_ep *ep, struct usb_request *req)
{
  int status;

  if (UsbSpeed.Speed == FULL_SPEED)
   {
       req->length = 64;
       req->actual = 64;
   }
   else
   {
       req->length = 1024;
       req->actual = 1024;
   }

  status = usb_ep_queue(ep, req, GFP_ATOMIC);
  if (status) {
    usb_ep_set_halt(ep);
    /* FIXME recover later ... somehow */
  }
}

static int read_data_from_host(struct usb_request *req)
{
  unsigned      i;
  u8        *buf = req->buf;

  int from_host_length = 0;  // NO ACCESS LOCKS YET

  // test for actual length > 0

  for (i = 0; i < req->actual; i++, buf++)
  {

      usb_char_buffer_out[i] = *buf;
      from_host_length++;
  }

  return (from_host_length);
}

static void write_data_to_the_host(struct usb_ep *ep, struct usb_request *req)
{
  unsigned  i;
  u8    *buf = req->buf;

  //#define DEBUG
  #ifdef DEBUG
    printk("WR to HOST req->length = %d\r\n", req->length);
  #endif

  #undef DEBUG
  //#define DEBUG
      #ifdef DEBUG
        	printk("USB = %d, %d\r\n", usb_char_buffer_in[2], usb_char_buffer_in[3]);
  #endif
  #undef DEBUG

  for (i = 0; i < req->length; i++)
    *buf++ = usb_char_buffer_in[i];
    usb_char_in_length = 0; // Reset and ready
}

static void rudolf_complete(struct usb_ep *ep, struct usb_request *req)
{
  struct f_rudolf *rudolf = ep->driver_data;
  int     status = req->status;


  switch ( status ) {

  case 0:       /* normal completion? */
    if (ep == rudolf->out_ep) // An OUT completion?
    {
      //#define DEBUG
      #ifdef DEBUG
        printk("Rudolf_complete OUT\n");
      #endif

      usb_char_out_length = read_data_from_host(req);
      usb_req_arm(ep, req);
    }
    else  // We have an INPUT request complete
    {
      //#define DEBUG
      #ifdef DEBUG
        printk("Rudolf_complete IN\n");
      #endif

      switch(input_state) // State of Brick data x-fer
      {
        case  USB_DATA_READY:     //should be BUSY or PENDING....

                                  #ifdef DEBUG
                                    printk("IN_IN_IN - READY ?????\n");
                                  #endif

                                  break;

        case  USB_DATA_PENDING:   //
                             //     #define DEBUG
                                  #ifdef DEBUG
                                    printk("IN_IN_IN - PENDING settes to BUSY\n");
                                  #endif

                                  input_state = USB_DATA_BUSY;
                                  write_data_to_the_host(ep, req);
                                  usb_req_arm(ep, req); // new request
                                  break;

        case  USB_DATA_BUSY:      //
                                  #ifdef DEBUG
                                    printk("IN_IN_IN - BUSY settes to READY\n");
                                  #endif
                                  #undef DEBUG
                                  input_state = USB_DATA_READY;
                                  // and relax
                                  break;

        case  USB_DATA_IDLE:      // too lazy
                                  #ifdef DEBUG
                                    printk("IN_IN_IN - IDLE\n");
                                  #endif
                                  //#undef DEBUG

                                  break;

        default:                  break;  // hmmm.
      }
              // Reset the buffer size - Ready again
              usb_char_in_length = 0;
    }
      break;

  /* this endpoint is normally active while we're configured */

  case -ESHUTDOWN:      /* disconnect from host */
                        // REMOVED 26102012 (*pUsbSpeed).Speed = FULL_SPEED;
  case -ECONNABORTED:   /* hardware forced ep reset */
  case -ECONNRESET:     /* request dequeued */
  //case -ESHUTDOWN:      /* disconnect from host */
        if (ep == rudolf->out_ep)
          read_data_from_host(req);
        free_ep_req(ep, req);
        return;

  case -EOVERFLOW:    /* buffer overrun on read means that
                         we didn't provide a big enough
                         buffer.
                      */
  default:
//#if 1
  //  DBG(cdev, "%s complete --> %d, %d/%d\n", ep->name,
    //    status, req->actual, req->length);
//#endif
  case -EREMOTEIO:    /* short read */
    break;
  }

}

static int rudolf_start_ep(struct f_rudolf *rudolf, bool is_in)
{
  struct usb_ep   *ep;
  struct usb_request  *req;
  int     status;

  ep = is_in ? rudolf->in_ep : rudolf->out_ep;
  req = alloc_ep_req(ep);
  if (!req)
    return -ENOMEM;

  req->complete = rudolf_complete;

  #ifdef DEBUG
    printk("UsbSpeed.Speed = %d\n\r", UsbSpeed.Speed);
  #endif

  if (UsbSpeed.Speed == FULL_SPEED)
   {
     #ifdef DEBUG
       printk("rudolf_start_ep FULL\n\r");
     #endif

     (*pUsbSpeed).Speed = FULL_SPEED;
     req->length = 64;    // Full speed max buffer size
     req->actual = 64;
   }
   else
   {
     #ifdef DEBUG
       printk("rudolf_start_ep HIGH\n\r");
     #endif

     (*pUsbSpeed).Speed = HIGH_SPEED;
     req->length = 1024;  // High speed max buffer size
     req->actual = 1024;
   }

  if (is_in)
  {
    save_in_ep = ep;
    save_in_req = req;

    #ifdef DEBUG
      printk("req->length = %d ***** Rudolf_Start_Ep_in\n\r", req->length);
    #endif

   // reinit_write_data(ep, req);
    input_state = USB_DATA_BUSY;
  }
  else
  {
    #ifdef DEBUG
      printk("***** Rudolf_Start_Ep_out\n");
    #endif
  }

  status = usb_ep_queue(ep, req, GFP_ATOMIC);

  if (status) {
    struct usb_composite_dev  *cdev;

    cdev = rudolf->function.config->cdev;
    ERROR(cdev, "start %s %s --> %d\n",
        is_in ? "IN" : "OUT",
        ep->name, status);

    free_ep_req(ep, req);
  }

  return status;
}

static void disable_rudolf(struct f_rudolf *rudolf)
{
  struct usb_composite_dev  *cdev;

  cdev = rudolf->function.config->cdev;
  disable_endpoints(cdev, rudolf->in_ep, rudolf->out_ep);
  VDBG(cdev, "%s disabled\n", rudolf->function.name);
}

static int
enable_rudolf(struct usb_composite_dev *cdev, struct f_rudolf *rudolf)
{
  int         result = 0;
  const struct usb_endpoint_descriptor  *ep_in, *ep_out;
  struct usb_ep       *ep;

  ep_in = ep_choose(cdev->gadget, &rudolf_in_hs_desc, &rudolf_in_fs_desc);

  ep_out = ep_choose(cdev->gadget, &rudolf_out_hs_desc, &rudolf_out_fs_desc);

  ep = rudolf->in_ep;

  result = usb_ep_enable(ep, ep_in);

  if (result < 0)
    return result;

  ep->driver_data = rudolf;

  result = rudolf_start_ep(rudolf, true);

  if (result < 0) {
fail:
    ep = rudolf->in_ep;
    usb_ep_disable(ep);
    ep->driver_data = NULL;
    return result;
  }

  /* one endpoint reads (sinks) anything OUT (from the host) */
  ep = rudolf->out_ep;
  result = usb_ep_enable(ep, ep_out);

  if (result < 0)
    goto fail;

  ep->driver_data = rudolf;

  result = rudolf_start_ep(rudolf, false);

  if (result < 0) {
    usb_ep_disable(ep);
    ep->driver_data = NULL;
    goto fail;
  }

  DBG(cdev, "%s enabled\n", rudolf->function.name);

  return result;
}

static int f_rudolf_set_alt(struct usb_function *f,
    unsigned intf, unsigned alt)
{
  struct f_rudolf     *rudolf = func_to_rudolf(f);
  struct usb_composite_dev  *cdev = f->config->cdev;

  /* we know alt is zero */
  if (rudolf->in_ep->driver_data)
    disable_rudolf(rudolf);

  return enable_rudolf(cdev, rudolf);
}

static void f_rudolf_disable(struct usb_function *f)
{
  struct f_rudolf   *rudolf = func_to_rudolf(f);

  disable_rudolf(rudolf);
}

/*-------------------------------------------------------------------------*/

static int rudolf_bind_config(struct usb_configuration *c)
{
  struct f_rudolf   *rudolf;
  int     status;

  rudolf = kzalloc(sizeof *rudolf, GFP_KERNEL);
  if (!rudolf)
    return -ENOMEM;

  rudolf->function.name = "rudolf xfer";
  rudolf->function.descriptors = hs_rudolf_descs;
  rudolf->function.bind = f_rudolf_bind;
  rudolf->function.unbind = f_rudolf_unbind;
  rudolf->function.set_alt = f_rudolf_set_alt;
  rudolf->function.disable = f_rudolf_disable;

  status = usb_add_function(c, &rudolf->function);
  if (status)
    kfree(rudolf);
  return status;
}

#ifndef   PCASM
static int rudolf_setup(struct usb_configuration *c,
    const struct usb_ctrlrequest *ctrl)
{
  struct usb_request  *req = c->cdev->req;
  int     value = -EOPNOTSUPP;
  u16     w_index = le16_to_cpu(ctrl->wIndex);
  u16     w_value = le16_to_cpu(ctrl->wValue);
  u16     w_length = le16_to_cpu(ctrl->wLength);
  u16     length = 0;

  /* composite driver infrastructure handles everything except
   * the two control test requests.
   */

  switch (ctrl->bRequest) {

  /*
   * These are the same vendor-specific requests supported by
   * Intel's USB 2.0 compliance test devices.  We exceed that
   * device spec by allowing multiple-packet requests.
   *
   * NOTE:  the Control-OUT data stays in req->buf ... better
   * would be copying it into a scratch buffer, so that other
   * requests may safely intervene.
   */

  case 0x5b:  /* control WRITE test -- fill the buffer */
    if (ctrl->bRequestType != (USB_DIR_OUT|USB_TYPE_VENDOR))
      goto unknown;
    if (w_value || w_index)
      break;
    /* just read that many bytes into the buffer */
    if (w_length > req->length)
      break;
    value = w_length;
    break;
  case 0x5c:  /* control READ test -- return the buffer */
    if (ctrl->bRequestType != (USB_DIR_IN|USB_TYPE_VENDOR))
      goto unknown;
    if (w_value || w_index)
      break;
    /* expect those bytes are still in the buffer; send back */
    if (w_length > req->length)
      break;
    value = w_length;
    break;

  default:
unknown:
    VDBG(c->cdev,
      "unknown control req%02x.%02x v%04x i%04x l%d\n",
      ctrl->bRequestType, ctrl->bRequest,
      w_value, w_index, w_length);
  }

  //HER SKAL HID DESC SENDES!!!
  switch ((ctrl->bRequestType << 8) | ctrl->bRequest) {


  case ((USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_INTERFACE) << 8
      | USB_REQ_GET_DESCRIPTOR):
    switch (w_value >> 8) {
    case HID_DT_REPORT:
      //VDBG(cdev, "USB_REQ_GET_DESCRIPTOR: REPORT\n");
      length = w_length;
      length = min_t(unsigned short, length,
               sizeof hs_hid_report_descriptor);
      memcpy(req->buf, hs_hid_report_descriptor, length);
      value = length;
      goto respond;
      break;

    default:
      //VDBG(cdev, "Unknown decriptor request 0x%x\n",
      //   value >> 8);
      goto stall;
      break;
    }
    break;
  default:
    //VDBG(cdev, "Unknown request 0x%x\n",
    //   ctrl->bRequest);
    goto stall;
    break;
  }

//HERTIL
  /* respond with data transfer or status phase? */
stall:
  return -EOPNOTSUPP;

respond:
  if (value >= 0) {
    VDBG(c->cdev, "source/sink req%02x.%02x v%04x i%04x l%d\n",
      ctrl->bRequestType, ctrl->bRequest,
      w_value, w_index, w_length);
    req->zero = 0;
    req->length = value;
    value = usb_ep_queue(c->cdev->gadget->ep0, req, GFP_ATOMIC);
    if (value < 0)
      ERROR(c->cdev, "source/sinkc response, err %d\n",
          value);
  }

  /* device either stalls (value < 0) or reports success */
  return value;
}
#else
// Keep Eclipse happy
#endif

static struct usb_configuration rudolf_driver = {
  .label    = "rudolf driver",
  .strings  = rudolf_strings,
  .bind     = rudolf_bind_config,
  .setup    = rudolf_setup,
  .bConfigurationValue = 1,
  .bmAttributes = USB_CONFIG_ATT_SELFPOWER,
  /* .iConfiguration = DYNAMIC */
};

/**
 *
 */

int rudolf_add(struct usb_composite_dev *cdev, bool autoresume)
{
  int id;

  /* allocate string ID(s) */
  id = usb_string_id(cdev);
  if (id < 0)
    return id;
  strings_rudolf[0].id = id;

  rudolf_intf.iInterface = id;
  rudolf_driver.iConfiguration = 1; // id;

  /* support autoresume for remote wakeup testing */
  if (autoresume)
    rudolf_driver.bmAttributes |= USB_CONFIG_ATT_WAKEUP;

  /* support OTG systems */
  if (gadget_is_otg(cdev->gadget)) {
    rudolf_driver.descriptors = otg_desc;
    rudolf_driver.bmAttributes |= USB_CONFIG_ATT_WAKEUP;
  }

  return usb_add_config(cdev, &rudolf_driver);
}
