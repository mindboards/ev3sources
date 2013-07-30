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


#include  "lms2012.h"
#include  "c_ui.h"
#include  "d_lcd.h"

#if       (HARDWARE != SIMULATION)

#include  <stdio.h>
#include  <string.h>
#include  <math.h>
#include  <fcntl.h>
#include  <unistd.h>

#endif



#ifndef   Linux_X86


#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <stdlib.h>
#include <errno.h>
#include <endian.h>
#include <linux/fb.h>


#define FBCTL(cmd, arg)                 \
        if(ioctl(UiInstance.DispFile, cmd, arg) == -1) { \
                LogErrorNumber(LCD_DEVICE_FILE_NOT_FOUND); }

struct fb_var_screeninfo var;
struct fb_fix_screeninfo fix;
int dll, fll;
unsigned char *dbuf=NULL;


UBYTE     PixelTab[] =
{
    0x00, // 000 00000000
    0xE0, // 001 11100000
    0x1C, // 010 00011100
    0xFC, // 011 11111100
    0x03, // 100 00000011
    0xE3, // 101 11100011
    0x1F, // 110 00011111
    0xFF  // 111 11111111
};


void      dLcdExec(LCD *pDisp)
{
  UBYTE   *pSrc;
  UBYTE   *pDst;
  ULONG   Pixels;
  UWORD   X;
  UWORD   Y;


  if (dbuf)
  {
    if (memcmp((const void*)pDisp,(const void*)&VMInstance.LcdBuffer,sizeof(LCD)) != 0)
    {
      pSrc  =  (*pDisp).Lcd;
      pDst  =  dbuf;

      for (Y = 0;Y < 128;Y++)
      {
        for (X = 0;X < 7;X++)
        {
          Pixels  =  (ULONG)*pSrc;
          pSrc++;
          Pixels |=  (ULONG)*pSrc << 8;
          pSrc++;
          Pixels |=  (ULONG)*pSrc << 16;
          pSrc++;

          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
          Pixels >>= 3;
          *pDst   =  PixelTab[Pixels & 0x07];
          pDst++;
        }
        Pixels  =  (ULONG)*pSrc;
        pSrc++;
        Pixels |=  (ULONG)*pSrc << 8;
        pSrc++;

        *pDst   =  PixelTab[Pixels & 0x07];
        pDst++;
        Pixels >>= 3;
        *pDst   =  PixelTab[Pixels & 0x07];
        pDst++;
        Pixels >>= 3;
        *pDst   =  PixelTab[Pixels & 0x07];
        pDst++;
        Pixels >>= 3;
        *pDst   =  PixelTab[Pixels & 0x07];
        pDst++;
      }

      LCDCopy(&UiInstance.LcdBuffer,&VMInstance.LcdBuffer,sizeof(LCD));
      VMInstance.LcdUpdated  =  1;
    }
  }
}


#ifdef MAX_FRAMES_PER_SEC
void      dLcdAutoUpdate(void)
{
  if (UiInstance.AllowUpdate)
  {
    if (UiInstance.DisplayUpdate)
    {
      dLcdExec(&UiInstance.LcdBuffer);
      UiInstance.DisplayUpdate  =  0;
      UiInstance.DisplayTimer   =  0;
      UiInstance.AllowUpdate    =  0;
    }
  }
}
#endif


void      dLcdUpdate(LCD *pDisp)
{
#ifdef MAX_FRAMES_PER_SEC
    LCDCopy(pDisp,&UiInstance.LcdBuffer,sizeof(LCD));
    UiInstance.DisplayUpdate  =  1;
    dLcdAutoUpdate();
#else
    dLcdExec(pDisp);
#endif
}


void      dLcdInit(UBYTE *pImage)
{
  UiInstance.DispFile = open(LCD_DEVICE_NAME, O_RDWR);
  if (UiInstance.DispFile < 0) LogErrorNumber(LCD_DEVICE_FILE_NOT_FOUND);

  ioctl(UiInstance.DispFile, _IOW('S',0, int), NULL);

  FBCTL(FBIOGET_VSCREENINFO, &var);
  FBCTL(FBIOGET_FSCREENINFO, &fix);

  /* Display line length in bytes */
  dll = fix.line_length;
  /* Image file line length in bytes */
  fll = (var.xres >> 3) + 1;

  dbuf = (unsigned char *)mmap(0, var.yres * dll, PROT_WRITE | PROT_READ, MAP_SHARED, UiInstance.DispFile, 0);
  if (dbuf == MAP_FAILED) LogErrorNumber(LCD_DEVICE_FILE_NOT_FOUND);
}


UBYTE     dLcdRead(void)
{
  return (0);
}


void      dLcdExit(void)
{
  if (UiInstance.DispFile >= MIN_HANDLE)
  {
    close(UiInstance.DispFile);
  }
}


#else

#include  <X11/Xlib.h>
#include  <X11/X.h>
#include  <X11/Xutil.h>
#include  <time.h>
#include  <unistd.h>
#include  <stdio.h>
#include  <X11/xpm.h>

