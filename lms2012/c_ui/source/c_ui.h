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


#ifndef C_UI_H_
#define C_UI_H_

#include  "lms2012.h"

#define   IMAGEBUFFER_SIZE              1000
#define   KEYBUF_SIZE                   100
#define   UI_WR_BUFFER_SIZE             255

#define   GRAPH_BUFFERS                 (INPUTS + OUTPUTS)
#define   GRAPH_BUFFER_SIZE             LCD_WIDTH

#define   MAX_NOTIFY_LINES              8
#define   MAX_NOTIFY_LINE_CHARS         32

RESULT    cUiInit(void);

RESULT    cUiOpen(void);

void      cUiUpdate(UWORD Time);

RESULT    cUiClose(void);

RESULT    cUiExit(void);

DATA8     cUiEscape(void);

void      cUiTestpin(DATA8 State);

void      cUiAlive(void);

void      cUiFlush(void);

void      cUiButton(void);

void      cUiRead(void);

void      cUiWrite(void);

void      cUiDraw(void);

void      cUiKeepAlive(void);


typedef   struct
{
  DATAF   *pMin;
  DATAF   *pMax;
  DATAF   *pVal;
  DATA16  *pOffset;
  DATA16  *pSpan;
  DATAF   Buffer[GRAPH_BUFFERS][GRAPH_BUFFER_SIZE];
  DATA16  Pointer;
  DATA16  GraphStartX;
  DATA16  GraphSizeX;
  DATA8   Type;
  DATA8   Items;
  DATA8   Initialized;

  DATAF   Value;
  DATAF   Inc;
  DATA8   Down;
}
GRAPH;

typedef   struct
{
  DATA16  ScreenStartX;
  DATA16  ScreenStartY;
  DATA16  ScreenWidth;
  DATA16  ScreenHeight;
  DATA16  NoOfIcons;
  DATA16  NoOfChars;
  DATA16  FontWidth;
  DATA16  TextStartX;
  DATA16  TextStartY;
  DATA16  TextSpaceY;
  DATA16  TextChars;
  DATA16  TextLines;
  DATA8   TextLine[MAX_NOTIFY_LINES][MAX_NOTIFY_LINE_CHARS];
  DATA16  IconWidth;
  DATA16  IconHeight;
  DATA16  IconStartX;
  DATA16  IconStartY;
  DATA16  IconSpaceX;
  DATA16  LineStartX;
  DATA16  LineStartY;
  DATA16  LineEndX;
  DATA16  YesNoStartX;
  DATA16  YesNoStartY;
  DATA8   Font;
  DATA8   NeedUpdate;
}
NOTIFY;


typedef   struct
{
  DATA16  ScreenStartX;
  DATA16  ScreenStartY;
  DATA16  ScreenWidth;
  DATA16  ScreenHeight;
  DATA16  Frame;
  DATA32  Icons;
  DATA16  NoOfIcons;
  DATA16  IconWidth;
  DATA16  IconHeight;
  DATA16  IconStartX;
  DATA16  IconStartY;
  DATA16  IconSpaceX;
  DATA16  PointerX;
  DATA16  SelectStartX;
  DATA16  SelectStartY;
  DATA16  SelectWidth;
  DATA16  SelectHeight;
  DATA16  SelectSpaceX;
  DATA8   NeedUpdate;
}
IQUESTION;


typedef   struct
{
  DATA16  ScreenStartX;
  DATA16  ScreenStartY;
  DATA16  ScreenWidth;
  DATA16  ScreenHeight;
  DATA16  NoOfIcons;
  DATA16  IconWidth;
  DATA16  IconHeight;
  DATA16  IconStartX;
  DATA16  IconStartY;
  DATA16  IconSpaceX;
  DATA16  LineStartX;
  DATA16  LineStartY;
  DATA16  LineEndX;
  DATA16  YesNoStartX;
  DATA16  YesNoStartY;
  DATA16  YesNoSpaceX;
  DATA8   Default;
  DATA8   NeedUpdate;
}
TQUESTION;


typedef   struct
{
  DATA16  ScreenStartX;
  DATA16  ScreenStartY;
  DATA16  IconStartX;
  DATA16  IconStartY;
  DATA16  TextStartX;
  DATA16  TextStartY;
  DATA16  StringStartX;
  DATA16  StringStartY;
  DATA16  KeybStartX;
  DATA16  KeybStartY;
  DATA16  KeybSpaceX;
  DATA16  KeybSpaceY;
  DATA16  KeybWidth;
  DATA16  KeybHeight;
  DATA16  PointerX;
  DATA16  PointerY;
  DATA16  OldX;
  DATA16  OldY;
  DATA8   Layout;
  DATA8   CharSet;
  DATA8   NeedUpdate;
}
KEYB;

