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


#include  <stdio.h>

#include  "lmstypes.h"
#include  "bytecodes.h"
#include  "bytecodes.c"
#include  "validate.h"

//#define   DEBUG

#ifndef LEGO_SIMULATION

  #include  <stdlib.h>
  #include  <unistd.h>
  VALIDATE_GLOBALS ValidateInstance;

#else

  VALIDATE_GLOBALS * gValidateInstance;

  void setValidateInstance(VALIDATE_GLOBALS * _Instance)
  {
    gValidateInstance= _Instance;
  }

  VALIDATE_GLOBALS* getValidateInstance()
  {
    return gValidateInstance;
  }

#endif


void      ShowOpcode(UBYTE OpCode,char *Buf,int Lng)
{
  ULONG   Pars;
  DATA8   Sub;
  UBYTE   Tab;
  UBYTE   ParType;
  UBYTE   Flag = 0;
  char    TmpBuf[255];
  int     Length;
  int     Size;


  Buf[0]  =  0;
  if (OpCodes[OpCode].Name)
  { // Byte code exist

    Size    =  0;
    Length  =  snprintf(&Buf[Size],Lng - Size,"  %s,",OpCodes[OpCode].Name);
    Size   +=  Length;
    Size   +=  snprintf(&Buf[Size],Lng - Size,"%*.*s",3 + OPCODE_NAMESIZE - Length,3 + OPCODE_NAMESIZE - Length,"");

    // Get opcode parameter types
    Pars    =  OpCodes[OpCode].Pars;
    do
    { // check for every parameter

      ParType     =  Pars & 0x0F;
      if (ParType == SUBP)
      { // Check existence of sub command

        Pars  >>= 4;
        Tab     =  Pars & 0x0F;

        for (Sub = 0;Sub < MAX_SUBCODES;Sub++)
        {
          Pars    =  0;
          if (SubCodes[Tab][Sub].Name != NULL)
          {
            if (Flag == 0)
            {
              snprintf(TmpBuf,255,"%s",Buf);
            }
            else
            {
              Size   +=  snprintf(&Buf[Size],Lng - Size,"\r\n");
              Size   +=  snprintf(&Buf[Size],Lng - Size,"%s",TmpBuf);
            }
            Flag  =  1;
            Size +=  snprintf(&Buf[Size],Lng - Size,"%s, ",SubCodes[Tab][Sub].Name);
            Pars    =  SubCodes[Tab][Sub].Pars;
          }
          while ((Pars & 0x0F) >= PAR)
          { // Check parameter

            if (((Pars >> 4) & 0x0F) != SUBP)
            {
              Size +=  snprintf(&Buf[Size],Lng - Size,"%s, ",ParTypeNames[(Pars & 0x0F) - PAR]);
            }

            // Next parameter
            Pars >>= 4;
          }
        }
      }
      if (ParType == PARNO)
      { // Check parameter

        if (((Pars >> 4) & 0x0F) != SUBP)
        {
          Size +=  snprintf(&Buf[Size],Lng - Size,"%s, ",ParTypeNames[DATA_8]);
        }

        // Next parameter
        Pars >>= 4;
      }
      if (ParType == PARLAB)
      { // Check parameter

        if (((Pars >> 4) & 0x0F) != SUBP)
        {
          Size +=  snprintf(&Buf[Size],Lng - Size,"%s, ",ParTypeNames[DATA_8]);
        }

        // Next parameter
        Pars >>= 4;
      }
      if (ParType == PARVALUES)
      { // Check parameter

        // Next parameter
        Pars >>= 4;
      }
      if (ParType >= PAR)
      { // Check parameter

        if (((Pars >> 4) & 0x0F) != SUBP)
        {
          Size +=  snprintf(&Buf[Size],Lng - Size,"%s, ",ParTypeNames[(Pars & 0x0F) - PAR]);
        }

        // Next parameter
        Pars >>= 4;
      }
    }
    while (ParType);
    Size +=  snprintf(&Buf[Size],Lng - Size,"\r\n");
  }
}


RESULT    cValidateInit(void)
{
  RESULT  Result = FAIL;
#ifdef DEBUG
  FILE    *pFile;
  UWORD   OpCode;
  char    Buffer[8000];

  pFile  =  fopen("../../../bytecodeassembler/o.c","w");
  fprintf(pFile,"//******************************************************************************\r\n");
  fprintf(pFile,"//Test Supported Opcodes in V%4.2f\r\n",VERS);
  fprintf(pFile,"//******************************************************************************\r\n\n");
  fprintf(pFile,"#define DATA8          LC0(0)                   \r\n");
  fprintf(pFile,"#define DATA16         GV0(0)                   \r\n");
  fprintf(pFile,"#define DATA32         LV0(0)                   \r\n");
  fprintf(pFile,"#define DATAF          LC0(0)                   \r\n");
  fprintf(pFile,"#define UNKNOWN        LV0(0)                   \r\n");
  fprintf(pFile,"#define STRING         LCS,'T','E','S','T',0    \r\n");
  fprintf(pFile,"\r\n");
  fprintf(pFile,"UBYTE     prg[] =\r\n");
  fprintf(pFile,"{\r\n");
  fprintf(pFile,"  PROGRAMHeader(0,2,4),\r\n");
  fprintf(pFile,"  VMTHREADHeader(0,4),\r\n");
  fprintf(pFile,"  VMTHREADHeader(0,4),\r\n");
  fprintf(pFile,"\r\n");
  for (OpCode = 0;OpCode < 256;OpCode++)
  {
    if ((OpCode != opERROR) && (OpCode != opNOP))
    {
      ShowOpcode((UBYTE)OpCode,Buffer,8000);
      fprintf(pFile,"%s",Buffer);
      if (OpCode == 0x60)
      {
//        fprintf(pFile,"  0x60,\r\n");
      }
    }
  }
  ShowOpcode(opOBJECT_END,Buffer,8000);
  fprintf(pFile,"%s",Buffer);
  fprintf(pFile,"\r\n");
  fprintf(pFile,"};\r\n\n");
  fprintf(pFile,"//******************************************************************************\r\n");
  fclose(pFile);

  if (system("~/projects/lms2012/bytecodeassembler/oasm") >= 0)
  {
    printf("Compiling\r\n");
    sync();
  }


#endif
  Result  =  OK;

  return (Result);
}