#define   DISPLAY_LEFT    10
#define   DISPLAY_TOP     100
#define   DISPLAY_WIDTH   366
#define   DISPLAY_HEIGHT  557

#define   LCD_LEFT        93
#define   LCD_TOP         65

enum      UiButPins
{
  BUT0,     // UP
  BUT1,     // ENTER
  BUT2,     // DOWN
  BUT3,     // RIGHT
  BUT4,     // LEFT
  BUT5,     // BACK
  BUT_PINS
};


Display   *pDisplay;
Window    hWindow;
GC        hGC;
int       Background;
int       Foreground;
XEvent    Event;
Colormap  CM;
XColor    colour_bk;

XImage  *pImg,*pClp;

Pixmap    hLcd;

XImage    Image;


void      dLcdExec(LCD *pDisp)
{
  if (memcmp((const void*)pDisp,(const void*)&VMInstance.LcdBuffer,sizeof(LCD)) != 0)
  {
    XPutImage(pDisplay,hWindow,hGC,&Image,0,0,LCD_LEFT,LCD_TOP,LCD_WIDTH,LCD_HEIGHT);
    XFlush(pDisplay);
    usleep(25);

    LCDCopy(&UiInstance.LcdBuffer,&VMInstance.LcdBuffer,sizeof(LCD));
    VMInstance.LcdUpdated  =  1;
  }
}


#ifdef MAX_FRAMES_PER_SEC
void      dLcdAutoUpdate(void)
{
  if (UiInstance.AllowUpdate)
  {
    if (UiInstance.DisplayUpdate)
    {
      dLcdExec(&UiInstance.LcdBuffer);
      UiInstance.DisplayUpdate  =  0;
      UiInstance.DisplayTimer   =  0;
      UiInstance.AllowUpdate    =  0;
    }
  }
}
#endif


void      dLcdUpdate(LCD *pDisp)
{
#ifdef MAX_FRAMES_PER_SEC
    LCDCopy(pDisp,&UiInstance.LcdBuffer,sizeof(LCD));
    UiInstance.DisplayUpdate  =  1;
    dLcdAutoUpdate();
#else
    dLcdExec(pDisp);
#endif
}


void      dLcdInit(UBYTE *pImage)
{
  XSetWindowAttributes WindowAttributes;
  unsigned long WindowMask;
  int     Depth;
  char    Buffer[255];
  int     ScreenNo;

  pDisplay      =  XOpenDisplay(NULL);
  if (pDisplay != NULL)
  {
    Background  =  BlackPixel(pDisplay,DefaultScreen(pDisplay));
    Foreground  =  WhitePixel(pDisplay,DefaultScreen(pDisplay));

    Depth             =  DefaultDepth(pDisplay,DefaultScreen(pDisplay));
    ScreenNo          =  DefaultScreen(pDisplay);
    CM                =  XDefaultColormap(pDisplay,ScreenNo);
    colour_bk.flags   =  DoRed|DoGreen|DoBlue;
    colour_bk.red     =  32512;
    colour_bk.green   =  34304;
    colour_bk.blue    =  34560;

    WindowAttributes.border_pixel            =  Background;
    WindowAttributes.background_pixel        =  Foreground;
    WindowAttributes.override_redirect       =  1;
    WindowMask = CWBackPixel|CWBorderPixel|CWOverrideRedirect;
    hWindow = XCreateWindow(pDisplay,RootWindow(pDisplay,ScreenNo),DISPLAY_LEFT,DISPLAY_TOP,DISPLAY_WIDTH,DISPLAY_HEIGHT,0,Depth,InputOutput,CopyFromParent,WindowMask,&WindowAttributes);



    if (XAllocColor(pDisplay,CM,&colour_bk) == 0)
    {
      printf("XAllocColor failure.\n");
    }
    if (XSetWindowBackground(pDisplay,hWindow, colour_bk.pixel) == BadGC)
    {
      printf("XSetBackground failure.\n");
    }

    snprintf(Buffer,255,"%s V%4.2f",PROJECT,VERS);
    XSetStandardProperties(pDisplay,hWindow,Buffer,Buffer,None,NULL,0,NULL);

    XSelectInput(pDisplay,hWindow,StructureNotifyMask);
    XMapWindow(pDisplay,hWindow);

    hGC         =  XCreateGC(pDisplay,hWindow,0,NULL);
    XSetForeground(pDisplay,hGC,Background);
    XSetBackground(pDisplay,hGC,colour_bk.pixel);
    XFlush(pDisplay);

    do
    {
      XNextEvent(pDisplay,&Event);
    }
    while (Event.type != MapNotify);

    hLcd   =  XCreatePixmap(pDisplay,hWindow,LCD_WIDTH,LCD_HEIGHT,Depth);

    // Load body bitmap
    if (XpmReadFileToImage(pDisplay,"body.xpm",&pImg,&pClp,NULL) == 0)
    {
      XPutImage(pDisplay,hWindow,hGC,pImg,0,0,0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT);
    }
    XFlush(pDisplay);

    XSetForeground(pDisplay,hGC,Background);
    XSetBackground(pDisplay,hGC,colour_bk.pixel);

    XCopyArea(pDisplay,hLcd,hWindow,hGC,0,0,LCD_WIDTH,LCD_HEIGHT,LCD_LEFT,LCD_TOP);
    XFlush(pDisplay);

    Image.width             =  LCD_WIDTH;
    Image.height            =  LCD_HEIGHT;
    Image.xoffset           =  0;
    Image.format            =  XYBitmap;
    Image.data              =  (char*)pImage;
    Image.byte_order        =  LSBFirst;
    Image.bitmap_unit       =  8;
    Image.bitmap_bit_order  =  LSBFirst;
    Image.bitmap_pad        =  8;
    Image.depth             =  1;
    Image.bytes_per_line    =  (LCD_WIDTH + 7) / 8;
    Image.bits_per_pixel    =  1;

    XInitImage(&Image);

    LCDClear(pImage);

    XSelectInput(pDisplay,hWindow,ExposureMask | ButtonPressMask | ButtonReleaseMask);
  }
}


