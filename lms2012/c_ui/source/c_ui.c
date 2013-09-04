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


/*! \page UserInterfaceLibrary User Interface Library
 *
 *- \subpage  UserInterLibraryDescription
 *- \subpage  UIdesign
 */


/*! \page UserInterLibraryDescription Description
 *
 *  <hr size="1"/>
 *
 *  The user interface library contains different functionality used to support a graphical user interface.
 *  Graphic files as fonts, icons and pictures are ".xbm" files containing monochrome bitmap that can also
 *  be opened with a normal text editor. These files are coming from the LINUX world.
 *
  \verbatim


    Fonts:      must be X axis aligned on 8 pixel boundaries (build into the firmware)

                Normal font   8  x 9    (W x H)         Used in browser
                Small font    8  x 8    (W x H)         Used on display top line
                Large font    16 x 19   (W x H)         Used in numbers


    Icons:      must be X axis aligned on 8 pixel boundaries (build into the firmware)

                Normal icon   16 x 12   (W x H)         Used in browser
                Small icon    16 x 8    (W x H)         Used on display top line
                Large icon    24 x 24   (W x H)         Used as keys


    Pictures:   no restrictions (files in the file system)

                Smallest       1 x 1    (W x H)
                Largest      178 x 128  (W x H)


  \endverbatim
 *
 */


/*! \page UIdesign Folder Structure
 *
 *  <hr size="1"/>
 *
 \verbatim

                            PROJECTS

    lms2012-------prjs-----,----xxxxxxxx------icon file       (icon.rgf)
                  |        |                  byte code file  (xxxxxxxx.rbf)
                  |        |                  sound files     (.rsf)
                  |        |                  graphics files  (.rgf)
                  |        |                  datalog files   (.rdf)
                  |        |
                  |        |----yyyyyyyy------icon file       (icon.rgf)
                  |        |                  byte code file  (yyyyyyyy.rbf)
                  |        |                  sound files
                  |        |                  graphics files  (.rgf)
                  |        |                  datalog files   (.rdf)
                  |        |
                  |        |--BrkProg_SAVE----byte code files (.rbf)
                  |        |
                  |        |---BrkDL_SAVE-----datalog files   (.rdf)
                  |        |
                  |        |
                  |        '---SD_Card---,----vvvvvvvv------icon file       (icon.rgf)
                  |                      |                  byte code file  (xxxxxxxx.rbf)
                  |                      |                  sound files     (.rsf)
                  |                      |                  graphics files  (.rgf)
                  |                      |                  datalog files   (.rdf)
                  |                      |
                  |                      |----wwwwwwww------icon file       (icon.rgf)
                  |                      |                  byte code file  (yyyyyyyy.rbf)
                  |                      |                  sound files
                  |                      |                  graphics files  (.rgf)
                  |                      |                  datalog files   (.rdf)
                  |                      |
                  |                      |--BrkProg_SAVE----byte code files (.rbf)
                  |                      |
                  |                      '---BrkDL_SAVE-----datalog files   (.rdf)
                  |
                  |
                  |         APPS
                  |
                  apps-----,----aaaaaaaa------icon file       (icon.rgf)
                  |        |                  byte code files (aaaaaaaa.rbf)
                  |        |                  sound files     (.rsf)
                  |        |                  graphics files  (.rgf)
                  |        |
                  |        '----bbbbbbbb------icon file       (icon.rgf)
                  |                           byte code files (bbbbbbbb.rbf)
                  |                           sound files     (.rsf)
                  |                           graphics files  (.rgf)
                  |
                  |         TOOLS
                  |
                  tools----,----cccccccc------icon file       (icon.rgf)
                  |        |                  byte code files (cccccccc.rbf)
                  |        |                  sound files     (.rsf)
                  |        |                  graphics files  (.rgf)
                  |        |
                  |        '----dddddddd------icon file       (icon.rgf)
                  |                           byte code files (dddddddd.rbf)
                  |                           sound files     (.rsf)
                  |                           graphics files  (.rgf)
                  |
                  |         SYSTEM
                  |
                  sys------,----ui------------byte code file  (gui.rbf)
                           |                  sound files     (.rsf)
                           |                  graphics files  (.rgf)
                           |
                           |----lib-----------shared librarys (.so)
                           |
                           |----mod-----------kernel modules  (.ko)
                           |
                           |----settings------config files    (.rcf, .rtf, .dat)
                           |                  typedata.rcf    (device type data)
                           |
                           |
                           '----lms2012   (executable)
                                bash files

  \endverbatim
 *
 */


#include  "lms2012.h"
#include  "c_ui.h"
#include  "d_terminal.h"
#include  "../../c_memory/source/c_memory.h"
#include  "../../c_com/source/c_com.h"
#include  "../../c_input/source/c_input.h"
#include  <string.h>
#include  <time.h>
extern    char *strptime(const char *s, const char *format, struct tm *tm);


#ifdef    DEBUG_C_UI
#define   DEBUG
#endif


#if       (HARDWARE != SIMULATION)

#include  <sys/stat.h>
#include  <sys/statvfs.h>
#include  <sys/types.h>
#include  <sys/sysinfo.h>

#include  <stdio.h>
#include  <stdlib.h>
#include  <fcntl.h>
#include  <unistd.h>
#include  <dirent.h>
#include  "d_lcd.h"

#include  <sys/mman.h>
#include  <sys/ioctl.h>
#include  <math.h>
#include  <sys/utsname.h>

#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <net/if.h>
#include  <arpa/inet.h>


UI_GLOBALS UiInstance;


#else

#define snprintf _snprintf
#include  <stdio.h>
#include  <stdlib.h>
  UI_GLOBALS * gUiInstance;

  void setUiInstance(UI_GLOBALS * _Instance)
  {
    gUiInstance= _Instance;
  }

  UI_GLOBALS* getUiInstance()
  {
    return gUiInstance;
  }

#endif


#ifndef DISABLE_VIRTUAL_BATT_TEMP

  //Defines der kan tænde for debug beskeder
  //#define __dbg1
  //#define __dbg2

  /*************************** Model parameters *******************************/
  //Approx. initial internal resistance of 6 Energizer industrial batteries :
  float R_bat_init = 0.63468;
  //Bjarke's proposal for spring resistance :
  //float spring_resistance = 0.42;
  //Batteries' heat capacity :
  float heat_cap_bat = 136.6598;
  //Newtonian cooling constant for electronics :
  float K_bat_loss_to_elec = -0.0003; //-0.000789767;
  //Newtonian heating constant for electronics :
  float K_bat_gain_from_elec = 0.001242896; //0.001035746;
  //Newtonian cooling constant for environment :
  float K_bat_to_room = -0.00012;
  //Battery power Boost
  float battery_power_boost = 1.7;
  //Battery R_bat negative gain
  float R_bat_neg_gain = 1.00;

  //Slope of electronics lossless heating curve (linear!!!) [Deg.C / s] :
  float K_elec_heat_slope = 0.0123175;
  //Newtonian cooling constant for battery packs :
  float K_elec_loss_to_bat = -0.004137487;
  //Newtonian heating constant for battery packs :
  float K_elec_gain_from_bat = 0.002027574; //0.00152068;
  //Newtonian cooling constant for environment :
  float K_elec_to_room = -0.001931431; //-0.001843639;

  // Function for estimating new battery temperature based on measurements
  // of battery voltage and battery power.
    float new_bat_temp (float V_bat, float I_bat)
    {

      static int   index       = 0; //Keeps track of sample index since power-on
      static float I_bat_mean  = 0; //Running mean current
      const float sample_period = 0.4; //Algorithm update period in seconds
      static float T_bat = 0; //Battery temperature
      static float T_elec = 0; //EV3 electronics temperature

      static float R_bat_model_old = 0;//Old internal resistance of the battery model
      float R_bat_model;    //Internal resistance of the battery model
      static float R_bat = 0; //Internal resistance of the batteries
      float slope_A;        //Slope obtained by linear interpolation
      float intercept_b;    //Offset obtained by linear interpolation
      const float I_1A = 0.05;      //Current carrying capacity at bottom of the curve
      const float I_2A = 2.0;      //Current carrying capacity at the top of the curve

      float R_1A = 0.0;          //Internal resistance of the batteries at 1A and V_bat
      float R_2A = 0.0;          //Internal resistance of the batteries at 2A and V_bat

      //Flag that prevents initialization of R_bat when the battery is charging
      static unsigned char has_passed_7v5_flag = 'N';

      float dT_bat_own = 0.0; //Batteries' own heat
      float dT_bat_loss_to_elec = 0.0; // Batteries' heat loss to electronics
      float dT_bat_gain_from_elec = 0.0; //Batteries' heat gain from electronics
      float dT_bat_loss_to_room = 0.0; //Batteries' cooling from environment

      float dT_elec_own = 0.0; //Electronics' own heat
      float dT_elec_loss_to_bat = 0.0;//Electronics' heat loss to the battery pack
      float dT_elec_gain_from_bat = 0.0;//Electronics' heat gain from battery packs
      float dT_elec_loss_to_room = 0.0; //Electronics' heat loss to the environment

      /***************************************************************************/

      //Update the average current: I_bat_mean
      if (index > 0)
        {
    I_bat_mean = ((index) * I_bat_mean + I_bat) / (index + 1) ;
        }
      else
        {
    I_bat_mean = I_bat;
        }

      index = index + 1;


      //Calculate R_1A as a function of V_bat (internal resistance at 1A continuous)
      R_1A  =   0.014071 * (V_bat * V_bat * V_bat * V_bat)
        - 0.335324 * (V_bat * V_bat * V_bat)
        + 2.933404 * (V_bat * V_bat)
        - 11.243047 * V_bat
        + 16.897461;

      //Calculate R_2A as a function of V_bat (internal resistance at 2A continuous)
      R_2A  =   0.014420 * (V_bat * V_bat * V_bat * V_bat)
        - 0.316728 * (V_bat * V_bat * V_bat)
        + 2.559347 * (V_bat * V_bat)
        - 9.084076 * V_bat
        + 12.794176;

      //Calculate the slope by linear interpolation between R_1A and R_2A
      slope_A  =  (R_1A - R_2A) / (I_1A - I_2A);

      //Calculate intercept by linear interpolation between R1_A and R2_A
      intercept_b  =  R_1A - slope_A * R_1A;

      //Reload R_bat_model:
      R_bat_model  =  slope_A * I_bat_mean + intercept_b;

      //Calculate batteries' internal resistance: R_bat
      if ((V_bat > 7.5) && (has_passed_7v5_flag == 'N'))
        {
    R_bat = R_bat_init; //7.5 V not passed a first time
        }
      else
        {
    //Only update R_bat with positive outcomes: R_bat_model - R_bat_model_old
    //R_bat updated with the change in model R_bat is not equal value in the model!
    if ((R_bat_model - R_bat_model_old) > 0)
      {
        R_bat = R_bat + R_bat_model - R_bat_model_old;
      }
      else // The negative outcome of R_bat_model added to only part of R_bat
        {
          R_bat = R_bat + ( R_bat_neg_gain * (R_bat_model - R_bat_model_old));
        }
    //Make sure we initialize R_bat later
    has_passed_7v5_flag = 'Y';
        }

      //Save R_bat_model for use in the next function call
      R_bat_model_old = R_bat_model;

      //Debug code:
  #ifdef __dbg1
      if (index < 500)
        {
    printf("%c %f %f %f %f %f %f\n", has_passed_7v5_flag, R_1A, R_2A,
           slope_A, intercept_b, R_bat_model - R_bat_model_old, R_bat);
        }
  #endif

      /*****Calculate the 4 types of temperature change for the batteries******/

      //Calculate the batteries' own temperature change
      dT_bat_own = R_bat * I_bat * I_bat * sample_period  * battery_power_boost
                   / heat_cap_bat;

      //Calculate the batteries' heat loss to the electronics
      if (T_bat > T_elec)
        {
    dT_bat_loss_to_elec = K_bat_loss_to_elec * (T_bat - T_elec)
                          * sample_period;
        }
      else
        {
    dT_bat_loss_to_elec = 0.0;
        }

      //Calculate the batteries' heat gain from the electronics
      if (T_bat < T_elec)
        {
    dT_bat_gain_from_elec = K_bat_gain_from_elec * (T_elec - T_bat)
      * sample_period;
        }
      else
        {
    dT_bat_gain_from_elec = 0.0;
        }

      //Calculate the batteries' heat loss to environment
      dT_bat_loss_to_room = K_bat_to_room * T_bat * sample_period;
      /************************************************************************/



      /*****Calculate the 4 types of temperature change for the electronics****/

      //Calculate the electronics' own temperature change
      dT_elec_own = K_elec_heat_slope * sample_period;

      //Calculate the electronics' heat loss to the batteries
      if (T_elec > T_bat)
        {
    dT_elec_loss_to_bat =   K_elec_loss_to_bat * (T_elec - T_bat)
      * sample_period;
        }
      else
        {
    dT_elec_loss_to_bat = 0.0;
        }

      //Calculate the electronics' heat gain from the batteries
      if (T_elec < T_bat)
        {
    dT_elec_gain_from_bat = K_elec_gain_from_bat * (T_bat - T_elec)
      * sample_period;
        }
      else
        {
    dT_elec_gain_from_bat = 0.0;
        }

      //Calculate the electronics' heat loss to the environment
      dT_elec_loss_to_room = K_elec_to_room * T_elec * sample_period;

      /*****************************************************************************/
      //Debug code:
  #ifdef __dbg2
      if (index < 500)
        {
    printf("%f %f %f %f %f <> %f %f %f %f %f\n",dT_bat_own, dT_bat_loss_to_elec,
           dT_bat_gain_from_elec, dT_bat_loss_to_room, T_bat,
           dT_elec_own, dT_elec_loss_to_bat, dT_elec_gain_from_bat,
           dT_elec_loss_to_room, T_elec);
        }
  #endif



      //Refresh battery temperature
      T_bat =  T_bat + dT_bat_own + dT_bat_loss_to_elec
        + dT_bat_gain_from_elec + dT_bat_loss_to_room;

      //Refresh electronics temperature
      T_elec =  T_elec + dT_elec_own + dT_elec_loss_to_bat
        + dT_elec_gain_from_bat + dT_elec_loss_to_room;

      return T_bat;

    }
#endif


#ifdef DEBUG_VIRTUAL_BATT_TEMP
static    int TempFile = -1;
#endif


#ifndef DISABLE_VIRTUAL_BATT_TEMP

void      cUiCheckTemp(void);

#define   CALL_INTERVAL     400   // [mS]

#ifdef DEBUG_VIRTUAL_BATT_TEMP

void      cUiInitTemp(void)
{
  char    Buffer[250];
  int     BufferSize;
  float   Const[11];
  int     Tmp;
  char    *Str;
  LFILE   *pFile;

  mkdir("../prjs/TempTest",DIRPERMISSIONS);
  chmod("../prjs/TempTest",DIRPERMISSIONS);
  sync();

  Tmp  =  0;
  pFile = fopen ("../prjs/Const/TempConst.rtf","r");
  if (pFile != NULL)
  {
    do
    {
      Str           =  fgets(Buffer,250,pFile);
      Buffer[249]   =  0;
      if (Str != NULL)
      {
        if ((Buffer[0] != '/') && (Buffer[0] != '*') && (Buffer[0] != ' '))
        {
          Const[Tmp]  =  DATAF_NAN;
          if (sscanf(Buffer,"%f",&Const[Tmp]) != 1)
          {
            Const[Tmp]  =  DATAF_NAN;
          }
          Tmp++;
        }
      }
    }
    while (Str != NULL);
    fclose (pFile);

    R_bat_init            =  Const[0];
    heat_cap_bat          =  Const[1];
    K_bat_loss_to_elec    =  Const[2];
    K_bat_gain_from_elec  =  Const[3];
    K_bat_to_room         =  Const[4];
    battery_power_boost   =  Const[5];
    R_bat_neg_gain        =  Const[6];
    K_elec_heat_slope     =  Const[7];
    K_elec_loss_to_bat    =  Const[8];
    K_elec_gain_from_bat  =  Const[9];
    K_elec_to_room        =  Const[10];

  }

  TempFile  =  open("../prjs/TempTest/TempFile.rtf",O_CREAT | O_WRONLY | O_APPEND | O_SYNC,FILEPERMISSIONS);
  chmod("../prjs/TempTest/TempFile.rtf",FILEPERMISSIONS);
  if (TempFile >= MIN_HANDLE)
  {
    if (Tmp)
    {
      BufferSize  =  snprintf(Buffer,250,"* TempConst.rtf ************************\r\n");
    }
    else
    {
      BufferSize  =  snprintf(Buffer,250,"* Build in *****************************\r\n");
    }
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  R_bat_init           = %13.9f\r\n",R_bat_init);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  heat_cap_bat         = %13.9f\r\n",heat_cap_bat);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_bat_loss_to_elec   = %13.9f\r\n",K_bat_loss_to_elec);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_bat_gain_from_elec = %13.9f\r\n",K_bat_gain_from_elec);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_bat_to_room        = %13.9f\r\n",K_bat_to_room);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  battery_power_boost  = %13.9f\r\n",battery_power_boost);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  R_bat_neg_gain       = %13.9f\r\n",R_bat_neg_gain);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_elec_heat_slope    = %13.9f\r\n",K_elec_heat_slope);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_elec_loss_to_bat   = %13.9f\r\n",K_elec_loss_to_bat);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_elec_gain_from_bat = %13.9f\r\n",K_elec_gain_from_bat);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"  K_elec_to_room       = %13.9f\r\n",K_elec_to_room);
    write(TempFile,Buffer,BufferSize);
    BufferSize  =  snprintf(Buffer,250,"****************************************\r\n");
    write(TempFile,Buffer,BufferSize);
  }
  UiInstance.TempTimer  =  (UiInstance.MilliSeconds - CALL_INTERVAL);
  cUiCheckTemp();
}


void      cUiExitTemp(void)
{
  if (TempFile >= MIN_HANDLE)
  {
    close(TempFile);
    TempFile  =  -1;
  }
}
#endif

#endif


IMGDATA   DownloadSuccesSound[]    =  { opINFO,LC0(GET_VOLUME),LV0(0),opSOUND,LC0(PLAY),LV0(0),LCS,'u','i','/','D','o','w','n','l','o','a','d','S','u','c','c','e','s',0,opSOUND_READY,opOBJECT_END };

void      cUiDownloadSuccesSound(void)
{
  VARDATA Locals[1];

  ExecuteByteCode(DownloadSuccesSound,NULL,Locals);
}


void      cUiButtonClr(void)
{
  DATA8   Button;

  for (Button = 0;Button < BUTTONS;Button++)
  {
    UiInstance.ButtonState[Button] &= ~BUTTON_CLR;
  }
}


void      cUiButtonFlush(void)
{
  DATA8   Button;

  for (Button = 0;Button < BUTTONS;Button++)
  {
    UiInstance.ButtonState[Button] &= ~BUTTON_FLUSH;
  }
}


void      cUiSetLed(DATA8 State)
{
  DATA8   Buffer[2];

  UiInstance.LedState  =  State;

  if (UiInstance.UiFile >= MIN_HANDLE)
  {
    if (UiInstance.Warnlight)
    {
      if ((State == LED_GREEN_FLASH) || (State == LED_RED_FLASH) || (State == LED_ORANGE_FLASH))
      {
        Buffer[0]  =  LED_ORANGE_FLASH + '0';
      }
      else
      {
        if ((State == LED_GREEN_PULSE) || (State == LED_RED_PULSE) || (State == LED_ORANGE_PULSE))
        {
          Buffer[0]  =  LED_ORANGE_PULSE + '0';
        }
        else
        {
          Buffer[0]  =  LED_ORANGE + '0';
        }
      }
    }
    else
    {
      Buffer[0]  =  UiInstance.LedState + '0';
    }
    Buffer[1]  =  0;
    write(UiInstance.UiFile,Buffer,2);
  }
}


void      cUiAlive(void)
{
  UiInstance.SleepTimer  =  0;
}


RESULT    cUiInit(void)
{
  RESULT  Result = OK;
  UI      *pUiTmp;
  ANALOG  *pAdcTmp;
  UBYTE   Tmp;
  DATAF   Hw;
  char    Buffer[32];
  char    OsBuf[2000];
  int     Lng;
  int     Start;
  int     Sid;
  struct  ifreq Sifr;
  struct  utsname *pOs;
  struct  tm tm;

  cUiAlive();

  UiInstance.ReadyForWarnings   =  0;
  UiInstance.UpdateState        =  0;
  UiInstance.RunLedEnabled      =  0;
  UiInstance.RunScreenEnabled   =  0;
  UiInstance.TopLineEnabled     =  0;
  UiInstance.BackButtonBlocked  =  0;
  UiInstance.Escape             =  0;
  UiInstance.KeyBufIn           =  0;
  UiInstance.Keys               =  0;
  UiInstance.UiWrBufferSize     =  0;

  UiInstance.ScreenBusy         =  0;
  UiInstance.ScreenBlocked      =  0;
  UiInstance.ScreenPrgId        =  -1;
  UiInstance.ScreenObjId        =  -1;

  UiInstance.PowerInitialized   =  0;
  UiInstance.ShutDown           =  0;

  UiInstance.PowerShutdown      =  0;
  UiInstance.PowerState         =  0;
  UiInstance.VoltShutdown       =  0;
  UiInstance.Warnlight          =  0;
  UiInstance.Warning            =  0;
  UiInstance.WarningShowed      =  0;
  UiInstance.WarningConfirmed   =  0;
  UiInstance.VoltageState       =  0;

  UiInstance.pLcd               =  &UiInstance.LcdSafe;
  UiInstance.pUi                =  &UiInstance.UiSafe;
  UiInstance.pAnalog            =  &UiInstance.Analog;

  UiInstance.Browser.PrgId      =  0;
  UiInstance.Browser.ObjId      =  0;

  UiInstance.Tbatt              =  0.0;
  UiInstance.Vbatt              =  9.0;
  UiInstance.Ibatt              =  0.0;
  UiInstance.Imotor             =  0.0;
  UiInstance.Iintegrated        =  0.0;
#ifdef Linux_X86
  UiInstance.Ibatt              =  0.1;
  UiInstance.Imotor             =  0.0;
#endif

  Result          =  dTerminalInit();

  UiInstance.PowerFile  =  open(POWER_DEVICE_NAME,O_RDWR);
  UiInstance.UiFile     =  open(UI_DEVICE_NAME,O_RDWR | O_SYNC);
  UiInstance.AdcFile    =  open(ANALOG_DEVICE_NAME,O_RDWR | O_SYNC);

  dLcdInit((*UiInstance.pLcd).Lcd);

  Hw  =  0;
  if (UiInstance.UiFile >= MIN_HANDLE)
  {
    pUiTmp    =  (UI*)mmap(0, sizeof(UI), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, UiInstance.UiFile, 0);

    if (pUiTmp == MAP_FAILED)
    {
#ifndef Linux_X86
      LogErrorNumber(UI_SHARED_MEMORY);
      Result  =  FAIL;
#endif
    }
    else
    {
      UiInstance.pUi     =  pUiTmp;
    }

    read(UiInstance.UiFile,UiInstance.HwVers,HWVERS_SIZE);
    sscanf(&UiInstance.HwVers[1],"%f",&Hw);
  }
  else
  {
#ifndef Linux_X86
    LogErrorNumber(UI_DEVICE_FILE_NOT_FOUND);
    Result  =  FAIL;
#else
    snprintf(UiInstance.HwVers,HWVERS_SIZE,"X86");
#endif
  }
  Hw *=  (DATAF)10;
  UiInstance.Hw  =  (DATA8)Hw;

  if (UiInstance.AdcFile >= MIN_HANDLE)
  {
    pAdcTmp  =  (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, UiInstance.AdcFile, 0);

    if (pAdcTmp == MAP_FAILED)
    {
#ifndef Linux_X86
      LogErrorNumber(ANALOG_SHARED_MEMORY);
      Result  =  FAIL;
#endif
    }
    else
    {
      UiInstance.pAnalog  =  pAdcTmp;
    }
  }
  else
  {
#ifndef Linux_X86
    LogErrorNumber(ANALOG_DEVICE_FILE_NOT_FOUND);
    Result  =  FAIL;
#endif
  }

  if (SPECIALVERS < '0')
  {
    snprintf(UiInstance.FwVers,FWVERS_SIZE,"V%4.2f",VERS);
  }
  else
  {
    snprintf(UiInstance.FwVers,FWVERS_SIZE,"V%4.2f%c",VERS,SPECIALVERS);
  }


  snprintf(Buffer,32,"%s %s",__DATE__,__TIME__);
  strptime((const char*)Buffer,(const char*)"%B %d %Y %H:%M:%S",(struct tm*)&tm);
  strftime(UiInstance.FwBuild,FWBUILD_SIZE,"%y%m%d%H%M",&tm);

  pOs  =  (struct utsname*)OsBuf;
  uname(pOs);

  snprintf(UiInstance.OsVers,OSVERS_SIZE,"%s %s",(*pOs).sysname,(*pOs).release);

  sprintf((char*)UiInstance.OsBuild,"?");

  Lng  =  strlen((*pOs).version) - 9;
  if (Lng > 0)
  {
    (*pOs).version[Lng++]  =  ' ';
    (*pOs).version[Lng++]  =  ' ';
    (*pOs).version[Lng++]  =  ' ';
    (*pOs).version[Lng++]  =  ' ';

    Lng     =  strlen((*pOs).version);
    Tmp     =  0;
    Start   =  0;

    while ((Start < Lng) && (Tmp == 0))
    {
      if (strptime((const char*)&(*pOs).version[Start],(const char*)"%B %d %H:%M:%S %Y",(struct tm*)&tm) != NULL)
      {
        Tmp  =  1;
      }
      Start++;
    }
    if (Tmp)
    {
      strftime((char*)UiInstance.OsBuild,OSBUILD_SIZE,"%y%m%d%H%M",&tm);
    }
  }

  UiInstance.IpAddr[0]  =  0;
  Sid   =  socket(AF_INET,SOCK_DGRAM,0);
  Sifr.ifr_addr.sa_family   =  AF_INET;
  strncpy(Sifr.ifr_name,"eth0",IFNAMSIZ - 1);
  if (ioctl(Sid,SIOCGIFADDR,&Sifr) >= 0)
  {
    snprintf(UiInstance.IpAddr,IPADDR_SIZE,"%s",inet_ntoa(((struct sockaddr_in *)&Sifr.ifr_addr)->sin_addr));
  }
  close(Sid);

  cUiButtonClr();

  UiInstance.BattIndicatorHigh      =  BATT_INDICATOR_HIGH;
  UiInstance.BattIndicatorLow       =  BATT_INDICATOR_LOW;
  UiInstance.BattWarningHigh        =  BATT_WARNING_HIGH;
  UiInstance.BattWarningLow         =  BATT_WARNING_LOW;
  UiInstance.BattShutdownHigh       =  BATT_SHUTDOWN_HIGH;
  UiInstance.BattShutdownLow        =  BATT_SHUTDOWN_LOW;

  UiInstance.Accu  =  0;
  if (UiInstance.PowerFile >= MIN_HANDLE)
  {
    read(UiInstance.PowerFile,Buffer,2);
    if (Buffer[0] == '1')
    {
      UiInstance.Accu               =  1;
      UiInstance.BattIndicatorHigh  =  ACCU_INDICATOR_HIGH;
      UiInstance.BattIndicatorLow   =  ACCU_INDICATOR_LOW;
      UiInstance.BattWarningHigh    =  ACCU_WARNING_HIGH;
      UiInstance.BattWarningLow     =  ACCU_WARNING_LOW;
      UiInstance.BattShutdownHigh   =  ACCU_SHUTDOWN_HIGH;
      UiInstance.BattShutdownLow    =  ACCU_SHUTDOWN_LOW;
    }
  }

#ifdef DEBUG_VIRTUAL_BATT_TEMP
  cUiInitTemp();
#endif

  return (Result);
}