#define   TEXTSIZE      24

typedef   struct
{
  DATA16  ScreenStartX;
  DATA16  ScreenStartY;
  DATA16  ScreenWidth;
  DATA16  ScreenHeight;
  DATA16  CharWidth;
  DATA16  CharHeight;
  DATA16  LineSpace;
  DATA16  LineHeight;
  DATA16  IconWidth;
  DATA16  IconHeight;
  DATA16  IconStartX;
  DATA16  IconStartY;
  DATA16  TextStartX;
  DATA16  TextStartY;
  DATA16  SelectStartX;
  DATA16  SelectStartY;
  DATA16  SelectWidth;
  DATA16  SelectHeight;
  DATA16  ScrollStartX;
  DATA16  ScrollStartY;
  DATA16  ScrollWidth;
  DATA16  ScrollHeight;
  DATA16  NobHeight;
  DATA16  ScrollSpan;

  DATA16  Chars;
  DATA16  Lines;

  HANDLER hFolders;
  HANDLER hFiles;
  PRGID   PrgId;
  OBJID   ObjId;

  DATA16  OldFiles;
  DATA16  Folders;                      // Number of folders [0..DIR_DEEPT]
  DATA16  OpenFolder;                   // Folder number open (0 = none) [0..DIR_DEEPT]
  DATA16  Files;                        // Number of files in open folder [0..DIR_DEEPT]
  DATA16  ItemStart;                    // Item number at top of list (shown)
  DATA16  ItemPointer;                  // Item list pointer - folder or file

  DATA8   NeedUpdate;                   // Flag set if returning without closing browser

#ifndef DISABLE_SDCARD_SUPPORT
  DATA8   Sdcard;
#endif

#ifndef DISABLE_USBSTICK_SUPPORT
  DATA8   Usbstick;
#endif

  DATA8   TopFolder[MAX_FILENAME_SIZE];
  DATA8   SubFolder[MAX_FILENAME_SIZE];
  DATA8   FullPath[MAX_FILENAME_SIZE];
  DATA8   Filename[MAX_FILENAME_SIZE];
  DATA8   Text[TEXTSIZE];
}
BROWSER;


typedef   struct
{
  DATA16  ScreenStartX;
  DATA16  ScreenStartY;
  DATA16  ScreenWidth;
  DATA16  ScreenHeight;
  DATA16  CharWidth;
  DATA16  CharHeight;
  DATA16  LineSpace;
  DATA16  LineHeight;
  DATA8   NeedUpdate;                   // Flag set if returning without closing browser
  DATA16  TextStartX;
  DATA16  TextStartY;
  DATA16  SelectStartX;
  DATA16  SelectStartY;
  DATA16  SelectWidth;
  DATA16  SelectHeight;
  DATA16  ScrollStartX;
  DATA16  ScrollStartY;
  DATA16  ScrollWidth;
  DATA16  ScrollHeight;
  DATA16  NobHeight;
  DATA16  ScrollSpan;

  DATA16  Chars;
  DATA16  Lines;

  DATA16  Items;
  DATA16  ItemStart;                    // Item number at top of list (shown)
  DATA16  ItemPointer;                  // Item list pointer - folder or file

  DATA8   Font;
  DATA8   Text[TEXTSIZE];
}
TXTBOX;


#define   HWVERS_SIZE       6
#define   FWVERS_SIZE       7
#define   FWBUILD_SIZE      11
#define   OSVERS_SIZE       17
#define   OSBUILD_SIZE      11
#define   IPADDR_SIZE       16


#define   BUTTON_ACTIVE                 0x01
#define   BUTTON_PRESSED                0x02  //!< button is pressed at the moment
#define   BUTTON_ACTIVATED              0x04  //!< button has been activated since last read
#define   BUTTON_LONGPRESS              0x08  //!< button long press detected
#define   BUTTON_BUMBED                 0x10  //!< button has been pressed and released
#define   BUTTON_LONG_LATCH             0x20

#define   BUTTON_CLR                    (BUTTON_ACTIVATED | BUTTON_LONGPRESS | BUTTON_BUMBED | BUTTON_LONG_LATCH)
#define   BUTTON_FLUSH                  (BUTTON_ACTIVATED | BUTTON_LONGPRESS | BUTTON_BUMBED | BUTTON_LONG_LATCH)