UBYTE     dLcdRead(void)
{
  UBYTE   Result = 0;
  int     X,Y;

  if (XCheckMaskEvent(pDisplay,ExposureMask | ButtonPressMask | ButtonReleaseMask,&Event))
  {
    //printf("%d %d\r\n",Event.type,Event.xbutton.button);

    switch (Event.type)
    {
      case ButtonPress :
      {
        X  =  Event.xbutton.x;
        Y  =  Event.xbutton.y;

        switch (Event.xbutton.button)
        {
          case Button1 :
          {
            //printf("%4d %4d\r\n",X,Y);

            if ((X >= 162) && (X <= 205) && (Y >= 288) && (Y <= 327))
            {
              UiInstance.ButtonState[BUT0]           |=  BUTTON_ACTIVE;
            }
            if ((X >= 162) && (X <= 205) && (Y >= 382) && (Y <= 421))
            {
              UiInstance.ButtonState[BUT2]           |=  BUTTON_ACTIVE;
            }
            if ((X >= 230) && (X <= 264) && (Y >= 338) && (Y <= 371))
            {
              UiInstance.ButtonState[BUT3]           |=  BUTTON_ACTIVE;
            }
            if ((X >= 98) && (X <= 137) && (Y >= 337) && (Y <= 370))
            {
              UiInstance.ButtonState[BUT4]           |=  BUTTON_ACTIVE;
            }
            if ((X >= 162) && (X <= 202) && (Y >= 333) && (Y <= 375))
            {
              UiInstance.ButtonState[BUT1]           |=  BUTTON_ACTIVE;
            }
            if ((X >= 51) && (X <= 120) && (Y >= 258) && (Y <= 295))
            {
              UiInstance.ButtonState[BUT5]           |=  BUTTON_ACTIVE;
            }
          }
          break;

          case Button3 :
          {
//            UiInstance.Event  =  vmEVENT_BT_PIN;
//            UiInstance.Event  =  vmEVENT_BT_REQ_CONF;
//            SetUiUpdate();
//            UiInstance.Warning |=  WARNING_TEMP;

            // printf("%4d %4d\r\n",X,Y);
          }
          break;

        }
      }
      break;

      case ButtonRelease :
      {
        X  =  Event.xbutton.x;
        Y  =  Event.xbutton.y;

        switch (Event.xbutton.button)
        {
          case Button1 :
          {
            //printf("%4d %4d\r\n",X,Y);

            if ((X >= 162) && (X <= 205) && (Y >= 288) && (Y <= 327))
            {
              UiInstance.ButtonState[BUT0]         &= ~BUTTON_ACTIVE;
            }
            if ((X >= 162) && (X <= 205) && (Y >= 382) && (Y <= 421))
            {
              UiInstance.ButtonState[BUT2]         &= ~BUTTON_ACTIVE;
            }
            if ((X >= 230) && (X <= 264) && (Y >= 338) && (Y <= 371))
            {
              UiInstance.ButtonState[BUT3]         &= ~BUTTON_ACTIVE;
            }
            if ((X >= 98) && (X <= 137) && (Y >= 337) && (Y <= 370))
            {
              UiInstance.ButtonState[BUT4]         &= ~BUTTON_ACTIVE;
            }
            if ((X >= 162) && (X <= 202) && (Y >= 333) && (Y <= 375))
            {
              UiInstance.ButtonState[BUT1]         &= ~BUTTON_ACTIVE;
            }
            if ((X >= 51) && (X <= 120) && (Y >= 258) && (Y <= 295))
            {
              UiInstance.ButtonState[BUT5]         &= ~BUTTON_ACTIVE;
            }
          }
          break;

        }
      }
      break;

    }
  }

  return (Result);
}


void      dLcdExit(void)
{
  if (pDisplay)
  {
    XFreePixmap(pDisplay,hLcd);
    XFreeGC(pDisplay,hGC);
    XCloseDisplay(pDisplay);
  }
}

#endif