RESULT    cUiOpen(void)
{
  RESULT  Result = FAIL;

  // Save screen before run
  LCDCopy(&UiInstance.LcdSafe,&UiInstance.LcdPool[0],sizeof(LCD));

  cUiButtonClr();
  cUiSetLed(LED_GREEN_PULSE);
  UiInstance.RunScreenEnabled   =  3;
  UiInstance.RunLedEnabled      =  1;
  UiInstance.TopLineEnabled     =  0;

  Result  =  OK;

  return (Result);
}


RESULT    cUiClose(void)
{
  RESULT  Result = FAIL;

  UiInstance.Warning           &= ~WARNING_BUSY;
  UiInstance.RunLedEnabled      =  0;
  UiInstance.RunScreenEnabled   =  0;
  UiInstance.TopLineEnabled     =  1;
  UiInstance.BackButtonBlocked  =  0;
  UiInstance.Browser.NeedUpdate =  1;
  cUiSetLed(LED_GREEN);

  cUiButtonClr();

  Result  =  OK;

  return (Result);
}


RESULT    cUiExit(void)
{
  RESULT  Result = FAIL;

#ifdef DEBUG_VIRTUAL_BATT_TEMP
  cUiExitTemp();
#endif

  Result  =  dTerminalExit();

  if (UiInstance.AdcFile >= MIN_HANDLE)
  {
    munmap(UiInstance.pAnalog,sizeof(ANALOG));
    close(UiInstance.AdcFile);
  }

  if (UiInstance.UiFile >= MIN_HANDLE)
  {
    munmap(UiInstance.pUi,sizeof(UI));
    close(UiInstance.UiFile);
  }

  if (UiInstance.PowerFile >= MIN_HANDLE)
  {
    close(UiInstance.PowerFile);
  }

  Result  =  OK;

  return (Result);
}


void      cUiUpdateButtons(DATA16 Time)
{
  DATA8   Button;

  for (Button = 0;Button < BUTTONS;Button++)
  {

    // Check real hardware buttons

    if ((*UiInstance.pUi).Pressed[Button])
    { // Button pressed

      if (UiInstance.ButtonDebounceTimer[Button] == 0)
      { // Button activated

        UiInstance.ButtonState[Button]           |=  BUTTON_ACTIVE;
      }

      UiInstance.ButtonDebounceTimer[Button]      =  BUTTON_DEBOUNCE_TIME;
    }
    else
    { // Button not pressed

      if (UiInstance.ButtonDebounceTimer[Button] > 0)
      { // Debounce delay

        UiInstance.ButtonDebounceTimer[Button]   -=  Time;

        if (UiInstance.ButtonDebounceTimer[Button] <= 0)
        { // Button released

          UiInstance.ButtonState[Button]         &= ~BUTTON_ACTIVE;
          UiInstance.ButtonDebounceTimer[Button]  =  0;
        }
      }
    }

    // Check virtual buttons (hardware, direct command, PC)

    if (UiInstance.ButtonState[Button] & BUTTON_ACTIVE)
    {
      if (!(UiInstance.ButtonState[Button] & BUTTON_PRESSED))
      { // Button activated

        UiInstance.Activated                      =  BUTTON_SET;
        UiInstance.ButtonState[Button]           |=  BUTTON_PRESSED;
        UiInstance.ButtonState[Button]           |=  BUTTON_ACTIVATED;
        UiInstance.ButtonTimer[Button]            =  0;
        UiInstance.ButtonRepeatTimer[Button]      =  BUTTON_START_REPEAT_TIME;
      }

      // Control auto repeat

      if (UiInstance.ButtonRepeatTimer[Button] > Time)
      {
        UiInstance.ButtonRepeatTimer[Button] -=  Time;
      }
      else
      {
        if ((Button != 1) && (Button != 5))
        { // No repeat on ENTER and BACK

          UiInstance.Activated                     |=  BUTTON_SET;
          UiInstance.ButtonState[Button]           |=  BUTTON_ACTIVATED;
          UiInstance.ButtonRepeatTimer[Button]      =  BUTTON_REPEAT_TIME;
        }
      }

      // Control long press

      UiInstance.ButtonTimer[Button]               +=  Time;

      if (UiInstance.ButtonTimer[Button] >= LONG_PRESS_TIME)
      {
        if (!(UiInstance.ButtonState[Button] & BUTTON_LONG_LATCH))
        { // Only once

          UiInstance.ButtonState[Button]           |=  BUTTON_LONG_LATCH;

#ifdef BUFPRINTSIZE
          if (Button == 2)
          {
            UiInstance.Activated                   |=  BUTTON_BUFPRINT;
          }
#endif
        }
        UiInstance.ButtonState[Button]           |=  BUTTON_LONGPRESS;
      }

    }
    else
    {
      if ((UiInstance.ButtonState[Button] & BUTTON_PRESSED))
      { // Button released

        UiInstance.ButtonState[Button]           &= ~BUTTON_PRESSED;
        UiInstance.ButtonState[Button]           &= ~BUTTON_LONG_LATCH;
        UiInstance.ButtonState[Button]           |=  BUTTON_BUMBED;
      }
    }
  }
}


#ifndef LEGO_SIMULATION
RESULT    cUiUpdateInput(void)
{
  UBYTE   Key;

  if (GetTerminalEnable())
  {

    if (dTerminalRead(&Key) == OK)
    {
      switch (Key)
      {
        case ' ' :
        {
          UiInstance.Escape  =  Key;
        }
        break;

        case '<' :
        {
          UiInstance.Escape  =  Key;
        }
        break;

        case '\r' :
        case '\n' :
        {
          if (UiInstance.KeyBufIn)
          {
            UiInstance.Keys      =  UiInstance.KeyBufIn;
            UiInstance.KeyBufIn  =  0;
          }
        }
        break;

        default :
        {
          UiInstance.KeyBuffer[UiInstance.KeyBufIn]  =  Key;
          if (++UiInstance.KeyBufIn >= KEYBUF_SIZE)
          {
            UiInstance.KeyBufIn--;
          }
          UiInstance.KeyBuffer[UiInstance.KeyBufIn]  =  0;
        }
        break;

      }
    }
  }
  dLcdRead();

  return (OK);
}
#endif


DATA8     cUiEscape(void)
{
  DATA8   Result;

  Result              =  UiInstance.Escape;
  UiInstance.Escape   =  0;

  return (Result);
}


void      cUiTestpin(DATA8 State)
{
  DATA8   Data8;

  Data8  =  State;
  if (UiInstance.PowerFile >= MIN_HANDLE)
  {
    write(UiInstance.PowerFile,&Data8,1);
  }
}


UBYTE     AtoN(DATA8 Char)
{
  UBYTE   Tmp = 0;

  if ((Char >= '0') && (Char <= '9'))
  {
    Tmp  =  (UBYTE)(Char - '0');
  }
  else
  {
    Char  |=  0x20;

    if ((Char >= 'a') && (Char <= 'f'))
    {
      Tmp  =  (UBYTE)(Char - 'a') + 10;
    }
  }

  return (Tmp);
}


void      cUiFlushBuffer(void)
{
  if (UiInstance.UiWrBufferSize)
  {
    if (GetTerminalEnable())
    {
      dTerminalWrite((UBYTE*)UiInstance.UiWrBuffer,UiInstance.UiWrBufferSize);
    }
    UiInstance.UiWrBufferSize              =  0;
  }
}


void      cUiWriteString(DATA8 *pString)
{
  while (*pString)
  {
    UiInstance.UiWrBuffer[UiInstance.UiWrBufferSize] = *pString;
    if (++UiInstance.UiWrBufferSize >= UI_WR_BUFFER_SIZE)
    {
      cUiFlushBuffer();
    }
    pString++;
  }
}


#define   REAL_ANY_BUTTON   6
#define   REAL_NO_BUTTON    7


DATA8     MappedToReal[BUTTONTYPES] =
{
// Mapped               Real
  [UP_BUTTON]         = 0,
  [ENTER_BUTTON]      = 1,
  [DOWN_BUTTON]       = 2,
  [RIGHT_BUTTON]      = 3,
  [LEFT_BUTTON]       = 4,
  [BACK_BUTTON]       = 5,
  [ANY_BUTTON]        = REAL_ANY_BUTTON,
  [NO_BUTTON]         = REAL_NO_BUTTON
};


DATA8     cUiButtonRemap(DATA8 Mapped)
{
  DATA8   Real;

  if ((Mapped >= 0) && (Mapped < BUTTONTYPES))
  {
    Real  =  MappedToReal[Mapped];
  }
  else
  {
    Real  =  REAL_ANY_BUTTON;
  }

  return (Real);
}


void      cUiSetPress(DATA8 Button,DATA8 Press)
{
  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (Press)
    {
      UiInstance.ButtonState[Button] |=  BUTTON_ACTIVE;
    }
    else
    {
      UiInstance.ButtonState[Button] &= ~BUTTON_ACTIVE;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      if (Press)
      {
        for (Button = 0;Button < BUTTONS;Button++)
        {
          UiInstance.ButtonState[Button] |=  BUTTON_ACTIVE;
        }
      }
      else
      {
        for (Button = 0;Button < BUTTONS;Button++)
        {
          UiInstance.ButtonState[Button] &= ~BUTTON_ACTIVE;
        }
      }
    }
  }
}


DATA8     cUiGetPress(DATA8 Button)
{
  DATA8   Result = 0;

  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (UiInstance.ButtonState[Button] & BUTTON_PRESSED)
    {
      Result                           =  1;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      for (Button = 0;Button < BUTTONS;Button++)
      {
        if (UiInstance.ButtonState[Button] & BUTTON_PRESSED)
        {
          Result                           =  1;
        }
      }
    }
  }

  return (Result);
}


DATA8     cUiTestShortPress(DATA8 Button)
{
  DATA8   Result = 0;

  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (UiInstance.ButtonState[Button] & BUTTON_ACTIVATED)
    {
      Result                           =  1;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      for (Button = 0;Button < BUTTONS;Button++)
      {
        if (UiInstance.ButtonState[Button] & BUTTON_ACTIVATED)
        {
          Result                           =  1;
        }
      }
    }
    else
    {
      if (Button == REAL_NO_BUTTON)
      {
        Result  =  1;
        for (Button = 0;Button < BUTTONS;Button++)
        {
          if (UiInstance.ButtonState[Button] & BUTTON_ACTIVATED)
          {
            Result                           =  0;
          }
        }
      }
    }
  }

  return (Result);
}


DATA8     cUiGetShortPress(DATA8 Button)
{
  DATA8   Result = 0;

  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (UiInstance.ButtonState[Button] & BUTTON_ACTIVATED)
    {
      UiInstance.ButtonState[Button] &= ~BUTTON_ACTIVATED;
      Result                           =  1;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      for (Button = 0;Button < BUTTONS;Button++)
      {
        if (UiInstance.ButtonState[Button] & BUTTON_ACTIVATED)
        {
          UiInstance.ButtonState[Button] &= ~BUTTON_ACTIVATED;
          Result                           =  1;
        }
      }
    }
    else
    {
      if (Button == REAL_NO_BUTTON)
      {
        Result  =  1;
        for (Button = 0;Button < BUTTONS;Button++)
        {
          if (UiInstance.ButtonState[Button] & BUTTON_ACTIVATED)
          {
            UiInstance.ButtonState[Button] &= ~BUTTON_ACTIVATED;
            Result                           =  0;
          }
        }
      }
    }
  }
  if (Result)
  {
    UiInstance.Click  =  1;
  }

  return (Result);
}


DATA8     cUiGetBumbed(DATA8 Button)
{
  DATA8   Result = 0;

  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (UiInstance.ButtonState[Button] & BUTTON_BUMBED)
    {
      UiInstance.ButtonState[Button] &= ~BUTTON_BUMBED;
      Result                           =  1;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      for (Button = 0;Button < BUTTONS;Button++)
      {
        if (UiInstance.ButtonState[Button] & BUTTON_BUMBED)
        {
          UiInstance.ButtonState[Button] &= ~BUTTON_BUMBED;
          Result                           =  1;
        }
      }
    }
    else
    {
      if (Button == REAL_NO_BUTTON)
      {
        Result  =  1;
        for (Button = 0;Button < BUTTONS;Button++)
        {
          if (UiInstance.ButtonState[Button] & BUTTON_BUMBED)
          {
            UiInstance.ButtonState[Button] &= ~BUTTON_BUMBED;
            Result                           =  0;
          }
        }
      }
    }
  }

  return (Result);
}


DATA8     cUiTestLongPress(DATA8 Button)
{
  DATA8   Result = 0;

  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (UiInstance.ButtonState[Button] & BUTTON_LONGPRESS)
    {
      Result                           =  1;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      for (Button = 0;Button < BUTTONS;Button++)
      {
        if (UiInstance.ButtonState[Button] & BUTTON_LONGPRESS)
        {
          Result                           =  1;
        }
      }
    }
    else
    {
      if (Button == REAL_NO_BUTTON)
      {
        Result  =  1;
        for (Button = 0;Button < BUTTONS;Button++)
        {
          if (UiInstance.ButtonState[Button] & BUTTON_LONGPRESS)
          {
            Result                           =  0;
          }
        }
      }
    }
  }

  return (Result);
}


DATA8     cUiGetLongPress(DATA8 Button)
{
  DATA8   Result = 0;

  Button  =  cUiButtonRemap(Button);

  if (Button < BUTTONS)
  {
    if (UiInstance.ButtonState[Button] & BUTTON_LONGPRESS)
    {
      UiInstance.ButtonState[Button] &= ~BUTTON_LONGPRESS;
      Result                           =  1;
    }
  }
  else
  {
    if (Button == REAL_ANY_BUTTON)
    {
      for (Button = 0;Button < BUTTONS;Button++)
      {
        if (UiInstance.ButtonState[Button] & BUTTON_LONGPRESS)
        {
          UiInstance.ButtonState[Button] &= ~BUTTON_LONGPRESS;
          Result                           =  1;
        }
      }
    }
    else
    {
      if (Button == REAL_NO_BUTTON)
      {
        Result  =  1;
        for (Button = 0;Button < BUTTONS;Button++)
        {
          if (UiInstance.ButtonState[Button] & BUTTON_LONGPRESS)
          {
            UiInstance.ButtonState[Button] &= ~BUTTON_LONGPRESS;
            Result                           =  0;
          }
        }
      }
    }
  }
  if (Result)
  {
    UiInstance.Click  =  1;
  }

  return (Result);
}


DATA16    cUiTestHorz(void)
{
  DATA16  Result = 0;

  if (cUiTestShortPress(LEFT_BUTTON))
  {
    Result  = -1;
  }
  if (cUiTestShortPress(RIGHT_BUTTON))
  {
    Result  =  1;
  }

  return (Result);
}


DATA16    cUiGetHorz(void)
{
  DATA16  Result = 0;

  if (cUiGetShortPress(LEFT_BUTTON))
  {
    Result            = -1;
  }
  if (cUiGetShortPress(RIGHT_BUTTON))
  {
    Result            =  1;
  }

  return (Result);
}


DATA16    cUiGetVert(void)
{
  DATA16  Result = 0;

  if (cUiGetShortPress(UP_BUTTON))
  {
    Result            = -1;
  }
  if (cUiGetShortPress(DOWN_BUTTON))
  {
    Result            =  1;
  }

  return (Result);
}


DATA8     cUiWaitForPress(void)
{
  DATA8   Result = 0;

  Result  =  cUiTestShortPress(ANY_BUTTON);

  return (Result);
}


DATA16    cUiAlignX(DATA16 X)
{
  return ((X + 7) & ~7);
}


#define   SHUNT_IN        0.11              //  [Ohm]
#define   AMP_CIN         22.0              //  [Times]

#define   EP2_SHUNT_IN    0.05              //  [Ohm]
#define   EP2_AMP_CIN     15.0              //  [Times]

#define   SHUNT_OUT       0.055             //  [Ohm]
#define   AMP_COUT        19.0              //  [Times]

#define   VCE             0.05              //  [V]
#define   AMP_VIN         0.5               //  [Times]

#define   AVR_CIN         300
#define   AVR_COUT        30
#define   AVR_VIN         30

#define   CNT_V(C)    (((DATAF)C * (DATAF)ADC_REF) / ((DATAF)ADC_RES * (DATAF)1000.0))


void      cUiUpdateCnt(void)
{
  if (UiInstance.PowerInitialized)
  {
    UiInstance.CinCnt  *= (DATAF)(AVR_CIN - 1);
    UiInstance.CoutCnt *= (DATAF)(AVR_COUT - 1);
    UiInstance.VinCnt  *= (DATAF)(AVR_VIN - 1);

    UiInstance.CinCnt  +=  (DATAF)(*UiInstance.pAnalog).BatteryCurrent;
    UiInstance.CoutCnt +=  (DATAF)(*UiInstance.pAnalog).MotorCurrent;
    UiInstance.VinCnt  +=  (DATAF)(*UiInstance.pAnalog).Cell123456;

    UiInstance.CinCnt  /= (DATAF)(AVR_CIN);
    UiInstance.CoutCnt /= (DATAF)(AVR_COUT);
    UiInstance.VinCnt  /= (DATAF)(AVR_VIN);
  }
  else
  {
    UiInstance.CinCnt   =  (DATAF)(*UiInstance.pAnalog).BatteryCurrent;
    UiInstance.CoutCnt  =  (DATAF)(*UiInstance.pAnalog).MotorCurrent;
    UiInstance.VinCnt   =  (DATAF)(*UiInstance.pAnalog).Cell123456;
    UiInstance.PowerInitialized  =  1;
  }
}


void      cUiUpdatePower(void)
{
#ifndef Linux_X86
  DATAF   CinV;
  DATAF   CoutV;

  if ((UiInstance.Hw == FINAL) || (UiInstance.Hw == FINALB))
  {
    CinV                =  CNT_V(UiInstance.CinCnt) / AMP_CIN;
    UiInstance.Vbatt    =  (CNT_V(UiInstance.VinCnt) / AMP_VIN) + CinV + VCE;

    UiInstance.Ibatt    =  CinV / SHUNT_IN;
    CoutV               =  CNT_V(UiInstance.CoutCnt) / AMP_COUT;
    UiInstance.Imotor   =  CoutV / SHUNT_OUT;

  }
  else
  {
    CinV                =  CNT_V(UiInstance.CinCnt) / EP2_AMP_CIN;
    UiInstance.Vbatt    =  (CNT_V(UiInstance.VinCnt) / AMP_VIN) + CinV + VCE;

    UiInstance.Ibatt    =  CinV / EP2_SHUNT_IN;
    UiInstance.Imotor   =  0;

  }

#endif
#ifdef DEBUG_TEMP_SHUTDOWN

  UiInstance.Vbatt  =  7.0;
  UiInstance.Ibatt  =  5.0;

#endif
}


//#define   TRACK_UPDATE


/*! \page pmbattind Battery Indicator
 *
 *  <hr size="1"/>
 *\n
 *  The battery indicator is located in the top right corner of the display and is updated when the display
 *  is updated and the "TopLine" is enabled (and at least every 400 mS).
 *  The range from BATT_INDICATOR_LOW to BATT_INDICATOR_HIGH is divided into three pictures and the appropriate
 *  one is shown: an empty battery when the voltage is below or equal to BATT_INDICATOR_LOW and a full
 *  battery when the voltage is above or equal to BATT_INDICATOR_HIGH.
 *  If a rechargeable accu is used the ranges ACCU_INDICATOR_LOW and ACCU_INDICATOR_HIGH is used.
\verbatim


                          V
                          ^
                          |
                          |
                          |
                          |    Full battery picture
  XXXX_INDICATOR_HIGH     -         -         -         -
                          |    Three/fourth full battery picture
                          -         -         -         -
                          |    Half full battery picture
                          -         -         -         -
                          |    One/fourth full battery picture
  XXXX_INDICATOR_LOW      -         -         -         -
                          |    Empty battery picture
                          |
                          |
                          |
                          |
                          |
                          |
                          |
                          '-----------------------------------

  XXXX = BATT/ACCU

 \endverbatim
 */


#define   TOP_BATT_ICONS     5
UBYTE     TopLineBattIconMap[TOP_BATT_ICONS] =
{
  SICON_BATT_0,           //  0
  SICON_BATT_1,           //  1
  SICON_BATT_2,           //  2
  SICON_BATT_3,           //  3
  SICON_BATT_4            //  4
};


#define   TOP_BT_ICONS       4
UBYTE     TopLineBtIconMap[TOP_BT_ICONS] =
{
  SICON_BT_ON,            //  001
  SICON_BT_VISIBLE,       //  011
  SICON_BT_CONNECTED,     //  101
  SICON_BT_CONNVISIB,     //  111
};


#define   TOP_WIFI_ICONS     4
UBYTE     TopLineWifiIconMap[TOP_WIFI_ICONS] =
{
  SICON_WIFI_3,           //  001
  SICON_WIFI_3,           //  011
  SICON_WIFI_CONNECTED,   //  101
  SICON_WIFI_CONNECTED,   //  111
};


void      cUiUpdateTopline(void)
{
  DATA16  X1,X2;
  DATA16  V;
  DATA8   BtStatus;
  DATA8   WifiStatus;
  DATA8   TmpStatus;
  DATA8   Name[NAME_LENGTH + 1];

#ifdef TRACK_UPDATE
  static  DATA16 Counter = 0;
  static  char   Buffer[10];
#endif
#ifdef DEBUG_VIRTUAL_BATT_TEMP
  char    TempBuf[10];
#endif
#ifdef ENABLE_MEMORY_TEST
  DATA32  Total;
  DATA32  Free;
#endif

  if (UiInstance.TopLineEnabled)
  {
    // Clear top line
    LCDClearTopline(UiInstance.pLcd);

#ifdef TRACK_UPDATE
    Counter++;
    sprintf(Buffer,"%d",Counter);
    dLcdDrawText((*UiInstance.pLcd).Lcd,FG_COLOR,16,1,SMALL_FONT,(DATA8*)Buffer);
#endif
    // Show BT status
    TmpStatus   =  0;
    BtStatus    =  cComGetBtStatus();
    if (BtStatus > 0)
    {
      TmpStatus   =  1;
      BtStatus >>= 1;
      if ((BtStatus >= 0 ) && (BtStatus < TOP_BT_ICONS))
      {
        dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,0,1,SMALL_ICON,TopLineBtIconMap[BtStatus]);
      }
    }
    if (UiInstance.BtOn != TmpStatus)
    {
      UiInstance.BtOn       =  TmpStatus;
      UiInstance.UiUpdate   =  1;
    }

    // Show WIFI status
    TmpStatus   =  0;
    WifiStatus  =  cComGetWifiStatus();
    if (WifiStatus > 0)
    {
      TmpStatus   =  1;
      WifiStatus >>= 1;
      if ((WifiStatus >= 0 ) && (WifiStatus < TOP_WIFI_ICONS))
      {
        dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,16,1,SMALL_ICON,TopLineWifiIconMap[WifiStatus]);
      }
    }
    if (UiInstance.WiFiOn != TmpStatus)
    {
      UiInstance.WiFiOn     =  TmpStatus;
      UiInstance.UiUpdate   =  1;
    }
#ifdef DEBUG_VIRTUAL_BATT_TEMP
    snprintf(TempBuf,10,"%4.1f",UiInstance.Tbatt);
    dLcdDrawText((*UiInstance.pLcd).Lcd,FG_COLOR,32,1,SMALL_FONT,(DATA8*)TempBuf);
#endif

    // Show brick name
    cComGetBrickName(NAME_LENGTH + 1,Name);

    X1  =  dLcdGetFontWidth(SMALL_FONT);
    X2  =  LCD_WIDTH / X1;
    X2 -=  strlen((char*)Name);
    X2 /=  2;
    X2 *=  X1;
    dLcdDrawText((*UiInstance.pLcd).Lcd,FG_COLOR,X2,1,SMALL_FONT,Name);

#ifdef ENABLE_PERFORMANCE_TEST
    X1  =  100;
    X2  =  (DATA16)VMInstance.PerformTime;
    X2  =  (X2 * 40) / 1000;
    if (X2 > 40)
    {
      X2  =  40;
    }
    dLcdRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1,3,40,5);
    dLcdFillRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1,3,X2,5);
#endif
#ifdef ENABLE_LOAD_TEST
    X1  =  100;
    X2  =  (DATA16)(UiInstance.Iintegrated * 1000.0);
    X2  =  (X2 * 40) / (DATA16)(LOAD_SHUTDOWN_HIGH * 1000.0);
    if (X2 > 40)
    {
      X2  =  40;
    }
    dLcdRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1,3,40,5);
    dLcdFillRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1,3,X2,5);
#endif
#ifdef ENABLE_MEMORY_TEST
    cMemoryGetUsage(&Total,&Free,0);

    X1  =  100;
    X2  =  (DATA16)((((Total - (DATA32)LOW_MEMORY) - Free) * (DATA32)40) / (Total - (DATA32)LOW_MEMORY));
    if (X2 > 40)
    {
      X2  =  40;
    }
    dLcdRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1,3,40,5);
    dLcdFillRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1,3,X2,5);
#endif

// 112,118,124,130,136,142,148
#ifdef ENABLE_STATUS_TEST
    X1  =  100;
    X2  =  102;
    for (V = 0;V < 7;V++)
    {
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,FG_COLOR,X2 + (V * 6),5);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,FG_COLOR,X2 + (V * 6),4);

      if ((VMInstance.Status & (0x40 >> V)))
      {
        dLcdFillRect((*UiInstance.pLcd).Lcd,FG_COLOR,X1 + (V * 6),2,5,6);
      }
    }
#endif

    // Calculate number of icons
    X1  =  dLcdGetIconWidth(SMALL_ICON);
    X2  =  (LCD_WIDTH - X1) / X1;

#ifndef Linux_X86
    // Show USB status
    if (cComGetUsbStatus())
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,(X2 - 1) * X1,1,SMALL_ICON,SICON_USB);
    }
#endif

#ifdef DEBUG_BACK_BLOCKED
    if (UiInstance.BackButtonBlocked)
    {
      dLcdFillRect((*UiInstance.pLcd).Lcd,FG_COLOR,100,2,5,6);
    }
