/*
 * lms2012.h
 *
 *  Created on: Mar 24, 2010
 *      Author: tommy
 */

/*! \page debug Terminal Configuration
 *
    \verbatim


TERMINAL CONFIGURATION (in top of "lms2012.h")

Normal release:                     DEBUG not defined, TERMINAL_ENABLED = 0, MAPPED = 0, DEBUG_UART = 4

Development:                        DEBUG not defined, TERMINAL_ENABLED = 1, MAPPED = 1, DEBUG_UART = 3

Development with debug messages:    DEBUG defined, TERMINAL_ENABLED = 1, MAPPED = 1, DEBUG_UART = 3

*/


//#define   DEBUG                         //!< When defined debug messages is output on standard I/O (d_uart is different)

#define   TERMINAL_ENABLED      0       //!< DEBUG terminal enabled (0 = disabled, 1 = enabled)
#define   MAPPED                0       //!< DEBUG input ports (0 = normal, 1 = mapped: 1 physical = 4 logical, 2 physical = 1 logical, ...)
#define   DEBUG_UART            4       //!< UART used for debug (0 = port1, 1 = port2, ... 4 = none) (mapped 3 = port1 0 = port2)


/*    \endverbatim */

//#define   DEBUG_VM
//#define   DEBUG_TRACE_TASK
//#define   DEBUG_C_COM
//#define   DEBUG_C_INPUT
//#define   DEBUG_C_MEMORY
//#define   DEBUG_C_MEMORY_LOG
//#define   DEBUG_C_MEMORY_FILE
//#define   DEBUG_C_SOUND
//#define   DEBUG_C_UI

//#define   DEBUG_D_ANALOG
//#define   DEBUG_D_POWER
//#define   DEBUG_D_UART
//#define   DEBUG_D_UART_ERROR
//#define   DEBUG_D_UI
//#define   DEBUG_D_SOUND
//#define   DEBUG_D_IIC
//#define   DEBUG_D_USBDEV

//#define   DEBUG_TRACE_USB_COMMAND
//#define   DEBUG_TRACE_MODE_CHANGE
//#define   DEBUG_TRACE_KEY
//#define   DEBUG_TRACE_ANGLE
//#define   DEBUG_TRACE_VM

#ifdef LEGO_SIMULATION
	#pragma warning(disable:4996)
	#pragma warning(disable:4200)
	#pragma warning(disable:4101)
	#pragma warning(disable:4244)
#endif

#ifndef   LMS2012_H_
#define   LMS2012_H_

//  HARDWARE PLATFORM

#define   SIMULATION    0               //!< LEGO digital simulation
#define   FINALB        1               //!< Schematics revision B
#define   FINAL         2               //!< Final prototype
#define   ONE2ONE       3               //!< First real size prototype
#define   A4            4               //!< First prototype
#define   EVALBOARD     5               //!< AM1808 evaluation board

//  FIRMWARE VARIANTS

//#define   REWORKED                      //!< Special to ONE2ONE prototype that has reworked output detection
//#define   LOWSPEED                      //!< USB communication compatible with V0.21
//#define   USB64                         //!< USB report only 64 bytes

//#define   OLDCALL                       //!< Don't use optimised sub calls
//#define   DISABLE_PRU_UARTS             //!< Don't use port 3 and 4 for UART sensors
//#define   DISABLE_OLD_COLOR             //!< Don't support NXT color sensor
//#define   DISABLE_LOW_VOLTAGE           //!< Don't shut down on low voltage
//#define   ADC_BITBANGING                //!< Don't use SPI for a/d converter
#define   DISABLE_UART_DATA_ERROR       //!< Don't reset UART sensor if timeout or crc error
#define   UART1_FAKE_INTERRUPT            //!< Don't use real interrupt on UART1 (linux prompt)
//#define   DISABLE_NEW_FILEHANDLE

#define   SERVICE_SENSOR_WATCHDOG
#define   BUMBED
#define   LOG_TO_RAM
#define   LOG_ASCII


#ifdef LEGO_SIMULATION
  #define HARDWARE SIMULATION
#else

#ifndef   HARDWARE
  #define HARDWARE      FINAL           //!< Actual hardware platform (must be one of above)
#endif

#if (HARDWARE == FINAL)
#ifdef FINALB
#define   HW_VERSION    0.30
#else
#define   HW_VERSION    0.20
#endif
#endif
#if (HARDWARE == ONE2ONE)
#define   HW_VERSION    0.10
#endif
#if (HARDWARE == A4)
#define   HW_VERSION    0.00
#endif

#endif

#ifndef PCASM
#include  <asm/types.h>
#endif

#include  "lmstypes.h"
#include  "bytecodes.h"

// Hardware

#define   OUTPUTS                       vmOUTPUTS                     //!< Number of output ports in the system
#define   INPUTS                        vmINPUTS                      //!< Number of input  ports in the system
#define   BUTTONS                       vmBUTTONS                     //!< Number of buttons in the system
#define   LEDS                          vmLEDS                        //!< Number of LEDs in the system

