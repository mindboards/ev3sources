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


#define   MAX_SUBCODES        33                //!< Max number of sub codes
#define   OPCODE_NAMESIZE     20                //!< Opcode and sub code name length
#define   MAX_LABELS          32                //!< Max number of labels per program


typedef   struct
{
  const   ULONG   Pars;                         //!< Contains parameter info nibbles (max 8)
  const   char    *Name;                        //!< Opcode name
}
OPCODE;

typedef   struct
{
  const   ULONG   Pars;                         //!< Contains parameter info nibbles (max 8)
  const   char    *Name;                        //!< Sub code name
}
SUBCODE;

#define   OC(OpCode,Par1,Par2,Par3,Par4,Par5,Par6,Par7,Par8)                  \
                                                                              \
                              [OpCode] =  {                                   \
                                            .Pars   = ((ULONG)Par1) +         \
                                                      ((ULONG)Par2 << 4) +    \
                                                      ((ULONG)Par3 << 8) +    \
                                                      ((ULONG)Par4 << 12) +   \
                                                      ((ULONG)Par5 << 16) +   \
                                                      ((ULONG)Par6 << 20) +   \
                                                      ((ULONG)Par7 << 24) +   \
                                                      ((ULONG)Par8 << 28),    \
                                            .Name   = #OpCode,                \
                                          }


#define   SC(ParameterFormat,SubCode,Par1,Par2,Par3,Par4,Par5,Par6,Par7,Par8) \
                                                                              \
            [ParameterFormat][SubCode] =  {                                   \
                                            .Pars   = ((ULONG)Par1) +         \
                                                      ((ULONG)Par2 << 4) +    \
                                                      ((ULONG)Par3 << 8) +    \
                                                      ((ULONG)Par4 << 12) +   \
                                                      ((ULONG)Par5 << 16) +   \
                                                      ((ULONG)Par6 << 20) +   \
                                                      ((ULONG)Par7 << 24) +   \
                                                      ((ULONG)Par8 << 28),    \
                                            .Name   = #SubCode,               \
                                          }


#define   SUBP                0x01              //!< Next nibble is sub parameter table no
#define   PARNO               0x02              //!< Defines no of following parameters
#define   PARLAB              0x03              //!< Defines label no
#define   PARVALUES           0x04              //!< Last parameter defines number of values to follow

#define   PAR                 0x08              //!< Plain  parameter as below:
#define   PAR8                (PAR + DATA_8)    //!< DATA8  parameter
#define   PAR16               (PAR + DATA_16)   //!< DATA16 parameter
#define   PAR32               (PAR + DATA_32)   //!< DATA32 parameter
#define   PARF                (PAR + DATA_F)    //!< DATAF  parameter
#define   PARS                (PAR + DATA_S)    //!< DATAS  parameter
#define   PARV                (PAR + DATA_V)    //!< Parameter type variable

static  const char *ParTypeNames[] =
{
  [DATA_8]   = "DATA8",
  [DATA_16]  = "DATA16",
  [DATA_32]  = "DATA32",
  [DATA_F]   = "DATAF",
  [DATA_S]   = "STRING",
  [DATA_V]   = "UNKNOWN",
};


static  const DATA32 ParMin[] =
{
  [DATA_8]   = DATA8_MIN,
  [DATA_16]  = DATA16_MIN,
  [DATA_32]  = DATA32_MIN,
};


static  const DATA32 ParMax[] =
{
  [DATA_8]   = DATA8_MAX,
  [DATA_16]  = DATA16_MAX,
  [DATA_32]  = DATA32_MAX,
};


enum
{
  UI_READ_SUBP    = 0,
  UI_WRITE_SUBP   = 1,
  UI_DRAW_SUBP    = 2,
  UI_BUTTON_SUBP  = 3,
  FILE_SUBP       = 4,
  PROGRAM_SUBP    = 5,
  VM_SUBP         = 6,
  STRING_SUBP     = 7,
  COM_READ_SUBP   = 8,
  COM_WRITE_SUBP  = 9,
  SOUND_SUBP      = 10,
  INPUT_SUBP      = 11,
  ARRAY_SUBP      = 12,
  MATH_SUBP       = 13,
  COM_GET_SUBP    = 14,
  COM_SET_SUBP    = 15,
  FILENAME_SUBP   = 16,

  SUBPS
};


#define FILENAME_SUBP   ARRAY_SUBP
#define TST_SUBP        VM_SUBP