RESULT    cValidateExit(void)
{
  RESULT  Result = FAIL;

  Result  =  OK;

  return (Result);
}


void      cValidateSetErrorIndex(IMINDEX Index)
{
  if (Index  ==  0)
  {
    ValidateInstance.ValidateErrorIndex  =  0;
  }
  if (ValidateInstance.ValidateErrorIndex == 0)
  {
    ValidateInstance.ValidateErrorIndex  =  Index;
  }
}


IMINDEX   cValidateGetErrorIndex(void)
{
  return (ValidateInstance.ValidateErrorIndex);
}


RESULT    cValidateDisassemble(IP pI,IMINDEX *pIndex,LABEL *pLabel)
{
  RESULT  Result = FAIL;  // Current status
  IMINDEX OpCode;         // Current opcode
  ULONG   Pars;           // Current parameter types
  UBYTE   ParType;        // Current parameter type
  DATA8   Sub;            // Current sub code if any
  UBYTE   Tab;            // Sub code table index
  ULONG   Value;
  UBYTE   ParCode = 0;
  void    *pParValue;
  DATA8   Parameters;
  DATA32  Bytes;
  int     Indent;
  int     LineLength;

  // Check for validation error
  if ((*pIndex) ==  cValidateGetErrorIndex())
  {
    printf("ERROR ");
  }

  // Get opcode
  OpCode  =  pI[*pIndex] & 0xFF;

  // Check if opcode exists
  if (OpCodes[OpCode].Name)
  { // Byte code exists

    Parameters  =  0;

    // Print opcode
    LineLength  =  printf("  /* %4d */  %s,",*pIndex,OpCodes[OpCode].Name);
    Indent      =  LineLength;

    (*pIndex)++;

    // Check if object ends or error
    if ((OpCode == opOBJECT_END) || (OpCode == opERROR))
    {
      Result  =  STOP;
    }
    else
    {
      Result  =  OK;

      // Get opcode parameter types
      Pars    =  OpCodes[OpCode].Pars;
      do
      { // check for every parameter

        // Isolate current parameter type
        ParType     =  Pars & 0x0F;

        if (ParType == SUBP)
        { // Prior plain parameter was a sub code

          // Get sub code from last plain parameter
          Sub     =  *(DATA8*)pParValue;

          // Isolate next parameter type
          Pars  >>= 4;
          Tab     =  Pars & 0x0F;
          Pars    =  0;

          // Check if sub code in right range
          if (Sub < MAX_SUBCODES)
          { // Sub code range ok

            // Check if sub code exists
            if (SubCodes[Tab][Sub].Name != NULL)
            { // Ok

              if (ParCode & PRIMPAR_LONG)
              { // long format

                if ((ParCode & PRIMPAR_BYTES) == PRIMPAR_1_BYTE)
                {
                  LineLength +=  printf("LC1(%s),",SubCodes[Tab][Sub].Name);
                }
                if ((ParCode & PRIMPAR_BYTES) == PRIMPAR_2_BYTES)
                {
                  LineLength +=  printf("LC2(%s),",SubCodes[Tab][Sub].Name);
                }
                if ((ParCode & PRIMPAR_BYTES) == PRIMPAR_4_BYTES)
                {
                  LineLength +=  printf("LC4(%s),",SubCodes[Tab][Sub].Name);
                }
              }
              else
              {
                LineLength +=  printf("LC0(%s),",SubCodes[Tab][Sub].Name);
              }

              // Get sub code parameter list
              Pars    =  SubCodes[Tab][Sub].Pars;
            }
          }
        }

        if (ParType == PARVALUES)
        {
          Bytes  =  *(DATA32*)pParValue;
          // Next parameter
          Pars >>=  4;
          Pars  &=  0x0F;

          while (Bytes)
          {
//***************************************************************************
            if ((LineLength + 10) >= 80)
            {
              printf("\r\n%*.*s",Indent,Indent,"");
              LineLength  =  Indent;
            }
            Value       =  (ULONG)0;
            pParValue   =  (void*)&Value;
            ParCode     =  (UBYTE)pI[(*pIndex)++] & 0xFF;

            // Calculate parameter value

            if (ParCode & PRIMPAR_LONG)
            { // long format

              if (ParCode & PRIMPAR_HANDLE)
              {
                LineLength +=  printf("HND(");
              }
              else
              {
                if (ParCode & PRIMPAR_ADDR)
                {
                  LineLength +=  printf("ADR(");
                }
            }
              if (ParCode & PRIMPAR_VARIABEL)
              { // variabel

                if (ParCode & PRIMPAR_GLOBAL)
                { // global

                  LineLength +=  printf("GV");
                }
                else
                { // local

                  LineLength +=  printf("LV");
                }
                switch(ParCode & PRIMPAR_BYTES)
                {
                  case PRIMPAR_1_BYTE :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];

                    LineLength +=  printf("1(%d)",(int)Value);
                  }
                  break;

                  case PRIMPAR_2_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);

                    LineLength +=  printf("2(%d)",(int)Value);
                  }
                  break;

                  case PRIMPAR_4_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);

                    LineLength +=  printf("4(%d)",(int)Value);
                  }
                  break;

                }
              }
              else
              { // constant

                if (ParCode & PRIMPAR_HANDLE)
                {
                  LineLength +=  printf("HND(");
                }
                else
                {
                  if (ParCode & PRIMPAR_ADDR)
                  {
                    LineLength +=  printf("ADR(");
                  }
                }
                if (ParCode & PRIMPAR_LABEL)
                { // label

                  Value    =  (ULONG)pI[(*pIndex)++];
                  if (Value & 0x00000080)
                  { // Adjust if negative

                    Value |=  0xFFFFFF00;
                  }

                  LineLength +=  printf("LAB1(%d)",(int)(Value & 0xFF));
                }
                else
                { // value

                  switch(ParCode & PRIMPAR_BYTES)
                  {
                    case PRIMPAR_STRING_OLD :
                    case PRIMPAR_STRING :
                    {

                      LineLength +=  printf("LCS,");

                      while (pI[(*pIndex)])
                      { // Adjust Ip

                        if ((LineLength + 5) >= 80)
                        {
                          printf("\r\n%*.*s",Indent,Indent,"");
                          LineLength  =  Indent;
                        }

                        switch (pI[(*pIndex)])
                        {
                          case '\r' :
                          {
                            LineLength +=  printf("'\\r',");
                          }
                          break;

                          case '\n' :
                          {
                            LineLength +=  printf("'\\n',");
                          }
                          break;

                          default :
                          {
                            LineLength +=  printf("'%c',",pI[(*pIndex)]);
                          }
                          break;

                        }

                        (*pIndex)++;
                      }
                      (*pIndex)++;

                      if ((LineLength + 2) >= 80)
                      {
                        printf("\r\n%*.*s",Indent,Indent,"");
                        LineLength  =  Indent;
                      }
                      LineLength +=  printf("0");
                    }
                    break;

                    case PRIMPAR_1_BYTE :
                    {
                      Value    =  (ULONG)pI[(*pIndex)++];
                      if (Value & 0x00000080)
                      { // Adjust if negative

                        Value |=  0xFFFFFF00;
                      }
                      if ((Pars & 0x0f) != SUBP)
                      {
                        LineLength +=  printf("LC1(%d)",(int)Value);
                      }
                    }
                    break;

                    case PRIMPAR_2_BYTES :
                    {
                      Value    =  (ULONG)pI[(*pIndex)++];
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                      if (Value & 0x00008000)
                      { // Adjust if negative

                        Value |=  0xFFFF0000;
                      }
                      if ((Pars & 0x0f) != SUBP)
                      {
                        LineLength +=  printf("LC2(%d)",(int)Value);
                      }
                    }
                    break;

                    case PRIMPAR_4_BYTES :
                    {
                      Value    =  (ULONG)pI[(*pIndex)++];
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);
                      if ((Pars & 0x0f) != SUBP)
                      {
                        LineLength +=  printf("LC4(%ld)",(long)Value);
                      }
                    }
                    break;

                  }
                }
              }
              if (ParCode & PRIMPAR_HANDLE)
              {
                LineLength +=  printf("),");
              }
              else
              {
                if (ParCode & PRIMPAR_ADDR)
                {
                  LineLength +=  printf("),");
                }
                else
                {
                  LineLength +=  printf(",");
                }
              }
            }
            else
            { // short format

              if (ParCode & PRIMPAR_VARIABEL)
              { // variabel

                if (ParCode & PRIMPAR_GLOBAL)
                { // global

                  LineLength +=  printf("GV0(%u)",(unsigned int)(ParCode & PRIMPAR_INDEX));
                }
                else
                { // local

                  LineLength +=  printf("LV0(%u)",(unsigned int)(ParCode & PRIMPAR_INDEX));
                }
              }
              else
              { // constant

                Value  =  (ULONG)(ParCode & PRIMPAR_VALUE);

                if (ParCode & PRIMPAR_CONST_SIGN)
                { // Adjust if negative

                  Value |= ~(ULONG)(PRIMPAR_VALUE);
                }
                LineLength +=  printf("LC0(%d)",(int)Value);

              }
              LineLength +=  printf(",");
            }
            if (ParType == PARNO)
            {
              if (!(ParCode & PRIMPAR_VARIABEL))
              {
                Parameters  =  (DATA8)(*(DATA32*)pParValue);
              }
            }
            if (ParType == PARLAB)
            {
              if (!(ParCode & PRIMPAR_VARIABEL))
              {
              }
            }
