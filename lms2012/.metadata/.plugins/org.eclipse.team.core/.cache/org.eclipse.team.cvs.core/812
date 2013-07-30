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


#ifndef   BYTECODES_H_
#define   BYTECODES_H_

#define   BYTECODE_VERSION              1.04

/*! \page system System Configuration
 *
 *  <hr size="1"/>
 *
 *  Following defines sets the system configuration and limitations.\n
 *  Defines with preceding "vm" is visible for the byte code assembler\n
 *
 *  \verbatim
 */

// HARDWARE

#define   vmOUTPUTS                     4                             //!< Number of output ports in the system
#define   vmINPUTS                      4                             //!< Number of input  ports in the system
#define   vmBUTTONS                     6                             //!< Number of buttons in the system
#define   vmLEDS                        4                             //!< Number of LEDs in the system

#define   vmLCD_WIDTH                   178                           //!< LCD horizontal pixels
#define   vmLCD_HEIGHT                  128                           //!< LCD vertical pixels
#define   vmTOPLINE_HEIGHT              10                            //!< Top line vertical pixels
#define   vmLCD_STORE_LEVELS            3                             //!< Store levels

#define   vmDEFAULT_VOLUME              100
#define   vmDEFAULT_SLEEPMINUTES        30

// SOFTWARE

#define   vmFG_COLOR                    1                             //!<  Forground color
#define   vmBG_COLOR                    0                             //!<  Background color

#define   vmCHAIN_DEPT                  4                             //!< Number of bricks in the USB daisy chain (master + slaves)

#define   FILEPERMISSIONS               (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)
#define   DIRPERMISSIONS                (S_IRWXU | S_IRWXG | S_IRWXO)
#define   SYSPERMISSIONS                (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define   vmPATHSIZE                    84                            //!< Max path size excluding trailing forward slash including zero termination
#define   vmNAMESIZE                    32                            //!< Max name size including zero termination (must be divideable by 4)
#define   vmEXTSIZE                     5                             //!< Max extension size including dot and zero termination
#define   vmFILENAMESIZE                120                           //!< Max filename size including path, name, extension and termination (must be divideable by 4)
#define   vmMACSIZE                     18                            //!< Max WIFI MAC size including zero termination
#define   vmIPSIZE                      16                            //!< Max WIFI IP size including zero termination
#define   vmBTADRSIZE                   13                            //!< Max bluetooth address size including zero termination

#define   vmERR_STRING_SIZE             32                            // Inclusive zero termination

#define   vmEVENT_BT_PIN                1
#define   vmEVENT_BT_REQ_CONF           2

#define   vmMAX_VALID_TYPE              101                           //!< Highest valid device type

// FOLDERS

#define   vmMEMORY_FOLDER               "/mnt/ramdisk"                //!< Folder for non volatile user programs/data
#define   vmPROGRAM_FOLDER              "../prjs/BrkProg_SAVE"        //!< Folder for On Brick Programming programs
#define   vmDATALOG_FOLDER              "../prjs/BrkDL_SAVE"          //!< Folder for On Brick Data log files
#define   vmSDCARD_FOLDER               "../prjs/SD_Card"             //!< Folder for SD card mount
#define   vmUSBSTICK_FOLDER             "../prjs/USB_Stick"           //!< Folder for USB stick mount

#define   vmPRJS_DIR                    "../prjs"                     //!< Project folder
#define   vmAPPS_DIR                    "../apps"                     //!< Apps folder
#define   vmTOOLS_DIR                   "../tools"                    //!< Tools folder
#define   vmTMP_DIR                     "../tmp"                      //!< Temporary folder

#define   vmSETTINGS_DIR                "../sys/settings"             //!< Folder for non volatile settings

#define   vmDIR_DEEPT                   127                           //!< Max directory items allocated including "." and ".."

// FILES USED IN APPLICATION

#define   vmLASTRUN_FILE_NAME           "lastrun"                     //!< Last run filename
#define   vmCALDATA_FILE_NAME           "caldata"                     //!< Calibration data filename

// FILES USED IN APPS

#define   vmSLEEP_FILE_NAME             "Sleep"                       //!< File used in "Sleep" app to save status
#define   vmVOLUME_FILE_NAME            "Volume"                      //!< File used in "Volume" app to save status
#define   vmWIFI_FILE_NAME              "WiFi"                        //!< File used in "WiFi" app to save status
#define   vmBLUETOOTH_FILE_NAME         "Bluetooth"                   //!< File used in "Bluetooth" app to save status

// EXTENSIONS

#define   vmEXT_SOUND                   ".rsf"                        //!< Robot Sound File
#define   vmEXT_GRAPHICS                ".rgf"                        //!< Robot Graphics File
#define   vmEXT_BYTECODE                ".rbf"                        //!< Robot Byte code File
#define   vmEXT_TEXT                    ".rtf"                        //!< Robot Text File
#define   vmEXT_DATALOG                 ".rdf"                        //!< Robot Datalog File
#define   vmEXT_PROGRAM                 ".rpf"                        //!< Robot Program File
#define   vmEXT_CONFIG                  ".rcf"                        //!< Robot Configuration File
#define   vmEXT_ARCHIVE                 ".raf"                        //!< Robot Archive File

// NAME LENGTHs

#define   vmBRICKNAMESIZE               120                           //!< Brick name maximal size (including zero termination)
#define   vmBTPASSKEYSIZE               7                             //!< Bluetooth pass key size (including zero termination)
#define   vmWIFIPASSKEYSIZE             33                            //!< WiFi pass key size (including zero termination)

// VALID CHARACTERS

#define   vmCHARSET_NAME                0x01                          //!< Character set allowed in brick name and raw filenames
#define   vmCHARSET_FILENAME            0x02                          //!< Character set allowed in file names
#define   vmCHARSET_BTPASSKEY           0x04                          //!< Character set allowed in bluetooth pass key
#define   vmCHARSET_WIFIPASSKEY         0x08                          //!< Character set allowed in WiFi pass key
#define   vmCHARSET_WIFISSID            0x10                          //!< Character set allowed in WiFi ssid


/* \endverbatim */


/*! \page bytecodedef Byte Code Defines
 *
 *  \verbatim
 */