#define   LCD_WIDTH                     vmLCD_WIDTH                   //!< LCD horizontal pixels
#define   LCD_HEIGHT                    vmLCD_HEIGHT                  //!< LCD vertical pixels
#define   TOPLINE_HEIGHT                vmTOPLINE_HEIGHT              //!< Top line vertical pixels
#define   LCD_STORE_LEVELS              vmLCD_STORE_LEVELS            //!< Store levels

// Software

#define   FG_COLOR                      vmFG_COLOR                    //!<  Forground color
#define   BG_COLOR                      vmBG_COLOR                    //!<  Background color

#define   CHAIN_DEPT                    vmCHAIN_DEPT                  //!< Number of bricks in the USB daisy chain (master + slaves)

#define   EVENT_BT_PIN                  vmEVENT_BT_PIN

// Folders

#define   PROGRAM_FOLDER                vmPROGRAM_FOLDER
#define   DATALOG_FOLDER                vmDATALOG_FOLDER

// Files
#define   DETAILS_FILE                  vmDETAILS_FILE                //!< File containing firmware version

// Extensions

#define   EXT_SOUND                     vmEXT_SOUND                   //!< Rudolf sound file
#define   EXT_GRAPHICS                  vmEXT_GRAPHICS                //!< Rudolf graphics file
#define   EXT_BYTECODE                  vmEXT_BYTECODE                //!< Rudolf byte code file
#define   EXT_TEXT                      vmEXT_TEXT                    //!< Rudolf text file
#define   EXT_DATALOG                   vmEXT_DATALOG                 //!< Rudolf dataog file
#define   EXT_PROGRAM                   vmEXT_PROGRAM                 //!< Rudolf program byte code file

/*! \page system
 *
 *  \verbatim
 */

#define   PROJECT                       "LMS2012"
#define   VERS                          0.50
#define   SPECIALVERS                   ' '


#define   TEMP_SHUTDOWN_HIGH    65.0                  //!< Temperature shutdown upper limit [C]
#define   TEMP_SHUTDOWN_LOW     55.0                  //!< Temperature shutdown lower limit [C]
#define   BATT_INDICATOR_LOW    6000                  //!< Battery indicator low [mV]
#define   BATT_INDICATOR_HIGH   7500                  //!< Battery indicator high [mV]
#define   BATT_SHUTDOWN_HIGH    5.5                   //!< Voltage shutdown upper limit [V]
#define   BATT_SHUTDOWN_LOW     4.5                   //!< Voltage shutdown lower limit [V]
#define   BATT_SHUTDOWN_TIME    5000                  //!< Time from shutdown lower limit to shutdown [mS]

#define   MAX_PROGRAMS          SLOTS                 //!< Max number of programs (including UI and direct commands) running at a time
#define   MAX_BREAKPOINTS       4                     //!< Max number of breakpoints (opCODES depends on this value)
#define   MAX_LABELS            32                    //!< Max number of labels per program
#define   MAX_DEVICE_TYPES      100                   //!< Max number of different device types and modes (type data list size)
#define   MAX_DEVICE_TYPE       127                   //!< Highest type number (positive)
#define   MAX_DEVICE_MODES      8                     //!< Max number of modes in one device
#define   MAX_DEVICE_DATASETS   4                     //!< Max number of data sets in one device
#define   MAX_DEVICE_BUSY_TIME  1000                  //!< Max number of mS a device can be busy when read

#define   MAX_HANDLES           32                    //!< Max number of handles to memory pools and arrays in one program
#define   MAX_ARRAY_SIZE        1000000               //!< Max array size
#define   MIN_ARRAY_ELEMENTS    0                     //!< Min elements in a DATA8 array

#define   LOGBUFFER_SIZE        1000                  //!< Min log buffer size
#define   MIN_LIVE_UPDATE_TIME  100                   //!< [mS] Min sample time when live update

#define   MAX_COMMAND_BYTECODES 64                    //!< Max number of byte codes in a debug terminal direct command
#define   MAX_COMMAND_LOCALS    64                    //!< Max number of bytes allocated for direct command local variables
#define   MAX_COMMAND_GLOBALS   1021                  //!< Max number of bytes allocated for direct command global variables

#define   UI_PRIORITY           20                    //!< UI byte codes before switching VM thread
#define   PRG_PRIORITY          200                   //!< Prg byte codes before switching VM thread

#define   LONG_PRESS_TIME       3000                  //!< [mS] Time pressed before long press recognized

#define   ADC_REF               5000                  //!< [mV]  maximal value on ADC
#define   ADC_RES               4095                  //!< [CNT] maximal count on ADC

#define   IN1_ID_HYSTERESIS     50                    //!< [mV]  half of the span one Id takes up on input connection 1 voltage
#define   OUT5_ID_HYSTERESIS    100                   //!< [mV]  half of the span one Id takes up on output connection 5 voltage

#define   DEVICE_UPDATE_TIME    1000000               //!< Min device (sensor) update time [nS]

#define   MAX_FILE_HANDLES      2                     //!< Max number of down load file handles
#define   MIN_HANDLE            3                     //!< Min file handle to close