//***************************************************************************
            Bytes--;
          }
          Pars    =  0;
        }

        if ((ParType >= PAR) || (ParType == PARNO) || (ParType == PARLAB))
        { // Plain parameter

          if ((LineLength + 10) >= 80)
          {
            printf("\r\n%*.*s",Indent,Indent,"");
            LineLength  =  Indent;
          }
          Value       =  (ULONG)0;
          pParValue   =  (void*)&Value;
          ParCode     =  (UBYTE)pI[(*pIndex)++] & 0xFF;

          // Next parameter
          Pars >>= 4;

          // Calculate parameter value

          if (ParCode & PRIMPAR_LONG)
          { // long format

            if (ParCode & PRIMPAR_HANDLE)
            {
              LineLength +=  printf("HND(");
            }
            else
            {
              if (ParCode & PRIMPAR_ADDR)
              {
                LineLength +=  printf("ADR(");
              }
            }
            if (ParCode & PRIMPAR_VARIABEL)
            { // variabel

              if (ParCode & PRIMPAR_GLOBAL)
              { // global

                LineLength +=  printf("GV");
              }
              else
              { // local

                LineLength +=  printf("LV");
              }
              switch(ParCode & PRIMPAR_BYTES)
              {
                case PRIMPAR_1_BYTE :
                {
                  Value    =  (ULONG)pI[(*pIndex)++];

                  LineLength +=  printf("1(%d)",(int)Value);
                }
                break;

                case PRIMPAR_2_BYTES :
                {
                  Value    =  (ULONG)pI[(*pIndex)++];
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);

                  LineLength +=  printf("2(%d)",(int)Value);
                }
                break;

                case PRIMPAR_4_BYTES :
                {
                  Value    =  (ULONG)pI[(*pIndex)++];
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);

                  LineLength +=  printf("4(%d)",(int)Value);
                }
                break;

              }
            }
            else
            { // constant

              if (ParCode & PRIMPAR_HANDLE)
              {
                LineLength +=  printf("HND(");
              }
              else
              {
                if (ParCode & PRIMPAR_ADDR)
                {
                  LineLength +=  printf("ADR(");
                }
              }
              if (ParCode & PRIMPAR_LABEL)
              { // label

                Value    =  (ULONG)pI[(*pIndex)++];
                if (Value & 0x00000080)
                { // Adjust if negative

                  Value |=  0xFFFFFF00;
                }

                LineLength +=  printf("LAB1(%d)",(int)(Value & 0xFF));
              }
              else
              { // value

                switch(ParCode & PRIMPAR_BYTES)
                {
                  case PRIMPAR_STRING_OLD :
                  case PRIMPAR_STRING :
                  {

                    LineLength +=  printf("LCS,");

                    while (pI[(*pIndex)])
                    { // Adjust Ip

                      if ((LineLength + 5) >= 80)
                      {
                        printf("\r\n%*.*s",Indent,Indent,"");
                        LineLength  =  Indent;
                      }

                      switch (pI[(*pIndex)])
                      {
                        case '\r' :
                        {
                          LineLength +=  printf("'\\r',");
                        }
                        break;

                        case '\n' :
                        {
                          LineLength +=  printf("'\\n',");
                        }
                        break;

                        default :
                        {
                          LineLength +=  printf("'%c',",pI[(*pIndex)]);
                        }
                        break;

                      }

                      (*pIndex)++;
                    }
                    (*pIndex)++;

                    if ((LineLength + 2) >= 80)
                    {
                      printf("\r\n%*.*s",Indent,Indent,"");
                      LineLength  =  Indent;
                    }
                    LineLength +=  printf("0");
                  }
                  break;

                  case PRIMPAR_1_BYTE :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    if (Value & 0x00000080)
                    { // Adjust if negative

                      Value |=  0xFFFFFF00;
                    }
                    if ((Pars & 0x0f) != SUBP)
                    {
                      LineLength +=  printf("LC1(%d)",(int)Value);
                    }
                  }
                  break;

                  case PRIMPAR_2_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                    if (Value & 0x00008000)
                    { // Adjust if negative

                      Value |=  0xFFFF0000;
                    }
                    if ((Pars & 0x0f) != SUBP)
                    {
                      LineLength +=  printf("LC2(%d)",(int)Value);
                    }
                  }
                  break;

                  case PRIMPAR_4_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);
                    if ((Pars & 0x0f) != SUBP)
                    {
                      LineLength +=  printf("LC4(%ld)",(long)Value);
                    }
                  }
                  break;

                }
              }
            }
            if (ParCode & PRIMPAR_HANDLE)
            {
              LineLength +=  printf("),");
            }
            else
            {
              if (ParCode & PRIMPAR_ADDR)
              {
                LineLength +=  printf("),");
              }
              else
              {
                LineLength +=  printf(",");
              }
            }
          }
          else
          { // short format

            if (ParCode & PRIMPAR_VARIABEL)
            { // variabel

              if (ParCode & PRIMPAR_GLOBAL)
              { // global

                LineLength +=  printf("GV0(%u)",(unsigned int)(ParCode & PRIMPAR_INDEX));
              }
              else
              { // local

                LineLength +=  printf("LV0(%u)",(unsigned int)(ParCode & PRIMPAR_INDEX));
              }
            }
            else
            { // constant

              Value  =  (ULONG)(ParCode & PRIMPAR_VALUE);

              if (ParCode & PRIMPAR_CONST_SIGN)
              { // Adjust if negative

                Value |= ~(ULONG)(PRIMPAR_VALUE);
              }
              if ((Pars & 0x0f) != SUBP)
              {
                LineLength +=  printf("LC0(%d)",(int)Value);
              }

            }
            if ((Pars & 0x0f) != SUBP)
            {
              LineLength +=  printf(",");
            }
          }
          if (ParType == PARNO)
          {
            if (!(ParCode & PRIMPAR_VARIABEL))
            {
              Parameters  =  (DATA8)(*(DATA32*)pParValue);
            }
          }
          if (ParType == PARLAB)
          {
            if (!(ParCode & PRIMPAR_VARIABEL))
            {


            }
          }
          if (Parameters)
          {
            Parameters--;
            Pars  =  PAR32;
          }

        }
      }
      while (ParType || Parameters);
    }
    printf("\r\n");
  }

  return (Result);
}