typedef   enum
{
//  \endverbatim \ref VM \verbatim
//                                        0000....
  opERROR                     = 0x00, //      0000
  opNOP                       = 0x01, //      0001
  opPROGRAM_STOP              = 0x02, //      0010
  opPROGRAM_START             = 0x03, //      0011
  opOBJECT_STOP               = 0x04, //      0100
  opOBJECT_START              = 0x05, //      0101
  opOBJECT_TRIG               = 0x06, //      0110
  opOBJECT_WAIT               = 0x07, //      0111
  opRETURN                    = 0x08, //      1000
  opCALL                      = 0x09, //      1001
  opOBJECT_END                = 0x0A, //      1010
  opSLEEP                     = 0x0B, //      1011
  opPROGRAM_INFO              = 0x0C, //      1100
  opLABEL                     = 0x0D, //      1101
  opPROBE                     = 0x0E, //      1110
  opDO                        = 0x0F, //      1111

//  \endverbatim \ref cMath "MATH" \verbatim
//                                        0001....
//                    ADD                     00..
  opADD8                      = 0x10, //        00
  opADD16                     = 0x11, //        01
  opADD32                     = 0x12, //        10
  opADDF                      = 0x13, //        11
//                    SUB                     01..
  opSUB8                      = 0x14, //        00
  opSUB16                     = 0x15, //        01
  opSUB32                     = 0x16, //        10
  opSUBF                      = 0x17, //        11
//                    MUL                     10..
  opMUL8                      = 0x18, //        00
  opMUL16                     = 0x19, //        01
  opMUL32                     = 0x1A, //        10
  opMULF                      = 0x1B, //        11
//                    DIV                     11..
  opDIV8                      = 0x1C, //        00
  opDIV16                     = 0x1D, //        01
  opDIV32                     = 0x1E, //        10
  opDIVF                      = 0x1F, //        11

//  \endverbatim \ref Logic "LOGIC" \verbatim
//        LOGIC                           0010....
//                    OR                      00..
  opOR8                       = 0x20, //        00
  opOR16                      = 0x21, //        01
  opOR32                      = 0x22, //        10

//                    AND                     01..
  opAND8                      = 0x24, //        00
  opAND16                     = 0x25, //        01
  opAND32                     = 0x26, //        10

//                    XOR                     10..
  opXOR8                      = 0x28, //        00
  opXOR16                     = 0x29, //        01
  opXOR32                     = 0x2A, //        10

//                    RL                      11..
  opRL8                       = 0x2C, //        00
  opRL16                      = 0x2D, //        01
  opRL32                      = 0x2E, //        10

//  \endverbatim \ref cMove "MOVE" \verbatim
  opINIT_BYTES                = 0x2F, //      1111
//        MOVE                            0011....
//                    MOVE8_                  00..
  opMOVE8_8                   = 0x30, //        00
  opMOVE8_16                  = 0x31, //        01
  opMOVE8_32                  = 0x32, //        10
  opMOVE8_F                   = 0x33, //        11
//                    MOVE16_                 01..
  opMOVE16_8                  = 0x34, //        00
  opMOVE16_16                 = 0x35, //        01
  opMOVE16_32                 = 0x36, //        10
  opMOVE16_F                  = 0x37, //        11
//                    MOVE32_                 10..
  opMOVE32_8                  = 0x38, //        00
  opMOVE32_16                 = 0x39, //        01
  opMOVE32_32                 = 0x3A, //        10
  opMOVE32_F                  = 0x3B, //        11
//                    MOVEF_                  11..
  opMOVEF_8                   = 0x3C, //        00
  opMOVEF_16                  = 0x3D, //        01
  opMOVEF_32                  = 0x3E, //        10
  opMOVEF_F                   = 0x3F, //        11

//  \endverbatim \ref cBranch "BRANCH" \verbatim
//        BRANCH                          010000..
  opJR                        = 0x40, //        00
  opJR_FALSE                  = 0x41, //        01
  opJR_TRUE                   = 0x42, //        10
  opJR_NAN                    = 0x43, //        11

//  \endverbatim \ref cCompare "COMPARE" \verbatim
//        COMPARE                         010.....
//                    CP_LT                  001..
  opCP_LT8                    = 0x44, //        00
  opCP_LT16                   = 0x45, //        01
  opCP_LT32                   = 0x46, //        10
  opCP_LTF                    = 0x47, //        11
//                    CP_GT                  010..
  opCP_GT8                    = 0x48, //        00
  opCP_GT16                   = 0x49, //        01
  opCP_GT32                   = 0x4A, //        10
  opCP_GTF                    = 0x4B, //        11
//                    CP_EQ                  011..
  opCP_EQ8                    = 0x4C, //        00
  opCP_EQ16                   = 0x4D, //        01
  opCP_EQ32                   = 0x4E, //        10
  opCP_EQF                    = 0x4F, //        11
//                    CP_NEQ                 100..
  opCP_NEQ8                   = 0x50, //        00
  opCP_NEQ16                  = 0x51, //        01
  opCP_NEQ32                  = 0x52, //        10
  opCP_NEQF                   = 0x53, //        11
//                    CP_LTEQ                101..
  opCP_LTEQ8                  = 0x54, //        00
  opCP_LTEQ16                 = 0x55, //        01
  opCP_LTEQ32                 = 0x56, //        10
  opCP_LTEQF                  = 0x57, //        11
//                    CP_GTEQ                110..
  opCP_GTEQ8                  = 0x58, //        00
  opCP_GTEQ16                 = 0x59, //        01
  opCP_GTEQ32                 = 0x5A, //        10
  opCP_GTEQF                  = 0x5B, //        11

//  \endverbatim \ref Select "SELECT" \verbatim
//        SELECT                          010111..
  opSELECT8                   = 0x5C, //        00
  opSELECT16                  = 0x5D, //        01
  opSELECT32                  = 0x5E, //        10
  opSELECTF                   = 0x5F, //        11


//  \endverbatim \ref VM \verbatim
  opSYSTEM                    = 0x60,
  opPORT_CNV_OUTPUT           = 0x61,
  opPORT_CNV_INPUT            = 0x62,
  opNOTE_TO_FREQ              = 0x63,

//  \endverbatim \ref cBranch "BRANCH" \verbatim
//        BRANCH                          011000..
                                      //?       00
                                      //?       01
                                      //?       10
                                      //?       11
//                    JR_LT                  001..
  opJR_LT8                    = 0x64, //        00
  opJR_LT16                   = 0x65, //        01
  opJR_LT32                   = 0x66, //        10
  opJR_LTF                    = 0x67, //        11
//                    JR_GT                  010..
  opJR_GT8                    = 0x68, //        00
  opJR_GT16                   = 0x69, //        01
  opJR_GT32                   = 0x6A, //        10
  opJR_GTF                    = 0x6B, //        11
//                    JR_EQ                  011..
  opJR_EQ8                    = 0x6C, //        00
  opJR_EQ16                   = 0x6D, //        01
  opJR_EQ32                   = 0x6E, //        10
  opJR_EQF                    = 0x6F, //        11
//                    JR_NEQ                 100..
  opJR_NEQ8                   = 0x70, //        00
  opJR_NEQ16                  = 0x71, //        01
  opJR_NEQ32                  = 0x72, //        10
  opJR_NEQF                   = 0x73, //        11
//                    JR_LTEQ                101..
  opJR_LTEQ8                  = 0x74, //        00
  opJR_LTEQ16                 = 0x75, //        01
  opJR_LTEQ32                 = 0x76, //        10
  opJR_LTEQF                  = 0x77, //        11
//                    JR_GTEQ                110..
  opJR_GTEQ8                  = 0x78, //        00
  opJR_GTEQ16                 = 0x79, //        01
  opJR_GTEQ32                 = 0x7A, //        10
  opJR_GTEQF                  = 0x7B, //        11

//  \endverbatim \ref VM \verbatim
  opINFO                      = 0x7C, //  01111100
  opSTRINGS                   = 0x7D, //  01111101
  opMEMORY_WRITE              = 0x7E, //  01111110
  opMEMORY_READ               = 0x7F, //  01111111

//        SYSTEM                          1.......

//  \endverbatim \ref cUi "UI" \verbatim
//        UI                              100000..
  opUI_FLUSH                  = 0x80, //        00
  opUI_READ                   = 0x81, //        01
  opUI_WRITE                  = 0x82, //        10
  opUI_BUTTON                 = 0x83, //        11
  opUI_DRAW                   = 0x84, //  10000100

//  \endverbatim \ref cTimer "TIMER" \verbatim
  opTIMER_WAIT                = 0x85, //  10000101
  opTIMER_READY               = 0x86, //  10000110
  opTIMER_READ                = 0x87, //  10000111

//  \endverbatim \ref VM \verbatim
//        BREAKPOINT                      10001...
  opBP0                       = 0x88, //       000
  opBP1                       = 0x89, //       001
  opBP2                       = 0x8A, //       010
  opBP3                       = 0x8B, //       011
  opBP_SET                    = 0x8C, //  10001100
  opMATH                      = 0x8D, //  10001101
  opRANDOM                    = 0x8E, //  10001110

//  \endverbatim \ref cTimer "TIMER" \verbatim
  opTIMER_READ_US             = 0x8F, //  10001111

//  \endverbatim \ref cUi "UI" \verbatim
  opKEEP_ALIVE                = 0x90, //  10010000

//  \endverbatim \ref cCom "COM" \verbatim
//                                        100100
  opCOM_READ                  = 0x91, //        01
  opCOM_WRITE                 = 0x92, //        10

//  \endverbatim \ref cSound "SOUND" \verbatim
//                                        100101
  opSOUND                     = 0x94, //        00
  opSOUND_TEST                = 0x95, //        01
  opSOUND_READY               = 0x96, //        10

//  \endverbatim \ref cInput "INPUT" \verbatim
//
  opINPUT_SAMPLE              = 0x97, //  10010111

//                                        10011...
  opINPUT_DEVICE_LIST         = 0x98, //       000
  opINPUT_DEVICE              = 0x99, //       001
  opINPUT_READ                = 0x9A, //       010
  opINPUT_TEST                = 0x9B, //       011
  opINPUT_READY               = 0x9C, //       100
  opINPUT_READSI              = 0x9D, //       101
  opINPUT_READEXT             = 0x9E, //       110
  opINPUT_WRITE               = 0x9F, //       111
//  \endverbatim \ref cOutput "OUTPUT" \verbatim
//                                        101.....
  opOUTPUT_GET_TYPE           = 0xA0, //     00000
  opOUTPUT_SET_TYPE           = 0xA1, //     00001
  opOUTPUT_RESET              = 0xA2, //     00010
  opOUTPUT_STOP               = 0xA3, //     00011
  opOUTPUT_POWER              = 0xA4, //     00100
  opOUTPUT_SPEED              = 0xA5, //     00101
  opOUTPUT_START              = 0xA6, //     00110
  opOUTPUT_POLARITY           = 0xA7, //     00111
  opOUTPUT_READ               = 0xA8, //     01000
  opOUTPUT_TEST               = 0xA9, //     01001
  opOUTPUT_READY              = 0xAA, //     01010
  opOUTPUT_POSITION           = 0xAB, //     01011
  opOUTPUT_STEP_POWER         = 0xAC, //     01100
  opOUTPUT_TIME_POWER         = 0xAD, //     01101
  opOUTPUT_STEP_SPEED         = 0xAE, //     01110
  opOUTPUT_TIME_SPEED         = 0xAF, //     01111

  opOUTPUT_STEP_SYNC          = 0xB0, //     10000
  opOUTPUT_TIME_SYNC          = 0xB1, //     10001
  opOUTPUT_CLR_COUNT          = 0xB2, //     10010
  opOUTPUT_GET_COUNT          = 0xB3, //     10011

  opOUTPUT_PRG_STOP           = 0xB4, //     10100

//  \endverbatim \ref cMemory "MEMORY" \verbatim
//                                        11000...
  opFILE                      = 0xC0, //       000
  opARRAY                     = 0xC1, //       001
  opARRAY_WRITE               = 0xC2, //       010
  opARRAY_READ                = 0xC3, //       011
  opARRAY_APPEND              = 0xC4, //       100
  opMEMORY_USAGE              = 0xC5, //       101
  opFILENAME                  = 0xC6, //       110

//  \endverbatim \ref cMove "READ" \verbatim
//                                        110010..
  opREAD8                     = 0xC8, //        00
  opREAD16                    = 0xC9, //        01
  opREAD32                    = 0xCA, //        10
  opREADF                     = 0xCB, //        11

//  \endverbatim \ref cMove "WRITE" \verbatim
//                                        110011..
  opWRITE8                    = 0xCC, //        00
  opWRITE16                   = 0xCD, //        01
  opWRITE32                   = 0xCE, //        10
  opWRITEF                    = 0xCF, //        11

//  \endverbatim \ref cCom "COM" \verbatim
//                                        11010...
  opCOM_READY                 = 0xD0, //       000
  opCOM_READDATA              = 0xD1, //       001
  opCOM_WRITEDATA             = 0xD2, //       010
  opCOM_GET                   = 0xD3, //       011
  opCOM_SET                   = 0xD4, //       100
  opCOM_TEST                  = 0xD5, //       101
  opCOM_REMOVE                = 0xD6, //       110
  opCOM_WRITEFILE             = 0xD7, //       111

//                                        11011...
  opMAILBOX_OPEN              = 0xD8, //       000
  opMAILBOX_WRITE             = 0xD9, //       001
  opMAILBOX_READ              = 0xDA, //       010
  opMAILBOX_TEST              = 0xDB, //       011
  opMAILBOX_READY             = 0xDC, //       100
  opMAILBOX_CLOSE             = 0xDD, //       101

//        SPARE                           111.....

//  \endverbatim \ref TST \verbatim
  opTST                       = 0xFF  //  11111111
}
OP;
/*
 *  \endverbatim
 */