#endif

    // Show battery
    V   =  (DATA16)(UiInstance.Vbatt * 1000.0);
    V  -=  UiInstance.BattIndicatorLow;
    V   =  (V * (TOP_BATT_ICONS - 1)) / (UiInstance.BattIndicatorHigh - UiInstance.BattIndicatorLow);
    if (V > (TOP_BATT_ICONS - 1))
    {
      V  =  (TOP_BATT_ICONS - 1);
    }
    if (V < 0)
    {
      V  =  0;
    }
    dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,X2 * X1,1,SMALL_ICON,TopLineBattIconMap[V]);

#ifdef DEBUG_RECHARGEABLE
    if (UiInstance.Accu == 0)
    {
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,FG_COLOR,176,4);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,FG_COLOR,176,5);
    }
#endif

    // Show bottom line
    dLcdDrawLine((*UiInstance.pLcd).Lcd,FG_COLOR,0,TOPLINE_HEIGHT,LCD_WIDTH,TOPLINE_HEIGHT);
  }
}


void      cUiUpdateLcd(void)
{
  UiInstance.Font  =  NORMAL_FONT;
  cUiUpdateTopline();
  dLcdUpdate(UiInstance.pLcd);
}


#include  "mindstorms.xbm"
#include  "Ani1x.xbm"
#include  "Ani2x.xbm"
#include  "Ani3x.xbm"
#include  "Ani4x.xbm"
#include  "Ani5x.xbm"
#include  "Ani6x.xbm"
#include  "POP2.xbm"
#include  "POP3.xbm"
#include  "PCApp.xbm"


void      cUiRunScreen(void)
{ // 100mS

  if (UiInstance.ScreenBlocked  ==  0)
  {
    switch (UiInstance.RunScreenEnabled)
    {
      case 0 :
      {
      }
      break;

      case 1 :
      { // 100mS

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 2 :
      { // 200mS

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 3 :
      {
        // Init animation number
        UiInstance.RunScreenNumber  =  1;

        // Clear screen
        LCDClear((*UiInstance.pLcd).Lcd);
        cUiUpdateLcd();


        // Enable top line

        // Draw fixed image
        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,39,mindstorms_width,mindstorms_height,(UBYTE*)mindstorms_bits);
        cUiUpdateLcd();

        // Draw user program name
        dLcdDrawText((*UiInstance.pLcd).Lcd,FG_COLOR,8,79,1,(DATA8*)VMInstance.Program[USER_SLOT].Name);
        cUiUpdateLcd();

        UiInstance.RunScreenTimer     =  0;
        UiInstance.RunScreenCounter   =  0;

        UiInstance.RunScreenEnabled++;
      }
      // FALL THROUGH

      case 4 :
      { //   0mS -> Ani1

        if (UiInstance.RunLedEnabled)
        {
          cUiSetLed(LED_GREEN_PULSE);
        }

        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,67,Ani1x_width,Ani1x_height,(UBYTE*)Ani1x_bits);
        cUiUpdateLcd();

        UiInstance.RunScreenTimer  =  UiInstance.MilliSeconds;
        UiInstance.RunScreenEnabled++;
      }
      break;

      case 5 :
      { // 100mS

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 6 :
      { // 200mS

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 7 :
      { // 300mS

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 8 :
      { // 400mS

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 9 :
      { // 500mS -> Ani2

        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,67,Ani2x_width,Ani2x_height,(UBYTE*)Ani2x_bits);
        cUiUpdateLcd();

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 10 :
      { // 600mS -> Ani3

        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,67,Ani3x_width,Ani3x_height,(UBYTE*)Ani3x_bits);
        cUiUpdateLcd();

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 11 :
      { // 700ms -> Ani4

        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,67,Ani4x_width,Ani4x_height,(UBYTE*)Ani4x_bits);
        cUiUpdateLcd();

        UiInstance.RunScreenEnabled++;
      }
      break;

      case 12 :
      { // 800ms -> Ani5

        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,67,Ani5x_width,Ani5x_height,(UBYTE*)Ani5x_bits);
        cUiUpdateLcd();

        UiInstance.RunScreenEnabled++;
      }
      break;

      default :
      { // 900ms -> Ani6

        dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,8,67,Ani6x_width,Ani6x_height,(UBYTE*)Ani6x_bits);
        cUiUpdateLcd();

        UiInstance.RunScreenEnabled   =  4;
      }
      break;

    }
  }
}

#ifndef   DISABLE_LOW_VOLTAGE

/*! \page pmbattsd Low Voltage Shutdown
 *
 *  <hr size="1"/>
 *\n
 *  Low voltage shutdown is testing the battery voltage against BATT_SHUTDOWN_LOW level and when the voltage is
 *  below that a running user program will be stopped, running motors will stop, a popup window will appear on the screen
 *  and a timer will be started. If the voltage not rises over the BATT_SHUTDOWN_LOW level within LOW_VOLTAGE_SHUTDOWN_TIME
 *  the brick will save to flash and power down.
\verbatim


                          V
                          ^
                          |
                          |
                          |
                          |
  XXXX_WARNING_HIGH       -    Make LEDs normal
                          |
                          |
                          |    Keep LED colors
                          |
                          |
  XXXX_WARNING_LOW        -    Make LEDs ORANGE
                          |
  XXXX_SHUTDOWN_HIGH      -         -         -         -
                          |
                          |    Reset shutdown timer
                          |
  XXXX_SHUTDOWN_LOW       -         -         -         -
                          |
                          |    Show popup and stop user program immediately
                          |
                          |    Power down after LOW_VOLTAGE_SHUTDOWN_TIME
                          |
                          |
                          '-----------------------------------

  XXXX = BATT/ACCU

 \endverbatim
 */

void      cUiCheckVoltage(void)
{ // 400mS

  cUiUpdatePower();

  if (UiInstance.Vbatt >= UiInstance.BattWarningHigh)
  {
    UiInstance.Warning &= ~WARNING_BATTLOW;
  }

  if (UiInstance.Vbatt <= UiInstance.BattWarningLow)
  {
    UiInstance.Warning |=  WARNING_BATTLOW;
  }

  if (UiInstance.Vbatt >= UiInstance.BattShutdownHigh)
  { // Good

    UiInstance.Warning &= ~WARNING_VOLTAGE;
  }

  if (UiInstance.Vbatt < UiInstance.BattShutdownLow)
  { // Bad

    UiInstance.Warning |=  WARNING_VOLTAGE;

    ProgramEnd(USER_SLOT);

    if ((UiInstance.MilliSeconds - UiInstance.VoltageTimer) >= LOW_VOLTAGE_SHUTDOWN_TIME)
    { // Realy bad

      UiInstance.ShutDown         =  1;
    }
  }
  else
  {
    UiInstance.VoltageTimer  =  UiInstance.MilliSeconds;
  }
}

#endif

#ifdef ENABLE_HIGH_CURRENT
/*! \page pmloadsd High Load Shutdown
 *
 *  <hr size="1"/>
 *\n
 *  High load shutdown is based on the total current "I" drawn from the battery. A virtual integrated current "Iint"
 *  simulates the load on and the temperature inside the battery and is maintained by integrating the draw current over time.
 *  LOAD_BREAK_EVEN is the level that defines if "Iint" integrates up or down.
 *  If or when "Iint" reaches the limit (LOAD_SHUTDOWN_HIGH or LOAD_SHUTDOWN_FAIL) the user program is stopped, motors are stopped
 *  and a popup screen appears on the display.
 *  The popup screen disappears when a button is pressed but no user program can be activated until the "Iint" has decreased below
 *  LOAD_BREAK_EVEN level.
 *
 \verbatim


                          I
                          ^
  LOAD_SHUTDOWN_FAIL     -|-         -         -         -         -         -|-
                          |                                                   |
                          |                                                   |
                          |                          Iint                     |
  LOAD_SHUTDOWN_HIGH     -|-         -         -    -/-                       |
                          |          ______         /.                        |
                          |         /      \       / .                        |
                          |        /        \_____/  .                        |
                          |   ,---/-,             ,--- I                      |
  LOAD_BREAK_EVEN        -|---'  /  '-----, ,-----'  .               I -------'
                          |     /         | |        .                        .
                          |____/          '-'        .                        .
                          |                          .                        .
                          '--------------------------|------------------------|------> T
                          | B |  A  |  B  |C|  B  |A|D|                      |E|



  A. When I is greater than LOAD_BREAK_EVEN, Iint increases with a slope given by the difference
     (I - LOAD_BREAK_EVEN) multiplied by LOAD_SLOPE_UP.

  B. When I is equal to LOAD_BREAK_EVEN, Iint stays at its level.

  C. When I is lower than LOAD_BREAK_EVEN, Iint decreases with a slope given by the difference
     (LOAD_BREAK_EVEN - I) multiplied by LOAD_SLOPE_DOWN.

  D. When Iint reaches the LOAD_SHUTDOWN_HIGH level a running user program is stopped
     and running motors are stopped.

  E. When I reaches the LOAD_SHUTDOWN_FAIL level a running user program is stopped
     and running motors are stopped.

 \endverbatim
 */


void      cUiCheckPower(UWORD Time)
{ // 400mS

  DATA16  X,Y;
  DATAF   I;
  DATAF   Slope;

  I  =  UiInstance.Ibatt + UiInstance.Imotor;

  if (I > LOAD_BREAK_EVEN)
  {
    Slope  =  LOAD_SLOPE_UP;
  }
  else
  {
    Slope  =  LOAD_SLOPE_DOWN;
  }

  UiInstance.Iintegrated +=  (I - LOAD_BREAK_EVEN) * (Slope * (DATAF)Time / 1000.0);

  if (UiInstance.Iintegrated < 0.0)
  {
    UiInstance.Iintegrated  =  0.0;
  }
  if (UiInstance.Iintegrated > LOAD_SHUTDOWN_FAIL)
  {
    UiInstance.Iintegrated  =  LOAD_SHUTDOWN_FAIL;
  }

  if ((UiInstance.Iintegrated >= LOAD_SHUTDOWN_HIGH) || (I >= LOAD_SHUTDOWN_FAIL))
  {
    UiInstance.Warning       |=  WARNING_CURRENT;
    UiInstance.PowerShutdown  =  1;
  }
  if (UiInstance.Iintegrated <= LOAD_BREAK_EVEN)
  {
    UiInstance.Warning       &= ~WARNING_CURRENT;
    UiInstance.PowerShutdown  =  0;
  }

  if (UiInstance.PowerShutdown)
  {
    if (UiInstance.ScreenBlocked == 0)
    {
      if (ProgramStatus(USER_SLOT) != STOPPED)
      {
        UiInstance.PowerState  =  1;
      }
    }
    ProgramEnd(USER_SLOT);
  }

  switch (UiInstance.PowerState)
  {
    case 0 :
    {
      if (UiInstance.PowerShutdown)
      {
        UiInstance.PowerState++;
      }
    }
    break;

    case 1 :
    {
      if (!UiInstance.ScreenBusy)
      {
        if ((!UiInstance.VoltShutdown))
        {
          UiInstance.ScreenBlocked  =  1;
          UiInstance.PowerState++;
        }
      }
    }
    break;

    case 2 :
    {
      LCDCopy(&UiInstance.LcdSafe,&UiInstance.LcdSave,sizeof(UiInstance.LcdSave));
      UiInstance.PowerState++;
    }
    break;

    case 3 :
    {
      X  =  16;
      Y  =  52;

      dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,X,Y,POP3_width,POP3_height,(UBYTE*)POP3_bits);

      dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,X + 48,Y + 10,LARGE_ICON,WARNSIGN);
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,X + 72,Y + 10,LARGE_ICON,WARN_POWER);
      dLcdDrawLine((*UiInstance.pLcd).Lcd,FG_COLOR,X + 5,Y + 39,X + 138,Y + 39);
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,X + 56,Y + 40,LARGE_ICON,YES_SEL);
      dLcdUpdate(UiInstance.pLcd);
      cUiButtonFlush();
      UiInstance.PowerState++;
    }
    break;

    case 4 :
    {
      if (cUiGetShortPress(ENTER_BUTTON))
      {
        if (UiInstance.ScreenBlocked)
        {
          UiInstance.ScreenBlocked  =  0;
        }
        LCDCopy(&UiInstance.LcdSave,&UiInstance.LcdSafe,sizeof(UiInstance.LcdSafe));
        dLcdUpdate(UiInstance.pLcd);
        UiInstance.PowerState++;
      }
      if ((!UiInstance.PowerShutdown))
      {
        UiInstance.PowerState  =  0;
      }
    }
    break;

    case 5 :
    {
      if ((!UiInstance.PowerShutdown))
      {
        UiInstance.PowerState  =  0;
      }
    }
    break;

  }
}
#endif


#ifndef DISABLE_VIRTUAL_BATT_TEMP

/*! \page pmtempsd High Temperature Shutdown
 *
 *  <hr size="1"/>
 *\n
 *  High temperature shutdown is based on the total current drawn from the battery and the battery voltage. An estimated
 *  temperature rise is calculated from the battery voltage, current, internal resistance and time
 *
 *
 \verbatim





 \endverbatim
 */

void      cUiCheckTemp(void)
{
  if ((UiInstance.MilliSeconds - UiInstance.TempTimer) >= CALL_INTERVAL)
  {
    UiInstance.TempTimer +=  CALL_INTERVAL;
    UiInstance.Tbatt      =  new_bat_temp(UiInstance.Vbatt,(UiInstance.Ibatt * (DATAF)1.1));
#ifdef DEBUG_TEMP_SHUTDOWN
    UiInstance.Tbatt     +=  35.0;
#endif
#ifdef DEBUG_VIRTUAL_BATT_TEMP
    char    Buffer[250];
    int     BufferSize;

    if (TempFile >= MIN_HANDLE)
    {
      BufferSize  =  snprintf(Buffer,250,"%8.1f,%9.6f,%9.6f,%11.6f\r\n",(float)UiInstance.MilliSeconds / (float)1000,UiInstance.Vbatt,UiInstance.Ibatt,UiInstance.Tbatt);
      write(TempFile,Buffer,BufferSize);
    }
#endif
  }

  if (UiInstance.Tbatt >= TEMP_SHUTDOWN_WARNING)
  {
    UiInstance.Warning   |=  WARNING_TEMP;
  }
  else
  {
    UiInstance.Warning   &= ~WARNING_TEMP;
  }


  if (UiInstance.Tbatt >= TEMP_SHUTDOWN_FAIL)
  {
    ProgramEnd(USER_SLOT);
    UiInstance.ShutDown         =  1;
  }
}
#endif

#ifndef   DISABLE_LOW_MEMORY

void      cUiCheckMemory(void)
{ // 400mS

  DATA32  Total;
  DATA32  Free;

  cMemoryGetUsage(&Total,&Free,0);

  if (Free > LOW_MEMORY)
  { // Good

    UiInstance.Warning   &= ~WARNING_MEMORY;
  }
  else
  { // Bad

    UiInstance.Warning   |=  WARNING_MEMORY;
  }

}

#endif


void      cUiCheckAlive(UWORD Time)
{
  ULONG   Timeout;

  UiInstance.SleepTimer +=  Time;

  if (UiInstance.Activated & BUTTON_ALIVE)
  {
    UiInstance.Activated &= ~BUTTON_ALIVE;
    cUiAlive();
  }
  Timeout  =  (ULONG)GetSleepMinutes();

  if (Timeout)
  {
    if (UiInstance.SleepTimer >= (Timeout * 60000L))
    {
      UiInstance.ShutDown         =  1;
    }
  }
}


void      cUiUpdate(UWORD Time)
{
  DATA8   Warning;
  DATA8   Tmp;

  UiInstance.MilliSeconds += (ULONG)Time;

  cUiUpdateButtons(Time);
  cUiUpdateInput();
  cUiUpdateCnt();

#ifdef MAX_FRAMES_PER_SEC
  UiInstance.DisplayTimer += (ULONG)Time;
  if (UiInstance.DisplayTimer >= (1000 / MAX_FRAMES_PER_SEC))
  {
    UiInstance.AllowUpdate    =  1;
    if (UiInstance.ScreenBusy == 0)
    {
      dLcdAutoUpdate();
    }
  }
#endif


  if ((UiInstance.MilliSeconds - UiInstance.UpdateStateTimer) >= 50)
  {
    UiInstance.UpdateStateTimer  =  UiInstance.MilliSeconds;

    if (!UiInstance.Event)
    {
      UiInstance.Event  =  cComGetEvent();
    }

    switch (UiInstance.UpdateState++)
    {
      case 0 :
      { //  50 mS

#ifdef ENABLE_HIGH_CURRENT
        if (UiInstance.ReadyForWarnings)
        {
          cUiCheckPower(400);
        }
#endif
#ifndef DISABLE_VIRTUAL_BATT_TEMP
        if (UiInstance.ReadyForWarnings)
        {
          if (!UiInstance.Accu)
          {
            cUiCheckTemp();
          }
        }
#endif
      }
      break;

      case 1 :
      { // 100 mS

        cUiRunScreen();
      }
      break;

      case 2 :
      { // 150 mS

        cUiCheckAlive(400);
#ifndef DISABLE_LOW_MEMORY
        if (UiInstance.ReadyForWarnings)
        {
          cUiCheckMemory();
        }
#endif
      }
      break;

      case 3 :
      { // 200 mS

        cUiRunScreen();
      }
      break;

      case 4 :
      { // 250 mS

#ifndef DISABLE_LOW_VOLTAGE
        if (UiInstance.ReadyForWarnings)
        {
          cUiCheckVoltage();
        }
#endif
      }
      break;

      case 5 :
      { // 300 mS

        cUiRunScreen();
      }
      break;

      case 6 :
      { // 350 mS

        if (UiInstance.ScreenBusy == 0)
        {
          cUiUpdateTopline();
          dLcdUpdate(UiInstance.pLcd);
        }
#ifdef BUFPRINTSIZE
        if ((*UiInstance.pUi).Activated & BUTTON_BUFPRINT)
        {
          (*UiInstance.pUi).Activated &= ~BUTTON_BUFPRINT;
          BufPrint('w',"");
        }
#endif
      }
      break;

      default :
      { // 400 mS

        cUiRunScreen();
        UiInstance.UpdateState       =  0;
        UiInstance.ReadyForWarnings  = 1;
      }
      break;

    }

    if (UiInstance.Warning)
    { // Some warning present

      if (!UiInstance.Warnlight)
      { // If not on - turn orange light on

        UiInstance.Warnlight  =  1;
        cUiSetLed(UiInstance.LedState);
      }
    }
    else
    { // No warning

      if (UiInstance.Warnlight)
      { // If orange light on - turn it off

        UiInstance.Warnlight  =  0;
        cUiSetLed(UiInstance.LedState);
      }
    }

    // Get valid popup warnings
    Warning   =  UiInstance.Warning & WARNINGS;

    // Find warnings that has not been showed
    Tmp       =  Warning & ~UiInstance.WarningShowed;

    if (Tmp)
    { // Show popup

      if (!UiInstance.ScreenBusy)
      { // Wait on screen

        if (UiInstance.ScreenBlocked  ==  0)
        {
          LCDCopy(&UiInstance.LcdSafe,&UiInstance.LcdSave,sizeof(UiInstance.LcdSave));
          dLcdDrawPicture((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_X,vmPOP3_ABS_Y,POP3_width,POP3_height,(UBYTE*)POP3_bits);

          if (Tmp & WARNING_TEMP)
          {
            dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X1,vmPOP3_ABS_WARN_ICON_Y,LARGE_ICON,WARNSIGN);
            dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X2,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,WARN_POWER);
            dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X3,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,TO_MANUAL);
            UiInstance.WarningShowed |=  WARNING_TEMP;
          }
          else
          {
            if (Tmp & WARNING_CURRENT)
            {
              dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X1,vmPOP3_ABS_WARN_ICON_Y,LARGE_ICON,WARNSIGN);
              dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X2,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,WARN_POWER);
              dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X3,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,TO_MANUAL);
              UiInstance.WarningShowed |=  WARNING_CURRENT;
            }
            else
            {
              if (Tmp & WARNING_VOLTAGE)
              {
                dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X,vmPOP3_ABS_WARN_ICON_Y,LARGE_ICON,WARNSIGN);
                dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_SPEC_ICON_X,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,WARN_BATT);
                UiInstance.WarningShowed |=  WARNING_VOLTAGE;
              }
              else
              {
                if (Tmp & WARNING_MEMORY)
                {
                  dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X,vmPOP3_ABS_WARN_ICON_Y,LARGE_ICON,WARNSIGN);
                  dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_SPEC_ICON_X,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,WARN_MEMORY);
                  UiInstance.WarningShowed |=  WARNING_MEMORY;
                }
                else
                {
                  if (Tmp & WARNING_DSPSTAT)
                  {
                    dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_ICON_X,vmPOP3_ABS_WARN_ICON_Y,LARGE_ICON,WARNSIGN);
                    dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_SPEC_ICON_X,vmPOP3_ABS_WARN_SPEC_ICON_Y,LARGE_ICON,PROGRAM_ERROR);
                    UiInstance.WarningShowed |=  WARNING_DSPSTAT;
                  }
                  else
                  {
                  }
                }
              }
            }
          }
          dLcdDrawLine((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_LINE_X,vmPOP3_ABS_WARN_LINE_Y,vmPOP3_ABS_WARN_LINE_ENDX,vmPOP3_ABS_WARN_LINE_Y);
          dLcdDrawIcon((*UiInstance.pLcd).Lcd,FG_COLOR,vmPOP3_ABS_WARN_YES_X,vmPOP3_ABS_WARN_YES_Y,LARGE_ICON,YES_SEL);
          dLcdUpdate(UiInstance.pLcd);
          cUiButtonFlush();
          UiInstance.ScreenBlocked  =  1;
        }
      }
    }

    // Find warnings that have been showed but not confirmed
    Tmp       =  UiInstance.WarningShowed;
    Tmp      &= ~UiInstance.WarningConfirmed;

    if (Tmp)
    {
      if (cUiGetShortPress(ENTER_BUTTON))
      {
        UiInstance.ScreenBlocked  =  0;
        LCDCopy(&UiInstance.LcdSave,&UiInstance.LcdSafe,sizeof(UiInstance.LcdSafe));
        dLcdUpdate(UiInstance.pLcd);
        if (Tmp & WARNING_TEMP)
        {
          UiInstance.WarningConfirmed |=  WARNING_TEMP;
        }
        else
        {
          if (Tmp & WARNING_CURRENT)
          {
            UiInstance.WarningConfirmed |=  WARNING_CURRENT;
          }
          else
          {
            if (Tmp & WARNING_VOLTAGE)
            {
              UiInstance.WarningConfirmed |=  WARNING_VOLTAGE;
            }
            else
            {
              if (Tmp & WARNING_MEMORY)
              {
                UiInstance.WarningConfirmed |=  WARNING_MEMORY;
              }
              else
              {
                if (Tmp & WARNING_DSPSTAT)
                {
                  UiInstance.WarningConfirmed |=  WARNING_DSPSTAT;
                  UiInstance.Warning          &= ~WARNING_DSPSTAT;
                }
                else
                {
                }
              }
            }
          }
        }
      }
    }

    // Find warnings that have been showed, confirmed and removed
    Tmp       = ~Warning;
    Tmp      &=  WARNINGS;
    Tmp      &=  UiInstance.WarningShowed;
    Tmp      &=  UiInstance.WarningConfirmed;

    UiInstance.WarningShowed     &= ~Tmp;
    UiInstance.WarningConfirmed  &= ~Tmp;
  }
}


const     DATA8 FiletypeToNormalIcon[FILETYPES] =
{
  [FILETYPE_UNKNOWN]  =  ICON_FOLDER,
  [TYPE_FOLDER]       =  ICON_FOLDER,
  [TYPE_SOUND]        =  ICON_SOUND,
  [TYPE_BYTECODE]     =  ICON_RUN,
  [TYPE_GRAPHICS]     =  ICON_IMAGE,
  [TYPE_DATALOG]      =  ICON_OBD,
  [TYPE_PROGRAM]      =  ICON_OBP,
  [TYPE_TEXT]         =  ICON_TEXT
};