void      dLcdScroll(UBYTE *pImage,DATA16 Y0)
{

  memmove(pImage,&pImage[((LCD_WIDTH + 7) / 8) * Y0],(LCD_HEIGHT - Y0) * ((LCD_WIDTH + 7) / 8));
  memset(&pImage[(LCD_HEIGHT - Y0) * ((LCD_WIDTH + 7) / 8)],0,((LCD_WIDTH + 7) / 8) * Y0);
}


void      dLcdDrawPixel(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0)
{
  if ((X0 >= 0) && (X0 < LCD_WIDTH) && (Y0 >= 0) && (Y0 < LCD_HEIGHT))
  {
    if (Color)
    {
      pImage[(X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3)]  |=  (1 << (X0 % 8));
    }
    else
    {
      pImage[(X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3)]  &= ~(1 << (X0 % 8));
    }
  }
}


void      dLcdInversePixel(UBYTE *pImage,DATA16 X0,DATA16 Y0)
{
  if ((X0 >= 0) && (X0 < LCD_WIDTH) && (Y0 >= 0) && (Y0 < LCD_HEIGHT))
  {
    pImage[(X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3)]  ^=  (1 << (X0 % 8));
  }
}


DATA8     dLcdReadPixel(UBYTE *pImage,DATA16 X0,DATA16 Y0)
{
  DATA8   Result = 0;

  if ((X0 >= 0) && (X0 < LCD_WIDTH) && (Y0 >= 0) && (Y0 < LCD_HEIGHT))
  {
    if ((pImage[(X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3)] & (1 << (X0 % 8))))
    {
      Result  =  1;
    }
  }

  return (Result);
}


void      dLcdDrawLine(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1)
{
  DATA16  XLength;
  DATA16  YLength;
  DATA16  XInc;
  DATA16  YInc;
  DATA16  Diff;
  DATA16  Tmp;

  if (X0 < X1)
  {
    XLength  =  X1 - X0;
    XInc     =  1;
  }
  else
  {
    XLength  =  X0 - X1;
    XInc     = -1;
  }
  if (Y0 < Y1)
  {
    YLength  =  Y1 - Y0;
    YInc     =  1;
  }
  else
  {
    YLength  =  Y0 - Y1;
    YInc     = -1;
  }
  Diff  =  XLength - YLength;

  dLcdDrawPixel(pImage,Color,X0,Y0);

  while ((X0 != X1) || (Y0 != Y1))
  {
    Tmp  =  Diff << 1;
    if (Tmp > (-YLength))
    {
      Diff -=  YLength;
      X0   +=  XInc;
    }
    if (Tmp < XLength)
    {
      Diff +=  XLength;
      Y0   +=  YInc;
    }
    dLcdDrawPixel(pImage,Color,X0,Y0);
  }
}


void      dLcdDrawDotLine(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1,DATA16 On,DATA16 Off)
{
  DATA16  XLength;
  DATA16  YLength;
  DATA16  XInc;
  DATA16  YInc;
  DATA16  Diff;
  DATA16  Tmp;
  DATA16  Count;

  if ((X0 != X1) && (Y0 != Y1))
  {
    dLcdDrawLine(pImage,Color,X0,Y0,X1,Y1);
  }
  else
  {
    if (X0 < X1)
    {
      XLength  =  X1 - X0;
      XInc     =  1;
    }
    else
    {
      XLength  =  X0 - X1;
      XInc     = -1;
    }
    if (Y0 < Y1)
    {
      YLength  =  Y1 - Y0;
      YInc     =  1;
    }
    else
    {
      YLength  =  Y0 - Y1;
      YInc     = -1;
    }
    Diff  =  XLength - YLength;

    dLcdDrawPixel(pImage,Color,X0,Y0);
    Count  =  1;

    while ((X0 != X1) || (Y0 != Y1))
    {
      Tmp  =  Diff << 1;
      if (Tmp > (-YLength))
      {
        Diff -=  YLength;
        X0   +=  XInc;
      }
      if (Tmp < XLength)
      {
        Diff +=  XLength;
        Y0   +=  YInc;
      }
      if (Count < (On + Off))
      {
        if (Count < On)
        {
          dLcdDrawPixel(pImage,Color,X0,Y0);
        }
        else
        {
          dLcdDrawPixel(pImage,1 - Color,X0,Y0);
        }
      }
      Count++;
      if (Count >= (On + Off))
      {
        Count  =  0;
      }
    }
  }
}


void      dLcdPlotPoints(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1)
{
  dLcdDrawPixel(pImage,Color,X0 + X1,Y0 + Y1);
  dLcdDrawPixel(pImage,Color,X0 - X1,Y0 + Y1);
  dLcdDrawPixel(pImage,Color,X0 + X1,Y0 - Y1);
  dLcdDrawPixel(pImage,Color,X0 - X1,Y0 - Y1);
  dLcdDrawPixel(pImage,Color,X0 + Y1,Y0 + X1);
  dLcdDrawPixel(pImage,Color,X0 - Y1,Y0 + X1);
  dLcdDrawPixel(pImage,Color,X0 + Y1,Y0 - X1);
  dLcdDrawPixel(pImage,Color,X0 - Y1,Y0 - X1);
}