#define   BUTTON_ALIVE                  0x01
#define   BUTTON_CLICK                  0x02
#define   BUTTON_BUFPRINT               0x04

#define   BUTTON_SET                    (BUTTON_ALIVE | BUTTON_CLICK)


typedef   struct
{
  //*****************************************************************************
  // Ui Global variables
  //*****************************************************************************

  LCD       LcdSafe;
  LCD       LcdSave;
  LCD       LcdPool[LCD_STORE_LEVELS];
  LCD       LcdBuffer;
  LCD       *pLcd;

  UI        UiSafe;
  UI        *pUi;

  ANALOG    Analog;
  ANALOG    *pAnalog;

  NOTIFY    Notify;
  TQUESTION Question;
  IQUESTION IconQuestion;
  BROWSER   Browser;
  KEYB      Keyboard;
  GRAPH     Graph;
  TXTBOX    Txtbox;

  int       PowerFile;
  int       UiFile;
  int       AdcFile;
  int       DispFile;

  ULONG     SleepTimer;

  ULONG     MilliSeconds;
  ULONG     RunScreenTimer;
  ULONG     PowerTimer;
  ULONG     VoltageTimer;
#ifdef MAX_FRAMES_PER_SEC
  ULONG     DisplayTimer;
  DATA8     DisplayUpdate;
  DATA8     AllowUpdate;
#endif

  DATAF     CinCnt;
  DATAF     CoutCnt;
  DATAF     VinCnt;

#ifndef DISABLE_VIRTUAL_BATT_TEMP
  DATA32    TempTimer;

#endif
  DATAF     Tbatt;
  DATAF     Vbatt;
  DATAF     Ibatt;
  DATAF     Imotor;
  DATAF     Iintegrated;

  UBYTE     PowerInitialized;

  DATA8     UpdateState;
  ULONG     UpdateStateTimer;
  UBYTE     ReadyForWarnings;

  DATA8     TopLineEnabled;
  DATA8     RunScreenEnabled;
  DATA8     RunLedEnabled;
  DATA8     BackButtonBlocked;
  DATA8     RunScreenNumber;
  DATA8     RunScreenCounter;
  DATA8     Escape;

  DATA8     LedState;

  DATA8     ButtonState[BUTTONS];
  DATA16    ButtonTimer[BUTTONS];
  DATA16    ButtonDebounceTimer[BUTTONS];
  DATA16    ButtonRepeatTimer[BUTTONS];
  DATA8     Activated;

  DATA8     ScreenBusy;
  DATA8     ScreenBlocked;
  PRGID     ScreenPrgId;
  OBJID     ScreenObjId;

  DATA8     ShutDown;
  DATA8     Accu;
  DATA8     PowerShutdown;
  DATA8     PowerState;
  DATA8     VoltShutdown;
  DATA8     VoltageState;
  DATA8     Warnlight;
  DATA8     Warning;
  DATA8     WarningShowed;
  DATA8     WarningConfirmed;
  DATA8     UiUpdate;

  DATA8     BtOn;
  DATA8     WiFiOn;

  DATA16    BattIndicatorHigh;
  DATA16    BattIndicatorLow;
  DATAF     BattWarningHigh;
  DATAF     BattWarningLow;
  DATAF     BattShutdownHigh;
  DATAF     BattShutdownLow;

  DATA8     Event;

  DATA8     Click;

  DATA8     Font;

  IMGDATA   ImageBuffer[IMAGEBUFFER_SIZE];

  DATA8     KeyBuffer[KEYBUF_SIZE + 1];
  DATA8     KeyBufIn;
  DATA8     Keys;

  DATA8     UiWrBuffer[UI_WR_BUFFER_SIZE];
  DATA16    UiWrBufferSize;

  IMINDEX   Point;
  IMINDEX   Size;

  char      HwVers[HWVERS_SIZE];
  char      FwVers[FWVERS_SIZE];
  char      FwBuild[FWBUILD_SIZE];
  char      OsVers[OSVERS_SIZE];
  char      OsBuild[OSBUILD_SIZE];
  char      IpAddr[IPADDR_SIZE];

  DATA8     Hw;

  IMGDATA   Globals[MAX_COMMAND_GLOBALS];

}
UI_GLOBALS;

#if       (HARDWARE == SIMULATION)
  extern UI_GLOBALS * gUiInstance;
  #define UiInstance (*gUiInstance)

  void setUiInstance(UI_GLOBALS * _Instance);
  UI_GLOBALS * getUiInstance();
#else
  extern UI_GLOBALS UiInstance;
#endif

#endif /* C_UI_H_ */