DATA8     cUiNotification(DATA8 Color,DATA16 X,DATA16 Y,DATA8 Icon1,DATA8 Icon2,DATA8 Icon3,DATA8 *pText,DATA8 *pState)
{
  RESULT  Result = BUSY;
  NOTIFY  *pQ;
  DATA16  AvailableX;
  DATA16  UsedX;
  DATA16  Line;
  DATA16  CharIn;
  DATA16  CharOut;
  DATA8   Character;
  DATA16  X2;
  DATA16  Y2;
  DATA16  AvailableY;
  DATA16  UsedY;

  pQ      =  &UiInstance.Notify;

  if (*pState == 0)
  {
    *pState             =  1;
    (*pQ).ScreenStartX  =  X;
    (*pQ).ScreenStartY  =  Y;
    (*pQ).ScreenWidth   =  POP3_width;
    (*pQ).ScreenHeight  =  POP3_height;
    (*pQ).IconStartY    =  (*pQ).ScreenStartY + 10;
    (*pQ).IconWidth     =  dLcdGetIconWidth(LARGE_ICON);
    (*pQ).IconHeight    =  dLcdGetIconHeight(LARGE_ICON);
    (*pQ).IconSpaceX    =  (*pQ).IconWidth;

    (*pQ).YesNoStartX   =  (*pQ).ScreenStartX + ((*pQ).ScreenWidth / 2);
    (*pQ).YesNoStartX  -=  ((*pQ).IconWidth + 8) / 2;
    (*pQ).YesNoStartX   =  cUiAlignX((*pQ).YesNoStartX);
    (*pQ).YesNoStartY   =  (*pQ).ScreenStartY + 40;

    (*pQ).LineStartX    =  (*pQ).ScreenStartX + 5;
    (*pQ).LineStartY    =  (*pQ).ScreenStartY + 39;
    (*pQ).LineEndX      =  (*pQ).LineStartX + 134;

    // Find no of icons
    (*pQ).NoOfIcons     =  0;
    if (Icon1 > ICON_NONE)
    {
      (*pQ).NoOfIcons++;
    }
    if (Icon2 > ICON_NONE)
    {
      (*pQ).NoOfIcons++;
    }
    if (Icon3 > ICON_NONE)
    {
      (*pQ).NoOfIcons++;
    }

    // Find no of text lines
    (*pQ).TextLines     =  0;
    if (pText[0])
    {
      (*pQ).IconStartX    =  (*pQ).ScreenStartX + 8;
      (*pQ).IconStartX    =  cUiAlignX((*pQ).IconStartX);

      AvailableX          =  (*pQ).ScreenWidth;
      AvailableX         -=  (((*pQ).IconStartX - (*pQ).ScreenStartX)) * 2;

      AvailableX         -=  (*pQ).NoOfIcons * (*pQ).IconSpaceX;


      (*pQ).NoOfChars     =  strlen((char*)pText);


      (*pQ).Font          =  SMALL_FONT;
      (*pQ).FontWidth     =  dLcdGetFontWidth((*pQ).Font);
      UsedX               =  (*pQ).FontWidth * (*pQ).NoOfChars;

      Line                =  0;

      if (UsedX <= AvailableX)
      { // One line - small font

        if ((AvailableX - UsedX) >= 32)
        {
          (*pQ).IconStartX += 32;
        }

        snprintf((char*)(*pQ).TextLine[Line],MAX_NOTIFY_LINE_CHARS,"%s",pText);
        Line++;
        (*pQ).TextLines++;

        (*pQ).TextStartX  =  (*pQ).IconStartX + ((*pQ).NoOfIcons * (*pQ).IconSpaceX) ;
        (*pQ).TextStartY  =  (*pQ).ScreenStartY + 18;
        (*pQ).TextSpaceY  =  dLcdGetFontHeight((*pQ).Font) + 1;
      }
      else
      { // one or more lines - tiny font

        (*pQ).Font        =  TINY_FONT;
        (*pQ).FontWidth   =  dLcdGetFontWidth((*pQ).Font);
        UsedX             =  (*pQ).FontWidth * (*pQ).NoOfChars;
        AvailableX       -=  (*pQ).FontWidth;

        CharIn            =  0;

        while ((pText[CharIn]) && (Line < MAX_NOTIFY_LINES))
        {
          CharOut         =  0;
          UsedX           =  0;
          while ((pText[CharIn]) && (CharOut < MAX_NOTIFY_LINE_CHARS) && (UsedX < (AvailableX - (*pQ).FontWidth)))
          {
            Character     =  pText[CharIn];
            if (Character == '_')
            {
              Character   =  ' ';
            }
            (*pQ).TextLine[Line][CharOut]  =  Character;
            CharIn++;
            CharOut++;
            UsedX        +=  (*pQ).FontWidth;
          }
          while ((CharOut > 8) && (pText[CharIn] != ' ') && (pText[CharIn] != '_') && (pText[CharIn] != 0))
          {
            CharIn--;
            CharOut--;
          }
          if (pText[CharIn] != 0)
          {
            CharIn++;
          }
          (*pQ).TextLine[Line][CharOut]  =  0;
          Line++;
        }

        (*pQ).TextLines   =  Line;

        (*pQ).TextStartX  =  (*pQ).IconStartX + ((*pQ).NoOfIcons * (*pQ).IconSpaceX) + (*pQ).FontWidth ;
        (*pQ).TextSpaceY  =  dLcdGetFontHeight((*pQ).Font) + 1;


        AvailableY        =  (*pQ).LineStartY - ((*pQ).ScreenStartY + 5);
        UsedY             =  (*pQ).TextLines * (*pQ).TextSpaceY;

        while (UsedY > AvailableY)
        {
          (*pQ).TextLines--;
          UsedY           =  (*pQ).TextLines * (*pQ).TextSpaceY;
        }
        Y2                =  (AvailableY - UsedY) / 2;

        (*pQ).TextStartY  =  (*pQ).ScreenStartY + Y2 + 5;
      }

    }
    else
    {
      (*pQ).IconStartX  =  (*pQ).ScreenStartX + ((*pQ).ScreenWidth / 2);
      (*pQ).IconStartX -=  ((*pQ).IconWidth + 8) / 2;
      (*pQ).IconStartX -=  ((*pQ).NoOfIcons / 2) * (*pQ).IconWidth;
      (*pQ).IconStartX  =  cUiAlignX((*pQ).IconStartX);
      (*pQ).TextStartY    =  (*pQ).ScreenStartY + 8;
    }

    (*pQ).NeedUpdate    =  1;
  }


  if ((*pQ).NeedUpdate)
  {
//* UPDATE ***************************************************************************************************
    (*pQ).NeedUpdate    =  0;

    dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pQ).ScreenStartX,(*pQ).ScreenStartY,POP3_width,POP3_height,(UBYTE*)POP3_bits);

    X2                  =  (*pQ).IconStartX;

    if (Icon1 > ICON_NONE)
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,X2,(*pQ).IconStartY,LARGE_ICON,Icon1);
      X2 +=  (*pQ).IconSpaceX;
    }
    if (Icon2 > ICON_NONE)
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,X2,(*pQ).IconStartY,LARGE_ICON,Icon2);
      X2 +=  (*pQ).IconSpaceX;
    }
    if (Icon3 > ICON_NONE)
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,X2,(*pQ).IconStartY,LARGE_ICON,Icon3);
      X2 +=  (*pQ).IconSpaceX;
    }

    Line  =  0;
    Y2    =  (*pQ).TextStartY;
    while (Line < (*pQ).TextLines)
    {
      dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pQ).TextStartX,Y2,(*pQ).Font,(*pQ).TextLine[Line]);
      Y2 +=  (*pQ).TextSpaceY;
      Line++;
    }

    dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,(*pQ).LineStartX,(*pQ).LineStartY,(*pQ).LineEndX,(*pQ).LineStartY);

    dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).YesNoStartX,(*pQ).YesNoStartY,LARGE_ICON,YES_SEL);

    cUiUpdateLcd();
    UiInstance.ScreenBusy       =  0;
  }

  if (cUiGetShortPress(ENTER_BUTTON))
  {
    cUiButtonFlush();
    Result  =  OK;
    *pState =  0;
  }

  return (Result);
}


DATA8     cUiQuestion(DATA8 Color, DATA16 X,DATA16 Y,DATA8 Icon1,DATA8 Icon2,DATA8 *pText,DATA8 *pState,DATA8 *pAnswer)
{
  RESULT  Result = BUSY;
  TQUESTION *pQ;
  DATA16  Inc;

  pQ      =  &UiInstance.Question;

  Inc  =  cUiGetHorz();
  if (Inc != 0)
  {
    (*pQ).NeedUpdate    =  1;

    *pAnswer           +=  Inc;

    if (*pAnswer > 1)
    {
      *pAnswer          =  1;
      (*pQ).NeedUpdate  =  0;
    }
    if (*pAnswer < 0)
    {
      *pAnswer          =  0;
      (*pQ).NeedUpdate  =  0;
    }
  }

  if (*pState == 0)
  {
    *pState             =  1;
    (*pQ).ScreenStartX  =  X;
    (*pQ).ScreenStartY  =  Y;
    (*pQ).IconWidth     =  dLcdGetIconWidth(LARGE_ICON);
    (*pQ).IconHeight    =  dLcdGetIconHeight(LARGE_ICON);

    (*pQ).NoOfIcons     =  0;
    if (Icon1 > ICON_NONE)
    {
      (*pQ).NoOfIcons++;
    }
    if (Icon2 > ICON_NONE)
    {
      (*pQ).NoOfIcons++;
    }
    (*pQ).Default       =  *pAnswer;

    (*pQ).NeedUpdate    =  1;
  }


  if ((*pQ).NeedUpdate)
  {
//* UPDATE ***************************************************************************************************
    (*pQ).NeedUpdate    =  0;

    dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pQ).ScreenStartX,(*pQ).ScreenStartY,POP3_width,POP3_height,(UBYTE*)POP3_bits);
    (*pQ).ScreenWidth   =  POP3_width;
    (*pQ).ScreenHeight  =  POP3_height;

    (*pQ).IconStartX    =  (*pQ).ScreenStartX + ((*pQ).ScreenWidth / 2);
    if ((*pQ).NoOfIcons > 1)
    {
      (*pQ).IconStartX -=  (*pQ).IconWidth;
    }
    else
    {
      (*pQ).IconStartX -=  (*pQ).IconWidth / 2;
    }
    (*pQ).IconStartX    =  cUiAlignX((*pQ).IconStartX);
    (*pQ).IconSpaceX    =  (*pQ).IconWidth;
    (*pQ).IconStartY    =  (*pQ).ScreenStartY + 10;

    (*pQ).YesNoStartX   =  (*pQ).ScreenStartX + ((*pQ).ScreenWidth / 2);
    (*pQ).YesNoStartX  -=  8;
    (*pQ).YesNoStartX  -=  (*pQ).IconWidth;
    (*pQ).YesNoStartX   =  cUiAlignX((*pQ).YesNoStartX);
    (*pQ).YesNoSpaceX   =  (*pQ).IconWidth + 16;
    (*pQ).YesNoStartY   =  (*pQ).ScreenStartY + 40;

    (*pQ).LineStartX    =  (*pQ).ScreenStartX + 5;
    (*pQ).LineStartY    =  (*pQ).ScreenStartY + 39;
    (*pQ).LineEndX      =  (*pQ).LineStartX + 134;

    switch ((*pQ).NoOfIcons)
    {
      case 1 :
      {
        dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).IconStartX,(*pQ).IconStartY,LARGE_ICON,Icon1);
      }
      break;

      case 2 :
      {
        dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).IconStartX,(*pQ).IconStartY,LARGE_ICON,Icon1);
        dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).IconStartX + (*pQ).IconSpaceX,(*pQ).IconStartY,LARGE_ICON,Icon2);
      }
      break;

    }

    if (*pAnswer == 0)
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).YesNoStartX,(*pQ).YesNoStartY,LARGE_ICON,NO_SEL);
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).YesNoStartX + (*pQ).YesNoSpaceX,(*pQ).YesNoStartY,LARGE_ICON,YES_NOTSEL);
    }
    else
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).YesNoStartX,(*pQ).YesNoStartY,LARGE_ICON,NO_NOTSEL);
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).YesNoStartX + (*pQ).YesNoSpaceX,(*pQ).YesNoStartY,LARGE_ICON,YES_SEL);
    }

    dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,(*pQ).LineStartX,(*pQ).LineStartY,(*pQ).LineEndX,(*pQ).LineStartY);

    cUiUpdateLcd();
    UiInstance.ScreenBusy       =  0;
  }
  if (cUiGetShortPress(ENTER_BUTTON))
  {
    cUiButtonFlush();
    Result    =  OK;
    *pState   =  0;
  }
  if (cUiGetShortPress(BACK_BUTTON))
  {
    cUiButtonFlush();
    Result    =  OK;
    *pState   =  0;
    *pAnswer  =  -1;
  }

  return (Result);
}


RESULT    cUiIconQuestion(DATA8 Color,DATA16 X,DATA16 Y,DATA8 *pState,DATA32 *pIcons)
{
  RESULT  Result = BUSY;
  IQUESTION *pQ;
  DATA32  Mask;
  DATA32  TmpIcons;
  DATA16  Tmp;
  DATA16  Loop;
  DATA8   Icon;

  pQ      =  &UiInstance.IconQuestion;

  if (*pState == 0)
  {
    *pState             =  1;
    (*pQ).ScreenStartX  =  X;
    (*pQ).ScreenStartY  =  Y;
    (*pQ).ScreenWidth   =  POP2_width;
    (*pQ).ScreenHeight  =  POP2_height;
    (*pQ).IconWidth     =  dLcdGetIconWidth(LARGE_ICON);
    (*pQ).IconHeight    =  dLcdGetIconHeight(LARGE_ICON);
    (*pQ).Frame         =  2;
    (*pQ).Icons         =  *pIcons;
    (*pQ).NoOfIcons     =  0;
    (*pQ).PointerX      =  0;

    TmpIcons   =  (*pQ).Icons;
    while (TmpIcons)
    {
      if (TmpIcons & 1)
      {
        (*pQ).NoOfIcons++;
      }
      TmpIcons >>= 1;
    }

    if ((*pQ).NoOfIcons)
    {
      (*pQ).IconStartY    =  (*pQ).ScreenStartY + (((*pQ).ScreenHeight - (*pQ).IconHeight) / 2);

      (*pQ).IconSpaceX    =  (((*pQ).ScreenWidth - ((*pQ).IconWidth * (*pQ).NoOfIcons)) / (*pQ).NoOfIcons) + (*pQ).IconWidth;
      (*pQ).IconSpaceX    =  (*pQ).IconSpaceX & ~7;

      Tmp                 =  (*pQ).IconSpaceX * (*pQ).NoOfIcons - ((*pQ).IconSpaceX - (*pQ).IconWidth);

      (*pQ).IconStartX    =  (*pQ).ScreenStartX + (((*pQ).ScreenWidth - Tmp) / 2);
      (*pQ).IconStartX    =  ((*pQ).IconStartX + 7) & ~7;

      (*pQ).SelectStartX  =  (*pQ).IconStartX - 1;
      (*pQ).SelectStartY  =  (*pQ).IconStartY - 1;
      (*pQ).SelectWidth   =  (*pQ).IconWidth + 2;
      (*pQ).SelectHeight  =  (*pQ).IconHeight + 2;
      (*pQ).SelectSpaceX  =  (*pQ).IconSpaceX;
    }
#ifdef DEBUG
    printf("Shown icons %d -> 0x%08X\r\n",(*pQ).NoOfIcons,(*pQ).Icons);
#endif

    (*pQ).NeedUpdate    =  1;
  }

  if ((*pQ).NoOfIcons)
  {
    // Check for move pointer
    Tmp  =  cUiGetHorz();
    if (Tmp)
    {
      (*pQ).PointerX +=  Tmp;

      (*pQ).NeedUpdate    =  1;

      if ((*pQ).PointerX < 0)
      {
        (*pQ).PointerX    =  0;
        (*pQ).NeedUpdate  =  0;
      }
      if ((*pQ).PointerX >= (*pQ).NoOfIcons)
      {
        (*pQ).PointerX    =  (*pQ).NoOfIcons - 1;
        (*pQ).NeedUpdate  =  0;
      }
    }
  }

  if ((*pQ).NeedUpdate)
  {
//* UPDATE ***************************************************************************************************
    (*pQ).NeedUpdate    =  0;

    dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pQ).ScreenStartX,(*pQ).ScreenStartY,POP2_width,POP2_height,(UBYTE*)POP2_bits);
    (*pQ).ScreenWidth   =  POP2_width;
    (*pQ).ScreenHeight  =  POP2_height;

    // Show icons
    Loop  =  0;
    Icon  =  0;
    TmpIcons   =  (*pQ).Icons;
    while (Loop < (*pQ).NoOfIcons)
    {
      while (!(TmpIcons & 1))
      {
        Icon++;
        TmpIcons >>=  1;
      }
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pQ).IconStartX + (*pQ).IconSpaceX * Loop,(*pQ).IconStartY,LARGE_ICON,Icon);
      Loop++;
      Icon++;
      TmpIcons >>=  1;
    }

    // Show selection
    dLcdInverseRect((*UiInstance.pLcd).Lcd,(*pQ).SelectStartX + (*pQ).SelectSpaceX * (*pQ).PointerX,(*pQ).SelectStartY,(*pQ).SelectWidth,(*pQ).SelectHeight);

    // Update screen
    cUiUpdateLcd();
    UiInstance.ScreenBusy       =  0;
  }
  if (cUiGetShortPress(ENTER_BUTTON))
  {
    if ((*pQ).NoOfIcons)
    {
      Mask  =  0x00000001;
      TmpIcons   =  (*pQ).Icons;
      Loop  =  (*pQ).PointerX + 1;

      do
      {
        if (TmpIcons & Mask)
        {
          Loop--;
        }
        Mask <<=  1;
      }
      while (Loop && Mask);
      Mask >>=  1;
      *pIcons  =  Mask;
    }
    else
    {
      *pIcons  =  0;
    }
    cUiButtonFlush();
    Result  =  OK;
    *pState =  0;
#ifdef DEBUG
    printf("Selecting icon %d -> 0x%08X\r\n",(*pQ).PointerX,*pIcons);
#endif
  }
  if (cUiGetShortPress(BACK_BUTTON))
  {
    *pIcons  =  0;
    cUiButtonFlush();
    Result  =  OK;
    *pState =  0;
  }

  return (Result);
}


#include  "keyboardc.xbm"
#include  "keyboards.xbm"
#include  "keyboardn.xbm"

#define   MAX_KEYB_DEEPT      3
#define   MAX_KEYB_WIDTH      12
#define   MAX_KEYB_HEIGHT     4

DATA8     cUiKeyboard(DATA8 Color,DATA16 X,DATA16 Y,DATA8 Icon,DATA8 Lng,DATA8 *pText,DATA8 *pCharSet,DATA8 *pAnswer)
{
  KEYB    *pK;
  DATA16  Width;
  DATA16  Height;
  DATA16  Inc;
  DATA16  SX;
  DATA16  SY;
  DATA16  X3;
  DATA16  X4;
  DATA16  Tmp;
  DATA8   TmpChar;
  DATA8   SelectedChar = 0;

  //            Value    Marking
  //  table  >  0x20  -> normal rect
  //  table  =  0x20  -> spacebar
  //  table  =  0     -> end
  //  table  =  0x01  -> num
  //  table  =  0x02  -> cap
  //  table  =  0x03  -> non cap
  //  table  =  0x04  -> big cap
  //  table  =  0x08  -> backspace
  //  table  =  0x0D  -> enter
  //


  DATA8     KeyboardLayout[MAX_KEYB_DEEPT][MAX_KEYB_HEIGHT][MAX_KEYB_WIDTH] =
  {
    {
      {  'Q','W','E','R','T','Y','U','I','O','P',  0x08,0x00 },
      { 0x03,'A','S','D','F','G','H','J','K','L',  0x0D,0x00 },
      { 0x01,'Z','X','C','V','B','N','M',0x0D,0x0D,0x0D,0x00 },
      {  ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',  0x0D,0x00 }
    },
    {
      {  'q','w','e','r','t','y','u','i','o','p',  0x08,0x00 },
      { 0x02,'a','s','d','f','g','h','j','k','l',  0x0D,0x00 },
      { 0x01,'z','x','c','v','b','n','m',0x0D,0x0D,0x0D,0x00 },
      {  ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',  0x0D,0x00 }
    },
    {
      {  '1','2','3','4','5','6','7','8','9','0',  0x08,0x00 },
      { 0x04,'+','-','=','<','>','/','\\','*',':', 0x0D,0x00 },
      { 0x04,'(',')','_','.','@','!','?',0x0D,0x0D,0x0D,0x00 },
      {  ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',  0x0D,0x00 }
    }
  };

  pK      =  &UiInstance.Keyboard;

  if (*pCharSet != 0)
  {
    (*pK).CharSet       =  *pCharSet;
    *pCharSet           =  0;
    (*pK).ScreenStartX  =  X;
    (*pK).ScreenStartY  =  Y;

    if ((Icon >= 0) && (Icon < N_ICON_NOS))
    {
      (*pK).IconStartX  =  cUiAlignX((*pK).ScreenStartX + 7);
      (*pK).IconStartY  =  (*pK).ScreenStartY + 4;
      (*pK).TextStartX  =  (*pK).IconStartX + dLcdGetIconWidth(NORMAL_ICON);
    }
    else
    {
      (*pK).TextStartX  =  cUiAlignX((*pK).ScreenStartX + 9);
    }
    (*pK).TextStartY    =  (*pK).ScreenStartY + 7;
    (*pK).StringStartX  =  (*pK).ScreenStartX + 8;
    (*pK).StringStartY  =  (*pK).ScreenStartY + 22;
    (*pK).KeybStartX    =  (*pK).ScreenStartX + 13;
    (*pK).KeybStartY    =  (*pK).ScreenStartY + 40;
    (*pK).KeybSpaceX    =  11;
    (*pK).KeybSpaceY    =  14;
    (*pK).KeybHeight    =  13;
    (*pK).KeybWidth     =  9;
    (*pK).Layout        =  0;
    (*pK).PointerX      =  10;
    (*pK).PointerY      =  1;
    (*pK).NeedUpdate    =  1;
  }

  Width             =  strlen((char*)KeyboardLayout[(*pK).Layout][(*pK).PointerY]) - 1;
  Height            =  MAX_KEYB_HEIGHT - 1;

  Inc               =  cUiGetHorz();
  (*pK).PointerX   +=  Inc;
  if ((*pK).PointerX < 0)
  {
    (*pK).PointerX  =  0;
  }
  if ((*pK).PointerX > Width)
  {
    (*pK).PointerX  =  Width;
  }
  Inc               =  cUiGetVert();
  (*pK).PointerY   +=  Inc;
  if ((*pK).PointerY < 0)
  {
    (*pK).PointerY  =  0;
  }
  if ((*pK).PointerY > Height)
  {
    (*pK).PointerY  =  Height;
  }


  TmpChar  =  KeyboardLayout[(*pK).Layout][(*pK).PointerY][(*pK).PointerX];

  if (cUiGetShortPress(BACK_BUTTON))
  {
    SelectedChar  =  0x0D;
    pAnswer[0]    =  0;
  }
  if (cUiGetShortPress(ENTER_BUTTON))
  {
    SelectedChar  =  TmpChar;

    switch (SelectedChar)
    {
      case 0x01 :
      {
        (*pK).Layout  =  2;
      }
      break;

      case 0x02 :
      {
        (*pK).Layout  =  0;
      }
      break;

      case 0x03 :
      {
        (*pK).Layout  =  1;
      }
      break;

      case 0x04 :
      {
        (*pK).Layout  =  0;
      }
      break;

      case 0x08 :
      {
        Tmp  =  (DATA16)strlen((char*)pAnswer);
        if (Tmp)
        {
          Tmp--;
          pAnswer[Tmp]  =  0;
        }
      }
      break;

      case '\r' :
      {
      }
      break;

      default :
      {
        if (ValidateChar(&SelectedChar,(*pK).CharSet) == OK)
        {
          Tmp           =  (DATA16)strlen((char*)pAnswer);
          pAnswer[Tmp]  =  SelectedChar;
          if (++Tmp >= Lng)
          {
            Tmp--;
          }
          pAnswer[Tmp]  =  0;
        }
      }
      break;


    }

    TmpChar  =  KeyboardLayout[(*pK).Layout][(*pK).PointerY][(*pK).PointerX];

    (*pK).NeedUpdate    =  1;
  }

  if (((*pK).OldX != (*pK).PointerX) || ((*pK).OldY != (*pK).PointerY))
  {
    (*pK).OldX  =  (*pK).PointerX;
    (*pK).OldY  =  (*pK).PointerY;
    (*pK).NeedUpdate    =  1;
  }

  if ((*pK).NeedUpdate)
  {
//* UPDATE ***************************************************************************************************
    (*pK).NeedUpdate    =  0;

    switch ((*pK).Layout)
    {
      case 0 :
      {
        dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pK).ScreenStartX,(*pK).ScreenStartY,keyboardc_width,keyboardc_height,(UBYTE*)keyboardc_bits);
      }
      break;

      case 1 :
      {
        dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pK).ScreenStartX,(*pK).ScreenStartY,keyboards_width,keyboards_height,(UBYTE*)keyboards_bits);
      }
      break;

      case 2 :
      {
        dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pK).ScreenStartX,(*pK).ScreenStartY,keyboardn_width,keyboardn_height,(UBYTE*)keyboardn_bits);
      }
      break;

    }
    if ((Icon >= 0) && (Icon < N_ICON_NOS))
    {
      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pK).IconStartX,(*pK).IconStartY,NORMAL_ICON,Icon);
    }
    if (pText[0])
    {
      dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pK).TextStartX,(*pK).TextStartY,SMALL_FONT,pText);
    }


    X4  =  0;
    X3  =  strlen((char*)pAnswer);
    if (X3 > 15)
    {
      X4  =  X3 - 15;
    }

    dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pK).StringStartX,(*pK).StringStartY,NORMAL_FONT,&pAnswer[X4]);
    dLcdDrawChar((*UiInstance.pLcd).Lcd,Color,(*pK).StringStartX + (X3 - X4) * 8,(*pK).StringStartY,NORMAL_FONT,'_');



    SX  =  (*pK).KeybStartX + (*pK).PointerX * (*pK).KeybSpaceX;
    SY  =  (*pK).KeybStartY + (*pK).PointerY * (*pK).KeybSpaceY;

    switch (TmpChar)
    {
      case 0x01 :
      case 0x02 :
      case 0x03 :
      {
        dLcdInverseRect((*UiInstance.pLcd).Lcd,SX - 8,SY,(*pK).KeybWidth + 8,(*pK).KeybHeight);
      }
      break;

      case 0x04 :
      {
        dLcdInverseRect((*UiInstance.pLcd).Lcd,SX - 8,(*pK).KeybStartY + 1 * (*pK).KeybSpaceY,(*pK).KeybWidth + 8,(*pK).KeybHeight * 2 + 1);
      }
      break;

      case 0x08 :
      {
        dLcdInverseRect((*UiInstance.pLcd).Lcd,SX + 2,SY,(*pK).KeybWidth + 5,(*pK).KeybHeight);
      }
      break;

      case 0x0D :
      {
        SX  =  (*pK).KeybStartX + 112;
        SY  =  (*pK).KeybStartY + 1 * (*pK).KeybSpaceY;
        dLcdInverseRect((*UiInstance.pLcd).Lcd,SX,SY,(*pK).KeybWidth + 5,(*pK).KeybSpaceY + 1);
        SX  =  (*pK).KeybStartX + 103;
        SY  =  (*pK).KeybStartY + 1 + 2 * (*pK).KeybSpaceY;
        dLcdInverseRect((*UiInstance.pLcd).Lcd,SX,SY,(*pK).KeybWidth + 14,(*pK).KeybSpaceY * 2 - 4);
      }
      break;

      case 0x20 :
      {
        dLcdInverseRect((*UiInstance.pLcd).Lcd,(*pK).KeybStartX + 11,SY + 1,(*pK).KeybWidth + 68,(*pK).KeybHeight - 3);
      }
      break;

      default :
      {
        dLcdInverseRect((*UiInstance.pLcd).Lcd,SX + 1,SY,(*pK).KeybWidth,(*pK).KeybHeight);
      }
      break;

    }
    cUiUpdateLcd();
    UiInstance.ScreenBusy       =  0;
  }

  return (SelectedChar);
}


void      cUiDrawBar(DATA8 Color,DATA16 X,DATA16 Y,DATA16 X1,DATA16 Y1,DATA16 Min,DATA16 Max,DATA16 Act)
{
  DATA16  Tmp;
  DATA16  Items;
  DATA16  KnobHeight = 7;

  Items       =  Max - Min;

  switch (X1)
  {
    case 5 :
    {
      KnobHeight    =  7;
    }
    break;

    case 6 :
    {
      KnobHeight    =  9;
    }
    break;

    default :
    {
      if (Items > 0)
      {
        KnobHeight  =  Y1 / Items;
      }
    }
    break;

  }

  if (Act < Min)
  {
    Act  =  Min;
  }
  if (Act > Max)
  {
    Act  =  Max;
  }

  // Draw scroll bar
  dLcdRect((*UiInstance.pLcd).Lcd,Color,X,Y,X1,Y1);

  // Draw nob
  Tmp  =  Y;
  if ((Items > 1) && (Act > 0))
  {
    Tmp        +=  ((Y1 - KnobHeight) * (Act - 1)) / (Items - 1);
  }

  switch (X1)
  {
    case 5 :
    {
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
    }
    break;

    case 6 :
    {
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 4,Tmp);
      Tmp++;
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
      Tmp++;
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 1,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 4,Tmp);
      Tmp++;
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 2,Tmp);
      dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X + 3,Tmp);
    }
    break;

    default :
    {
      dLcdFillRect((*UiInstance.pLcd).Lcd,Color,X,Tmp,X1,KnobHeight);
    }
    break;

  }

}