void      dLcdDrawCircle(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 R)
{
  int     X = 0;
  int     Y = R;
  int     P = 3 - 2 * R;

  while (X<Y)
  {
    dLcdPlotPoints(pImage,Color,X0,Y0,X,Y);
    if (P < 0)
    {
      P = P + 4 * X + 6;
    }
    else
    {
      P = P + 4 * (X - Y) + 10;
      Y = Y - 1;
    }
    X = X + 1;
  }
  dLcdPlotPoints(pImage,Color,X0,Y0,X,Y);
}


typedef   struct
{
  const   char    *pFontBits;           // Pointer to start of font bitmap
  const   DATA16  FontHeight;           // Character height (all inclusive)
  const   DATA16  FontWidth;            // Character width (all inclusive)
  const   DATA16  FontHorz;             // Number of horizontal character in font bitmap
  const   DATA8   FontFirst;            // First character supported
  const   DATA8   FontLast;             // Last character supported
}
FONTINFO;


#include  "normal_font.xbm"
#include  "small_font.xbm"
#include  "large_font.xbm"
#include  "tiny_font.xbm"


FONTINFO  FontInfo[] =
{
  [NORMAL_FONT] = {
                    .pFontBits    = (const char*)normal_font_bits,
                    .FontHeight   = 9,
                    .FontWidth    = 8,
                    .FontHorz     = 16,
                    .FontFirst    = 0x20,
                    .FontLast     = 0x7F
                  },
  [SMALL_FONT] =  {
                    .pFontBits    = (const char*)small_font_bits,
                    .FontHeight   = 8,
                    .FontWidth    = 8,
                    .FontHorz     = 16,
                    .FontFirst    = 0x20,
                    .FontLast     = 0x7F
                  },
  [LARGE_FONT] =  {
                    .pFontBits    = (const char*)large_font_bits,
                    .FontHeight   = 16,
                    .FontWidth    = 16,
                    .FontHorz     = 16,
                    .FontFirst    = 0x20,
                    .FontLast     = 0x7F
                  },
  [TINY_FONT] =   {
                    .pFontBits    = (const char*)tiny_font_bits,
                    .FontHeight   = 7,
                    .FontWidth    = 5,
                    .FontHorz     = 16,
                    .FontFirst    = 0x20,
                    .FontLast     = 0x7F
                  },

};


DATA16    dLcdGetFontWidth(DATA8 Font)
{
  return (FontInfo[Font].FontWidth);
}


DATA16    dLcdGetFontHeight(DATA8 Font)
{
  return (FontInfo[Font].FontHeight);
}


void      dLcdDrawChar(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA8 Font,DATA8 Char)
{
  DATA16  CharWidth;
  DATA16  CharHeight;
  DATA16  CharByteIndex;
  DATA16  LcdByteIndex;
  UBYTE   CharByte;
  DATA16  Tmp,X,Y,TmpX,MaxX;


  CharWidth   =  FontInfo[Font].FontWidth;
  CharHeight  =  FontInfo[Font].FontHeight;

  if ((Char >= FontInfo[Font].FontFirst) && (Char <= FontInfo[Font].FontLast))
  {
    Char -=  FontInfo[Font].FontFirst;

    CharByteIndex  =  (Char % FontInfo[Font].FontHorz) * ((CharWidth + 7) / 8);
    CharByteIndex +=  ((Char / FontInfo[Font].FontHorz) * ((CharWidth + 7) / 8) * CharHeight * FontInfo[Font].FontHorz);

    if (((CharWidth % 8) == 0) && ((X0 % 8) == 0))
    { // Font aligned

      X0             =  (X0 >> 3) << 3;
      LcdByteIndex   =  (X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3);

      if (Color)
      {
        while (CharHeight)
        {
          Tmp  =  0;
          do
          {
            if (LcdByteIndex < sizeof(LCD))
            {
              pImage[LcdByteIndex + Tmp]  =  FontInfo[Font].pFontBits[CharByteIndex + Tmp];
            }
            Tmp++;
          }
          while (Tmp < (CharWidth / 8));

          CharByteIndex +=  (CharWidth * FontInfo[Font].FontHorz) / 8;
          LcdByteIndex  +=  ((LCD_WIDTH + 7) >> 3);
          CharHeight--;
        }
      }
      else
      {
        while (CharHeight)
        {
          Tmp  =  0;
          do
          {
            if (LcdByteIndex < sizeof(LCD))
            {
              pImage[LcdByteIndex + Tmp]  = ~FontInfo[Font].pFontBits[CharByteIndex + Tmp];
            }
            Tmp++;
          }
          while (Tmp < (CharWidth / 8));

          CharByteIndex +=  (CharWidth * FontInfo[Font].FontHorz) / 8;
          LcdByteIndex  +=  ((LCD_WIDTH + 7) >> 3);
          CharHeight--;
        }
      }
    }
    else
    { // Font not aligned

      MaxX          =  X0 + CharWidth;

      if (Color)
      {
        for (Y = 0;Y < CharHeight;Y++)
        {
          TmpX              =  X0;

          for (X = 0;X < ((CharWidth + 7) / 8);X++)
          {
            CharByte  =  FontInfo[Font].pFontBits[CharByteIndex + X];

            for (Tmp = 0;(Tmp < 8) && (TmpX < MaxX);Tmp++)
            {
              if (CharByte & 1)
              {
                dLcdDrawPixel(pImage,1,TmpX,Y0);
              }
              else
              {
                dLcdDrawPixel(pImage,0,TmpX,Y0);
              }
              CharByte >>= 1;
              TmpX++;
            }
          }
          Y0++;
          CharByteIndex +=  ((CharWidth + 7) / 8) * FontInfo[Font].FontHorz;

        }
      }
      else
      {
        for (Y = 0;Y < CharHeight;Y++)
        {
          TmpX              =  X0;

          for (X = 0;X < ((CharWidth + 7) / 8);X++)
          {
            CharByte  =  FontInfo[Font].pFontBits[CharByteIndex + X];

            for (Tmp = 0;(Tmp < 8) && (TmpX < MaxX);Tmp++)
            {
              if (CharByte & 1)
              {
                dLcdDrawPixel(pImage,0,TmpX,Y0);
              }
              else
              {
                dLcdDrawPixel(pImage,1,TmpX,Y0);
              }
              CharByte >>= 1;
              TmpX++;
            }
          }
          Y0++;
          CharByteIndex +=  ((CharWidth + 7) / 8) * FontInfo[Font].FontHorz;
        }
      }
    }
  }
}


