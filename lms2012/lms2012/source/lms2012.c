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


/*! \page virtualmachine Virtual Machine
 *
 *  The virtual machine will handle the actual program execution and call into the shared
 *  libraries (Functionality device drivers). Program execution is done through byte-code
 *  execution which all will represent an abstraction of various functionalities.
 *\n
 *  To help program memory allocation within the embedded targeted and program exe­cution
 *  speed we are targeting to implement a thin and very fast approach towards program execution.
 *  The virtual machine are represented as the blue layer under \ref architecture.
 *\n\n
 *  3rd party functionality which is needed within VM will be treated as sub-routines
 *  meaning that 3rd party can choose to implement needed functionality within the VM by
 *  implementing all this functionality as sub-routines written in byte-code.
 *\n
 *  Product shipping functionality should be implemented within the shared libraries to
 *  help program execution speed.
 *\n\n
 *  The VM will be implemented using memory based architecture to better match the output
 *  from the compiler.
 *\n\n
 *  Below is mentioned a few topics for the VM implementation:
 *\n
 *- Should not include polymorphism functionality => Data type definition done at compile time
 *- Should support the following data types
 *  -  Integer 8 – 16 – 32 bit
 *  -  Floats (IEEE 754 single precision - 32 bits)
 *  -  String
 *  -  Arrays (Implemented as shared library functionality)
 *
 *\n
 *
 *  The approach for enabling debug functionality has not been fully aligned at this point.
 *\n
 *  But in general we currently evaluate debug and normal execution as potentially being
 *  two different programs to avoid potentially slowing down program execution when debug
 *  functionality is not requested by the user.
 *\n\n
 */

#include  "lms2012.h"
#include  "c_math.h"
#include  "c_move.h"
#include  "c_branch.h"
#include  "c_compare.h"
#include  "c_timer.h"
#include  "c_output.h"
#include  "c_input.h"
#include  "c_ui.h"
#include  "c_memory.h"
#include  "c_com.h"
#include  "c_sound.h"
#ifndef Linux_X86
#include  "c_bt.h"
#include  "c_i2c.h"
#endif
#include  "validate.h"

#if       (HARDWARE != SIMULATION)

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <time.h>
#include  <syslog.h>
#include  <math.h>

#include  <dirent.h>
#include  <sys/stat.h>
#include  <sys/statvfs.h>
#include  <sys/types.h>
#include  <sys/time.h>
#include  <fcntl.h>
#include  <sys/ioctl.h>

#include  <sys/mman.h>


#endif

#ifdef    DEBUG_VM
#define   DEBUG
#endif

#ifdef    DEBUG
#define   DEBUG_TRACE_TASK
#define   DEBUG_TRACE_VM
#endif

// Buttons are mapped differently in the enum BUTTONTYPE and in ButtonState.
#define IDX_BACK_BUTTON BACK_BUTTON-1

//*****************************************************************************
// UI bootloader
//*****************************************************************************

UBYTE     UiImage[] =
{
  // IMAGE header
  PROGRAMHeader(0,1,0),                 // VersionInfo,Objects,GlobalBytes
  VMTHREADHeader(0,1),                  // OffsetToInstructions,LocalBytes

  opFILE,LC0(LOAD_IMAGE),LC0(GUI_SLOT),GV0(0),LV0(4),LV0(0),
  opPROGRAM_START,LC0(GUI_SLOT),LC0(0),LV0(0),LC0(0),
  opOBJECT_END,

};

#if       (HARDWARE != SIMULATION)
  GLOBALS VMInstance;
#else
  GLOBALS * gInstance;

  void setInstance(GLOBALS * _Instance)
  {
    gInstance= _Instance;
  }

  GLOBALS* getInstance()
  {
    return gInstance;
  }

#endif
//*****************************************************************************
// Forward declarations
//*****************************************************************************
#define   PRIMDISPATHTABLE_SIZE   256

extern PRIM      PrimDispatchTabel[PRIMDISPATHTABLE_SIZE];          //!< Dispatch table

void      Error(void);
void      Nop(void);
void      ObjectStop(void);
void      ObjectStart(void);
void      ObjectTrig(void);
void      ObjectWait(void);
void      ObjectCall(void);
void      ObjectReturn(void);
void      ObjectEnd(void);
void      ProgramStart(void);
void      ProgramStop(void);
void      Sleep(void);
void      ProgramInfo(void);
void      DefLabel(void);
void      Do(void);
void      Probe(void);
void      BreakPoint(void);
void      BreakSet(void);
void      Random(void);
void      Info(void);
void      Strings(void);
void      MemoryWrite(void);
void      MemoryRead(void);
void      cBranchJr(void);
void      PortCnvOutput(void);
void      PortCnvInput(void);
void      NoteToFreq(void);
void      System(void);
void      Monitor(void);

void      TstClose(void);
void      Tst(void);


//*****************************************************************************
// Interface for shared libraries
//*****************************************************************************

/*! \brief    Get calling object id
 *
 *  \return   OBJID Current object id
 *
 */
OBJID         CallingObjectId(void)
{
  return (VMInstance.ObjectId);
}


/*! \brief    Get current program id
 *
 *  \return   PRGID Current program id
 *
 */
PRGID     CurrentProgramId(void)
{
  return (VMInstance.ProgramId);
}


/*! \brief    Get program status
 *
 *  \return   OBJSTAT Program status
 *
 */
OBJSTAT   ProgramStatus(PRGID PrgId)
{
  return (VMInstance.Program[PrgId].Status);
}


/*! \brief    Get program status change
 *
 *  \return   OBJSTAT Program status change
 *
 */
OBJSTAT   ProgramStatusChange(PRGID PrgId)
{
  OBJSTAT Status;

  Status  =  (DATA8)VMInstance.Program[PrgId].StatusChange;
  VMInstance.Program[PrgId].StatusChange  =  0;

  return (Status);
}


/*! \brief    Get pointer to actual start of byte code image
 *
 *  \return   IP Pointer to image
 *
 */
IP        GetImageStart(void)
{
  return (VMInstance.pImage);
}


/*! \brief    Set object (dispatch) status
 *
 *  \param    DspStat New dispatch status
 *
 */
void      SetDispatchStatus(DSPSTAT DspStat)
{
  VMInstance.DispatchStatus  =  DspStat;

  if (VMInstance.DispatchStatus != NOBREAK)
  {
    VMInstance.Priority  =  0;
  }

}


/*! \brief    Set instructions
 *
 *  \param    ULONG Instructions
 *
 */
void      SetInstructions(ULONG Instructions)
{
  if (Instructions <= PRG_PRIORITY)
  {
    VMInstance.Priority  =  Instructions;
  }
}


/*! \brief    Adjust current instruction pointer
 *
 *  \param    Value Signed offset to add
 *
 */
void      AdjustObjectIp(IMOFFS Value)
{
  VMInstance.ObjectIp += Value;
}


/*! \brief    Get current instruction pointer
 *
 *  \return   Current instruction pointer
 *
 */
IP        GetObjectIp(void)
{
  return (VMInstance.ObjectIp);
}


/*! \brief    Set current instruction pointer
 *
 *  \param    Ip New instruction pointer value
 *
 */
void      SetObjectIp(IP Ip)
{
  VMInstance.ObjectIp  =  Ip;
}


ULONG     GetTime(void)
{
  return (cTimerGetuS() - VMInstance.Program[VMInstance.ProgramId].RunTime);
}


ULONG     GetTimeMS(void)
{
  return (cTimerGetmS());
}


ULONG     GetTimeUS(void)
{
  return (cTimerGetuS());
}


ULONG     CurrentObjectIp(void)
{
  return ((ULONG)(VMInstance.ObjectIp - VMInstance.pImage));
}


void      VmPrint(char *pString)
{
  if (VMInstance.TerminalEnabled)
  {
    printf("%s",pString);
  }
}


void      SetTerminalEnable(DATA8 Value)
{
  VMInstance.TerminalEnabled  =  Value;
}


DATA8     GetTerminalEnable(void)
{
  return (VMInstance.TerminalEnabled);
}


void      GetResourcePath(char *pString,DATA8 MaxLength)
{
  cMemoryGetResourcePath(VMInstance.ProgramId,pString,MaxLength);
}


void*     VmMemoryResize(HANDLER Handle,DATA32 Elements)
{
  return (cMemoryResize(VMInstance.ProgramId,Handle,Elements));
}


void      SetVolumePercent(DATA8 Volume)
{
  VMInstance.NonVol.VolumePercent  =  Volume;
}


DATA8     GetVolumePercent(void)
{
  return(VMInstance.NonVol.VolumePercent);
}


void      SetSleepMinutes(DATA8 Minutes)
{
  VMInstance.NonVol.SleepMinutes  =  Minutes;
}


DATA8     GetSleepMinutes(void)
{
  return (VMInstance.NonVol.SleepMinutes);
}


void      SetUiUpdate(void)
{
  UiInstance.UiUpdate   =  1;
}


#define   ERROR(ErrorCode)    [ErrorCode]  =  #ErrorCode

char      ErrString[ERRORS][ERR_STRING_SIZE] =
{

  ERROR(TOO_MANY_ERRORS_TO_BUFFER),
  ERROR(TYPEDATA_TABEL_FULL),
  ERROR(TYPEDATA_FILE_NOT_FOUND),
  ERROR(ANALOG_DEVICE_FILE_NOT_FOUND),
  ERROR(ANALOG_SHARED_MEMORY),
  ERROR(UART_DEVICE_FILE_NOT_FOUND),
  ERROR(UART_SHARED_MEMORY),
  ERROR(IIC_DEVICE_FILE_NOT_FOUND),
  ERROR(IIC_SHARED_MEMORY),
  ERROR(DISPLAY_SHARED_MEMORY),
  ERROR(UI_SHARED_MEMORY),
  ERROR(UI_DEVICE_FILE_NOT_FOUND),
  ERROR(LCD_DEVICE_FILE_NOT_FOUND),
  ERROR(OUTPUT_SHARED_MEMORY),
  ERROR(COM_COULD_NOT_OPEN_FILE),
  ERROR(COM_NAME_TOO_SHORT),
  ERROR(COM_NAME_TOO_LONG),
  ERROR(COM_INTERNAL),
  ERROR(VM_INTERNAL),
  ERROR(VM_PROGRAM_VALIDATION),
  ERROR(VM_PROGRAM_NOT_STARTED),
  ERROR(VM_PROGRAM_FAIL_BREAK),
  ERROR(VM_PROGRAM_INSTRUCTION_BREAK),
  ERROR(VM_PROGRAM_NOT_FOUND),
  ERROR(SOUND_DEVICE_FILE_NOT_FOUND),
  ERROR(SOUND_SHARED_MEMORY),
  ERROR(FILE_OPEN_ERROR),
  ERROR(FILE_READ_ERROR),
  ERROR(FILE_WRITE_ERROR),
  ERROR(FILE_CLOSE_ERROR),
  ERROR(FILE_GET_HANDLE_ERROR),
  ERROR(FILE_NAME_ERROR),
  ERROR(USB_SHARED_MEMORY),
  ERROR(OUT_OF_MEMORY),

};


void      LogErrorNumber(ERR Err)
{
  UBYTE   Tmp;

  if (Err > TOO_MANY_ERRORS_TO_BUFFER)
  {
    Tmp  =  VMInstance.ErrorIn;
    if (++Tmp >= ERROR_BUFFER_SIZE)
    {
      Tmp  =  0;
    }
    if (Tmp != VMInstance.ErrorOut)
    {
      VMInstance.Errors[VMInstance.ErrorIn]   =  Err;
      VMInstance.ErrorIn                      =  Tmp;
    }
    else
    {
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"\r\n#### %s ####\r\n\n",ErrString[TOO_MANY_ERRORS_TO_BUFFER]);
      VmPrint(VMInstance.PrintBuffer);
    }
  }
}


DATA8     LogErrorNumberExists(ERR Error)
{
  DATA8   Result = 0;
  UBYTE   Tmp;

  Tmp  =  VMInstance.ErrorOut;
  while ((Tmp != VMInstance.ErrorIn) && (Result == 0))
  {
    if (VMInstance.Errors[Tmp] == Error)
    {
      Result  =  1;
    }
    else
    {
      if (++Tmp >= ERROR_BUFFER_SIZE)
      {
        Tmp  =  0;
      }
    }
  }

  return (Result);
}


ERR       LogErrorGet(void)
{
  ERR     Number =  0;

  if (VMInstance.ErrorIn != VMInstance.ErrorOut)
  {
    Number  =  VMInstance.Errors[VMInstance.ErrorOut];
    if (++VMInstance.ErrorOut >= ERROR_BUFFER_SIZE)
    {
      VMInstance.ErrorOut  =  0;
    }
  }

  return (Number);
}


void      CleanLogErrors(void)
{
  VMInstance.ErrorIn    =  0;
  VMInstance.ErrorOut   =  0;
}


/*! \brief    Execute byte code stream (C-call)
 *
 *  This call is able to execute up to "C_PRIORITY" byte codes instructions (no header necessary)
 *
 *  \param  (IP)      Pointer to byte code stream (last byte code must be "opOBJECT_END")
 *  \param  (GP)      Pointer to global variables to use  (if none -> NULL)
 *  \param  (LP)      Pointer to Local variables to use   (if none -> NULL)
 *  \return (DSPSTAT) Dispatch status
 *
 *  Example:
 *
 *  IMGDATA BC1[] = { opUI_WRITE,PUT_STRING,LCS,'L','E','G','O','\r','\n',0,opUI_WRITE,WRITE_FLUSH,opOBJECT_END };
 *  ExecuteByteCode(BC1,NULL,NULL);
 *
 *
 */
DSPSTAT   ExecuteByteCode(IP pByteCode,GP pGlobals,LP pLocals)
{
  DSPSTAT Result;
  ULONG   Time;

  // Save running object parameters
  VMInstance.ObjIpSave            =  VMInstance.ObjectIp;
  VMInstance.ObjGlobalSave        =  VMInstance.pGlobal;
  VMInstance.ObjLocalSave         =  VMInstance.ObjectLocal;
  VMInstance.DispatchStatusSave   =  VMInstance.DispatchStatus;
  VMInstance.PrioritySave         =  VMInstance.Priority;

  // InitExecute special byte code stream
  VMInstance.ObjectIp             =  pByteCode;
  VMInstance.pGlobal              =  pGlobals;
  VMInstance.ObjectLocal          =  pLocals;
  VMInstance.Priority             =  1;

  // Execute special byte code stream
  UiInstance.ButtonState[IDX_BACK_BUTTON] &= ~BUTTON_LONGPRESS;
  while ((*VMInstance.ObjectIp != opOBJECT_END) && (!(UiInstance.ButtonState[IDX_BACK_BUTTON] & BUTTON_LONGPRESS)))
  {
    VMInstance.DispatchStatus       =  NOBREAK;
    VMInstance.Priority             =  C_PRIORITY;

    while ((VMInstance.Priority) && (*VMInstance.ObjectIp != opOBJECT_END))
    {
      VMInstance.Priority--;
      PrimDispatchTabel[*(VMInstance.ObjectIp++)]();
    }

    VMInstance.NewTime  =  GetTimeMS();

    Time  =  VMInstance.NewTime - VMInstance.OldTime1;

    if (Time >= UPDATE_TIME1)
    {
      VMInstance.OldTime1 +=  Time;

      cComUpdate();
      cSoundUpdate();
    }

    Time  =  VMInstance.NewTime - VMInstance.OldTime2;

    if (Time >= UPDATE_TIME2)
    {
      VMInstance.OldTime2 +=  Time;

      usleep(10);
      cInputUpdate((UWORD)Time);
      cUiUpdate((UWORD)Time);
    }
  }
  Result                          =  VMInstance.DispatchStatus;

  UiInstance.ButtonState[IDX_BACK_BUTTON] &= ~BUTTON_LONGPRESS;

  // Restore running object parameters
  VMInstance.Priority             =  VMInstance.PrioritySave;
  VMInstance.DispatchStatus       =  VMInstance.DispatchStatusSave;
  VMInstance.ObjectLocal          =  VMInstance.ObjLocalSave;
  VMInstance.pGlobal              =  VMInstance.ObjGlobalSave;
  VMInstance.ObjectIp             =  VMInstance.ObjIpSave;

  return (Result);
}


/*! \page howtobytecodes How To Pass Parameters
 *
 *  Examples of parsing parameters to byte codes :
 *
 * \verbatim


1.  Move 8 bit signed value to floating point variable


    opMOVE8_F (SOURCE, DESTINATION)

    Parameters:
        (DATA8) SOURCE              (could be constant or variable number)

    Returns:
        (DATAF) DESTINATINATION     (this must be a variable number that has 4 bytes allocated and is aligned at a 4 byte boundary)






2.  Pass two parameters to a SUBCALL

    Parameters must be aligned so 32 bit variables come first then 16 bit and at last 8 bit variables

    Parameters are placed in the SUBCALLs local variable space so the first parameter will be LV0(0)

    When referring to parameters in a SUBCALL refer to local variables.

    Array handles must be seen as input (or input/output) parameters because it is a handle (number)
    to be copied into the subcalls local parameters.
   \endverbatim
-   \ref subparexample1

 *
 */


/*! \page parameterencoding Parameter Encoding
 *
 *  Parameter types and values for primitives, system calls and subroutine calls are encoded in the callers byte code stream as follows:
 *
 *  opADD8 (ParCode1, ParCode2, ParCodeN)
 * \verbatim
Bits  76543210
      --------
      0Ttxxxxx    short format
      ||||||||
      |0||||||    constant
      ||||||||
      ||0vvvvv    positive value
      ||1vvvvv    negative value
      |||
      |1|         variable
      | |
      | 0iiiii    local index
      | 1iiiii    global index
      |
      1ttt-bbb    long format
       ||| |||
       0|| |||    constant
       ||| |||
       |0| |||    value
       |1| |||    label
       ||| |||
       1|| |||    variable
        || |||
        0| |||    local
        1| |||    global
         | |||
         0 |||    value
         1 |||    handle
           |||
           000    Zero terminated string  (subject to change)
           001    1 bytes to follow       (subject to change)
           010    2 bytes to follow       (subject to change)
           011    4 bytes to follow       (subject to change)
           100    Zero terminated string  \endverbatim
 *
 */


/*! \brief    Get next encoded parameter from byte code stream
 *
 *  \return   void Pointer to value
 *
 *
 */
void*     PrimParPointer(void)
{
  void*   Result;
  IMGDATA Data;

  Result              =  (void*)&VMInstance.Value;
  Data                = *((IMGDATA*)VMInstance.ObjectIp++);
  VMInstance.Handle   =  -1;

  if (Data & PRIMPAR_LONG)
  { // long format

    if (Data & PRIMPAR_VARIABEL)
    { // variabel

      switch(Data & PRIMPAR_BYTES)
      {

        case PRIMPAR_1_BYTE :
        { // One byte to follow

          VMInstance.Value    =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);
        }
        break;

        case PRIMPAR_2_BYTES :
        { // Two bytes to follow

          VMInstance.Value    =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);
          VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 8);
        }
        break;

        case PRIMPAR_4_BYTES :
        { // Four bytes to follow

          VMInstance.Value    =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);
          VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 8);
          VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 16);
          VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 24);
        }

      }
      if (Data & PRIMPAR_GLOBAL)
      { // global

        Result  =  (void*)(&VMInstance.pGlobal[VMInstance.Value]);
      }
      else
      { // local

        Result  =  (void*)(&VMInstance.ObjectLocal[VMInstance.Value]);
      }
    }
    else
    { // constant

      if (Data & PRIMPAR_LABEL)
      { // label

        VMInstance.Value      =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);

        if ((VMInstance.Value > 0) && (VMInstance.Value < MAX_LABELS))
        {
          VMInstance.Value    =  (ULONG)VMInstance.Program[VMInstance.ProgramId].Label[VMInstance.Value].Addr;
          VMInstance.Value   -=  ((ULONG)VMInstance.ObjectIp - (ULONG)VMInstance.Program[VMInstance.ProgramId].pImage);
          Result              =  (void*)&VMInstance.Value;
        }
      }
      else
      { // value

        switch(Data & PRIMPAR_BYTES)
        {
          case PRIMPAR_STRING_OLD :
          case PRIMPAR_STRING :
          { // Zero terminated

            Result   =  (DATA8*)VMInstance.ObjectIp;
            while (*((IMGDATA*)VMInstance.ObjectIp++))
            { // Adjust Ip
            }
          }
          break;

          case PRIMPAR_1_BYTE :
          { // One byte to follow

            VMInstance.Value    =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);
            if (VMInstance.Value & 0x00000080)
            { // Adjust if negative

              VMInstance.Value |=  0xFFFFFF00;
            }
          }
          break;

          case PRIMPAR_2_BYTES :
          { // Two bytes to follow

            VMInstance.Value    =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);
            VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 8);
            if (VMInstance.Value & 0x00008000)
            { // Adjust if negative

              VMInstance.Value |=  0xFFFF0000;
            }
          }
          break;

          case PRIMPAR_4_BYTES :
          { // Four bytes to follow

            VMInstance.Value    =  (ULONG)*((IMGDATA*)VMInstance.ObjectIp++);
            VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 8);
            VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 16);
            VMInstance.Value   |=  ((ULONG)*((IMGDATA*)VMInstance.ObjectIp++)  << 24);
          }

        }
      }
    }
    if (Data & PRIMPAR_HANDLE)
    {
      VMInstance.Handle  =  *(HANDLER*)Result;
      cMemoryArraryPointer(VMInstance.ProgramId,VMInstance.Handle,&Result);
    }
    else
    {
      if (Data & PRIMPAR_ADDR)
      {
        Result  =  (void*)*(DATA32*)Result;
        VMInstance.Value  =  (DATA32)Result;
      }
    }
  }
  else
  { // short format

    if (Data & PRIMPAR_VARIABEL)
    { // variabel

      VMInstance.Value  =  (ULONG)(Data & PRIMPAR_INDEX);

      if (Data & PRIMPAR_GLOBAL)
      { // global

        Result  =  (void*)(&VMInstance.pGlobal[VMInstance.Value]);
      }
      else
      { // local

        Result  =  (void*)(&VMInstance.ObjectLocal[VMInstance.Value]);
      }
    }
    else
    { // constant

      VMInstance.Value  =  (ULONG)(Data & PRIMPAR_VALUE);

      if (Data & PRIMPAR_CONST_SIGN)
      { // Adjust if negative

        VMInstance.Value |= ~(ULONG)(PRIMPAR_VALUE);
      }
    }
  }

  return (Result);
}