#define   ID_LENGTH             7                     //!< Id length  (BT MAC id) (incl. zero terminator)
#define   NAME_LENGTH           12                    //!< Name length (not including zero termination)

#define   ERROR_BUFFER_SIZE     8                     //!< Number of errors in buffer

#define   UARTRECEIVEBUFFERS    5                     //!< Uart 1 & 2 - no of receive buffers

#define   PWM_DEVICE            "lms_pwm"             //!< PWM device name
#define   PWM_DEVICE_NAME       "/dev/lms_pwm"        //!< PWM device file name

#define   MOTOR_DEVICE          "lms_motor"           //!< TACHO device name
#define   MOTOR_DEVICE_NAME     "/dev/lms_motor"      //!< TACHO device file name

#define   ANALOG_DEVICE         "lms_analog"          //!< ANALOG device name
#define   ANALOG_DEVICE_NAME    "/dev/lms_analog"     //!< ANALOG device file name

#define   POWER_DEVICE          "lms_power"           //!< POWER device name
#define   POWER_DEVICE_NAME     "/dev/lms_power"      //!< POWER device file name

#define   DCM_DEVICE            "lms_dcm"             //!< DCM device name
#define   DCM_DEVICE_NAME       "/dev/lms_dcm"        //!< DCM device file name

#define   UI_DEVICE             "lms_ui"              //!< UI device name
#define   UI_DEVICE_NAME        "/dev/lms_ui"         //!< UI device file name

#define   LCD_DEVICE            "lms_display"         //!< DISPLAY device name
#define   LCD_DEVICE_NAME       "/dev/lms_display"    //!< DISPLAY device file name

#define   UART_DEVICE           "lms_uart"            //!< UART device name
#define   UART_DEVICE_NAME      "/dev/lms_uart"       //!< UART device file name

#define   USBDEV_DEVICE         "lms_usbdev"          //!< USB device
#define   USBDEV_DEVICE_NAME    "/dev/lms_usbdev"     //!< USB device

#define   USBHOST_DEVICE        "lms_usbhost"         //!< USB host
#define   USBHOST_DEVICE_NAME   "/dev/lms_usbhost"    //!< USB host

#define   SOUND_DEVICE          "lms_sound"           //!< SOUND device name
#define   SOUND_DEVICE_NAME     "/dev/lms_sound"      //!< SOUND device

#define   IIC_DEVICE            "lms_iic"             //!< IIC device name
#define   IIC_DEVICE_NAME       "/dev/lms_iic"        //!< IIC device

#define   BT_DEVICE             "lms_bt"              //!< BT device name
#define   BT_DEVICE_NAME        "/dev/lms_bt"         //!< BT device



#define   DIR_DEEPT             32                    //!< Max directory items allocated

// File

#define   FILENAME_SIZE         52                    //!< User filename size without extension including zero
#define   TYPEDATE_FILE_NAME    "typedata"            //!< TypeData filename
#define   CALDATA_FILE_NAME     "caldata"             //!< Calibration data filename
#define   LASTRUN_FILE_NAME     "lastrun"             //!< Last run filename
#define   NONVOL_FILE_NAME      "nonvol"              //!< Non volatile memory filename
#define   ICON_FILE_NAME        "icon"                //!< Icon image filename
#define   TEXT_FILE_NAME        "text"                //!< Text filename


/*
#define   DEFAULT_PRJS_FOLDER   "~/lms2012/prjs"      //!< Project folder name without trailing "/"
#define   DEFAULT_APPS_FOLDER   "~/lms2012/apps"      //!< Applet folder name without trailing "/"
#define   DEFAULT_TOOLS_FOLDER  "~/lms2012/tools"     //!< Setting folder name without trailing "/"
#define   DEFAULT_SYS_FOLDER    "~/lms2012/sys"       //!< System folder name without trailing "/"
#define   DEFAULT_SETUP_FOLDER  "~/lms2012/sys"       //!< Setup folder name without trailing "/"
#define   DEFAULT_SD_FOLDER     "~/lms2012/SD Card"   //!< Sd card folder name without trailing "/"
#define   DEFAULT_USB_FOLDER    "~/lms2012/USB Memory"//!< USB Memory folder name without trailing "/"
*/

#define   FOLDERNAME_SIZE       10                    //!< Folder name size relative to "lms2012" folder including zero
#define   DEFAULT_FOLDER        "ui"                  //!< Folder containing the first small programs
#define   DEFAULT_UI            "ui"                  //!< Default user interface

#define   SUBFOLDERNAME_SIZE    FILENAME_SIZE         //!< Sub folder name size without "/" including zero

// Extension

#define   EXTENSION_SIZE        5                     //!< Extension size including zero

#define   MAX_FILENAME_SIZE     (FOLDERNAME_SIZE + SUBFOLDERNAME_SIZE + FILENAME_SIZE + EXTENSION_SIZE + 1)

#define   DEFAULT_VOLUME        50
#define   DEFAULT_SLEEPMINUTES  10
#define   DEFAULT_UNITSINCH     0
#define   DEFAULT_UNITSFAHRENHEIT 0