//! \page uireadsubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  GET_VBATT     = 1,
  GET_IBATT     = 2,
  GET_OS_VERS   = 3,
  GET_EVENT     = 4,
  GET_TBATT     = 5,
  GET_IINT      = 6,
  GET_IMOTOR    = 7,
  GET_STRING    = 8,
  GET_HW_VERS   = 9,
  GET_FW_VERS   = 10,
  GET_FW_BUILD  = 11,
  GET_OS_BUILD  = 12,
  GET_ADDRESS   = 13,
  GET_CODE      = 14,
  KEY           = 15,
  GET_SHUTDOWN  = 16,
  GET_WARNING   = 17,
  GET_LBATT     = 18,
  TEXTBOX_READ  = 21,
  GET_VERSION   = 26,
  GET_IP        = 27,
  GET_POWER     = 29,
  GET_SDCARD    = 30,
  GET_USBSTICK  = 31,

  UI_READ_SUBCODES
}
UI_READ_SUBCODE;

//! \endverbatim


//! \page uiwritesubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  WRITE_FLUSH   = 1,
  FLOATVALUE    = 2,
  STAMP         = 3,
  PUT_STRING    = 8,
  VALUE8        = 9,
  VALUE16       = 10,
  VALUE32       = 11,
  VALUEF        = 12,
  ADDRESS       = 13,
  CODE          = 14,
  DOWNLOAD_END  = 15,
  SCREEN_BLOCK  = 16,
  TEXTBOX_APPEND = 21,
  SET_BUSY      = 22,
  SET_TESTPIN   = 24,
  INIT_RUN      = 25,
  UPDATE_RUN    = 26,
  LED           = 27,
  POWER         = 29,
  GRAPH_SAMPLE  = 30,
  TERMINAL      = 31,

  UI_WRITE_SUBCODES
}
UI_WRITE_SUBCODE;