/*! \brief    Skip next encoded parameter from byte code stream
 *
 *
 */
void      PrimParAdvance(void)
{
  IMGDATA Data;

  Data      = *((IMGDATA*)VMInstance.ObjectIp++);

  if (Data & PRIMPAR_LONG)
  { // long format

    if (Data & PRIMPAR_VARIABEL)
    { // variabel

      switch(Data & PRIMPAR_BYTES)
      {

        case PRIMPAR_1_BYTE :
        { // One byte to follow

          VMInstance.ObjectIp++;
        }
        break;

        case PRIMPAR_2_BYTES :
        { // Two bytes to follow

          VMInstance.ObjectIp++;
          VMInstance.ObjectIp++;
        }
        break;

        case PRIMPAR_4_BYTES :
        { // Four bytes to follow

          VMInstance.ObjectIp++;
          VMInstance.ObjectIp++;
          VMInstance.ObjectIp++;
          VMInstance.ObjectIp++;
        }

      }
    }
    else
    { // constant

      if (Data & PRIMPAR_LABEL)
      { // label

        VMInstance.ObjectIp++;

      }
      else
      { // value

        switch(Data & PRIMPAR_BYTES)
        {
          case PRIMPAR_STRING_OLD :
          case PRIMPAR_STRING :
          { // Zero terminated

            while (*((IMGDATA*)VMInstance.ObjectIp++))
            { // Adjust Ip
            }
          }
          break;

          case PRIMPAR_1_BYTE :
          { // One byte to follow

            VMInstance.ObjectIp++;
          }
          break;

          case PRIMPAR_2_BYTES :
          { // Two bytes to follow

            VMInstance.ObjectIp++;
            VMInstance.ObjectIp++;
          }
          break;

          case PRIMPAR_4_BYTES :
          { // Four bytes to follow

            VMInstance.ObjectIp++;
            VMInstance.ObjectIp++;
            VMInstance.ObjectIp++;
            VMInstance.ObjectIp++;
          }

        }
      }
    }
  }
}


/*! \page parameterencoding
 *
 *  \anchor subpar
 *
 *  For subroutine parameters additional information about no of pars, direction and size are needed and placed in front of the called code\n
 *  Parameters MUST be sorted so: largest (4 bytes) parameters is first and smallest (1 byte) is last in the list.
 *
 *  OffsetToInstructions -> NoOfPars, ParType1, ParType2, ParTypen
 * \verbatim
Bits  76543210
      --------
      io---bbb    long format
      ||   |||
      1x   |||    parameter in
      x1   |||    parameter out
           |||
           000    8  bits
           001    16 bits
           010    32 bits
           011    float
           100    Zero terminated string (next byte tells allocated size) \endverbatim
 *
 */
/*! \brief    Copy encoded parameters to local variables
 *
 *  \param    Id Object to copy to
 *
 *
 */
void      CopyParsToLocals(OBJID Id)
{
  IP      TmpIp;      // Save calling Ip
  IP      TypeIp;     // Called Ip
  void*   pLocals;    // Called locals
  PARS    NoOfPars;   // Called no of parameters
  IMGDATA Type;       // Coded type
  void*   Result;     // Pointer to value
  DATA32  Size;
  DATA8   Flag;

  TmpIp       =  VMInstance.ObjectIp;
  TypeIp      =  VMInstance.Program[VMInstance.ProgramId].pImage;
  TypeIp      =  &TypeIp[(ULONG)VMInstance.pObjHead[Id].OffsetToInstructions];
  pLocals     =  (*VMInstance.pObjList[Id]).pLocal;

  NoOfPars    =  (PARS)*((IMGDATA*)TypeIp++);

  while (NoOfPars--)
  {
    // Get type from sub preamble
    Type      =  (IMGDATA)*((IMGDATA*)TypeIp++);

    // Get pointer to value and increment VMInstance.ObjectIP

    Result    =  PrimParPointer();

    if ((Type & CALLPAR_IN))
    { // Input

      switch (Type & CALLPAR_TYPE)
      {

        case CALLPAR_DATA8 :
        {
          (*(DATA8*)pLocals)   =  *(DATA8*)Result;
          pLocals              =  ((DATA8*)pLocals) + 1;
        }
        break;

        case CALLPAR_DATA16 :
        {
#ifndef DISABLE_PAR_ALIGNMENT
          pLocals  =  (void*)(((IMINDEX)pLocals + 1) & ~1);
#endif
          (*(DATA16*)pLocals)  =  *(DATA16*)Result;
          pLocals              =  ((DATA16*)pLocals) + 1;
        }
        break;

        case CALLPAR_DATA32 :
        {
#ifndef DISABLE_PAR_ALIGNMENT
          pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
          (*(DATA32*)pLocals)  =  *(DATA32*)Result;
          pLocals              =  ((DATA32*)pLocals) + 1;
        }
        break;

        case CALLPAR_DATAF :
        {
#ifndef DISABLE_PAR_ALIGNMENT
          pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
          (*(DATAF*)pLocals)   =  *(DATAF*)Result;
          pLocals              =  ((DATAF*)pLocals) + 1;
        }
        break;

        case CALLPAR_STRING :
        {
          Size                 =  (DATA32)*((IMGDATA*)TypeIp++);
          Flag                 =  1;
          while (Size)
          {
            if (Flag)
            {
              Flag              =  *(DATA8*)Result;
            }
            (*(DATA8*)pLocals)  =  Flag;
            Result              =  ((DATA8*)Result) + 1;
            pLocals             =  ((DATA8*)pLocals) + 1;
            Size--;
          }
          pLocals               =  ((DATA8*)pLocals) - 1;
          (*(DATA8*)pLocals)    =  0;
          pLocals               =  ((DATA8*)pLocals) + 1;
        }
        break;

      }
    }
    else
    {
      if ((Type & CALLPAR_OUT))
      { // Output

        switch (Type & CALLPAR_TYPE)
        {

          case CALLPAR_DATA8 :
          {
            pLocals              =  ((DATA8*)pLocals) + 1;
          }
          break;

          case CALLPAR_DATA16 :
          {
#ifndef DISABLE_PAR_ALIGNMENT
            pLocals  =  (void*)(((IMINDEX)pLocals + 1) & ~1);
#endif
            pLocals              =  ((DATA16*)pLocals) + 1;
          }
          break;

          case CALLPAR_DATA32 :
          {
#ifndef DISABLE_PAR_ALIGNMENT
            pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
            pLocals              =  ((DATA32*)pLocals) + 1;
          }
          break;

          case CALLPAR_DATAF :
          {
#ifndef DISABLE_PAR_ALIGNMENT
            pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
            pLocals              =  ((DATAF*)pLocals) + 1;
          }
          break;

          case CALLPAR_STRING :
          {
            Size                 =  (DATA32)*((IMGDATA*)TypeIp++);
            pLocals              =  ((DATA8*)pLocals) + Size;
          }
          break;

        }
      }
    }
  }
  (*VMInstance.pObjList[Id]).Ip  =  TypeIp;

  // Rewind caller Ip
  VMInstance.ObjectIp  =  TmpIp;
}


/*! \brief    Copy local variables to encoded parameters
 *
 *  \param    Id Object to copy to
 *
 *
 */
void      CopyLocalsToPars(OBJID Id)
{
  IP      TmpIp;      // Calling Ip
  IP      TypeIp;     // Called Ip
  void*   pLocals;    // Called locals
  PARS    NoOfPars;   // Called no of parameters
  IMGDATA Type;       // Coded type
  void*   Result;     // Pointer to value
  DATA32  Size;
  DATA8   Flag;

  // Point to start of parameters
  TmpIp               =  VMInstance.ObjectIp;
  VMInstance.ObjectIp    =  (*VMInstance.pObjList[Id]).Ip;

  // Point to start of sub
  TypeIp      =  VMInstance.Program[VMInstance.ProgramId].pImage;
  TypeIp      =  &TypeIp[(ULONG)VMInstance.pObjHead[VMInstance.ObjectId].OffsetToInstructions];
  pLocals     =  (*VMInstance.pObjList[VMInstance.ObjectId]).pLocal;

  NoOfPars    =  (PARS)*((IMGDATA*)TypeIp++);

  while (NoOfPars--)
  {
    // Get type from sub preamble
    Type      =  (IMGDATA)*((IMGDATA*)TypeIp++);

    // Get pointer to value and increment VMInstance.ObjectIp
    Result    =  PrimParPointer();

    if ((Type & CALLPAR_OUT))
    { // Output

      switch (Type & CALLPAR_TYPE)
      {

        case CALLPAR_DATA8 :
        {
          *(DATA8*)Result      =  (*(DATA8*)pLocals);
          pLocals              =  ((DATA8*)pLocals) + 1;
        }
        break;

        case CALLPAR_DATA16 :
        {
#ifndef DISABLE_PAR_ALIGNMENT
          pLocals  =  (void*)(((IMINDEX)pLocals + 1) & ~1);
#endif
          *(DATA16*)Result     =  (*(DATA16*)pLocals);
          pLocals              =  ((DATA16*)pLocals) + 1;
        }
        break;

        case CALLPAR_DATA32 :
        {
#ifndef DISABLE_PAR_ALIGNMENT
          pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
          *(DATA32*)Result     =  (*(DATA32*)pLocals);
          pLocals              =  ((DATA32*)pLocals) + 1;
        }
        break;

        case CALLPAR_DATAF :
        {
#ifndef DISABLE_PAR_ALIGNMENT
          pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
          *(DATAF*)Result      =  (*(DATAF*)pLocals);
          pLocals              =  ((DATAF*)pLocals) + 1;
        }
        break;

        case CALLPAR_STRING :
        {
          Size                 =  (DATA32)*((IMGDATA*)TypeIp++);
          Flag                 =  1;
          while (Size)
          {
            if (Flag)
            {
              Flag              =  (*(DATA8*)pLocals);
            }
            *(DATA8*)Result     =  Flag;
            Result              =  ((DATA8*)Result) + 1;
            pLocals             =  ((DATA8*)pLocals) + 1;
            Size--;
          }
          Result                =  ((DATA8*)Result) - 1;
          *(DATA8*)Result       =  0;
        }
        break;

      }
    }
    else
    {
      if ((Type & CALLPAR_IN))
      { // Input

        switch (Type & CALLPAR_TYPE)
        {

          case CALLPAR_DATA8 :
          {
            pLocals              =  ((DATA8*)pLocals) + 1;
          }
          break;

          case CALLPAR_DATA16 :
          {
#ifndef DISABLE_PAR_ALIGNMENT
            pLocals  =  (void*)(((IMINDEX)pLocals + 1) & ~1);
#endif
            pLocals              =  ((DATA16*)pLocals) + 1;
          }
          break;

          case CALLPAR_DATA32 :
          {
#ifndef DISABLE_PAR_ALIGNMENT
            pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
            pLocals              =  ((DATA32*)pLocals) + 1;
          }
          break;

          case CALLPAR_DATAF :
          {
#ifndef DISABLE_PAR_ALIGNMENT
            pLocals  =  (void*)(((IMINDEX)pLocals + 3) & ~3);
#endif
            pLocals              =  ((DATAF*)pLocals) + 1;
          }
          break;

          case CALLPAR_STRING :
          {
            Size                 =  (DATA32)*((IMGDATA*)TypeIp++);
            pLocals              =  ((DATA8*)pLocals) + Size;
          }
          break;

        }
      }
    }
  }

  // Adjust caller Ip
  (*VMInstance.pObjList[Id]).Ip       =  VMInstance.ObjectIp;
  // Restore calling Ip
  VMInstance.ObjectIp                 =  TmpIp;
}


//*****************************************************************************
// VM routines
//*****************************************************************************

/*! \brief    Initialise object instruction pointer and trigger counter
 *
 *  \param    ObjId Object to reset
 *
 */
void      ObjectReset(OBJID ObjId)
{
  (*VMInstance.pObjList[ObjId]).Ip              =  &VMInstance.pImage[(ULONG)VMInstance.pObjHead[ObjId].OffsetToInstructions];

  (*VMInstance.pObjList[ObjId]).u.TriggerCount  =  VMInstance.pObjHead[ObjId].TriggerCount;
}


/*! \brief    Get amount of ram to allocate for program
 *
 *  \param    pI Pointer to image
 *
 */
GBINDEX   GetAmountOfRamForImage(IP pI)
{
  GBINDEX Bytes = 0;
  OBJID   NoOfObj;
  OBJID   ObjId;
  OBJHEAD *pHead;

  NoOfObj    =  (*(IMGHEAD*)pI).NumberOfObjects;

  Bytes     +=  (*(IMGHEAD*)pI).GlobalBytes;
  Bytes      =  (Bytes + 3)  & 0xFFFFFFFC;
  Bytes     +=  sizeof(VMInstance.pObjList) * (NoOfObj + 1);

  pHead      =  (OBJHEAD*)&pI[sizeof(IMGHEAD)];

  for (ObjId = 1;ObjId <= NoOfObj;ObjId++)
  {
    Bytes    =  (Bytes + 3)  & 0xFFFFFFFC;
    Bytes   +=  sizeof(OBJ) + (*pHead).LocalBytes;
    pHead++;
  }

  Bytes      =  0;

  NoOfObj    =  (*(IMGHEAD*)pI).NumberOfObjects;

  Bytes     +=  (*(IMGHEAD*)pI).GlobalBytes;
  Bytes      =  (Bytes + 3)  & 0xFFFFFFFC;
  Bytes     +=  sizeof(VMInstance.pObjList) * (NoOfObj + 1);

  pHead      =  (OBJHEAD*)&pI[sizeof(IMGHEAD) - sizeof(OBJHEAD)];

  for (ObjId = 1;ObjId <= NoOfObj;ObjId++)
  {
    Bytes    =  (Bytes + 3)  & 0xFFFFFFFC;
    Bytes   +=  sizeof(OBJ) + pHead[ObjId].LocalBytes;
  }

  return (Bytes);
}


/*! \brief    Initialise program for execution
 *
 *  \param    PrgId Program id (index)
 *  \param    pI    Pointer to image
 *  \param    pG    Pointer to global variables
 *  \param    Deb   debug flag
 *
 *
 */
RESULT    ProgramReset(PRGID PrgId,IP pI,GP pG,UBYTE Deb)
{

  RESULT  Result = FAIL;
  GBINDEX Index;
  GBINDEX RamSize;
  VARDATA *pData;
  OBJID   ObjIndex;
  DATA8   No;

  VMInstance.Program[PrgId].Status          =  STOPPED;
  VMInstance.Program[PrgId].StatusChange    =  STOPPED;
  VMInstance.Program[PrgId].Result          =  FAIL;

  if (pI != NULL)
  {

    // Allocate memory for globals and objects

    RamSize       =  GetAmountOfRamForImage(pI);

    if (cMemoryOpen(PrgId,RamSize,(void**)&pData) == OK)
    { // Memory reserved

      // Save start of image
      if (Deb == 1)
      {
        VMInstance.Program[PrgId].Debug        =  1;
      }
      else
      {
        VMInstance.Program[PrgId].Debug        =  0;
      }
      VMInstance.Program[PrgId].pImage       =  pI;

      if (cValidateProgram(PrgId,pI,VMInstance.Program[PrgId].Label,VMInstance.TerminalEnabled) != OK)
      {
        if (PrgId != CMD_SLOT)
        {
          LogErrorNumber(VM_PROGRAM_VALIDATION);
        }
      }
      else
      {

        // Clear memory

        for (Index = 0;Index < RamSize;Index++)
        {
          pData[Index]  =  0;
        }

        for (No = 0;No < MAX_BREAKPOINTS;No++)
        {
          VMInstance.Program[PrgId].Brkp[No].Addr    =  0;
          VMInstance.Program[PrgId].Brkp[No].OpCode  =  0;
        }

        // Get VMInstance.Objects

        VMInstance.Program[PrgId].Objects      =  (*(IMGHEAD*)pI).NumberOfObjects;

        // Allocate GlobalVariables

        VMInstance.Program[PrgId].pData        =  pData;
        if (pG != NULL)
        {
          VMInstance.Program[PrgId].pGlobal    =  pG;
        }
        else
        {
          VMInstance.Program[PrgId].pGlobal    =  pData;
        }

        pData                       =  &pData[(*(IMGHEAD*)pI).GlobalBytes];

        // Align & allocate ObjectPointerList (+1)

        pData                       =  (VARDATA*)(((ULONG)pData + 3) & 0xFFFFFFFC);
        VMInstance.Program[PrgId].pObjList     =  (OBJ**)pData;
        pData                       =  &pData[sizeof(VMInstance.Program[PrgId].pObjList) * (VMInstance.Program[PrgId].Objects + 1)];

        // Make pointer to access object headers starting at one (not zero)

        VMInstance.Program[PrgId].pObjHead     =  (OBJHEAD*)&pI[sizeof(IMGHEAD) - sizeof(OBJHEAD)];

        for (ObjIndex = 1;ObjIndex <= VMInstance.Program[PrgId].Objects;ObjIndex++)
        {
          // Align

          pData       =  (VARDATA*)(((ULONG)pData + 3) & 0xFFFFFFFC);

          // Save object pointer in Object list

          VMInstance.Program[PrgId].pObjList[ObjIndex]   =  (OBJ*)pData;

          // Initialise instruction pointer, trigger counts and status

          (*VMInstance.Program[PrgId].pObjList[ObjIndex]).Ip              =  &pI[(ULONG)VMInstance.Program[PrgId].pObjHead[ObjIndex].OffsetToInstructions];

          (*VMInstance.Program[PrgId].pObjList[ObjIndex]).u.TriggerCount  =  VMInstance.Program[PrgId].pObjHead[ObjIndex].TriggerCount;

          if (((*VMInstance.Program[PrgId].pObjList[ObjIndex]).u.TriggerCount) || (ObjIndex > 1))
          {
            (*VMInstance.Program[PrgId].pObjList[ObjIndex]).ObjStatus     =  STOPPED;
          }
          else
          {
            if (Deb == 2)
            {
              (*VMInstance.Program[PrgId].pObjList[ObjIndex]).ObjStatus   =  WAITING;
            }
            else
            {
              (*VMInstance.Program[PrgId].pObjList[ObjIndex]).ObjStatus   =  RUNNING;
            }
          }

#ifndef DISABLE_NEW_CALL_MUTEX
          (*VMInstance.Program[PrgId].pObjList[ObjIndex]).Blocked  =  0;
#endif
          if (VMInstance.Program[PrgId].pObjHead[ObjIndex].OwnerObjectId)
          {
            (*VMInstance.Program[PrgId].pObjList[ObjIndex]).pLocal  =  (*VMInstance.Program[PrgId].pObjList[VMInstance.Program[PrgId].pObjHead[ObjIndex].OwnerObjectId]).Local;
          }
          else
          {
            (*VMInstance.Program[PrgId].pObjList[ObjIndex]).pLocal  =  (*VMInstance.Program[PrgId].pObjList[ObjIndex]).Local;
          }

          // Advance data pointer

          pData                =  &pData[sizeof(OBJ) + VMInstance.Program[PrgId].pObjHead[ObjIndex].LocalBytes];
        }

        VMInstance.Program[PrgId].ObjectId        =  1;
        VMInstance.Program[PrgId].Status          =  RUNNING;
        VMInstance.Program[PrgId].StatusChange    =  RUNNING;

        VMInstance.Program[PrgId].Result          =  BUSY;

        Result  =  OK;

        if (PrgId == USER_SLOT)
        {

          if (VMInstance.RefCount == 0)
          {
            Result     =  0;
            Result    |=  cUiOpen();
            Result    |=  cOutputOpen();
            Result    |=  cInputOpen();
            Result    |=  cComOpen();
            Result    |=  cSoundOpen();
          }
          VMInstance.RefCount++;
        }
        VMInstance.Program[PrgId].InstrCnt        =  0;
        VMInstance.Program[PrgId].StartTime       =  GetTimeMS();
        VMInstance.Program[PrgId].RunTime         =  cTimerGetuS();
      }
    }
  }

  return (Result);
}