RESULT    cValidateDisassembleProgram(PRGID PrgId,IP pI,LABEL *pLabel)
{
  RESULT  Result = OK;
  IMINDEX Size;
  OBJID   ObjIndex;
  IMINDEX MinIndex;
  IMINDEX MaxIndex;
  IMINDEX Index;
  IMINDEX Addr;
  ULONG   LastOffset;
  OBJID   LastObject;
  UBYTE   Stop;
  IMINDEX TmpIndex;
  UBYTE   Type;
  DATA32  Lng;

  IMGHEAD *pIH;
  OBJHEAD *pOH;
  OBJID   Objects;

  pIH       =  (IMGHEAD*)pI;
  pOH       =  (OBJHEAD*)&pI[sizeof(IMGHEAD) - sizeof(OBJHEAD)];
  Objects   =  (*(IMGHEAD*)pI).NumberOfObjects;
  Size      =  (*(IMGHEAD*)pI).ImageSize;

  printf("\r\n//****************************************************************************");
  printf("\r\n// Disassembler Listing");
  printf("\r\n//****************************************************************************");
  printf("\r\n\nUBYTE     prg[] =\r\n{\r\n");

  Addr        =  0;
  LastOffset  =  0;
  LastObject  =  0;
  // Check for validation error
  if ((cValidateGetErrorIndex()) && (cValidateGetErrorIndex() < sizeof(IMGHEAD)))
  {
    printf("ERROR ");
  }
  printf("  /* %4u */  PROGRAMHeader(%.2f,%d,%d),\r\n",Addr,(float)(*pIH).VersionInfo / (float)100,(*pIH).NumberOfObjects,(*pIH).GlobalBytes);
  Addr +=  sizeof(IMGHEAD);

  for (ObjIndex = 1;ObjIndex <= Objects;ObjIndex++)
  {
    if ((cValidateGetErrorIndex() >= Addr) && (cValidateGetErrorIndex() < (Addr + sizeof(OBJHEAD))))
    {
      printf("ERROR ");
    }
    if (pOH[ObjIndex].OwnerObjectId)
    { // BLOCK object

      ValidateInstance.Row   =  printf("  /* %4u */  BLOCKHeader(%u,%u,%u),",Addr,(ULONG)pOH[ObjIndex].OffsetToInstructions,pOH[ObjIndex].OwnerObjectId,pOH[ObjIndex].TriggerCount);
    }
    else
    {
      if (pOH[ObjIndex].TriggerCount == 1)
      { // SUBCALL object

        if (LastOffset  ==  (ULONG)pOH[ObjIndex].OffsetToInstructions)
        { // SUBCALL alias object

          ValidateInstance.Row   =    printf("  /* %4u */  SUBCALLHeader(%u,%u),",Addr,(ULONG)pOH[ObjIndex].OffsetToInstructions,pOH[ObjIndex].LocalBytes);
        }
        else
        {
          ValidateInstance.Row   =    printf("  /* %4u */  SUBCALLHeader(%u,%u),",Addr,(ULONG)pOH[ObjIndex].OffsetToInstructions,pOH[ObjIndex].LocalBytes);
        }
      }
      else
      { // VMTHREAD object

        ValidateInstance.Row   =    printf("  /* %4u */  VMTHREADHeader(%u,%u),",Addr,(ULONG)pOH[ObjIndex].OffsetToInstructions,pOH[ObjIndex].LocalBytes);
      }
    }
    ValidateInstance.Row  =  41 - ValidateInstance.Row;
    if (LastOffset  ==  (ULONG)pOH[ObjIndex].OffsetToInstructions)
    {
      printf("%*.*s// Object %-3u (Alias for object %u)\r\n",ValidateInstance.Row,ValidateInstance.Row," ",ObjIndex,LastObject);
    }
    else
    {
      printf("%*.*s// Object %-3u\r\n",ValidateInstance.Row,ValidateInstance.Row," ",ObjIndex);
    }
    ValidateInstance.Row  =  0;
    LastOffset  =  (ULONG)pOH[ObjIndex].OffsetToInstructions;
    LastObject  =  ObjIndex;
    Addr +=  sizeof(OBJHEAD);
  }

  for (ObjIndex = 1;ObjIndex <= Objects;ObjIndex++)
  { // for every object - find minimum and maximum instruction pointer values

    MinIndex      =  (IMINDEX)pOH[ObjIndex].OffsetToInstructions;

    if (ObjIndex == Objects)
    {
      MaxIndex    =  Size;
    }
    else
    {
      MaxIndex    =  (IMINDEX)pOH[ObjIndex + 1].OffsetToInstructions;
    }

    if (pOH[ObjIndex].OwnerObjectId)
    { // BLOCK object

      printf("\r\n  /* Object %2d (BLOCK) [%lu..%lu] */\r\n\n",ObjIndex,(long unsigned int)MinIndex,(long unsigned int)MaxIndex);
    }
    else
    {
      if (pOH[ObjIndex].TriggerCount == 1)
      { // SUBCALL object

        printf("\r\n  /* Object %2d (SUBCALL) [%lu..%lu] */\r\n\n",ObjIndex,(long unsigned int)MinIndex,(long unsigned int)MaxIndex);

        ValidateInstance.Row +=    printf("  /* %4d */  ",MinIndex);
        TmpIndex  =  (IMINDEX)pI[MinIndex++];
        printf("%u,",TmpIndex);
        while (TmpIndex)
        {
          Type  =  pI[MinIndex++];
          if (Type & CALLPAR_IN)
          {
            printf("IN_");
          }
          if (Type & CALLPAR_OUT)
          {
            printf("OUT_");
          }
          switch (Type & CALLPAR_TYPE)
          {
            case CALLPAR_DATA8 :
            {
              printf("8,");
            }
            break;

            case CALLPAR_DATA16 :
            {
              printf("16,");
            }
            break;

            case CALLPAR_DATA32 :
            {
              printf("32,");
            }
            break;

            case CALLPAR_DATAF :
            {
              printf("F,");
            }
            break;

            case CALLPAR_STRING :
            {
              Lng  =  (DATA32)pI[MinIndex++];
              printf("(%d),",Lng);
            }
            break;

          }
          TmpIndex--;
        }
        printf("\r\n\n");

      }
      else
      { // VMTHREAD object

        printf("\r\n  /* Object %2d (VMTHREAD) [%lu..%lu] */\r\n\n",ObjIndex,(long unsigned int)MinIndex,(long unsigned int)MaxIndex);
      }
    }

    Stop          =  OK;
    ValidateInstance.Row           =  0;

    for (Index = MinIndex;((MaxIndex == 0) || (Index < MaxIndex)) && (Stop == OK);)
    { // for every opcode - find number of parameters

      Stop  =  cValidateDisassemble(pI,&Index,pLabel);
    }

    Addr  =  Index;

  }
  printf("};\r\n");
  printf("\r\n//****************************************************************************\r\n\n");


  return (Result);
}