//! \endverbatim


//! \page uibuttonsubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  SHORTPRESS      = 1,
  LONGPRESS       = 2,
  WAIT_FOR_PRESS  = 3,
  FLUSH           = 4,
  PRESS           = 5,
  RELEASE         = 6,
  GET_HORZ        = 7,
  GET_VERT        = 8,
  PRESSED         = 9,
  SET_BACK_BLOCK  = 10,
  GET_BACK_BLOCK  = 11,
  TESTSHORTPRESS  = 12,
  TESTLONGPRESS   = 13,
  GET_BUMBED      = 14,
  GET_CLICK       = 15,

  UI_BUTTON_SUBCODES
}
UI_BUTTON_SUBCODE;

//! \endverbatim


//! \page comreadsubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  COMMAND       = 14,

  COM_READ_SUBCODES
}
COM_READ_SUBCODE;

//! \endverbatim


//! \page comwritesubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  REPLY         = 14,

  COM_WRITE_SUBCODES
}
COM_WRITE_SUBCODE;

//! \endverbatim


//! \page comgetsubcode Specific command parameter
//!
//! \verbatim
//!

typedef   enum
{
  GET_ON_OFF    = 1,                    //!< Set, Get
  GET_VISIBLE   = 2,                    //!< Set, Get
  GET_RESULT    = 4,                    //!<      Get
  GET_PIN       = 5,                    //!< Set, Get
  SEARCH_ITEMS  = 8,                    //!<      Get
  SEARCH_ITEM   = 9,                    //!<      Get
  FAVOUR_ITEMS  = 10,                   //!<      Get
  FAVOUR_ITEM   = 11,                   //!<      Get
  GET_ID        = 12,
  GET_BRICKNAME = 13,
  GET_NETWORK   = 14,
  GET_PRESENT   = 15,
  GET_ENCRYPT   = 16,
  CONNEC_ITEMS  = 17,
  CONNEC_ITEM   = 18,
  GET_INCOMING  = 19,
  GET_MODE2     = 20,

  COM_GET_SUBCODES
}
COM_GET_SUBCODE;

//! \endverbatim


//! \page comsetsubcode Specific command parameter
//!
//! \verbatim
//!

typedef   enum
{
  SET_ON_OFF    = 1,                    //!< Set, Get
  SET_VISIBLE   = 2,                    //!< Set, Get
  SET_SEARCH    = 3,                    //!< Set
  SET_PIN       = 5,                    //!< Set, Get
  SET_PASSKEY   = 6,                    //!< Set
  SET_CONNECTION = 7,                    //!< Set
  SET_BRICKNAME = 8,
  SET_MOVEUP    = 9,
  SET_MOVEDOWN  = 10,
  SET_ENCRYPT   = 11,
  SET_SSID      = 12,
  SET_MODE2     = 13,

  COM_SET_SUBCODES
}
COM_SET_SUBCODE;

//! \endverbatim


//! \page inputdevicesubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  GET_FORMAT      = 2,
  CAL_MINMAX      = 3,
  CAL_DEFAULT     = 4,
  GET_TYPEMODE    = 5,
  GET_SYMBOL      = 6,
  CAL_MIN         = 7,
  CAL_MAX         = 8,
  SETUP           = 9,
  CLR_ALL         = 10,
  GET_RAW         = 11,
  GET_CONNECTION  = 12,
  STOP_ALL        = 13,
  GET_NAME        = 21,
  GET_MODENAME    = 22,
  SET_RAW         = 23,
  GET_FIGURES     = 24,
  GET_CHANGES     = 25,
  CLR_CHANGES     = 26,
  READY_PCT       = 27,
  READY_RAW       = 28,
  READY_SI        = 29,
  GET_MINMAX      = 30,
  GET_BUMPS       = 31,

  INPUT_DEVICESUBCODES
}
INPUT_DEVICE_SUBCODE;

//! \endverbatim


//! \page programinfosubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  OBJ_STOP      = 0,    // VM
  OBJ_START     = 4,    // VM
  GET_STATUS    = 22,   // VM
  GET_SPEED     = 23,   // VM
  GET_PRGRESULT = 24,   // VM
  SET_INSTR     = 25,   // VM

  PROGRAM_INFO_SUBCODES,
}
PROGRAM_INFO_SUBCODE;

//! \endverbatim


//! \page uidrawsubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  UPDATE        = 0,
  CLEAN         = 1,
  PIXEL         = 2,
  LINE          = 3,
  CIRCLE        = 4,
  TEXT          = 5,
  ICON          = 6,
  PICTURE       = 7,
  VALUE         = 8,
  FILLRECT      = 9,
  RECT          = 10,
  NOTIFICATION  = 11,
  QUESTION      = 12,
  KEYBOARD      = 13,
  BROWSE        = 14,
  VERTBAR       = 15,
  INVERSERECT   = 16,
  SELECT_FONT   = 17,
  TOPLINE       = 18,
  FILLWINDOW    = 19,
  SCROLL        = 20,
  DOTLINE       = 21,
  VIEW_VALUE    = 22,
  VIEW_UNIT     = 23,
  FILLCIRCLE    = 24,
  STORE         = 25,
  RESTORE       = 26,
  ICON_QUESTION = 27,
  BMPFILE       = 28,
  POPUP         = 29,
  GRAPH_SETUP   = 30,
  GRAPH_DRAW    = 31,
  TEXTBOX       = 32,

  UI_DRAW_SUBCODES
}
UI_DRAW_SUBCODE;

//! \endverbatim


//! \page memoryfilesubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  OPEN_APPEND         = 0,
  OPEN_READ           = 1,
  OPEN_WRITE          = 2,
  READ_VALUE          = 3,
  WRITE_VALUE         = 4,
  READ_TEXT           = 5,
  WRITE_TEXT          = 6,
  CLOSE               = 7,
  LOAD_IMAGE          = 8,
  GET_HANDLE          = 9,
  MAKE_FOLDER         = 10,
  GET_POOL            = 11,
  SET_LOG_SYNC_TIME   = 12,
  GET_FOLDERS         = 13,
  GET_LOG_SYNC_TIME   = 14,
  GET_SUBFOLDER_NAME  = 15,
  WRITE_LOG           = 16,
  CLOSE_LOG           = 17,
  GET_IMAGE           = 18,
  GET_ITEM            = 19,
  GET_CACHE_FILES     = 20,
  PUT_CACHE_FILE      = 21,
  GET_CACHE_FILE      = 22,
  DEL_CACHE_FILE      = 23,
  DEL_SUBFOLDER       = 24,
  GET_LOG_NAME        = 25,

  OPEN_LOG            = 27,
  READ_BYTES          = 28,
  WRITE_BYTES         = 29,
  REMOVE              = 30,
  MOVE                = 31,

  FILE_SUBCODES
}
FILE_SUBCODE;