void      dLcdDrawText(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA8 Font,DATA8 *pText)
{
  while (*pText)
  {
    if (X0 < (LCD_WIDTH - FontInfo[Font].FontWidth))
    {
      dLcdDrawChar(pImage,Color,X0,Y0,Font,*pText);
      X0 +=  FontInfo[Font].FontWidth;
    }
    pText++;
  }
}


typedef   struct
{
  const   char    *pIconBits;
  const   DATA16  IconSize;
  const   DATA16  IconHeight;
  const   DATA16  IconWidth;
}
ICONINFO;

#include  "normal_icons.xbm"
#include  "small_icons.xbm"
#include  "large_icons.xbm"
#include  "menu_icons.xbm"
#include  "arrow_icons.xbm"


ICONINFO  IconInfo[] =
{
  [NORMAL_ICON] = {
                    .pIconBits    = normal_icons_bits,
                    .IconSize     = normal_icons_height,
                    .IconHeight   = 12,
                    .IconWidth    = normal_icons_width
                  },
  [SMALL_ICON] =  {
                    .pIconBits    = small_icons_bits,
                    .IconSize     = small_icons_height,
                    .IconHeight   = 8,
                    .IconWidth    = small_icons_width
                  },
  [LARGE_ICON] =  {
                    .pIconBits    = large_icons_bits,
                    .IconSize     = large_icons_height,
                    .IconHeight   = 22,
                    .IconWidth    = large_icons_width
                  },
  [MENU_ICON] =   {
                    .pIconBits    = menu_icons_bits,
                    .IconSize     = menu_icons_height,
                    .IconHeight   = 12,
                    .IconWidth    = menu_icons_width
                  },
  [ARROW_ICON] =  {
                    .pIconBits    = arrow_icons_bits,
                    .IconSize     = arrow_icons_height,
                    .IconHeight   = 12,
                    .IconWidth    = arrow_icons_width
                  },
};


UBYTE    *dLcdGetIconBits(DATA8 Type)
{
  UBYTE  *pResult;

  pResult  =  (UBYTE*)IconInfo[Type].pIconBits;

  return (pResult);
}


DATA16    dLcdGetIconWidth(DATA8 Type)
{
  return (IconInfo[Type].IconWidth);
}


DATA16    dLcdGetIconHeight(DATA8 Type)
{
  return (IconInfo[Type].IconHeight);
}


DATA16    dLcdGetNoOfIcons(DATA8 Type)
{
  return (IconInfo[Type].IconSize / IconInfo[Type].IconHeight);
}


void      dLcdDrawPicture(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 IconWidth,DATA16 IconHeight,UBYTE *pIconBits)
{
  DATA16  IconByteIndex;
  DATA16  LcdByteIndex;
  DATA16  Tmp;

  IconByteIndex  =  0;

  X0             =  (X0 >> 3) << 3;
  LcdByteIndex   =  (X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3);


  if (Color)
  {
    while (IconHeight)
    {
      for (Tmp = 0;Tmp < (IconWidth / 8);Tmp++)
      {
        pImage[LcdByteIndex + Tmp] =  pIconBits[IconByteIndex + Tmp];
      }

      IconByteIndex +=  IconWidth / 8;
      LcdByteIndex  +=  ((LCD_WIDTH + 7) >> 3);
      IconHeight--;
    }
  }
  else
  {
    while (IconHeight)
    {
      for (Tmp = 0;Tmp < (IconWidth / 8);Tmp++)
      {
        pImage[LcdByteIndex + Tmp] = ~pIconBits[IconByteIndex + Tmp];
      }

      IconByteIndex +=  IconWidth / 8;
      LcdByteIndex  +=  ((LCD_WIDTH + 7) >> 3);
      IconHeight--;
    }
  }
}