RESULT    cValidateCheckAlignment(ULONG Value,DATA8 Type)
{
  RESULT  Result = OK;

  switch (Type)
  {
    case PAR16 :
    {
      if ((Value & 1))
      {
        Result  =  FAIL;
      }
    }
    break;

    case PAR32 :
    {
      if ((Value & 3))
      {
        Result  =  FAIL;
      }
    }
    break;

    case PARF :
    {
      if ((Value & 3))
      {
        Result  =  FAIL;
      }
    }
    break;

    default :
    {
      Result  =  OK;
    }
    break;

  }

  return (Result);
}


RESULT    cValidateBytecode(IP pI,IMINDEX *pIndex,LABEL *pLabel)
{
  RESULT  Result = FAIL;
  RESULT  Aligned = OK;
  IMINDEX OpCode;
  ULONG   Pars;
  DATA8   Sub;
  IMGDATA Tab;
  ULONG   Value;
  UBYTE   ParType = PAR;
  UBYTE   ParCode;
  void    *pParValue;
  DATA8   Parameters;
  DATA8   ParNo;
  DATA32  Bytes;

  OpCode  =  pI[*pIndex] & 0xFF;

  if (OpCodes[OpCode].Name)
  { // Byte code exist

    Parameters  =  0;
    ParNo       =  0;

    (*pIndex)++;

    if ((OpCode == opERROR) || (OpCode == opOBJECT_END))
    {
      if (OpCode == opOBJECT_END)
      {
        Result  =  STOP;
      }
    }
    else
    {
      Result      =  FAIL;

      if (OpCode == opJR_TRUE)
      {
        Bytes  =  0;
      }
      // Get opcode parameter types
      Pars    =  OpCodes[OpCode].Pars;
      do
      { // check for every parameter

        ParType     =  Pars & 0x0F;
        if (ParType == 0)
        {
          Result  =  OK;
        }

        if (ParType == SUBP)
        { // Check existence of sub command

          Sub     =  *(DATA8*)pParValue;
          Pars  >>= 4;
          Tab     =  Pars & 0x0F;
          Pars    =  0;

          if (Sub < MAX_SUBCODES)
          {
            if (SubCodes[Tab][Sub].Name != NULL)
            {
              Pars    =  SubCodes[Tab][Sub].Pars;
              Result  =  OK;
            }
          }
        }

        if (ParType == PARVALUES)
        { // Last parameter tells number of bytes to follow

          Bytes  =  *(DATA32*)pParValue;

          Pars >>=  4;
          Pars  &=  0x0F;
//***************************************************************************

          while (Bytes)
          {
            Value       =  (ULONG)0;
            pParValue   =  (void*)&Value;
            ParCode     =  (UBYTE)pI[(*pIndex)++] & 0xFF;
            Aligned     =  OK;

            // Calculate parameter value
            if (ParCode & PRIMPAR_LONG)
            { // long format

              if (ParCode & PRIMPAR_VARIABEL)
              { // variabel

                switch(ParCode & PRIMPAR_BYTES)
                {
                  case PRIMPAR_1_BYTE :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                  }
                  break;

                  case PRIMPAR_2_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                  }
                  break;

                  case PRIMPAR_4_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);
                  }
                  break;

                }
              }
              else
              { // constant

                if (ParCode & PRIMPAR_LABEL)
                { // label

                  Value    =  (ULONG)pI[(*pIndex)++];
                  if (Value & 0x00000080)
                  { // Adjust if negative

                    Value |=  0xFFFFFF00;
                  }
                }
                else
                { // value

                  switch(ParCode & PRIMPAR_BYTES)
                  {
                    case PRIMPAR_STRING_OLD :
                    case PRIMPAR_STRING :
                    {
                      while (pI[(*pIndex)])
                      { // Adjust Ip
                        (*pIndex)++;
                      }
                      (*pIndex)++;
                    }
                    break;

                    case PRIMPAR_1_BYTE :
                    {
                      Value    =  (ULONG)pI[(*pIndex)++];
                      if (Value & 0x00000080)
                      { // Adjust if negative

                        Value |=  0xFFFFFF00;
                      }
                    }
                    break;

                    case PRIMPAR_2_BYTES :
                    {
                      Value    =  (ULONG)pI[(*pIndex)++];
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                      if (Value & 0x00008000)
                      { // Adjust if negative

                        Value |=  0xFFFF0000;
                      }
                    }
                    break;

                    case PRIMPAR_4_BYTES :
                    {
                      Value    =  (ULONG)pI[(*pIndex)++];
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                      Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);
                    }
                    break;

                  }
                }
              }
            }
            else
            { // short format

              if (ParCode & PRIMPAR_VARIABEL)
              { // variabel

                Value  =  (ULONG)(ParCode & PRIMPAR_VALUE);
              }
              else
              { // constant

                Value  =  (ULONG)(ParCode & PRIMPAR_VALUE);

                if (ParCode & PRIMPAR_CONST_SIGN)
                { // Adjust if negative

                  Value |= ~(ULONG)(PRIMPAR_VALUE);
                }
              }
            }

            // Check parameter value
            if ((Pars >= PAR8) && (Pars <= PAR32))
            {
              if (((*(DATA32*)pParValue) >= ParMin[ParType - PAR]) && ((*(DATA32*)pParValue) <= ParMax[ParType - PAR]))
              {
                Result  =  OK;
              }
            }
            if ((Pars == PARF))
            {
              if ((*(DATAF*)pParValue >= DATAF_MIN) && (*(DATAF*)pParValue <= DATAF_MAX))
              {
                Result  =  OK;
              }
            }
            if (Pars == PARV)
            {
              Result  =  OK;
            }
            Bytes--;
          }