//! \endverbatim


//! \page memoryarraysubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  DELETE              = 0,
  CREATE8             = 1,
  CREATE16            = 2,
  CREATE32            = 3,
  CREATEF             = 4,
  RESIZE              = 5,
  FILL                = 6,
  COPY                = 7,
  INIT8               = 8,
  INIT16              = 9,
  INIT32              = 10,
  INITF               = 11,
  SIZE                = 12,
  READ_CONTENT        = 13,
  WRITE_CONTENT       = 14,
  READ_SIZE           = 15,

  ARRAY_SUBCODES
}
ARRAY_SUBCODE;

//! \endverbatim


//! \page memoryfilenamesubcode Specific command parameter
//!
//! \verbatim
//!

typedef   enum
{
  EXIST               = 16,     //!< MUST BE GREATER OR EQUAL TO "ARRAY_SUBCODES"
  TOTALSIZE           = 17,
  SPLIT               = 18,
  MERGE               = 19,
  CHECK               = 20,
  PACK                = 21,
  UNPACK              = 22,
  GET_FOLDERNAME      = 23,

  FILENAME_SUBCODES
}
FILENAME_SUBCODE;

//! \endverbatim


//! \page infosubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  SET_ERROR           = 1,
  GET_ERROR           = 2,
  ERRORTEXT           = 3,

  GET_VOLUME          = 4,
  SET_VOLUME          = 5,
  GET_MINUTES         = 6,
  SET_MINUTES         = 7,

  INFO_SUBCODES
}
INFO_SUBCODE;

//! \endverbatim


//! \page soundsubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  BREAK               = 0,
  TONE                = 1,
  PLAY                = 2,
  REPEAT              = 3,
  SERVICE             = 4,

  SOUND_SUBCODES
}
SOUND_SUBCODE;

//! \endverbatim


//! \page stringsubcode Specific command parameter
//!
//!
//! \verbatim
//!

typedef   enum
{
  GET_SIZE            = 1,    // VM       get string size
  ADD                 = 2,    // VM       add two strings
  COMPARE             = 3,    // VM       compare two strings
  DUPLICATE           = 5,    // VM       duplicate one string to another
  VALUE_TO_STRING     = 6,
  STRING_TO_VALUE     = 7,
  STRIP               = 8,
  NUMBER_TO_STRING    = 9,
  SUB                 = 10,
  VALUE_FORMATTED     = 11,
  NUMBER_FORMATTED    = 12,

  STRING_SUBCODES
}
STRING_SUBCODE;

//! \endverbatim


/*! \page programid Program ID's (Slots)

    \anchor prgid

    \verbatim */

typedef   enum
{
  GUI_SLOT                      = 0,    //!< Program slot reserved for executing the user interface
  USER_SLOT                     = 1,    //!< Program slot used to execute user projects, apps and tools
  CMD_SLOT                      = 2,    //!< Program slot used for direct commands coming from c_com
  TERM_SLOT                     = 3,    //!< Program slot used for direct commands coming from c_ui
  DEBUG_SLOT                    = 4,    //!< Program slot used to run the debug ui

  SLOTS,                                //!< Maximum slots supported by the VM

  // ONLY VALID IN opPROGRAM_STOP
  CURRENT_SLOT                  = -1
}
SLOT;

/*  \endverbatim */


/*! \page buttons Button

    \verbatim */

typedef   enum
{
  NO_BUTTON                     = 0,
  UP_BUTTON                     = 1,
  ENTER_BUTTON                  = 2,
  DOWN_BUTTON                   = 3,
  RIGHT_BUTTON                  = 4,
  LEFT_BUTTON                   = 5,
  BACK_BUTTON                   = 6,
  ANY_BUTTON                    = 7,

  BUTTONTYPES                   = 8
}
BUTTONTYPE;

/*  \endverbatim */


/*! \page mathsubcode Specific command parameter

    \verbatim */

typedef   enum
{
  EXP                           = 1,    //!< e^x            r = expf(x)
  MOD                           = 2,    //!< Modulo         r = fmod(x,y)
  FLOOR                         = 3,    //!< Floor          r = floor(x)
  CEIL                          = 4,    //!< Ceiling        r = ceil(x)
  ROUND                         = 5,    //!< Round          r = round(x)
  ABS                           = 6,    //!< Absolute       r = fabs(x)
  NEGATE                        = 7,    //!< Negate         r = 0.0 - x
  SQRT                          = 8,    //!< Squareroot     r = sqrt(x)
  LOG                           = 9,    //!< Log            r = log10(x)
  LN                            = 10,   //!< Ln             r = log(x)
  SIN                           = 11,   //!<
  COS                           = 12,   //!<
  TAN                           = 13,   //!<
  ASIN                          = 14,   //!<
  ACOS                          = 15,   //!<
  ATAN                          = 16,   //!<
  MOD8                          = 17,   //!< Modulo DATA8   r = x % y
  MOD16                         = 18,   //!< Modulo DATA16  r = x % y
  MOD32                         = 19,   //!< Modulo DATA32  r = x % y
  POW                           = 20,   //!< Exponent       r = powf(x,y)
  TRUNC                         = 21,   //!< Truncate       r = (float)((int)(x * pow(y))) / pow(y)

  MATHTYPES                             //!< Maximum number of math functions supported by the VM
}
MATHTYPE;

/*  \endverbatim */


//! \page tstsubcode Specific command parameter
//!
//! \verbatim
//!

typedef   enum
{
  TST_OPEN                      = 10,   //!< MUST BE GREATER OR EQUAL TO "INFO_SUBCODES"
  TST_CLOSE                     = 11,
  TST_READ_PINS                 = 12,
  TST_WRITE_PINS                = 13,
  TST_READ_ADC                  = 14,
  TST_WRITE_UART                = 15,
  TST_READ_UART                 = 16,
  TST_ENABLE_UART               = 17,
  TST_DISABLE_UART              = 18,
  TST_ACCU_SWITCH               = 19,
  TST_BOOT_MODE2                = 20,
  TST_POLL_MODE2                = 21,
  TST_CLOSE_MODE2               = 22,
  TST_RAM_CHECK                 = 23,

  TST_SUBCODES
}
TST_SUBCODE;

//! \endverbatim


/*! \page browsers Browser Types Avaliable

    \verbatim */