RESULT    cUiBrowser(DATA8 Type,DATA16 X,DATA16 Y,DATA16 X1,DATA16 Y1,DATA8 Lng,DATA8 *pType,DATA8 *pAnswer)
{
  RESULT  Result = BUSY;
  DATA32  Image;
  BROWSER *pB;
  PRGID   PrgId;
  OBJID   ObjId;
  DATA16  Tmp;
  DATA16  Indent;
  DATA16  Item;
  DATA16  TotalItems;
  DATA8   TmpType;
  DATA8   Folder;
  DATA8   OldPriority;
  DATA8   Priority;
  DATA8   Color;
  DATA16  Ignore;
  DATA8   Data8;
  DATA32  Total;
  DATA32  Free;
  RESULT  TmpResult;
  HANDLER TmpHandle;

  PrgId   =  CurrentProgramId();
  ObjId   =  CallingObjectId();
  pB      =  &UiInstance.Browser;

  Color   =  FG_COLOR;

  // Test ignore horizontal update
  if ((Type & 0x20))
  {
    Ignore  =  -1;
  }
  else
  {
    if ((Type & 0x10))
    {
      Ignore  =  1;
    }
    else
    {
      Ignore  =  0;
    }
  }

  // Isolate browser type
  Type   &=  0x0F;

  CheckUsbstick(&Data8,&Total,&Free,0);
  if (Data8)
  {
    UiInstance.UiUpdate   =  1;
  }
  CheckSdcard(&Data8,&Total,&Free,0);
  if (Data8)
  {
    UiInstance.UiUpdate   =  1;
  }

  if (ProgramStatusChange(USER_SLOT) == STOPPED)
  {
    if (Type != BROWSE_FILES)
    {
      Result              =  OK;
      *pType              =  0;
#ifdef DEBUG
      printf("Browser interrupted\r\n");
#endif
    }
  }

  if ((*pType == TYPE_REFRESH_BROWSER))
  {
    UiInstance.UiUpdate   =  1;
  }

  if ((*pType == TYPE_RESTART_BROWSER))
  {
    if ((*pB).hFiles)
    {
      cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
    }
    if ((*pB).hFolders)
    {
      cMemoryCloseFolder((*pB).PrgId,&(*pB).hFolders);
    }
    (*pB).PrgId         =  0;
    (*pB).ObjId         =  0;
//    pAnswer[0]          =  0;
    *pType              =  0;
#ifdef DEBUG
    printf("Restarting browser\r\n");
#endif
  }

  if (((*pB).PrgId == 0) && ((*pB).ObjId == 0))
  {
//* INIT *****************************************************************************************************

    // Define screen
    (*pB).ScreenStartX  =  X;
    (*pB).ScreenStartY  =  Y;
    (*pB).ScreenWidth   =  X1;
    (*pB).ScreenHeight  =  Y1;

    // calculate lines on screen
    (*pB).LineSpace     =  5;
    (*pB).IconHeight    =  dLcdGetIconHeight(NORMAL_ICON);
    (*pB).LineHeight    =  (*pB).IconHeight + (*pB).LineSpace;
    (*pB).Lines         =  ((*pB).ScreenHeight / (*pB).LineHeight);

    // calculate chars and lines on screen
    (*pB).CharWidth     =  dLcdGetFontWidth(NORMAL_FONT);
    (*pB).CharHeight    =  dLcdGetFontHeight(NORMAL_FONT);
    (*pB).IconWidth     =  dLcdGetIconWidth(NORMAL_ICON);
    (*pB).Chars         =  (((*pB).ScreenWidth - (*pB).IconWidth) / (*pB).CharWidth);

    // calculate start of icon
    (*pB).IconStartX    =  cUiAlignX((*pB).ScreenStartX);
    (*pB).IconStartY    =  (*pB).ScreenStartY + (*pB).LineSpace / 2;

    // calculate start of text
    (*pB).TextStartX    =  cUiAlignX((*pB).ScreenStartX + (*pB).IconWidth);
    (*pB).TextStartY    =  (*pB).ScreenStartY + ((*pB).LineHeight - (*pB).CharHeight) / 2;

    // Calculate selection barBrowser
    (*pB).SelectStartX  =  (*pB).ScreenStartX + 1;
    (*pB).SelectWidth   =  (*pB).ScreenWidth - ((*pB).CharWidth + 5);
    (*pB).SelectStartY  =  (*pB).IconStartY - 1;
    (*pB).SelectHeight  =  (*pB).IconHeight + 2;

    // Calculate scroll bar
    (*pB).ScrollWidth   =  6;
    (*pB).NobHeight     =  9;
    (*pB).ScrollStartX  =  (*pB).ScreenStartX + (*pB).ScreenWidth - (*pB).ScrollWidth;
    (*pB).ScrollStartY  =  (*pB).ScreenStartY + 1;
    (*pB).ScrollHeight  =  (*pB).ScreenHeight - 2;
    (*pB).ScrollSpan    =  (*pB).ScrollHeight - (*pB).NobHeight;

    strncpy((char*)(*pB).TopFolder,(char*)pAnswer,MAX_FILENAME_SIZE);

    (*pB).PrgId         =  PrgId;
    (*pB).ObjId         =  ObjId;

    (*pB).OldFiles        =  0;
    (*pB).Folders         =  0;
    (*pB).OpenFolder      =  0;
    (*pB).Files           =  0;
    (*pB).ItemStart       =  1;
    (*pB).ItemPointer     =  1;
    (*pB).NeedUpdate      =  1;
    UiInstance.UiUpdate   =  1;
  }

  if (((*pB).PrgId == PrgId) && ((*pB).ObjId == ObjId))
  {
//* CTRL *****************************************************************************************************


    if (UiInstance.UiUpdate)
    {
      UiInstance.UiUpdate  =  0;
#ifdef DEBUG
    printf("Refreshing browser\r\n");
#endif

      if ((*pB).hFiles)
      {
        cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
      }
      if ((*pB).hFolders)
      {
        cMemoryCloseFolder((*pB).PrgId,&(*pB).hFolders);
      }

      (*pB).OpenFolder    =  0;
      (*pB).Files         =  0;
      *pType              =  0;

      switch (Type)
      {
        case BROWSE_FOLDERS :
        case BROWSE_FOLDS_FILES :
        {
          if (cMemoryOpenFolder(PrgId,TYPE_FOLDER,(*pB).TopFolder,&(*pB).hFolders) == OK)
          {
      #ifdef DEBUG
            printf("\r\n%d %d Opening browser in %s\r\n",PrgId,ObjId,(char*)(*pB).TopFolder);
      #endif
//******************************************************************************************************
            if ((*pB).OpenFolder)
            {
              cMemoryGetItem((*pB).PrgId,(*pB).hFolders,(*pB).OpenFolder,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).SubFolder,&TmpType);
  #ifdef DEBUG
              printf("Open  folder %3d (%s)\r\n",(*pB).OpenFolder,(*pB).SubFolder);
  #endif
              if (strcmp((char*)(*pB).SubFolder,SDCARD_FOLDER) == 0)
              {
                Item    =  (*pB).ItemPointer;
                cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,MAX_FILENAME_SIZE,(*pB).Filename,pType,&Priority);
                Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFolders,Item,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).FullPath,pType);
                *pType  =  TYPE_SDCARD;

                snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);
              }
              else
              {
                if (strcmp((char*)(*pB).SubFolder,USBSTICK_FOLDER) == 0)
                {
                  Item    =  (*pB).ItemPointer;
                  cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,MAX_FILENAME_SIZE,(*pB).Filename,pType,&Priority);
                  Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFolders,Item,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).FullPath,pType);
                  *pType  =  TYPE_USBSTICK;

                  snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);
                }
                else
                {
                  Result  =  cMemoryOpenFolder(PrgId,FILETYPE_UNKNOWN,(*pB).SubFolder,&(*pB).hFiles);
                  Result  =  BUSY;
                }
              }
            }
//******************************************************************************************************
          }
          else
          {
      #ifdef DEBUG
            printf("\r\n%d %d Open error\r\n",PrgId,ObjId);
      #endif
            (*pB).PrgId         =  0;
            (*pB).ObjId         =  0;
          }
        }
        break;

        case BROWSE_CACHE:
        {
        }
        break;

        case BROWSE_FILES :
        {
          if (cMemoryOpenFolder(PrgId,FILETYPE_UNKNOWN,(*pB).TopFolder,&(*pB).hFiles) == OK)
          {
      #ifdef DEBUG
            printf("\r\n%d %d Opening browser in %s\r\n",PrgId,ObjId,(char*)(*pB).TopFolder);
      #endif

          }
          else
          {
      #ifdef DEBUG
            printf("\r\n%d %d Open error\r\n",PrgId,ObjId);
      #endif
            (*pB).PrgId         =  0;
            (*pB).ObjId         =  0;
          }
        }
        break;

      }
    }

#ifndef DISABLE_SDCARD_SUPPORT
    if (strstr((char*)(*pB).SubFolder,SDCARD_FOLDER) != NULL)
    {
      (*pB).Sdcard  =  1;
    }
    else
    {
      (*pB).Sdcard  =  0;
    }
#endif

#ifndef DISABLE_USBSTICK_SUPPORT
    if (strstr((char*)(*pB).SubFolder,USBSTICK_FOLDER) != NULL)
    {
      (*pB).Usbstick  =  1;
    }
    else
    {
      (*pB).Usbstick  =  0;
    }
#endif

    TmpResult  =  OK;
    switch (Type)
    {
      case BROWSE_FOLDERS :
      case BROWSE_FOLDS_FILES :
      {
        // Collect folders in directory
        TmpResult  =  cMemoryGetFolderItems((*pB).PrgId,(*pB).hFolders,&(*pB).Folders);

        // Collect files in folder
        if (((*pB).OpenFolder) && (TmpResult == OK))
        {
          TmpResult  =  cMemoryGetFolderItems((*pB).PrgId,(*pB).hFiles,&(*pB).Files);
        }
      }
      break;

      case BROWSE_CACHE :
      {
        (*pB).Folders   =  (DATA16)cMemoryGetCacheFiles();
      }
      break;

      case BROWSE_FILES :
      {
        TmpResult  =  cMemoryGetFolderItems((*pB).PrgId,(*pB).hFiles,&(*pB).Files);
      }
      break;

    }

    if (((*pB).OpenFolder != 0) && ((*pB).OpenFolder == (*pB).ItemPointer))
    {
      if (cUiGetShortPress(BACK_BUTTON))
      {
        // Close folder
  #ifdef DEBUG
        printf("Close folder %3d\r\n",(*pB).OpenFolder);
  #endif

        cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
        if ((*pB).ItemPointer > (*pB).OpenFolder)
        {
          (*pB).ItemPointer -=  (*pB).Files;
        }
        (*pB).OpenFolder  =  0;
        (*pB).Files       =  0;
      }
    }

#ifndef DISABLE_SDCARD_SUPPORT
    if ((*pB).Sdcard == 1)
    {
      if ((*pB).OpenFolder == 0)
      {
        if (cUiGetShortPress(BACK_BUTTON))
        {
          // Collapse sdcard
    #ifdef DEBUG
          printf("Collapse sdcard\r\n");
    #endif
          if ((*pB).hFiles)
          {
            cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
          }
          if ((*pB).hFolders)
          {
            cMemoryCloseFolder((*pB).PrgId,&(*pB).hFolders);
          }
          (*pB).PrgId         =  0;
          (*pB).ObjId         =  0;
          strcpy((char*)pAnswer,vmPRJS_DIR);
          *pType              =  0;
          (*pB).SubFolder[0]  =  0;
        }
      }
    }
#endif
#ifndef DISABLE_USBSTICK_SUPPORT
    if ((*pB).Usbstick == 1)
    {
      if ((*pB).OpenFolder == 0)
      {
        if (cUiGetShortPress(BACK_BUTTON))
        {
          // Collapse usbstick
    #ifdef DEBUG
          printf("Collapse usbstick\r\n");
    #endif
          if ((*pB).hFiles)
          {
            cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
          }
          if ((*pB).hFolders)
          {
            cMemoryCloseFolder((*pB).PrgId,&(*pB).hFolders);
          }
          (*pB).PrgId         =  0;
          (*pB).ObjId         =  0;
          strcpy((char*)pAnswer,vmPRJS_DIR);
          *pType              =  0;
          (*pB).SubFolder[0]  =  0;
        }
      }
    }
#endif
    if ((*pB).OldFiles != ((*pB).Files + (*pB).Folders))
    {
      (*pB).OldFiles    =  ((*pB).Files + (*pB).Folders);
      (*pB).NeedUpdate  =  1;
    }

    if (cUiGetShortPress(ENTER_BUTTON))
    {
      (*pB).OldFiles      =  0;
      if ((*pB).OpenFolder)
      {
        if (((*pB).ItemPointer > (*pB).OpenFolder) && ((*pB).ItemPointer <= ((*pB).OpenFolder + (*pB).Files)))
        { // File selected

          Item    =  (*pB).ItemPointer - (*pB).OpenFolder;
          Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFiles,Item,Lng,(*pB).FullPath,pType);

          snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);


#ifdef DEBUG
          printf("Select file %3d\r\n",Item);
#endif
        }
        else
        { // Folder selected

          if ((*pB).OpenFolder == (*pB).ItemPointer)
          { // Enter on open folder

            Item    =  (*pB).OpenFolder;
            Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFolders,Item,Lng,pAnswer,pType);
#ifdef DEBUG
            printf("Select folder %3d\r\n",Item);
#endif
          }
          else
          { // Close folder

#ifdef DEBUG
            printf("Close folder %3d\r\n",(*pB).OpenFolder);
#endif

            cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
            if ((*pB).ItemPointer > (*pB).OpenFolder)
            {
              (*pB).ItemPointer -=  (*pB).Files;
            }
            (*pB).OpenFolder  =  0;
            (*pB).Files       =  0;
          }
        }
      }
      if ((*pB).OpenFolder == 0)
      { // Open folder

        switch (Type)
        {
          case BROWSE_FOLDERS :
          { // Folder

            Item    =  (*pB).ItemPointer;
            cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,MAX_FILENAME_SIZE,(*pB).Filename,pType,&Priority);
            Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFolders,Item,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).FullPath,pType);

            snprintf((char*)pAnswer,Lng,"%s/%s",(char*)(*pB).FullPath,(char*)(*pB).Filename);
            *pType  =  TYPE_BYTECODE;
#ifdef DEBUG
            printf("Select folder %3d\r\n",Item);
#endif
          }
          break;

          case BROWSE_FOLDS_FILES :
          { // Folder & File

            (*pB).OpenFolder  =  (*pB).ItemPointer;
            cMemoryGetItem((*pB).PrgId,(*pB).hFolders,(*pB).OpenFolder,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).SubFolder,&TmpType);
#ifdef DEBUG
            printf("Open  folder %3d (%s)\r\n",(*pB).OpenFolder,(*pB).SubFolder);
#endif
            if (strcmp((char*)(*pB).SubFolder,SDCARD_FOLDER) == 0)
            {
              Item    =  (*pB).ItemPointer;
              cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,MAX_FILENAME_SIZE,(*pB).Filename,pType,&Priority);
              Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFolders,Item,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).FullPath,pType);
              *pType  =  TYPE_SDCARD;

              snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);
            }
            else
            {
              if (strcmp((char*)(*pB).SubFolder,USBSTICK_FOLDER) == 0)
              {
                Item    =  (*pB).ItemPointer;
                cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,MAX_FILENAME_SIZE,(*pB).Filename,pType,&Priority);
                Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFolders,Item,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(*pB).FullPath,pType);
                *pType  =  TYPE_USBSTICK;

                snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);
              }
              else
              {
                (*pB).ItemStart  =  (*pB).ItemPointer;
                Result  =  cMemoryOpenFolder(PrgId,FILETYPE_UNKNOWN,(*pB).SubFolder,&(*pB).hFiles);

                Result  =  BUSY;
              }
            }
          }
          break;

          case BROWSE_CACHE :
          { // Cache

            Item    =  (*pB).ItemPointer;

            *pType  =  cMemoryGetCacheName(Item,FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE,(char*)(*pB).FullPath,(char*)(*pB).Filename);
            snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);
            Result  =  OK;
#ifdef DEBUG
            printf("Select folder %3d\r\n",Item);
#endif
          }
          break;

          case BROWSE_FILES :
          { // File

            if (((*pB).ItemPointer > (*pB).OpenFolder) && ((*pB).ItemPointer <= ((*pB).OpenFolder + (*pB).Files)))
            { // File selected

              Item    =  (*pB).ItemPointer - (*pB).OpenFolder;

              Result  =  cMemoryGetItem((*pB).PrgId,(*pB).hFiles,Item,Lng,(*pB).FullPath,pType);

              snprintf((char*)pAnswer,Lng,"%s",(char*)(*pB).FullPath);
              Result  =  OK;
  #ifdef DEBUG
              printf("Select file %3d\r\n",Item);
  #endif
            }
          }
          break;

        }
      }
      (*pB).NeedUpdate    =  1;
    }

    TotalItems  =  (*pB).Folders + (*pB).Files;
    if (TmpResult == OK)
    {
      if (TotalItems)
      {
        if ((*pB).ItemPointer > TotalItems)
        {

          (*pB).ItemPointer   =  TotalItems;
          (*pB).NeedUpdate    =  1;
        }
        if ((*pB).ItemStart > (*pB).ItemPointer)
        {
          (*pB).ItemStart     = (*pB).ItemPointer;
          (*pB).NeedUpdate    =  1;
        }
      }
      else
      {
        (*pB).ItemStart       =  1;
        (*pB).ItemPointer     =  1;
      }
    }

    Tmp  =  cUiGetVert();
    if (Tmp != 0)
    { // up/down arrow pressed

      (*pB).NeedUpdate    =  1;

      // Calculate item pointer
      (*pB).ItemPointer    +=  Tmp;
      if ((*pB).ItemPointer < 1)
      {
        (*pB).ItemPointer   =  1;
        (*pB).NeedUpdate    =  0;
      }
      if ((*pB).ItemPointer > TotalItems)
      {
        (*pB).ItemPointer   =  TotalItems;
        (*pB).NeedUpdate    =  0;
      }
    }

    // Calculate item start
    if ((*pB).ItemPointer < (*pB).ItemStart)
    {
      if ((*pB).ItemPointer > 0)
      {
        (*pB).ItemStart  =  (*pB).ItemPointer;
      }
    }
    if ((*pB).ItemPointer >= ((*pB).ItemStart + (*pB).Lines))
    {
      (*pB).ItemStart  =  (*pB).ItemPointer - (*pB).Lines;
      (*pB).ItemStart++;
    }

    if ((*pB).NeedUpdate)
    {
//* UPDATE ***************************************************************************************************
      (*pB).NeedUpdate    =  0;

#ifdef DEBUG
#ifndef DISABLE_SDCARD_SUPPORT
      if ((*pB).Sdcard)
      {
        printf("SDCARD\r\n");
      }
#endif
#ifndef DISABLE_USBSTICK_SUPPORT
      if ((*pB).Usbstick)
      {
        printf("USBSTICK\r\n");
      }
#endif
      printf("Folders = %3d, OpenFolder = %3d, Files = %3d, ItemStart = %3d, ItemPointer = %3d, TotalItems = %3d\r\n\n",(*pB).Folders,(*pB).OpenFolder,(*pB).Files,(*pB).ItemStart,(*pB).ItemPointer,TotalItems);
#endif

      // clear screen
      dLcdFillRect((*UiInstance.pLcd).Lcd,BG_COLOR,(*pB).ScreenStartX,(*pB).ScreenStartY,(*pB).ScreenWidth,(*pB).ScreenHeight);

      OldPriority   =  0;
      for (Tmp = 0;Tmp < (*pB).Lines;Tmp++)
      {
        Item          =  Tmp + (*pB).ItemStart;
        Folder        =  1;
        Priority      =  OldPriority;

        if (Item <= TotalItems)
        {
          if ((*pB).OpenFolder)
          {
            if ((Item > (*pB).OpenFolder) && (Item <= ((*pB).OpenFolder + (*pB).Files)))
            {
              Item -=  (*pB).OpenFolder;
              Folder  =  0;
            }
            else
            {
              if (Item > (*pB).OpenFolder)
              {
                Item -=  (*pB).Files;
              }
            }
          }
#ifdef DEBUG
          if ((*pB).ItemPointer == (Tmp + (*pB).ItemStart))
          {
            printf("> ");
          }
          else
          {
            printf("  ");
          }
#endif

//*** Graphics ***********************************************************************************************

          if (Folder)
          { // Show folder

            switch (Type)
            {
              case BROWSE_FOLDERS :
              {
                cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,(*pB).Chars,(*pB).Filename,&TmpType,&Priority);
                if (cMemoryGetItemIcon((*pB).PrgId,(*pB).hFolders,Item,&TmpHandle,&Image) == OK)
                {
                  dLcdDrawBitmap((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),(IP)Image);
                  cMemoryCloseFile((*pB).PrgId,TmpHandle);
                }
                else
                {
                  dLcdDrawPicture((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),PCApp_width,PCApp_height,(UBYTE*)PCApp_bits);
                }

                (*pB).Text[0]  =  0;
                if (strcmp((char*)(*pB).Filename,"Bluetooth") == 0)
                {
                  if (UiInstance.BtOn)
                  {
                    (*pB).Text[0]  =  '+';
                  }
                  else
                  {
                    (*pB).Text[0]  =  '-';
                  }
                }
                else
                {
                  if (strcmp((char*)(*pB).Filename,"WiFi") == 0)
                  {
                    if (UiInstance.WiFiOn)
                    {
                      (*pB).Text[0]  =  '+';
                    }
                    else
                    {
                      (*pB).Text[0]  =  '-';
                    }
                  }
                  else
                  {
                    if (cMemoryGetItemText((*pB).PrgId,(*pB).hFolders,Item,(*pB).Chars,(*pB).Text) != OK)
                    {
                      (*pB).Text[0]  =  0;
                    }
                  }
                }
                switch ((*pB).Text[0])
                {
                  case 0 :
                  {
                  }
                  break;

                  case '+' :
                  {
                    Indent  =  ((*pB).Chars - 1) *  (*pB).CharWidth - dLcdGetIconWidth(MENU_ICON);
                    dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).TextStartX + Indent,((*pB).TextStartY - 2) + (Tmp * (*pB).LineHeight),MENU_ICON,ICON_CHECKED);
                  }
                  break;

                  case '-' :
                  {
                    Indent  =  ((*pB).Chars - 1) *  (*pB).CharWidth - dLcdGetIconWidth(MENU_ICON);
                    dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).TextStartX + Indent,((*pB).TextStartY - 2) + (Tmp * (*pB).LineHeight),MENU_ICON,ICON_CHECKBOX);
                  }
                  break;

                  default :
                  {
                    Indent  =  (((*pB).Chars - 1) - (DATA16)strlen((char*)(*pB).Text)) * (*pB).CharWidth;
                    dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pB).TextStartX + Indent,(*pB).TextStartY + (Tmp * (*pB).LineHeight),NORMAL_FONT,(*pB).Text);
                  }
                  break;

                }

              }
              break;

              case BROWSE_FOLDS_FILES :
              {
                cMemoryGetItemName((*pB).PrgId,(*pB).hFolders,Item,(*pB).Chars,(*pB).Filename,&TmpType,&Priority);
                dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,FiletypeToNormalIcon[TmpType]);

                if ((Priority == 1) || (Priority == 2))
                {
                  dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,ICON_FOLDER2);
                }
                else
                {
                  if (Priority == 3)
                  {
                    dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,ICON_SD);
                  }
                  else
                  {
                    if (Priority == 4)
                    {
                      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,ICON_USB);
                    }
                    else
                    {
                      dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,FiletypeToNormalIcon[TmpType]);
                    }
                  }
                }
                if (Priority != OldPriority)
                {
                  if ((Priority == 1) || (Priority >= 3))
                  {
                    if (Tmp)
                    {
                      dLcdDrawDotLine((*UiInstance.pLcd).Lcd,Color,(*pB).SelectStartX,(*pB).SelectStartY + ((Tmp - 1) * (*pB).LineHeight) + (*pB).LineHeight - 2,(*pB).SelectStartX + (*pB).SelectWidth,(*pB).SelectStartY + ((Tmp - 1) * (*pB).LineHeight) + (*pB).LineHeight - 2,1,2);
                    }
                  }
                }
              }
              break;

              case BROWSE_CACHE :
              {
                TmpType  =  cMemoryGetCacheName(Item,(*pB).Chars,(char*)(*pB).FullPath,(char*)(*pB).Filename);
                dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,FiletypeToNormalIcon[TmpType]);
              }
              break;

              case BROWSE_FILES :
              {
                cMemoryGetItemName((*pB).PrgId,(*pB).hFiles,Item,(*pB).Chars,(*pB).Filename,&TmpType,&Priority);
                dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,FiletypeToNormalIcon[TmpType]);
              }
              break;

            }
            // Draw folder name
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pB).TextStartX,(*pB).TextStartY + (Tmp * (*pB).LineHeight),NORMAL_FONT,(*pB).Filename);

            // Draw open folder
            if (Item == (*pB).OpenFolder)
            {
              dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,144,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,ICON_OPENFOLDER);
            }

            // Draw selection
            if ((*pB).ItemPointer == (Tmp + (*pB).ItemStart))
            {
              dLcdInverseRect((*UiInstance.pLcd).Lcd,(*pB).SelectStartX,(*pB).SelectStartY + (Tmp * (*pB).LineHeight),(*pB).SelectWidth + 1,(*pB).SelectHeight);
            }

            // Draw end line
            switch (Type)
            {
              case BROWSE_FOLDERS :
              case BROWSE_FOLDS_FILES :
              case BROWSE_FILES :
              {
                if (((Tmp + (*pB).ItemStart) == TotalItems) && (Tmp < ((*pB).Lines - 1)))
                {
                  dLcdDrawDotLine((*UiInstance.pLcd).Lcd,Color,(*pB).SelectStartX,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 2,(*pB).SelectStartX + (*pB).SelectWidth,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 2,1,2);
                }
              }
              break;

              case BROWSE_CACHE :
              {
                if (((Tmp + (*pB).ItemStart) == 1) && (Tmp < ((*pB).Lines - 1)))
                {
                  dLcdDrawDotLine((*UiInstance.pLcd).Lcd,Color,(*pB).SelectStartX,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 2,(*pB).SelectStartX + (*pB).SelectWidth,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 2,1,2);
                }
                if (((Tmp + (*pB).ItemStart) == TotalItems) && (Tmp < ((*pB).Lines - 1)))
                {
                  dLcdDrawDotLine((*UiInstance.pLcd).Lcd,Color,(*pB).SelectStartX,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 2,(*pB).SelectStartX + (*pB).SelectWidth,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 2,1,2);
                }
              }
              break;

            }

#ifdef DEBUG
            printf("%s %d %d %d\r\n",(char*)(*pB).Filename,Item,(*pB).OpenFolder,Priority);