#ifdef USB64
#define   COM_CMD_DEVICE_NAME   "/dev/hidg0"          //!< USB HID command pipe device file name
#else
#define   COM_CMD_DEVICE_NAME   USBDEV_DEVICE_NAME    //!< USB HID command pipe device file name
#endif

#ifdef LOWSPEED
#define   COM_FILE_DEVICE_NAME  "/dev/hidg1"          //!< USB HID download pipe device file name
#endif

/*! \endverbatim
 *
 */

#define   UPDATE_TIME           2000       //!< Update repeat time (device connection, sound) [uS]


// Per start of (polution) defines
#define   MAX_SOUND_DATA_SIZE   250
#define   SOUND_CHUNK           250
#define SOUND_ADPCM_CHUNK       125
#define SOUND_MASTER_CLOCK  132000000
#define SOUND_TONE_MASTER_CLOCK 1031250
#define SOUND_MIN_FRQ       250
#define SOUND_MAX_FRQ       10000
#define SOUND_MAX_LEVEL     8
#define SOUND_FILE_BUFFER_SIZE  SOUND_CHUNK + 2 // 12.8 mS @ 8KHz
#define SOUND_BUFFER_COUNT  3
#define SOUND_FILE_FORMAT_NORMAL    0x0100  // RSO-file
#define SOUND_FILE_FORMAT_COMPRESSED  0x0101  // RSO-file compressed
// Per end of defines


/*! \todo Clean interface header file
 *
 * \n
 */


#define   VtoC(V)               ((UWORD)((V * ADC_RES) / ADC_REF))
#define   CtoV(C)               ((UWORD)((C * ADC_REF) / ADC_RES))
#define   MtoV(M)               ((UWORD)((M * ADC_REF * 100) / (ADC_RES * 52)))


// Reserved device types
typedef   enum
{
//  TYPE_KEEP                     =   0,  //!< Type value that won't change type in byte codes
  TYPE_NXT_TOUCH                =   1,  //!< Device is NXT touch sensor
  TYPE_NXT_LIGHT                =   2,  //!< Device is NXT light sensor
  TYPE_NXT_SOUND                =   3,  //!< Device is NXT sound sensor
  TYPE_NXT_COLOR                =   4,  //!< Device is NXT color sensor
  TYPE_NXT_ULTRASONIC           =   5,  //!< Device is NXT ultrasonic sensor
  TYPE_NXT_TEMPERATURE          =   6,  //!< Device is NXT temperature sensor

  TYPE_TACHO                    =   7,  //!< Device is a tacho motor
  TYPE_MINITACHO                =   8,  //!< Device is a mini tacho motor
  TYPE_NEWTACHO                 =   9,  //!< Device is a new tacho motor

  TYPE_NXT_TEST                 = 101,  //!< Device is a NXT ADC test sensor

  TYPE_TERMINAL                 = 124,  //!< Port is connected to a terminal
  TYPE_UNKNOWN                  = 125,  //!< Port not empty but type has not been determined
  TYPE_NONE                     = 126,  //!< Port empty or not available
  TYPE_ERROR                    = 127,  //!< Port not empty and type is invalid
}
TYPE;



/*! \page connections Connections
 *
 *  \anchor connectiontypes
 *
 *  Following defines sets the input and output port connection type\n
 *
 *  These connection types are evaluated in the DCM driver \ref DcmDriver "Device Detection Manager Driver"
 *
 *  \verbatim
 */

typedef   enum
{
#ifndef DISABLE_OLD_COLOR

  CONN_UNKNOWN                  = 111,  //!< Connection is fake (test)

  CONN_NXT_COLOR                = 118,  //!< Connection type is NXT color sensor
#else
  CONN_UNKNOWN                  = 118,  //!< Connection is fake (test)
#endif
  CONN_NXT_DUMB                 = 119,  //!< Connection type is NXT analog sensor
  CONN_NXT_IIC                  = 120,  //!< Connection type is NXT IIC sensor

  CONN_INPUT_DUMB               = 121,  //!< Connection type is LMS2012 input device with ID resistor
  CONN_INPUT_UART               = 122,  //!< Connection type is LMS2012 UART sensor

  CONN_OUTPUT_DUMB              = 123,  //!< Connection type is LMS2012 output device with ID resistor
  CONN_OUTPUT_INTELLIGENT       = 124,  //!< Connection type is LMS2012 output device with communication
  CONN_OUTPUT_TACHO             = 125,  //!< Connection type is LMS2012 tacho motor with series ID resistance

  CONN_NONE                     = 126,  //!< Port empty or not available
  CONN_ERROR                    = 127,  //!< Port not empty and type is invalid
}
CONN;

/*\endverbatim
 *
 *  \n
 */