void      dLcdDrawIcon(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA8 Type,DATA8 No)
{
  DATA16  IconByteIndex;
  DATA16  IconHeight;
  DATA16  IconWidth;
  UBYTE   *pIconBits;

  IconHeight  =  dLcdGetIconHeight(Type);
  IconWidth   =  dLcdGetIconWidth(Type);

  if ((No >= 0) && (No <= dLcdGetNoOfIcons(Type)))
  {
    pIconBits      =  dLcdGetIconBits(Type);
    IconByteIndex  =  ((DATA16)No * IconWidth * IconHeight) / 8;
    dLcdDrawPicture(pImage,Color,X0,Y0,IconWidth,IconHeight,&pIconBits[IconByteIndex]);
  }
}


void      dLcdGetBitmapSize(IP pBitmap,DATA16 *pWidth,DATA16 *pHeight)
{
  *pWidth     =  0;
  *pHeight    =  0;

  if (pBitmap)
  {

    *pWidth     =  (DATA16)pBitmap[0];
    *pHeight    =  (DATA16)pBitmap[1];
  }
}


void      dLcdDrawBitmap(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,IP pBitmap)
{
  DATA16  BitmapWidth;
  DATA16  BitmapHeight;
  DATA16  BitmapByteIndex;
  UBYTE   *pBitmapBytes;
  UBYTE   BitmapByte;
  DATA16  Tmp,X,Y,TmpX,MaxX;

  DATA16  LcdByteIndex;

  if (pBitmap)
  {

    BitmapWidth   =  (DATA16)pBitmap[0];
    BitmapHeight  =  (DATA16)pBitmap[1];
    MaxX          =  X0 + BitmapWidth;
    pBitmapBytes  =  &pBitmap[2];

    if ((BitmapWidth >=0) && (BitmapHeight >= 0))
    {
      if ((X0 % 8) || (BitmapWidth % 8))
      { // X is not aligned

        BitmapWidth       =  ((BitmapWidth + 7) >> 3) << 3;

        if (Color)
        {
          for (Y = 0;Y < BitmapHeight;Y++)
          {
            BitmapByteIndex   =  (Y * BitmapWidth) / 8;
            TmpX              =  X0;

            for (X = 0;X < (BitmapWidth / 8);X++)
            {
              BitmapByte  =  pBitmapBytes[BitmapByteIndex + X];

              for (Tmp = 0;(Tmp < 8) && (TmpX < MaxX);Tmp++)
              {
                if (BitmapByte & 1)
                {
                  dLcdDrawPixel(pImage,1,TmpX,Y0);
                }
                else
                {
                  dLcdDrawPixel(pImage,0,TmpX,Y0);
                }
                BitmapByte >>= 1;
                TmpX++;
              }
            }
            Y0++;
          }
        }
        else
        {
          for (Y = 0;Y < BitmapHeight;Y++)
          {
            BitmapByteIndex   =  (Y * BitmapWidth) / 8;
            TmpX              =  X0;

            for (X = 0;X < (BitmapWidth / 8);X++)
            {
              BitmapByte  =  pBitmapBytes[BitmapByteIndex + X];

              for (Tmp = 0;(Tmp < 8) && (TmpX < MaxX);Tmp++)
              {
                if (BitmapByte & 1)
                {
                  dLcdDrawPixel(pImage,0,TmpX,Y0);
                }
                else
                {
                  dLcdDrawPixel(pImage,1,TmpX,Y0);
                }
                BitmapByte >>= 1;
                TmpX++;
              }
            }
            Y0++;
          }
        }
      }

      else
      { // X is byte aligned

        BitmapByteIndex   =  0;

        LcdByteIndex      =  (X0 >> 3) + Y0 * ((LCD_WIDTH + 7) >> 3);

        if (Color)
        {
          while (BitmapHeight)
          {
            X  =  X0;
            for (Tmp = 0;Tmp < (BitmapWidth / 8);Tmp++)
            {
              if (((LcdByteIndex + Tmp) < LCD_BUFFER_SIZE) && (X < LCD_WIDTH) && (X >= 0))
              {
                pImage[LcdByteIndex + Tmp]  =  pBitmapBytes[BitmapByteIndex + Tmp];
              }
              X +=  8;
            }

            BitmapByteIndex +=  BitmapWidth / 8;
            LcdByteIndex    +=  ((LCD_WIDTH + 7) >> 3);

            BitmapHeight--;
          }
        }
        else
        {
          while (BitmapHeight)
          {
            X  =  X0;
            for (Tmp = 0;Tmp < (BitmapWidth / 8);Tmp++)
            {
              if (((LcdByteIndex + Tmp) < LCD_BUFFER_SIZE) && (X < LCD_WIDTH) && (X >= 0))
              {
                pImage[LcdByteIndex + Tmp]  = ~pBitmapBytes[BitmapByteIndex + Tmp];
              }
              X +=  8;
            }

            BitmapByteIndex +=  BitmapWidth / 8;
            LcdByteIndex    +=  ((LCD_WIDTH + 7) >> 3);

            BitmapHeight--;
          }
        }
      }

    }
  }
}