//***************************************************************************
          Pars    =  0;
        }

        if ((ParType >= PAR) || (ParType == PARNO) || (ParType == PARLAB))
        { // Check parameter

          Value       =  (ULONG)0;
          pParValue   =  (void*)&Value;
          ParCode     =  (UBYTE)pI[(*pIndex)++] & 0xFF;
          Aligned     =  OK;

          // Calculate parameter value



          if (ParCode & PRIMPAR_LONG)
          { // long format

            if (ParCode & PRIMPAR_VARIABEL)
            { // variabel

              switch(ParCode & PRIMPAR_BYTES)
              {
                case PRIMPAR_1_BYTE :
                {
                  Value    =  (ULONG)pI[(*pIndex)++];
                }
                break;

                case PRIMPAR_2_BYTES :
                {
                  Value    =  (ULONG)pI[(*pIndex)++];
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                }
                break;

                case PRIMPAR_4_BYTES :
                {
                  Value    =  (ULONG)pI[(*pIndex)++];
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                  Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);
                }
                break;

              }
            }
            else
            { // constant

              if (ParCode & PRIMPAR_LABEL)
              { // label

                Value    =  (ULONG)pI[(*pIndex)++];
                if (Value & 0x00000080)
                { // Adjust if negative

                  Value |=  0xFFFFFF00;
                }
              }
              else
              { // value

                switch(ParCode & PRIMPAR_BYTES)
                {
                  case PRIMPAR_STRING_OLD :
                  case PRIMPAR_STRING :
                  {
                    while (pI[(*pIndex)])
                    { // Adjust Ip
                      (*pIndex)++;
                    }
                    (*pIndex)++;
                  }
                  break;

                  case PRIMPAR_1_BYTE :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    if (Value & 0x00000080)
                    { // Adjust if negative

                      Value |=  0xFFFFFF00;
                    }
                  }
                  break;

                  case PRIMPAR_2_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                    if (Value & 0x00008000)
                    { // Adjust if negative

                      Value |=  0xFFFF0000;
                    }
                  }
                  break;

                  case PRIMPAR_4_BYTES :
                  {
                    Value    =  (ULONG)pI[(*pIndex)++];
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 8);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 16);
                    Value   |=  ((ULONG)pI[(*pIndex)++]  << 24);
                  }
                  break;

                }
              }
            }
          }
          else
          { // short format

            if (ParCode & PRIMPAR_VARIABEL)
            { // variabel

              Value  =  (ULONG)(ParCode & PRIMPAR_VALUE);
            }
            else
            { // constant

              Value  =  (ULONG)(ParCode & PRIMPAR_VALUE);

              if (ParCode & PRIMPAR_CONST_SIGN)
              { // Adjust if negative

                Value |= ~(ULONG)(PRIMPAR_VALUE);
              }
            }
          }

          if (ParCode & PRIMPAR_VARIABEL)
          {
            Result  =  OK;
          }
          else
          { // Check constant parameter value

            if ((ParType >= PAR8) && (ParType <= PAR32))
            {
              if (((*(DATA32*)pParValue) >= ParMin[ParType - PAR]) && ((*(DATA32*)pParValue) <= ParMax[ParType - PAR]))
              {
                Result  =  OK;
              }
            }
            if ((ParType == PARF))
            {
              Result  =  OK;
            }
          }
          if (ParType == PARV)
          {
            Result  =  OK;
          }
          if (ParType == PARNO)
          { // Check number of parameters

            ParNo   =  1;
            if (!(ParCode & PRIMPAR_VARIABEL))
            { // Must be constant

              if (((*(DATA32*)pParValue) >= 0) && ((*(DATA32*)pParValue) <= DATA8_MAX))
              { // Must be positive and than less than or equal to 127

                Parameters  =  (DATA8)(*(DATA32*)pParValue);
                Result  =  OK;
              }
            }
          }
          if (ParType == PARLAB)
          { // Check number of parameters

            if (!(ParCode & PRIMPAR_VARIABEL))
            { // Must be constant

              if (((*(DATA32*)pParValue) >= 0) && ((*(DATA32*)pParValue) < MAX_LABELS))
              {
                if (pLabel != NULL)
                {
                  pLabel[Value].Addr  =  *pIndex;
                }

                Result  =  OK;
              }
            }
          }

          // Check allocation
          if (ParNo == 0)
          {
            if (ParCode & PRIMPAR_LONG)
            { // long format

              if (ParCode & PRIMPAR_VARIABEL)
              { // variabel

                if (ParCode & PRIMPAR_HANDLE)
                { // handle

                  Aligned  =  cValidateCheckAlignment(Value,PAR16);
                }
                else
                {

                  if (ParCode & PRIMPAR_GLOBAL)
                  { // global
                  }
                  else
                  { // local
                  }
                  Aligned  =  cValidateCheckAlignment(Value,ParType);
                }
              }
              else
              { // constant

                if (ParCode & PRIMPAR_LABEL)
                { // label

                }
                else
                { // value

                }
              }
            }
            else
            { // short format

              if (ParCode & PRIMPAR_VARIABEL)
              { // variabel

                if (ParCode & PRIMPAR_GLOBAL)
                { // global
                }
                else
                { // local
                }
                Aligned  =  cValidateCheckAlignment(Value,ParType);
              }
            }
          }

          // Next parameter
          Pars >>= 4;
          if (Parameters)
          {
            Parameters--;
            Pars  =  PAR32;
          }

        }
        if (Aligned != OK)
        {
          Result  =  FAIL;
        }
      }
      while ((ParType) && (Result == OK));
    }
  }

  return (Result);
}