#endif
          }
          else
          { // Show file

            // Get file name and type
            cMemoryGetItemName((*pB).PrgId,(*pB).hFiles,Item,(*pB).Chars - 1,(*pB).Filename,&TmpType,&Priority);

            // show File icons
            dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX + (*pB).CharWidth,(*pB).IconStartY + (Tmp * (*pB).LineHeight),NORMAL_ICON,FiletypeToNormalIcon[TmpType]);

            // Draw file name
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pB).TextStartX + (*pB).CharWidth,(*pB).TextStartY + (Tmp * (*pB).LineHeight),NORMAL_FONT,(*pB).Filename);

            // Draw folder line
            if ((Tmp == ((*pB).Lines - 1)) || (Item == (*pB).Files))
            {
              dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX + (*pB).CharWidth - 3,(*pB).SelectStartY + (Tmp * (*pB).LineHeight),(*pB).IconStartX + (*pB).CharWidth - 3,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).SelectHeight - 1);
            }
            else
            {
              dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,(*pB).IconStartX + (*pB).CharWidth - 3,(*pB).SelectStartY + (Tmp * (*pB).LineHeight),(*pB).IconStartX + (*pB).CharWidth - 3,(*pB).SelectStartY + (Tmp * (*pB).LineHeight) + (*pB).LineHeight - 1);
            }

            // Draw selection
            if ((*pB).ItemPointer == (Tmp + (*pB).ItemStart))
            {
              dLcdInverseRect((*UiInstance.pLcd).Lcd,(*pB).SelectStartX + (*pB).CharWidth,(*pB).SelectStartY + (Tmp * (*pB).LineHeight),(*pB).SelectWidth + 1 - (*pB).CharWidth,(*pB).SelectHeight);
            }

#ifdef DEBUG
            printf(" | %s\r\n",(char*)(*pB).Filename);
#endif

          }

//************************************************************************************************************
        }
#ifdef DEBUG
        else
        {
          printf("\r\n");
        }
#endif
        OldPriority  =  Priority;
      }
#ifdef DEBUG
      printf("\r\n");
#endif

      cUiDrawBar(1,(*pB).ScrollStartX,(*pB).ScrollStartY,(*pB).ScrollWidth,(*pB).ScrollHeight,0,TotalItems,(*pB).ItemPointer);

      // Update
      cUiUpdateLcd();
      UiInstance.ScreenBusy       =  0;
    }

    if (Result != OK)
    {
      Tmp  =  cUiTestHorz();
      if (Ignore == Tmp)
      {
        Tmp  =  cUiGetHorz();
        Tmp  =  0;
      }

      if ((Tmp != 0) || (cUiTestShortPress(BACK_BUTTON)) || (cUiTestLongPress(BACK_BUTTON)))
      {
        if (Type != BROWSE_CACHE)
        {
          if ((*pB).OpenFolder)
          {
            if ((*pB).hFiles)
            {
              cMemoryCloseFolder((*pB).PrgId,&(*pB).hFiles);
            }
          }
          if ((*pB).hFolders)
          {
            cMemoryCloseFolder((*pB).PrgId,&(*pB).hFolders);
          }
        }
        (*pB).PrgId         =  0;
        (*pB).ObjId         =  0;
        (*pB).SubFolder[0]  =  0;
        pAnswer[0]          =  0;
        *pType              =  0;

  #ifdef DEBUG
        printf("%d %d Closing browser with [%s] type [%d]\r\n",PrgId,ObjId,(char*)pAnswer,*pType);
  #endif
        Result  =  OK;
      }
    }
    else
    {
      (*pB).NeedUpdate    =  1;
    }
  }
  else
  {
    pAnswer[0]          =  0;
    *pType              =  TYPE_RESTART_BROWSER;
    Result              =  FAIL;
  }

  if (*pType > 0)
  {
#ifndef DISABLE_SDCARD_SUPPORT
    if ((*pB).Sdcard)
    {
      *pType |=  TYPE_SDCARD;
    }
#endif
#ifndef DISABLE_USBSTICK_SUPPORT
    if ((*pB).Usbstick)
    {
      *pType |=  TYPE_USBSTICK;
    }
#endif
  }

  if (Result != BUSY)
  {
//* EXIT *****************************************************************************************************

#ifdef DEBUG
    printf("%d %d Return from browser with [%s] type [0x%02X]\r\n\n",PrgId,ObjId,(char*)pAnswer,*pType);
#endif
  }

  return (Result);
}


static char Delimiter[][3] =
{
  [DEL_NONE]      = "",
  [DEL_TAB]       = "\t",
  [DEL_SPACE]     = " ",
  [DEL_RETURN]    = "\r",
  [DEL_COLON]     = ":",
  [DEL_COMMA]     = ",",
  [DEL_LINEFEED]  = "\n",
  [DEL_CRLF]      = "\r\n",
};


DATA16    cUiTextboxGetLines(DATA8 *pText,DATA32 Size,DATA8 Del)
{
  DATA32  Point = 0;
  DATA16  Lines = 0;
  DATA8   DelPoi;

  if (Del < DELS)
  {
    while (pText[Point] && (Point < Size))
    {
      DelPoi  =  0;
      while ((pText[Point]) && (Point < Size) && (Delimiter[Del][DelPoi]) && (pText[Point] == Delimiter[Del][DelPoi]))
      {
        DelPoi++;
        Point++;
      }
      if (Delimiter[Del][DelPoi] == 0)
      {
        Lines++;
      }
      else
      {
        if ((pText[Point]) && (Point < Size))
        {
          Point++;
        }
      }
    }
  }

  return (Lines);
}


void      cUiTextboxAppendLine(DATA8 *pText,DATA32 Size,DATA8 Del,DATA8 *pLine,DATA8 Font)
{
  DATA32  Point = 0;
  DATA8   DelPoi = 0;

  if (Del < DELS)
  {
    while ((pText[Point]) && (Point < Size))
    {
      Point++;
    }
    if ((Point < Size) && (Font))
    {
      pText[Point]  =  Font;
      Point++;
    }

    while ((Point < Size) && (*pLine))
    {
      pText[Point]  =  *pLine;
      Point++;
      pLine++;
    }
    while ((Point < Size) && (Delimiter[Del][DelPoi]))
    {
      pText[Point]  =  Delimiter[Del][DelPoi];
      Point++;
      DelPoi++;
    }
  }
}


DATA32    cUiTextboxFindLine(DATA8 *pText,DATA32 Size,DATA8 Del,DATA16 Line,DATA8 *pFont)
{
  DATA32  Result = -1;
  DATA32  Point = 0;
  DATA8   DelPoi = 0;

  *pFont  =  0;
  if (Del < DELS)
  {
    Result  =  Point;
    while ((Line) && (pText[Point]) && (Point < Size))
    {

      DelPoi  =  0;
      while ((pText[Point]) && (Point < Size) && (Delimiter[Del][DelPoi]) && (pText[Point] == Delimiter[Del][DelPoi]))
      {
        DelPoi++;
        Point++;
      }
      if (Delimiter[Del][DelPoi] == 0)
      {
        Line--;
        if (Line)
        {
          Result  =  Point;
        }
      }
      else
      {
        if ((pText[Point]) && (Point < Size))
        {
          Point++;
        }
      }
    }
    if (Line != 0)
    {
      Result  = -1;
    }
    if (Result >= 0)
    {
      if ((pText[Result] > 0) && (pText[Result] < FONTTYPES))
      {
        *pFont  =  pText[Result];
        Result++;
      }
    }
  }

  return (Result);
}

void      cUiTextboxReadLine(DATA8 *pText,DATA32 Size,DATA8 Del,DATA8 Lng,DATA16 Line,DATA8 *pLine,DATA8 *pFont)
{
  DATA32  Start;
  DATA32  Point = 0;
  DATA8   DelPoi = 0;
  DATA8   Run = 1;

  Start  =  cUiTextboxFindLine(pText,Size,Del,Line,pFont);
  Point  =  Start;

  pLine[0]  =  0;

  if (Point >= 0)
  {
    while ((Run) && (pText[Point]) && (Point < Size))
    {
      DelPoi  =  0;
      while ((pText[Point]) && (Point < Size) && (Delimiter[Del][DelPoi]) && (pText[Point] == Delimiter[Del][DelPoi]))
      {
        DelPoi++;
        Point++;
      }
      if (Delimiter[Del][DelPoi] == 0)
      {
        Run  =  0;
      }
      else
      {
        if ((pText[Point]) && (Point < Size))
        {
          Point++;
        }
      }
    }
    Point -= (DATA32)DelPoi;

    if (((Point - Start) + 1) < (DATA32)Lng)
    {
      Lng  =  (DATA8)(Point - Start) + 1;
    }
    snprintf((char*)pLine,Lng,"%s",(char*)&pText[Start]);
  }
}


RESULT    cUiTextbox(DATA16 X,DATA16 Y,DATA16 X1,DATA16 Y1,DATA8 *pText,DATA32 Size,DATA8 Del,DATA16 *pLine)
{
  RESULT  Result = BUSY;
  TXTBOX  *pB;
  DATA16  Item;
  DATA16  TotalItems;
  DATA16  Tmp;
  DATA16  Ypos;
  DATA8   Color;

  pB      =  &UiInstance.Txtbox;
  Color   =  FG_COLOR;

  if (*pLine < 0)
  {
//* INIT *****************************************************************************************************
    // Define screen
    (*pB).ScreenStartX  =  X;
    (*pB).ScreenStartY  =  Y;
    (*pB).ScreenWidth   =  X1;
    (*pB).ScreenHeight  =  Y1;

    (*pB).Font          =  UiInstance.Font;

    // calculate chars and lines on screen
    (*pB).CharWidth     =  dLcdGetFontWidth((*pB).Font);
    (*pB).CharHeight    =  dLcdGetFontHeight((*pB).Font);
    (*pB).Chars         =  ((*pB).ScreenWidth / (*pB).CharWidth);

    // calculate lines on screen
    (*pB).LineSpace     =  5;
    (*pB).LineHeight    =  (*pB).CharHeight + (*pB).LineSpace;
    (*pB).Lines         =  ((*pB).ScreenHeight / (*pB).LineHeight);

    // calculate start of text
    (*pB).TextStartX    =  cUiAlignX((*pB).ScreenStartX);
    (*pB).TextStartY    =  (*pB).ScreenStartY + ((*pB).LineHeight - (*pB).CharHeight) / 2;

    // Calculate selection barBrowser
    (*pB).SelectStartX  =  (*pB).ScreenStartX;
    (*pB).SelectWidth   =  (*pB).ScreenWidth - ((*pB).CharWidth + 5);
    (*pB).SelectStartY  =  (*pB).TextStartY - 1;
    (*pB).SelectHeight  =  (*pB).CharHeight + 1;

    // Calculate scroll bar
    (*pB).ScrollWidth   =  5;
    (*pB).NobHeight     =  7;
    (*pB).ScrollStartX  =  (*pB).ScreenStartX + (*pB).ScreenWidth - (*pB).ScrollWidth;
    (*pB).ScrollStartY  =  (*pB).ScreenStartY + 1;
    (*pB).ScrollHeight  =  (*pB).ScreenHeight - 2;
    (*pB).ScrollSpan    =  (*pB).ScrollHeight - (*pB).NobHeight;

    (*pB).Items         =  cUiTextboxGetLines(pText,Size,Del);
    (*pB).ItemStart     =  1;
    (*pB).ItemPointer   =  1;

    (*pB).NeedUpdate    =  1;

    *pLine  =  0;
  }

  TotalItems  =  (*pB).Items;

  Tmp  =  cUiGetVert();
  if (Tmp != 0)
  { // up/down arrow pressed

    (*pB).NeedUpdate    =  1;

    // Calculate item pointer
    (*pB).ItemPointer    +=  Tmp;
    if ((*pB).ItemPointer < 1)
    {
      (*pB).ItemPointer   =  1;
      (*pB).NeedUpdate    =  0;
    }
    if ((*pB).ItemPointer > TotalItems)
    {
      (*pB).ItemPointer   =  TotalItems;
      (*pB).NeedUpdate    =  0;
    }
  }

  // Calculate item start
  if ((*pB).ItemPointer < (*pB).ItemStart)
  {
    if ((*pB).ItemPointer > 0)
    {
      (*pB).ItemStart  =  (*pB).ItemPointer;
    }
  }
  if ((*pB).ItemPointer >= ((*pB).ItemStart + (*pB).Lines))
  {
    (*pB).ItemStart  =  (*pB).ItemPointer - (*pB).Lines;
    (*pB).ItemStart++;
  }

  if (cUiGetShortPress(ENTER_BUTTON))
  {
    *pLine  =  (*pB).ItemPointer;

    Result  =  OK;
  }
  if (cUiGetShortPress(BACK_BUTTON))
  {
    *pLine  =  -1;

    Result  =  OK;
  }


  if ((*pB).NeedUpdate)
  {
//* UPDATE ***************************************************************************************************
    (*pB).NeedUpdate    =  0;

    // clear screen
    dLcdFillRect((*UiInstance.pLcd).Lcd,BG_COLOR,(*pB).ScreenStartX,(*pB).ScreenStartY,(*pB).ScreenWidth,(*pB).ScreenHeight);

    Ypos  =  (*pB).TextStartY + 2;

    for (Tmp = 0;Tmp < (*pB).Lines;Tmp++)
    {
      Item          =  Tmp + (*pB).ItemStart;

      if (Item <= TotalItems)
      {
        cUiTextboxReadLine(pText,Size,Del,TEXTSIZE,Item,(*pB).Text,&(*pB).Font);

        // calculate chars and lines on screen
        (*pB).CharWidth     =  dLcdGetFontWidth((*pB).Font);
        (*pB).CharHeight    =  dLcdGetFontHeight((*pB).Font);

        // calculate lines on screen
        (*pB).LineSpace     =  2;
        (*pB).LineHeight    =  (*pB).CharHeight + (*pB).LineSpace;
        (*pB).Lines         =  ((*pB).ScreenHeight / (*pB).LineHeight);

        // Calculate selection barBrowser
        (*pB).SelectStartX  =  (*pB).ScreenStartX;
        (*pB).SelectWidth   =  (*pB).ScreenWidth - ((*pB).ScrollWidth + 2);
        (*pB).SelectStartY  =  (*pB).TextStartY - 1;
        (*pB).SelectHeight  =  (*pB).CharHeight + 1;

        (*pB).Chars         =  ((*pB).SelectWidth / (*pB).CharWidth);

        (*pB).Text[(*pB).Chars]  =  0;

        if ((Ypos + (*pB).LineHeight) <= ((*pB).ScreenStartY + (*pB).ScreenHeight))
        {
          dLcdDrawText((*UiInstance.pLcd).Lcd,Color,(*pB).TextStartX,Ypos,(*pB).Font,(*pB).Text);
        }
        else
        {
          Tmp  =  (*pB).Lines;
        }
      }

      cUiDrawBar(1,(*pB).ScrollStartX,(*pB).ScrollStartY,(*pB).ScrollWidth,(*pB).ScrollHeight,0,TotalItems,(*pB).ItemPointer);

      if ((Ypos + (*pB).LineHeight) <= ((*pB).ScreenStartY + (*pB).ScreenHeight))
      {
        // Draw selection
        if ((*pB).ItemPointer == (Tmp + (*pB).ItemStart))
        {
          dLcdInverseRect((*UiInstance.pLcd).Lcd,(*pB).SelectStartX,Ypos - 1,(*pB).SelectWidth,(*pB).LineHeight);
        }
      }
      Ypos +=  (*pB).LineHeight;
    }

    // Update
    cUiUpdateLcd();
    UiInstance.ScreenBusy       =  0;
  }

  return(Result);
}


void      cUiGraphSetup(DATA16 StartX,DATA16 SizeX,DATA8 Items,DATA16 *pOffset,DATA16 *pSpan,DATAF *pMin,DATAF *pMax,DATAF *pVal)
{
  DATA16  Item;
  DATA16  Pointer;

  UiInstance.Graph.Initialized  =  0;

  UiInstance.Graph.pOffset  =  pOffset;
  UiInstance.Graph.pSpan    =  pSpan;
  UiInstance.Graph.pMin     =  pMin;
  UiInstance.Graph.pMax     =  pMax;
  UiInstance.Graph.pVal     =  pVal;

  if (Items < 0)
  {
    Items  =  0;
  }
  if (Items > GRAPH_BUFFERS)
  {
    Items  =  GRAPH_BUFFERS;
  }


  UiInstance.Graph.GraphStartX  =  StartX;
  UiInstance.Graph.GraphSizeX   =  SizeX;
  UiInstance.Graph.Items        =  Items;
  UiInstance.Graph.Pointer      =  0;

  for (Item = 0;Item < UiInstance.Graph.Items;Item++)
  {
    for (Pointer = 0;Pointer < UiInstance.Graph.GraphSizeX;Pointer++)
    {
      UiInstance.Graph.Buffer[Item][Pointer]  =  DATAF_NAN;
    }
  }

  UiInstance.Graph.Initialized  =  1;

  // Simulate graph
  UiInstance.Graph.Value        =  UiInstance.Graph.pMin[0];
  UiInstance.Graph.Down         =  0;
  UiInstance.Graph.Inc          =  (UiInstance.Graph.pMax[0] - UiInstance.Graph.pMin[0]) / (DATAF)20;
}


void      cUiGraphSample(void)
{
  DATAF   Sample;
  DATA16  Item;
  DATA16  Pointer;

  if (UiInstance.Graph.Initialized)
  { // Only if initialized

    if (UiInstance.Graph.Pointer < UiInstance.Graph.GraphSizeX)
    {
      for (Item = 0;Item < (UiInstance.Graph.Items);Item++)
      {
        // Insert sample
        Sample      =  UiInstance.Graph.pVal[Item];

        if (!(isnan(Sample)))
        {
          UiInstance.Graph.Buffer[Item][UiInstance.Graph.Pointer]  =  Sample;
        }
        else
        {
          UiInstance.Graph.Buffer[Item][UiInstance.Graph.Pointer]  =  DATAF_NAN;
        }
      }
      UiInstance.Graph.Pointer++;
    }
    else
    {
      // Scroll buffers
      for (Item = 0;Item < (UiInstance.Graph.Items);Item++)
      {
        for (Pointer = 0;Pointer < (UiInstance.Graph.GraphSizeX - 1);Pointer++)
        {
          UiInstance.Graph.Buffer[Item][Pointer]  =  UiInstance.Graph.Buffer[Item][Pointer + 1];
        }

        // Insert sample
        Sample      =  UiInstance.Graph.pVal[Item];

        if (!(isnan(Sample)))
        {
          UiInstance.Graph.Buffer[Item][Pointer]  =  Sample;
        }
        else
        {
          UiInstance.Graph.Buffer[Item][Pointer]  =  DATAF_NAN;
        }
      }
    }
  }
}


void      cUiGraphDraw(DATA8 View,DATAF *pActual,DATAF *pLowest,DATAF *pHighest,DATAF *pAverage)
{
  DATAF   Sample;
  DATA8   Samples;
  DATA16  Value;
  DATA16  Item;
  DATA16  Pointer;
  DATA16  X;
  DATA16  Y1;
  DATA16  Y2;
  DATA8   Color = 1;

  *pActual    =  DATAF_NAN;
  *pLowest    =  DATAF_NAN;
  *pHighest   =  DATAF_NAN;
  *pAverage   =  DATAF_NAN;
  Samples     =  0;

  if (UiInstance.Graph.Initialized)
  { // Only if initialized

    if (UiInstance.ScreenBlocked == 0)
    {

      // View or all
      if ((View >= 0) && (View < UiInstance.Graph.Items))
      {
        Item  =  View;

        Y1  =  (UiInstance.Graph.pOffset[Item] + UiInstance.Graph.pSpan[Item]);

        // Draw buffers
        X  =  UiInstance.Graph.GraphStartX;
        for (Pointer = 0;Pointer < UiInstance.Graph.Pointer;Pointer++)
        {
          Sample  =  UiInstance.Graph.Buffer[Item][Pointer];
          if (!(isnan(Sample)))
          {
            *pActual      =  Sample;
            if (isnan(*pAverage))
            {
              *pAverage   =  (DATAF)0;
              *pLowest    =  *pActual;
              *pHighest   =  *pActual;
            }
            else
            {
              if (*pActual < *pLowest)
              {
                *pLowest  =  *pActual;
              }
              if (*pActual > *pHighest)
              {
                *pHighest =  *pActual;
              }
            }
            *pAverage    +=  *pActual;
            Samples++;

            // Scale Y axis
            Value       =  (DATA16)((((Sample - UiInstance.Graph.pMin[Item]) * (DATAF)UiInstance.Graph.pSpan[Item]) / (UiInstance.Graph.pMax[Item] - UiInstance.Graph.pMin[Item])));

            // Limit Y axis
            if (Value > UiInstance.Graph.pSpan[Item])
            {
              Value  =  UiInstance.Graph.pSpan[Item];
            }
            if (Value < 0)
            {
              Value  =  0;
            }
/*
            printf("S=%-3d V=%3.0f L=%3.0f H=%3.0f A=%3.0f v=%3.0f ^=%3.0f O=%3d S=%3d Y=%d\r\n",Samples,*pActual,*pLowest,*pHighest,*pAverage,UiInstance.Graph.pMin[Item],UiInstance.Graph.pMax[Item],UiInstance.Graph.pOffset[Item],UiInstance.Graph.pSpan[Item],Value);
*/
            Y2  =  (UiInstance.Graph.pOffset[Item] + UiInstance.Graph.pSpan[Item]) - Value;
            if (Pointer > 1)
            {
              if (Y2 > Y1)
              {
                dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 2,Y1 - 1,X - 1,Y2 - 1);
                dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X,Y1 + 1,X + 1,Y2 + 1);
              }
              else
              {
                if (Y2 < Y1)
                {
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X,Y1 - 1,X + 1,Y2 - 1);
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 2,Y1 + 1,X - 1,Y2 + 1);
                }
                else
                {
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 1,Y1 - 1,X,Y2 - 1);
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 1,Y1 + 1,X,Y2 + 1);
                }
              }
              dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 1,Y1,X,Y2);
            }
            else
            {
              dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X,Y2);
            }

            Y1  =  Y2;
          }
          X++;
        }
        if (Samples != 0)
        {
          *pAverage  =  *pAverage / (DATAF)Samples;
        }

      }
      else
      {
        // Draw buffers
        for (Item = 0;Item < UiInstance.Graph.Items;Item++)
        {
          Y1  =  (UiInstance.Graph.pOffset[Item] + UiInstance.Graph.pSpan[Item]);

          X  =  UiInstance.Graph.GraphStartX + 1;
          for (Pointer = 0;Pointer < UiInstance.Graph.Pointer;Pointer++)
          {
            Sample  =  UiInstance.Graph.Buffer[Item][Pointer];

            // Scale Y axis
            Value       =  (DATA16)((((Sample - UiInstance.Graph.pMin[Item]) * (DATAF)UiInstance.Graph.pSpan[Item]) / (UiInstance.Graph.pMax[Item] - UiInstance.Graph.pMin[Item])));

            // Limit Y axis
            if (Value > UiInstance.Graph.pSpan[Item])
            {
              Value  =  UiInstance.Graph.pSpan[Item];
            }
            if (Value < 0)
            {
              Value  =  0;
            }
            Y2  =  (UiInstance.Graph.pOffset[Item] + UiInstance.Graph.pSpan[Item]) - Value;
            if (Pointer > 1)
            {

              if (Y2 > Y1)
              {
                dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 2,Y1 - 1,X - 1,Y2 - 1);
                dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X,Y1 + 1,X + 1,Y2 + 1);
              }
              else
              {
                if (Y2 < Y1)
                {
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X,Y1 - 1,X + 1,Y2 - 1);
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 2,Y1 + 1,X - 1,Y2 + 1);
                }
                else
                {
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 1,Y1 - 1,X,Y2 - 1);
                  dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 1,Y1 + 1,X,Y2 + 1);
                }
              }
              dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X - 1,Y1,X,Y2);

            }
            else
            {
              dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X,Y2);
            }

            Y1  =  Y2;
            X++;
          }
        }
      }
      UiInstance.ScreenBusy  =  1;
    }
  }
}


//******* BYTE CODE SNIPPETS **************************************************