/*! \page objectstatus Object Status
 *
\verbatim

Image load  ->   Initialize all objects


Initialize  ->   if TriggerCount == 0 ->  RUNNING   (VMTHREAD)
                 else                 ->  STOPPED   (waiting for 1. trigger)

1.Trigger   ->   Initialise           ->  WAITING

Triggered   ->   Enqueue              ->  RUNNING

Done        ->   Dequeue              ->  STOPPED


Program start
           |
           v
        STOPPED -------> WAITING -------> RUNNING --------,
           ^    1.trig/          n.trig/          done/   |
           |                     Reset+Enqueue    Dequeue |
           |                                              |
           '----------------------------------------------'

\endverbatim
 */


/*! \enum DSPSTAT
 *
 *        Dispatch status values
 */
typedef   enum
{
  NOBREAK       = 0x0100,               //!< Dispatcher running (looping)
  STOPBREAK     = 0x0200,               //!< Break because of program stop
  SLEEPBREAK    = 0x0400,               //!< Break because of sleeping
  INSTRBREAK    = 0x0800,               //!< Break because of opcode break
  BUSYBREAK     = 0x1000,               //!< Break because of waiting for completion
  PRGBREAK      = 0x2000,               //!< Break because of program break
  USERBREAK     = 0x4000,               //!< Break because of user decision
  FAILBREAK     = 0x8000                //!< Break because of fail
}
DSPSTAT;

typedef   void      (*PRIM)(void);      //!< Prototype for all byte codes



/*! \page memorylayout Memory Layout
 *
 *  RAM layout
 *
 *-   GlobalVariables                   (aligned)
 *
 *-   ObjectPointerList                 (aligned)
 *
 *-   OBJ                               (aligned)
 *  -   Ip                              (4 bytes)
 *  -   Status                          (2 bytes)
 *  -   TriggerCount/CallerId           (2 bytes)
 *  -   Local                           (0..MAX Bytes)\n
 *
 */

/*! \struct OBJ
 *          Object data is used to hold the variables used for an object (allocated at image load time)
 */
typedef   struct                        // Object
{
  IP      Ip;                           //!< Object instruction pointer
  LP      pLocal;                       //!< Local variable pointer
  UWORD   Status;                       //!< Object status
  union                                 //!< Different meaning for SUBCALL and BLOCKS
  {
    OBJID   CallerId;                   //!< Caller id used for SUBCALL to save object id to return to
    TRIGGER TriggerCount;               //!< Trigger count used by BLOCK's trigger logic
  }u;
  VARDATA Local[];                      //!< Poll of bytes used for local variables
}
OBJ;


/*! \struct BRKP
 *          Breakpoint data hold information used for breakpoint
 */
typedef   struct
{
  IMINDEX Addr;                         //!< Offset to breakpoint address from image start
  OP      OpCode;                       //!< Saved substituted opcode
}
BRKP;

/*! \struct PRG
 *          Program data hold information about a program
 */
typedef   struct
{
  ULONG     InstrCnt;                   //!< Instruction counter used for performance analyses
  ULONG     InstrTime;                  //!< Instruction time used for performance analyses

  ULONG     RunTime;                    //!< Program run time [mS]

  IP        pImage;                     //!< Pointer to start of image
  GP        pData;                      //!< Pointer to start of data
  GP        pGlobal;                    //!< Pointer to start of global bytes
  OBJHEAD*  pObjHead;                   //!< Pointer to start of object headers
  OBJ**     pObjList;                   //!< Pointer to object pointer list
  IP        ObjectIp;                   //!< Working object Ip
  LP        ObjectLocal;                //!< Working object locals

  OBJID     Objects;                    //!< No of objects in image
  OBJID     ObjectId;                   //!< Active object id

  OBJSTAT   Status;                     //!< Program status
  OBJSTAT   StatusChange;               //!< Program status change

  BRKP      Brkp[MAX_BREAKPOINTS];      //!< Storage for breakpoint logic

  LABEL     Label[MAX_LABELS];          //!< Storage for labels
  UWORD     Debug;                      //!< Debug flag

  DATA8     Name[FILENAME_SIZE];

}
PRG;


#define     TYPE_NAME_LENGTH    31
#define     SYMBOL_LENGTH       8       //!< Symbol leng th (not including zero)

/*! \struct TYPES
 *          Device type data
 */
typedef   struct // if data type changes - remember to change "cInputTypeDataInit" !
{
  SBYTE     Name[TYPE_NAME_LENGTH + 1]; //!< Device name
  DATA8     Type;                       //!< Device type
  DATA8     Connection;
  DATA8     Mode;                       //!< Device mode
  DATA8     DataSets;
  DATA8     Format;
  DATA8     Figures;
  DATA8     Decimals;
  DATA8     Views;
  DATA8     Pins;                       //!< Device pin setup
  DATAF     RawMin;                     //!< Raw minimum value      (e.c. 0.0)
  DATAF     RawMax;                     //!< Raw maximum value      (e.c. 1023.0)
  DATAF     PctMin;                     //!< Percent minimum value  (e.c. -100.0)
  DATAF     PctMax;                     //!< Percent maximum value  (e.c. 100.0)
  DATAF     SiMin;                      //!< SI unit minimum value  (e.c. -100.0)
  DATAF     SiMax;                      //!< SI unit maximum value  (e.c. 100.0)
  UWORD     InvalidTime;                //!< mS from type change to valid data
  UWORD     IdValue;                    //!< Device id value        (e.c. 0 ~ UART)
  SBYTE     Symbol[SYMBOL_LENGTH + 1];  //!< SI unit symbol
}
TYPES;