RESULT    cValidateProgram(PRGID PrgId,IP pI,LABEL *pLabel,DATA8 Disassemble)
{
  RESULT  Result;
  IMGHEAD *pIH;             // Pointer to image header
  IMINDEX TotalSize;        // Total image size
  OBJID   Objects;          // Total number of objects
  IMINDEX TmpSize;
  OBJHEAD *pOH;
  OBJID   ObjIndex;
  IMINDEX ImageIndex;       // Index into total image
  IMINDEX TmpIndex = 0;     // Lached "ImageIndex"
  UBYTE   ParIndex;
  UBYTE   Type;


#ifdef    DEBUG
  printf("Validation started\r\n");
#endif
  cValidateSetErrorIndex(0);
  pIH         =  (IMGHEAD*)pI;

  TotalSize   =  (*(IMGHEAD*)pI).ImageSize;
  Objects     =  (*(IMGHEAD*)pI).NumberOfObjects;

  // Check size
  ImageIndex  =  sizeof(IMGHEAD) + Objects * sizeof(OBJHEAD);

  if ((TotalSize < ImageIndex) || (Objects == 0))
  { // Size too small

    cValidateSetErrorIndex(4);
  }
  else
  {
    pOH       =  (OBJHEAD*)&pI[sizeof(IMGHEAD) - sizeof(OBJHEAD)];

    // Scan headers for size of instructions
    TmpSize     =  0;
    for (ObjIndex = 1;ObjIndex <= Objects;ObjIndex++)
    {
      TmpSize  +=  (IMINDEX)pOH[ObjIndex].OffsetToInstructions;
    }
    Result  =  OK;

    // Scan every object
    for (ObjIndex = 1;(ObjIndex <= Objects) && (Result == OK);ObjIndex++)
    {
      if (pOH[ObjIndex].OffsetToInstructions == 0)
      {
        pOH[ObjIndex].OffsetToInstructions  =  (IP)ImageIndex;
      }

      // Check for SUBCALL parameter description
      if ((pOH[ObjIndex].OwnerObjectId == 0) && (pOH[ObjIndex].TriggerCount == 1))
      { // SUBCALL object

        if (pOH[ObjIndex].OffsetToInstructions >= (IP)ImageIndex)
        { // Only if not alias

          ParIndex  =  (IMINDEX)pI[ImageIndex++];
          while (ParIndex)
          {
            Type  =  pI[ImageIndex++];
            if ((Type & CALLPAR_TYPE) == CALLPAR_STRING)
            {
              ImageIndex++;
            }
            ParIndex--;
          }
        }
        else
        {
          Result  =  STOP;
        }
      }

      // Scan all byte codes in object
      while ((Result == OK) && (ImageIndex < TotalSize))
      {
        TmpIndex  =  ImageIndex;
        Result    =  cValidateBytecode(pI,&ImageIndex,pLabel);
      }
      if (Result == FAIL)
      {
        cValidateSetErrorIndex(TmpIndex);
      }
      else
      {
        Result  =  OK;
      }
    }
    if (ImageIndex != TotalSize)
    {
#ifdef    DEBUG
      printf("%d %d\r\n",ImageIndex,TotalSize);
#endif
      cValidateSetErrorIndex(TmpIndex);
    }
  }

  // Check version
  if (((*pIH).VersionInfo < ((UWORD)(BYTECODE_VERSION * 100.0))) && ((*pIH).VersionInfo != 0))
  {
    cValidateSetErrorIndex(8);
  }

#ifdef    DEBUG
  printf("Validation ended\r\n");
#endif
  if (Disassemble)
  {
    cValidateDisassembleProgram(PrgId,pI,pLabel);
  }
  // Result of validation
  if (cValidateGetErrorIndex())
  {
    if (cValidateGetErrorIndex() == 8)
    {
      Result  =  OK;
    }
    else
    {
      Result  =  FAIL;
    }
  }
  else
  {
    Result  =  OK;
  }

  return (Result);
}