typedef   enum
{
  BROWSE_FOLDERS                = 0,    //!< Browser for folders
  BROWSE_FOLDS_FILES            = 1,    //!< Browser for folders and files
  BROWSE_CACHE                  = 2,    //!< Browser for cached / recent files
  BROWSE_FILES                  = 3,    //!< Browser for files

  BROWSERTYPES                          //!< Maximum font types supported by the VM
}
BROWSERTYPE;

/*  \endverbatim */


/*! \page fonts Font Types Avaliable

    \verbatim */

typedef   enum
{
  NORMAL_FONT                   = 0,
  SMALL_FONT                    = 1,
  LARGE_FONT                    = 2,
  TINY_FONT                     = 3,

  FONTTYPES                             //!< Maximum font types supported by the VM
}
FONTTYPE;

/*  \endverbatim */


/*! \page icons Icon Types Avaliable

    \verbatim */

typedef   enum
{
  NORMAL_ICON                   = 0,    //!< "24x12_Files_Folders_Settings.bmp"
  SMALL_ICON                    = 1,
  LARGE_ICON                    = 2,    //!< "24x22_Yes_No_OFF_FILEOps.bmp"
  MENU_ICON                     = 3,
  ARROW_ICON                    = 4,    //!< "8x12_miniArrows.bmp"

  ICONTYPES                             //!< Maximum icon types supported by the VM
}
ICONTYPE;


typedef   enum
{
  SICON_CHARGING                = 0,
  SICON_BATT_4                  = 1,
  SICON_BATT_3                  = 2,
  SICON_BATT_2                  = 3,
  SICON_BATT_1                  = 4,
  SICON_BATT_0                  = 5,
  SICON_WAIT1                   = 6,
  SICON_WAIT2                   = 7,
  SICON_BT_ON                   = 8,
  SICON_BT_VISIBLE              = 9,
  SICON_BT_CONNECTED            = 10,
  SICON_BT_CONNVISIB            = 11,
  SICON_WIFI_3                  = 12,
  SICON_WIFI_2                  = 13,
  SICON_WIFI_1                  = 14,
  SICON_WIFI_CONNECTED          = 15,

  SICON_USB                     = 21,

  S_ICON_NOS
}
S_ICON_NO;


typedef   enum
{
  ICON_NONE                     = -1,
  ICON_RUN                      = 0,
  ICON_FOLDER                   = 1,
  ICON_FOLDER2                  = 2,
  ICON_USB                      = 3,
  ICON_SD                       = 4,
  ICON_SOUND                    = 5,
  ICON_IMAGE                    = 6,
  ICON_SETTINGS                 = 7,
  ICON_ONOFF                    = 8,
  ICON_SEARCH                   = 9,
  ICON_WIFI                     = 10,
  ICON_CONNECTIONS              = 11,
  ICON_ADD_HIDDEN               = 12,
  ICON_TRASHBIN                 = 13,
  ICON_VISIBILITY               = 14,
  ICON_KEY                      = 15,
  ICON_CONNECT                  = 16,
  ICON_DISCONNECT               = 17,
  ICON_UP                       = 18,
  ICON_DOWN                     = 19,
  ICON_WAIT1                    = 20,
  ICON_WAIT2                    = 21,
  ICON_BLUETOOTH                = 22,
  ICON_INFO                     = 23,
  ICON_TEXT                     = 24,


  ICON_QUESTIONMARK             = 27,
  ICON_INFO_FILE                = 28,
  ICON_DISC                     = 29,
  ICON_CONNECTED                = 30,
  ICON_OBP                      = 31,
  ICON_OBD                      = 32,
  ICON_OPENFOLDER               = 33,
  ICON_BRICK1                   = 34,
  N_ICON_NOS
}
N_ICON_NO;


typedef   enum
{
  YES_NOTSEL                    = 0,
  YES_SEL                       = 1,
  NO_NOTSEL                     = 2,
  NO_SEL                        = 3,
  OFF                           = 4,
  WAIT_VERT                     = 5,
  WAIT_HORZ                     = 6,
  TO_MANUAL                     = 7,
  WARNSIGN                      = 8,
  WARN_BATT                     = 9,
  WARN_POWER                    = 10,
  WARN_TEMP                     = 11,
  NO_USBSTICK                   = 12,
  TO_EXECUTE                    = 13,
  TO_BRICK                      = 14,
  TO_SDCARD                     = 15,
  TO_USBSTICK                   = 16,
  TO_BLUETOOTH                  = 17,
  TO_WIFI                       = 18,
  TO_TRASH                      = 19,
  TO_COPY                       = 20,
  TO_FILE                       = 21,
  CHAR_ERROR                    = 22,
  COPY_ERROR                    = 23,
  PROGRAM_ERROR                 = 24,


  WARN_MEMORY                   = 27,
  L_ICON_NOS
}
L_ICON_NO;


typedef   enum
{
  ICON_STAR                     = 0,
  ICON_LOCKSTAR                 = 1,
  ICON_LOCK                     = 2,
  ICON_PC                       = 3,    //!< Bluetooth type PC
  ICON_PHONE                    = 4,    //!< Bluetooth type PHONE
  ICON_BRICK                    = 5,    //!< Bluetooth type BRICK
  ICON_UNKNOWN                  = 6,    //!< Bluetooth type UNKNOWN
  ICON_FROM_FOLDER              = 7,
  ICON_CHECKBOX                 = 8,
  ICON_CHECKED                  = 9,
  ICON_XED                      = 10,

  M_ICON_NOS
}
M_ICON_NO;


typedef   enum
{

  ICON_LEFT                     = 1,
  ICON_RIGHT                    = 2,

  A_ICON_NOS
}
A_ICON_NO;


/*  \endverbatim */


/*! \page bttypes Bluetooth Device Types

    \verbatim */

typedef   enum
{
  BTTYPE_PC                     = 3,    //!< Bluetooth type PC
  BTTYPE_PHONE                  = 4,    //!< Bluetooth type PHONE
  BTTYPE_BRICK                  = 5,    //!< Bluetooth type BRICK
  BTTYPE_UNKNOWN                = 6,    //!< Bluetooth type UNKNOWN

  BTTYPES
}
BTTYPE;


/*  \endverbatim */


/*! \page ledpatterns LED Pattern

    \verbatim */

typedef   enum
{
  LED_BLACK                     = 0,
  LED_GREEN                     = 1,
  LED_RED                       = 2,
  LED_ORANGE                    = 3,
  LED_GREEN_FLASH               = 4,
  LED_RED_FLASH                 = 5,
  LED_ORANGE_FLASH              = 6,
  LED_GREEN_PULSE               = 7,
  LED_RED_PULSE                 = 8,
  LED_ORANGE_PULSE              = 9,

  LEDPATTERNS
}
LEDPATTERN;


/*  \endverbatim */


typedef   enum
{
  LED_ALL                       = 0,        //!< All LEDs
  LED_RR                        = 1,        //!< Right red
  LED_RG                        = 2,        //!< Right green
  LED_LR                        = 3,        //!< Left red
  LED_LG                        = 4         //!< Left green
}
LEDTYPE;


/*! \page filetypes File Types Avaliable

    \verbatim */