#define   TYPE_PARAMETERS   19         //!< Number of members in the structure above


//        INTERFACE FOR SHARED LIBRARIES

extern    void      PrimParAdvance(void);                    // Dummy get parameter

extern    void*     PrimParPointer(void);                    // Get pointer to primitives and system calls parameters

extern    IP        GetImageStart(void);                     // Get pointer to start of image

extern    void      SetDispatchStatus(DSPSTAT Status);       // Set dispatch status (result from executing byte code)

extern    void      SetInstructions(ULONG Instructions);     // Set number of instructions before VMThread change

extern    PRGID     CurrentProgramId(void);                  // Get current program id

extern    OBJSTAT   ProgramStatus(PRGID PrgId);              // Get program status

extern    OBJSTAT   ProgramStatusChange(PRGID PrgId);        // Get program status change

extern    void      ProgramEnd(PRGID PrgId);

extern    OBJID     CallingObjectId(void);                   // Get calling objects id

extern    void      AdjustObjectIp(IMOFFS Value);            // Adjust IP

extern    IP        GetObjectIp(void);                       // Get IP

extern    void      SetObjectIp(IP Ip);                      // Set IP

extern    ULONG     GetTime(void);                           // Get actual program time

extern    ULONG     CurrentObjectIp(void);                   // Get current object ip

extern    void      VmPrint(char *pString);                  // print string

extern    void      SetTerminalEnable(DATA8 Value);          // Terminal enable/disable

extern    DATA8     GetTerminalEnable(void);                 // Get terminal enable state

extern    void      GetResourcePath(char *pString,DATA8 MaxLength);// Get resource path

extern    void*     VmMemoryResize(HANDLER Handle,DATA32 Elements);

extern    void      SetVolumePercent(DATA8 Volume);

extern    DATA8     GetVolumePercent(void);

extern    void      SetSleepMinutes(DATA8 Volume);

extern    DATA8     GetSleepMinutes(void);

extern    void      SetUnitsInch(DATA8 Volume);

extern    DATA8     GetUnitsInch(void);

extern    void      SetUnitsFahrenheit(DATA8 Volume);

extern    DATA8     GetUnitsFahrenheit(void);


/*!\enum  ERR
 *
 *        Describes error numbers
 */
typedef   enum
{
  TOO_MANY_ERRORS_TO_BUFFER,
  TYPEDATA_TABEL_FULL,
  TYPEDATA_FILE_NOT_FOUND,
  ANALOG_DEVICE_FILE_NOT_FOUND,
  ANALOG_SHARED_MEMORY,
  UART_DEVICE_FILE_NOT_FOUND,
  UART_SHARED_MEMORY,
  DISPLAY_SHARED_MEMORY,
  UI_SHARED_MEMORY,
  UI_DEVICE_FILE_NOT_FOUND,
  LCD_DEVICE_FILE_NOT_FOUND,
  OUTPUT_SHARED_MEMORY,
  COM_COULD_NOT_OPEN_FILE,
  COM_NAME_TOO_SHORT,
  COM_NAME_TOO_LONG,
  COM_INTERNAL,
  VM_PROGRAM_VALIDATION,
  VM_PROGRAM_NOT_STARTED,
  VM_PROGRAM_FAIL_BREAK,
  VM_PROGRAM_INSTRUCTION_BREAK,
  VM_PROGRAM_NOT_FOUND,
  SOUND_DEVICE_FILE_NOT_FOUND,
  SOUND_SHARED_MEMORY,
  FILE_OPEN_ERROR,
  FILE_READ_ERROR,
  FILE_WRITE_ERROR,
  FILE_CLOSE_ERROR,
  FILE_GET_HANDLE_ERROR,
  ERRORS
}
ERR;

#define   ERR_STRING_SIZE         vmERR_STRING_SIZE          // Inclusive zero termination

extern    void      LogErrorNumber(ERR Error);               // Log error number


#ifndef   DISABLE_OLD_COLOR

#define   COLORS                        4
#define   CALPOINTS                     3


/*! \page NxtColorMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *
 *  \verbatim
 */
/*
typedef   struct
{
  DATA32  CalVal[COLORS];
}
NXTCAL;

typedef   struct
{
  NXTCAL  Point[CALPOINTS];
  DATA16  Limits[CALPOINTS - 1];
  DATA16  Crc;
  DATA16  ColVal[COLORS];
}
NXTCOL;
*/

typedef   struct
{
  ULONG   Calibration[CALPOINTS][COLORS];
  UWORD   CalLimits[CALPOINTS - 1];
  UWORD   Crc;
  UWORD   ADRaw[COLORS];
  UWORD   SensorRaw[COLORS];
}
COLORSTRUCT;





/*\endverbatim
 *
 *  \n
 */

#endif