static OPCODE OpCodes[256] =
{
  //    OpCode                  Parameters                                      Unused
  //    VM
  OC(   opERROR,                0,                                              0,0,0,0,0,0,0         ),
  OC(   opNOP,                  0,                                              0,0,0,0,0,0,0         ),
  OC(   opPROGRAM_STOP,         PAR16,                                          0,0,0,0,0,0,0         ),
  OC(   opPROGRAM_START,        PAR16,PAR32,PAR32,PAR8,                         0,0,0,0               ),
  OC(   opOBJECT_STOP,          PAR16,                                          0,0,0,0,0,0,0         ),
  OC(   opOBJECT_START,         PAR16,                                          0,0,0,0,0,0,0         ),
  OC(   opOBJECT_TRIG,          PAR16,                                          0,0,0,0,0,0,0         ),
  OC(   opOBJECT_WAIT,          PAR16,                                          0,0,0,0,0,0,0         ),
  OC(   opRETURN,               0,                                              0,0,0,0,0,0,0         ),
  OC(   opCALL,                 PAR16,PARNO,                                    0,0,0,0,0,0           ),
  OC(   opOBJECT_END,           0,                                              0,0,0,0,0,0,0         ),
  OC(   opSLEEP,                0,                                              0,0,0,0,0,0,0         ),
  OC(   opPROGRAM_INFO,         PAR8, SUBP, PROGRAM_SUBP,                       0,0,0,0,0             ),
  OC(   opLABEL,                PARLAB,                                         0,0,0,0,0,0,0         ),
  OC(   opPROBE,                PAR16,PAR16,PAR32,PAR32,                        0,0,0,0               ),
  OC(   opDO,                   PAR16,PAR32,PAR32,                              0,0,0,0,0             ),
  //    Math
  OC(   opADD8,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opADD16,                PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opADD32,                PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opADDF,                 PARF,PARF,PARF,                                 0,0,0,0,0             ),
  OC(   opSUB8,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opSUB16,                PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opSUB32,                PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opSUBF,                 PARF,PARF,PARF,                                 0,0,0,0,0             ),
  OC(   opMUL8,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opMUL16,                PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opMUL32,                PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opMULF,                 PARF,PARF,PARF,                                 0,0,0,0,0             ),
  OC(   opDIV8,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opDIV16,                PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opDIV32,                PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opDIVF,                 PARF,PARF,PARF,                                 0,0,0,0,0             ),
  //    Logic
  OC(   opOR8,                  PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOR16,                 PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opOR32,                 PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opAND8,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opAND16,                PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opAND32,                PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opXOR8,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opXOR16,                PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opXOR32,                PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opRL8,                  PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opRL16,                 PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opRL32,                 PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  //    Move
  OC(   opINIT_BYTES,           PAR8,PAR32,PARVALUES,PAR8,                      0,0,0,0               ),
  OC(   opMOVE8_8,              PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opMOVE8_16,             PAR8,PAR16,                                     0,0,0,0,0,0           ),
  OC(   opMOVE8_32,             PAR8,PAR32,                                     0,0,0,0,0,0           ),
  OC(   opMOVE8_F,              PAR8,PARF,                                      0,0,0,0,0,0           ),
  OC(   opMOVE16_8,             PAR16,PAR8,                                     0,0,0,0,0,0           ),
  OC(   opMOVE16_16,            PAR16,PAR16,                                    0,0,0,0,0,0           ),
  OC(   opMOVE16_32,            PAR16,PAR32,                                    0,0,0,0,0,0           ),
  OC(   opMOVE16_F,             PAR16,PARF,                                     0,0,0,0,0,0           ),
  OC(   opMOVE32_8,             PAR32,PAR8,                                     0,0,0,0,0,0           ),
  OC(   opMOVE32_16,            PAR32,PAR16,                                    0,0,0,0,0,0           ),
  OC(   opMOVE32_32,            PAR32,PAR32,                                    0,0,0,0,0,0           ),
  OC(   opMOVE32_F,             PAR32,PARF,                                     0,0,0,0,0,0           ),
  OC(   opMOVEF_8,              PARF,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opMOVEF_16,             PARF,PAR16,                                     0,0,0,0,0,0           ),
  OC(   opMOVEF_32,             PARF,PAR32,                                     0,0,0,0,0,0           ),
  OC(   opMOVEF_F,              PARF,PARF,                                      0,0,0,0,0,0           ),
  //    Branch
  OC(   opJR,                   PAR32,                                          0,0,0,0,0,0,0         ),
  OC(   opJR_FALSE,             PAR8,PAR32,                                     0,0,0,0,0,0           ),
  OC(   opJR_TRUE,              PAR8,PAR32,                                     0,0,0,0,0,0           ),
  OC(   opJR_NAN,               PARF,PAR32,                                     0,0,0,0,0,0           ),
  //    Compare
  OC(   opCP_LT8,               PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_LT16,              PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_LT32,              PAR32,PAR32,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_LTF,               PARF,PARF,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_GT8,               PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_GT16,              PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_GT32,              PAR32,PAR32,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_GTF,               PARF,PARF,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_EQ8,               PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_EQ16,              PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_EQ32,              PAR32,PAR32,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_EQF,               PARF,PARF,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_NEQ8,              PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_NEQ16,             PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_NEQ32,             PAR32,PAR32,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_NEQF,              PARF,PARF,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_LTEQ8,             PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_LTEQ16,            PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_LTEQ32,            PAR32,PAR32,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_LTEQF,             PARF,PARF,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_GTEQ8,             PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCP_GTEQ16,            PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_GTEQ32,            PAR32,PAR32,PAR8,                               0,0,0,0,0             ),
  OC(   opCP_GTEQF,             PARF,PARF,PAR8,                                 0,0,0,0,0             ),
  //    Select
  OC(   opSELECT8,              PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  OC(   opSELECT16,             PAR8,PAR16,PAR16,PAR16,                         0,0,0,0               ),
  OC(   opSELECT32,             PAR8,PAR32,PAR32,PAR32,                         0,0,0,0               ),
  OC(   opSELECTF,              PAR8,PARF,PARF,PARF,                            0,0,0,0               ),

  OC(   opSYSTEM,               PAR8,PAR32,                                     0,0,0,0,0,0           ),
  OC(   opPORT_CNV_OUTPUT,      PAR32,PAR8,PAR8,PAR8,                           0,0,0,0               ),
  OC(   opPORT_CNV_INPUT,       PAR32,PAR8,PAR8,                                0,0,0,0,0             ),
  OC(   opNOTE_TO_FREQ,         PAR8,PAR16,                                     0,0,0,0,0,0           ),

  //    Branch
  OC(   opJR_LT8,               PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_LT16,              PAR16,PAR16,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_LT32,              PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_LTF,               PARF,PARF,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_GT8,               PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_GT16,              PAR16,PAR16,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_GT32,              PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_GTF,               PARF,PARF,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_EQ8,               PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_EQ16,              PAR16,PAR16,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_EQ32,              PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_EQF,               PARF,PARF,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_NEQ8,              PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_NEQ16,             PAR16,PAR16,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_NEQ32,             PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_NEQF,              PARF,PARF,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_LTEQ8,             PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_LTEQ16,            PAR16,PAR16,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_LTEQ32,            PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_LTEQF,             PARF,PARF,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_GTEQ8,             PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opJR_GTEQ16,            PAR16,PAR16,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_GTEQ32,            PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  OC(   opJR_GTEQF,             PARF,PARF,PAR32,                                0,0,0,0,0             ),
  //    VM
  OC(   opINFO,                 PAR8,SUBP,VM_SUBP,                              0,0,0,0,0             ),
  OC(   opSTRINGS,              PAR8,SUBP,STRING_SUBP,                          0,0,0,0,0             ),
  OC(   opMEMORY_WRITE,         PAR16,PAR16,PAR32,PAR32,PAR8,                   0,0,0                 ),
  OC(   opMEMORY_READ,          PAR16,PAR16,PAR32,PAR32,PAR8,                   0,0,0                 ),
  //    UI
  OC(   opUI_FLUSH,             0,                                              0,0,0,0,0,0,0         ),
  OC(   opUI_READ,              PAR8,SUBP,UI_READ_SUBP,                         0,0,0,0,0             ),
  OC(   opUI_WRITE,             PAR8,SUBP,UI_WRITE_SUBP,                        0,0,0,0,0             ),
  OC(   opUI_BUTTON,            PAR8,SUBP,UI_BUTTON_SUBP,                       0,0,0,0,0             ),
  OC(   opUI_DRAW,              PAR8,SUBP,UI_DRAW_SUBP,                         0,0,0,0,0             ),
  //    Timer
  OC(   opTIMER_WAIT,           PAR32,PAR32,                                    0,0,0,0,0,0           ),
  OC(   opTIMER_READY,          PAR32,                                          0,0,0,0,0,0,0         ),
  OC(   opTIMER_READ,           PAR32,                                          0,0,0,0,0,0,0         ),
  //    VM
  OC(   opBP0,                  0,                                              0,0,0,0,0,0,0         ),
  OC(   opBP1,                  0,                                              0,0,0,0,0,0,0         ),
  OC(   opBP2,                  0,                                              0,0,0,0,0,0,0         ),
  OC(   opBP3,                  0,                                              0,0,0,0,0,0,0         ),
  OC(   opBP_SET,               PAR16,PAR8,PAR32,                               0,0,0,0,0             ),
  OC(   opMATH,                 PAR8,SUBP,MATH_SUBP,                            0,0,0,0,0             ),
  OC(   opRANDOM,               PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  OC(   opTIMER_READ_US,        PAR32,                                          0,0,0,0,0,0,0         ),
  OC(   opKEEP_ALIVE,           PAR8,                                           0,0,0,0,0,0,0         ),
  //    Com
  OC(   opCOM_READ,             PAR8,SUBP,COM_READ_SUBP,                        0,0,0,0,0             ),
  OC(   opCOM_WRITE,            PAR8,SUBP,COM_WRITE_SUBP,                       0,0,0,0,0             ),
  //    Sound
  OC(   opSOUND,                PAR8,SUBP,SOUND_SUBP,                           0,0,0,0,0             ),
  OC(   opSOUND_TEST,           PAR8,                                           0,0,0,0,0,0,0         ),
  OC(   opSOUND_READY,          0,                                              0,0,0,0,0,0,0         ),
  //    Input
  OC(   opINPUT_SAMPLE,         PAR32,PAR16,PAR16,PAR8,PAR8,PAR8,PAR8,PARF                            ),
  OC(   opINPUT_DEVICE_LIST,    PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opINPUT_DEVICE,         PAR8,SUBP,INPUT_SUBP,                           0,0,0,0,0             ),
  OC(   opINPUT_READ,           PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  OC(   opINPUT_READSI,         PAR8,PAR8,PAR8,PAR8,PARF,                       0,0,0                 ),
  OC(   opINPUT_TEST,           PAR8,                                           0,0,0,0,0,0,0         ),
  OC(   opINPUT_TEST,           PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opINPUT_READY,          PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opINPUT_READEXT,        PAR8,PAR8,PAR8,PAR8,PAR8,PARNO,                 0,0                   ),
  OC(   opINPUT_WRITE,          PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  //    Output
  OC(   opOUTPUT_SET_TYPE,      PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOUTPUT_RESET,         PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opOUTPUT_STOP,          PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOUTPUT_SPEED,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOUTPUT_POWER,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOUTPUT_START,         PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opOUTPUT_POLARITY,      PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOUTPUT_READ,          PAR8,PAR8,PAR8,PAR32,                           0,0,0,0               ),
  OC(   opOUTPUT_READY,         PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opOUTPUT_TEST,          PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opOUTPUT_STEP_POWER,    PAR8,PAR8,PAR8,PAR32,PAR32,PAR32,PAR8,          0                     ),
  OC(   opOUTPUT_TIME_POWER,    PAR8,PAR8,PAR8,PAR32,PAR32,PAR32,PAR8,          0                     ),
  OC(   opOUTPUT_STEP_SPEED,    PAR8,PAR8,PAR8,PAR32,PAR32,PAR32,PAR8,          0                     ),
  OC(   opOUTPUT_TIME_SPEED,    PAR8,PAR8,PAR8,PAR32,PAR32,PAR32,PAR8,          0                     ),
  OC(   opOUTPUT_STEP_SYNC,     PAR8,PAR8,PAR8,PAR16,PAR32,PAR8,                0,0                   ),
  OC(   opOUTPUT_TIME_SYNC,     PAR8,PAR8,PAR8,PAR16,PAR32,PAR8,                0,0                   ),
  OC(   opOUTPUT_CLR_COUNT,     PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opOUTPUT_GET_COUNT,     PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  OC(   opOUTPUT_PRG_STOP,      0,                                              0,0,0,0,0,0,0         ),
  //    Memory
  OC(   opFILE,                 PAR8,SUBP,FILE_SUBP,                            0,0,0,0,0             ),
  OC(   opARRAY,                PAR8,SUBP,ARRAY_SUBP,                           0,0,0,0,0             ),
  OC(   opARRAY_WRITE,          PAR16,PAR32,PARV,                               0,0,0,0,0             ),
  OC(   opARRAY_READ,           PAR16,PAR32,PARV,                               0,0,0,0,0             ),
  OC(   opARRAY_APPEND,         PAR16,PARV,                                     0,0,0,0,0,0           ),
  OC(   opMEMORY_USAGE,         PAR32,PAR32,                                    0,0,0,0,0,0           ),
  OC(   opFILENAME,             PAR8,SUBP,FILENAME_SUBP,                        0,0,0,0,0             ),
  //    Move
  OC(   opREAD8,                PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opREAD16,               PAR16,PAR8,PAR16,                               0,0,0,0,0             ),
  OC(   opREAD32,               PAR32,PAR8,PAR32,                               0,0,0,0,0             ),
  OC(   opREADF,                PARF,PAR8,PARF,                                 0,0,0,0,0             ),
  OC(   opWRITE8,               PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opWRITE16,              PAR16,PAR8,PAR16,                               0,0,0,0,0             ),
  OC(   opWRITE32,              PAR32,PAR8,PAR32,                               0,0,0,0,0             ),
  OC(   opWRITEF,               PARF,PAR8,PARF,                                 0,0,0,0,0             ),
  //    Com
  OC(   opCOM_READY,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opCOM_READDATA,         PAR8,PAR8,PAR16,PAR8,                           0,0,0,0               ),
  OC(   opCOM_WRITEDATA,        PAR8,PAR8,PAR16,PAR8,                           0,0,0,0               ),
  OC(   opCOM_GET,              PAR8,SUBP,COM_GET_SUBP,                         0,0,0,0,0             ),
  OC(   opCOM_SET,              PAR8,SUBP,COM_SET_SUBP,                         0,0,0,0,0             ),
  OC(   opCOM_TEST,             PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  OC(   opCOM_REMOVE,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opCOM_WRITEFILE,        PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),

  OC(   opMAILBOX_OPEN,         PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  OC(   opMAILBOX_WRITE,        PAR8,PAR8,PAR8,PAR8,PARNO,                      0,0,0                 ),
  OC(   opMAILBOX_READ,         PAR8,PAR8,PARNO,                                0,0,0,0,0             ),
  OC(   opMAILBOX_TEST,         PAR8,PAR8,                                      0,0,0,0,0,0           ),
  OC(   opMAILBOX_READY,        PAR8,                                           0,0,0,0,0,0,0         ),
  OC(   opMAILBOX_CLOSE,        PAR8,                                           0,0,0,0,0,0,0         ),
  //    Test
  OC(   opTST,                  PAR8,SUBP,TST_SUBP,                             0,0,0,0,0             ),


};


static  SUBCODE SubCodes[SUBPS][MAX_SUBCODES] =
{
  //    ParameterFormat         SubCode                 Parameters                                      Unused

  //    VM
  SC(   PROGRAM_SUBP,           OBJ_STOP,               PAR16,PAR16,                                    0,0,0,0,0,0           ),
  SC(   PROGRAM_SUBP,           OBJ_START,              PAR16,PAR16,                                    0,0,0,0,0,0           ),
  SC(   PROGRAM_SUBP,           GET_STATUS,             PAR16,PAR8,                                     0,0,0,0,0,0           ),
  SC(   PROGRAM_SUBP,           GET_SPEED,              PAR16,PAR32,                                    0,0,0,0,0,0           ),
  SC(   PROGRAM_SUBP,           GET_PRGRESULT,          PAR16,PAR8,                                     0,0,0,0,0,0           ),
  SC(   PROGRAM_SUBP,           SET_INSTR,              PAR16,                                          0,0,0,0,0,0,0         ),
  //    Memory
  SC(   FILE_SUBP,              OPEN_APPEND,            PAR8,PAR16,                                     0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              OPEN_READ,              PAR8,PAR16,PAR32,                               0,0,0,0,0             ),
  SC(   FILE_SUBP,              OPEN_WRITE,             PAR8,PAR16,                                     0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              READ_VALUE,             PAR16,PAR8,PARF,                                0,0,0,0,0             ),
  SC(   FILE_SUBP,              WRITE_VALUE,            PAR16,PAR8,PARF,PAR8,PAR8,                      0,0,0                 ),
  SC(   FILE_SUBP,              READ_TEXT,              PAR16,PAR8,PAR16,PAR8,                          0,0,0,0               ),
  SC(   FILE_SUBP,              WRITE_TEXT,             PAR16,PAR8,PAR8,                                0,0,0,0,0             ),
  SC(   FILE_SUBP,              CLOSE,                  PAR16,                                          0,0,0,0,0,0,0         ),
  SC(   FILE_SUBP,              LOAD_IMAGE,             PAR16,PAR8,PAR32,PAR32,                         0,0,0,0               ),
  SC(   FILE_SUBP,              GET_HANDLE,             PAR8,PAR16,PAR8,                                0,0,0,0,0             ),
  SC(   FILE_SUBP,              MAKE_FOLDER,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              GET_LOG_NAME,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              GET_POOL,               PAR32,PAR16,PAR32,                              0,0,0,0,0             ),
  SC(   FILE_SUBP,              GET_FOLDERS,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              GET_SUBFOLDER_NAME,     PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   FILE_SUBP,              WRITE_LOG,              PAR16,PAR32,PAR8,PARF,                          0,0,0,0               ),
  SC(   FILE_SUBP,              CLOSE_LOG,              PAR16,PAR8,                                     0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              SET_LOG_SYNC_TIME,      PAR32,PAR32,                                    0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              DEL_SUBFOLDER,          PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              GET_LOG_SYNC_TIME,      PAR32,PAR32,                                    0,0,0,0,0,0           ),
  SC(   FILE_SUBP,              GET_IMAGE,              PAR8,PAR16,PAR8,PAR32,                          0,0,0,0               ),
  SC(   FILE_SUBP,              GET_ITEM,               PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   FILE_SUBP,              GET_CACHE_FILES,        PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   FILE_SUBP,              GET_CACHE_FILE,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   FILE_SUBP,              PUT_CACHE_FILE,         PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   FILE_SUBP,              DEL_CACHE_FILE,         PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   FILE_SUBP,              OPEN_LOG,               PAR8,PAR32,PAR32,PAR32,PAR32,PAR32,PAR8,PAR16                         ),
  SC(   FILE_SUBP,              READ_BYTES,             PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  SC(   FILE_SUBP,              WRITE_BYTES,            PAR16,PAR16,PAR8,                               0,0,0,0,0             ),
  SC(   FILE_SUBP,              REMOVE,                 PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   FILE_SUBP,              MOVE,                   PAR8,PAR8,                                      0,0,0,0,0,0           ),

  SC(   ARRAY_SUBP,             CREATE8,                PAR32,PAR16,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             CREATE16,               PAR32,PAR16,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             CREATE32,               PAR32,PAR16,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             CREATEF,                PAR32,PAR16,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             RESIZE,                 PAR16,PAR32,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             DELETE,                 PAR16,                                          0,0,0,0,0,0,0         ),
  SC(   ARRAY_SUBP,             FILL,                   PAR16,PARV,                                     0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             COPY,                   PAR16,PAR16,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             INIT8,                  PAR16,PAR32,PAR32,PARVALUES,PAR8,               0,0,0                 ),
  SC(   ARRAY_SUBP,             INIT16,                 PAR16,PAR32,PAR32,PARVALUES,PAR16,              0,0,0                 ),
  SC(   ARRAY_SUBP,             INIT32,                 PAR16,PAR32,PAR32,PARVALUES,PAR32,              0,0,0                 ),
  SC(   ARRAY_SUBP,             INITF,                  PAR16,PAR32,PAR32,PARVALUES,PARF,               0,0,0                 ),
  SC(   ARRAY_SUBP,             SIZE,                   PAR16,PAR32,                                    0,0,0,0,0,0           ),
  SC(   ARRAY_SUBP,             READ_CONTENT,           PAR16,PAR16,PAR32,PAR32,PAR8,                   0,0,0                 ),
  SC(   ARRAY_SUBP,             WRITE_CONTENT,          PAR16,PAR16,PAR32,PAR32,PAR8,                   0,0,0                 ),
  SC(   ARRAY_SUBP,             READ_SIZE,              PAR16,PAR16,PAR32,                              0,0,0,0,0             ),

  SC(   FILENAME_SUBP,          EXIST,                  PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   FILENAME_SUBP,          TOTALSIZE,              PAR8,PAR32,PAR32,                               0,0,0,0,0             ),
  SC(   FILENAME_SUBP,          SPLIT,                  PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  SC(   FILENAME_SUBP,          MERGE,                  PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  SC(   FILENAME_SUBP,          CHECK,                  PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   FILENAME_SUBP,          PACK,                   PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   FILENAME_SUBP,          UNPACK,                 PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   FILENAME_SUBP,          GET_FOLDERNAME,         PAR8,PAR8,                                      0,0,0,0,0,0           ),

  //    VM
  SC(   VM_SUBP,                SET_ERROR,              PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   VM_SUBP,                GET_ERROR,              PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   VM_SUBP,                ERRORTEXT,              PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),

  SC(   VM_SUBP,                GET_VOLUME,             PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   VM_SUBP,                SET_VOLUME,             PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   VM_SUBP,                GET_MINUTES,            PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   VM_SUBP,                SET_MINUTES,            PAR8,                                           0,0,0,0,0,0,0         ),

  SC(   TST_SUBP,               TST_OPEN,               0,                                              0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_CLOSE,              0,                                              0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_READ_PINS,          PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   TST_SUBP,               TST_WRITE_PINS,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   TST_SUBP,               TST_READ_ADC,           PAR8,PAR16,                                     0,0,0,0,0,0           ),
  SC(   TST_SUBP,               TST_WRITE_UART,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   TST_SUBP,               TST_READ_UART,          PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   TST_SUBP,               TST_ENABLE_UART,        PAR32,                                          0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_DISABLE_UART,       0,                                              0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_ACCU_SWITCH,        PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_BOOT_MODE2,         0,                                              0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_POLL_MODE2,         PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_CLOSE_MODE2,        0,                                              0,0,0,0,0,0,0         ),
  SC(   TST_SUBP,               TST_RAM_CHECK,          PAR8,                                           0,0,0,0,0,0,0         ),

  SC(   STRING_SUBP,            GET_SIZE,               PAR8,PAR16,                                     0,0,0,0,0,0           ),
  SC(   STRING_SUBP,            ADD,                    PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   STRING_SUBP,            COMPARE,                PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   STRING_SUBP,            DUPLICATE,              PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   STRING_SUBP,            VALUE_TO_STRING,        PARF,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   STRING_SUBP,            STRING_TO_VALUE,        PAR8,PARF,                                      0,0,0,0,0,0           ),
  SC(   STRING_SUBP,            STRIP,                  PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   STRING_SUBP,            NUMBER_TO_STRING,       PAR16,PAR8,PAR8,                                0,0,0,0,0             ),
  SC(   STRING_SUBP,            SUB,                    PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   STRING_SUBP,            VALUE_FORMATTED,        PARF,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   STRING_SUBP,            NUMBER_FORMATTED,       PAR32,PAR8,PAR8,PAR8,                           0,0,0,0               ),

  //    UI
  SC(   UI_READ_SUBP,           GET_VBATT,              PARF,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_IBATT,              PARF,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_OS_VERS,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_EVENT,              PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_TBATT,              PARF,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_IINT,               PARF,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_IMOTOR,             PARF,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_STRING,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           KEY,                    PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_SHUTDOWN,           PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_WARNING,            PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_LBATT,              PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_ADDRESS,            PAR32,                                          0,0,0,0,0,0,0         ),
  SC(   UI_READ_SUBP,           GET_CODE,               PAR32,PAR32,PAR32,PAR8,                         0,0,0,0               ),
  SC(   UI_READ_SUBP,           TEXTBOX_READ,           PAR8,PAR32,PAR8,PAR8,PAR16,PAR8,                0,0                   ),
  SC(   UI_READ_SUBP,           GET_HW_VERS,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_FW_VERS,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_FW_BUILD,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_OS_BUILD,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_VERSION,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_IP,                 PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_READ_SUBP,           GET_SDCARD,             PAR8,PAR32,PAR32,                               0,0,0,0,0             ),
  SC(   UI_READ_SUBP,           GET_USBSTICK,           PAR8,PAR32,PAR32,                               0,0,0,0,0             ),

  SC(   UI_WRITE_SUBP,          WRITE_FLUSH,            0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          FLOATVALUE,             PARF,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   UI_WRITE_SUBP,          STAMP,                  PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          PUT_STRING,             PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          CODE,                   PAR8,PAR32,                                     0,0,0,0,0,0           ),
  SC(   UI_WRITE_SUBP,          DOWNLOAD_END,           0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          SCREEN_BLOCK,           PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          TEXTBOX_APPEND,         PAR8,PAR32,PAR8,PAR8,                           0,0,0,0               ),
  SC(   UI_WRITE_SUBP,          SET_BUSY,               PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          VALUE8,                 PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          VALUE16,                PAR16,                                          0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          VALUE32,                PAR32,                                          0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          VALUEF,                 PARF,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          INIT_RUN,               0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          UPDATE_RUN,             0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          LED,                    PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          POWER,                  PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          TERMINAL,               PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          GRAPH_SAMPLE,           0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_WRITE_SUBP,          SET_TESTPIN,            PAR8,                                           0,0,0,0,0,0,0         ),

  SC(   UI_DRAW_SUBP,           UPDATE,                 0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           CLEAN,                  0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           FILLRECT,               PAR8,PAR16,PAR16,PAR16,PAR16,                   0,0,0                 ),
  SC(   UI_DRAW_SUBP,           RECT,                   PAR8,PAR16,PAR16,PAR16,PAR16,                   0,0,0                 ),
  SC(   UI_DRAW_SUBP,           PIXEL,                  PAR8,PAR16,PAR16,                               0,0,0,0,0             ),
  SC(   UI_DRAW_SUBP,           LINE,                   PAR8,PAR16,PAR16,PAR16,PAR16,                   0,0,0                 ),
  SC(   UI_DRAW_SUBP,           CIRCLE,                 PAR8,PAR16,PAR16,PAR16,                         0,0,0,0               ),
  SC(   UI_DRAW_SUBP,           TEXT,                   PAR8,PAR16,PAR16,PAR8,                          0,0,0,0               ),
  SC(   UI_DRAW_SUBP,           ICON,                   PAR8,PAR16,PAR16,PAR8,PAR8,                     0,0,0                 ),
  SC(   UI_DRAW_SUBP,           PICTURE,                PAR8,PAR16,PAR16,PAR32,                         0,0,0,0               ),
  SC(   UI_DRAW_SUBP,           VALUE,                  PAR8,PAR16,PAR16,PARF,PAR8,PAR8,                0,0                   ),
  SC(   UI_DRAW_SUBP,           NOTIFICATION,           PAR8,PAR16,PAR16,PAR8,PAR8,PAR8,PAR8,PAR8                             ),
  SC(   UI_DRAW_SUBP,           QUESTION,               PAR8,PAR16,PAR16,PAR8,PAR8,PAR8,PAR8,PAR8                             ),
  SC(   UI_DRAW_SUBP,           KEYBOARD,               PAR8,PAR16,PAR16,PAR8,PAR8,PAR8,PAR8,PAR8                             ),
  SC(   UI_DRAW_SUBP,           BROWSE,                 PAR8,PAR16,PAR16,PAR16,PAR16,PAR8,PAR8,PAR8                           ),
  SC(   UI_DRAW_SUBP,           VERTBAR,                PAR8,PAR16,PAR16,PAR16,PAR16,PAR16,PAR16,PAR16                        ),
  SC(   UI_DRAW_SUBP,           INVERSERECT,            PAR16,PAR16,PAR16,PAR16,                        0,0,0,0               ),
  SC(   UI_DRAW_SUBP,           SELECT_FONT,            PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           TOPLINE,                PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           FILLWINDOW,             PAR8,PAR16,PAR16,                               0,0,0,0,0             ),
  SC(   UI_DRAW_SUBP,           SCROLL,                 PAR16,                                          0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           DOTLINE,                PAR8,PAR16,PAR16,PAR16,PAR16,PAR16,PAR16,       0                     ),
  SC(   UI_DRAW_SUBP,           VIEW_VALUE,             PAR8,PAR16,PAR16,PARF,PAR8,PAR8,                0,0                   ),
  SC(   UI_DRAW_SUBP,           VIEW_UNIT,              PAR8,PAR16,PAR16,PARF,PAR8,PAR8,PAR8,PAR8                             ),
  SC(   UI_DRAW_SUBP,           FILLCIRCLE,             PAR8,PAR16,PAR16,PAR16,                         0,0,0,0               ),
  SC(   UI_DRAW_SUBP,           STORE,                  PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           RESTORE,                PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           ICON_QUESTION,          PAR8,PAR16,PAR16,PAR8,PAR32,                    0,0,0                 ),
  SC(   UI_DRAW_SUBP,           BMPFILE,                PAR8,PAR16,PAR16,PAR8,                          0,0,0,0               ),
  SC(   UI_DRAW_SUBP,           GRAPH_SETUP,            PAR16,PAR16,PAR16,PAR16,PAR8,PAR16,PAR16,PAR16                        ),
  SC(   UI_DRAW_SUBP,           GRAPH_DRAW,             PAR8,PARF,PARF,PARF,PARF,                       0,0,0                 ),
  SC(   UI_DRAW_SUBP,           POPUP,                  PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_DRAW_SUBP,           TEXTBOX,                PAR16,PAR16,PAR16,PAR16,PAR8,PAR32,PAR8,PAR8                          ),

  SC(   UI_BUTTON_SUBP,         SHORTPRESS,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_BUTTON_SUBP,         LONGPRESS,              PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_BUTTON_SUBP,         FLUSH,                  0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         WAIT_FOR_PRESS,         0,                                              0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         PRESS,                  PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         RELEASE,                PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         GET_HORZ,               PAR16,                                          0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         GET_VERT,               PAR16,                                          0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         PRESSED,                PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_BUTTON_SUBP,         SET_BACK_BLOCK,         PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         GET_BACK_BLOCK,         PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   UI_BUTTON_SUBP,         TESTSHORTPRESS,         PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_BUTTON_SUBP,         TESTLONGPRESS,          PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_BUTTON_SUBP,         GET_BUMBED,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   UI_BUTTON_SUBP,         GET_CLICK,              PAR8,                                           0,0,0,0,0,0,0         ),

  //    Com
  SC(   COM_READ_SUBP,          COMMAND,                PAR32,PAR32,PAR32,PAR8,                         0,0,0,0               ),
  SC(   COM_WRITE_SUBP,         REPLY,                  PAR32,PAR32,PAR8,                               0,0,0,0,0             ),

  //    Sound
  SC(   SOUND_SUBP,             BREAK,                  0,                                              0,0,0,0,0,0,0         ),
  SC(   SOUND_SUBP,             TONE,                   PAR8,PAR16,PAR16,                               0,0,0,0,0             ),
  SC(   SOUND_SUBP,             PLAY,                   PAR8,PARS,                                      0,0,0,0,0,0           ),
  SC(   SOUND_SUBP,             REPEAT,                 PAR8,PARS,                                      0,0,0,0,0,0           ),
  SC(   SOUND_SUBP,             SERVICE,                0,                                              0,0,0,0,0,0,0         ),

  //    Input
  SC(   INPUT_SUBP,             GET_TYPEMODE,           PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   INPUT_SUBP,             GET_CONNECTION,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   INPUT_SUBP,             GET_NAME,               PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   INPUT_SUBP,             GET_SYMBOL,             PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   INPUT_SUBP,             GET_FORMAT,             PAR8,PAR8,PAR8,PAR8,PAR8,PAR8,                  0,0                   ),
  SC(   INPUT_SUBP,             GET_RAW,                PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  SC(   INPUT_SUBP,             GET_MODENAME,           PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  SC(   INPUT_SUBP,             SET_RAW,                PAR8,PAR8,PAR8,PAR32,                           0,0,0,0               ),
  SC(   INPUT_SUBP,             GET_FIGURES,            PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   INPUT_SUBP,             GET_CHANGES,            PAR8,PAR8,PARF,                                 0,0,0,0,0             ),
  SC(   INPUT_SUBP,             CLR_CHANGES,            PAR8,PAR8,0,                                    0,0,0,0,0             ),
  SC(   INPUT_SUBP,             READY_PCT,              PAR8,PAR8,PAR8,PAR8,PARNO,                      0,0,0                 ),
  SC(   INPUT_SUBP,             READY_RAW,              PAR8,PAR8,PAR8,PAR8,PARNO,                      0,0,0                 ),
  SC(   INPUT_SUBP,             READY_SI,               PAR8,PAR8,PAR8,PAR8,PARNO,                      0,0,0                 ),
  SC(   INPUT_SUBP,             GET_MINMAX,             PAR8,PAR8,PARF,PARF,                            0,0,0,0               ),
  SC(   INPUT_SUBP,             CAL_MINMAX,             PAR8,PAR8,PAR32,PAR32,                          0,0,0,0               ),
  SC(   INPUT_SUBP,             CAL_DEFAULT,            PAR8,PAR8,0,                                    0,0,0,0,0             ),
  SC(   INPUT_SUBP,             CAL_MIN,                PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  SC(   INPUT_SUBP,             CAL_MAX,                PAR8,PAR8,PAR32,                                0,0,0,0,0             ),
  SC(   INPUT_SUBP,             GET_BUMPS,              PAR8,PAR8,PARF,                                 0,0,0,0,0             ),
  SC(   INPUT_SUBP,             SETUP,                  PAR8,PAR8,PAR8,PAR16,PAR8,PAR8,PAR8,PAR8                              ),
  SC(   INPUT_SUBP,             CLR_ALL,                PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   INPUT_SUBP,             STOP_ALL,               PAR8,                                           0,0,0,0,0,0,0         ),

  //    Math
  SC(   MATH_SUBP,              EXP,                    PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              MOD,                    PARF,PARF,PARF,                                 0,0,0,0,0             ),
  SC(   MATH_SUBP,              FLOOR,                  PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              CEIL,                   PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              ROUND,                  PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              ABS,                    PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              NEGATE,                 PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              SQRT,                   PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              LOG,                    PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              LN,                     PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              SIN,                    PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              COS,                    PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              TAN,                    PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              ASIN,                   PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              ACOS,                   PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              ATAN,                   PARF,PARF,                                      0,0,0,0,0,0           ),
  SC(   MATH_SUBP,              MOD8,                   PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   MATH_SUBP,              MOD16,                  PAR16,PAR16,PAR16,                              0,0,0,0,0             ),
  SC(   MATH_SUBP,              MOD32,                  PAR32,PAR32,PAR32,                              0,0,0,0,0             ),
  SC(   MATH_SUBP,              POW,                    PARF,PARF,PARF,                                 0,0,0,0,0             ),
  SC(   MATH_SUBP,              TRUNC,                  PARF,PAR8,PARF,                                 0,0,0,0,0             ),

  //    ComGet
  SC(   COM_GET_SUBP,           GET_ON_OFF,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           GET_VISIBLE,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           GET_RESULT,             PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   COM_GET_SUBP,           GET_PIN,                PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   COM_GET_SUBP,           SEARCH_ITEMS,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           SEARCH_ITEM,            PAR8,PAR8,PAR8,PAR8,PAR8,PAR8,PAR8,PAR8                               ),
  SC(   COM_GET_SUBP,           FAVOUR_ITEMS,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           FAVOUR_ITEM,            PAR8,PAR8,PAR8,PAR8,PAR8,PAR8,PAR8,             0                     ),
  SC(   COM_GET_SUBP,           GET_ID,                 PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   COM_GET_SUBP,           GET_BRICKNAME,          PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           GET_NETWORK,            PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  SC(   COM_GET_SUBP,           GET_PRESENT,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           GET_ENCRYPT,            PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   COM_GET_SUBP,           CONNEC_ITEMS,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_GET_SUBP,           CONNEC_ITEM,            PAR8,PAR8,PAR8,PAR8,PAR8,                       0,0,0                 ),
  SC(   COM_GET_SUBP,           GET_INCOMING,           PAR8,PAR8,PAR8,PAR8,                            0,0,0,0               ),
  SC(   COM_GET_SUBP,           GET_MODE2,              PAR8,PAR8,                                      0,0,0,0,0,0           ),

  //    ComSet
  SC(   COM_SET_SUBP,           SET_ON_OFF,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_VISIBLE,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_SEARCH,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_PIN,                PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   COM_SET_SUBP,           SET_PASSKEY,            PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_CONNECTION,         PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   COM_SET_SUBP,           SET_BRICKNAME,          PAR8,                                           0,0,0,0,0,0,0         ),
  SC(   COM_SET_SUBP,           SET_MOVEUP,             PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_MOVEDOWN,           PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_ENCRYPT,            PAR8,PAR8,PAR8,                                 0,0,0,0,0             ),
  SC(   COM_SET_SUBP,           SET_SSID,               PAR8,PAR8,                                      0,0,0,0,0,0           ),
  SC(   COM_SET_SUBP,           SET_MODE2,              PAR8,PAR8,                                      0,0,0,0,0,0           ),

};