typedef   enum
{
  FILETYPE_UNKNOWN              = 0x00,
  TYPE_FOLDER                   = 0x01,
  TYPE_SOUND                    = 0x02,
  TYPE_BYTECODE                 = 0x03,
  TYPE_GRAPHICS                 = 0x04,
  TYPE_DATALOG                  = 0x05,
  TYPE_PROGRAM                  = 0x06,
  TYPE_TEXT                     = 0x07,
  TYPE_SDCARD                   = 0x10,
  TYPE_USBSTICK                 = 0x20,

  FILETYPES,                                //!< Maximum icon types supported by the VM

  TYPE_RESTART_BROWSER          = -1,
  TYPE_REFRESH_BROWSER          = -2
}
FILETYPE;

/*  \endverbatim */


/*! \page results Results

    Describes result from executing functions

    \verbatim */


typedef   enum
{
  OK            = 0,                    //!< No errors to report
  BUSY          = 1,                    //!< Busy - try again
  FAIL          = 2,                    //!< Something failed
  STOP          = 4                     //!< Stopped
}
RESULT;

/*  \endverbatim */


/*! \page formats
 *
 *        Data formats used in device type formats
 *  \verbatim
 */

typedef   enum
{
  DATA_8        = 0x00,                 //!< DATA8  (don't change)
  DATA_16       = 0x01,                 //!< DATA16 (don't change)
  DATA_32       = 0x02,                 //!< DATA32 (don't change)
  DATA_F        = 0x03,                 //!< DATAF  (don't change)
  DATA_S        = 0x04,                 //!< Zero terminated string
  DATA_A        = 0x05,                 //!< Array handle

  DATA_V        = 0x07,                 //!< Variable type

  DATA_PCT      = 0x10,                 //!< Percent (used in opINPUT_READEXT)
  DATA_RAW      = 0x12,                 //!< Raw     (used in opINPUT_READEXT)
  DATA_SI       = 0x13,                 //!< SI unit (used in opINPUT_READEXT)

  DATA_FORMATS
}
DATA_FORMAT;

/*\endverbatim
 *
 *  \n
 */


/*! \page delimiters
 *
 *        Delimiter codes used to define how data is separated in files
 *  \verbatim
 */

typedef   enum
{
  DEL_NONE      = 0,                    //!< No delimiter at all
  DEL_TAB       = 1,                    //!< Use tab as delimiter
  DEL_SPACE     = 2,                    //!< Use space as delimiter
  DEL_RETURN    = 3,                    //!< Use return as delimiter
  DEL_COLON     = 4,                    //!< Use colon as delimiter
  DEL_COMMA     = 5,                    //!< Use comma as delimiter
  DEL_LINEFEED  = 6,                    //!< Use line feed as delimiter
  DEL_CRLF      = 7,                    //!< Use return+line feed as delimiter

  DELS
}
DEL;

/*\endverbatim
 *
 *  \n
 */


/*! \page transportlayers Hardware Transport Layer

    \verbatim */

typedef   enum
{
  HW_USB                        = 1,
  HW_BT                         = 2,
  HW_WIFI                       = 3,

  HWTYPES
}
HWTYPE;

/*  \endverbatim */


/*! \page encryptions Encryption Types

    \verbatim */

typedef   enum
{
  ENCRYPT_NONE                  = 0,
  ENCRYPT_WPA2                  = 1,

  ENCRYPTS
}
ENCRYPT;

/*  \endverbatim */


typedef   enum
{
  MODE_KEEP     = -1,
  TYPE_KEEP     =  0,

  MIXS
}
MIX;


typedef   enum
{
  RED           =  0,
  GREEN         =  1,
  BLUE          =  2,
  BLANK         =  3,
  COLORS
}
COLOR;


/* Constants related to color sensor value using */
/* Color sensor as color detector                */
typedef   enum
{
  BLACKCOLOR   = 1,
  BLUECOLOR    = 2,
  GREENCOLOR   = 3,
  YELLOWCOLOR  = 4,
  REDCOLOR     = 5,
  WHITECOLOR   = 6
}
NXTCOLOR;


typedef   enum
{
  WARNING_TEMP      = 0x01,
  WARNING_CURRENT   = 0x02,
  WARNING_VOLTAGE   = 0x04,
  WARNING_MEMORY    = 0x08,
  WARNING_DSPSTAT   = 0x10,

  WARNING_BATTLOW   = 0x40,
  WARNING_BUSY      = 0x80,

  WARNINGS          = 0x3F
}
WARNING;


#define   DATA8_NAN     ((DATA8)(-128))
#define   DATA16_NAN    ((DATA16)(-32768))
#define   DATA32_NAN    ((DATA32)(0x80000000))
#define   DATAF_NAN     ((float)0 / (float)0) //(0x7FC00000)

#define   DATA8_MIN     (-127)
#define   DATA8_MAX     (127)
#define   DATA16_MIN    (-32767)
#define   DATA16_MAX    (32767)
#define   DATA32_MIN    (-2147483647)
#define   DATA32_MAX    (2147483647)
#define   DATAF_MIN     (-2147483647)
#define   DATAF_MAX     (2147483647)




/*! \enum OBJSTAT
 *
 *        Values used to describe an object's status
 */
typedef   enum
{
  RUNNING = 0x0010,                     //!< Object code is running
  WAITING = 0x0020,                     //!< Object is waiting for final trigger
  STOPPED = 0x0040,                     //!< Object is stopped or not triggered yet
  HALTED  = 0x0080,                     //!< Object is halted because a call is in progress
}
OBJSTAT;


/*! \page devicecommands
 *
 *        Device commands used to control (UART sensors) devices
 *  \verbatim
 */

typedef   enum
{
  DEVCMD_RESET        = 0x11,           //!< UART device reset
  DEVCMD_FIRE         = 0x11,           //!< UART device fire   (ultrasonic)
  DEVCMD_CHANNEL      = 0x12,           //!< UART device channel (IR seeker)

  DEVCMDS
}
DEVCMD;

/*\endverbatim
 *
 *  \n
 */


// GRAPHICS

#define   vmPOP3_ABS_X                  16        //
#define   vmPOP3_ABS_Y                  50        //

#define   vmPOP3_ABS_WARN_ICON_X        64
#define   vmPOP3_ABS_WARN_ICON_X1       40
#define   vmPOP3_ABS_WARN_ICON_X2       72
#define   vmPOP3_ABS_WARN_ICON_X3       104
#define   vmPOP3_ABS_WARN_ICON_Y        60
#define   vmPOP3_ABS_WARN_SPEC_ICON_X   88
#define   vmPOP3_ABS_WARN_SPEC_ICON_Y   60
#define   vmPOP3_ABS_WARN_TEXT_X        80
#define   vmPOP3_ABS_WARN_TEXT_Y        68
#define   vmPOP3_ABS_WARN_YES_X         72
#define   vmPOP3_ABS_WARN_YES_Y         90
#define   vmPOP3_ABS_WARN_LINE_X        21
#define   vmPOP3_ABS_WARN_LINE_Y        89
#define   vmPOP3_ABS_WARN_LINE_ENDX     155