/*! \page cUi User Interface
 *  <hr size="1"/>
 *  <b>     opUI_DRAW (CMD, ....)  </b>
 *
 *- UI draw\n
 *- Dispatch status can change to BUSYBREAK
 *
 *  \param  (DATA8)   CMD           - \ref uidrawsubcode
 *
 *  - CMD = UPDATE\n
 *
 *\n
 *  - CMD = FILLRECT
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X size [0..LCD_WIDTH - X0]\n
 *    -  \param  (DATA16)  Y1       - Y size [0..LCD_HEIGHT - Y0]\n
 *
 *\n
 *  - CMD = INVERSERECT
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X size [0..LCD_WIDTH - X0]\n
 *    -  \param  (DATA16)  Y1       - Y size [0..LCD_HEIGHT - Y0]\n
 *
 *\n
 *  - CMD = RECT
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X size [0..LCD_WIDTH - X0]\n
 *    -  \param  (DATA16)  Y1       - Y size [0..LCD_HEIGHT - Y0]\n
 *
 *\n
 *  - CMD = PIXEL
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y cord [0..LCD_HEIGHT]\n
 *
 *\n
 *  - CMD = LINE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X end [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y1       - Y end [0..LCD_HEIGHT]\n
 *
 *\n
 *  - CMD = DOTLINE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X end [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y1       - Y end [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  ON       - On pixels\n
 *    -  \param  (DATA16)  OFF      - Off pixels\n
 *
 *\n
 *  - CMD = CIRCLE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  R        - Radius\n
 *
 *\n
 *  - CMD = FILLCIRCLE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  R        - Radius\n
 *
 *\n
 *  - CMD = TEXT
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   STRING   - First character in string to draw\n
 *
 *\n
 *  - CMD = ICON
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   TYPE     - Icon type (pool)\n
 *    -  \param  (DATA8)   NO       - Icon no\n
 *
 *\n
 *  - CMD = BMPFILE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   NAME     - First character in filename (character string)\n
 *
 *\n
 *  - CMD = PICTURE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA32)  *IP      - Address of picture\n
 *
 *\n
 *  - CMD = VALUE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATAF)   VALUE    - Value to write\n
 *    -  \param  (DATA8)   FIGURES  - Total number of figures inclusive decimal point\n
 *    -  \param  (DATA8)   DECIMALS - Number of decimals\n
 *
 *\n
 *  - CMD = VIEW_VALUE
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATAF)   VALUE    - Value to write\n
 *    -  \param  (DATA8)   FIGURES  - Total number of figures inclusive decimal point\n
 *    -  \param  (DATA8)   DECIMALS - Number of decimals\n
 *
 *\n
 *  - CMD = VIEW_UNIT
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X        - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y        - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATAF)   VALUE    - Value to write\n
 *    -  \param  (DATA8)   FIGURES  - Total number of figures inclusive decimal point\n
 *    -  \param  (DATA8)   DECIMALS - Number of decimals\n
 *    -  \param  (DATA8)   LENGTH   - Maximal string length\n
 *    -  \param  (DATA8)   STRING   - First character in string to draw\n
 *
 *\n
 *  - CMD = NOTIFICATION
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   ICON1    - First icon\n
 *    -  \param  (DATA8)   ICON2    - Second icon\n
 *    -  \param  (DATA8)   ICON2    - Third icon\n
 *    -  \param  (DATA8)   STRING   - First character in notification string\n
 *    -  \param  (DATA8)   *STATE   - State 0 = INIT\n
 *
 *\n
 *  - CMD = QUESTION
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   ICON1    - First icon\n
 *    -  \param  (DATA8)   ICON2    - Second icon\n
 *    -  \param  (DATA8)   STRING   - First character in question string\n
 *    -  \param  (DATA8)   *STATE   - State 0 = NO, 1 = OK\n
 *    -  \return (DATA8)   OK       - Answer 0 = NO, 1 = OK, -1 = SKIP\n
 *
 *\n
 *  - CMD = ICON_QUESTION
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   *STATE   - State 0 = INIT\n
 *    -  \param  (DATA32)  ICONS    - bitfield with icons\n
 *
 *\n
 *  - CMD = KEYBOARD
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   LENGTH   - Maximal string length\n
 *    -  \param  (DATA8)   DEFAULT  - Default string (0 = none)\n
 *    -  \param  (DATA8)   *CHARSET - Internal use (must be a variable initialised by a "valid character set")\n
 *    -  \return (DATA8)   STRING   - First character in string receiving keyboard input\n
 *
 *\n
 *  - CMD = BROWSE
 *    -  \param  (DATA8)   TYPE     - \ref browsers
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X size [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y1       - Y size [0..LCD_HEIGHT]\n
 *    -  \param  (DATA8)   LENGTH   - Maximal string length\n
 *    -  \return (DATA8)   TYPE     - Item type (folder, byte code file, sound file, ...)(must be a zero initialised variable)\n
 *    -  \return (DATA8)   STRING   - First character in string receiving selected item name\n
 *
 *\n
 *  - CMD = VERTBAR
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR]\n
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  X1       - X size [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  Y1       - Y size [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  MIN      - Minimum value\n
 *    -  \param  (DATA16)  MAX      - Maximum value\n
 *    -  \param  (DATA16)  ACT      - Actual value\n
 *
 *\n
 *  - CMD = SELECT_FONT
 *    -  \param  (DATA8)   TYPE     - Font type [0..2] font will change to 0 when UPDATE is called\n
 *
 *\n
 *  - CMD = TOPLINE
 *    -  \param  (DATA8)   ENABLE   - Enable top status line (0 = disabled, 1 = enabled)\n
 *
 *\n
 *  - CMD = FILLWINDOW
 *    -  \param  (DATA8)   COLOR    - Color [BG_COLOR..FG_COLOR] (Color != BG_COLOR and FG_COLOR -> test pattern)\n
 *    -  \param  (DATA16)  Y0       - Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  Y1       - Y size [0..LCD_HEIGHT]\n
 *
 *\n
 *  - CMD = STORE
 *    -  \param  (DATA8)   NO       - Level number\n
 *
 *\n
 *  - CMD = RESTORE
 *    -  \param  (DATA8)   NO       - Level number (N=0 -> Saved screen just before run)\n
 *
 *\n
 *  - CMD = GRAPH_SETUP
 *    -  \param  (DATA16)  X0       - X start cord [0..LCD_WIDTH]\n
 *    -  \param  (DATA16)  X1       - X size [0..(LCD_WIDTH - X0)]\n
 *    -  \param  (DATA8)   ITEMS    - Number of datasets in arrayes\n
 *    -  \param  (DATA16)  OFFSET   - DATA16 array (handle) containing Y start cord [0..LCD_HEIGHT]\n
 *    -  \param  (DATA16)  SPAN     - DATA16 array (handle) containing Y size [0..(LCD_HEIGHT - hOFFSET[])]\n
 *    -  \param  (DATAF)   MIN      - DATAF array (handle) containing min values\n
 *    -  \param  (DATAF)   MAX      - DATAF array (handle) containing max values\n
 *    -  \param  (DATAF)   SAMPLE   - DATAF array (handle) containing sample values\n
 *
 *\n
 *  - CMD = GRAPH_DRAW
 *    -  \param  (DATA8)   VIEW     - Dataset number to view (0=all)\n
 *
 *\n
 *
 */
/*! \brief  opUI_DRAW byte code
 *
 */