/*! \brief    Exit program nicely (freeing up memory)
 *
 *  \param    PrgId Program id (index)
 *
 *
 */
void      ProgramEnd(PRGID PrgId)
{
  if (VMInstance.Program[PrgId].Status != STOPPED)
  {

    VMInstance.Program[PrgId].InstrTime       =  cTimerGetuS() - VMInstance.Program[PrgId].RunTime;

    VMInstance.Program[PrgId].Objects         =  0;
    VMInstance.Program[PrgId].Status          = STOPPED;
    VMInstance.Program[PrgId].StatusChange    =  STOPPED;
    if (PrgId != 0)
    {
      if (PrgId != VMInstance.ProgramId)
      {
        VMInstance.Program[PrgId].InstrCnt +=  VMInstance.InstrCnt;
      }
    }

    if (PrgId == USER_SLOT)
    {
      if (VMInstance.RefCount)
      {
        VMInstance.RefCount--;
      }
      if (VMInstance.RefCount == 0)
      {
        cSoundClose();
        cComClose();
        cInputClose();
        cOutputClose();
        cUiClose();
      }
    }

    cMemoryClose(PrgId);

    if (PrgId == VMInstance.ProgramId)
    {
      SetDispatchStatus(PRGBREAK);
    }

    VMInstance.Program[PrgId].Result          =  OK;
  }
}


/*! \brief    Switch in current program
 *            ProgramId holds program to switch to
 *
 */
void      ProgramInit(void)
{
  PRG     *pProgram;

  if (VMInstance.ProgramId < MAX_PROGRAMS)
  {
    pProgram       =  &VMInstance.Program[VMInstance.ProgramId];

    if (((*pProgram).Status == RUNNING) || ((*pProgram).Status == WAITING))
    {
      VMInstance.pGlobal        =  (*pProgram).pGlobal;
      VMInstance.pImage         =  (*pProgram).pImage;
      VMInstance.pObjHead       =  (*pProgram).pObjHead;
      VMInstance.pObjList       =  (*pProgram).pObjList;
      VMInstance.Objects        =  (*pProgram).Objects;
      VMInstance.ObjectId       =  (*pProgram).ObjectId;
      VMInstance.ObjectIp       =  (*pProgram).ObjectIp;
      VMInstance.ObjectLocal    =  (*pProgram).ObjectLocal;
      VMInstance.InstrCnt       =  0;
      VMInstance.Debug          =  (*pProgram).Debug;

    }
  }
}


/*! \brief    Switch out current program
 *            ProgramId holds program to switch from
 *
 */
void      ProgramExit(void)
{
  PRG     *pProgram;

  if (VMInstance.ProgramId < MAX_PROGRAMS)
  {
    pProgram                  =  &VMInstance.Program[VMInstance.ProgramId];
    (*pProgram).pGlobal       =  VMInstance.pGlobal;
    (*pProgram).pImage        =  VMInstance.pImage;
    (*pProgram).pObjHead      =  VMInstance.pObjHead;
    (*pProgram).pObjList      =  VMInstance.pObjList;
    (*pProgram).Objects       =  VMInstance.Objects;
    (*pProgram).ObjectId      =  VMInstance.ObjectId;
    (*pProgram).ObjectIp      =  VMInstance.ObjectIp;
    (*pProgram).ObjectLocal   =  VMInstance.ObjectLocal;
    (*pProgram).InstrCnt     +=  VMInstance.InstrCnt;
    (*pProgram).Debug         =  VMInstance.Debug;

    VMInstance.InstrCnt       =  0;
    (*pProgram).Debug         =  VMInstance.Debug;
  }
}


/*! \brief    Find a program to run
 *            ProgramId holds found program
 *
 *  \return   RESULT Succes [OK or STOP]
 */
RESULT   ProgramExec(void)
{
  RESULT  Result = STOP;
  OBJID   TmpId  = 0;


  for (TmpId = 0;(TmpId < MAX_PROGRAMS) && (Result == STOP);TmpId++)
  {
    if (VMInstance.Program[TmpId].Status != STOPPED)
    {
      Result  =  OK;
    }
  }
  if (Result == OK)
  {
    do
    {
      // next program

      if (++VMInstance.ProgramId >= MAX_PROGRAMS)
      {
        // wrap around

        VMInstance.ProgramId  =  0;
      }

    }
    while (VMInstance.Program[VMInstance.ProgramId].Status == STOPPED);
  }


  return (Result);
}


/*! \brief    Restore object context
 *
 *            Restore object context if id is valid and running by loading current IP and current pointer to Locals
 *
 *            Uses following from current program context:
 *            ObjectId, Objects, pObjList, pObjHead, ObjectIp, ObjectLocal
 *
 *  \return   DSPSTAT New value for dispatch status [NOBREAK, STOPBREAK]
 *
 *
 */
DSPSTAT   ObjectInit(void)
{
  DSPSTAT Result = STOPBREAK;

  if ((VMInstance.ObjectId > 0) && (VMInstance.ObjectId <= VMInstance.Objects))
  { // object valid

    if ((*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus == RUNNING)
    { // Restore object context

      VMInstance.ObjectIp        =  (*VMInstance.pObjList[VMInstance.ObjectId]).Ip;

      VMInstance.ObjectLocal     =  (*VMInstance.pObjList[VMInstance.ObjectId]).pLocal;

      Result  =  NOBREAK;
    }
  }

  if((VMInstance.ProgramId == GUI_SLOT) || (VMInstance.ProgramId == DEBUG_SLOT))
  { // UI

    VMInstance.Priority  =  UI_PRIORITY;
  }
  else
  { // user program

    VMInstance.Priority  =  PRG_PRIORITY;
  }

  return (Result);
}


/*! \brief    Save object context
 *
 *            ObjectId holds object to switch from
 *
 *            Save object context if id is valid and running by saving current IP
 *
 *            Uses following from current program context:
 *            ObjectId, Objects, pObjList, ObjectIp
 *
 */
void      ObjectExit(void)
{
  if ((VMInstance.ObjectId > 0) && (VMInstance.ObjectId <= VMInstance.Objects) && (VMInstance.Program[VMInstance.ProgramId].Status != STOPPED))
  { // object valid

    if ((*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus == RUNNING)
    { // Save object context

      (*VMInstance.pObjList[VMInstance.ObjectId]).Ip      =  VMInstance.ObjectIp;
    }
  }
}


/*! \brief    Find next object to run
 *
 *            Uses following from current program context:
 *            ObjectId, Objects, pObjList
 *
 *  \return   RESULT  Succes [OK or STOP]
 */
RESULT    ObjectExec(void)
{
  RESULT  Result = OK;
  OBJID   TmpId  = 0;


  if ((VMInstance.ProgramId == GUI_SLOT) && (VMInstance.Program[USER_SLOT].Status != STOPPED))
  { // When user program is running - only schedule UI background task

    if ((VMInstance.Objects>= 3) && (VMInstance.Program[GUI_SLOT].Status != STOPPED))
    {
      if (VMInstance.ObjectId != 2)
      {
        VMInstance.ObjectId  =  2;
      }
      else
      {
        VMInstance.ObjectId  =  3;
      }
    }
  }
  else
  {
    do
    {
      // Next object
      if (++VMInstance.ObjectId > VMInstance.Objects)
      {
        // wrap around

        VMInstance.ObjectId  =  1;
      }

      if (++TmpId > VMInstance.Objects)
      {
        // no programs running

        Result  =  STOP;
      }

    }
    while ((Result == OK) && ((*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus != RUNNING));
  }

  return (Result);
}


/*! \brief    Put object on run queue
 *
 *  \param    Id Object to queue
 *
 */
void      ObjectEnQueue(OBJID Id)
{
  if ((Id > 0) && (Id <= VMInstance.Objects))
  {
    (*VMInstance.pObjList[Id]).ObjStatus        =  RUNNING;
    (*VMInstance.pObjList[Id]).Ip               = &VMInstance.pImage[(ULONG)VMInstance.pObjHead[Id].OffsetToInstructions];
    (*VMInstance.pObjList[Id]).u.TriggerCount   =  VMInstance.pObjHead[Id].TriggerCount;
  }
}


/*! \brief    Remove object from run queue
 *
 *  \param    Id Object to enqueue
 *
 */
void      ObjectDeQueue(OBJID Id)
{
  if ((Id > 0) && (Id <= VMInstance.Objects))
  {
    (*VMInstance.pObjList[Id]).Ip         =  VMInstance.ObjectIp;
    (*VMInstance.pObjList[Id]).ObjStatus  =  STOPPED;

    SetDispatchStatus(STOPBREAK);
  }
}


DATA8     CheckSdcard(DATA8 *pChanged,DATA32 *pTotal,DATA32 *pFree,DATA8 Force)
{
  DATA8   Result = 0;
  DATAF   Tmp;
  char    Name[vmNAMESIZE];
  char    Buffer[250];

  *pChanged   =  0;
  *pTotal     =  0;
  *pFree      =  0;

#ifndef DISABLE_SDCARD_SUPPORT
  ULONG   Time;
  struct  statvfs Status;

  Time  =  VMInstance.NewTime - VMInstance.SdcardTimer;

  if ((Time >= UPDATE_SDCARD) || (VMInstance.SdcardTimer == 0) || (Force))
  { // Update values

    VMInstance.SdcardTimer +=  Time;

    if (cMemoryGetMediaName("m",Name) == OK)
    {
      if (VMInstance.SdcardOk == 0)
      {
        VMInstance.SdcardOk  =  1;
        snprintf(Buffer,250,"ln -s /media/card %s &> /dev/null",vmSDCARD_FOLDER);
        system(Buffer);
        sync();
        *pChanged  =  1;
#ifdef DEBUG_SDCARD
        printf("system(%s)\r\n",Buffer);
#endif
      }
      snprintf(Buffer,250,"/media/card");
      statvfs(Buffer,&Status);
#ifdef DEBUG_SDCARD
      if (*pChanged)
      {
        printf("statvfs(%s)\r\n",Buffer);

        printf("f_bsize   %ld\r\n",Status.f_bsize);
        printf("f_frsize  %ld\r\n",Status.f_frsize);
        printf("f_blocks  %ld\r\n",Status.f_blocks);
        printf("f_bavail  %ld\r\n",Status.f_bavail);
        printf("f_files   %ld\r\n",Status.f_files);
        printf("f_ffree   %ld\r\n",Status.f_ffree);
        printf("f_favail  %ld\r\n",Status.f_favail);
        printf("f_fside   %ld\r\n",Status.f_fsid);
        printf("f_flag    %ld\r\n",Status.f_flag);
        printf("f_namemax %ld\r\n",Status.f_namemax);
      }
#endif
      Tmp  =  (DATAF)Status.f_blocks;
      Tmp  =  (Tmp + (DATAF)(KB - 1)) / (DATAF)KB;
      Tmp *=  (DATAF)Status.f_bsize;
      VMInstance.SdcardSize  =  (DATA32)Tmp;
      Tmp  =  (DATAF)Status.f_bavail;
      Tmp  =  (Tmp + (DATAF)(KB - 1)) / (DATAF)KB;
      Tmp *=  (DATAF)Status.f_bsize;

#ifdef DEBUG_SDCARD
      if(VMInstance.SdcardFree != (DATA32)Tmp)
      {
        printf("%d T=%-8d F=%-8d\r\n",VMInstance.SdcardOk,VMInstance.SdcardSize,VMInstance.SdcardFree);
      }
#endif
      VMInstance.SdcardFree  =  (DATA32)Tmp;
    }
    else
    {
      if (VMInstance.SdcardOk == 1)
      {
        VMInstance.SdcardOk  =  0;
        snprintf(Buffer,250,"rm -r %s &> /dev/null",vmSDCARD_FOLDER);
        system(Buffer);
        *pChanged  =  1;
#ifdef DEBUG_SDCARD
        if (*pChanged)
        {
          printf("system(%s)\r\n",Buffer);
        }
#endif
      }
      VMInstance.SdcardSize  =  0;
      VMInstance.SdcardFree  =  0;
    }
#ifdef DEBUG_SDCARD
    if (*pChanged)
    {
      printf("%d T=%-8d F=%-8d\r\n",VMInstance.SdcardOk,VMInstance.SdcardSize,VMInstance.SdcardFree);
    }
#endif
  }
  *pTotal   =  VMInstance.SdcardSize;
  *pFree    =  VMInstance.SdcardFree;
  Result    =  VMInstance.SdcardOk;
#endif

  return (Result);
}


DATA8     CheckUsbstick(DATA8 *pChanged,DATA32 *pTotal,DATA32 *pFree,DATA8 Force)
{
  DATA8   Result = 0;

  *pChanged   =  0;
  *pTotal     =  0;
  *pFree      =  0;

#ifndef DISABLE_USBSTICK_SUPPORT
  ULONG   Time;
  struct  statvfs Status;
  DATAF   Tmp;
  char    Name[vmNAMESIZE];
  char    Buffer[250];

  Time  =  VMInstance.NewTime - VMInstance.UsbstickTimer;

  if ((Time >= UPDATE_USBSTICK) || (VMInstance.UsbstickTimer == 0) || (Force))
  { // Update values

    VMInstance.UsbstickTimer +=  Time;

    if (cMemoryGetMediaName("s",Name) == OK)
    {
      if (VMInstance.UsbstickOk == 0)
      {
        VMInstance.UsbstickOk  =  1;
        snprintf(Buffer,250,"ln -s /media/usb %s &> /dev/null",vmUSBSTICK_FOLDER);
        system(Buffer);
        sync();
        *pChanged  =  1;
#ifdef DEBUG_USBSTICK
        printf("system(%s)\r\n",Buffer);
#endif
      }
      snprintf(Buffer,250,"/media/usb");
      statvfs(Buffer,&Status);
#ifdef DEBUG_USBSTICK
      if (*pChanged)
      {
        printf("statvfs(%s)\r\n",Buffer);

        printf("f_bsize   %ld\r\n",Status.f_bsize);
        printf("f_frsize  %ld\r\n",Status.f_frsize);
        printf("f_blocks  %ld\r\n",Status.f_blocks);
        printf("f_bavail  %ld\r\n",Status.f_bavail);
        printf("f_files   %ld\r\n",Status.f_files);
        printf("f_ffree   %ld\r\n",Status.f_ffree);
        printf("f_favail  %ld\r\n",Status.f_favail);
        printf("f_fside   %ld\r\n",Status.f_fsid);
        printf("f_flag    %ld\r\n",Status.f_flag);
        printf("f_namemax %ld\r\n",Status.f_namemax);
      }
#endif
      Tmp  =  (DATAF)Status.f_blocks;
      Tmp  =  (Tmp + (DATAF)(KB - 1)) / (DATAF)KB;
      Tmp *=  (DATAF)Status.f_bsize;
      VMInstance.UsbstickSize  =  (DATA32)Tmp;
      Tmp  =  (DATAF)Status.f_bavail;
      Tmp  =  (Tmp + (DATAF)(KB - 1)) / (DATAF)KB;
      Tmp *=  (DATAF)Status.f_bsize;

#ifdef DEBUG_USBSTICK
      if(VMInstance.UsbstickFree != (DATA32)Tmp)
      {
        printf("%d T=%-8d F=%-8d\r\n",VMInstance.UsbstickOk,VMInstance.UsbstickSize,VMInstance.UsbstickFree);
      }
#endif
      VMInstance.UsbstickFree  =  (DATA32)Tmp;
    }
    else
    {
      if (VMInstance.UsbstickOk == 1)
      {
        VMInstance.UsbstickOk  =  0;
        snprintf(Buffer,250,"rm -r %s &> /dev/null",vmUSBSTICK_FOLDER);
        system(Buffer);
        *pChanged  =  1;
#ifdef DEBUG_USBSTICK
        if (*pChanged)
        {
          printf("system(%s)\r\n",Buffer);
        }
#endif
      }
      VMInstance.UsbstickSize  =  0;
      VMInstance.UsbstickFree  =  0;
    }
#ifdef DEBUG_USBSTICK
//    if (*pChanged)
    {
      printf("%d T=%-8d F=%-8d\r\n",VMInstance.UsbstickOk,VMInstance.UsbstickSize,VMInstance.UsbstickFree);
    }
#endif
  }
  *pTotal   =  VMInstance.UsbstickSize;
  *pFree    =  VMInstance.UsbstickFree;
  Result    =  VMInstance.UsbstickOk;
#endif

  return (Result);
}


RESULT    mSchedInit(int argc,char *argv[])
{
  DATA32  Result = OK;
  PRGID   PrgId;
  IMGHEAD *pImgHead;
  DATA16  Loop;
  DATA8   Ok;
  DATA32  Total;
  DATA32  Free;
  float   Tmp;
  int     File;
  char    PrgNameBuf[vmFILENAMESIZE];
  char    ParBuf[255];

#ifndef Linux_X86
  struct  timeval tv;
#endif

#ifdef ENABLE_STATUS_TEST
  VMInstance.Status  =  0x00;
#endif

#ifndef DISABLE_PREEMPTED_VM
  ANALOG  *pAdcTmp;

  VMInstance.pAnalog     =  &VMInstance.Analog;
  VMInstance.AdcFile     =  open(ANALOG_DEVICE_NAME,O_RDWR | O_SYNC);

  if (VMInstance.AdcFile >= MIN_HANDLE)
  {
    pAdcTmp  =  (ANALOG*)mmap(0, sizeof(ANALOG), PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, VMInstance.AdcFile, 0);

    if (pAdcTmp == MAP_FAILED)
    {
//#ifndef Linux_X86
      LogErrorNumber(ANALOG_SHARED_MEMORY);
//#endif
    }
    else
    {
      VMInstance.pAnalog  =  pAdcTmp;
    }
    close(VMInstance.AdcFile);
  }
  else
  {
//#ifndef Linux_X86
    LogErrorNumber(ANALOG_DEVICE_FILE_NOT_FOUND);
//#endif
  }
#endif

  // Reset time
#ifndef Linux_X86
  tv.tv_sec   =  0;
  tv.tv_usec  =  0;
  settimeofday(&tv,NULL);
#endif

  // Fill holes in PrimDispatchTabel
  for (Loop = 0;Loop < PRIMDISPATHTABLE_SIZE;Loop++)
  {
    if (PrimDispatchTabel[Loop] == NULL)
    {
      PrimDispatchTabel[Loop]  =  &Error;
    }
  }

  // Be sure necessary folders exist
  if (mkdir(vmSETTINGS_DIR,DIRPERMISSIONS) == 0)
  {
    chmod(vmSETTINGS_DIR,DIRPERMISSIONS);
  }

  CheckUsbstick(&Ok,&Total,&Free,0);
  CheckSdcard(&Ok,&Total,&Free,0);

  // Be sure necessary files exist
  Ok  =  0;
  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmWIFI_FILE_NAME,vmEXT_TEXT);
  File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,SYSPERMISSIONS);
  if (File >= MIN_HANDLE)
  {
    sprintf(ParBuf,"-\t");
    write(File,ParBuf,strlen(ParBuf));
    close(File);
  }

  Ok  =  0;
  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmBLUETOOTH_FILE_NAME,vmEXT_TEXT);
  File  =  open(PrgNameBuf,O_RDONLY);
  if (File >= MIN_HANDLE)
  {
    close(File);
  }
  else
  {
    File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,SYSPERMISSIONS);
    if (File >= MIN_HANDLE)
    {
      sprintf(ParBuf,"-\t");
      write(File,ParBuf,strlen(ParBuf));
      close(File);
    }
  }

  Ok  =  0;
  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmSLEEP_FILE_NAME,vmEXT_TEXT);
  File  =  open(PrgNameBuf,O_RDONLY);
  if (File >= MIN_HANDLE)
  {
    ParBuf[0]  =  0;
    read(File,ParBuf,sizeof(ParBuf));
    if (sscanf(ParBuf,"%f",&Tmp) > 0)
    {
      if ((Tmp >= (float)0) && (Tmp <= (float)127))
      {
        SetSleepMinutes((DATA8)Tmp);
        Ok  =  1;
      }
    }
    else
    {
      ParBuf[5]  =  0;
      if (strcmp(ParBuf,"never") == 0)
      {
        SetSleepMinutes(0);
        Ok  =  1;
      }
    }
    close(File);
  }
  if (!Ok)
  {
    File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,SYSPERMISSIONS);
    if (File >= MIN_HANDLE)
    {
      SetSleepMinutes((DATA8)DEFAULT_SLEEPMINUTES);
      sprintf(ParBuf,"%dmin\t",DEFAULT_SLEEPMINUTES);
      write(File,ParBuf,strlen(ParBuf));
      close (File);
    }
  }

  Ok  =  0;
  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmVOLUME_FILE_NAME,vmEXT_TEXT);
  File  =  open(PrgNameBuf,O_RDONLY);
  if (File >= MIN_HANDLE)
  {
    ParBuf[0]  =  0;
    read(File,ParBuf,sizeof(ParBuf));
    if (sscanf(ParBuf,"%f",&Tmp) > 0)
    {
      if ((Tmp >= (float)0) && (Tmp <= (float)100))
      {
        SetVolumePercent((DATA8)Tmp);
        Ok  =  1;
      }
    }
    close (File);
  }