#define   LONGToBytes(_x)               (UBYTE)((_x) & 0xFF),(UBYTE)((_x >> 8) & 0xFF),(UBYTE)((_x >> 16) & 0xFF),(UBYTE)((_x >> 24) & 0xFF)
#define   WORDToBytes(_x)               (UBYTE)((_x) & 0xFF),(UBYTE)((_x >> 8) & 0xFF)
#define   BYTEToBytes(_x)               (UBYTE)((_x) & 0xFF)

#define   PROGRAMHeader(VersionInfo,NumberOfObjects,GlobalBytes)\
                                        'L','E','G','O',LONGToBytes(0),WORDToBytes((UWORD)(BYTECODE_VERSION * 100.0)),WORDToBytes(NumberOfObjects),LONGToBytes(GlobalBytes)

#define   VMTHREADHeader(OffsetToInstructions,LocalBytes)\
                                        LONGToBytes(OffsetToInstructions),0,0,0,0,LONGToBytes(LocalBytes)

#define   SUBCALLHeader(OffsetToInstructions,LocalBytes)\
                                        LONGToBytes(OffsetToInstructions),0,0,1,0,LONGToBytes(LocalBytes)

#define   BLOCKHeader(OffsetToInstructions,OwnerObjectId,TriggerCount)\
                                        LONGToBytes(OffsetToInstructions),WORDToBytes(OwnerObjectId),WORDToBytes(TriggerCount),LONGToBytes(0)

//        MACROS FOR PRIMITIVES AND SYSTEM CALLS

#define   PRIMPAR_SHORT                 0x00
#define   PRIMPAR_LONG                  0x80

#define   PRIMPAR_CONST                 0x00
#define   PRIMPAR_VARIABEL              0x40
#define   PRIMPAR_LOCAL                 0x00
#define   PRIMPAR_GLOBAL                0x20
#define   PRIMPAR_HANDLE                0x10
#define   PRIMPAR_ADDR                  0x08

#define   PRIMPAR_INDEX                 0x1F
#define   PRIMPAR_CONST_SIGN            0x20
#define   PRIMPAR_VALUE                 0x3F

#define   PRIMPAR_BYTES                 0x07

#define   PRIMPAR_STRING_OLD            0
#define   PRIMPAR_1_BYTE                1
#define   PRIMPAR_2_BYTES               2
#define   PRIMPAR_4_BYTES               3
#define   PRIMPAR_STRING                4

#define   PRIMPAR_LABEL                 0x20

#define   HND(x)                        PRIMPAR_HANDLE | x
#define   ADR(x)                        PRIMPAR_ADDR | x

#define   LCS                           (PRIMPAR_LONG | PRIMPAR_STRING)

#define   LAB1(v)                       (PRIMPAR_LONG | PRIMPAR_LABEL),(v & 0xFF)

#define   LC0(v)                        ((v & PRIMPAR_VALUE) | PRIMPAR_SHORT | PRIMPAR_CONST)
#define   LC1(v)                        (PRIMPAR_LONG  | PRIMPAR_CONST | PRIMPAR_1_BYTE),(v & 0xFF)
#define   LC2(v)                        (PRIMPAR_LONG  | PRIMPAR_CONST | PRIMPAR_2_BYTES),(v & 0xFF),((v >> 8) & 0xFF)
#define   LC4(v)                        (PRIMPAR_LONG  | PRIMPAR_CONST | PRIMPAR_4_BYTES),((ULONG)v & 0xFF),(((ULONG)v >> (ULONG)8) & 0xFF),(((ULONG)v >> (ULONG)16) & 0xFF),(((ULONG)v >> (ULONG)24) & 0xFF)
#define   LCA(h)                        (PRIMPAR_LONG  | PRIMPAR_CONST | PRIMPAR_1_BYTE | PRIMPAR_ARRAY),(i & 0xFF)

#define   LV0(i)                        ((i & PRIMPAR_INDEX) | PRIMPAR_SHORT | PRIMPAR_VARIABEL | PRIMPAR_LOCAL)
#define   LV1(i)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_LOCAL | PRIMPAR_1_BYTE),(i & 0xFF)
#define   LV2(i)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_LOCAL | PRIMPAR_2_BYTES),(i & 0xFF),((i >> 8) & 0xFF)
#define   LV4(i)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_LOCAL | PRIMPAR_4_BYTES),(i & 0xFF),((i >> 8) & 0xFF),((i >> 16) & 0xFF),((i >> 24) & 0xFF)
#define   LVA(h)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_LOCAL | PRIMPAR_1_BYTE | PRIMPAR_ARRAY),(i & 0xFF)

#define   GV0(i)                        ((i & PRIMPAR_INDEX) | PRIMPAR_SHORT | PRIMPAR_VARIABEL | PRIMPAR_GLOBAL)
#define   GV1(i)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_GLOBAL | PRIMPAR_1_BYTE),(i & 0xFF)
#define   GV2(i)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_GLOBAL | PRIMPAR_2_BYTES),(i & 0xFF),((i >> 8) & 0xFF)
#define   GV4(i)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_GLOBAL | PRIMPAR_4_BYTES),(i & 0xFF),((i >> 8) & 0xFF),((i >> 16) & 0xFF),((i >> 24) & 0xFF)
#define   GVA(h)                        (PRIMPAR_LONG  | PRIMPAR_VARIABEL | PRIMPAR_GLOBAL | PRIMPAR_1_BYTE | PRIMPAR_ARRAY),(i & 0xFF)

//        MACROS FOR SUB CALLS


#define   CALLPAR_IN                    0x80
#define   CALLPAR_OUT                   0x40

#define   CALLPAR_TYPE                  0x07
#define   CALLPAR_DATA8                 DATA_8
#define   CALLPAR_DATA16                DATA_16
#define   CALLPAR_DATA32                DATA_32
#define   CALLPAR_DATAF                 DATA_F
#define   CALLPAR_STRING                DATA_S

#define   IN_8                          (CALLPAR_IN  | CALLPAR_DATA8)
#define   IN_16                         (CALLPAR_IN  | CALLPAR_DATA16)
#define   IN_32                         (CALLPAR_IN  | CALLPAR_DATA32)
#define   IN_F                          (CALLPAR_IN  | CALLPAR_DATAF)
#define   IN_S                          (CALLPAR_IN  | CALLPAR_STRING)
#define   OUT_8                         (CALLPAR_OUT | CALLPAR_DATA8)
#define   OUT_16                        (CALLPAR_OUT | CALLPAR_DATA16)
#define   OUT_32                        (CALLPAR_OUT | CALLPAR_DATA32)
#define   OUT_F                         (CALLPAR_OUT | CALLPAR_DATAF)
#define   OUT_S                         (CALLPAR_OUT | CALLPAR_STRING)

#define   IO_8                          IN_8  | OUT_8
#define   IO_16                         IN_16 | OUT_16
#define   IO_32                         IN_32 | OUT_32
#define   IO_F                          IN_F  | OUT_F
#define   IO_S                          IN_S  | OUT_S


#endif /* BYTECODES_H_ */