void      cUiDraw(void)
{
  PRGID   TmpPrgId;
  OBJID   TmpObjId;
  IP      TmpIp;
  DATA8   GBuffer[25];
  UBYTE   pBmp[LCD_BUFFER_SIZE];
  DATA8   Cmd;
  DATA8   Color;
  DATA16  X;
  DATA16  Y;
  DATA16  X1;
  DATA16  Y1;
  DATA16  Y2;
  DATA16  Y3;
  DATA32  Size;
  DATA16  R;
  DATA8   *pText;
  DATA8   No;
  DATAF   DataF;
  DATA8   Figures;
  DATA8   Decimals;
  IP      pI;
  DATA8   *pState;
  DATA8   *pAnswer;
  DATA8   Lng;
  DATA8   SelectedChar;
  DATA8   *pType;
  DATA8   Type;
  DATA16  On;
  DATA16  Off;
  DATA16  CharWidth;
  DATA16  CharHeight;
  DATA8   TmpColor;
  DATA16  Tmp;
  DATA8   Length;
  DATA8   *pUnit;
  DATA32  *pIcons;
  DATA8   Items;
  DATA8   View;
  DATA16  *pOffset;
  DATA16  *pSpan;
  DATAF   *pMin;
  DATAF   *pMax;
  DATAF   *pVal;
  DATA16  Min;
  DATA16  Max;
  DATA16  Act;
  DATAF   Actual;
  DATAF   Lowest;
  DATAF   Highest;
  DATAF   Average;
  DATA8   Icon1;
  DATA8   Icon2;
  DATA8   Icon3;
  DATA8   Blocked;
  DATA8   Open;
  DATA8   Del;
  DATA8   *pCharSet;
  DATA16  *pLine;


  TmpPrgId      =  CurrentProgramId();

  if ((TmpPrgId != GUI_SLOT) && (TmpPrgId != DEBUG_SLOT))
  {
    UiInstance.RunScreenEnabled   =  0;
  }
  if (UiInstance.ScreenBlocked == 0)
  {
    Blocked  =  0;
  }
  else
  {
    TmpObjId      =  CallingObjectId();
    if ((TmpPrgId == UiInstance.ScreenPrgId) && (TmpObjId == UiInstance.ScreenObjId))
    {
      Blocked  =  0;
    }
    else
    {
      Blocked  =  1;
    }
  }

  TmpIp   =  GetObjectIp();
  Cmd     =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case UPDATE :
    {
      if (Blocked == 0)
      {
        cUiUpdateLcd();
        UiInstance.ScreenBusy  =  0;
      }
    }
    break;

    case CLEAN :
    {
      if (Blocked == 0)
      {
        UiInstance.Font  =  NORMAL_FONT;

        Color  =  BG_COLOR;
        if (Color)
        {
          Color   =  -1;
        }
        memset(&((*UiInstance.pLcd).Lcd[0]),Color,LCD_BUFFER_SIZE);

        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case TEXTBOX :
    {
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      X1        =  *(DATA16*)PrimParPointer();  // size x
      Y1        =  *(DATA16*)PrimParPointer();  // size y
      pText     =  (DATA8*)PrimParPointer();    // textbox
      Size      =  *(DATA32*)PrimParPointer();  // textbox size
      Del       =  *(DATA8*)PrimParPointer();   // delimitter
      pLine     =  (DATA16*)PrimParPointer();   // line

      if (Blocked == 0)
      {
        if (cUiTextbox(X,Y,X1,Y1,pText,Size,Del,pLine) == BUSY)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case FILLRECT :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      X1        =  *(DATA16*)PrimParPointer();
      Y1        =  *(DATA16*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdFillRect((*UiInstance.pLcd).Lcd,Color,X,Y,X1,Y1);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case INVERSERECT :
    {
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      X1        =  *(DATA16*)PrimParPointer();
      Y1        =  *(DATA16*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdInverseRect((*UiInstance.pLcd).Lcd,X,Y,X1,Y1);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case RECT :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      X1        =  *(DATA16*)PrimParPointer();
      Y1        =  *(DATA16*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdRect((*UiInstance.pLcd).Lcd,Color,X,Y,X1,Y1);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case PIXEL :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdDrawPixel((*UiInstance.pLcd).Lcd,Color,X,Y);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case LINE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      X1        =  *(DATA16*)PrimParPointer();
      Y1        =  *(DATA16*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdDrawLine((*UiInstance.pLcd).Lcd,Color,X,Y,X1,Y1);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case DOTLINE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      X1        =  *(DATA16*)PrimParPointer();
      Y1        =  *(DATA16*)PrimParPointer();
      On        =  *(DATA16*)PrimParPointer();
      Off       =  *(DATA16*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdDrawDotLine((*UiInstance.pLcd).Lcd,Color,X,Y,X1,Y1,On,Off);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case CIRCLE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      R         =  *(DATA16*)PrimParPointer();
      if (R)
      {
        if (Blocked == 0)
        {
          dLcdDrawCircle((*UiInstance.pLcd).Lcd,Color,X,Y,R);
          UiInstance.ScreenBusy  =  1;
        }
      }
    }
    break;

    case FILLCIRCLE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      R         =  *(DATA16*)PrimParPointer();
      if (R)
      {
        if (Blocked == 0)
        {
          dLcdDrawFilledCircle((*UiInstance.pLcd).Lcd,Color,X,Y,R);
          UiInstance.ScreenBusy  =  1;
        }
      }
    }
    break;

    case TEXT :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      pText     =  (DATA8*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X,Y,UiInstance.Font,pText);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case ICON :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      Type      =  *(DATA8*)PrimParPointer();
      No        =  *(DATA8*)PrimParPointer();
      if (Blocked == 0)
      {
        dLcdDrawIcon((*UiInstance.pLcd).Lcd,Color,X,Y,Type,No);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case BMPFILE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      pText     =  (DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        if (cMemoryGetImage(pText,LCD_BUFFER_SIZE,pBmp) == OK)
        {
          dLcdDrawBitmap((*UiInstance.pLcd).Lcd,Color,X,Y,pBmp);
          UiInstance.ScreenBusy  =  1;
        }
      }
    }
    break;

    case PICTURE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      pI        =  *(IP*)PrimParPointer();
      if (pI != NULL)
      {
        if (Blocked == 0)
        {
          dLcdDrawBitmap((*UiInstance.pLcd).Lcd,Color,X,Y,pI);
          UiInstance.ScreenBusy  =  1;
        }
      }
    }
    break;

    case VALUE :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();
      DataF     =  *(DATAF*)PrimParPointer();
      Figures   =  *(DATA8*)PrimParPointer();
      Decimals  =  *(DATA8*)PrimParPointer();

      if (isnan(DataF))
      {
        for (Lng = 0;Lng < Figures;Lng++)
        {
          GBuffer[Lng]  =  '-';
        }
      }
      else
      {
        if (Figures < 0)
        {
          Figures  =  0 - Figures;
          snprintf((char*)GBuffer,24,"%.*f",Decimals,DataF);
        }
        else
        {
          snprintf((char*)GBuffer,24,"%*.*f",Figures,Decimals,DataF);
        }
        if (GBuffer[0] == '-')
        { // Negative

          Figures++;
        }
      }
      GBuffer[Figures]  =  0;
      pText     =  GBuffer;
      if (Blocked == 0)
      {
        dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X,Y,UiInstance.Font,pText);
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case VIEW_VALUE :
    {
      Color       =  *(DATA8*)PrimParPointer();
      X           =  *(DATA16*)PrimParPointer();
      Y           =  *(DATA16*)PrimParPointer();
      DataF       =  *(DATAF*)PrimParPointer();
      Figures     =  *(DATA8*)PrimParPointer();
      Decimals    =  *(DATA8*)PrimParPointer();
      if (Blocked == 0)
      {

        TmpColor    =  Color;
        CharWidth   =  dLcdGetFontWidth(UiInstance.Font);
        CharHeight  =  dLcdGetFontHeight(UiInstance.Font);
        X1          =  ((CharWidth + 2) / 3) - 1;
        Y1          =  (CharHeight / 2);

        Lng         =  (DATA8)snprintf((char*)GBuffer,24,"%.*f",Decimals,DataF);

        if (Lng)
        {
          if (GBuffer[0] == '-')
          { // Negative

            TmpColor  =  Color;
            Lng--;
            pText     =  &GBuffer[1];
          }
          else
          { // Positive

            TmpColor  =  1 - Color;
            pText     =  GBuffer;
          }

          // Make sure negative sign is deleted from last time
          dLcdDrawLine((*UiInstance.pLcd).Lcd,1 - Color,X - X1,Y + Y1,X + (Figures * CharWidth),Y + Y1);
          if (CharHeight > 12)
          {
            dLcdDrawLine((*UiInstance.pLcd).Lcd,1 - Color,X - X1,Y + Y1 - 1,X + (Figures * CharWidth),Y + Y1 - 1);
          }

          // Check for "not a number"
          Tmp         =  0;
          while((pText[Tmp] != 0) && (pText[Tmp] != 'n'))
          {
            Tmp++;
          }
          if (pText[Tmp] == 'n')
          { // "nan"

            for (Tmp = 0;Tmp < (DATA16)Figures;Tmp++)
            {
              GBuffer[Tmp]  =  '-';
            }
            GBuffer[Tmp]    =  0;

            // Draw figures
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X,Y,UiInstance.Font,GBuffer);
          }
          else
          { // Normal number

            // Check number of figures
            if (Lng > Figures)
            { // Limit figures

              for (Tmp = 0;Tmp < (DATA16)Figures;Tmp++)
              {
                GBuffer[Tmp]  =  '>';
              }
              GBuffer[Tmp]    =  0;
              Lng             =  (DATA16)Figures;
              pText           =  GBuffer;
              TmpColor        =  1 - Color;

              // Find X indent
              Tmp             =  ((DATA16)Figures - Lng) * CharWidth;
            }
            else
            { // Centre figures

              // Find X indent
              Tmp             =  ((((DATA16)Figures - Lng) + 1) / 2) * CharWidth;
            }

            // Draw figures
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X + Tmp,Y,UiInstance.Font,pText);

            // Draw negative sign
            dLcdDrawLine((*UiInstance.pLcd).Lcd,TmpColor,X - X1 + Tmp,Y + Y1,X + Tmp,Y + Y1);
            if (CharHeight > 12)
            {
              dLcdDrawLine((*UiInstance.pLcd).Lcd,TmpColor,X - X1 + Tmp,Y + Y1 - 1,X + Tmp,Y + Y1 - 1);
            }
          }
        }
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case VIEW_UNIT :
    {
      Color       =  *(DATA8*)PrimParPointer();
      X           =  *(DATA16*)PrimParPointer();
      Y           =  *(DATA16*)PrimParPointer();
      DataF       =  *(DATAF*)PrimParPointer();
      Figures     =  *(DATA8*)PrimParPointer();
      Decimals    =  *(DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pUnit       =  (DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        TmpColor    =  Color;
        CharWidth   =  dLcdGetFontWidth(LARGE_FONT);
        CharHeight  =  dLcdGetFontHeight(LARGE_FONT);
        X1          =  ((CharWidth + 2) / 3) - 1;
        Y1          =  (CharHeight / 2);

        Lng         =  (DATA8)snprintf((char*)GBuffer,24,"%.*f",Decimals,DataF);

        if (Lng)
        {
          if (GBuffer[0] == '-')
          { // Negative

            TmpColor  =  Color;
            Lng--;
            pText     =  &GBuffer[1];
          }
          else
          { // Positive

            TmpColor  =  1 - Color;
            pText     =  GBuffer;
          }

          // Make sure negative sign is deleted from last time
          dLcdDrawLine((*UiInstance.pLcd).Lcd,1 - Color,X - X1,Y + Y1,X + (Figures * CharWidth),Y + Y1);
          if (CharHeight > 12)
          {
            dLcdDrawLine((*UiInstance.pLcd).Lcd,1 - Color,X - X1,Y + Y1 - 1,X + (Figures * CharWidth),Y + Y1 - 1);
          }

          // Check for "not a number"
          Tmp         =  0;
          while((pText[Tmp] != 0) && (pText[Tmp] != 'n'))
          {
            Tmp++;
          }
          if (pText[Tmp] == 'n')
          { // "nan"

            for (Tmp = 0;Tmp < (DATA16)Figures;Tmp++)
            {
              GBuffer[Tmp]  =  '-';
            }
            GBuffer[Tmp]    =  0;

            // Draw figures
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X,Y,LARGE_FONT,GBuffer);
          }
          else
          { // Normal number

            // Check number of figures
            if (Lng > Figures)
            { // Limit figures

              for (Tmp = 0;Tmp < (DATA16)Figures;Tmp++)
              {
                GBuffer[Tmp]  =  '>';
              }
              GBuffer[Tmp]    =  0;
              Lng             =  (DATA16)Figures;
              pText           =  GBuffer;
              TmpColor        =  1 - Color;

              // Find X indent
              Tmp             =  ((DATA16)Figures - Lng) * CharWidth;
            }
            else
            { // Centre figures

              // Find X indent
              Tmp             =  ((((DATA16)Figures - Lng) + 1) / 2) * CharWidth;
            }
            Tmp               =  0;

            // Draw figures
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X + Tmp,Y,LARGE_FONT,pText);

            // Draw negative sign
            dLcdDrawLine((*UiInstance.pLcd).Lcd,TmpColor,X - X1 + Tmp,Y + Y1,X + Tmp,Y + Y1);
            if (CharHeight > 12)
            {
              dLcdDrawLine((*UiInstance.pLcd).Lcd,TmpColor,X - X1 + Tmp,Y + Y1 - 1,X + Tmp,Y + Y1 - 1);
            }

            Tmp               =  ((((DATA16)Lng))) * CharWidth;
            snprintf((char*)GBuffer,Length,"%s",pUnit);
            dLcdDrawText((*UiInstance.pLcd).Lcd,Color,X + Tmp,Y,SMALL_FONT,GBuffer);

          }
        }
        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case NOTIFICATION :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      Icon1     =  *(DATA8*)PrimParPointer();
      Icon2     =  *(DATA8*)PrimParPointer();
      Icon3     =  *(DATA8*)PrimParPointer();
      pText     =  (DATA8*)PrimParPointer();
      pState    =  (DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        if (cUiNotification(Color,X,Y,Icon1,Icon2,Icon3,pText,pState) == BUSY)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case QUESTION :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      Icon1     =  *(DATA8*)PrimParPointer();
      Icon2     =  *(DATA8*)PrimParPointer();
      pText     =  (DATA8*)PrimParPointer();
      pState    =  (DATA8*)PrimParPointer();
      pAnswer   =  (DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        if (cUiQuestion(Color,X,Y,Icon1,Icon2,pText,pState,pAnswer) == BUSY)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;


    case ICON_QUESTION :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      pState    =  (DATA8*)PrimParPointer();
      pIcons    =  (DATA32*)PrimParPointer();

      if (Blocked == 0)
      {
        if (cUiIconQuestion(Color,X,Y,pState,pIcons) == BUSY)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;


    case KEYBOARD :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      No        =  *(DATA8*)PrimParPointer();   // Icon
      Lng       =  *(DATA8*)PrimParPointer();   // length
      pText     =  (DATA8*)PrimParPointer();    // default
      pCharSet  =  (DATA8*)PrimParPointer();    // valid char set
      pAnswer   =  (DATA8*)PrimParPointer();    // string

      if (VMInstance.Handle >= 0)
      {
        pAnswer  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }

      if (Blocked == 0)
      {
        SelectedChar  =  cUiKeyboard(Color,X,Y,No,Lng,pText,pCharSet,pAnswer);

        // Wait for "ENTER"
        if (SelectedChar != 0x0D)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case BROWSE :
    {
      Type      =  *(DATA8*)PrimParPointer();   // Browser type
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      X1        =  *(DATA16*)PrimParPointer();  // size x
      Y1        =  *(DATA16*)PrimParPointer();  // size y
      Lng       =  *(DATA8*)PrimParPointer();   // length
      pType     =  (DATA8*)PrimParPointer();    // item type
      pAnswer   =  (DATA8*)PrimParPointer();    // item name

      if (VMInstance.Handle >= 0)
      {
        pAnswer  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }

      if (Blocked == 0)
      {
        if (cUiBrowser(Type,X,Y,X1,Y1,Lng,pType,pAnswer) == BUSY)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case VERTBAR :
    {
      Color     =  *(DATA8*)PrimParPointer();
      X         =  *(DATA16*)PrimParPointer();  // start x
      Y         =  *(DATA16*)PrimParPointer();  // start y
      X1        =  *(DATA16*)PrimParPointer();  // size x
      Y1        =  *(DATA16*)PrimParPointer();  // size y
      Min       =  *(DATA16*)PrimParPointer();  // min
      Max       =  *(DATA16*)PrimParPointer();  // max
      Act       =  *(DATA16*)PrimParPointer();  // actual

      if (Blocked == 0)
      {
        cUiDrawBar(Color,X,Y,X1,Y1,Min,Max,Act);
      }
    }
    break;

    case SELECT_FONT :
    {
      UiInstance.Font  =  *(DATA8*)PrimParPointer();
      if (Blocked == 0)
      {
        if (UiInstance.Font >= FONTTYPES)
        {
          UiInstance.Font  =  (FONTTYPES - 1);
        }
        if (UiInstance.Font < 0)
        {
          UiInstance.Font  =  0;
        }
      }
    }
    break;

    case TOPLINE :
    {
      UiInstance.TopLineEnabled  =  *(DATA8*)PrimParPointer();
    }
    break;

    case FILLWINDOW :
    {
      Color     =  *(DATA8*)PrimParPointer();
      Y         =  *(DATA16*)PrimParPointer();  // start y
      Y1        =  *(DATA16*)PrimParPointer();  // size y
      if (Blocked == 0)
      {
        UiInstance.Font  =  NORMAL_FONT;

        if ((Y + Y1) < LCD_HEIGHT)
        {
          if ((Color == 0) || (Color == 1))
          {
            Y        *=  ((LCD_WIDTH + 7) / 8);

            if (Y1)
            {
              Y1     *=  ((LCD_WIDTH + 7) / 8);
            }
            else
            {
              Y1      =  LCD_BUFFER_SIZE - Y;
            }

            if (Color)
            {
              Color   =  -1;
            }
            memset(&((*UiInstance.pLcd).Lcd[Y]),Color,Y1);
          }
          else
          {
            if (Y1 == 0)
            {
              Y1  =  LCD_HEIGHT;
            }
            Y2  =  ((LCD_WIDTH + 7) / 8);
            for (Tmp = Y;Tmp < Y1;Tmp++)
            {
              Y3        =  Tmp * ((LCD_WIDTH + 7) / 8);
              memset(&((*UiInstance.pLcd).Lcd[Y3]),Color,Y2);
              Color  =  ~Color;
            }
          }
        }

        UiInstance.ScreenBusy  =  1;
      }
    }
    break;

    case STORE :
    {
      No  =  *(DATA8*)PrimParPointer();
      if (Blocked == 0)
      {
        if (No < LCD_STORE_LEVELS)
        {
          LCDCopy(&UiInstance.LcdSafe,&UiInstance.LcdPool[No],sizeof(LCD));
        }
      }
    }
    break;

    case RESTORE :
    {
      No  =  *(DATA8*)PrimParPointer();
      if (Blocked == 0)
      {
        if (No < LCD_STORE_LEVELS)
        {
          LCDCopy(&UiInstance.LcdPool[No],&UiInstance.LcdSafe,sizeof(LCD));
          UiInstance.ScreenBusy  =  1;
        }
      }
    }
    break;

    case GRAPH_SETUP :
    {
      X         =  *(DATA16*)PrimParPointer();  // start x
      X1        =  *(DATA16*)PrimParPointer();  // size y
      Items     =  *(DATA8*)PrimParPointer();   // items
      pOffset   =  (DATA16*)PrimParPointer();  // handle to offset Y
      pSpan     =  (DATA16*)PrimParPointer();  // handle to span y
      pMin      =  (DATAF*)PrimParPointer();  // handle to min
      pMax      =  (DATAF*)PrimParPointer();  // handle to max
      pVal      =  (DATAF*)PrimParPointer();  // handle to val

      if (Blocked == 0)
      {
        cUiGraphSetup(X,X1,Items,pOffset,pSpan,pMin,pMax,pVal);
      }
    }
    break;

    case GRAPH_DRAW :
    {
      View      =  *(DATA8*)PrimParPointer();   // view

      cUiGraphDraw(View,&Actual,&Lowest,&Highest,&Average);

      *(DATAF*)PrimParPointer()  =  Actual;
      *(DATAF*)PrimParPointer()  =  Lowest;
      *(DATAF*)PrimParPointer()  =  Highest;
      *(DATAF*)PrimParPointer()  =  Average;
    }
    break;

    case SCROLL :
    {
      Y  =  *(DATA16*)PrimParPointer();
      if ((Y > 0) && (Y < LCD_HEIGHT))
      {
        dLcdScroll((*UiInstance.pLcd).Lcd,Y);
      }
    }
    break;

    case POPUP :
    {
      Open  =  *(DATA8*)PrimParPointer();
      if (Blocked == 0)
      {
        if (Open)
        {
          if (!UiInstance.ScreenBusy)
          {
            TmpObjId                  =  CallingObjectId();

            LCDCopy(&UiInstance.LcdSafe,&UiInstance.LcdSave,sizeof(UiInstance.LcdSave));
            UiInstance.ScreenPrgId    =  TmpPrgId;
            UiInstance.ScreenObjId    =  TmpObjId;
            UiInstance.ScreenBlocked  =  1;
          }
          else
          { // Wait on scrreen

            SetObjectIp(TmpIp - 1);
            SetDispatchStatus(BUSYBREAK);
          }
        }
        else
        {
          LCDCopy(&UiInstance.LcdSave,&UiInstance.LcdSafe,sizeof(UiInstance.LcdSafe));
          dLcdUpdate(UiInstance.pLcd);

          UiInstance.ScreenPrgId      =  -1;
          UiInstance.ScreenObjId      =  -1;
          UiInstance.ScreenBlocked    =  0;
        }
      }
      else
      { // Wait on not blocked

        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

  }
}


/*! \page cUi
 *  <hr size="1"/>
 *  <b>     opUI_FLUSH ()  </b>
 *
 *- User Interface flush buffers\n
 *- Dispatch status unchanged
 *
 */
/*! \brief  opUI_FLUSH byte code
 *
 */
void      cUiFlush(void)
{
  cUiFlushBuffer();
}


/*! \page cUi
 *  <hr size="1"/>
 *  <b>     opUI_READ (CMD, ....)  </b>
 *
 *- User Interface read\n
 *- Dispatch status can change to BUSYBREAK and FAILBREAK
 *
 *  \param  (DATA8)   CMD     - \ref uireadsubcode
 *
 *\n
 *  - CMD = GET_STRING
 *\n  Get string from terminal\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = KEY
 *    -  \return (DATA8)   VALUE    - Key value from lms_cmdin (0 = no key)\n
 *
 *\n
 *  - CMD = GET_ADDRESS
 *    -  \return (DATA32)  VALUE    - Address from lms_cmdin\n
 *
 *\n
 *  - CMD = GET_CODE
 *    -  \param  (DATA32)  LENGTH   - Maximal code stream length\n
 *    -  \return (DATA32)  *IMAGE   - Address of image\n
 *    -  \return (DATA32)  *GLOBAL  - Address of global variables\n
 *    -  \return (DATA8)   FLAG     - Flag tells if image is ready\n
 *
 *\n
 *  - CMD = GET_HW_VERS
 *\n  Get hardware version string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_FW_VERS
 *\n  Get firmware version string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *
 *\n
 *  - CMD = GET_FW_BUILD
 *\n  Get firmware build string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_OS_VERS
 *\n  Get os version string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_OS_BUILD
 *\n  Get os build string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_VERSION
 *\n  Get version string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_IP
 *\n  Get IP address string\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_VBATT
 *    -  \return (DATAF)   VALUE    - Battery voltage [V]\n
 *
 *\n
 *  - CMD = GET_IBATT
 *    -  \return (DATAF)   VALUE    - Battery current [A]\n
 *
 *\n
 *  - CMD = GET_TBATT
 *    -  \return (DATAF)   VALUE    - Battery temperature rise [C]\n
 *
 *\n
 *  - CMD = GET_IMOTOR
 *    -  \return (DATAF)    VALUE    - Motor current [A]\n
 *
 *\n
 *  - CMD = GET_SDCARD
 *    -  \return (DATA8)    STATE    - SD card present [0..1]\n
 *    -  \return (DATA32)   TOTAL    - Kbytes in total\n
 *    -  \return (DATA32)   FREE     - Kbytes free\n
 *
 *\n
 *  - CMD = GET_USBSTICK
 *    -  \return (DATA8)    STATE    - USB stick present [0..1]\n
 *    -  \return (DATA32)   TOTAL    - Kbytes in total\n
 *    -  \return (DATA32)   FREE     - Kbytes free\n
 *
 *\n
 *  - CMD = GET_LBATT
 *\n  Get battery level in %\n
 *    -  \return (DATA8)    PCT      - Battery level [0..100]\n
 *
 *\n
 */
/*! \brief  opUI_READ byte code
 *
 */
void      cUiRead(void)
{
  IP      TmpIp;
  DATA8   Cmd;
  DATA8   Lng;
  DATA8   Data8;
  DATA8   *pSource;
  DATA8   *pDestination;
  DATA16  Data16;
  IMGDATA Tmp;
  DATA32  pImage;
  DATA32  Length;
  DATA32  Total;
  DATA32  Size;
  IMGHEAD *pImgHead;
  OBJHEAD *pObjHead;


  TmpIp   =  GetObjectIp();
  Cmd     =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case GET_STRING :
    {
      if (UiInstance.Keys)
      {
        Lng           = *(DATA8*)PrimParPointer();
        pDestination  =  (DATA8*)PrimParPointer();
        pSource       =  (DATA8*)UiInstance.KeyBuffer;

        while ((UiInstance.Keys) && (Lng))
        {
          *pDestination  =  *pSource;
          pDestination++;
          pSource++;
          UiInstance.Keys--;
          Lng--;
        }
        *pDestination  =  0;
        UiInstance.KeyBufIn       =  0;
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case KEY :
    {
      if (UiInstance.KeyBufIn)
      {
        *(DATA8*)PrimParPointer()  =  (DATA8)UiInstance.KeyBuffer[0];
        UiInstance.KeyBufIn--;

        for (Lng = 0;Lng < UiInstance.KeyBufIn;Lng++)
        {
          UiInstance.KeyBuffer[Lng]  =  UiInstance.KeyBuffer[Lng + 1];
        }
#ifdef  DEBUG_TRACE_KEY
        printf("%s",(char*)UiInstance.KeyBuffer);
#endif
      }
      else
      {
        *(DATA8*)PrimParPointer()  =  (DATA8)0;
      }
    }
    break;

    case GET_SHUTDOWN :
    {
      *(DATA8*)PrimParPointer()   =  UiInstance.ShutDown;
      UiInstance.ShutDown         =  0;
    }
    break;

    case GET_WARNING :
    {
      *(DATA8*)PrimParPointer()   =  UiInstance.Warning;
    }
    break;

    case GET_LBATT :
    {
      Data16  =  (DATA16)(UiInstance.Vbatt * 1000.0);
      Data16  -=  UiInstance.BattIndicatorLow;
      Data16   =  (Data16 * 100) / (UiInstance.BattIndicatorHigh - UiInstance.BattIndicatorLow);
      if (Data16 > 100)
      {
        Data16  =  100;
      }
      if (Data16 < 0)
      {
        Data16  =  0;
      }
      *(DATA8*)PrimParPointer()   =  (DATA8)Data16;
    }
    break;

    case ADDRESS :
    {
      if (UiInstance.Keys)
      {
        *(DATA32*)PrimParPointer()  =  (DATA32)atol((const char*)UiInstance.KeyBuffer);
        UiInstance.Keys     =  0;
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case CODE :
    {
      if (UiInstance.Keys)
      {
        Length        = *(DATA32*)PrimParPointer();
        pImage        = *(DATA32*)PrimParPointer();

        pImgHead      = (IMGHEAD*)pImage;
        pObjHead      = (OBJHEAD*)(pImage + sizeof(IMGHEAD));
        pDestination  = (DATA8*)(pImage + sizeof(IMGHEAD) + sizeof(OBJHEAD));

        if (Length > (sizeof(IMGHEAD) + sizeof(OBJHEAD)))
        {

          (*pImgHead).Sign[0]                =  'l';
          (*pImgHead).Sign[1]                =  'e';
          (*pImgHead).Sign[2]                =  'g';
          (*pImgHead).Sign[3]                =  'o';
          (*pImgHead).ImageSize              =  0;
          (*pImgHead).VersionInfo            =  (UWORD)(VERS * 100.0);
          (*pImgHead).NumberOfObjects        =  1;
          (*pImgHead).GlobalBytes            =  0;

          (*pObjHead).OffsetToInstructions   =  (IP)(sizeof(IMGHEAD) + sizeof(OBJHEAD));
          (*pObjHead).OwnerObjectId          =  0;
          (*pObjHead).TriggerCount           =  0;
          (*pObjHead).LocalBytes             =  MAX_COMMAND_LOCALS;

          pSource           =  (DATA8*)UiInstance.KeyBuffer;
          Size              =  sizeof(IMGHEAD) + sizeof(OBJHEAD);

          Length           -=  sizeof(IMGHEAD) + sizeof(OBJHEAD);
          Length--;
          while ((UiInstance.Keys) && (Length))
          {
            Tmp         =  (IMGDATA)(AtoN(*pSource) << 4);
            pSource++;
            UiInstance.Keys--;
            if (UiInstance.Keys)
            {
              Tmp      +=  (IMGDATA)(AtoN(*pSource));
              pSource++;
              UiInstance.Keys--;
            }
            else
            {
              Tmp       =  0;
            }
            *pDestination  =  Tmp;
            pDestination++;
            Length--;
            Size++;
          }
          *pDestination  =  opOBJECT_END;
          Size++;
          (*pImgHead).ImageSize         =  Size;
          memset(UiInstance.Globals,0,sizeof(UiInstance.Globals));

          *(DATA32*)PrimParPointer()    =  (DATA32)UiInstance.Globals;
          *(DATA8*)PrimParPointer()     =  1;
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case GET_HW_VERS :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        Data8  =  (DATA8)strlen((char*)UiInstance.HwVers) + 1;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.HwVers);
      }
    }
    break;

    case GET_FW_VERS :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        Data8  =  (DATA8)strlen((char*)UiInstance.FwVers) + 1;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.FwVers);
      }
    }
    break;

    case GET_FW_BUILD :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        Data8  =  (DATA8)strlen((char*)UiInstance.FwBuild) + 1;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.FwBuild);
      }
    }
    break;

    case GET_OS_VERS :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        Data8  =  (DATA8)strlen((char*)UiInstance.OsVers) + 1;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.OsVers);
      }
    }
    break;

    case GET_OS_BUILD :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        Data8  =  (DATA8)strlen((char*)UiInstance.OsBuild) + 1;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.OsBuild);
      }
    }
    break;

    case GET_VERSION :
    {
      snprintf((char*)UiInstance.ImageBuffer,IMAGEBUFFER_SIZE,"%s V%4.2f%c(%s %s)",PROJECT,VERS,SPECIALVERS,__DATE__,__TIME__);
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      pSource       =  (DATA8*)UiInstance.ImageBuffer;

      if (VMInstance.Handle >= 0)
      {
        Data8  =  (DATA8)strlen((char*)UiInstance.ImageBuffer) + 1;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.ImageBuffer);
      }
    }
    break;

    case GET_IP :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        Data8  =  IPADDR_SIZE;
        if ((Lng > Data8) || (Lng == -1))
        {
          Lng  =  Data8;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Lng);
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Lng,"%s",UiInstance.IpAddr);
      }

    }
    break;

    case GET_POWER :
    {
      *(DATAF*)PrimParPointer()  =  UiInstance.Vbatt;
      *(DATAF*)PrimParPointer()  =  UiInstance.Ibatt;
      *(DATAF*)PrimParPointer()  =  UiInstance.Iintegrated;
      *(DATAF*)PrimParPointer()  =  UiInstance.Imotor;
    }
    break;

    case GET_VBATT :
    {
      *(DATAF*)PrimParPointer()  =  UiInstance.Vbatt;
    }
    break;

    case GET_IBATT :
    {
      *(DATAF*)PrimParPointer()  =  UiInstance.Ibatt;
    }
    break;

    case GET_IINT :
    {
      *(DATAF*)PrimParPointer()  =  UiInstance.Iintegrated;
    }
    break;

    case GET_IMOTOR :
    {
      *(DATAF*)PrimParPointer()  =  UiInstance.Imotor;
    }
    break;

    case GET_EVENT :
    {
      *(DATA8*)PrimParPointer()  =  UiInstance.Event;
      UiInstance.Event           =  0;
    }
    break;

    case GET_TBATT :
    {
      *(DATAF*)PrimParPointer()  =  UiInstance.Tbatt;
    }
    break;

    case TEXTBOX_READ :
    {
      pSource       =  (DATA8*)PrimParPointer();
      Size          = *(DATA32*)PrimParPointer();
      Data8         = *(DATA8*)PrimParPointer();
      Lng           = *(DATA8*)PrimParPointer();
      Data16        = *(DATA16*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      cUiTextboxReadLine(pSource,Size,Data8,Lng,Data16,pDestination,&Data8);
    }
    break;

    case GET_SDCARD :
    {
      *(DATA8*)PrimParPointer()   =  CheckSdcard(&Data8,&Total,&Size,0);
      *(DATA32*)PrimParPointer()  =  Total;
      *(DATA32*)PrimParPointer()  =  Size;
    }
    break;

    case GET_USBSTICK :
    {
      *(DATA8*)PrimParPointer()   =  CheckUsbstick(&Data8,&Total,&Size,0);
      *(DATA32*)PrimParPointer()  =  Total;
      *(DATA32*)PrimParPointer()  =  Size;
    }
    break;

  }
}


/*! \page cUi
 *  <hr size="1"/>
 *  <b>     opUI_WRITE (CMD, ....)  </b>
 *
 *- UI write data\n
 *- Dispatch status can change to BUSYBREAK and FAILBREAK
 *
 *  \param  (DATA8)   CMD     - \ref uiwritesubcode
 *
 *\n
 *  - CMD = WRITE_FLUSH
 *
 *\n
 *  - CMD = FLOATVALUE
 *    -  \param  (DATAF)   VALUE    - Value to write\n
 *    -  \param  (DATA8)   FIGURES  - Total number of figures inclusive decimal point\n
 *    -  \param  (DATA8)   DECIMALS - Number of decimals\n
 *
 *\n
 *  - CMD = PUT_STRING
 *    -  \param  (DATA8)   STRING   - First character in string to write\n
 *
 *\n
 *  - CMD = CODE
 *    -  \param  (DATA8)   ARRAY    - First byte in byte array to write\n
 *    -  \param  (DATA32)  LENGTH   - Length of array\n
 *
 *\n
 *  - CMD = VALUE8
 *    -  \param  (DATA8)   VALUE    - Value to write\n
 *
 *\n
 *  - CMD = VALUE16
 *    -  \param  (DATA16)  VALUE    - Value to write\n
 *
 *\n
 *  - CMD = VALUE32
 *    -  \param  (DATA32)  VALUE    - Value to write\n
 *
 *\n
 *  - CMD = VALUEF
 *    -  \param  (DATAF)   VALUE    - Value to write\n
 *
 *\n
 *  - CMD = LED
 *    -  \param  (DATA8)   PATTERN  - \ref ledpatterns \n
 *
 *\n
 *  - CMD = SET_BUSY
 *    -  \param  (DATA8)   VALUE    - Value [0,1]\n
 *
 *\n
 *  - CMD = POWER
 *    -  \param  (DATA8)   VALUE    - Value [0,1]\n
 *
 *\n
 *  - CMD = TERMINAL
 *    -  \param  (DATA8)   STATE    - Value [0 = Off,1 = On]\n
 *
 *\n
 *  - CMD = SET_TESTPIN
 *    -  \param  (DATA8)   STATE    - Value [0 = low,1 = high]\n
 *
 *\n
 *  - CMD = INIT_RUN
 *
 *\n
 *  - CMD = UPDATE_RUN
 *
 *\n
 *  - CMD = GRAPH_SAMPLE
 *
 *\n
 *  - CMD = DOWNLOAD_END
 *\n  Send to brick when file down load is completed (plays sound and updates the UI browser)\n
 *
 *\n
 *
 *\n
 *
 */
/*! \brief  opUI_WRITE byte code
 *
 */
void      cUiWrite(void)
{
  IP      TmpIp;
  DATA8   Cmd;
  DATA8   *pSource;
  DSPSTAT DspStat = BUSYBREAK;
  DATA8   Buffer[50];
  DATA8   Data8;
  DATA16  Data16;
  DATA32  Data32;
  DATA32  pGlobal;
  DATA32  Tmp;
  DATAF   DataF;
  DATA8   Figures;
  DATA8   Decimals;
  DATA8   No;
  DATA8   *pText;


  TmpIp   =  GetObjectIp();
  Cmd     =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case WRITE_FLUSH :
    {
      cUiFlush();
      DspStat  =  NOBREAK;
    }
    break;

    case FLOATVALUE :
    {
      DataF     =  *(DATAF*)PrimParPointer();
      Figures   =  *(DATA8*)PrimParPointer();
      Decimals  =  *(DATA8*)PrimParPointer();

      snprintf((char*)Buffer,32,"%*.*f",Figures,Decimals,DataF);
      cUiWriteString(Buffer);

      DspStat  =  NOBREAK;
    }
    break;

    case STAMP :
    { // write time, prgid, objid, ip

      pSource  =  (DATA8*)PrimParPointer();
      snprintf((char*)Buffer,50,"####[ %09u %01u %03u %06u %s]####\r\n",GetTime(),CurrentProgramId(),CallingObjectId(),CurrentObjectIp(),pSource);
      cUiWriteString(Buffer);
      cUiFlush();
      DspStat  =  NOBREAK;
    }
    break;

    case PUT_STRING :
    {
      pSource  =  (DATA8*)PrimParPointer();
      cUiWriteString(pSource);
      DspStat  =  NOBREAK;
    }
    break;

    case CODE :
    {
      pGlobal  =  *(DATA32*)PrimParPointer();
      Data32   = *(DATA32*)PrimParPointer();

      pSource  =  (DATA8*)pGlobal;

      cUiWriteString((DATA8*)"\r\n    ");
      for (Tmp = 0;Tmp < Data32;Tmp++)
      {
        snprintf((char*)Buffer,7,"%02X ",pSource[Tmp] & 0xFF);
        cUiWriteString(Buffer);
        if (((Tmp & 0x3) == 0x3) && ((Tmp & 0xF) != 0xF))
        {
          cUiWriteString((DATA8*)" ");
        }
        if (((Tmp & 0xF) == 0xF) && (Tmp < (Data32 - 1)))
        {
          cUiWriteString((DATA8*)"\r\n    ");
        }
      }
      cUiWriteString((DATA8*)"\r\n");
      DspStat  =  NOBREAK;
    }
    break;

    case TEXTBOX_APPEND :
    {
      pText     =  (DATA8*)PrimParPointer();
      Data32    =  *(DATA32*)PrimParPointer();
      Data8     =  *(DATA8*)PrimParPointer();
      pSource   =  (DATA8*)PrimParPointer();

      cUiTextboxAppendLine(pText,Data32,Data8,pSource,UiInstance.Font);

      DspStat  =  NOBREAK;
    }
    break;

    case SET_BUSY :
    {
      Data8     =  *(DATA8*)PrimParPointer();

      if (Data8)
      {
        UiInstance.Warning |=  WARNING_BUSY;
      }
      else
      {
        UiInstance.Warning &= ~WARNING_BUSY;
      }

      DspStat  =  NOBREAK;
    }
    break;

    case VALUE8 :
    {
      Data8  =  *(DATA8*)PrimParPointer();
      if (Data8 != DATA8_NAN)
      {
        snprintf((char*)Buffer,7,"%d",(int)Data8);
      }
      else
      {
        snprintf((char*)Buffer,7,"nan");
      }
      cUiWriteString(Buffer);

      DspStat  =  NOBREAK;
    }
    break;

    case VALUE16 :
    {
      Data16  =  *(DATA16*)PrimParPointer();
      if (Data16 != DATA16_NAN)
      {
        snprintf((char*)Buffer,9,"%d",Data16 & 0xFFFF);
      }
      else
      {
        snprintf((char*)Buffer,7,"nan");
      }
      cUiWriteString(Buffer);

      DspStat  =  NOBREAK;
    }
    break;

    case VALUE32 :
    {
      Data32  =  *(DATA32*)PrimParPointer();
      if (Data32 != DATA32_NAN)
      {
        snprintf((char*)Buffer,14,"%ld",(long unsigned int)(Data32 & 0xFFFFFFFF));
      }
      else
      {
        snprintf((char*)Buffer,7,"nan");
      }

      cUiWriteString(Buffer);

      DspStat  =  NOBREAK;
    }
    break;

    case VALUEF :
    {
      DataF  =  *(DATAF*)PrimParPointer();
      snprintf((char*)Buffer,24,"%f",DataF);
      cUiWriteString(Buffer);

      DspStat  =  NOBREAK;
    }
    break;

    case LED :
    {
      Data8     =  *(DATA8*)PrimParPointer();
      if (Data8 < 0)
      {
        Data8   =  0;
      }
      if (Data8 >= LEDPATTERNS)
      {
        Data8   =  LEDPATTERNS - 1;
      }
      cUiSetLed(Data8);
      UiInstance.RunLedEnabled  =  0;

      DspStat  =  NOBREAK;
    }
    break;

    case POWER :
    {
      Data8  =  *(DATA8*)PrimParPointer();

      if (UiInstance.PowerFile >= 0)
      {
        ioctl(UiInstance.PowerFile,0,(size_t)&Data8);
      }

      DspStat  =  NOBREAK;
    }
    break;

    case TERMINAL :
    {
      No     =  *(DATA8*)PrimParPointer();

      if (No)
      {
        SetTerminalEnable(1);
      }
      else
      {
        SetTerminalEnable(0);
      }

      DspStat  =  NOBREAK;
    }
    break;

    case SET_TESTPIN :
    {
      Data8   =  *(DATA8*)PrimParPointer();
      cUiTestpin(Data8);
      DspStat  =  NOBREAK;
    }
    break;

    case INIT_RUN :
    {
      UiInstance.RunScreenEnabled  =  3;

      DspStat  =  NOBREAK;
    }
    break;

    case UPDATE_RUN :
    {
      DspStat  =  NOBREAK;
    }
    break;

    case GRAPH_SAMPLE :
    {
      cUiGraphSample();
      DspStat  =  NOBREAK;
    }
    break;

    case DOWNLOAD_END :
    {
      UiInstance.UiUpdate  =  1;
      cUiDownloadSuccesSound();
      DspStat  =  NOBREAK;
    }
    break;

    case SCREEN_BLOCK :
    {
      UiInstance.ScreenBlocked  =  *(DATA8*)PrimParPointer();
      DspStat  =  NOBREAK;
    }
    break;

    default :
    {
      DspStat  =  FAILBREAK;
    }
    break;

  }

  if (DspStat == BUSYBREAK)
  { // Rewind IP

    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cUi
 *  <hr size="1"/>
 *  <b>     opUI_BUTTON (CMD, ....)  </b>
 *
 *- UI button\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD           - \ref uibuttonsubcode
 *
 *  - CMD = FLUSH
 *
 *\n
 *
 *  - CMD = WAIT_FOR_PRESS
 *
 *\n
 *
 *  - CMD = PRESSED
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *    -  \return (DATA8)   STATE    - Button is pressed (0 = no, 1 = yes)\n
 *
 *\n
 *
 *  - CMD = SHORTPRESS
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *    -  \return (DATA8)   STATE    - Button has been pressed (0 = no, 1 = yes)\n
 *
 *\n
 *
 *  - CMD = GET_BUMBED
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *    -  \return (DATA8)   STATE    - Button has been pressed (0 = no, 1 = yes)\n
 *
 *\n
 *
 *  - CMD = LONGPRESS
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *    -  \return (DATA8)   STATE    - Button has been hold down(0 = no, 1 = yes)\n
 *
 *\n
 *  - CMD = PRESS
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *
 *\n
 *  - CMD = RELEASE
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *
 *\n
 *  - CMD = GET_HORZ
 *    -  \return (DATA16)  VALUE    - Horizontal arrows data (-1 = left, +1 = right, 0 = not pressed)\n
 *
 *\n
 *  - CMD = GET_VERT
 *    -  \return (DATA16)  VALUE    - Vertical arrows data (-1 = up, +1 = down, 0 = not pressed)\n
 *
 *\n
 *  - CMD = SET_BACK_BLOCK
 *    -  \param  (DATA8)   BLOCKED  - Set UI back button blocked flag (0 = not blocked, 1 = blocked)\n
 *
 *\n
 *  - CMD = GET_BACK_BLOCK
 *    -  \return (DATA8)   BLOCKED  - Get UI back button blocked flag (0 = not blocked, 1 = blocked)\n
 *
 *\n
 *
 *  - CMD = TESTSHORTPRESS
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *    -  \return (DATA8)   STATE    - Button has been hold down(0 = no, 1 = yes)\n
 *
 *\n
 *
 *  - CMD = TESTLONGPRESS
 *    -  \param  (DATA8)   BUTTON   - \ref buttons \n
 *    -  \return (DATA8)   STATE    - Button has been hold down(0 = no, 1 = yes)\n
 *
 *\n
 *
 */
/*! \brief  opUI_BUTTON byte code
 *
 */
void      cUiButton(void)
{
  PRGID   TmpPrgId;
  OBJID   TmpObjId;
  IP      TmpIp;
  DATA8   Cmd;
  DATA8   Button;
  DATA8   State;
  DATA16  Inc;
  DATA8   Blocked;

  TmpIp         =  GetObjectIp();
  TmpPrgId      =  CurrentProgramId();

  if (UiInstance.ScreenBlocked == 0)
  {
    Blocked  =  0;
  }
  else
  {
    TmpObjId      =  CallingObjectId();
    if ((TmpPrgId == UiInstance.ScreenPrgId) && (TmpObjId == UiInstance.ScreenObjId))
    {
      Blocked  =  0;
    }
    else
    {
      Blocked  =  1;
    }
  }

  Cmd     =  *(DATA8*)PrimParPointer();

  State   =  0;
  Inc     =  0;

  switch (Cmd)
  { // Function

    case PRESS :
    {
      Button  =  *(DATA8*)PrimParPointer();
      cUiSetPress(Button,1);
    }
    break;

    case RELEASE :
    {
      Button  =  *(DATA8*)PrimParPointer();
      cUiSetPress(Button,0);
    }
    break;

    case SHORTPRESS :
    {
      Button  =  *(DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        State  =  cUiGetShortPress(Button);
      }
      *(DATA8*)PrimParPointer()  =  State;
    }
    break;

    case GET_BUMBED :
    {
      Button  =  *(DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        State  =  cUiGetBumbed(Button);
      }
      *(DATA8*)PrimParPointer()  =  State;
    }
    break;

    case PRESSED :
    {
      Button  =  *(DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        State  =  cUiGetPress(Button);
      }
      *(DATA8*)PrimParPointer()  =  State;
    }
    break;

    case LONGPRESS :
    {
      Button  =  *(DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        State  =  cUiGetLongPress(Button);
      }
      *(DATA8*)PrimParPointer()  =  State;
    }
    break;

    case FLUSH :
    {
      if (Blocked == 0)
      {
        cUiButtonFlush();
      }
    }
    break;

    case WAIT_FOR_PRESS :
    {
      if (Blocked == 0)
      {
        if (cUiWaitForPress() == 0)
        {
          SetObjectIp(TmpIp - 1);
          SetDispatchStatus(BUSYBREAK);
        }
      }
      else
      {
        SetObjectIp(TmpIp - 1);
        SetDispatchStatus(BUSYBREAK);
      }
    }
    break;

    case GET_HORZ :
    {
      if (Blocked == 0)
      {
        Inc  =  cUiGetHorz();
      }
      *(DATA16*)PrimParPointer()  =  Inc;
    }
    break;

    case GET_VERT :
    {
      if (Blocked == 0)
      {
        Inc  =  cUiGetVert();
      }
      *(DATA16*)PrimParPointer()  =  Inc;
    }
    break;

    case SET_BACK_BLOCK :
    {
      UiInstance.BackButtonBlocked  =  *(DATA8*)PrimParPointer();
    }
    break;

    case GET_BACK_BLOCK :
    {
      *(DATA8*)PrimParPointer()  =  UiInstance.BackButtonBlocked;
    }
    break;

    case TESTSHORTPRESS :
    {
      Button  =  *(DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        State  =  cUiTestShortPress(Button);
      }
      *(DATA8*)PrimParPointer()  =  State;
    }
    break;

    case TESTLONGPRESS :
    {
      Button  =  *(DATA8*)PrimParPointer();

      if (Blocked == 0)
      {
        State  =  cUiTestLongPress(Button);
      }
      *(DATA8*)PrimParPointer()  =  State;
    }
    break;

    case GET_CLICK :
    {
      *(DATA8*)PrimParPointer()  =  UiInstance.Click;
      UiInstance.Click           =  0;
    }
    break;

  }
}


/*! \page cUi
 *  \anchor KEEPALIVE
 *  <hr size="1"/>
 *  <b>     opKEEP_ALIVE (MINUTES)  </b>
 *
 *- Keep alive\n
 *- Dispatch status unchanged
 *
 *  \return (DATA8)   MINUTES     - Number of minutes before sleep
 *
 *\n
 */
/*! \brief  opKEEP_ALIVE byte code
 *
 */
void      cUiKeepAlive(void)
{
  cUiAlive();
  *(DATA8*)PrimParPointer()  =  GetSleepMinutes();
}


//*****************************************************************************