#ifdef DISABLE_SOUND
  SetVolumePercent((DATA8)0);
  File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,SYSPERMISSIONS);
  if (File >= MIN_HANDLE)
  {
    sprintf(ParBuf,"%d%%\t",0);
    write(File,ParBuf,strlen(ParBuf));
    close (File);
  }
#else
  if (!Ok)
  {
    SetVolumePercent((DATA8)DEFAULT_VOLUME);
    File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,SYSPERMISSIONS);
    if (File >= MIN_HANDLE)
    {
      sprintf(ParBuf,"%d%%\t",DEFAULT_VOLUME);
      write(File,ParBuf,strlen(ParBuf));
      close (File);
    }
  }
#endif

  VMInstance.RefCount  =  0;

  Result |=  cOutputInit();
  Result |=  cInputInit();
  Result |=  cUiInit();
  Result |=  cMemoryInit();
  Result |=  cComInit();
  Result |=  cSoundInit();

  cValidateInit();

  for (PrgId = 0;PrgId < MAX_PROGRAMS;PrgId++)
  {
    VMInstance.Program[PrgId].Status        =  STOPPED;
    VMInstance.Program[PrgId].StatusChange  =  0;
  }

  SetTerminalEnable(TERMINAL_ENABLED);

  VMInstance.Test         =  0;

  VmPrint("\r\n\n\n\n\n\nLMS2012 VM STARTED\r\n{\r\n");
  VMInstance.ProgramId    =  DEBUG_SLOT;
  pImgHead                =  (IMGHEAD*)UiImage;
  (*pImgHead).ImageSize   =  sizeof(UiImage);

#if (HARDWARE != SIMULATION)
  openlog(PROJECT,LOG_NDELAY,LOG_USER);
  syslog(LOG_INFO,"VM Started");

  if (argc >= 2)
  {
    snprintf((char*)VMInstance.FirstProgram,MAX_FILENAME_SIZE,"%s",argv[1]);
  }
  else
  {
    snprintf((char*)VMInstance.FirstProgram,MAX_FILENAME_SIZE,DEFAULT_UI);
  }

#else
  //TBD
#endif

  ProgramReset(VMInstance.ProgramId,UiImage,(GP)VMInstance.FirstProgram,0);

  return (RESULT)(Result);
}


RESULT    mSchedCtrl(UBYTE *pRestart)
{
  RESULT  Result   = FAIL;
  ULONG   Time;
  IP      TmpIp;
#ifdef DEBUG_TRACE_VM
  IMINDEX Index;
#endif
#ifdef DEBUG_TRACE_FREEZE
  static  ULONG   Timer;
  IMINDEX Addr;
#endif


  if (VMInstance.DispatchStatus != STOPBREAK)
  {
    ProgramInit();
  }

  SetDispatchStatus(ObjectInit());

#ifdef DEBUG_TRACE_TASK
  if (VMInstance.Program[USER_SLOT].Status != STOPPED)
  {
    printf("\r\n  %d  %2d",VMInstance.ProgramId,VMInstance.ObjectId);
  }
#endif

#ifdef ENABLE_PERFORMANCE_TEST
  Time  =  cTimerGetuS();
  Time -=  VMInstance.PerformTimer;
  VMInstance.PerformTime *=  (DATAF)199;
  VMInstance.PerformTime +=  (DATAF)Time;
  VMInstance.PerformTime /=  (DATAF)200;
#endif

/*** Execute BYTECODES *******************************************************/

#ifdef DEBUG_BYTECODE_TIME
  VMInstance.InstrCnt  =  0;
#endif

#ifndef DISABLE_PREEMPTED_VM
  (*VMInstance.pAnalog).PreemptMilliSeconds  =  0;

  while ((VMInstance.Priority) && ((*VMInstance.pAnalog).PreemptMilliSeconds < 2))
#else
  while (VMInstance.Priority)
#endif
  {
    if (VMInstance.Debug)
    {
      Monitor();
    }
    else
    {
      VMInstance.Priority--;
#ifdef DEBUG_TRACE_VM
      if (VMInstance.ProgramId != GUI_SLOT)
      {
        Index  =  (IMINDEX)VMInstance.ObjectIp - (IMINDEX)VMInstance.pImage;
        cValidateDisassemble(VMInstance.pImage,&Index,VMInstance.Program[VMInstance.ProgramId].Label);
      }
#endif
      PrimDispatchTabel[*(VMInstance.ObjectIp++)]();
      VMInstance.InstrCnt++;
#ifdef DEBUG_TRACE_TASK
      if (VMInstance.Program[USER_SLOT].Status != STOPPED)
      {
        printf(".");
      }
#endif
    }
  }

/*****************************************************************************/

#ifdef ENABLE_PERFORMANCE_TEST
  VMInstance.PerformTimer  =  cTimerGetuS();
#endif

  VMInstance.NewTime  =  GetTimeMS();

  Time  =  VMInstance.NewTime - VMInstance.OldTime1;

  if (Time >= UPDATE_TIME1)
  {
    VMInstance.OldTime1 +=  Time;

#ifdef DEBUG_BYTECODE_TIME
#ifndef DISABLE_PREEMPTED_VM
    if (Time >= 3)
    {
      printf("%-6d %-3d %-3d\r\n",Time,(*VMInstance.pAnalog).PreemptMilliSeconds,VMInstance.InstrCnt);
    }
#else
    if (Time >= 3)
    {
      printf("%-6d %-3d\r\n",Time,VMInstance.InstrCnt);
    }
#endif
#endif
    cComUpdate();
    cSoundUpdate();
  }


  Time  =  VMInstance.NewTime - VMInstance.OldTime2;

  if (Time >= UPDATE_TIME2)
  {
    VMInstance.OldTime2 +=  Time;

#ifdef DEBUG_TRACE_FREEZE

    Timer +=  Time;
    if (Timer >= 100)
    {
      Timer -=  100;
      Addr   =  (IMINDEX)VMInstance.ObjectIp - (IMINDEX)VMInstance.pImage;

      printf("%10.3f P=%-1d A=%5d \r\n",(float)VMInstance.NewTime / (float)1000,VMInstance.ProgramId,Addr);

    }
#endif
    usleep(10);
    cInputUpdate((UWORD)Time);
    cUiUpdate((UWORD)Time);

    if (VMInstance.Test)
    {
      if (VMInstance.Test > (UWORD)Time)
      {
        VMInstance.Test -=  (UWORD)Time;
      }
      else
      {
        TstClose();
      }
    }
  }

  if (VMInstance.DispatchStatus == FAILBREAK)
  {
    if (VMInstance.ProgramId != GUI_SLOT)
    {
      if (VMInstance.ProgramId != CMD_SLOT)
      {
        UiInstance.Warning |=  WARNING_DSPSTAT;
      }
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"}\r\nPROGRAM \"%d\" FAIL BREAK just before %lu!\r\n",VMInstance.ProgramId,(unsigned long)(VMInstance.ObjectIp - VMInstance.Program[VMInstance.ProgramId].pImage));
      VmPrint(VMInstance.PrintBuffer);
      ProgramEnd(VMInstance.ProgramId);
      VMInstance.Program[VMInstance.ProgramId].Result          =  FAIL;
    }
    else
    {
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"UI FAIL BREAK just before %lu!\r\n",(unsigned long)(VMInstance.ObjectIp - VMInstance.Program[VMInstance.ProgramId].pImage));
      VmPrint(VMInstance.PrintBuffer);
      LogErrorNumber(VM_INTERNAL);
      *pRestart  =  1;
    }
  }
  else
  {

    if (VMInstance.DispatchStatus == INSTRBREAK)
    {
      if (VMInstance.ProgramId != CMD_SLOT)
      {
        LogErrorNumber(VM_PROGRAM_INSTRUCTION_BREAK);
      }
      TmpIp  =  VMInstance.ObjectIp - 1;
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"\r\n%4u [%2d] ",(UWORD)(((ULONG)TmpIp) - (ULONG)VMInstance.pImage),VMInstance.ObjectId);
      VmPrint(VMInstance.PrintBuffer);
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"VM       ERROR    [0x%02X]\r\n",*TmpIp);
      VmPrint(VMInstance.PrintBuffer);
      VMInstance.Program[VMInstance.ProgramId].Result          =  FAIL;
    }

    ObjectExit();

    Result  =  ObjectExec();

    if (Result == STOP)
    {
      ProgramExit();
      ProgramEnd(VMInstance.ProgramId);
      VMInstance.DispatchStatus  =  NOBREAK;
    }
    else
    {
      if (VMInstance.DispatchStatus != STOPBREAK)
      {
        ProgramExit();
      }
    }
  }

  if (VMInstance.DispatchStatus != STOPBREAK)
  {
    Result  =  ProgramExec();
  }

  if (*pRestart == 1)
  {
    Result  =  FAIL;
  }

#ifdef Linux_X86
  usleep(1);
#endif

  return (Result);
}


RESULT    mSchedExit(void)
{
  DATA32  Result = OK;

  VmPrint("}\r\nVM STOPPED\r\n\n");

#ifndef DISABLE_SDCARD_SUPPORT
  char    SDBuffer[250];
  if (VMInstance.SdcardOk == 1)
  {
    sync();
    VMInstance.SdcardOk  =  0;
    snprintf(SDBuffer,250,"rm -r %s &> /dev/null",vmSDCARD_FOLDER);
    system(SDBuffer);
  }
#endif

#ifndef DISABLE_USBSTICK_SUPPORT
  char    USBBuffer[250];
  if (VMInstance.UsbstickOk == 1)
  {
    sync();
    VMInstance.UsbstickOk  =  0;
    snprintf(USBBuffer,250,"rm -r %s &> /dev/null",vmUSBSTICK_FOLDER);
    system(USBBuffer);
  }
#endif

  Result    |=  cValidateExit();
  Result    |=  cSoundExit();
  Result    |=  cComExit();
  Result    |=  cMemoryExit();
  Result    |=  cUiExit();
  Result    |=  cInputExit();
  Result    |=  cOutputExit();

#if (HARDWARE != SIMULATION)
  closelog();
#else
//TBD
#endif

#ifndef DISABLE_PREEMPTED_VM
  VMInstance.AdcFile     =  open(ANALOG_DEVICE_NAME,O_RDWR | O_SYNC);
  if (VMInstance.AdcFile >= MIN_HANDLE)
  {
    munmap(VMInstance.pAnalog,sizeof(ANALOG));
    close(VMInstance.AdcFile);
  }
#endif

  return (RESULT)(Result);
}


#if (HARDWARE != SIMULATION)
int       main(int argc,char *argv[])
{
  RESULT  Result = FAIL;
  UBYTE   Restart;

  do
  {
    Restart  =  0;
    Result   =  mSchedInit(argc,argv);

    if (Result == OK)
    {
      do
      {
        Result  =  mSchedCtrl(&Restart);
/*
        if ((*UiInstance.pUi).State[BACK_BUTTON] & BUTTON_LONGPRESS)
        {
          Restart  =  1;
          Result   =  FAIL;
        }
*/
      }
      while (Result == OK);

      Result  =  mSchedExit();
    }
    else
    {
//TCP      system("reboot");
    }
  }
  while (Restart);

  return ((int)Result);
}

#endif


PRIM      PrimDispatchTabel[PRIMDISPATHTABLE_SIZE] =
{
  [opERROR]               =   &Error,
  [opNOP]                 =   &Nop,
  [opPROGRAM_STOP]        =   &ProgramStop,
  [opPROGRAM_START]       =   &ProgramStart,
  [opOBJECT_STOP]         =   &ObjectStop,
  [opOBJECT_START]        =   &ObjectStart,
  [opOBJECT_TRIG]         =   &ObjectTrig,
  [opOBJECT_WAIT]         =   &ObjectWait,
  [opRETURN]              =   &ObjectReturn,
  [opCALL]                =   &ObjectCall,
  [opOBJECT_END]          =   &ObjectEnd,
  [opSLEEP]               =   &Sleep,
  [opPROGRAM_INFO]        =   &ProgramInfo,
  [opLABEL]               =   &DefLabel,
  [opPROBE]               =   &Probe,
  [opDO]                  =   &Do,
  [opADD8]                =   &cMathAdd8,
  [opADD16]               =   &cMathAdd16,
  [opADD32]               =   &cMathAdd32,
  [opADDF]                =   &cMathAddF,
  [opSUB8]                =   &cMathSub8,
  [opSUB16]               =   &cMathSub16,
  [opSUB32]               =   &cMathSub32,
  [opSUBF]                =   &cMathSubF,
  [opMUL8]                =   &cMathMul8,
  [opMUL16]               =   &cMathMul16,
  [opMUL32]               =   &cMathMul32,
  [opMULF]                =   &cMathMulF,
  [opDIV8]                =   &cMathDiv8,
  [opDIV16]               =   &cMathDiv16,
  [opDIV32]               =   &cMathDiv32,
  [opDIVF]                =   &cMathDivF,
  [opOR8]                 =   &cMathOr8,
  [opOR16]                =   &cMathOr16,
  [opOR32]                =   &cMathOr32,

  [opAND8]                =   &cMathAnd8,
  [opAND16]               =   &cMathAnd16,
  [opAND32]               =   &cMathAnd32,

  [opXOR8]                =   &cMathXor8,
  [opXOR16]               =   &cMathXor16,
  [opXOR32]               =   &cMathXor32,

  [opRL8]                 =   &cMathRl8,
  [opRL16]                =   &cMathRl16,
  [opRL32]                =   &cMathRl32,
  [opINIT_BYTES]          =   &cMoveInitBytes,
  [opMOVE8_8]             =   &cMove8to8,
  [opMOVE8_16]            =   &cMove8to16,
  [opMOVE8_32]            =   &cMove8to32,
  [opMOVE8_F]             =   &cMove8toF,
  [opMOVE16_8]            =   &cMove16to8,
  [opMOVE16_16]           =   &cMove16to16,
  [opMOVE16_32]           =   &cMove16to32,
  [opMOVE16_F]            =   &cMove16toF,
  [opMOVE32_8]            =   &cMove32to8,
  [opMOVE32_16]           =   &cMove32to16,
  [opMOVE32_32]           =   &cMove32to32,
  [opMOVE32_F]            =   &cMove32toF,
  [opMOVEF_8]             =   &cMoveFto8,
  [opMOVEF_16]            =   &cMoveFto16,
  [opMOVEF_32]            =   &cMoveFto32,
  [opMOVEF_F]             =   &cMoveFtoF,
  [opJR]                  =   &cBranchJr,
  [opJR_FALSE]            =   &cBranchJrFalse,
  [opJR_TRUE]             =   &cBranchJrTrue,
  [opJR_NAN]              =   &cBranchJrNan,
  [opCP_LT8]              =   &cCompareLt8,
  [opCP_LT16]             =   &cCompareLt16,
  [opCP_LT32]             =   &cCompareLt32,
  [opCP_LTF]              =   &cCompareLtF,
  [opCP_GT8]              =   &cCompareGt8,
  [opCP_GT16]             =   &cCompareGt16,
  [opCP_GT32]             =   &cCompareGt32,
  [opCP_GTF]              =   &cCompareGtF,
  [opCP_EQ8]              =   &cCompareEq8,
  [opCP_EQ16]             =   &cCompareEq16,
  [opCP_EQ32]             =   &cCompareEq32,
  [opCP_EQF]              =   &cCompareEqF,
  [opCP_NEQ8]             =   &cCompareNEq8,
  [opCP_NEQ16]            =   &cCompareNEq16,
  [opCP_NEQ32]            =   &cCompareNEq32,
  [opCP_NEQF]             =   &cCompareNEqF,
  [opCP_LTEQ8]            =   &cCompareLtEq8,
  [opCP_LTEQ16]           =   &cCompareLtEq16,
  [opCP_LTEQ32]           =   &cCompareLtEq32,
  [opCP_LTEQF]            =   &cCompareLtEqF,
  [opCP_GTEQ8]            =   &cCompareGtEq8,
  [opCP_GTEQ16]           =   &cCompareGtEq16,
  [opCP_GTEQ32]           =   &cCompareGtEq32,
  [opCP_GTEQF]            =   &cCompareGtEqF,
  [opSELECT8]             =   &cCompareSelect8,
  [opSELECT16]            =   &cCompareSelect16,
  [opSELECT32]            =   &cCompareSelect32,
  [opSELECTF]             =   &cCompareSelectF,
  [opSYSTEM]              =   &System,
  [opPORT_CNV_OUTPUT]     =   &PortCnvOutput,
  [opPORT_CNV_INPUT]      =   &PortCnvInput,
  [opNOTE_TO_FREQ]        =   &NoteToFreq,

  [opJR_LT8]              =   &cBranchJrLt8,
  [opJR_LT16]             =   &cBranchJrLt16,
  [opJR_LT32]             =   &cBranchJrLt32,
  [opJR_LTF]              =   &cBranchJrLtF,
  [opJR_GT8]              =   &cBranchJrGt8,
  [opJR_GT16]             =   &cBranchJrGt16,
  [opJR_GT32]             =   &cBranchJrGt32,
  [opJR_GTF]              =   &cBranchJrGtF,
  [opJR_EQ8]              =   &cBranchJrEq8,
  [opJR_EQ16]             =   &cBranchJrEq16,
  [opJR_EQ32]             =   &cBranchJrEq32,
  [opJR_EQF]              =   &cBranchJrEqF,
  [opJR_NEQ8]             =   &cBranchJrNEq8,
  [opJR_NEQ16]            =   &cBranchJrNEq16,
  [opJR_NEQ32]            =   &cBranchJrNEq32,
  [opJR_NEQF]             =   &cBranchJrNEqF,
  [opJR_LTEQ8]            =   &cBranchJrLtEq8,
  [opJR_LTEQ16]           =   &cBranchJrLtEq16,
  [opJR_LTEQ32]           =   &cBranchJrLtEq32,
  [opJR_LTEQF]            =   &cBranchJrLtEqF,
  [opJR_GTEQ8]            =   &cBranchJrGtEq8,
  [opJR_GTEQ16]           =   &cBranchJrGtEq16,
  [opJR_GTEQ32]           =   &cBranchJrGtEq32,
  [opJR_GTEQF]            =   &cBranchJrGtEqF,
  [opINFO]                =   &Info,
  [opSTRINGS]             =   &Strings,
  [opMEMORY_WRITE]        =   &MemoryWrite,
  [opMEMORY_READ]         =   &MemoryRead,
  [opUI_FLUSH]            =   &cUiFlush,
  [opUI_READ]             =   &cUiRead,
  [opUI_WRITE]            =   &cUiWrite,
  [opUI_BUTTON]           =   &cUiButton,
  [opUI_DRAW]             =   &cUiDraw,
  [opTIMER_WAIT]          =   &cTimerWait,
  [opTIMER_READY]         =   &cTimerReady,
  [opTIMER_READ]          =   &cTimerRead,
  [opBP0]                 =   &BreakPoint,
  [opBP1]                 =   &BreakPoint,
  [opBP2]                 =   &BreakPoint,
  [opBP3]                 =   &BreakPoint,
  [opBP_SET]              =   &BreakSet,
  [opMATH]                =   &cMath,
  [opRANDOM]              =   &Random,
  [opTIMER_READ_US]       =   &cTimerReaduS,
  [opKEEP_ALIVE]          =   &cUiKeepAlive,
  [opCOM_READ]            =   &cComRead,
  [opCOM_WRITE]           =   &cComWrite,
  [opSOUND]               =   &cSoundEntry,
  [opSOUND_TEST]          =   &cSoundTest,
  [opSOUND_READY]         =   &cSoundReady,
  [opINPUT_SAMPLE]        =   &cInputSample,
  [opINPUT_DEVICE_LIST]   =   &cInputDeviceList,
  [opINPUT_DEVICE]        =   &cInputDevice,
  [opINPUT_READ]          =   &cInputRead,
  [opINPUT_TEST]          =   &cInputTest,
  [opINPUT_READY]         =   &cInputReady,
  [opINPUT_READSI]        =   &cInputReadSi,
  [opINPUT_READEXT]       =   &cInputReadExt,
  [opINPUT_WRITE]         =   &cInputWrite,
  [opOUTPUT_SET_TYPE]     =   &cOutputSetType,
  [opOUTPUT_RESET]        =   &cOutputReset,
  [opOUTPUT_STOP]         =   &cOutputStop,
  [opOUTPUT_POWER]        =   &cOutputPower,
  [opOUTPUT_SPEED]        =   &cOutputSpeed,
  [opOUTPUT_START]        =   &cOutputStart,
  [opOUTPUT_POLARITY]     =   &cOutputPolarity,
  [opOUTPUT_READ]         =   &cOutputRead,
  [opOUTPUT_TEST]         =   &cOutputTest,
  [opOUTPUT_READY]        =   &cOutputReady,
  [opOUTPUT_STEP_POWER]   =   &cOutputStepPower,
  [opOUTPUT_TIME_POWER]   =   &cOutputTimePower,
  [opOUTPUT_STEP_SPEED]   =   &cOutputStepSpeed,
  [opOUTPUT_TIME_SPEED]   =   &cOutputTimeSpeed,
  [opOUTPUT_STEP_SYNC]    =   &cOutputStepSync,
  [opOUTPUT_TIME_SYNC]    =   &cOutputTimeSync,
  [opOUTPUT_CLR_COUNT]    =   &cOutputClrCount,
  [opOUTPUT_GET_COUNT]    =   &cOutputGetCount,
  [opOUTPUT_PRG_STOP]     =   &cOutputPrgStop,
  [opFILE]                =   &cMemoryFile,
  [opARRAY]               =   &cMemoryArray,
  [opARRAY_WRITE]         =   &cMemoryArrayWrite,
  [opARRAY_READ]          =   &cMemoryArrayRead,
  [opARRAY_APPEND]        =   &cMemoryArrayAppend,
  [opMEMORY_USAGE]        =   &cMemoryUsage,
  [opFILENAME]            =   &cMemoryFileName,
  [opREAD8]               =   &cMoveRead8,
  [opREAD16]              =   &cMoveRead16,
  [opREAD32]              =   &cMoveRead32,
  [opREADF]               =   &cMoveReadF,
  [opWRITE8]              =   &cMoveWrite8,
  [opWRITE16]             =   &cMoveWrite16,
  [opWRITE32]             =   &cMoveWrite32,
  [opWRITEF]              =   &cMoveWriteF,
  [opCOM_READY]           =   &cComReady,
  [opCOM_READDATA]        =   &Error,
  [opCOM_WRITEDATA]       =   &Error,
  [opCOM_GET]             =   &cComGet,
  [opCOM_SET]             =   &cComSet,
  [opCOM_TEST]            =   &cComTest,
  [opCOM_REMOVE]          =   &cComRemove,
  [opCOM_WRITEFILE]       =   &cComWriteFile,
  [opMAILBOX_OPEN]        =   &cComOpenMailBox,
  [opMAILBOX_WRITE]       =   &cComWriteMailBox,
  [opMAILBOX_READ]        =   &cComReadMailBox,
  [opMAILBOX_TEST]        =   &cComTestMailBox,
  [opMAILBOX_READY]       =   &cComReadyMailBox,
  [opMAILBOX_CLOSE]       =   &cComCloseMailBox,

  [opTST]                 =   &Tst
};