//        INTERFACE BETWEEN SHARED LIBRARIES AND MODULES

/*! \page AnalogModuleMemory
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the raw analogue values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

typedef   struct
{
  DATA16  InPin1[INPUTS];         //!< Analog value at input port connection 1
  DATA16  InPin6[INPUTS];         //!< Analog value at input port connection 6
  DATA16  OutPin5[OUTPUTS];       //!< Analog value at output port connection 5
#if       (HARDWARE != ONE2ONE)
  DATA16  BatteryTemp;            //!< Battery temperature
  DATA16  MotorCurrent;           //!< Current flowing to motors
  DATA16  BatteryCurrent;         //!< Current flowing from the battery
  DATA16  Cell123456;             //!< Voltage at battery cell 1, 2, 3,4, 5, and 6
#else
  DATA16  SensorVoltageCnt;       //!< Voltage at input and output connection 4
  DATA16  MotorCurrent;           //!< Current flowing to motors
  DATA16  BatteryCurrent;         //!< Current flowing from the battery
  DATA16  BatteryMultiplexer;     //!< Various battery values

  DATA16  Cell1;                  //!< Voltage at battery cell 1
  DATA16  Cell12;                 //!< Voltage at battery cell 1 and 2
  DATA16  Cell123;                //!< Voltage at battery cell 1, 2 and 3
  DATA16  Cell1234;               //!< Voltage at battery cell 1, 2, 3 and 4
  DATA16  Cell12345;              //!< Voltage at battery cell 1, 2 3, 4, and 5
  DATA16  Cell123456;             //!< Voltage at battery cell 1, 2, 3,4, 5, and 6
  DATA16  AmbientTemp;            //!< Ambient temperature
  DATA16  BatteryTemp;            //!< Battery temperature
#endif
#ifndef   DISABLE_OLD_COLOR
  COLORSTRUCT  NxtCol[INPUTS];
#endif
  DATA16  OutPin5Low[OUTPUTS];    //!< Analog value at output port connection 5 when connection 6 is low

  DATA8   Updated[INPUTS];

  DATA8   InDcm[INPUTS];          //!< Input port device types
  DATA8   InConn[INPUTS];

  DATA8   OutDcm[OUTPUTS];        //!< Output port device types
  DATA8   OutConn[OUTPUTS];
}
ANALOG;

/*\endverbatim
 *
 *  \n
 */


/*! \page UartModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the uart values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define   UART_DATA_LENGTH        32

typedef   struct
{
  DATA8   Raw[INPUTS][UART_DATA_LENGTH];      //!< Raw value from UART device
  DATA8   Output[INPUTS][UART_DATA_LENGTH];   //!< Bytes to UART device
  DATA8   OutputLength[INPUTS];
  DATA8   Status[INPUTS];                     //!< Status
  TYPES   TypeData[INPUTS][MAX_DEVICE_MODES]; //!< TypeData
  DATA8   Changed[INPUTS][MAX_DEVICE_MODES];
}
UART;

/*\endverbatim
 *
 *  \n
 */


#define   UART_PORT_CHANGED       0x01        //!< Input port changed
#define   UART_DATA_READY         0x08        //!< Data is ready
#define   UART_WRITE_REQUEST      0x10        //!< Write request


/*! \page IicModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the iic values for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define   IIC_DATA_LENGTH         32

typedef   struct
{
  TYPES   TypeData[INPUTS][MAX_DEVICE_MODES]; //!< TypeData
  DATA8   Raw[INPUTS][IIC_DATA_LENGTH];       //!< Raw value from UART device
  DATA8   Output[INPUTS][IIC_DATA_LENGTH];    //!< Bytes to UART device
  DATA8   OutputLength[INPUTS];
  DATA8   Status[INPUTS];                     //!< Status
  DATA8   Changed[INPUTS][MAX_DEVICE_MODES];
}
IIC;

/*\endverbatim
 *
 *  \n
 */


/*! \page UiModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the ui button states for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

#define   BUTTON_ACTIVATED              0x01  //!< button has been activated since last read
#define   BUTTON_PRESSED                0x02  //!< button is pressed at the moment
#define   BUTTON_LONGPRESS              0x04  //!< button long press detected

typedef   struct
{
  DATA8   State[BUTTONS];                     //!< State bit field
}
UI;

/*\endverbatim
 *
 *  \n
 */


#define   LCD_BUFFER_SIZE (((LCD_WIDTH + 7) / 8) * LCD_HEIGHT)
#define   LCD_TOPLINE_SIZE (((LCD_WIDTH + 7) / 8) * (TOPLINE_HEIGHT + 1))


/*! \page DisplayModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the display memory for use in userspace
 *  this pointer will point to a frame buffer as follows:
 *
 *  \verbatim
 */

typedef   struct
{
  UBYTE   Lcd[LCD_BUFFER_SIZE];
}
LCD;

/*\endverbatim
 *
 *  \n
 */