void      dLcdRect(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1)
{
  X1--;
  Y1--;
  dLcdDrawLine(pImage,Color,X0,Y0,X0 + X1,Y0);
  dLcdDrawLine(pImage,Color,X0 + X1,Y0,X0 + X1,Y0 + Y1);
  dLcdDrawLine(pImage,Color,X0 + X1,Y0 + Y1,X0,Y0 + Y1);
  dLcdDrawLine(pImage,Color,X0,Y0 + Y1,X0,Y0);
}


void      dLcdFillRect(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1)
{
  DATA16  StartX;
  DATA16  MaxX;
  DATA16  MaxY;

  StartX  =  X0;
  MaxX    =  X0 + X1;
  MaxY    =  Y0 + Y1;

  for (;Y0 < MaxY;Y0++)
  {
    for (X0 = StartX;X0 < MaxX;X0++)
    {
      dLcdDrawPixel(pImage,Color,X0,Y0);
    }
  }
}


void      dLcdInverseRect(UBYTE *pImage,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1)
{
  DATA16  StartX;
  DATA16  MaxX;
  DATA16  MaxY;

  StartX  =  X0;
  MaxX    =  X0 + X1;
  MaxY    =  Y0 + Y1;

  for (;Y0 < MaxY;Y0++)
  {
    for (X0 = StartX;X0 < MaxX;X0++)
    {
      dLcdInversePixel(pImage,X0,Y0);
    }
  }
}


void      dLcdPlotLines(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 X1,DATA16 Y1)
{
  dLcdDrawLine(pImage,Color,X0 - X1,Y0 + Y1,X0 + X1,Y0 + Y1);
  dLcdDrawLine(pImage,Color,X0 - X1,Y0 - Y1,X0 + X1,Y0 - Y1);
  dLcdDrawLine(pImage,Color,X0 - Y1,Y0 + X1,X0 + Y1,Y0 + X1);
  dLcdDrawLine(pImage,Color,X0 - Y1,Y0 - X1,X0 + Y1,Y0 - X1);
}


void      dLcdDrawFilledCircle(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0,DATA16 R)
{
  int     X = 0;
  int     Y = R;
  int     P = 3 - 2 * R;

  while (X<Y)
  {
    dLcdPlotLines(pImage,Color,X0,Y0,X,Y);
    if (P < 0)
    {
      P = P + 4 * X + 6;
    }
    else
    {
      P = P + 4 * (X - Y) + 10;
      Y = Y - 1;
    }
    X = X + 1;
  }
  dLcdPlotLines(pImage,Color,X0,Y0,X,Y);
}


DATA8     dLcdCheckPixel(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0)
{
  DATA8   Result = 0;

  if ((X0 >= 0) && (X0 < LCD_WIDTH) && (Y0 >= 0) && (Y0 < LCD_HEIGHT))
  {
    if (dLcdReadPixel(pImage,X0,Y0) != Color)
    {
      Result  =  1;
    }
  }

  return (Result);
}


void      dLcdFlodfill(UBYTE *pImage,DATA8 Color,DATA16 X0,DATA16 Y0)
{
  DATA16  X;
  DATA16  Y;

  Y  =  Y0;
  X  =  X0;

  while (dLcdCheckPixel(pImage,Color,X,Y))
  {
    while (dLcdCheckPixel(pImage,Color,X,Y))
    {
      if (X != X0)
      {
        dLcdDrawPixel(pImage,Color,X,Y);
      }
      X--;
    }
    X  =  X0;
    Y--;
  }

  Y  =  Y0;
  X  =  X0;

  while (dLcdCheckPixel(pImage,Color,X,Y))
  {
    while (dLcdCheckPixel(pImage,Color,X,Y))
    {
      if (X != X0)
      {
        dLcdDrawPixel(pImage,Color,X,Y);
      }
      X--;
    }
    X  =  X0;
    Y++;
  }

  Y  =  Y0;
  X  =  X0;

  while (dLcdCheckPixel(pImage,Color,X,Y))
  {
    while (dLcdCheckPixel(pImage,Color,X,Y))
    {
      dLcdDrawPixel(pImage,Color,X,Y);
      X++;
    }
    X  =  X0;
    Y--;
  }

  Y  =  Y0 + 1;
  X  =  X0;

  while (dLcdCheckPixel(pImage,Color,X,Y))
  {
    while (dLcdCheckPixel(pImage,Color,X,Y))
    {
      dLcdDrawPixel(pImage,Color,X,Y);
      X++;
    }
    X  =  X0;
    Y++;
  }

}