//******* BYTE CODE SNIPPETS **************************************************


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opERROR </b>
 *
 *- This code does not exist in normal program\n
 *- Dispatch status changes to INSTRBREAK
 *
 */
/*! \brief    opOUTPUT_READY byte code
 *
 *            Uses following from current program context:
 *            DispatchStatus
 */
void      Error(void)
{
  ProgramEnd(VMInstance.ProgramId);
  VMInstance.Program[VMInstance.ProgramId].Result          =  FAIL;
  SetDispatchStatus(INSTRBREAK);
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opNOP </b>
 *
 *- This code does absolutely nothing\n
 *
 */
/*! \brief  opNOP byte code
 *
 */
void      Nop(void)
{
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opPROGRAM_STOP (PRGID)</b>
 *
 *- Stops specific program id slot\n
 *- Dispatch status changes to PRGBREAK
 *
 *  \param  (DATA16)  PRGID  - Program id (GUI_SLOT = all, CURRENT_SLOT = current) (see \ref prgid)
 */
/*! \brief    opPROGRAM_STOP byte code
 *
 *            Stops specific program id slot
 *
 *            Uses following from current program context:
 *            Program[], DispatchStatus
 *
 */
void      ProgramStop(void)
{
  DATA16  PrgId;

  PrgId    =  *(DATA16*)PrimParPointer();

  if (PrgId == GUI_SLOT)
  {
    PrgId  =  MAX_PROGRAMS;
    do
    {
      PrgId--;
      ProgramEnd(PrgId);
    }
    while (PrgId);
  }
  else
  {
    if (PrgId == CURRENT_SLOT)
    {
      PrgId  =  CurrentProgramId();
    }
    ProgramEnd(PrgId);
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opPROGRAM_START (PRGID, SIZE, *IP, DEBUG)</b>
 *
 *- Start program id slot\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)    PRGID  - Program id  (see \ref prgid)
 *  \param  (DATA32)    SIZE   - Size of image\n
 *  \param  (DATA32)    *IP    - Address of image (value from opFILE(LOAD_IMAGE,..)  )
 *  \param  (DATA8)     DEBUG  - Debug mode (0=normal, 1=debug, 2=don't execute)
 */
/*! \brief    opPROGRAM_START byte code
 *
 *            Start program id slot
 *
 *            Uses following from current program context:
 *            Program[], DispatchStatus
 *
 */
void      ProgramStart(void)
{
  PRGID   PrgId;
  PRGID   TmpPrgId;
  IP      pI;
  UBYTE   DB;
  UBYTE   Flag = 0;


  PrgId  =  *(PRGID*)PrimParPointer();

  // Dummy
  pI     =  *(IP*)PrimParPointer();

  pI     =  *(IP*)PrimParPointer();
  DB     =  *(DATA8*)PrimParPointer();


  if (VMInstance.Program[PrgId].Status == STOPPED)
  {
    TmpPrgId  =  CurrentProgramId();

    if ((TmpPrgId == CMD_SLOT) || (TmpPrgId == TERM_SLOT))
    { // Direct command starting a program

      if ((VMInstance.Program[USER_SLOT].Status == STOPPED) && (VMInstance.Program[DEBUG_SLOT].Status == STOPPED))
      { // User and debug must be stooped

        if (ProgramReset(PrgId,pI,NULL,DB) == OK)
        {
          Flag  =  1;
        }
      }
    }
    else
    { // Gui, user or debug starting a program

      if (ProgramReset(PrgId,pI,NULL,DB) == OK)
      {
        Flag  =  1;
      }
    }
  }
  if (!Flag)
  {
//    LogErrorNumber(VM_PROGRAM_NOT_STARTED);
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opOBJECT_STOP (OBJID)</b>
 *
 *- Stops specific object\n
 *- Dispatch status changes to STOPBREAK
 *
 *  \param  (DATA16)  OBJID  - Object id
 */
/*! \brief    opOBJECT_STOP byte code
 *
 *            Stops specific object
 *
 *            Uses following from current program context:
 *            pObjectList, ObjectIp, DispatchStatus
 *
 */
void      ObjectStop(void)
{
  ObjectDeQueue(*(OBJID*)PrimParPointer());
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opOBJECT_START (OBJID)</b>
 *
 *- Start specific object\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  OBJID  - Object id
 */
/*! \brief    opOBJECT_START byte code
 *
 *            Start specific object
 *
 *            Uses following from current program context:
 *            pObjectList
 *
 */
void      ObjectStart(void)
{
  ObjectEnQueue(*(OBJID*)PrimParPointer());
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opOBJECT_TRIG (OBJID)</b>
 *
 *- Triggers object and run the object if fully triggered\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  OBJID  - Object id
 */
/*! \brief    opOBJECT_TRIG byte code
 *
 *            Triggers object and run the object if fully triggered
 *
 *            Uses following from current program context:
 *            pObjList
 *
 */
void      ObjectTrig(void)
{
  OBJID   TmpId;

  TmpId  =  *(OBJID*)PrimParPointer();

  (*VMInstance.pObjList[TmpId]).ObjStatus  =  WAITING;
  if ((*VMInstance.pObjList[TmpId]).u.TriggerCount)
  {
    ((*VMInstance.pObjList[TmpId]).u.TriggerCount)--;
    if ((*VMInstance.pObjList[TmpId]).u.TriggerCount == 0)
    {
      ObjectReset(TmpId);
      ObjectEnQueue(TmpId);
/*
  #ifdef OLDCALL
      ObjectEnQueue(TmpId);
  #else
      (*VMInstance.pObjList[VMInstance.ObjectId]).Ip      =  VMInstance.ObjectIp;
      VMInstance.ObjectId                                 =  TmpId;
      (*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus  =  RUNNING;
      VMInstance.ObjectIp        =  (*VMInstance.pObjList[VMInstance.ObjectId]).Ip;
      VMInstance.ObjectLocal     =  (*VMInstance.pObjList[VMInstance.ObjectId]).pLocal;
  #endif
*/
    }
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opOBJECT_WAIT (OBJID)</b>
 *
 *- Wait until object has run\n
 *- Dispatch status can change to BUSYBREAK
 *
 *  \param  (DATA16)  OBJID  - Object id
 */
/*! \brief    opOBJECT_WAIT byte code
 *
 *            Wait until object has run
 *
 *            Uses following from current program context:
 *            ObjectIp, pObjList, DispatchStatus
 *
 */
void      ObjectWait(void)
{
  OBJID   TmpId;
  IP      TmpIp;

  TmpIp  =  VMInstance.ObjectIp;
  TmpId  =  *(OBJID*)PrimParPointer();

  if ((*VMInstance.pObjList[TmpId]).ObjStatus != STOPPED)
  {
    VMInstance.ObjectIp        =  TmpIp - 1;
    SetDispatchStatus(BUSYBREAK);
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opRETURN </b>
 *
 *- Return from byte code subroutine\n
 *- Dispatch status changes to STOPBREAK
 *
 */
/*! \brief    opRETURN byte code
 *
 *            Return from byte code subroutine
 *
 *            Uses following from current program context:
 *            ObjectId, pObjList, DispatchStatus
 *
 */
void      ObjectReturn(void)
{
  OBJID   ObjectIdCaller;

  // Get caller id from saved
  ObjectIdCaller  =  (*VMInstance.pObjList[VMInstance.ObjectId]).u.CallerId;

  // Copy local variables to parameters
  VMInstance.ObjectLocal =  (*VMInstance.pObjList[ObjectIdCaller]).pLocal;
  CopyLocalsToPars(ObjectIdCaller);

  // Stop called object and start calling object
#ifdef OLDCALL
  ObjectDeQueue(VMInstance.ObjectId);
  ObjectEnQueue(ObjectIdCaller);
#else
  (*VMInstance.pObjList[VMInstance.ObjectId]).Ip          =  VMInstance.ObjectIp;
  (*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus   =  STOPPED;

#ifndef DISABLE_NEW_CALL_MUTEX
  if ((*VMInstance.pObjList[VMInstance.ObjectId]).Blocked)
  {
    (*VMInstance.pObjList[VMInstance.ObjectId]).Blocked  =  0;
    SetDispatchStatus(STOPBREAK);
  }
#endif
  VMInstance.ObjectId                                     =  ObjectIdCaller;
  (*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus   =  RUNNING;
  VMInstance.ObjectIp        =  (*VMInstance.pObjList[VMInstance.ObjectId]).Ip;
  VMInstance.ObjectLocal     =  (*VMInstance.pObjList[VMInstance.ObjectId]).pLocal;
#endif
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opCALL (OBJID, PARAMETERS, ....)</b>
 *
 *- Calls byte code subroutine\n
 *- Dispatch status changes to STOPBREAK or BUSYBREAK
 *
 *  \param  (DATA16)  OBJID       - Object id
 *  \param  (DATA8)   PARAMETERS  - Number of parameters\n
 *
 *- \ref parameterencoding
 *
 */
/*! \brief    opCALL byte code
 *
 *            Calls byte code subroutine
 *
 *            Uses following from current program context:
 *            ObjectIp, pObjList, pObjHead, DispatchStatus
 *
 */
void      ObjectCall(void)
{
  IP      TmpIp;
  OBJID   ObjectIdToCall;

  // Save IP in case object are locked
  TmpIp   =  GetObjectIp();

  // Get object to call from byte stream
  ObjectIdToCall  =  *(OBJID*)PrimParPointer();
  if ((*VMInstance.pObjList[ObjectIdToCall]).ObjStatus == STOPPED)
  { // Object free

    // Get number of parameters
    PrimParPointer();

    // Initialise  object
    ObjectReset(ObjectIdToCall);

    // Save mother id
    (*VMInstance.pObjList[ObjectIdToCall]).u.CallerId    =  VMInstance.ObjectId;

    // Copy parameters to local variables
    CopyParsToLocals(ObjectIdToCall);

    // Halt calling object
    (*VMInstance.pObjList[VMInstance.ObjectId]).Ip          =  VMInstance.ObjectIp;
    (*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus   =  HALTED;

    // Start called object
#ifdef OLDCALL
    SetDispatchStatus(STOPBREAK);
    ObjectEnQueue(ObjectIdToCall);
#else
    VMInstance.ObjectId                                     =  ObjectIdToCall;
    (*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus   =  RUNNING;
    VMInstance.ObjectIp        =  (*VMInstance.pObjList[VMInstance.ObjectId]).Ip;
    VMInstance.ObjectLocal     =  (*VMInstance.pObjList[VMInstance.ObjectId]).pLocal;
#endif

  }
  else
  { // Object locked - rewind IP

#ifdef DEBUG
  printf("SUBCALL %u BUSY status = %u\r\n",ObjectIdToCall,(*VMInstance.pObjList[ObjectIdToCall]).ObjStatus);
#endif
    SetObjectIp(TmpIp - 1);
    SetDispatchStatus(BUSYBREAK);
#ifndef DISABLE_NEW_CALL_MUTEX
    (*VMInstance.pObjList[ObjectIdToCall]).Blocked  =  1;
#endif
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opOBJECT_END </b>
 *
 *- Stops current object\n
 *- Dispatch status changes to STOPBREAK
 *
 */
/*! \brief    opOBJECT_END byte code
 *
 *            Stops current object
 *
 *            Uses following from current program context:
 *            pObjectList, ObjectIp, DispatchStatus
 *
 */
void      ObjectEnd(void)
{
  (*VMInstance.pObjList[VMInstance.ObjectId]).Ip          =  &VMInstance.Program[VMInstance.ProgramId].pImage[(ULONG)VMInstance.Program[VMInstance.ProgramId].pObjHead[VMInstance.ObjectId].OffsetToInstructions];
  (*VMInstance.pObjList[VMInstance.ObjectId]).ObjStatus   =  STOPPED;
  SetDispatchStatus(STOPBREAK);
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opSLEEP </b>
 *
 *- Breaks execution of current VMTHREAD\n
 *- Dispatch status changes to INSTRBREAK
 *
 */
/*! \brief    opSLEEP byte code
 *
 *            Breaks execution of current VMTHREAD
 *
 *            Uses following from current program context:
 *            DispatchStatus
 *
 *
 */
void      Sleep(void)
{
  SetDispatchStatus(SLEEPBREAK);
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opPROGRAM_INFO (CMD, PRGID, DATA)</b>
 *
 *- Get program data\n
 *- Dispatch status can change to FAILBREAK
 *
 *  \param  (DATA8)   CMD     - \ref programinfosubcode
 *
 *\n
 *  - CMD = OBJ_STOP
 *    - \param  (DATA16)    PRGID  - Program slot number  (see \ref prgid)
 *    - \param  (DATA16)    OBJID  - Object id\n
 *
 *\n
 *  - CMD = OBJ_START
 *    - \param  (DATA16)    PRGID  - Program slot number  (see \ref prgid)
 *    - \param  (DATA16)    OBJID  - Object id\n
 *
 *\n
 *  - CMD = GET_STATUS
 *    - \param  (DATA16)    PRGID  - Program slot number  (see \ref prgid)
 *    - \return (DATA8)     DATA   - Program status\n
 *
 *\n
 *  - CMD = GET_PRGRESULT
 *    - \param  (DATA16)    PRGID  - Program slot number  (see \ref prgid)
 *    - \return (DATA8)     DATA   - Program result [OK, BUSY, FAIL]\n
 *
 *\n
 *  - CMD = GET_SPEED
 *    - \param  (DATA16)    PRGID  - Program slot number  (see \ref prgid)
 *    - \return (DATA32)    DATA   - Program speed [instr/S]\n
 *
 *\n
 */
/*! \brief    opPROGRAM_INFO byte code
 *
 *            Get program informations
 *
 *            Uses following from current program context:
 *            None
 *
 */
void      ProgramInfo(void)
{
  DATA8   Cmd;
  DATA16  Instr;
  PRGID   PrgId;
  OBJID   ObjIndex;

  Cmd             =  *(DATA8*)PrimParPointer();
  PrgId           =  *(PRGID*)PrimParPointer();

  switch (Cmd)
  {
    case OBJ_STOP :
    {
      ObjIndex    =  *(OBJID*)PrimParPointer();
      if ((ObjIndex > 0) && (ObjIndex <= VMInstance.Program[PrgId].Objects) && (VMInstance.Program[PrgId].Status != STOPPED))
      {
        (*VMInstance.Program[PrgId].pObjList[ObjIndex]).ObjStatus  =  STOPPED;
      }
    }
    break;

    case OBJ_START :
    {
      ObjIndex    =  *(OBJID*)PrimParPointer();
      if ((ObjIndex > 0) && (ObjIndex <= VMInstance.Program[PrgId].Objects) && (VMInstance.Program[PrgId].Status != STOPPED))
      {
        if (ObjIndex == 1)
        {
          VMInstance.Program[PrgId].StartTime                           =  GetTimeMS();
          VMInstance.Program[PrgId].RunTime                             =  cTimerGetuS();
        }
        (*VMInstance.Program[PrgId].pObjList[ObjIndex]).ObjStatus       =  RUNNING;
        (*VMInstance.Program[PrgId].pObjList[ObjIndex]).Ip              = &VMInstance.Program[PrgId].pImage[(ULONG)VMInstance.Program[PrgId].pObjHead[ObjIndex].OffsetToInstructions];
        (*VMInstance.Program[PrgId].pObjList[ObjIndex]).u.TriggerCount  =  VMInstance.Program[PrgId].pObjHead[ObjIndex].TriggerCount;
      }
    }
    break;

    case GET_STATUS :
    {
      *(DATA8*)PrimParPointer()               =  (DATA8)VMInstance.Program[PrgId].Status;
    }
    break;

    case GET_PRGRESULT :
    {
      *(DATA8*)PrimParPointer()               =  (DATA8)VMInstance.Program[PrgId].Result;
    }
    break;

    case GET_SPEED :
    {
      *(DATA32*)PrimParPointer()              =  (DATA32)(((float)VMInstance.Program[PrgId].InstrCnt  * (float)1000000) / (float)VMInstance.Program[PrgId].InstrTime);
    }
    break;

    case SET_INSTR :
    {
      Instr  =  *(DATA16*)PrimParPointer();
      SetInstructions((ULONG)Instr);
    }
    break;

    default :
    {
      SetDispatchStatus(FAILBREAK);
    }
    break;

  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opLABEL (NO)</b>
 *
 *- This code does nothing besides marking an address to a label\n
 *
 *  \param    (DATA8)   NO - Label number
 */
/*! \brief  opLABEL byte code
 *
 */
void      DefLabel(void)
{
  PrimParPointer();
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opPROBE (PRGID, OBJID, OFFSET, SIZE)</b>
 *
 *- Display globals or object locals on terminal\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  PRGID   - Program slot number (see \ref prgid)
 *  \param  (DATA16)  OBJID   - Object id (zero means globals)
 *  \param  (DATA32)  OFFSET  - Offset (start from)
 *  \param  (DATA32)  SIZE    - Size (length of dump) zero means all (max 1024)
 */
/*! \brief    opPROBE byte code
 *
 *            Display globals or object locals on terminal
 *
 *            Uses following from current program context:
 *            None
 *
 */
void      Probe(void)
{
  PRGID   PrgId;
  OBJID   ObjId;
  GBINDEX RamOffset;
  VARDATA *Ram;
  GBINDEX Size;
  GBINDEX Tmp;
  GBINDEX Lng;

  PrgId       =  *(PRGID*)PrimParPointer();
  ObjId       =  *(OBJID*)PrimParPointer();
  RamOffset   =  *(GBINDEX*)PrimParPointer();
  Size        =  *(GBINDEX*)PrimParPointer();

  if (VMInstance.Program[PrgId].Status != STOPPED)
  {
    if (ObjId == 0)
    {
      Ram  =  VMInstance.Program[PrgId].pGlobal;
      Lng  =  (*(IMGHEAD*)VMInstance.Program[PrgId].pImage).GlobalBytes;
    }
    else
    {
      Ram  =  (*VMInstance.Program[PrgId].pObjList[ObjId]).pLocal;
      Lng  =  VMInstance.Program[PrgId].pObjHead[ObjId].LocalBytes;
    }
    Ram   +=  RamOffset;
    if (Size == 0)
    {
      Size  =  Lng;
    }
    if (Size)
    {
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"    PROBE  Prg=%d Obj=%d Offs=%ld Lng=%ld\r\n    {\r\n  ",PrgId,ObjId,(long unsigned int)RamOffset,(long unsigned int)Size);
      VmPrint(VMInstance.PrintBuffer);

      for (Tmp = 0;(Tmp < Size) && (Tmp < Lng) && (Tmp < 1024);Tmp++)
      {
        if ((Tmp & 0x0F) == 0)
        {
          snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"    %08lX ",(long unsigned int)(RamOffset + (GBINDEX)Tmp));
          VmPrint(VMInstance.PrintBuffer);
        }
        snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"%02X ",(UBYTE)(*Ram & 0xFF));
        VmPrint(VMInstance.PrintBuffer);
        if (((Tmp & 0x0F) == 0xF) && (Tmp < (Size - 1)))
        {
          VmPrint("\r\n    ");
        }
        Ram++;
      }

      VmPrint("\r\n    }\r\n");
    }
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opDO (PRGID, IMAGE)</b>
 *
 *- Run byte code snippet\n
 *- Dispatch status can change to BUSYBREAK
 *
 *  \param    (DATA16)   PRGID    - Program slot number
 *  \param    (DATA32)   *IMAGE   - Address of image
 *  \param    (DATA32)   *GLOBAL  - Address of global variables
 */
/*! \brief    opDO byte code
 *
 *            Run byte code snippet
 *
 *            Uses following from current program context:
 *            ObjectIp, ProgramId
 *
 */
void      Do(void)
{
  DATA16  PrgId;
  DATA32  pImage;
  DATA32  pGlobal;

  PrgId       =  *(DATA16*)PrimParPointer();
  pImage      =  *(DATA32*)PrimParPointer();
  pGlobal     =  *(DATA32*)PrimParPointer();

  if (ProgramStatus(PrgId) != STOPPED)
  {
    ProgramEnd(PrgId);
  }
  if (ProgramStatus(PrgId) == STOPPED)
  {
    if (ProgramReset(PrgId,(IP)pImage,(GP)pGlobal,0) != OK)
    {
      if (PrgId != CMD_SLOT)
      {
        LogErrorNumber(VM_PROGRAM_NOT_STARTED);
      }
    }
  }
  cUiAlive();

}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opBP0 - opBP3 </b>
 *
 *- Display globals or object locals on terminal\n
 *- Removes it self when done
 *- Dispatch status unchanged
 *
 */
/*! \brief    opBP0 - opBP3 byte code
 *
 *            Display globals or object locals on terminal
 *
 *            Uses following from current program context:
 *            pObjHead, ObjectIp, ProgramId
 *
 */
void      BreakPoint(void)
{
  IP      TmpIp;
  DATA8   No;
  float   Instr;

  TmpIp   =  (--VMInstance.ObjectIp);
  No      =  *(DATA8*)TmpIp;

  if (VMInstance.Program[VMInstance.ProgramId].Brkp[No & 0x03].OpCode)
  {
    *(DATA8*)TmpIp  =  VMInstance.Program[VMInstance.ProgramId].Brkp[No & 0x03].OpCode;
  }
  else
  {
    VMInstance.ObjectIp++;
  }

  if ((No & 0x03) == 3)
  {
    cUiTestpin(1);
    cUiTestpin(0);
  }
  else
  {
    Instr  =  VMInstance.Program[VMInstance.ProgramId].InstrCnt +  VMInstance.InstrCnt;

    snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"\r\nBREAKPOINT #%d (%.0f)",No & 0x03,Instr);
    VmPrint(VMInstance.PrintBuffer);

    VMInstance.Debug  =  1;
  }
  PrimDispatchTabel[*(VMInstance.ObjectIp++)]();
  *(DATA8*)TmpIp  =  No;
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opBP_SET (PRGID, NO, ADDRESS)</b>
 *
 *- Set break point in byte code program\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  PRGID   - Program slot number (see \ref prgid)
 *  \param  (DATA8)   NO      - Breakpoint number [0..2] (3 = trigger out on TP4)
 *  \param  (DATA32)  ADDRESS - Address (Offset from start of image) (0 = remove breakpoint)
 */
/*! \brief    opBP_SET byte code
 *
 *            Set break point in byte code program
 *
 *            Uses following from current program context:
 *            None
 *
 */
void      BreakSet(void)
{
  PRGID   PrgId;
  DATA8   No;
  DATA32  Addr;

  PrgId   =  *(PRGID*)PrimParPointer();
  No      =  *(DATA8*)PrimParPointer();
  Addr    =  *(IMINDEX*)PrimParPointer();

  if (No < MAX_BREAKPOINTS)
  {
    if (VMInstance.Program[PrgId].Status != STOPPED)
    {
      if (Addr)
      {
        VMInstance.Program[PrgId].Brkp[No].Addr     =  Addr;
        VMInstance.Program[PrgId].Brkp[No].OpCode   =  (OP)VMInstance.Program[PrgId].pImage[Addr];
        VMInstance.Program[PrgId].pImage[Addr]      =  opBP0 + No;
      }
      else
      {
        if ((VMInstance.Program[PrgId].Brkp[No].Addr != 0) && (VMInstance.Program[PrgId].Brkp[No].OpCode != 0))
        {
          Addr  =  VMInstance.Program[PrgId].Brkp[No].Addr;
          VMInstance.Program[PrgId].pImage[Addr]  =  VMInstance.Program[PrgId].Brkp[No].OpCode;
        }
        VMInstance.Program[PrgId].Brkp[No].Addr     =  0;
        VMInstance.Program[PrgId].Brkp[No].OpCode   =  0;
      }
    }
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opRANDOM (MIN, MAX, VALUE)</b>
 *
 *- Get random value\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  MIN     - Minimum value
 *  \param  (DATA16)  MAX     - Maximum value
 *  \return (DATA16)  VALUE   - Value
 */
/*! \brief    opRANDOM byte code
 *
 *            Get random VALUE between MIN and MAX
 *
 *            Uses following from current program context:
 *            None
 *
 */
void      Random(void)
{
  DATA16  Min;
  DATA16  Max;
  DATA16  Result;

  Min     =  *(DATA16*)PrimParPointer();
  Max     =  *(DATA16*)PrimParPointer();

  Result  =  (DATA16)rand();

  if (Result < 0)
  {
    Result  =  0 - Result;
  }

  Result  =  (((Result * (Max - Min)) + 16383) / 32767);
  Result +=  Min;

  *(DATA16*)PrimParPointer()  =  Result;
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opINFO (CMD, ....)  </b>
 *
 *- Info functions entry\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD           - \ref infosubcode
 *
 *\n
 *  - CMD = SET_ERROR
 *    -  \param  (DATA8)   NUMBER   - Error number\n
 *
 *\n
 *  - CMD = GET_ERROR
 *    -  \return (DATA8)   NUMBER   - Error number\n
 *
 *\n
 *  - CMD = ERRORTEXT
 *\n  Convert error number to text string\n
 *    -  \param  (DATA8)   NUMBER       - Error number\n
 *    -  \param  (DATA8)   LENGTH       - Maximal length of string returned (-1 = no check)\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = GET_VOLUME
 *    -  \return (DATA8)   VALUE    - Volume [0..100%]\n
 *
 *\n
 *  - CMD = SET_VOLUME
 *    -  \param  (DATA8)   VALUE    - Volume [0..100%]\n
 *
 *\n
 *  - CMD = GET_MINUTES
 *    -  \return (DATA8)   VALUE    - Minutes to sleep [0..120min] (0 = ~)\n
 *
 *\n
 *  - CMD = SET_MINUTES
 *    -  \param  (DATA8)   VALUE    - Minutes to sleep [0..120min] (0 = ~)\n
 *
 *\n
 *
 */
/*! \brief  opINFO byte code
 *
 */
void      Info(void)
{
  DATA8   Cmd;
  DATA8   Tmp;
  DATA8   Length;
  DATA8   *pDestination;
  DATA8   Number;

  Cmd           =  *(DATA8*)PrimParPointer();
  switch (Cmd)
  { // Function

    case SET_ERROR :
    {
      LogErrorNumber(*(DATA8*)PrimParPointer());
    }
    break;

    case GET_ERROR :
    {
      *(DATA8*)PrimParPointer()  =  LogErrorGet();
    }
    break;

    case ERRORTEXT :
    {
      Number          =  *(DATA8*)PrimParPointer();
      Length          =  *(DATA8*)PrimParPointer();
      pDestination    =  (DATA8*)PrimParPointer();

      if (VMInstance.Handle >= 0)
      {
        if (Number < ERRORS)
        {
          Tmp  =  (DATA8)strlen(ErrString[Number]) + 1;
        }
        else
        {
          Tmp  =  10;
        }
        if ((Length > Tmp) || (Length == -1))
        {
          Length  =  Tmp;
        }
        pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,(DATA32)Length);
      }
      if (pDestination != NULL)
      {
        *pDestination   =  0;

        if (Number)
        {
          if (Number < ERRORS)
          {
            snprintf((char*)pDestination,Length,"%s",ErrString[Number]);
          }
          else
          {
            snprintf((char*)pDestination,Length,"Number %02d",Number);
          }
        }
      }
    }
    break;

    case GET_VOLUME :
    {
      *(DATA8*)PrimParPointer()  =  GetVolumePercent();
    }
    break;

    case SET_VOLUME :
    {
      SetVolumePercent(*(DATA8*)PrimParPointer());
    }
    break;

    case GET_MINUTES :
    {
      *(DATA8*)PrimParPointer()  =  GetSleepMinutes();
    }
    break;

    case SET_MINUTES :
    {
      SetSleepMinutes(*(DATA8*)PrimParPointer());
    }
    break;

  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opSTRINGS (CMD, ....)  </b>
 *
 *- String function entry\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD     - \ref stringsubcode
 *
 *\n
 *  - CMD = GET_SIZE
 *\n  Get size of string (not including zero termination)\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \return (DATA16)  SIZE         - Size\n
 *
 *\n
 *  - CMD = ADD
 *\n  Add two strings (SOURCE1 + SOURCE2 -> DESTINATION)\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \param  (DATA8)   SOURCE2      - String variable or handle to string\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = COMPARE
 *\n  Compare two strings\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \param  (DATA8)   SOURCE2      - String variable or handle to string\n
 *    -  \return (DATA8)   RESULT       - Result (0 = not equal, 1 = equal)\n
 *
 *\n
 *  - CMD = DUPLICATE
 *\n  Duplicate a string (SOURCE1 -> DESTINATION)\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = VALUE_TO_STRING
 *\n  Convert floating point value to a string (strips trailing zeroes)\n
 *    -  \param  (DATAF)   VALUE        - Value to write (if "nan" op to 4 dashes is returned: "----")\n
 *    -  \param  (DATA8)   FIGURES      - Total number of figures inclusive decimal point (FIGURES < 0 -> Left adjusted)\n
 *    -  \param  (DATA8)   DECIMALS     - Number of decimals\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = NUMBER_TO_STRING
 *\n  Convert integer value to a string\n
 *    -  \param  (DATA16)  VALUE        - Value to write\n
 *    -  \param  (DATA8)   FIGURES      - Total number of figures\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = STRING_TO_VALUE
 *\n  Convert string to floating point value\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \return (DATAF)   VALUE        - Value\n
 *
 *\n
 *  - CMD = STRIP
 *\n  Strip a string for spaces (SOURCE1 -> DESTINATION)\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = SUB
 *\n  Return DESTINATION: a substring from SOURCE1 that starts were SOURCE2 ends\n
 *    -  \param  (DATA8)   SOURCE1      - String variable or handle to string\n
 *    -  \param  (DATA8)   SOURCE2      - String variable or handle to string\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = VALUE_FORMATTED
 *\n  Convert floating point value to a formatted string\n
 *    -  \param  (DATAF)   VALUE        - Value to write\n
 *    -  \param  (DATA8)   FORMAT       - Format string variable or handle to string\n
 *    -  \param  (DATA8)   SIZE         - Total size of destination string\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *  - CMD = NUMBER_FORMATTED
 *\n  Convert integer number to a formatted string\n
 *    -  \param  (DATA32)  NUMBER       - Number to write\n
 *    -  \param  (DATA8)   FORMAT       - Format string variable or handle to string\n
 *    -  \param  (DATA8)   SIZE         - Total size of destination string\n
 *    -  \return (DATA8)   DESTINATION  - String variable or handle to string\n
 *
 *\n
 *
 *\ref stringexample1 "Program Example"
 */
/*! \brief  opSTRINGS byte code
 *
 */
void      Strings(void)
{
  DATA8   Cmd;
  DATA8   *pSource1;
  DATA8   *pSource2;
  DATA8   *pDestination;
  DATAF   DataF;
  DATA16  Data16;
  DATA32  Data32;
  DATA32  Start;
  DATA8   Tmp;
  DATA8   Figures;
  DATA8   Decimals;
  char    Buffer[1024];

  Cmd           =  *(DATA8*)PrimParPointer();
  switch (Cmd)
  { // Function

    case GET_SIZE :
    {
      pSource1                    =  (DATA8*)PrimParPointer();
      *(DATA16*)PrimParPointer()  =  (DATA16)strlen((char*)pSource1);
    }
    break;

    case ADD :
    {
      pSource1                    =  (DATA8*)PrimParPointer();
      pSource2                    =  (DATA8*)PrimParPointer();
      pDestination                =  (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        Data32        =  (DATA32)strlen((char*)pSource1);
        Data32       +=  (DATA32)strlen((char*)pSource2);
        Data32       +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
      }
      if (pDestination != NULL)
      {
        sprintf((char*)pDestination,"%s%s",(char*)pSource1,(char*)pSource2);
      }
    }
    break;

    case COMPARE :
    {
      pSource1                    =  (DATA8*)PrimParPointer();
      pSource2                    =  (DATA8*)PrimParPointer();

      if (strcmp((char*)pSource1,(char*)pSource2) == 0)
      {
        *(DATA8*)PrimParPointer()  =  1;
      }
      else
      {
        *(DATA8*)PrimParPointer()  =  0;
      }
    }
    break;

    case DUPLICATE :
    {
      pSource1                    =  (DATA8*)PrimParPointer();
      pDestination                =  (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        Data32        =  (DATA32)strlen((char*)pSource1);
        Data32       +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
      }
      if (pDestination != NULL)
      {
        strcpy((char*)pDestination,(char*)pSource1);
      }
    }
    break;

    case VALUE_TO_STRING :
    {
      DataF         =  *(DATAF*)PrimParPointer();
      Figures       =  *(DATA8*)PrimParPointer();
      Decimals      =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        if (Figures >= 0)
        {
          Data32        =  (DATA32)Figures;
        }
        else
        {
          Data32        =  (DATA32)(0 - Figures);
        }
        Data32       +=  2;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
      }
      if (pDestination != NULL)
      {
        if (isnan(DataF))
        {
          if (Figures < 0)
          { // "----    "
            Figures  =  0 - Figures;

            Tmp  =  0;
            while ((Tmp < 4) && (Tmp < Figures))
            {
              pDestination[Tmp]  =  '-';
              Tmp++;
            }
            while (Tmp < Figures)
            {
              pDestination[Tmp]  =  ' ';
              Tmp++;
            }
          }
          else
          { // "    ----"

            Tmp  =  0;
            while (Tmp < (Figures - 4))
            {
              pDestination[Tmp]  =  ' ';
              Tmp++;
            }
            while (Tmp < Figures)
            {
              pDestination[Tmp]  =  '-';
              Tmp++;
            }
          }
          pDestination[Tmp]  =  0;
        }
        else
        {
          if (Figures >= 0)
          {
            snprintf((char*)pDestination,Figures + 1,"%*.*f",Figures,Decimals,DataF);
          }
          else
          {
            Figures  =  0 - Figures;
            snprintf((char*)pDestination,Figures + 1,"%.*f",Decimals,DataF);
          }
          if (Decimals)
          { // Strip trailing zeroes

            Figures  =  (DATA8)strlen((char*)pDestination);

            while ((Figures) && ((pDestination[Figures] == '0') || (pDestination[Figures] == 0)))
            {
              pDestination[Figures]  =  0;
              Figures--;
            }
            if (pDestination[Figures] == '.')
            {
              pDestination[Figures]  =  0;
            }
          }
        }
      }
    }
    break;

    case NUMBER_TO_STRING :
    {
      Data16        =  *(DATA16*)PrimParPointer();
      Figures       =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        Data32        =  (DATA32)Figures;
        Data32       +=  2;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Figures + 1,"%0*d",Figures,Data16);
      }
    }
    break;

    case STRING_TO_VALUE :
    {
      pSource1    =  (DATA8*)PrimParPointer();

      Data16      =  0;
      while (pSource1[Data16])
      {
        if (pSource1[Data16] == ',')
        {
          pSource1[Data16]  =  '.';
        }
        Data16++;
      }

      sscanf((char*)pSource1,"%f",&DataF);

      *(DATAF*)PrimParPointer()  =  DataF;
    }
    break;

    case STRIP :
    {
      pSource1      =  (DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();
      if (VMInstance.Handle >= 0)
      {
        Data32        =  (DATA32)strlen((char*)pSource1);
        Data32       +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
      }
      if (pDestination != NULL)
      {
        while (*pSource1)
        {
          if ((*pSource1 != ' '))
          {
            *pDestination  =  *pSource1;
            pDestination++;
          }
          pSource1++;
        }
        *pDestination  =  *pSource1;
      }
    }
    break;

    case SUB :
    {
      pSource1                    =  (DATA8*)PrimParPointer();
      pSource2                    =  (DATA8*)PrimParPointer();
      pDestination                =  (DATA8*)PrimParPointer();

      Start                       =  (DATA32)strlen((char*)pSource2);
      if (VMInstance.Handle >= 0)
      {
        Data32        =  (DATA32)strlen((char*)pSource1);
        Data32       +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
      }
      if (pDestination != NULL)
      {
        snprintf(Buffer,1024,"%s",strstr((char*)pSource1,(char*)pSource2));
        sprintf((char*)pDestination,"%s",&Buffer[(DATA16)Start]);
      }
    }
    break;

    case VALUE_FORMATTED :
    {
      DataF         =  *(DATAF*)PrimParPointer();
      pSource1      =  (DATA8*)PrimParPointer();
      Figures       =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      snprintf(Buffer,1024,(char*)pSource1,DataF);
      if (VMInstance.Handle >= 0)
      {
        Data32      =  (DATA32)strlen((char*)Buffer);
        Data32     +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
        Figures     =  (DATA8)Data32;
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Figures,"%s",Buffer);
      }

    }
    break;

    case NUMBER_FORMATTED :
    {
      Data32        =  *(DATA32*)PrimParPointer();
      pSource1      =  (DATA8*)PrimParPointer();
      Figures       =  *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      snprintf(Buffer,1024,(char*)pSource1,Data32);
      if (VMInstance.Handle >= 0)
      {
        Data32      =  (DATA32)strlen((char*)Buffer);
        Data32     +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
        Figures     =  (DATA8)Data32;
      }
      if (pDestination != NULL)
      {
        snprintf((char*)pDestination,Figures,"%s",Buffer);
      }

    }
    break;

  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opMEMORY_WRITE (PRGID, OBJID, OFFSET, SIZE, ARRAY)</b>
 *
 *- Write VM memory\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  PRGID   - Program slot number (must be running) (see \ref prgid)
 *  \param  (DATA16)  OBJID   - Object id (zero means globals)
 *  \param  (DATA32)  OFFSET  - Offset (start from)
 *  \param  (DATA32)  SIZE    - Size (length of array to write)
 *  \param  (DATA8)   ARRAY   - First element of DATA8 array to write\n
 *
 *\ref opMEMORY_WRITE1 "Direct command example"
 */
/*! \brief    opMEMORY_WRITE byte code
 *
 *            Write to  VM memory
 *
 *            Uses following from current program context:
 *            None
 *
 */
void      MemoryWrite(void)
{
  PRGID   PrgId;
  OBJID   ObjId;
  GBINDEX Offset;
  GBINDEX Size;
  DATA8   *pArray;
  VARDATA *Ram;
  GBINDEX Tmp;
  GBINDEX Lng;

  PrgId       =  *(PRGID*)PrimParPointer();
  ObjId       =  *(OBJID*)PrimParPointer();
  Offset      =  *(GBINDEX*)PrimParPointer();
  Size        =  *(GBINDEX*)PrimParPointer();
  pArray      =   (DATA8*)PrimParPointer();

//  printf("p=%-1d o=%-2d f=%-4d s=%-4d d=0x%02X\r\n",PrgId,ObjId,Offset,Size,*pArray);

  if (PrgId < MAX_PROGRAMS)
  {
    if (VMInstance.Program[PrgId].Status != STOPPED)
    {
      if ((ObjId >= 0) && (ObjId <= VMInstance.Program[PrgId].Objects))
      {
        if (ObjId == 0)
        {
          Ram  =  VMInstance.Program[PrgId].pGlobal;
          Lng  =  (*(IMGHEAD*)VMInstance.Program[PrgId].pImage).GlobalBytes;
        }
        else
        {
          Ram  =  (*VMInstance.Program[PrgId].pObjList[ObjId]).pLocal;
          Lng  =  VMInstance.Program[PrgId].pObjHead[ObjId].LocalBytes;
        }

        for (Tmp = 0;Tmp < Size;Tmp++)
        {
          if ((Tmp + Offset) < Lng)
          {
            Ram[Tmp + Offset]  =  pArray[Tmp];
          }
        }
      }
    }
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opMEMORY_READ (PRGID, OBJID, OFFSET, SIZE, ARRAY)</b>
 *
 *- Read VM memory\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA16)  PRGID   - Program slot number (must be running) (see \ref prgid)
 *  \param  (DATA16)  OBJID   - Object id (zero means globals)
 *  \param  (DATA32)  OFFSET  - Offset (start from)
 *  \param  (DATA32)  SIZE    - Size (length of array to read)
 *  \return (DATA8)   ARRAY   - First element of DATA8 array to receive data\n
 *
 *  \ref opMEMORY_READ1 "Direct command example"
 */
/*! \brief    opMEMORY_READ byte code
 *
 *            Read VM memory
 *
 *            Uses following from current program context:
 *            None
 *
 */
void      MemoryRead(void)
{
  PRGID   PrgId;
  OBJID   ObjId;
  GBINDEX Offset;
  GBINDEX Size;
  DATA8   *pArray;
  VARDATA *Ram;
  GBINDEX Tmp;
  GBINDEX Lng;

  PrgId       =  *(PRGID*)PrimParPointer();
  ObjId       =  *(OBJID*)PrimParPointer();
  Offset      =  *(GBINDEX*)PrimParPointer();
  Size        =  *(GBINDEX*)PrimParPointer();
  pArray      =   (DATA8*)PrimParPointer();

  if (PrgId < MAX_PROGRAMS)
  {
    if (VMInstance.Program[PrgId].Status != STOPPED)
    {
      if ((ObjId >= 0) && (ObjId <= VMInstance.Program[PrgId].Objects))
      {
        if (ObjId == 0)
        {
          Ram  =  VMInstance.Program[PrgId].pGlobal;
          Lng  =  (*(IMGHEAD*)VMInstance.Program[PrgId].pImage).GlobalBytes;
        }
        else
        {
          Ram  =  (*VMInstance.Program[PrgId].pObjList[ObjId]).pLocal;
          Lng  =  VMInstance.Program[PrgId].pObjHead[ObjId].LocalBytes;
        }

        for (Tmp = 0;Tmp < Size;Tmp++)
        {
          if ((Tmp + Offset) < Lng)
          {
            pArray[Tmp]  =  Ram[Tmp + Offset];
          }
          else
          {
            pArray[Tmp]  =  0;
          }
        }
      }
    }
  }
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opPORT_CNV_OUTPUT (PortIn, Layer, Bitfield, Inverted)</b>
 *
 *- Convert encoded port to Layer and Bitfield\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  PortIn   - EncodedPortNumber
 *  \return (DATA8)   Layer    - Layer
 *  \return (DATA8)   Bitfield - Bitfield
 *  \return (DATA8)   Inverted - True if left/right motor are inverted (ie, C&A)
 *
 */
void PortCnvOutput(void)
{
  DATA32 Ports = *(DATA32*)PrimParPointer();
  DATA32 inputPorts = Ports;
  DATA8 secondPortBitfield = (DATA8) 1 << ((inputPorts % 10) - 1);
  inputPorts /= 10;
  DATA8 firstPortBitfield = (DATA8) 1 << ((inputPorts % 10) - 1);
  inputPorts /= 10;
  DATA8 layer = (DATA8)(inputPorts % 10) - 1;
  if (layer < 0)
  {
    layer = 0;
  }
  DATA8 bitfield = firstPortBitfield | secondPortBitfield;
  *(DATA8*)PrimParPointer() = layer;
  *(DATA8*)PrimParPointer() = bitfield;
  *(DATA8*)PrimParPointer() = firstPortBitfield > secondPortBitfield;
}


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opPORT_CNV_INPUT (PortIn, Layer, PortOut)</b>
 *
 *- Convert encoded port to Layer and Port\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA32)  PortIn   - EncodedPortNumber
 *  \return (DATA8)   Layer - Layer
 *  \return (DATA8)   PortOut - 0-index port for use with VM commands
 *
 */
void PortCnvInput(void)
{
  DATA32 inputPorts = *(DATA32*)PrimParPointer();
  DATA8 port = (DATA8) (inputPorts % 10) - 1;
  inputPorts /= 100;
  DATA8 layer = (DATA8)(inputPorts % 10) - 1;
  if (layer < 0)
  {
    layer = 0;
  }
  *(DATA8*)PrimParPointer() = layer;
  *(DATA8*)PrimParPointer() = port;
}


typedef   struct
{
  DATA8   Note[4];
  DATA16  Freq;
}
NOTEFREQ;

#define   NOTES     36

NOTEFREQ  NoteFreq[NOTES] =
{
  { "C4"  ,  262 },
  { "D4"  ,  294 },
  { "E4"  ,  330 },
  { "F4"  ,  349 },
  { "G4"  ,  392 },
  { "A4"  ,  440 },
  { "B4"  ,  494 },
  { "C5"  ,  523 },
  { "D5"  ,  587 },
  { "E5"  ,  659 },
  { "F5"  ,  698 },
  { "G5"  ,  784 },
  { "A5"  ,  880 },
  { "B5"  ,  988 },
  { "C6"  , 1047 },
  { "D6"  , 1175 },
  { "E6"  , 1319 },
  { "F6"  , 1397 },
  { "G6"  , 1568 },
  { "A6"  , 1760 },
  { "B6"  , 1976 },

  { "C#4" ,  277 },
  { "D#4" ,  311 },

  { "F#4" ,  370 },
  { "G#4" ,  415 },
  { "A#4" ,  466 },

  { "C#5" ,  554 },
  { "D#5" ,  622 },

  { "F#5" ,  740 },
  { "G#5" ,  831 },
  { "A#5" ,  932 },

  { "C#6" , 1109 },
  { "D#6" , 1245 },

  { "F#6" , 1480 },
  { "G#6" , 1661 },
  { "A#6" , 1865 }
};


/*! \page VM
 *  <hr size="1"/>
 *  <b>     opNOTE_TO_FREQ (NOTE, FREQ)</b>
 *
 *- Convert note to tone\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   NOTE     - Note string (HND) (e.c. "C#4")
 *  \return (DATA16)  FREQ     - Frequency [Hz]
 *
 */
void NoteToFreq(void)
{
  DATA8   *pNote;
  DATA16  Freq;
  DATA8   Note;

  Freq  =  440;

  pNote  =  (DATA8*)PrimParPointer();

  for (Note = 0;Note < NOTES;Note++)
  {
    if (strcmp((char*)NoteFreq[Note].Note,(char*)pNote) == 0)
    {
      Freq  =  NoteFreq[Note].Freq;
    }
  }

  *(DATA16*)PrimParPointer()  =  Freq;
}

#ifndef DISABLE_SYSTEM_BYTECODE
/*! \page VM
 *  <hr size="1"/>
 *  <b>     opSYSTEM(COMMAND, STATUS)</b>
 *
 *- Executes a system command\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   COMMAND  - Command string (HND)
 *  \return (DATA32)  STATUS   - Return status of the command
 *
 */
#endif
void  System(void)
{
  DATA32  Status = -1;
  DATA8   *pCmd;

  pCmd    =  (DATA8*)PrimParPointer();

#ifndef DISABLE_SYSTEM_BYTECODE
  Status  =  (DATA32)system((char*)pCmd);
#endif
  sync();

  *(DATA32*)PrimParPointer()  =  Status;
}


//*****************************************************************************

void      Monitor(void)
{
  static  ULONG SavedPriority;
  IMINDEX Index;
  LBINDEX Lv,Ln;
  OBJID   ObjId;
  OBJID   OwnerObjId;
  LP      pObjectLocal;
  char    ObjStat;
  DATA8   Esc;


  if (VMInstance.Debug == 1)
  {
    SavedPriority  =  VMInstance.Priority;

    snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"\r\n         %d %2d IP=%08lX -> ",VMInstance.ProgramId,VMInstance.ObjectId,(unsigned long)VMInstance.ObjectIp);
    VmPrint(VMInstance.PrintBuffer);

    Ln  =  16;

    for (Lv = 0;Lv < Ln;Lv++)
    {
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"%02X ",VMInstance.ObjectIp[Lv] & 0xFF);
      VmPrint(VMInstance.PrintBuffer);
      if (((Lv & 0x3) == 0x3) && ((Lv & 0xF) != 0xF))
      {
        VmPrint(" ");
      }
      if (((Lv & 0xF) == 0xF) && (Lv < (Ln - 1)))
      {
        VmPrint("\r\n");
      }
    }
    VmPrint("\r\n");

    snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"              GV=%08lX -> ",(unsigned long)VMInstance.pGlobal);
    VmPrint(VMInstance.PrintBuffer);

    Ln  =  (*(IMGHEAD*)VMInstance.pImage).GlobalBytes;

    for (Lv = 0;Lv < Ln;Lv++)
    {
      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"%02X ",VMInstance.pGlobal[Lv] & 0xFF);
      VmPrint(VMInstance.PrintBuffer);
      if (((Lv & 0x3) == 0x3) && ((Lv & 0xF) != 0xF))
      {
        VmPrint(" ");
      }
      if (((Lv & 0xF) == 0xF) && (Lv < (Ln - 1)))
      {
        VmPrint("\r\n                             ");
      }
    }
    VmPrint("\r\n");

    for (ObjId = 1;ObjId <= VMInstance.Objects;ObjId++)
    {
      switch ((*VMInstance.pObjList[ObjId]).ObjStatus)
      {
        case RUNNING :
        {
          ObjStat  =  'r';
        }
        break;

        case WAITING :
        {
          ObjStat  =  'w';
        }
        break;

        case STOPPED :
        {
          ObjStat  =  's';
        }
        break;

        case HALTED :
        {
          ObjStat  =  'h';
        }
        break;

        default :
        {
          ObjStat  =  '?';
        }
        break;

      }
      if (ObjId == VMInstance.ObjectId)
      {
        ObjStat  =  '>';
      }

      OwnerObjId      =  VMInstance.pObjHead[ObjId].OwnerObjectId;
      pObjectLocal    =  (*VMInstance.pObjList[ObjId]).pLocal;

      if (OwnerObjId)
      { // Reuse locals from owner

        Ln                       =  VMInstance.pObjHead[OwnerObjId].LocalBytes;

      }
      else
      { // Use local allocated to object

        Ln                       =  VMInstance.pObjHead[ObjId].LocalBytes;
      }



      snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"         %c %2d LV=%08lX -> ",ObjStat,ObjId,(unsigned long)pObjectLocal);
      VmPrint(VMInstance.PrintBuffer);

      for (Lv = 0;Lv < Ln;Lv++)
      {
        snprintf(VMInstance.PrintBuffer,PRINTBUFFERSIZE,"%02X ",pObjectLocal[Lv] & 0xFF);
        VmPrint(VMInstance.PrintBuffer);
        if (((Lv & 0x3) == 0x3) && ((Lv & 0xF) != 0xF))
        {
          VmPrint(" ");
        }
        if (((Lv & 0xF) == 0xF) && (Lv < (Ln - 1)))
        {
          VmPrint("\r\n                             ");
        }
      }
      VmPrint("\r\n");

    }

    VmPrint("\r\n");

    Index  =  (IMINDEX)VMInstance.ObjectIp - (IMINDEX)VMInstance.pImage;
    cValidateDisassemble(VMInstance.pImage,&Index,VMInstance.Program[VMInstance.ProgramId].Label);
    VMInstance.Debug++;
    cUiEscape();
  }
  if (VMInstance.Debug == 2)
  {
    Esc  =  cUiEscape();
    switch (Esc)
    {
      case ' ' :
      {
        VMInstance.Priority  =  SavedPriority;
        VMInstance.Priority--;
        PrimDispatchTabel[*(VMInstance.ObjectIp++)]();
        VMInstance.Debug--;
      }
      break;

      case '<' :
      {
        VMInstance.Priority  =  SavedPriority;
        VMInstance.Priority--;
        PrimDispatchTabel[*(VMInstance.ObjectIp++)]();
        VMInstance.Debug     =  0;
      }
      break;

      default :
      {
        VMInstance.Priority  =  0;
      }
      break;

    }
  }
}


RESULT    TstOpen(UWORD Time)
{
  RESULT  Result = FAIL;
  int     File;

  if ((Time > 0) && (Time <= 30000))
  {
    if (VMInstance.Test == 0)
    {
      File        =  open(TEST_PIN_DEVICE_NAME,O_RDWR | O_SYNC);
      if (File >= MIN_HANDLE)
      {
        ioctl(File,TST_PIN_ON,NULL);
        close(File);

        File        =  open(TEST_UART_DEVICE_NAME,O_RDWR | O_SYNC);
        if (File >= MIN_HANDLE)
        {
          ioctl(File,TST_UART_ON,NULL);
          close(File);
          VMInstance.Test   =  Time;
          Result            =  OK;
        }
      }
    }
    else
    {
      VMInstance.Test     =  Time;
      Result              =  OK;
    }
  }

  return (Result);
}


void      TstClose(void)
{
  int     File;

  if (VMInstance.Test != 0)
  {
    File        =  open(TEST_UART_DEVICE_NAME,O_RDWR | O_SYNC);
    if (File >= MIN_HANDLE)
    {
      ioctl(File,TST_UART_OFF,NULL);
      close(File);

      File        =  open(TEST_PIN_DEVICE_NAME,O_RDWR | O_SYNC);
      if (File >= MIN_HANDLE)
      {
        ioctl(File,TST_PIN_OFF,NULL);
        close(File);
        VMInstance.Test   =  0;
      }
    }
  }
}


/*! \page TST
 *  <hr size="1"/>
 *  <b>     opTST (CMD, ....)  </b>
 *
 *- System test functions entry\n
 *- This set of commands are for test only as they change behaviour in some driver
 *  modules.
 *  When test is open every command keeps the test byte codes enabled for 10 seconds
 *\n
 *
 *  \param  (DATA8)   CMD     - \ref tstsubcode
 *
 *\n
 *  - CMD = TST_OPEN
 *\n  Enables test byte codes for 10 seconds\n
 *
 *\n
 *  - CMD = TST_CLOSE
 *\n  Disables test byte codes\n
 *
 *\n
 *  - CMD = TST_READ_PINS
 *\n  Read connector pin status\n
 *    -  \param  (DATA8)   PORT         - Input connector [0..3], output connector [16..19]\n
 *    -  \param  (DATA8)   LENGTH       - Number of pins in returned string\n
 *    -  \return (DATA8)   STRING       - String variable start index ('0' = low, '1' = high)\n
 *\n
 *\n
 *    Input\n
 *    --------\n
 *    STRING[0]     Pin1   I_ONx    current source control output ['0','1']\n
 *    STRING[1]     Pin2   LEGDETx  ['0','1']\n
 *    STRING[2]     Pin5   DIGIx0   ['0','1']\n
 *    STRING[3]     Pin6   DIGIx1   ['0','1']\n
 *    STRING[4]     -      TXINx_EN ['0','1']\n
 *\n
 *    Output\n
 *    --------\n
 *    STRING[0]     Pin1   MxIN0    ['0','1']\n
 *    STRING[1]     Pin2   MxIN1    ['0','1']\n
 *    STRING[2]     Pin5   DETx0    ['0','1']\n
 *    STRING[3]     Pin5   INTx0    ['0','1']\n
 *    STRING[4]     Pin6   DIRx     ['0','1']\n
 *
 *
 *\n
 *  - CMD = TST_WRITE_PINS
 *\n  Write to connector pin\n
 *    -  \param  (DATA8)   PORT         - Input connector [0..3], output connector [16..19]\n
 *    -  \param  (DATA8)   LENGTH       - Number of pins to write\n
 *    -  \return (DATA8)   STRING       - String variable start index ('0' = set low, '1' = set high, 'X' = tristate, '-' = don't touch)\n
 *\n
 *\n
 *    Input\n
 *    --------\n
 *    STRING[0]     Pin1   I_ONx    current source control output ['0','1','X','-']\n
 *    STRING[1]     Pin2   LEGDETx  ['0','1','X','-']\n
 *    STRING[2]     Pin5   DIGIx0   ['0','1','X','-']\n
 *    STRING[3]     Pin6   DIGIx1   ['0','1','X','-']\n
 *    STRING[4]     -      TXINx_EN ['0','1','X','-']\n
 *\n
 *    Output\n
 *    --------\n
 *    STRING[0]     Pin1   MxIN0    ['0','1','X','-']\n
 *    STRING[1]     Pin2   MxIN1    ['0','1','X','-']\n
 *    STRING[2]     Pin5   DETx0    Write   ['0','1','X','-']\n
 *    STRING[3]     Pin5   INTx0    Read    ['0','1','X','-']\n
 *    STRING[4]     Pin6   DIRx     ['0','1','X','-']\n
 *
 *
 *\n
 *  - CMD = TST_READ_ADC
 *\n  Read raw count from ADC\n
 *    -  \param  (DATA8)   INDEX        - Input mapped index (see below) [0..15]\n
 *    -  \return (DATA16)  VALUE        - Raw count [0..4095]\n
 *\n
 *\n
 *    INDEX 0..3    Input  connector pin 1 ( 0=conn1,  1=conn2,  2=conn3,  3=conn4)\n
 *    INDEX 4..7    Input  connector pin 6 ( 4=conn1,  5=conn2,  6=conn3,  7=conn4)\n
 *    INDEX 8..11   Output connector pin 5 ( 8=conn1,  9=conn2, 10=conn3, 11=conn4)\n
 *\n
 *    INDEX 12      Battery temperature\n
 *    INDEX 13      Current flowing to motors\n
 *    INDEX 14      Current flowing from the battery\n
 *    INDEX 15      Voltage at battery cell 1, 2, 3,4, 5, and 6\n
 *
 *
 *\n
 *  - CMD = TST_ENABLE_UART
 *\n  Enable all UARTs\n
 *    -  \param  (DATA32)  BITRATE      - Bit rate [2400..115200 b/S]\n
 *
 *
 *\n
 *  - CMD = TST_WRITE_UART
 *\n  Write data to port through UART\n
 *    -  \param  (DATA8)   PORT         - Input connector [0..3]\n
 *    -  \param  (DATA8)   LENGTH       - Length of string to write [0..63]\n
 *    -  \param  (DATA8)   STRING       - String of data\n
 *
 *
 *\n
 *  - CMD = TST_READ_UART
 *\n  Read data from port through UART\n
 *    -  \param  (DATA8)   PORT         - Input connector [0..3]\n
 *    -  \param  (DATA8)   LENGTH       - Length of string to read [0..63]\n
 *    -  \param  (DATA8)   STRING       - String of data\n
 *
 *
 *\n
 *  - CMD = TST_DISABLE_UART
 *\n  Disable all UARTs\n
 *
 *
 *\n
 *  - CMD = TST_ACCU_SWITCH
 *\n  Read accu switch state\n
 *    -  \return (DATA8)   ACTIVE       - State [0..1]\n
 *
 *
 *\n
 *  - CMD = TST_BOOT_MODE"
 *\n  Turn on mode2\n
 *
 *
 *\n
 *  - CMD = TST_POLL_MODE2
 *\n  Read mode2 status\n
 *    -  \return (DATA8)   STATUS       - State [0..2]\n
 *
 *
 *\n
 *  - CMD = TST_CLOSE_MODE2
 *\n  Closes mode2\n
 *
 *
 *
 *\n
 *  - CMD = TST_RAM_CHECK
 *\n  Read RAM test status status\n
 *    -  \return (DATA8)   STATUS       - State [0,1]  0 = FAIL, 1 = SUCCESS\n
 *
 *
 *\n
 *
 *  <hr size="1"/>
 *  <b>Direct test command examples:</b>
 *\verbatim
*

--------------------------------------
Set DIGIBI0, low
--------------------------------------
              v
12000000000000FF0AFF0D0105842D2D302D2D00

0300000002

--------------------------------------
Set DIGIBI0, high
--------------------------------------
              v
12000000000000FF0AFF0D0105842D2D312D2D00

0300000002

--------------------------------------
Set DIGIBI0, float
--------------------------------------
              v
12000000000000FF0AFF0D0105842D2D582D2D00

0300000002

--------------------------------------
Read ADC value, Port 2, Pin 6
--------------------------------------
              v
0B000000000200FF0AFF0E0560

0500000002xxxx
xxxx  = A/D count [0..4095]

--------------------------------------
Activate 9V, Input Port 2
--------------------------------------
              v
12000000000000FF0AFF0D010584312D2D2D2D00

0300000002

--------------------------------------
Read ADC value, Port 2, Pin 1
--------------------------------------
              v
0B000000000200FF0AFF0E0160

0500000002xxxx
xxxx  = A/D count [0..4095]

--------------------------------------
Deactivate 9V, Input Port 2
--------------------------------------
              v
12000000000000FF0AFF0D010584302D2D2D2D00

0300000002

--------------------------------------
Transmit and receive "UUUUU" on Port 2
--------------------------------------
              v
20000000000500FF0AFF118300C20100FF0F010584555555555500FF10010560FF12

0800000002xxxxxxxxxx
xxxxxxxxxx = 5555555555 = "UUUUU"

--------------------------------------
Read rechargeable battery availability
--------------------------------------
              v
0A000000000100FF0AFF1360

0400000002xx
xx    = SW1     xx = 00 not active, xx = 01 active


--------------------------------------
Check for mode2
--------------------------------------

Initiate mode2:
              v
09000000000000FF0AFF14

0300000002


Poll mode2 status:
              v
0A000000000100FF0AFF1560

0400000002xx

xx = Mode2 status  xx = 00 success, xx = 01 in progress, xx = 02 error


Close mode2:
              v
09000000000000FF0AFF16

0300000002

--------------------------------------
RAM Check poll
--------------------------------------

Poll RAM check:
              v
0A000000000100FF0AFF1760

0400000002xx

xx = RAM check  xx = 00 OK, xx = 02 FAIL


****************************************************************
**** OTHER FUNCTIONS WITH NORMAL BYTE CODES ********************
****************************************************************

--------------------------------------
Read UI Button status
--------------------------------------
              v
1D000000000600830901608309026183090362830904638309056483090665

0900000002uueeddrrllbb
uu    = up      uu = 00 not active, uu = 01 active
ee    = enter   ee = 00 not active, ee = 01 active
dd    = down    dd = 00 not active, dd = 01 active
rr    = right   rr = 00 not active, rr = 01 active
ll    = left    ll = 00 not active, ll = 01 active
bb    = back    bb = 00 not active, bb = 01 active

--------------------------------------
Tone 1 KHz, forever
--------------------------------------
              v
0D0000000000009401816482E80300

0300000002

--------------------------------------
Stop Sound output
--------------------------------------
              v
070000000000009400

0300000002

--------------------------------------
Show Chess pattern 1 (start with black)
--------------------------------------
              v
160000000000008412008413000000841382550000008400

0300000002

--------------------------------------
Show Chess pattern 1 (start with white)
--------------------------------------
              v
160000000000008412008413000000841382AA0000008400

0300000002

--------------------------------------
Clear display again
--------------------------------------
              v
0C00000000000084130000008400

0300000002

--------------------------------------
Check for SD card
--------------------------------------
              v
0A000000000900811E686064

0C00000002ttttttttffffffffpp
tttttttt = total size
ffffffff = free space
pp       = present [00 = no, 01 = yes]

--------------------------------------
Bluetooth on and visible
--------------------------------------
              v
13000000000000D4010201D00200D4020201D00200

0300000002

--------------------------------------
Bluetooth off
--------------------------------------
              v
0C000000000000D4010200D00200

0300000002

*\endverbatim
 *
 *
 */
/*! \brief  opTST byte code
 *
 */
void      Tst(void)
{
  DATA8   Cmd;
  DATA8   *pPins;
  DATA8   Index;
  DATA8   Data8;
  DATA16  Value;
  DATA8   Buffer[2];
  TSTPIN  Tstpin;
  TSTUART Tstuart;
  int     File;

  Cmd  =  *(DATA8*)PrimParPointer();

  if (Cmd == TST_OPEN)
  { // Test open

    TstOpen(10000);
  }
  else
  {
    if (VMInstance.Test)
    {
      TstOpen(10000);

      switch (Cmd)
      { // Function

        case TST_READ_PINS :
        { // Read pins

          Tstpin.Port     =  *(DATA8*)PrimParPointer();
          Tstpin.Length   =  *(DATA8*)PrimParPointer();
          pPins           =   (DATA8*)PrimParPointer();

          File            =  open(TEST_PIN_DEVICE_NAME,O_RDWR | O_SYNC);
          if (File >= 0)
          {
            ioctl(File,TST_PIN_READ,&Tstpin);
            close(File);
            for (Index = 0;Index < Tstpin.Length;Index++)
            {
              pPins[Index]  =  Tstpin.String[Index];
            }
          }
        }
        break;

        case TST_WRITE_PINS :
        { // Write to pins

          Tstpin.Port     =  *(DATA8*)PrimParPointer();
          Tstpin.Length   =  *(DATA8*)PrimParPointer();
          pPins           =   (DATA8*)PrimParPointer();

          for (Index = 0;Index < Tstpin.Length;Index++)
          {
            Tstpin.String[Index]  =  pPins[Index];
          }
          File        =  open(TEST_PIN_DEVICE_NAME,O_RDWR | O_SYNC);
          if (File >= MIN_HANDLE)
          {
            ioctl(File,TST_PIN_WRITE,&Tstpin);
            close(File);
          }

        }
        break;

        case TST_READ_ADC :
        {
          Index       =  *(DATA8*)PrimParPointer();
          Value       =  DATA16_NAN;

          if ((Index >= 0) && (Index < 4))
          {
            Value  =  (*InputInstance.pAnalog).InPin1[Index];
          }
          if ((Index >= 4) && (Index < 8))
          {
            Value  =  (*InputInstance.pAnalog).InPin6[Index - 4];
          }
          if ((Index >= 8) && (Index < 12))
          {
            Value  =  (*InputInstance.pAnalog).OutPin5[Index - 8];
          }
          if (Index == 12)
          {
            Value  =  (*InputInstance.pAnalog).BatteryTemp;
          }
          if (Index == 13)
          {
            Value  =  (*InputInstance.pAnalog).MotorCurrent;
          }
          if (Index == 14)
          {
            Value  =  (*InputInstance.pAnalog).BatteryCurrent;
          }
          if (Index == 15)
          {
            Value  =  (*InputInstance.pAnalog).Cell123456;
          }

          *(DATA16*)PrimParPointer()  =  Value;
        }
        break;

        case TST_ENABLE_UART :
        {
          Tstuart.Bitrate   =  *(DATA32*)PrimParPointer();
          File              =  open(TEST_UART_DEVICE_NAME,O_RDWR | O_SYNC);
          if (File >= 0)
          {
            ioctl(File,TST_UART_EN,&Tstuart);
            close(File);
          }
        }
        break;

        case TST_DISABLE_UART :
        {
          File              =  open(TEST_UART_DEVICE_NAME,O_RDWR | O_SYNC);
          if (File >= MIN_HANDLE)
          {
            ioctl(File,TST_UART_DIS,&Tstuart);
            close(File);
          }
        }
        break;

        case TST_WRITE_UART :
        {
          Tstuart.Port      =  *(DATA8*)PrimParPointer();
          Tstuart.Length    =  *(DATA8*)PrimParPointer();
          pPins             =   (DATA8*)PrimParPointer();

          if (Tstuart.Port < INPUTS)
          {
            for (Index = 0;(Index < Tstuart.Length) && (Index < TST_UART_LENGTH);Index++)
            {
              Tstuart.String[Index]  =  pPins[Index];
            }
            File        =  open(TEST_UART_DEVICE_NAME,O_RDWR | O_SYNC);
            if (File >= MIN_HANDLE)
            {
              ioctl(File,TST_UART_WRITE,&Tstuart);
              close(File);
            }
          }
        }
        break;

        case TST_READ_UART :
        {
          Tstuart.Port      =  *(DATA8*)PrimParPointer();
          Tstuart.Length    =  *(DATA8*)PrimParPointer();
          pPins             =   (DATA8*)PrimParPointer();

          for (Index = 0;(Index < Tstuart.Length) && (Index < TST_UART_LENGTH);Index++)
          {
            Tstuart.String[Index]  =  0;
          }
          if (Tstuart.Port < INPUTS)
          {
            File            =  open(TEST_UART_DEVICE_NAME,O_RDWR | O_SYNC);
            if (File >= MIN_HANDLE)
            {
              ioctl(File,TST_UART_READ,&Tstuart);
              close(File);
            }
          }
          for (Index = 0;(Index < Tstuart.Length) && (Index < TST_UART_LENGTH);Index++)
          {
            pPins[Index]  =  Tstuart.String[Index];
          }
        }
        break;

        case TST_ACCU_SWITCH :
        {
          Data8  =  0;
          if (UiInstance.PowerFile >= MIN_HANDLE)
          {
            read(UiInstance.PowerFile,Buffer,2);
            if (Buffer[0] == '1')
            {
              Data8  =  1;
            }
          }
          *(DATA8*)PrimParPointer()  =  Data8;
        }
        break;

#ifndef Linux_X86
        case TST_BOOT_MODE2:
        {
          I2cStart();
        }
        break;

        case TST_POLL_MODE2:
        {
          Data8 = I2cGetBootStatus();
          *(DATA8*)PrimParPointer() = Data8;
        }
        break;

        case TST_CLOSE_MODE2:
        {
          I2cStop();
        }
        break;
#endif

        case TST_RAM_CHECK:
        {
          ULONG      RamCheckFile;
          UBYTE      RamStatus[2];

          RamCheckFile  =  open(UPDATE_DEVICE_NAME,O_RDWR);
          Data8 = FAIL;
          if (RamCheckFile >= 0)
          {
            read(RamCheckFile,RamStatus,2);
            close(RamCheckFile);

            if ((RamStatus[0] == ((UBYTE)(~(RamStatus[1])))) && (0 == RamStatus[0]))
            {
              Data8 = OK;
            }
          }
          *(DATA8*)PrimParPointer() = Data8;
        }
        break;

        default :
        { // Test close

          TstClose();
        }
        break;

      }
    }
    else
    {
      ProgramEnd(VMInstance.ProgramId);
      VMInstance.Program[VMInstance.ProgramId].Result          =  FAIL;
      SetDispatchStatus(INSTRBREAK);
    }
  }
}

#ifdef BUFPRINTSIZE

#include  <stdarg.h>

void      BufPrint(char Cmd,char *pFormat, ...)
{
  static  char Buf[BUFPRINTSIZE];
  static  ULONG BufIn = 0;
  static  char Full = 0;
  char    Format[16];
  int     FormatIn = 0;
  int     FormatOut = 0;
  char    Par = 0;
  char    Char;
#ifdef DEBUG_D_UART_ERROR
  int     File;
#endif
  va_list pArg;

  va_start(pArg,pFormat);

  switch (Cmd)
  {
    case 'p' :
    {
      if (!Full)
      {
        while ((pFormat[FormatIn]) && (BufIn < (BUFPRINTSIZE - 250)))
        {
          Char  =  pFormat[FormatIn];
          if ((Char != '%') && (Par == 0))
          {
            Buf[BufIn]  =  Char;
            BufIn++;
            FormatIn++;
          }
          else
          {
            Par                 =  1;
            Format[FormatOut]   =  Char;
            FormatOut++;
            FormatIn++;
            Format[FormatOut]   =  0;
            switch (Char)
            {
              case 'c' :
              case 'd' :
              case 'x' :
              case 'X' :
              {
                BufIn  +=  sprintf(&Buf[BufIn],"%-3d",va_arg(pArg, int));
                Par     =  0;
              }
              break;

              case 'f' :
              {
                BufIn  +=  sprintf(&Buf[BufIn],Format,va_arg(pArg, double));
                Par     =  0;
              }
              break;

              case 's' :
              {
                BufIn  +=  sprintf(&Buf[BufIn],Format,va_arg(pArg, char *));
                Par     =  0;
              }
              break;

              default :
              {

              }
              break;

            }
            if (Par == 0)
            {
              FormatOut  =  0;
            }
          }
        }
        if (BufIn >= (BUFPRINTSIZE - 250))
        {
          sprintf(&Buf[BufIn],"\r\nBUFFER FULL\r\n");
        }
      }
    }
    break;

    case 'w' :
    {
      Buf[BufIn]  =  0;
      printf("\r\n------------------------------------------------------------\r\n");
      printf("%s",Buf);
      if ((BufIn > 0) && (Buf[BufIn - 1] != '\n'))
      {
        printf("\r\n");
      }
      printf("------------------------------------------------------------\r\n\n");
      Par         =  0;
      BufIn       =  0;
      FormatOut   =  0;
      Full        =  0;

#ifdef DEBUG_D_UART_ERROR
      Buf[BufIn]  =  0;
      File  =  open(UART_DEVICE_NAME,O_RDONLY);
      if (File >= MIN_HANDLE)
      {
        read(File,Buf,BUFPRINTSIZE);
        printf("%s",Buf);

        close(File);
      }
#endif
    }
    break;

  }

  va_end(pArg);
}

#endif

/*
#define   vmCHARSET_NAME                0x01                          //!< Character set allowed in brick name and raw filenames
#define   vmCHARSET_FILENAME            0x02                          //!< Character set allowed in file names
#define   vmCHARSET_BTPASSKEY           0x04                          //!< Character set allowed in bluetooth pass key
#define   vmCHARSET_WIFIPASSKEY         0x08                          //!< Character set allowed in WiFi pass key
#define   vmCHARSET_WIFISSID            0x10                          //!< Character set allowed in WiFi ssid
*/

DATA8     ValidChars[] =
{
  0x00,   // 0x00      NUL
  0x00,   // 0x01      SOH
  0x00,   // 0x02      STX
  0x00,   // 0x03      ETX
  0x00,   // 0x04      EOT
  0x00,   // 0x05      ENQ
  0x00,   // 0x06      ACK
  0x00,   // 0x07      BEL
  0x00,   // 0x08      BS
  0x00,   // 0x09      TAB
  0x00,   // 0x0A      LF
  0x00,   // 0x0B      VT
  0x00,   // 0x0C      FF
  0x00,   // 0x0D      CR
  0x00,   // 0x0E      SO
  0x00,   // 0x0F      SI
  0x00,   // 0x10      DLE
  0x00,   // 0x11      DC1
  0x00,   // 0x12      DC2
  0x00,   // 0x13      DC3
  0x00,   // 0x14      DC4
  0x00,   // 0x15      NAK
  0x00,   // 0x16      SYN
  0x00,   // 0x17      ETB
  0x00,   // 0x18      CAN
  0x00,   // 0x19      EM
  0x00,   // 0x1A      SUB
  0x00,   // 0x1B      ESC
  0x00,   // 0x1C      FS
  0x00,   // 0x1D      GS
  0x00,   // 0x1E      RS
  0x00,   // 0x1F      US
  0x12,   // 0x20      (space)
  0x00,   // 0x21      !
  0x00,   // 0x22      "
  0x00,   // 0x23      #
  0x00,   // 0x24      $
  0x00,   // 0x25      %
  0x00,   // 0x26      &
  0x00,   // 0x27      '
  0x00,   // 0x28      (
  0x00,   // 0x29      )
  0x00,   // 0x2A      *
  0x00,   // 0x2B      +
  0x00,   // 0x2C      ,
  0x03,   // 0x2D      -
  0x02,   // 0x2E      .
  0x02,   // 0x2F      /
  0x1F,   // 0x30      0
  0x1F,   // 0x31      1
  0x1F,   // 0x32      2
  0x1F,   // 0x33      3
  0x1F,   // 0x34      4
  0x1F,   // 0x35      5
  0x1F,   // 0x36      6
  0x1F,   // 0x37      7
  0x1F,   // 0x38      8
  0x1F,   // 0x39      9
  0x00,   // 0x3A      :
  0x00,   // 0x3B      ;
  0x00,   // 0x3C      <
  0x00,   // 0x3D      =
  0x00,   // 0x3E      >
  0x00,   // 0x3F      ?
  0x00,   // 0x40      @
  0x1F,   // 0x41      A
  0x1F,   // 0x42      B
  0x1F,   // 0x43      C
  0x1F,   // 0x44      D
  0x1F,   // 0x45      E
  0x1F,   // 0x46      F
  0x1F,   // 0x47      G
  0x1F,   // 0x48      H
  0x1F,   // 0x49      I
  0x1F,   // 0x4A      J
  0x1F,   // 0x4B      K
  0x1F,   // 0x4C      L
  0x1F,   // 0x4D      M
  0x1F,   // 0x4E      N
  0x1F,   // 0x4F      O
  0x1F,   // 0x50      P
  0x1F,   // 0x51      Q
  0x1F,   // 0x52      R
  0x1F,   // 0x53      S
  0x1F,   // 0x54      T
  0x1F,   // 0x55      U
  0x1F,   // 0x56      V
  0x1F,   // 0x57      W
  0x1F,   // 0x58      X
  0x1F,   // 0x59      Y
  0x1F,   // 0x5A      Z
  0x00,   // 0x5B      [
  0x00,   // 0x5C      '\'
  0x00,   // 0x5D      ]
  0x00,   // 0x5E      ^
  0x1F,   // 0x5F      _
  0x00,   // 0x60      `
  0x1F,   // 0x61      a
  0x1F,   // 0x62      b
  0x1F,   // 0x63      c
  0x1F,   // 0x64      d
  0x1F,   // 0x65      e
  0x1F,   // 0x66      f
  0x1F,   // 0x67      g
  0x1F,   // 0x68      h
  0x1F,   // 0x69      i
  0x1F,   // 0x6A      j
  0x1F,   // 0x6B      k
  0x1F,   // 0x6C      l
  0x1F,   // 0x6D      m
  0x1F,   // 0x6E      n
  0x1F,   // 0x6F      o
  0x1F,   // 0x70      p
  0x1F,   // 0x71      q
  0x1F,   // 0x72      r
  0x1F,   // 0x73      s
  0x1F,   // 0x74      t
  0x1F,   // 0x75      u
  0x1F,   // 0x76      v
  0x1F,   // 0x77      w
  0x1F,   // 0x78      x
  0x1F,   // 0x79      y
  0x1F,   // 0x7A      z
  0x00,   // 0x7B      {
  0x00,   // 0x7C      |
  0x00,   // 0x7D      }
  0x02,   // 0x7E      ~
  0x00    // 0x7F      
};


RESULT    ValidateChar(DATA8 *pChar,DATA8 Set)
{
  RESULT  Result = OK;

  if ((ValidChars[*pChar] & Set) == 0)
  {
    *pChar  =  '_';
    Result  =  FAIL;
  }

  return (Result);
}


RESULT    ValidateString(DATA8 *pString,DATA8 Set)
{
  RESULT  Result = OK;

  while (*pString)
  {
    Result |=  ValidateChar(pString,Set);
    pString++;
  }

  return (Result);
}