/*! \page SoundModuleMemory
 *
 *  <b>     Shared Memory </b>
 *
 *  <hr size="1"/>
 *
 *  It is possible to get a pointer to the sound ready flag for use in userspace
 *  this pointer will point to a struct and the layout is following:
 *
 *  \verbatim
 */

typedef   struct
{
  DATA8   Status;                       //!< Status
}
SOUND;

/*\endverbatim
 *
 *  \n
 */


/*! \page VmNonvolatileMemory
 *
 *  <b>     VM non volatile Memory </b>
 *
 *  <hr size="1"/>
 *
 *  This struct will be loaded at start up and save when closing down VM
 *
 *  \verbatim
 */

typedef   struct
{
  DATA8   BrickId[ID_LENGTH];           //!< Unique brick id
  DATA8   BrickName[NAME_LENGTH + 1];   //!< Brick name
  DATA8   VolumePercent;                //!< System default volume [0..100%]
  DATA8   SleepMinutes;                 //!< System sleep          [0..120min] (0 = ~)
  DATA8   UnitsInch;                    //!< Unit 0 = cm,   1 = Inch
  DATA8   UnitsFahrenheit;              //!< Unit 0 = Deg,  1 = Fahrenheit
}
NONVOL;

/*\endverbatim
 *
 *  \n
 */


/*
 *                      Motor/OUTPUT Typedef
 */
typedef struct
{
  SLONG TachoCounts;
  SBYTE Speed;
  SLONG TachoSensor;
}MOTORDATA;

typedef struct
{
  DATA8   Cmd;
  DATA8   Nos;
  DATA8   Power;
  DATA32  Step1;
  DATA32  Step2;
  DATA32  Step3;
  DATA8   Brake;
} STEPPOWER;

typedef struct
{
  DATA8   Cmd;
  DATA8   Nos;
  DATA8   Power;
  DATA32  Time1;
  DATA32  Time2;
  DATA32  Time3;
  DATA8   Brake;
} TIMEPOWER;

typedef struct
{
  DATA8   Cmd;
  DATA8   Nos;
  DATA8   Speed;
  DATA32  Step1;
  DATA32  Step2;
  DATA32  Step3;
  DATA8   Brake;
} STEPSPEED;

typedef struct
{
  DATA8   Cmd;
  DATA8   Nos;
  DATA8   Speed;
  DATA32  Time1;
  DATA32  Time2;
  DATA32  Time3;
  DATA8   Brake;
} TIMESPEED;

typedef struct
{
  DATA8   Cmd;
  DATA8   Nos;
  DATA8   Speed;
  DATA16  Turn;
  DATA32  Step;
  DATA8   Brake;
} STEPSYNC;

typedef struct
{
  DATA8   Cmd;
  DATA8   Nos;
  DATA8   Speed;
  DATA16  Turn;
  DATA32  Time;
  DATA8   Brake;
} TIMESYNC;
/*
 *                 End of Motor/OUTPUT Typedef
 */


#define     PRINTBUFFERSIZE             160

typedef struct
{
  NONVOL    NonVol;
  DATA8     FirstProgram[MAX_FILENAME_SIZE];

  char      PrintBuffer[PRINTBUFFERSIZE + 1];
  DATA8     TerminalEnabled;

  PRGID     FavouritePrg;
  PRGID     ProgramId;                    //!< Program id running
  PRG       Program[MAX_PROGRAMS];        //!< Program[0] is the UI byte codes running


  ULONG     InstrCnt;                     //!< Instruction counter (performance test)
  IP        pImage;                       //!< Pointer to start of image
  GP        pGlobal;                      //!< Pointer to start of global bytes
  OBJHEAD*  pObjHead;                     //!< Pointer to start of object headers
  OBJ**     pObjList;                     //!< Pointer to object pointer list

  IP        ObjectIp;                     //!< Working object Ip
  LP        ObjectLocal;                  //!< Working object locals
  OBJID     Objects;                      //!< No of objects in image
  OBJID     ObjectId;                     //!< Active object id

  long      TimerDataSec;
  long      TimerDatanSec;

  UWORD     Debug;

  UWORD     RefCount;

  ULONG     TimeuS;

  ULONG     OldTime;
  ULONG     NewTime;

  DSPSTAT   DispatchStatus;               //!< Dispatch status
  ULONG     Priority;                     //!< Object priority

  ULONG     Value;
  HANDLER   Handle;

  ERR       Errors[ERROR_BUFFER_SIZE];
  UBYTE     ErrorIn;
  UBYTE     ErrorOut;

  LCD       LcdBuffer;                    //!< Copy of last LCD update
  DATA8     LcdUpdated;                   //!< LCD updated


#if       (HARDWARE == SIMULATION)
  class NXT * nxt;
#endif
}GLOBALS;


#if       (HARDWARE == SIMULATION)
  extern GLOBALS * gInstance;
  #define VMInstance (*gInstance)
  RESULT mSchedInit();
  RESULT mSchedCtrl();
  RESULT mSchedExit();

  void setInstance(GLOBALS * _Instance);
  GLOBALS * getInstance();
#else
  extern GLOBALS VMInstance;
#endif


#endif /* LMS2012_H_ */
