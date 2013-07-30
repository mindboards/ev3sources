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


/*! \page MemoryLibrary Memory Library
 *
 *- \subpage  MemoryLibraryDescription
 *
 *
 *
 */


/*! \page MemoryLibraryDescription Description
 *
 *  When "create array" is called - memory is allocated using "malloc" for the descriptor and the array itself. The memory pointer is stored in a
 *  list specific to every program. The returned handle is simply the index in that tabel.\n
 *  When a program is terminated the memory is automatically freed.
 *
\verbatim
                Slot 1        Array 1
                ---------     ----------

    HANDLE  ->  pARRAY1   ->  DESCRIPTOR
                              Element 0
                              Element 1
                              - - - - -
                              Element n


    If an array needs to be resized it just uses "realloc".

\endverbatim
 *
 */


#include  "lms2012.h"
#include  "c_memory.h"

#if (HARDWARE != SIMULATION)
	#include  <stdlib.h>
	#include  <string.h>
	#include  <stdio.h>
	#include  <unistd.h>
	#include  <dirent.h>
	#include  <sys/stat.h>
  #include  <sys/statvfs.h>
  #include  <dirent.h>
  #include  <sys/types.h>
  #include  <fcntl.h>
  #include  <sys/sysinfo.h>
  #include  <mntent.h>

MEMORY_GLOBALS MemoryInstance;

#else
  #define snprintf _snprintf
  #include  <stdio.h>
  #include  <stdlib.h>

  MEMORY_GLOBALS * gMemoryInstance;

  void setMemoryInstance(MEMORY_GLOBALS * _Instance)
  {
    gMemoryInstance= _Instance;
  }

  MEMORY_GLOBALS* getMemoryInstance()
  {
    return gMemoryInstance;
  }

#endif

#ifdef    DEBUG_C_MEMORY
#define   DEBUG
#define   DEBUG_C_MEMORY_LOG
#define   DEBUG_C_MEMORY_FILE
#endif

#ifdef    DEBUG
#define   DUBUG_TRACE_FILENAME
#endif


void      cMemoryGetUsage(DATA32 *pTotal,DATA32 *pFree,DATA8 Force)
{
  ULONG   Time;
#ifndef Linux_X86
  DATA32  Used;
  struct  statvfs Status;
#endif

  Time      =  VMInstance.NewTime - VMInstance.MemoryTimer;

  if ((Time >= UPDATE_MEMORY) || (VMInstance.MemoryTimer == 0) || (Force))
  { // Update values

    VMInstance.MemoryTimer +=  Time;

#ifndef Linux_X86
    if (statvfs(MEMORY_FOLDER,&Status) == 0)
    {
      VMInstance.MemorySize  =  INSTALLED_MEMORY;
      Used  =  (DATA32)((((Status.f_blocks - Status.f_bavail) * Status.f_bsize) + (KB - 1)) / KB);
      VMInstance.MemoryFree  =  INSTALLED_MEMORY - Used;
    }
#else
    VMInstance.MemorySize  =  INSTALLED_MEMORY;
    VMInstance.MemoryFree  =  INSTALLED_MEMORY;
#endif
#ifdef DEBUG_C_MEMORY_LOW
    VMInstance.MemoryFree -=  4400;
#endif
  }

  *pTotal   =  VMInstance.MemorySize;
  *pFree    =  VMInstance.MemoryFree;
}


RESULT    cMemoryMalloc(void **ppMemory,DATA32 Size)
{
  RESULT  Result = FAIL;
  DATA32  Total;
  DATA32  Free;

  cMemoryGetUsage(&Total,&Free,0);
  if (((Size + (KB - 1)) / KB) < (Free - RESERVED_MEMORY))
  {
    *ppMemory  =  malloc((size_t)Size);
    if (*ppMemory != NULL)
    {
      Result  =  OK;
    }
  }

  return (Result);
}


RESULT    cMemoryRealloc(void *pOldMemory,void **ppMemory,DATA32 Size)
{
  RESULT  Result = FAIL;

  *ppMemory  =  realloc(pOldMemory,(size_t)Size);
  if (*ppMemory != NULL)
  {
    Result  =  OK;
  }

  return (Result);
}


RESULT    cMemoryAlloc(PRGID PrgId,DATA8 Type,GBINDEX Size,void **pMemory,HANDLER *pHandle)
{
  RESULT  Result = FAIL;
  HANDLER TmpHandle;

  *pHandle    =  -1;

  if ((PrgId < MAX_PROGRAMS) && (Size > 0) && (Size <= MAX_ARRAY_SIZE))
  {
    TmpHandle   =  0;

    while ((TmpHandle < MAX_HANDLES) && (MemoryInstance.pPoolList[PrgId][TmpHandle].pPool != NULL))
    {
      TmpHandle++;
    }
    if (TmpHandle < MAX_HANDLES)
    {

      if (cMemoryMalloc(&MemoryInstance.pPoolList[PrgId][TmpHandle].pPool,(DATA32)Size) == OK)
      {
        *pMemory  =  MemoryInstance.pPoolList[PrgId][TmpHandle].pPool;
        MemoryInstance.pPoolList[PrgId][TmpHandle].Type   =  Type;
        MemoryInstance.pPoolList[PrgId][TmpHandle].Size   =  Size;
        *pHandle  =  TmpHandle;
        Result    =  OK;
#ifdef DEBUG
        printf("  Malloc P=%1u H=%1u T=%1u S=%8lu A=%8p\r\n",(unsigned int)PrgId,(unsigned int)TmpHandle,(unsigned int)Type,(unsigned long)Size,MemoryInstance.pPoolList[PrgId][TmpHandle].pPool);
#endif
      }
    }
  }
#ifdef DEBUG
  if (Result != OK)
  {
    printf("  Malloc error P=%1u S=%8lu\r\n",(unsigned int)PrgId,(unsigned long)Size);
  }
#endif

  return (Result);
}


void*     cMemoryReallocate(PRGID PrgId,HANDLER Handle,GBINDEX Size)
{
  void    *pTmp;

  pTmp  =  NULL;
  if ((PrgId < MAX_PROGRAMS) && (Handle >= 0) && (Handle < MAX_HANDLES))
  {
    if ((Size > 0) && (Size <= MAX_ARRAY_SIZE))
    {
      if (cMemoryRealloc(MemoryInstance.pPoolList[PrgId][Handle].pPool,&pTmp,(DATA32)Size) == OK)
      {
        MemoryInstance.pPoolList[PrgId][Handle].pPool  =  pTmp;
        MemoryInstance.pPoolList[PrgId][Handle].Size   =  Size;
      }
      else
      {
        pTmp  =  NULL;
        printf("cMemoryReallocate out of memory\r\n");
      }
    }
  }
#ifdef DEBUG
  if (pTmp != NULL)
  {
    printf("  Reallocate  P=%1u H=%1u     S=%8lu A=%8p\r\n",(unsigned int)PrgId,(unsigned int)Handle,(unsigned long)Size,MemoryInstance.pPoolList[PrgId][Handle].pPool);
  }
  else
  {
    printf("  Reallocate error P=%1u H=%1u S=%8lu\r\n",(unsigned int)PrgId,(unsigned int)Handle,(unsigned long)Size);
  }
#endif

  return (pTmp);
}


RESULT    cMemoryGetPointer(PRGID PrgId,HANDLER Handle,void **pMemory)
{
  RESULT  Result = FAIL;

  *pMemory  =  NULL;

  if ((PrgId < MAX_PROGRAMS) && (Handle >= 0) && (Handle < MAX_HANDLES))
  {
    if (MemoryInstance.pPoolList[PrgId][Handle].pPool != NULL)
    {
      *pMemory  =  MemoryInstance.pPoolList[PrgId][Handle].pPool;
      Result    =  OK;
    }
  }
#ifdef DEBUG
  if (Result != OK)
  {
    printf("  Get pointer error P=%1u H=%1u\r\n",(unsigned int)PrgId,(unsigned int)Handle);
  }
#endif

  return (Result);
}


RESULT    cMemoryArraryPointer(PRGID PrgId,HANDLER Handle,void **pMemory)
{
  RESULT  Result = FAIL;
  void    *pTmp;

  if (cMemoryGetPointer(PrgId,Handle,&pTmp) == OK)
  {
    *pMemory  =  (*(DESCR*)pTmp).pArray;
    Result    =  OK;
  }

  return (Result);
}


DSPSTAT   cMemoryFreeHandle(PRGID PrgId,HANDLER Handle)
{
  DSPSTAT Result = FAILBREAK;
  FDESCR  *pFDescr;

  if ((PrgId < MAX_PROGRAMS) && (Handle >= 0) && (Handle < MAX_HANDLES))
  {
    if (MemoryInstance.pPoolList[PrgId][Handle].pPool != NULL)
    {
      if (MemoryInstance.pPoolList[PrgId][Handle].Type == POOL_TYPE_FILE)
      {
        pFDescr  =  (FDESCR*)MemoryInstance.pPoolList[PrgId][Handle].pPool;
        if (((*pFDescr).Access))
        {
          (*pFDescr).Access  =  0;
          close((*pFDescr).hFile);
          sync();
          Result  =  NOBREAK;
        }
#ifdef DEBUG
        printf("  Close file %d\r\n",(*pFDescr).hFile);
#endif
      }
      else
      {
        Result  =  NOBREAK;
      }

#ifdef DEBUG
      printf("  Free   P=%1u H=%1u T=%1u S=%8lu A=%8p\r\n",(unsigned int)PrgId,(unsigned int)Handle,(unsigned int)MemoryInstance.pPoolList[PrgId][Handle].Type,(unsigned long)MemoryInstance.pPoolList[PrgId][Handle].Size,MemoryInstance.pPoolList[PrgId][Handle].pPool);
#endif
      free(MemoryInstance.pPoolList[PrgId][Handle].pPool);
      MemoryInstance.pPoolList[PrgId][Handle].pPool  =  NULL;
      MemoryInstance.pPoolList[PrgId][Handle].Size   =  0;

    }
  }

  return (Result);
}


void      cMemoryFreePool(PRGID PrgId,void *pMemory)
{
  HANDLER TmpHandle;

  TmpHandle  =  0;
  while ((TmpHandle < MAX_HANDLES) && (MemoryInstance.pPoolList[PrgId][TmpHandle].pPool != pMemory))
  {
    TmpHandle++;
  }
  if (TmpHandle < MAX_HANDLES)
  {
    cMemoryFreeHandle(PrgId,TmpHandle);
  }
}


void      cMemoryFreeProgram(PRGID PrgId)
{
  HANDLER TmpHandle;

  for (TmpHandle = 0;TmpHandle < MAX_HANDLES;TmpHandle++)
  {
    cMemoryFreeHandle(PrgId,TmpHandle);
  }

  // Ensure that path is emptied
  MemoryInstance.PathList[PrgId][0]  =  0;
}


void      cMemoryFreeAll(void)
{
  PRGID   TmpPrgId;

  for (TmpPrgId = 0;TmpPrgId < MAX_PROGRAMS;TmpPrgId++)
  {
    cMemoryFreeProgram(TmpPrgId);
  }
}


RESULT    cMemoryInit(void)
{
  RESULT  Result = FAIL;
  DATA8   Tmp;
  PRGID   TmpPrgId;
  HANDLER TmpHandle;
  int     File;
  char    PrgNameBuf[vmFILENAMESIZE];

  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmLASTRUN_FILE_NAME,vmEXT_CONFIG);
  File  =  open(PrgNameBuf,O_RDONLY);
  if (File >= MIN_HANDLE)
  {
    if (read(File,MemoryInstance.Cache,sizeof(MemoryInstance.Cache)) != sizeof(MemoryInstance.Cache))
    {
      close (File);
      File  =  -1;
    }
    else
    {
      close (File);
    }
  }
  if (File < 0)
  {
    for (Tmp = 0;Tmp < CACHE_DEEPT;Tmp++)
    {
      MemoryInstance.Cache[Tmp][0]  =  0;
    }
  }

  for (TmpPrgId = 0;TmpPrgId < MAX_PROGRAMS;TmpPrgId++)
  {
    for (TmpHandle = 0;TmpHandle < MAX_HANDLES;TmpHandle++)
    {
      MemoryInstance.pPoolList[TmpPrgId][TmpHandle].pPool  =  NULL;
    }
  }

  VMInstance.MemorySize     =  INSTALLED_MEMORY;
  VMInstance.MemoryFree     =  INSTALLED_MEMORY;

  MemoryInstance.SyncTime   =  (DATA32)0;
  MemoryInstance.SyncTick   =  (DATA32)0;

  Result  =  OK;

  return (Result);
}


RESULT    cMemoryOpen(PRGID PrgId,GBINDEX Size,void **pMemory)
{
  RESULT  Result = FAIL;
  HANDLER TmpHandle;

  Result  =  cMemoryAlloc(PrgId,POOL_TYPE_MEMORY,Size,pMemory,&TmpHandle);

  return (Result);
}


RESULT    cMemoryClose(PRGID PrgId)
{
  RESULT  Result = FAIL;

  cMemoryFreeProgram(PrgId);
  Result  =  OK;

  return (Result);
}


RESULT    cMemoryExit(void)
{
  RESULT  Result = FAIL;
  int     File;
  char    PrgNameBuf[vmFILENAMESIZE];

  snprintf(PrgNameBuf,vmFILENAMESIZE,"%s/%s%s",vmSETTINGS_DIR,vmLASTRUN_FILE_NAME,vmEXT_CONFIG);
  File  =  open(PrgNameBuf,O_CREAT | O_WRONLY | O_TRUNC,FILEPERMISSIONS);
  if (File >= MIN_HANDLE)
  {
    write(File,MemoryInstance.Cache,sizeof(MemoryInstance.Cache));
    close (File);
  }

  Result  =  OK;

  return (Result);
}


void*     cMemoryResize(PRGID PrgId,HANDLER TmpHandle,DATA32 Elements)
{
  DATA32  Size;
  void    *pTmp = NULL;


  if (cMemoryGetPointer(PrgId,TmpHandle,&pTmp) == OK)
  {
    Size    =  Elements * (*(DESCR*)pTmp).ElementSize + sizeof(DESCR);
    pTmp    =  cMemoryReallocate(PrgId,TmpHandle,(GBINDEX)Size);
    if (pTmp != NULL)
    {
      (*(DESCR*)pTmp).Elements  =  Elements;
    }

#ifdef DEBUG
    printf("  Resize P=%1u H=%1u T=%1u S=%8lu A=%8p\r\n",(unsigned int)PrgId,(unsigned int)TmpHandle,(unsigned int)MemoryInstance.pPoolList[PrgId][TmpHandle].Type,(unsigned long)MemoryInstance.pPoolList[PrgId][TmpHandle].Size,MemoryInstance.pPoolList[PrgId][TmpHandle].pPool);
#endif
  }
  if (pTmp != NULL)
  {
    pTmp  =  (*(DESCR*)pTmp).pArray;
  }

  return (pTmp);
}


void      FindName(char *pSource,char *pPath,char *pName,char *pExt)
{
  int     Source      = 0;
  int     Destination = 0;

  while (pSource[Source])
  {
    Source++;
  }
  while ((Source > 0) && (pSource[Source] != '/'))
  {
    Source--;
  }
  if (pSource[Source] == '/')
  {
    if (pPath != NULL)
    {
      for (Destination = 0;Destination <= Source;Destination++)
      {
        pPath[Destination]  =  pSource[Destination];
      }
    }
    Source++;
  }
  if (pPath != NULL)
  {
    pPath[Destination]  =  0;
  }
  Destination  =  0;
  while ((pSource[Source]) && (pSource[Source] != '.'))
  {
    if (pName != NULL)
    {
      pName[Destination]  =  pSource[Source];
      Destination++;
    }
    Source++;
  }
  if (pName != NULL)
  {
    pName[Destination]  =  0;
  }
  if (pExt != NULL)
  {
    Destination         =  0;
    while (pSource[Source])
    {
      pExt[Destination]  =  pSource[Source];
      Source++;
      Destination++;
    }
    pExt[Destination]  =  0;
  }
}


RESULT    cMemoryCheckFilename(char *pFilename,char *pPath,char *pName,char *pExt)
{
  RESULT  Result = FAIL;
  char    Path[vmFILENAMESIZE];
  char    Name[vmFILENAMESIZE];
  char    Ext[vmFILENAMESIZE];

#ifndef DISABLE_FILENAME_CHECK

  if (strlen(pFilename) < vmFILENAMESIZE)
  {
    if (ValidateString((DATA8*)pFilename,vmCHARSET_FILENAME) == OK)
    {
      FindName(pFilename,Path,Name,Ext);
      if (strlen(Path) < vmPATHSIZE)
      {
        if (pPath != NULL)
        {
          strcpy(pPath,Path);
        }
        if (strlen(Name) < vmNAMESIZE)
        {
          if (pName != NULL)
          {
            strcpy(pName,Name);
          }
          if (strlen(Ext) < vmEXTSIZE)
          {
            if (pExt != NULL)
            {
              strcpy(pExt,Ext);
            }
            Result  =  OK;
          }
        }
      }
    }
  }
#else
  if (strlen(pFilename) < vmFILENAMESIZE)
  {
    FindName(pFilename,Path,Name,Ext);
    if (strlen(Path) < vmPATHSIZE)
    {
      if (pPath != NULL)
      {
        strcpy(pPath,Path);
      }
      if (strlen(Name) < vmNAMESIZE)
      {
        if (pName != NULL)
        {
          strcpy(pName,Name);
        }
        if (strlen(Ext) < vmEXTSIZE)
        {
          if (pExt != NULL)
          {
            strcpy(pExt,Ext);
          }
          Result  =  OK;
        }
      }
    }
  }
#endif
  if (Result != OK)
  {
#ifdef DEBUG_TRACE_FILENAME
    printf("Filename error in [%s]\r\n",pFilename);
#endif
    if (!LogErrorNumberExists(FILE_NAME_ERROR))
    {
      LogErrorNumber(FILE_NAME_ERROR);
    }
  }

  return (Result);
}


RESULT    ConstructFilename(PRGID PrgId,char *pFilename,char *pName,char *pDefaultExt)
{
  RESULT  Result = FAIL;
  char    Path[vmPATHSIZE];
  char    Name[vmNAMESIZE];
  char    Ext[vmEXTSIZE];

  Result  =  cMemoryCheckFilename(pFilename,Path,Name,Ext);

  if (Result == OK)
  { // Filename OK

    if (Path[0] == 0)
    { // Default path

      snprintf(Path,vmPATHSIZE,"%s",(char*)MemoryInstance.PathList[PrgId]);
    }

    if (Ext[0] == 0)
    { // Default extension

      snprintf(Ext,vmEXTSIZE,"%s",pDefaultExt);
    }

    // Construct filename for open
    snprintf(pName,vmFILENAMESIZE,"%s%s%s",Path,Name,Ext);

#ifdef DEBUG_TRACE_FILENAME
    printf("c_memory  ConstructFilename:       [%s]\r\n",pName);
#endif

  }

  return (Result);
}


int       FindDot(char *pString)
{
  int     Result = -1;
  int     Pointer = 0;

  while (pString[Pointer])
  {
    if (pString[Pointer] == '.')
    {
      Result  =  Pointer;
    }
    Pointer++;
  }

  return (Result);
}

void      cMemoryDeleteCacheFile(char *pFileName)
{
  DATA8   Item;
  DATA8   Tmp;

#ifdef DEBUG
  printf("DEL_CACHE_FILE %s\r\n",(char*)pFileName);
#endif

  Item  =  0;
  Tmp   =  0;

  while ((Item < CACHE_DEEPT) && (Tmp == 0))
  {
    if (strcmp((char*)MemoryInstance.Cache[Item],(char*)pFileName) == 0)
    {
      Tmp  =  1;
    }
    else
    {
      Item++;
    }
  }
  while (Item < (CACHE_DEEPT - 1))
  {
    strcpy((char*)MemoryInstance.Cache[Item],(char*)MemoryInstance.Cache[Item + 1]);
    Item++;
  }
  MemoryInstance.Cache[Item][0]   =  0;
}


int       cMemorySort(void *ppFirst,void *ppSecond)
{
  int     Result;
  int     First,Second;
  char    *pFirst;
  char    *pSecond;

  pFirst    =  (char*)(*(const struct dirent **)ppFirst)->d_name;
  pSecond   =  (char*)(*(const struct dirent **)ppSecond)->d_name;

  First     =  FindDot(pFirst);
  Second    =  FindDot(pSecond);

  if ((First >= 0) && (Second >= 0))
  {
    Result  =  strcmp(&pFirst[First],&pSecond[Second]);
    if (Result == 0)
    {
      Result  =  strcmp(pFirst,pSecond);
    }
  }
  else
  {
    if ((First < 0) && (Second < 0))
    {
      Result  =  strcmp(pFirst,pSecond);
    }
    else
    {
      if (First < 0)
      {
        Result  =  1;
      }
      else
      {
        Result  = -1;
      }
    }
  }

  return (Result);
}


DATA8     cMemoryFindSubFolders(char *pFolderName)
{
  struct  dirent **NameList;
  int     Items;
  DATA8   Folders = 0;

#ifdef Linux_X86
  Items     =  scandir(pFolderName,&NameList,0,(int (*)(const struct dirent **,const struct dirent **))cMemorySort);
#else
  Items     =  scandir(pFolderName,&NameList,0,(int (*)(const void *,const void *))cMemorySort);
#endif
  if (Items >= 0)
  {
    while (Items--)
    {
      if ((*NameList[Items]).d_name[0] != '.')
      {
        if (((*NameList[Items]).d_name[0] != 'C') || ((*NameList[Items]).d_name[1] != 'V') || ((*NameList[Items]).d_name[2] != 'S'))
        {
          Folders++;
        }
      }
      free(NameList[Items]);
    }
    free(NameList);
  }

  return (Folders);
}


DATA8     cMemoryFindType(char *pExt)
{
  DATA8   Result = 0;

  if (pExt[0])
  {
    if (strcmp(pExt,EXT_SOUND) == 0)
    {
      Result  =  TYPE_SOUND;
    }
    else
    {
      if (strcmp(pExt,EXT_GRAPHICS) == 0)
      {
        Result  =  TYPE_GRAPHICS;
      }
      else
      {
        if (strcmp(pExt,EXT_BYTECODE) == 0)
        {
          Result  =  TYPE_BYTECODE;
        }
        else
        {
          if (strcmp(pExt,EXT_DATALOG) == 0)
          {
            Result  =  TYPE_DATALOG;
          }
          else
          {
            if (strcmp(pExt,EXT_PROGRAM) == 0)
            {
              Result  =  TYPE_PROGRAM;
            }
            else
            {
              if (strcmp(pExt,EXT_TEXT) == 0)
              {
                Result  =  TYPE_TEXT;
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
  else
  {
    Result  =  TYPE_FOLDER;
  }

  return (Result);
}


DATA8     cMemoryGetSubFolderName(DATA8 Item,DATA8 MaxLength,char *pFolderName,char *pSubFolderName)
{
  DATA8   Filetype = 0;
  struct  dirent **NameList;
  int     Items;
  int     Tmp;
  DATA8   Char;
  DATA8   Folders = 0;

  pSubFolderName[0]  =  0;
#ifdef Linux_X86
  Items     =  scandir(pFolderName,&NameList,0,(int (*)(const struct dirent **,const struct dirent **))cMemorySort);
#else
  Items     =  scandir(pFolderName,&NameList,0,(int (*)(const void *,const void *))cMemorySort);
#endif
  Tmp       =  0;

  if (Items >= 0)
  {
    while (Tmp < Items)
    {
      if ((*NameList[Tmp]).d_name[0] != '.')
      {
        if (((*NameList[Tmp]).d_name[0] != 'C') || ((*NameList[Tmp]).d_name[1] != 'V') || ((*NameList[Tmp]).d_name[2] != 'S'))
        {
          Folders++;
          if (Item == Folders)
          {
            Char  =  0;
            while (((*NameList[Tmp]).d_name[Char]) && ((*NameList[Tmp]).d_name[Char] != '.'))
            {
              Char++;
            }
            if ((*NameList[Tmp]).d_name[Char] == '.')
            {
              Filetype  =  cMemoryFindType(&(*NameList[Tmp]).d_name[Char]);

              // delete extension
              (*NameList[Tmp]).d_name[Char]  =  0;
              snprintf((char*)pSubFolderName,(int)MaxLength,"%s",(*NameList[Tmp]).d_name);
            }
            else
            { // must be a folder or file without extension
              snprintf((char*)pSubFolderName,(int)MaxLength,"%s",(*NameList[Tmp]).d_name);
              Filetype  =  TYPE_FOLDER;
            }
          }
        }
      }
      free(NameList[Tmp]);
      Tmp++;
    }
    free(NameList);
  }

  return (Filetype);
}


void      cMemoryDeleteSubFolders(char *pFolderName)
{
  struct  dirent *d;
  DIR     *dir;
  char    buf[256];
  DATA8   DeleteOk = 1;

  if (strcmp(pFolderName,DEMO_FILE_NAME) == 0)
  {
    DeleteOk  =  0;
  }

  if (DeleteOk)
  {
    dir = opendir(pFolderName);

    if (dir != NULL)
    {
      while((d = readdir(dir)))
      {
#ifdef DEBUG
        printf("%s\r\n",d->d_name);
#endif

        sprintf(buf, "%s/%s", pFolderName, d->d_name);
        remove(buf);
        cMemoryDeleteCacheFile(buf);
      }
      closedir(dir);
      remove(pFolderName);
    }
    else
    {
      cMemoryDeleteCacheFile(pFolderName);
      remove(pFolderName);
    }
  }
}


DATA32    cMemoryFindSize(char *pFolderName,DATA32 *pFiles)
{
  struct  dirent **NameList;
  struct  stat Status;
  int     Items;
  DATA32  Size = 0;

  *pFiles  =  0;
  if (stat(pFolderName,&Status) == 0)
  {
    Size +=  (DATA32)Status.st_size;
#ifdef Linux_X86
    Items     =  scandir(pFolderName,&NameList,0,(int (*)(const struct dirent **,const struct dirent **))cMemorySort);
#else
    Items     =  scandir(pFolderName,&NameList,0,(int (*)(const void *,const void *))cMemorySort);
#endif
    if (Items >= 0)
    {
      *pFiles  =  (DATA32)Items;

      while (Items--)
      {
        stat((*NameList[Items]).d_name,&Status);
        Size +=  (DATA32)Status.st_size;
        free(NameList[Items]);
      }
      free(NameList);
    }
  }
  Size  =  (Size + (KB - 1)) / KB;

  return (Size);
}


DATA8     cMemoryGetCacheName(DATA8 Item,DATA8 MaxLength,char *pFileName,char *pName)
{
  DATA8   Result = 0;
  char    Path[vmPATHSIZE];
  char    Name[vmNAMESIZE];
  char    Ext[vmEXTSIZE];
  char    Filename[vmFILENAMESIZE];

  if ((Item > 0) && (Item <= CACHE_DEEPT))
  {
    if (MemoryInstance.Cache[Item - 1])
    {
      snprintf(Filename,vmFILENAMESIZE,"%s",MemoryInstance.Cache[Item - 1]);

      if (cMemoryCheckFilename(Filename,Path,Name,Ext) == OK)
      {
        snprintf((char*)pFileName,MaxLength,"%s",Filename);
        if (MaxLength >= 2)
        {
          if (strlen(Name) >= MaxLength)
          {
            Name[MaxLength - 1]  =  0;
            Name[MaxLength - 2]  =  0x7F;
          }
          snprintf((char*)pName,MaxLength,"%s",Name);
          Result  =  cMemoryFindType(Ext);
        }
      }
    }
  }

  return(Result);
}


DATA8     cMemoryGetCacheFiles(void)
{
  DATA8   Result = 0;
  DATA8   Tmp;

  for (Tmp = 0;Tmp < CACHE_DEEPT;Tmp++)
  {
    if (MemoryInstance.Cache[Tmp][0])
    {
      Result++;
    }
  }

  return (Result);
}


DATA8     cMemoryFindFiles(char *pFolderName)
{
  struct  dirent **NameList;
  int     Items;
  DATA8   Files = 0;

  Items     =  scandir(pFolderName,&NameList,0,alphasort);
  if (Items >= 0)
  {
    while (Items--)
    {
      if ((*NameList[Items]).d_name[0] != '.')
      {
        if (((*NameList[Items]).d_name[0] != 'C') || ((*NameList[Items]).d_name[1] != 'V') || ((*NameList[Items]).d_name[2] != 'S'))
        {
          Files++;
        }
      }
      free(NameList[Items]);
    }
    free(NameList);
  }

  return (Files);
}


void      cMemoryGetResourcePath(PRGID PrgId,char *pString,DATA8 MaxLength)
{
  snprintf(pString,MaxLength,"%s",MemoryInstance.PathList[PrgId]);
}


RESULT    cMemoryGetIcon(DATA8 *pFolderName,DATA8 Item,DATA32 *pImagePointer)
{
  RESULT  Result = FAIL;

  DATA32  ISize;
  IP      pImage;
  HANDLER TmpHandle;
  PRGID   TmpPrgId;
  char    PrgNamePath[SUBFOLDERNAME_SIZE];
  char    PrgNameBuf[MAX_FILENAME_SIZE];
  LFILE    *pFile;

  TmpPrgId      =  CurrentProgramId();

  cMemoryGetSubFolderName(Item,SUBFOLDERNAME_SIZE,(char*)pFolderName,PrgNamePath);

  if (PrgNamePath[0])
  {
    snprintf(PrgNameBuf,MAX_FILENAME_SIZE,"%s%s/icon%s",(char*)pFolderName,PrgNamePath,EXT_GRAPHICS);

    pFile = fopen (PrgNameBuf, "rb");
    if (NULL != pFile)
    {
      fseek (pFile , 0 , SEEK_END);
      ISize = ftell (pFile);
      rewind (pFile);

      // allocate memory to contain the whole file:
      if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pImage,&TmpHandle) == OK)
      {
        if (ISize == fread(pImage,1,ISize,pFile))
        {
          *pImagePointer  =  (DATA32)pImage;
          Result  =  OK;
        }
      }
      fclose (pFile);
    }
  }

  return (Result);
}


void      cMemoryFilename(PRGID PrgId,char *pName,char *pExt,DATA8 Length,char *pResult)
{
  if ((pName[0] == '.') || (pName[0] == '/') || (pName[0] == '~') || (pName[0] == '\"'))
  { // Path to file

    snprintf(pResult,Length,"%s%s",pName,pExt);
  }
  else
  { // Local file

    snprintf(pResult,Length,"%s%s%s",(char*)MemoryInstance.PathList[PrgId],pName,pExt);
  }
#ifdef DEBUG
  printf("Filename = [%s]\r\n",pResult);
#endif
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


enum
{
  OPEN_FOR_WRITE    = 1,
  OPEN_FOR_APPEND   = 2,
  OPEN_FOR_READ     = 3,
  OPEN_FOR_LOG      = 4
};


DSPSTAT   cMemoryGetFileHandle(PRGID PrgId,char *pFileName,HANDLER *pHandle,DATA8 *pOpenForWrite)
{
  DSPSTAT Result = FAILBREAK;
  FDESCR  *pFDescr;
  HANDLER TmpHandle;

  *pHandle        = -1;
  *pOpenForWrite  =  0;

  TmpHandle  =  0;
  while ((TmpHandle < MAX_HANDLES) && (*pHandle == -1))
  {
    if (MemoryInstance.pPoolList[PrgId][TmpHandle].Type == POOL_TYPE_FILE)
    {
      if (cMemoryGetPointer(PrgId,TmpHandle,(void**)&pFDescr) == OK)
      {
        if ((*pFDescr).Access)
        {
          if (strcmp(pFileName,(*pFDescr).Filename) == 0)
          {
            *pHandle  =  TmpHandle;
            if (((*pFDescr).Access == OPEN_FOR_WRITE) || ((*pFDescr).Access == OPEN_FOR_APPEND) || ((*pFDescr).Access == OPEN_FOR_LOG))
            {
              *pOpenForWrite  =  1;
            }

            Result    =  NOBREAK;

          }
        }
      }
    }
    TmpHandle++;
  }

#ifdef DEBUG_C_MEMORY_FILE
  printf("Handle for file %5d %s\r\n",*pHandle,pFileName);
#endif
  Result  =  NOBREAK;

  return (Result);
}


RESULT    cMemoryCheckOpenWrite(char *pFileName)
{
  RESULT  Result = FAIL;
  HANDLER Handle;
  DATA8   OpenForWrite;
  char    FilenameBuf[vmFILENAMESIZE];

  if (ConstructFilename(USER_SLOT,(char*)pFileName,FilenameBuf,"") == OK)
  {
    cMemoryGetFileHandle(USER_SLOT,FilenameBuf,&Handle,&OpenForWrite);
    if (OpenForWrite)
    {
      Result  =  OK;
    }
  }

  return(Result);
}


DSPSTAT   cMemoryOpenFile(PRGID PrgId,DATA8 Access,char *pFileName,HANDLER *pHandle,DATA32 *pSize)
{
  DSPSTAT Result = FAILBREAK;
  FDESCR  *pFDescr;
  struct  stat FileStatus;
  int     hFile  = -1;

  *pHandle  =  0;
  *pSize    =  0;

  switch (Access)
  {
    case OPEN_FOR_WRITE :
    {
      hFile  =  open(pFileName,O_CREAT | O_WRONLY | O_TRUNC,FILEPERMISSIONS);
      chmod(pFileName,FILEPERMISSIONS);
#ifdef DEBUG_C_MEMORY_FILE
      printf("Open for write  %5d %s\r\n",hFile,pFileName);
#endif
    }
    break;

    case OPEN_FOR_APPEND :
    {
      hFile  =  open(pFileName,O_CREAT | O_WRONLY | O_APPEND,FILEPERMISSIONS);
      chmod(pFileName,FILEPERMISSIONS);
#ifdef DEBUG_C_MEMORY_FILE
      printf("Open for append %5d %s\r\n",hFile,pFileName);
#endif
    }
    break;

    case OPEN_FOR_READ :
    {
      hFile  =  open(pFileName,O_RDONLY);
      Result  =  NOBREAK;
#ifdef DEBUG_C_MEMORY_FILE
      printf("Open for read   %5d %s\r\n",hFile,pFileName);
#endif
    }
    break;

    case OPEN_FOR_LOG :
    {
      hFile  =  open(pFileName,O_CREAT | O_WRONLY | O_APPEND,FILEPERMISSIONS);
      chmod(pFileName,FILEPERMISSIONS);
#ifdef DEBUG_C_MEMORY_FILE
      printf("Open for append %5d %s\r\n",hFile,pFileName);
#endif
    }
    break;

  }

  if (hFile >= MIN_HANDLE)
  {
    if (cMemoryAlloc(PrgId,POOL_TYPE_FILE,(GBINDEX)sizeof(FDESCR),(void**)&pFDescr,pHandle) == OK)
    {
      (*pFDescr).hFile    =  hFile;
      (*pFDescr).Access   =  Access;
      snprintf((*pFDescr).Filename,MAX_FILENAME_SIZE,"%s",pFileName);

      stat(pFileName,&FileStatus);
      *pSize  =  FileStatus.st_size;

      Result  =  NOBREAK;
    }
    else
    {
      close(hFile);
    }
  }

  if (Result == FAILBREAK)
  {
    LogErrorNumber(FILE_OPEN_ERROR);
  }

  return (Result);
}


DSPSTAT   cMemoryWriteFile(PRGID PrgId,HANDLER Handle,DATA32 Size,DATA8 Del,DATA8 *pSource)
{
  DSPSTAT Result = FAILBREAK;
  FDESCR  *pFDescr;

  if (cMemoryGetPointer(PrgId,Handle,(void**)&pFDescr) == OK)
  {
    if (((*pFDescr).Access == OPEN_FOR_WRITE) || ((*pFDescr).Access == OPEN_FOR_APPEND) || ((*pFDescr).Access == OPEN_FOR_LOG))
    {
      if (write((*pFDescr).hFile,pSource,Size) == Size)
      {
#ifdef DEBUG_C_MEMORY_FILE
        printf("Write to  %-2d    %5d %s [%d]\r\n",Handle,(*pFDescr).hFile,(*pFDescr).Filename,Size);
#endif
        if (Del < DELS)
        {
          if (Del != DEL_NONE)
          {
            Size  =  strlen(Delimiter[Del]);
            if (write((*pFDescr).hFile,Delimiter[Del],Size) == Size)
            {
              Result  =  NOBREAK;
            }
          }
          else
          {
            Result  =  NOBREAK;
          }
        }
      }
    }
  }

  if (Result == FAILBREAK)
  {
    LogErrorNumber(FILE_WRITE_ERROR);
  }

  return (Result);
}


DSPSTAT   cMemoryReadFile(PRGID PrgId,HANDLER Handle,DATA32 Size,DATA8 Del,DATA8 *pDestination)
{
  DSPSTAT Result = FAILBREAK;
  FDESCR  *pFDescr;
  DATA8   No;
  DATA8   Tmp;
  DATA8   Last;

  if (cMemoryGetPointer(PrgId,Handle,(void**)&pFDescr) == OK)
  {
    if ((*pFDescr).hFile > MIN_HANDLE)
    {
      if (((*pFDescr).Access == OPEN_FOR_READ))
      {
  #ifdef DEBUG_C_MEMORY_FILE
        printf("Read from %-2d    %5d %s [%d]\r\n",Handle,(*pFDescr).hFile,(*pFDescr).Filename,Size);
  #endif
        if (VMInstance.Handle >= 0)
        {
          if (Size > MIN_ARRAY_ELEMENTS)
          {
            pDestination  =  (DATA8*)VmMemoryResize(VMInstance.Handle,Size);
          }
        }
        No    =  1;
        Last  =  0;
        while ((No == 1) && (Size > 0))
        {
          No  =  (DATA8)read((*pFDescr).hFile,&Tmp,1);

          if (Del < DELS)
          {
            if (Del != DEL_NONE)
            {
              if (Del != DEL_CRLF)
              {
                if (Tmp == Delimiter[Del][0])
                {
                  No  =  0;
                }
              }
              else
              {
                if ((Tmp == Delimiter[Del][1]) && (Last == Delimiter[Del][0]))
                {
                  No  =  0;
                }
                Last  =  Tmp;
              }
            }
          }

          if (No)
          {
            *pDestination  =  Tmp;
            pDestination++;
            Size--;
          }
        }
        if (Size)
        {
          *pDestination  =  0;
        }

        Result  =  NOBREAK;
      }
    }
  }

  if (Result == FAILBREAK)
  {
    LogErrorNumber(FILE_READ_ERROR);
  }

  return (Result);
}


DSPSTAT   cMemoryCloseFile(PRGID PrgId,HANDLER Handle)
{
  DSPSTAT Result = FAILBREAK;

#ifdef DEBUG_C_MEMORY_FILE
  FDESCR  *pFDescr;

  if (MemoryInstance.pPoolList[PrgId][Handle].Type == POOL_TYPE_FILE)
  {
    if (cMemoryGetPointer(PrgId,Handle,(void**)&pFDescr) == OK)
    {
      printf("Close file %-2d    %5d %s\r\n",Handle,(*pFDescr).hFile,(*pFDescr).Filename);
    }
  }
  else
  {
    printf("Close pool %-2d\r\n",Handle);
  }
#endif

  Result  =  cMemoryFreeHandle(PrgId,Handle);

  if (Result == FAILBREAK)
  {
    LogErrorNumber(FILE_CLOSE_ERROR);
  }

  return (Result);
}


void      cMemoryFindLogName(PRGID PrgId,char* pName)
{
  HANDLER TmpHandle;
  FDESCR  *pFDescr;

  pName[0]  =  0;

  if ((PrgId >= 0) && (PrgId < MAX_PROGRAMS))
  {
    for (TmpHandle = 0;TmpHandle < MAX_HANDLES;TmpHandle++)
    {
      if (MemoryInstance.pPoolList[PrgId][TmpHandle].pPool != NULL)
      {
        if (MemoryInstance.pPoolList[PrgId][TmpHandle].Type == POOL_TYPE_FILE)
        {
          pFDescr  =  (FDESCR*)MemoryInstance.pPoolList[PrgId][TmpHandle].pPool;
          if ((*pFDescr).Access == OPEN_FOR_LOG)
          {
            snprintf(pName,MAX_FILENAME_SIZE,"%s",(*pFDescr).Filename);
            TmpHandle  =  MAX_HANDLES;
          }
        }
      }
    }
  }
}


RESULT    cMemoryGetImage(DATA8 *pFileName,DATA16 Size,UBYTE *pBmp)
{
  RESULT  Result = FAIL;
  PRGID   TmpPrgId;
  int     File;
  char    FilenameBuf[MAX_FILENAME_SIZE];

  TmpPrgId  =  CurrentProgramId();

  if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,EXT_GRAPHICS) == OK)
  {
    File      =  open(FilenameBuf,O_RDONLY);
    if (File >= MIN_HANDLE)
    {
      read(File,pBmp,(size_t)Size);
      close(File);
      Result  =  OK;
    }
  }

  return (Result);
}


RESULT    cMemoryGetMediaName(char *pChar,char *pName)
{
  RESULT  Result = FAIL;
  FILE    *pFile;
  struct mntent *mountEntry;

  pFile = setmntent("/proc/mounts","r");

  while ((mountEntry = getmntent(pFile)) != NULL)
  {
   if(pChar[0] == 'm') // MMCCard detection
   {

     if(!strcmp(mountEntry->mnt_dir, "/media/card"))
     {
       pName = "card";
       Result = OK;
     }

   }
   if(pChar[0] == 's') // MassStorage detection
   {
     if(!strcmp(mountEntry->mnt_dir, "/media/usb"))
     {
       pName = "usb";
       Result = OK;
     }
   }
  }

  endmntent(pFile);
  return (Result);
}


typedef   struct
{
  DIR     *pDir;
  DATA16  Entries;
  DATA8   Type;
  DATA8   Sort;
  DATA8   Folder[MAX_FILENAME_SIZE];
  DATA8   Entry[DIR_DEEPT][FILENAME_SIZE];
  DATA8   Priority[DIR_DEEPT];
}
FOLDER;


enum
{
  SORT_NONE,
  SORT_PRJS,
  SORT_APPS,
  SORT_TOOLS,

  SORT_TYPES
};

DATA8     NoOfFavourites[SORT_TYPES] =
{
  0,  // -
  6,  // Prjs
  5,  // Apps
  8   // Tools
};

DATA8     FavouriteExts[FILETYPES] =
{
  [TYPE_BYTECODE] = 1,
  [TYPE_SOUND]    = 2,
  [TYPE_GRAPHICS] = 3,
  [TYPE_TEXT]     = 4,
};

char      *pFavourites[SORT_TYPES][8] =
{ // Priority
  //  0             1                 2                 3                 4
  { },
  { "",           "BrkProg_SAVE",   "BrkDL_SAVE",     "SD_Card",        "USB_Stick",          "TEST"   },
  { "Port View",  "Motor Control",  "IR Control",     "Brick Program",  "Brick Datalog" },
  { "Volume",     "Sleep",          "Bluetooth",      "WiFi",           "Brick Info",         "Test",       "Performance",        "Debug" }
};


void      cMemorySortEntry(FOLDER *pMemory,UBYTE Type,char *pName)
{
  DATA8   Sort;
  DATA8   Sort1;
  DATA8   Sort2;
  DATA8   Pointer;
  DATA8   Priority;
  DATA8   TmpPriority;
  DATA8   Favourites;
  char    TmpEntry[FILENAME_SIZE];

  Sort        =  (*pMemory).Sort;
  Favourites  =  NoOfFavourites[Sort];
  Priority    =  Favourites;

  if ((Type != DT_DIR) && (Type != DT_LNK))
  { // Files

    // Get extension
    Pointer  =  0;
    while ((pName[Pointer]) && (pName[Pointer] != '.'))
    {
      Pointer++;
    }

    // Priorities
    Priority    =  FavouriteExts[cMemoryFindType(&pName[Pointer])];
    if (Priority == 0)
    {
      Priority  =  FILETYPES;
    }
    Favourites  =  FILETYPES;
  }
  else
  { // Folders

    if (Sort == SORT_PRJS)
    {
      Priority  =  0;

      for (Pointer = 1;Pointer < NoOfFavourites[Sort];Pointer++)
      {
        if (strcmp(pName,pFavourites[Sort][Pointer]) == 0)
        {
          Priority  =  Pointer;
        }
      }
    }
    else
    {
      for (Pointer = 0;Pointer < NoOfFavourites[Sort];Pointer++)
      {
        if (strcmp(pName,pFavourites[Sort][Pointer]) == 0)
        {
          Priority  =  Pointer;
        }
      }
    }
  }
  snprintf((char*)(*pMemory).Entry[(*pMemory).Entries],FILENAME_SIZE,"%s",pName);
  (*pMemory).Priority[(*pMemory).Entries]  =  Priority;
  ((*pMemory).Entries)++;
  if (Priority < Favourites)
  {
    for (Sort1 = 0;Sort1 < ((*pMemory).Entries - 1);Sort1++)
    {
      for (Sort2 = 0;Sort2 < ((*pMemory).Entries - 1);Sort2++)
      {
        if ((*pMemory).Priority[Sort2 + 1] < (*pMemory).Priority[Sort2])
        {
          TmpPriority  =  (*pMemory).Priority[Sort2];
          strcpy(TmpEntry,(char*)(*pMemory).Entry[Sort2]);
          (*pMemory).Priority[Sort2]  =  (*pMemory).Priority[Sort2 + 1];
          strcpy((char*)(*pMemory).Entry[Sort2],(char*)(*pMemory).Entry[Sort2 + 1]);
          (*pMemory).Priority[Sort2 + 1]  =  TmpPriority;
          strcpy((char*)(*pMemory).Entry[Sort2 + 1],(char*)TmpEntry);
        }
      }
    }
  }
}


void      cMemorySortList(FOLDER *pMemory)
{
  DATA16  Pointer;

  for (Pointer = 0;Pointer < (*pMemory).Entries;Pointer++)
  {
#ifdef DEBUG
    printf("[%s](%d)(%d) %s\r\n",(char*)(*pMemory).Folder,(*pMemory).Sort,(*pMemory).Priority[Pointer],(char*)(*pMemory).Entry[Pointer]);
#endif
  }
}


/*
 *  Opens directory stream and allocate space for that and its items
 *  Gives back the memory handle
 *
 */

RESULT    cMemoryOpenFolder(PRGID PrgId,DATA8 Type,DATA8 *pFolderName,HANDLER *pHandle)
{
  RESULT  Result;
  FOLDER  *pMemory;

  Result  =  cMemoryAlloc(PrgId,POOL_TYPE_MEMORY,(GBINDEX)sizeof(FOLDER),((void**)&pMemory),pHandle);
  if (Result == OK)
  {
    (*pMemory).pDir     =  NULL;
    (*pMemory).Entries  =  0;
    (*pMemory).Type     = Type;
    snprintf((char*)(*pMemory).Folder,MAX_FILENAME_SIZE,"%s",(char*)pFolderName);
    (*pMemory).pDir  =  opendir((char*)(*pMemory).Folder);
    if ((*pMemory).pDir == NULL)
    {
      Result  =  FAIL;
    }
    else
    {
      if (strcmp((char*)pFolderName,vmPRJS_DIR) == 0)
      {
        (*pMemory).Sort  =  SORT_PRJS;
      }
      else
      {
        if (strcmp((char*)pFolderName,vmAPPS_DIR) == 0)
        {
          (*pMemory).Sort  =  SORT_APPS;
        }
        else
        {
          if (strcmp((char*)pFolderName,vmTOOLS_DIR) == 0)
          {
            (*pMemory).Sort  =  SORT_TOOLS;
          }
          else
          {
            (*pMemory).Sort  =  SORT_NONE;
          }
        }
      }
    }
  }

  return (Result);
}


/*
 *  Count and sort items - one for each call
 *  Return total count
 */
RESULT    cMemoryGetFolderItems(PRGID PrgId,HANDLER Handle,DATA16 *pItems)
{
  RESULT  Result;
  FOLDER  *pMemory;
  char    Ext[vmEXTSIZE];
  struct  dirent *pEntry;

  Result  =  cMemoryGetPointer(PrgId,Handle,((void**)&pMemory));

  if (Result == OK)
  { // Handle ok

    if ((*pMemory).pDir != NULL)
    {
      pEntry  =  readdir((*pMemory).pDir);
      if (pEntry != NULL)
      { // More entries

        if ((*pMemory).Entries < DIR_DEEPT)
        {
          if ((*pEntry).d_name[0] != '.')
          {
            if (strcmp((*pEntry).d_name,"CVS") != 0)
            {
              if ((*pMemory).Type == TYPE_FOLDER)
              {
                if (((*pEntry).d_type == DT_DIR) || ((*pEntry).d_type == DT_LNK))
                { // Folders

                  cMemorySortEntry(pMemory,(*pEntry).d_type,(*pEntry).d_name);
#ifdef DEBUG
                  printf("[%s](%d) %s\r\n",(char*)(*pMemory).Folder,(*pMemory).Sort,(*pEntry).d_name);
#endif
                }
              }
              else
              {
                if ((*pEntry).d_type == DT_REG)
                { // Files

                  FindName((*pEntry).d_name,NULL,NULL,Ext);
                  if (cMemoryFindType(Ext))
                  {
                    cMemorySortEntry(pMemory,(*pEntry).d_type,(*pEntry).d_name);
#ifdef DEBUG
                    printf("[%s](%d) %s\r\n",(char*)(*pMemory).Folder,(*pMemory).Sort,(*pEntry).d_name);
#endif
                  }
                }
              }
            }
          }
        }
        Result  =  BUSY;
      }
      else
      { // No more entries

        cMemorySortList(pMemory);
        closedir((*pMemory).pDir);
        (*pMemory).pDir  =  NULL;
      }
    }
  }
  *pItems  =  ((*pMemory).Entries);

  return (Result);
}


/*
 *  Get display-able name - only name not path and extension
 */
RESULT    cMemoryGetItemName(PRGID PrgId,HANDLER Handle,DATA16 Item,DATA8 Length,DATA8 *pName,DATA8 *pType,DATA8 *pPriority)
{
  RESULT  Result = FAIL;
  FOLDER  *pMemory;
  char    Name[vmNAMESIZE];
  char    Ext[vmEXTSIZE];

  Result      =  cMemoryGetPointer(PrgId,Handle,((void**)&pMemory));
  *pType      =  0;
  *pPriority  =  127;

  if (Result == OK)
  { // Handle ok

    if ((Item > 0) && (Item <= (*pMemory).Entries))
    { // Item ok

      if (Length >= 2)
      {
        if (cMemoryCheckFilename((char*)(*pMemory).Entry[Item - 1],NULL,Name,Ext) == OK)
        {
          *pType  =  cMemoryFindType(Ext);
          if (strlen(Name) >= Length)
          {
            Name[Length - 1]  =  0;
            Name[Length - 2]  =  0x7F;
          }

          snprintf((char*)pName,(int)Length,"%s",Name);
          *pPriority  =  (*pMemory).Priority[Item - 1];
        }
        else
        {
          Result  =  FAIL;
        }
      }
      else
      {
        Result  =  FAIL;
      }
    }
    else
    {
      Result  =  FAIL;
    }

  }
  return (Result);
}


/*
 *  Get icon image
 */
RESULT    cMemoryGetItemIcon(PRGID PrgId,HANDLER Handle,DATA16 Item,HANDLER *pHandle,DATA32 *pImagePointer)
{
  RESULT  Result = FAIL;
  FOLDER  *pMemory;
  char    Filename[MAX_FILENAME_SIZE];
  DATA32  ISize;
  IP      pImage;
  struct  stat FileStatus;
  int     hFile;

  Result  =  cMemoryGetPointer(PrgId,Handle,((void**)&pMemory));

  if (Result == OK)
  { // Handle ok

    Result  =  FAIL;
    if ((Item > 0) && (Item <= (*pMemory).Entries))
    { // Item ok

      snprintf(Filename,MAX_FILENAME_SIZE,"%s/%s/%s%s",(char*)(*pMemory).Folder,(char*)(*pMemory).Entry[Item - 1],ICON_FILE_NAME,EXT_GRAPHICS);

      hFile  =  open(Filename,O_RDONLY);

      if (hFile >= MIN_HANDLE)
      {

        stat(Filename,&FileStatus);
        ISize  =  FileStatus.st_size;

        // allocate memory to contain the whole file:
        if (cMemoryAlloc(PrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pImage,pHandle) == OK)
        {

          if ((DATA32)read(hFile,pImage,ISize) == ISize)
          {
            *pImagePointer  =  (DATA32)pImage;
            Result  =  OK;
          }
        }

        close(hFile);
      }
    }
  }

  return (Result);
}


/*
 *  Get text
 */
RESULT    cMemoryGetItemText(PRGID PrgId,HANDLER Handle,DATA16 Item,DATA8 Length,DATA8 *pText)
{
  RESULT  Result = FAIL;
  FOLDER  *pMemory;
  char    Filename[MAX_FILENAME_SIZE];
  int     hFile;
  DATA8   Tmp;
  char    Termination[2] = "\t";
  ssize_t No;

  for (Tmp = 0;Tmp < Length;Tmp++)
  {
    pText[Tmp]  =  0;
  }
  Result  =  cMemoryGetPointer(PrgId,Handle,((void**)&pMemory));

  if (Result == OK)
  { // Handle ok

    if ((Item > 0) && (Item <= (*pMemory).Entries) && Length)
    { // Item ok

//      snprintf(Filename,MAX_FILENAME_SIZE,"%s/%s/%s%s",(char*)(*pMemory).Folder,(char*)(*pMemory).Entry[Item - 1],TEXT_FILE_NAME,EXT_TEXT);
      snprintf(Filename,MAX_FILENAME_SIZE,"%s/%s%s",vmSETTINGS_DIR,(char*)(*pMemory).Entry[Item - 1],EXT_TEXT);
      hFile   =  open(Filename,O_RDONLY);
      if (hFile >= MIN_HANDLE)
      {
        Result  =  OK;
        No  =  1;
        while ((No == 1) && (Length > 1))
        {
          No  =  read(hFile,&Tmp,1);
          if ((Tmp == Termination[0]) || (Tmp == '\r') || (Tmp == '\n'))
          {
            No  =  0;
          }
          if (No)
          {
            *pText  =  Tmp;
            pText++;
            *pText  =  0;
            Length--;
          }
        }
        close(hFile);
      }
    }
    else
    {
      Result  =  FAIL;
    }

  }

  return (Result);
}


/*
 *  Get text
 */
RESULT    cMemorySetItemText(PRGID PrgId,HANDLER Handle,DATA16 Item,DATA8 *pText)
{
  RESULT  Result = FAIL;
  FOLDER  *pMemory;
  char    Filename[MAX_FILENAME_SIZE];
  LFILE    *pFile;
  DATA8   Length;

  Length    =  (DATA8)strlen((char*)pText);
  Result    =  cMemoryGetPointer(PrgId,Handle,((void**)&pMemory));

  if (Result == OK)
  { // Handle ok

    if ((Item > 0) && (Item <= (*pMemory).Entries) && Length)
    { // Item ok

      snprintf(Filename,MAX_FILENAME_SIZE,"%s/%s/%s%s",(char*)(*pMemory).Folder,(char*)(*pMemory).Entry[Item - 1],TEXT_FILE_NAME,EXT_TEXT);

      pFile = fopen (Filename, "wb");
      if (NULL != pFile)
      {
        fwrite(pText,1,Length,pFile);
        Result  =  OK;
        fclose (pFile);
      }
    }
    else
    {
      Result  =  FAIL;
    }

  }

  return (Result);
}


/*
 *  Get full name including path and extension
 */
RESULT    cMemoryGetItem(PRGID PrgId,HANDLER Handle,DATA16 Item,DATA8 Length,DATA8 *pName,DATA8 *pType)
{
  RESULT  Result = FAIL;
  FOLDER  *pMemory;
  char    Folder[vmPATHSIZE];
  char    Name[vmNAMESIZE];
  char    Ext[vmEXTSIZE];

  Result  =  cMemoryGetPointer(PrgId,Handle,((void**)&pMemory));
  *pType  =  0;

  if (Result == OK)
  { // Handle ok

    if ((Item > 0) && (Item <= (*pMemory).Entries))
    { // Item ok

      if (cMemoryCheckFilename((char*)(*pMemory).Entry[Item - 1],Folder,Name,Ext) == OK)
      {
        *pType  =  cMemoryFindType(Ext);
        snprintf((char*)pName,(int)Length,"%s%s/%s",(char*)(*pMemory).Folder,Folder,Name);
      }
      else
      {
        Result  =  FAIL;
      }
    }
    else
    {
      Result  =  FAIL;
    }

  }

  return (Result);
}


/*
 *  Close directory stream and free memory handle
 */
void      cMemoryCloseFolder(PRGID PrgId,HANDLER *pHandle)
{
  RESULT  Result;
  FOLDER  *pMemory;

  Result  =  cMemoryGetPointer(PrgId,*pHandle,((void**)&pMemory));

  if (Result == OK)
  { // Handle ok

    if ((*pMemory).pDir != NULL)
    {
      closedir((*pMemory).pDir);
    }
    cMemoryFreePool(PrgId,(void*)pMemory);
  }
  *pHandle  =  0;
}


//******* BYTE CODE SNIPPETS **************************************************


/*! \page cMemory Memory
 *  <hr size="1"/>
 *  \subpage MemoryLibraryCodes
 *  \n
 *  \n
 *  <b>     opFILE (CMD, ....)  </b>
 *
 *- Memory file entry\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD               - \ref memoryfilesubcode
 *
 *\n
 *  - CMD = OPEN_APPEND
 *\n  Create file or open for append (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \return (HANDLER)  HANDLE      - Handle to file\n
 *
 *\n
 *  - CMD = OPEN_READ
 *\n  Open file for read (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \return (HANDLER)  HANDLE      - Handle to file\n
 *    -  \return (DATA32)   SIZE        - File size (0 = not found)\n
 *
 *\n
 *  - CMD = OPEN_WRITE
 *\n  Create file for write (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \return (HANDLER)  HANDLE      - Handle to file\n
 *
 *\n
 *  - CMD = CLOSE
 *\n  Close file\n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *
 *\n
 *  - CMD = REMOVE
 *\n  Delete file (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *
 *\n
 *  - CMD = MOVE
 *\n  Move file SOURCE to DEST (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    SOURCE      - First character in source file name (character string)\n
 *    -  \param  (DATA8)    DEST        - First character in destination file name (character string)\n
 *
 *\n
 *  - CMD = WRITE_TEXT
 *\n  Write text to file \n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA8)     \ref delimiters "DEL" - Delimiter code\n
 *    -  \param  (DATA8)    TEXT        - First character in text to write (character string)\n
 *
 *\n
 *  - CMD = READ_TEXT
 *\n  Read text from file \n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA8)     \ref delimiters "DEL" - Delimiter code\n
 *    -  \param  (DATA8)    LENGTH      - Maximal string length\n
 *    -  \return (DATA8)    TEXT        - First character in text to read (character string)\n
 *
 *\n
 *  - CMD = WRITE_VALUE
 *\n  Write floating point value to file \n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA8)     \ref delimiters "DEL" - Delimiter code\n
 *    -  \param  (DATAF)    VALUE       - Value to write\n
 *    -  \param  (DATA8)    FIGURES     - Total number of figures inclusive decimal point\n
 *    -  \param  (DATA8)    DECIMALS    - Number of decimals\n
 *
 *\n
 *  - CMD = READ_VALUE
 *\n  Read floating point value from file \n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA8)     \ref delimiters "DEL" - Delimiter code\n
 *    -  \return (DATAF)    VALUE       - Value to write\n
 *
 *\n
 *  - CMD = WRITE_BYTES
 *\n  Write a number of bytes to file \n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA16)   BYTES       - Number of bytes to write\n
 *    -  \param  (DATA8)    SOURCE      - First byte in byte stream to write\n
 *
 *\n
 *  - CMD = READ_BYTES
 *\n  Read a number of bytes from file \n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA16)   BYTES       - Number of bytes to write\n
 *    -  \return (DATA8)    DESTINATION - First byte in byte stream to write\n
 *
 *\n
 *  - CMD = OPEN_LOG
 *\n  Create file for data logging (if name starts with '~','/' or '.' it is not from current folder) (see \ref cinputsample "Example")\n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \param  (DATA32)   syncedTime  -
 *    -  \param  (DATA32)   syncedTick  -
 *    -  \param  (DATA32)   nowTick     -
 *    -  \param  (DATA32)   sample_interval_in_ms -
 *    -  \param  (DATA32)   duration_in_ms -
 *    -  \param  (DATA8)    SDATA       - First character in sensor type data (character string)\n
 *    -  \return (HANDLER)  HANDLE      - Handle to file\n
 *
 *\n
 *  - CMD = WRITE_LOG
 *\n  Write time slot samples to file (see \ref cinputsample "Example")\n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA32)   TIME        - Relative time in mS\n
 *    -  \param  (DATA8)    ITEMS       - Total number of values in this time slot\n
 *    .  \param  (DATAF)    VALUES      - DATAF array (handle) containing values\n
 *
 *\n
 *  - CMD = CLOSE_LOG
 *\n  Close data log file (see \ref cinputsample "Example")\n
 *    -  \param  (HANDLER)  HANDLE      - Handle to file\n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *
 *\n
 *  - CMD = GET_LOG_NAME
 *\n  Get the current open log filename\n
 *    -  \param  (DATA8)    LENGTH      - Max string length (don't care if NAME is a HND\n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string or HND)\n
 *
 *\n
 *  - CMD = GET_HANDLE
 *\n  Get handle from filename (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \return (HANDLER)  HANDLE      - Handle to file\n
 *    -  \return (DATA8)    WRITE       - Open for write / append (0 = no, 1 = yes)\n
 *
 *\n
 *  - CMD = MAKE_FOLDER
 *\n  Make folder if not present\n
 *    -  \param  (DATA8)    NAME        - First character in folder name (character string)\n
 *    -  \return (DATA8)    SUCCESS     - Success (0 = no, 1 = yes)\n
 *
 *\n
 *  - CMD = LOAD_IMAGE
 *    -  \param  (DATA16)   PRGID   - Program id (see \ref prgid)\n
 *    -  \param  (DATA8)    NAME    - First character in name (character string)\n
 *    -  \return (DATA32)   SIZE    - Size\n
 *    -  \return (DATA32)   *IP     - Address of image\n
 *
 *\n
 *  - CMD = GET_POOL
 *    -  \param  (DATA32)   SIZE    - Size of pool\n
 *    -  \return (HANDLER)  HANDLE  - Handle to pool\n
 *    -  \return (DATA32)   *IP     - Address of image\n
 *
 *\n
 *  - CMD = GET_FOLDERS
 *    -  \param  (DATA8)    NAME    - First character in folder name (ex "../prjs/")\n
 *    -  \return (DATA8)    ITEMS   - No of sub folders\n
 *
 *\n
 *  - CMD = GET_SUBFOLDER_NAME
 *    -  \param  (DATA8)    NAME    - First character in folder name (ex "../prjs/")\n
 *    -  \param  (DATA8)    ITEM    - Sub folder index [1..ITEMS]\n
 *    -  \param  (DATA8)    LENGTH  - Maximal string length\n
 *    -  \return (DATA8)    STRING  - First character in character string\n
 *
 *\n
 *  - CMD = DEL_SUBFOLDER
 *    -  \param  (DATA8)    NAME    - First character in folder name (ex "../prjs/")\n
 *    -  \param  (DATA8)    ITEM    - Sub folder index [1..ITEMS]\n
 *
 *\n
 *  - CMD = SET_LOG_SYNC_TIME
 *    -  \param  (DATA32)   TIME    - Sync time used in data log files\n
 *    -  \param  (DATA32)   TICK    - Sync tick used in data log files\n
 *
 *\n
 *  - CMD = GET_LOG_SYNC_TIME
 *    -  \return (DATA32)   TIME    - Sync time used in data log files\n
 *    -  \return (DATA32)   TICK    - Sync tick used in data log files\n
 *
 *\n
 *  - CMD = GET_IMAGE
 *    -  \param  (DATA8)    NAME    - First character in folder name (ex "../prjs/")\n
 *    -  \param  (DATA16)   PRGID   - Program id (see \ref prgid)\n
 *    -  \param  (DATA8)    ITEM    - Sub folder index [1..ITEMS]\n
 *    -  \return (DATA32)   *IP     - Address of image\n
 *
 *\n
 *  - CMD = GET_ITEM
 *    -  \param  (DATA8)    NAME    - First character in folder name (ex "../prjs/")\n
 *    -  \param  (DATA8)    STRING  - First character in item name string\n
 *    -  \return (DATA8)    ITEM    - Sub folder index [1..ITEMS]\n
 *
 *\n
 *  - CMD = GET_CACHE_FILES
 *    -  \return (DATA8)    ITEMS   - No of files in cache\n
 *
 *\n
 *  - CMD = GET_CACHE_FILE
 *    -  \param  (DATA8)    ITEM    - Cache index [1..ITEMS]\n
 *    -  \param  (DATA8)    LENGTH  - Maximal string length\n
 *    -  \return (DATA8)    STRING  - First character in character string\n
 *
 *\n
 *  - CMD = PUT_CACHE_FILE
 *    -  \param  (DATA8)    STRING  - First character in character string\n
 *
 *\n
 *  - CMD = DEL_CACHE_FILE
 *    -  \param  (DATA8)    ITEM    - Cache index [1..ITEMS]\n
 *    -  \param  (DATA8)    LENGTH  - Maximal string length\n
 *    -  \return (DATA8)    STRING  - First character in character string\n
 *
 *\n
 *
 */
/*! \brief  opFILE byte code
 *
 */
void      cMemoryFile(void)
{

  IP      TmpIp;
  DSPSTAT DspStat = BUSYBREAK;
  DATA8   Cmd;
  DATA32  ImagePointer;
  DATA32  ISize;
  IP      pImage;
  HANDLER TmpHandle;
  PRGID   TmpPrgId;
  DATA32  Data32;

  DATA8   *pFileName;
  char    FilenameBuf[vmFILENAMESIZE];
  char    PathBuf[vmPATHSIZE];
  char    NameBuf[vmNAMESIZE];
  char    ExtBuf[vmEXTSIZE];

  char    SourceBuf[vmFILENAMESIZE];
  char    DestinationBuf[vmFILENAMESIZE];

  DATA8   *pSource;
  DATA8   *pDestination;

  char    PrgNamePath[SUBFOLDERNAME_SIZE];
  char    PrgNameBuf[MAX_FILENAME_SIZE];
  char    DestinationName[MAX_FILENAME_SIZE];
  DATA16  PrgNo;
  DATA8   Item;
  DATA8   Items;
  DATA8   Lng;
  DATA8   Tmp;
  DATA8   Del;
  DATA8   *pFolderName;
  int     hFile;
  char    Buffer[LOGBUFFER_SIZE];
  DATAF   DataF;
  DATA16  Bytes;
  DATA8   Figures;
  DATA8   Decimals;
  struct  stat FileStatus;
  DATAF   *pValue;
  DATA32  Time;
  DESCR   *pDescr;
  DATA32  UsedElements;
  DATA32  Elements;
  DATA32  ElementSize;

  DATA32  STime;
  DATA32  STick;
  DATA32  NTick;
  DATA32  SIIM;
  DATA32  DIM;
  DATA8   *pSData;
  DATA8   Error = 0;
  DATA32  TotalRam;
  DATA32  FreeRam;

  void    *pTmp;
  HANDLER TmpHandle2;

  TmpPrgId      =  CurrentProgramId();
  TmpIp         =  GetObjectIp();
  Cmd           =  *(DATA8*)PrimParPointer();
  ImagePointer  =  (DATA32)NULL;
  ISize         =  (DATA32)0;

  switch (Cmd)
  { // Function

    case OPEN_APPEND :
    {
      pFileName     =  (DATA8*)PrimParPointer();

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,"") == OK)
      {

#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: OPEN_APPEND [%s]\r\n",FilenameBuf);
#endif
        DspStat       =   cMemoryOpenFile(TmpPrgId,OPEN_FOR_APPEND,(char*)FilenameBuf,&TmpHandle,&ISize);
      }

      *(DATA16*)PrimParPointer()  =  TmpHandle;
    }
    break;

    case OPEN_READ :
    {
      pFileName     =  (DATA8*)PrimParPointer();

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,"") == OK)
      {

#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: OPEN_READ   [%s]\r\n",FilenameBuf);
#endif
        DspStat       =  cMemoryOpenFile(TmpPrgId,OPEN_FOR_READ,FilenameBuf,&TmpHandle,&ISize);
      }

      *(DATA16*)PrimParPointer()  =  TmpHandle;
      *(DATA32*)PrimParPointer()  =  ISize;
    }
    break;

    case OPEN_WRITE :
    {
      pFileName     =  (DATA8*)PrimParPointer();

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,"") == OK)
      {

#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: OPEN_WRITE  [%s]\r\n",FilenameBuf);
#endif
        DspStat       =  cMemoryOpenFile(TmpPrgId,OPEN_FOR_WRITE,FilenameBuf,&TmpHandle,&ISize);

      }

      *(DATA16*)PrimParPointer()  =  TmpHandle;
    }
    break;

    case CLOSE :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();

      DspStat       =  cMemoryCloseFile(TmpPrgId,TmpHandle);
    }
    break;

    case WRITE_TEXT :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Del           =  *(DATA8*)PrimParPointer();
      pSource       =  (DATA8*)PrimParPointer();

      DspStat       =  cMemoryWriteFile(TmpPrgId,TmpHandle,(DATA32)strlen((char*)pSource),Del,pSource);
    }
    break;

    case READ_TEXT :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Del           =  *(DATA8*)PrimParPointer();
      Lng           =  (DATA8)(*(DATA16*)PrimParPointer());
      pDestination  =  (DATA8*)PrimParPointer();

      DspStat       =  cMemoryReadFile(TmpPrgId,TmpHandle,(DATA32)Lng,Del,pDestination);
    }
    break;

    case WRITE_VALUE :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Del           =  *(DATA8*)PrimParPointer();
      DataF         =  *(DATAF*)PrimParPointer();
      Figures       =  *(DATA8*)PrimParPointer();
      Decimals      =  *(DATA8*)PrimParPointer();

      snprintf(Buffer,LOGBUFFER_SIZE,"%*.*f",Figures,Decimals,DataF);
      DspStat       =  cMemoryWriteFile(TmpPrgId,TmpHandle,(DATA32)strlen((char*)Buffer),Del,(DATA8*)Buffer);
    }
    break;

    case READ_VALUE :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Del           =  *(DATA8*)PrimParPointer();

      Lng           =  64;
      Buffer[0]     =  0;
      pDestination  =  (DATA8*)Buffer;
      DataF         =  (DATAF)0;
      DspStat       =  cMemoryReadFile(TmpPrgId,TmpHandle,(DATA32)Lng,Del,pDestination);
      sscanf(Buffer,"%f",&DataF);

      *(DATAF*)PrimParPointer()  =  DataF;
    }
    break;

    case WRITE_BYTES :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Bytes         =  *(DATA16*)PrimParPointer();
      pSource       =  (DATA8*)PrimParPointer();

      DspStat       =  cMemoryWriteFile(TmpPrgId,TmpHandle,(DATA32)Bytes,DEL_NONE,pSource);
    }
    break;

    case READ_BYTES :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Bytes         =  *(DATA16*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      DspStat       =  cMemoryReadFile(TmpPrgId,TmpHandle,(DATA32)Bytes,DEL_NONE,pDestination);
    }
    break;

    case OPEN_LOG :
    {
      pFileName     =  (DATA8*)PrimParPointer();

      STime         =  *(DATA32*)PrimParPointer();
      STick         =  *(DATA32*)PrimParPointer();
      NTick         =  *(DATA32*)PrimParPointer();
      SIIM          =  *(DATA32*)PrimParPointer();
      DIM           =  *(DATA32*)PrimParPointer();

      pSData        =  (DATA8*)PrimParPointer();

      TmpHandle     =  0;

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,vmEXT_DATALOG) == OK)
      {

#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: OPEN_LOG    [%s]\r\n",FilenameBuf);
#endif
        Bytes         =  snprintf(Buffer,LOGBUFFER_SIZE,"Sync data\t%d\t%d\t%d\t%d\t%d\r\n%s",STime,STick,NTick,SIIM,DIM,pSData);

        DspStat       =  NOBREAK;

        if ((SIIM < MIN_LIVE_UPDATE_TIME) || (FilenameBuf[0] == 0))
        { // Log in ram

          if (FilenameBuf[0])
          {
            DspStat   =  cMemoryOpenFile(TmpPrgId,OPEN_FOR_LOG,FilenameBuf,&TmpHandle,&ISize);
          }
          if (DspStat == NOBREAK)
          {
            Elements      =  LOGBUFFER_SIZE;

            ElementSize   =  sizeof(DATA8);
            ISize         =  Elements * ElementSize + sizeof(DESCR);

            if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pTmp,&TmpHandle) == OK)
            {
              (*(DESCR*)pTmp).Type          =  DATA_8;
              (*(DESCR*)pTmp).ElementSize   =  (DATA8)ElementSize;
              (*(DESCR*)pTmp).Elements      =  Elements;
              (*(DESCR*)pTmp).UsedElements  =  0;

  #ifdef DEBUG_C_MEMORY_LOG
              if (pFileName[0])
              {
                printf("LOG_OPEN  %d into ram file %s\r\n",TmpHandle,FilenameBuf);
                printf("  header  %d into ram file %d bytes\r\n",TmpHandle,Bytes);
              }
              else
              {
                printf("LOG_OPEN  %d into ram\r\n",TmpHandle);
                printf("  header  %d into ram %d bytes\r\n",TmpHandle,Bytes);
              }
  #endif
              pDescr        =  (DESCR*)pTmp;

              pDestination  =  (DATA8*)(*pDescr).pArray;
              UsedElements  =  (*pDescr).UsedElements;

              Elements      =  0;
              while (Bytes)
              {
                pDestination[UsedElements]  =  Buffer[Elements];
                UsedElements++;
                Elements++;
                Bytes--;
              }
              (*pDescr).UsedElements  =  UsedElements;
            }
            else
            {
              DspStat   =  FAILBREAK;
            }
          }
        }
        else
        { // Log in file

          DspStat   =  cMemoryOpenFile(TmpPrgId,OPEN_FOR_LOG,FilenameBuf,&TmpHandle,&ISize);
          if (DspStat == NOBREAK)
          {
            DspStat =  cMemoryWriteFile(TmpPrgId,TmpHandle,(DATA32)Bytes,DEL_NONE,(DATA8*)Buffer);
  #ifdef DEBUG_C_MEMORY_LOG
            printf("LOG_OPEN  %d into file %s\r\n",TmpHandle,(char*)pFileName);
            printf("  header  %d file %d bytes\r\n",TmpHandle,Bytes);
  #endif
          }
        }
      }
      *(HANDLER*)PrimParPointer()     =  (HANDLER)TmpHandle;
    }
    break;

    case WRITE_LOG :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      Time          =  *(DATA32*)PrimParPointer();
      Items         =  *(DATA8*)PrimParPointer();
      pValue        =  (DATAF*)PrimParPointer();

      DspStat       =  FAILBREAK;

      if (Items)
      {
#ifndef LOG_ASCII

        DataF     =  (DATAF)Time;
        Bytes     =  0;

        memcpy((void*)&Buffer[Bytes],(void*)&DataF,sizeof(DATAF));
        Bytes    +=  sizeof(DATAF);

        for (Item = 0;Item < Items;Item++)
        {
          memcpy((void*)&Buffer[Bytes],(void*)&pValue[Item],sizeof(DATAF));
          Bytes  +=  sizeof(DATAF);
        }

#else
        Bytes  =  (DATA16)snprintf(Buffer,LOGBUFFER_SIZE,"%08d\t",Time);
        for (Item = 0;Item < Items;Item++)
        {

          if (Item != (Items - 1))
          {
            Bytes +=  snprintf(&Buffer[Bytes],LOGBUFFER_SIZE - Bytes,"%.1f\t",pValue[Item]);
          }
          else
          {
            Bytes +=  snprintf(&Buffer[Bytes],LOGBUFFER_SIZE - Bytes,"%.1f\r\n",pValue[Item]);
          }
        }
#endif

        if (MemoryInstance.pPoolList[TmpPrgId][TmpHandle].Type == POOL_TYPE_MEMORY)
        { // Log to memory

          if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
          {
            pDescr        =  (DESCR*)pTmp;

            UsedElements  =  (DATA32)Bytes + (*pDescr).UsedElements;
            Elements      =  (*pDescr).Elements;

            if (UsedElements > Elements)
            {
              Elements +=  LOGBUFFER_SIZE;
#ifdef DEBUG_C_MEMORY_LOG
              printf("LOG_WRITE %d resizing ram to %d\r\n",TmpHandle,Elements);
#endif
              cMemoryGetUsage(&TotalRam,&FreeRam,0);
#ifdef DEBUG_C_MEMORY_LOG
              printf("Free memory %u KB\r\n",FreeRam);
#endif
              if (FreeRam > RESERVED_MEMORY)
              {
                if (cMemoryResize(TmpPrgId,TmpHandle,Elements) == NULL)
                {
                  Error  =  OUT_OF_MEMORY;
                }
              }
              else
              {
                Error  =  OUT_OF_MEMORY;
              }

            }
            if (!Error)
            {
              if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
              {
                pDescr        =  (DESCR*)pTmp;

                pDestination  =  (DATA8*)(*pDescr).pArray;
                UsedElements  =  (*pDescr).UsedElements;

  #ifdef DEBUG_C_MEMORY_LOG
                printf("LOG_WRITE %d ram %d bytes\r\n",TmpHandle,Bytes);
  #endif
                memcpy((void*)&pDestination[UsedElements],Buffer,(size_t)Bytes);
                (*pDescr).UsedElements  =  UsedElements + (DATA32)Bytes;

                DspStat     =  NOBREAK;

              }
              else
              {
                Error  =  OUT_OF_MEMORY;
              }
            }
          }

        }
        else
        { // Log to file

#ifdef DEBUG_C_MEMORY_LOG
          printf("LOG_WRITE %d file %d bytes\r\n",TmpHandle,Bytes);
#endif
          DspStat       =  cMemoryWriteFile(TmpPrgId,TmpHandle,(DATA32)Bytes,DEL_NONE,(DATA8*)Buffer);
        }
      }
      DspStat     =  NOBREAK;
    }
    break;

    case CLOSE_LOG :
    {
      TmpHandle     =  *(DATA16*)PrimParPointer();
      pFileName     =  (DATA8*)PrimParPointer();

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,vmEXT_DATALOG) == OK)
      {
        DspStat     =  cMemoryGetFileHandle(TmpPrgId,FilenameBuf,&TmpHandle2,&Tmp);
#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: CLOSE_LOG   [%s]\r\n",FilenameBuf);
#endif

        if (MemoryInstance.pPoolList[TmpPrgId][TmpHandle].Type == POOL_TYPE_MEMORY)
        { // Log to memory

          if (FilenameBuf[0])
          {
            if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
            {
              pDescr      =  (DESCR*)pTmp;

              if (DspStat == NOBREAK)
              {
                pSource       =  (DATA8*)(*pDescr).pArray;
                UsedElements  =  (*pDescr).UsedElements;

#ifndef LOG_ASCII
                Buffer[0]  =  0xFF;
                Buffer[1]  =  0xFF;
                Buffer[2]  =  0xFF;
                Buffer[3]  =  0xFF;
                Buffer[4]  =  0xFF;
                Buffer[5]  =  0xFF;
                Buffer[6]  =  0xFF;
                Buffer[7]  =  0xFF;

                Bytes      =  8;
#else
                Buffer[0]  =  'F';
                Buffer[1]  =  'F';
                Buffer[2]  =  'F';
                Buffer[3]  =  'F';
                Buffer[4]  =  'F';
                Buffer[5]  =  'F';
                Buffer[6]  =  'F';
                Buffer[7]  =  'F';
                Buffer[8]  =  '\r';
                Buffer[9]  =  '\n';

                Bytes      =  10;
#endif


  #ifdef DEBUG_C_MEMORY_LOG
                printf("LOG_WRITE %d ram %d log end signature\r\n",TmpHandle,Bytes);
  #endif
                memcpy((void*)&pSource[UsedElements],Buffer,(size_t)Bytes);
                UsedElements +=  (DATA32)Bytes;

                cMemoryGetUsage(&TotalRam,&FreeRam,0);
                if (UsedElements > ((FreeRam - RESERVED_MEMORY) * KB))
                {
                  UsedElements  =  (FreeRam - RESERVED_MEMORY) * KB;
                }

  #ifdef DEBUG_C_MEMORY_LOG
                printf("LOG_CLOSE %d ram and save %d bytes to %s\r\n",TmpHandle,UsedElements,(char*)pFileName);
  #endif
                DspStat   =  cMemoryWriteFile(TmpPrgId,TmpHandle2,(DATA32)UsedElements,DEL_NONE,pSource);
              }
              if (DspStat == NOBREAK)
              {
                DspStat   =  cMemoryCloseFile(TmpPrgId,TmpHandle2);
                sync();
              }
            }
          }
          else
          {
            cMemoryFreeHandle(TmpPrgId,TmpHandle);

            DspStat       =  NOBREAK;
          }
        }
        else
        {
#ifndef LOG_ASCII
          Buffer[0]  =  0xFF;
          Buffer[1]  =  0xFF;
          Buffer[2]  =  0xFF;
          Buffer[3]  =  0xFF;
          Buffer[4]  =  0xFF;
          Buffer[5]  =  0xFF;
          Buffer[6]  =  0xFF;
          Buffer[7]  =  0xFF;

          Bytes      =  8;
#else
          Buffer[0]  =  'F';
          Buffer[1]  =  'F';
          Buffer[2]  =  'F';
          Buffer[3]  =  'F';
          Buffer[4]  =  'F';
          Buffer[5]  =  'F';
          Buffer[6]  =  'F';
          Buffer[7]  =  'F';
          Buffer[8]  =  '\r';
          Buffer[9]  =  '\n';

          Bytes      =  10;
#endif

  #ifdef DEBUG_C_MEMORY_LOG
          printf("LOG_WRITE %d file %d 0xFF\r\n",TmpHandle,Bytes);
  #endif
          DspStat       =  cMemoryWriteFile(TmpPrgId,TmpHandle,(DATA32)Bytes,DEL_NONE,(DATA8*)Buffer);

          DspStat       =  cMemoryCloseFile(TmpPrgId,TmpHandle);

  #ifdef DEBUG_C_MEMORY_LOG
          printf("LOG_CLOSE %d file\r\n",TmpHandle);
  #endif
          sync();
        }
      }
      DspStat       =  NOBREAK;
    }
    break;

    case GET_LOG_NAME :
    {
      Lng           = *(DATA8*)PrimParPointer();
      pFileName     =  (DATA8*)PrimParPointer();

      cMemoryFindLogName(TmpPrgId,DestinationName);

      if (VMInstance.Handle >= 0)
      {
        Data32        =  (DATA32)strlen(DestinationName);
        Data32       +=  1;
        if (Data32 > MIN_ARRAY_ELEMENTS)
        {
          pFileName   =  (DATA8*)VmMemoryResize(VMInstance.Handle,Data32);
        }
        Lng  =  (DATA8)Data32;
      }
      if (pFileName != NULL)
      {
        snprintf((char*)pFileName,(int)Lng,"%s",DestinationName);
      }
      DspStat       =  NOBREAK;
    }
    break;

    case GET_HANDLE :
    {
      pFileName     =  (DATA8*)PrimParPointer();

      TmpHandle     =  0;
      Tmp           =  0;

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,"") == OK)
      {
        DspStat     =  cMemoryGetFileHandle(TmpPrgId,FilenameBuf,&TmpHandle,&Tmp);
#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: GET_HANDLE  [%s]\r\n",FilenameBuf);
#endif
      }

      *(DATA16*)PrimParPointer()  =  TmpHandle;
      *(DATA8*)PrimParPointer()   =  Tmp;
    }
    break;

    case REMOVE :
    {
      pFileName     =  (DATA8*)PrimParPointer();

      if (ConstructFilename(TmpPrgId,(char*)pFileName,FilenameBuf,"") == OK)
      {
        cMemoryDeleteSubFolders(FilenameBuf);
#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: REMOVE      [%s]\r\n",FilenameBuf);
#endif
        SetUiUpdate();
      }
      DspStat       =  NOBREAK;
    }
    break;

    case MAKE_FOLDER :
    {
      pDestination  =  (DATA8*)PrimParPointer();

      snprintf(PathBuf,vmPATHSIZE,"%s",pDestination);

      Data32  =  (DATA32)strlen(PathBuf);
      if (Data32)
      {
        if (PathBuf[Data32 - 1] == '/')
        {
          PathBuf[Data32 - 1]  =  0;
        }
      }

      Tmp        =  0;
      if (stat((char*)PathBuf,&FileStatus) == 0)
      {
        Tmp      =  1;
#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: MAKE_FOLDER [%s] already present\r\n",PathBuf);
#endif
      }
      else
      {
        mkdir((char*)PathBuf,DIRPERMISSIONS);
        chmod((char*)PathBuf,DIRPERMISSIONS);
        sync();

#ifdef DEBUG_TRACE_FILENAME
        printf("c_memory  cMemoryFile: MAKE_FOLDER [%s]\r\n",PathBuf);
#endif

        if (stat((char*)PathBuf,&FileStatus) == 0)
        {
          Tmp      =  1;
        }
      }
      *(DATA8*)PrimParPointer()  =  Tmp;

      DspStat       =  NOBREAK;
    }
    break;

    case MOVE :
    {
      pSource       =  (DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      if (ConstructFilename(TmpPrgId,(char*)pSource,SourceBuf,"") == OK)
      {
        if (ConstructFilename(TmpPrgId,(char*)pDestination,DestinationBuf,"") == OK)
        {

          snprintf(Buffer,LOGBUFFER_SIZE,"cp -r \"%s\" \"%s\"",SourceBuf,DestinationBuf);
#ifdef DEBUG_TRACE_FILENAME
          printf("c_memory  cMemoryFile: MOVE        [%s]\r\n",Buffer);
#endif

          if (stat(DestinationBuf,&FileStatus) == 0)
          { // Exist

            cMemoryDeleteSubFolders(DestinationBuf);
#ifdef DEBUG_TRACE_FILENAME
            printf("  c_memory  cMemoryFile: remove    [%s]\r\n",DestinationBuf);
#endif
            sync();
          }

          system(Buffer);
          sync();
          SetUiUpdate();
        }
      }

      DspStat       =  NOBREAK;
    }
    break;

    case LOAD_IMAGE :
    {
      PrgNo         =  *(DATA16*)PrimParPointer();
      pFileName     =   (DATA8*)PrimParPointer();
      DspStat       =  FAILBREAK;
      ISize         =  0;
      ImagePointer  =  0;

      if (ProgramStatus(PrgNo) == STOPPED)
      {
        if (cMemoryCheckFilename((char*)pFileName,PathBuf,NameBuf,ExtBuf) == OK)
        { // Filename OK

          if (PathBuf[0] == 0)
          { // Default path

            snprintf(PathBuf,vmPATHSIZE,"%s/",DEFAULT_FOLDER);
          }

          if (ExtBuf[0] == 0)
          { // Default extension

            snprintf(ExtBuf,vmEXTSIZE,"%s",EXT_BYTECODE);
          }

          // Save path
          snprintf((char*)MemoryInstance.PathList[PrgNo],vmPATHSIZE,"%s",PathBuf);

          // Save name for run screen
          snprintf((char*)VMInstance.Program[PrgNo].Name,vmNAMESIZE,"%s",NameBuf);

          // Construct filename for open
          snprintf(FilenameBuf,vmFILENAMESIZE,"%s%s%s",PathBuf,NameBuf,ExtBuf);

#ifdef DEBUG_TRACE_FILENAME
          printf("c_memory  cMemoryFile: LOAD_IMAGE  [%s]\r\n",FilenameBuf);
#endif
          hFile         =  open(FilenameBuf,O_RDONLY);

          if (hFile >= MIN_HANDLE)
          {
            stat(FilenameBuf,&FileStatus);
            ISize  =  FileStatus.st_size;

            // allocate memory to contain the whole file:
            if (cMemoryAlloc(PrgNo,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pImage,&TmpHandle) == OK)
            {
              if (ISize == read(hFile,pImage,ISize))
              {
                ImagePointer  =  (DATA32)pImage;
                DspStat       =  NOBREAK;
              }
            }

            close(hFile);
          }
          *(DATA32*)PrimParPointer()  =  ISize;
          *(DATA32*)PrimParPointer()  =  ImagePointer;
        }
        else
        {
          PrimParPointer();
          PrimParPointer();
        }
      }
      else
      {
        PrimParPointer();
        PrimParPointer();
      }
      DspStat       =  NOBREAK;
    }
    break;

    case GET_POOL :
    {
      ISize       =  *(DATA32*)PrimParPointer();
      DspStat     =  FAILBREAK;
      TmpHandle   = -1;

      if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pImage,&TmpHandle) == OK)
      {
        ImagePointer  =  (DATA32)pImage;
        DspStat       =  NOBREAK;
      }

      *(DATA16*)PrimParPointer()  =  TmpHandle;
      *(DATA32*)PrimParPointer()  =  ImagePointer;
    }
    break;

    case GET_FOLDERS :
    {
      pFolderName  =  (DATA8*)PrimParPointer();
      *(DATA8*)PrimParPointer()  =  cMemoryFindSubFolders((char*)pFolderName);

      DspStat  =  NOBREAK;
    }
    break;

    case GET_SUBFOLDER_NAME :
    {
      pFolderName   =  (DATA8*)PrimParPointer();
      Item          = *(DATA8*)PrimParPointer();
      Lng           = *(DATA8*)PrimParPointer();
      pDestination  =  (DATA8*)PrimParPointer();

      cMemoryGetSubFolderName(Item,Lng,(char*)pFolderName,(char*)pDestination);

      DspStat  =  NOBREAK;
    }
    break;

    case DEL_SUBFOLDER :
    {
      pFolderName   =  (DATA8*)PrimParPointer();
      Item          = *(DATA8*)PrimParPointer();
      cMemoryGetSubFolderName(Item,SUBFOLDERNAME_SIZE,(char*)pFolderName,PrgNamePath);

      snprintf(PrgNameBuf,MAX_FILENAME_SIZE,"%s%s",(char*)pFolderName,PrgNamePath);
#ifdef DEBUG
      printf("Trying to delete %s\r\n",PrgNameBuf);
#endif
      cMemoryDeleteSubFolders(PrgNameBuf);

      DspStat  =  NOBREAK;
    }
    break;

    case SET_LOG_SYNC_TIME :
    {
      MemoryInstance.SyncTime   =  *(DATA32*)PrimParPointer();
      MemoryInstance.SyncTick   =  *(DATA32*)PrimParPointer();

      DspStat  =  NOBREAK;
    }
    break;

    case GET_LOG_SYNC_TIME :
    {
      *(DATA32*)PrimParPointer()  =  MemoryInstance.SyncTime;
      *(DATA32*)PrimParPointer()  =  MemoryInstance.SyncTick;

      DspStat  =  NOBREAK;
    }
    break;

    case GET_ITEM :
    {
      pFolderName   =  (DATA8*)PrimParPointer();
      pFileName     =  (DATA8*)PrimParPointer();
      DspStat       =  NOBREAK;

      Items   =  cMemoryFindSubFolders((char*)pFolderName);
      Tmp     =  0;
      Item    =  0;

      while ((Items > 0) && (Item == 0))
      {
        Tmp++;
        cMemoryGetSubFolderName(Tmp,SUBFOLDERNAME_SIZE,(char*)pFolderName,(char*)PrgNamePath);
#ifdef DEBUG
        printf("%s %s\r\n",(char*)pFileName,PrgNamePath);
#endif
        if (strcmp((char*)pFileName,PrgNamePath) == 0)
        {
          Item  =  Tmp;
#ifdef DEBUG
          printf("Found %i\r\n",Item);
#endif
        }
        Items--;
      }
      *(DATA8*)PrimParPointer()  =  Item;

    }
    break;

    case GET_CACHE_FILES :
    {
      Items  =  0;
      for (Tmp = 0;Tmp < CACHE_DEEPT;Tmp++)
      {
        if (MemoryInstance.Cache[Tmp][0])
        {
          Items++;
        }
      }
      *(DATA8*)PrimParPointer()  =  Items;
#ifdef DEBUG
      printf("GET_CACHE_FILES %d\r\n",Items);
#endif
      DspStat  =  NOBREAK;
    }
    break;

    case PUT_CACHE_FILE :
    {
      pFileName       =   (DATA8*)PrimParPointer();

#ifdef DEBUG
      printf("PUT_CACHE_FILE %s\r\n",(char*)pFileName);
#endif
      DspStat                     =  NOBREAK;

      if (cMemoryCheckFilename((char*)pFileName,PathBuf,NameBuf,ExtBuf) == OK)
      { // Filename OK

        Item  =  0;
        Tmp   =  0;
        while ((Item < CACHE_DEEPT) && (Tmp == 0))
        {
          if (strcmp((char*)MemoryInstance.Cache[Item],(char*)pFileName) == 0)
          {
            Tmp  =  1;
          }
          else
          {
            Item++;
          }
        }
        while (Item < (CACHE_DEEPT - 1))
        {
          strcpy((char*)MemoryInstance.Cache[Item],(char*)MemoryInstance.Cache[Item + 1]);
          Item++;
        }
        MemoryInstance.Cache[Item][0]   =  0;

        for (Item = (CACHE_DEEPT - 1);Item > 0;Item--)
        {
          strcpy((char*)MemoryInstance.Cache[Item],(char*)MemoryInstance.Cache[Item - 1]);
        }
        strcpy((char*)MemoryInstance.Cache[Item],(char*)pFileName);
      }
    }
    break;

    case DEL_CACHE_FILE :
    {
      pFileName   =   (DATA8*)PrimParPointer();

      cMemoryDeleteCacheFile((char*)pFileName);

      DspStat     =  NOBREAK;
    }
    break;

    case GET_CACHE_FILE :
    {
      Item              = *(DATA8*)PrimParPointer();
      Lng               = *(DATA8*)PrimParPointer();
      pDestination      =  (DATA8*)PrimParPointer();

      DspStat  =  FAILBREAK;
      if ((Item > 0) && (Item <= CACHE_DEEPT))
      {
        if (MemoryInstance.Cache[Item - 1])
        {
          snprintf((char*)pDestination,Lng,"%s",MemoryInstance.Cache[Item - 1]);
          DspStat  =  NOBREAK;
        }
      }
    }
    break;

    default :
    {
      DspStat  =  FAILBREAK;
    }
    break;

  }

  if (Error)
  {
    if (!LogErrorNumberExists(Error))
    {
      LogErrorNumber(Error);
    }
  }

  if (DspStat == BUSYBREAK)
  { // Rewind IP

    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cMemory
 *  <hr size="1"/>
 *  <b>     opARRAY (CMD, ....)  </b>
 *
 *- Array entry\n
 *- Dispatch status can change to BUSYBREAK or FAILBREAK
 *
 *  \param  (DATA8)   CMD     - \ref memoryarraysubcode
 *
 *\n
 *  - CMD = CREATE8
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements\n
 *    -   \return (HANDLER) HANDLE    - Array handle\n
 *
 *\n
 *  - CMD = CREATE16
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements\n
 *    -   \return (HANDLER) HANDLE    - Array handle\n
 *
 *\n
 *  - CMD = CREATE32
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements\n
 *    -   \return (HANDLER) HANDLE    - Array handle\n
 *
 *\n
 *  - CMD = CREATEF
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements\n
 *    -   \return (HANDLER) HANDLE    - Array handle\n
 *
 *\n
 *  - CMD = SIZE
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \return (DATA32)  ELEMENTS  - Total number of elements in array\n
 *
 *\n
 *  - CMD = RESIZE
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  ELEMENTS  - Total number of elements\n
 *
 *\n
 *  - CMD = DELETE
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *
 *\n
 *  - CMD = FILL
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (type)    VALUE     - Value to write - type depends on type of array\n
 *
 *\n
 *  - CMD = COPY
 *    -   \param  (HANDLER) HSOURCE   - Source array handle\n
 *    -   \param  (HANDLER) HDEST     - Destination array handle\n
 *
 *\n
 *  - CMD = INIT8
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  INDEX     - Index to element to write
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements to write\n
 *
 *    Below a number of VALUES equal to "ELEMENTS"
 *    -   \param  (DATA8)   VALUE     - First value to write - type must be equal to the array type\n
 *
 *\n
 *  - CMD = INIT16
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  INDEX     - Index to element to write
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements to write\n
 *
 *    Below a number of VALUES equal to "ELEMENTS"
 *    -   \param  (DATA16)  VALUE     - First value to write - type must be equal to the array type\n
 *
 *\n
 *  - CMD = INIT32
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  INDEX     - Index to element to write
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements to write\n
 *
 *    Below a number of VALUES equal to "ELEMENTS"
 *    -   \param  (DATA32)  VALUE     - First value to write - type must be equal to the array type\n
 *
 *\n
 *  - CMD = INITF
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  INDEX     - Index to element to write
 *    -   \param  (DATA32)  ELEMENTS  - Number of elements to write\n
 *
 *    Below a number of VALUES equal to "ELEMENTS"
 *    -   \param  (DATAF)   VALUE     - First value to write - type must be equal to the array type\n
 *
 *\n
 *  - CMD = READ_CONTENT
 *    -   \param  (DATA16)  PRGID     - Program slot number (must be running) (see \ref prgid)\n
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  INDEX     - Index to first byte to read\n
 *    -   \param  (DATA32)  BYTES     - Number of bytes to read\n
 *    -   \return (DATA8)   ARRAY     - First byte of array to receive data\n
 *
 *\n
 *  - CMD = WRITE_CONTENT
 *    -   \param  (DATA16)  PRGID     - Program slot number (must be running) (see \ref prgid)\n
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \param  (DATA32)  INDEX     - Index to first byte to write\n
 *    -   \param  (DATA32)  BYTES     - Number of bytes to write\n
 *    -   \param  (DATA8)   ARRAY     - First byte of array to deliver data\n
 *
 *\n
 *  - CMD = READ_SIZE
 *    -   \param  (DATA16)  PRGID     - Program slot number (must be running) (see \ref prgid)\n
 *    -   \param  (HANDLER) HANDLE    - Array handle\n
 *    -   \return (DATA32)  BYTES     - Number of bytes in array\n
 *
 *\n
 *
 */


/*! \brief  opARRAY byte code
 *
 */
void      cMemoryArray(void)
{
  DSPSTAT DspStat = BUSYBREAK;
  PRGID   TmpPrgId;
  PRGID   PrgId;
  IP      TmpIp;
  DATA8   Cmd;
  HANDLER TmpHandle;
  HANDLER hSource;
  HANDLER hDest;
  void    *pTmp;
  void    *pSource;
  void    *pDest;
  DATA32  Elements;
  DATA32  Index;
  DATA32  ISize;
  DATA32  ElementSize;
  DATA8   *pData8;
  DATA16  *pData16;
  DATA32  *pData32;
  DATAF   *pDataF;
  DATA8   *pDData8;
  DATA8   Data8;
  DATA16  Data16;
  DATA32  Data32;
  DATAF   DataF;
  DATA32  Bytes;
  void    *pArray;

  TmpPrgId        =  CurrentProgramId();
  TmpIp           =  GetObjectIp();

  Cmd             =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case CREATE8 :
    {
      Elements    =  *(DATA32*)PrimParPointer();

      if (Elements < MIN_ARRAY_ELEMENTS)
      {
        Elements  =  MIN_ARRAY_ELEMENTS;
      }

      ElementSize =  sizeof(DATA8);
      ISize       =  Elements * ElementSize + sizeof(DESCR);

      if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pTmp,&TmpHandle) == OK)
      {
        (*(DESCR*)pTmp).Type          =  DATA_8;
        (*(DESCR*)pTmp).ElementSize   =  (DATA8)ElementSize;
        (*(DESCR*)pTmp).Elements      =  Elements;

        DspStat   =  NOBREAK;
      }
      else
      {
        DspStat   =  FAILBREAK;
      }

      *(HANDLER*)PrimParPointer()     =  (HANDLER)TmpHandle;
    }
    break;

    case CREATE16 :
    {
      Elements    =  *(DATA32*)PrimParPointer();
      ElementSize =  sizeof(DATA16);
      ISize       =  Elements * ElementSize + sizeof(DESCR);

      if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pTmp,&TmpHandle) == OK)
      {
        (*(DESCR*)pTmp).Type          =  DATA_16;
        (*(DESCR*)pTmp).ElementSize   =  (DATA8)ElementSize;
        (*(DESCR*)pTmp).Elements      =  Elements;

        DspStat   =  NOBREAK;
      }
      else
      {
        DspStat   =  FAILBREAK;
      }

      *(HANDLER*)PrimParPointer()     =  (HANDLER)TmpHandle;
    }
    break;

    case CREATE32 :
    {
      Elements    =  *(DATA32*)PrimParPointer();
      ElementSize =  sizeof(DATA32);
      ISize       =  Elements * ElementSize + sizeof(DESCR);

      if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pTmp,&TmpHandle) == OK)
      {
        (*(DESCR*)pTmp).Type          =  DATA_32;
        (*(DESCR*)pTmp).ElementSize   =  (DATA8)ElementSize;
        (*(DESCR*)pTmp).Elements      =  Elements;

        DspStat   =  NOBREAK;
      }
      else
      {
        DspStat   =  FAILBREAK;
      }

      *(HANDLER*)PrimParPointer()     =  (HANDLER)TmpHandle;
    }
    break;

    case CREATEF :
    {
      Elements    =  *(DATA32*)PrimParPointer();
      ElementSize =  sizeof(DATAF);
      ISize       =  Elements * ElementSize + sizeof(DESCR);

      if (cMemoryAlloc(TmpPrgId,POOL_TYPE_MEMORY,(GBINDEX)ISize,(void**)&pTmp,&TmpHandle) == OK)
      {
        (*(DESCR*)pTmp).Type          =  DATA_F;
        (*(DESCR*)pTmp).ElementSize   =  (DATA8)ElementSize;
        (*(DESCR*)pTmp).Elements      =  Elements;

        DspStat   =  NOBREAK;
      }
      else
      {
        DspStat   =  FAILBREAK;
      }

      *(HANDLER*)PrimParPointer()     =  (HANDLER)TmpHandle;
    }
    break;

    case SIZE :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      Elements    =  0;
      TmpPrgId    =  CurrentProgramId();

      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        Elements    =  (*(DESCR*)pTmp).Elements;
      }

      *(DATA32*)PrimParPointer()  =  Elements;
      DspStat     =  NOBREAK;
    }
    break;

    case RESIZE :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      Elements    =  *(DATA32*)PrimParPointer();

      if (cMemoryResize(TmpPrgId,TmpHandle,Elements) != NULL)
      {
        DspStat     =  NOBREAK;
      }
      else
      {
        DspStat     =  FAILBREAK;
      }
    }
    break;

    case DELETE :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      cMemoryFreeHandle(TmpPrgId,TmpHandle);
      DspStat     =  NOBREAK;
    }
    break;

    case FILL :
    {
      TmpPrgId        =  CurrentProgramId();
      TmpHandle       =  *(HANDLER*)PrimParPointer();

      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        pArray      =  (*(DESCR*)pTmp).pArray;
        Elements    =  (*(DESCR*)pTmp).Elements;
        Index       =  0;
        switch ((*(DESCR*)pTmp).Type)
        {
          case DATA_8 :
          {
            Data8                       =  *(DATA8*)PrimParPointer();
            pData8                      =  (DATA8*)pArray;

            while (Index < Elements)
            {
              pData8[Index]             =  Data8;
              Index++;
            }
            DspStat                     =  NOBREAK;
          }
          break;

          case DATA_16 :
          {
            Data16                      =  *(DATA16*)PrimParPointer();
            pData16                     =  (DATA16*)pArray;

            while (Index < Elements)
            {
              pData16[Index]            =  Data16;
              Index++;
            }
            DspStat                     =  NOBREAK;
          }
          break;

          case DATA_32 :
          {
            Data32                      =  *(DATA32*)PrimParPointer();
            pData32                     =  (DATA32*)pArray;

            while (Index < Elements)
            {
              pData32[Index]            =  Data32;
              Index++;
            }
            DspStat                     =  NOBREAK;
          }
          break;

          case DATA_F :
          {
            DataF                       =  *(DATAF*)PrimParPointer();
            pDataF                      =  (DATAF*)pArray;

            while (Index < Elements)
            {
              pDataF[Index]             =  DataF;
              Index++;
            }
            DspStat                     =  NOBREAK;
          }
          break;

        }
      }
    }
    break;

    case COPY :
    {
      hSource     =  *(HANDLER*)PrimParPointer();
      hDest       =  *(HANDLER*)PrimParPointer();

      DspStat     =  FAILBREAK;
      if (cMemoryGetPointer(TmpPrgId,hSource,&pSource) == OK)
      {
        if (cMemoryGetPointer(TmpPrgId,hDest,&pDest) == OK)
        {
          if ((*(DESCR*)pSource).Type == (*(DESCR*)pDest).Type)
          {
            Elements  =  (*(DESCR*)pSource).Elements;
            DspStat   =  NOBREAK;

            if (cMemoryResize(TmpPrgId,hDest,Elements) == NULL)
            {
              DspStat   =  FAILBREAK;
            }
            if (DspStat == NOBREAK)
            {
              if (cMemoryGetPointer(TmpPrgId,hDest,&pDest) == OK)
              {
                ISize   =  Elements * (*(DESCR*)pSource).ElementSize;
                memcpy((*(DESCR*)pDest).pArray,(*(DESCR*)pSource).pArray,ISize);
              }
              else
              {
                DspStat   =  FAILBREAK;
              }
            }
          }
        }
      }
    }
    break;

    case INIT8 :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      Index       =  *(DATA32*)PrimParPointer();
      Elements    =  *(DATA32*)PrimParPointer();

      DspStat     =  FAILBREAK;

      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        if ((Index >= 0) && ((Index + Elements) <= (*(DESCR*)pTmp).Elements))
        {
          pArray      =  (*(DESCR*)pTmp).pArray;
          pData8      =  (DATA8*)pArray;

          while (Elements)
          {
            pData8[Index]  =  *(DATA8*)PrimParPointer();
            Elements--;
            Index++;
          }
          DspStat     =  NOBREAK;
        }
      }
    }
    break;

    case INIT16 :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      Index       =  *(DATA32*)PrimParPointer();
      Elements    =  *(DATA32*)PrimParPointer();

      DspStat     =  FAILBREAK;

      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        if ((Index >= 0) && ((Index + Elements) <= (*(DESCR*)pTmp).Elements))
        {
          pArray      =  (*(DESCR*)pTmp).pArray;
          pData16     =  (DATA16*)pArray;

          while (Elements)
          {
            pData16[Index]  =  *(DATA16*)PrimParPointer();
            Elements--;
            Index++;
          }
          DspStat     =  NOBREAK;
        }
      }
    }
    break;

    case INIT32 :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      Index       =  *(DATA32*)PrimParPointer();
      Elements    =  *(DATA32*)PrimParPointer();

      DspStat     =  FAILBREAK;

      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        if ((Index >= 0) && ((Index + Elements) <= (*(DESCR*)pTmp).Elements))
        {
          pArray      =  (*(DESCR*)pTmp).pArray;
          pData32     =  (DATA32*)pArray;

          while (Elements)
          {
            pData32[Index]  =  *(DATA32*)PrimParPointer();
            Elements--;
            Index++;
          }
          DspStat     =  NOBREAK;
        }
      }
    }
    break;

    case INITF :
    {
      TmpHandle   =  *(HANDLER*)PrimParPointer();
      Index       =  *(DATA32*)PrimParPointer();
      Elements    =  *(DATA32*)PrimParPointer();

      DspStat     =  FAILBREAK;

      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        if ((Index >= 0) && ((Index + Elements) <= (*(DESCR*)pTmp).Elements))
        {
          pArray      =  (*(DESCR*)pTmp).pArray;
          pDataF      =  (DATAF*)pArray;

          while (Elements)
          {
            pDataF[Index]  =  *(DATAF*)PrimParPointer();
            Elements--;
            Index++;
          }
          DspStat     =  NOBREAK;
        }
      }
    }
    break;

    case READ_CONTENT :
    {
      PrgId           =  *(DATA16*)PrimParPointer();
      TmpHandle       =  *(DATA16*)PrimParPointer();
      Index           =  *(DATA32*)PrimParPointer();
      Bytes           =  *(DATA32*)PrimParPointer();
      pDData8         =  (DATA8*)PrimParPointer();

      DspStat         =  FAILBREAK;

      if (PrgId == (PRGID)CURRENT_SLOT)
      {
        PrgId         =  TmpPrgId;
      }

      if (cMemoryGetPointer(PrgId,TmpHandle,&pTmp) == OK)
      {
        if (VMInstance.Handle >= 0)
        {
          pDData8   =  (DATA8*)VmMemoryResize(VMInstance.Handle,Bytes);
        }

        ISize         =  (*(DESCR*)pTmp).Elements * (DATA32)((*(DESCR*)pTmp).ElementSize);

        if ((Index >= 0) && (pDData8 != NULL))
        {
          pArray      =  (*(DESCR*)pTmp).pArray;

          pData8      =  (DATA8*)pArray;
          Data32      =  0;

          while ((Data32 < Bytes) && (Index < ISize))
          {
            pDData8[Data32]           =  pData8[Index];
            Data32++;
            Index++;
          }
          while (Data32 < Bytes)
          {
            pDData8[Data32]           =  0;
            Data32++;
          }
          DspStat     =  NOBREAK;
        }
      }
    }
    break;

    case WRITE_CONTENT :
    {
      PrgId           =  *(DATA16*)PrimParPointer();
      TmpHandle       =  *(DATA16*)PrimParPointer();
      Index           =  *(DATA32*)PrimParPointer();
      Bytes           =  *(DATA32*)PrimParPointer();
      pDData8         =  (DATA8*)PrimParPointer();

      DspStat         =  FAILBREAK;

#ifdef DEBUG
      printf("ARRAY WRITE_CONTENT CP=%d PP=%d\r\n",TmpPrgId,PrgId);
#endif

      if (PrgId == (PRGID)CURRENT_SLOT)
      {
        PrgId         =  TmpPrgId;
      }

      if (cMemoryGetPointer(PrgId,TmpHandle,&pTmp) == OK)
      {
        ElementSize   =  (DATA32)(*(DESCR*)pTmp).ElementSize;
        if (ElementSize)
        {
          Elements      =  (Index + Bytes + (ElementSize - 1)) / ElementSize;
          ISize         =  Elements * ElementSize;

          pTmp          =  cMemoryResize(PrgId,TmpHandle,Elements);
          if (pTmp != NULL)
          {
            if ((Index >= 0) && (pDData8 != NULL))
            {
              pData8      =  (DATA8*)pTmp;
              Data32      =  0;

              while ((Data32 < Bytes) && (Index < ISize))
              {
                pData8[Index]   =  pDData8[Data32];
                Data32++;
                Index++;
              }
              DspStat     =  NOBREAK;
            }
          }
        }
      }
    }
    break;

    case READ_SIZE :
    {
      PrgId           =  *(DATA16*)PrimParPointer();
      TmpHandle       =  *(DATA16*)PrimParPointer();

      Bytes           =  0;
      DspStat         =  FAILBREAK;

      if (PrgId == (PRGID)CURRENT_SLOT)
      {
        PrgId         =  TmpPrgId;
      }

      if (cMemoryGetPointer(PrgId,TmpHandle,&pTmp) == OK)
      {
        Bytes         =  (*(DESCR*)pTmp).Elements * (DATA32)((*(DESCR*)pTmp).ElementSize);
      }

      *(DATA32*)PrimParPointer()  =  Bytes;
      DspStat                     =  NOBREAK;
    }
    break;

  }


  if (DspStat == BUSYBREAK)
  { // Rewind IP

    SetObjectIp(TmpIp - 1);
  }
  SetDispatchStatus(DspStat);
}


/*! \page cMemory
 *
 *  <hr size="1"/>
 *  <b>     opARRAY_WRITE (HANDLE, INDEX, VALUE)  </b>
 *
 *- Array element write\n
 *- Dispatch status can change to FAILBREAK
 *
 *  \param  (HANDLER) HANDLE    - Array handle
 *  \param  (DATA32)  INDEX     - Index to element to write
 *  \param  (type)    VALUE     - Value to write - type depends on type of array\n
 *
 *\n
 *
 */
/*! \brief  opARRAY_WRITE byte code
 *
 */
void      cMemoryArrayWrite(void)
{
  DSPSTAT DspStat = FAILBREAK;
  PRGID   TmpPrgId;
  HANDLER TmpHandle;
  void    *pTmp;
  void    *pValue;
  DESCR   *pDescr;
  DATA32  Elements;
  DATA32  Index;
  void    *pArray;
  DATA8   *pData8;
  DATA16  *pData16;
  DATA32  *pData32;
  DATAF   *pDataF;

  TmpPrgId        =  CurrentProgramId();
  TmpHandle       =  *(HANDLER*)PrimParPointer();
  Index           =  *(DATA32*)PrimParPointer();
  pValue          =  PrimParPointer();

  if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
  {
    pDescr        =  (DESCR*)pTmp;
    if (Index >= 0)
    {
      Elements  =  Index + 1;

      DspStat   =  NOBREAK;
      if (Elements > (*pDescr).Elements)
      {
        if (cMemoryResize(TmpPrgId,TmpHandle,Elements) == NULL)
        {
          DspStat   =  FAILBREAK;
        }
      }
      if (DspStat == NOBREAK)
      {
        if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
        {
          pDescr      =  (DESCR*)pTmp;
          pArray      =  (*pDescr).pArray;
#ifdef DEBUG
          printf("  Write  P=%1u H=%1u     I=%8lu A=%8p\r\n",(unsigned int)TmpPrgId,(unsigned int)TmpHandle,(unsigned long)Index,pArray);
#endif
          switch ((*pDescr).Type)
          {
            case DATA_8 :
            {
              pData8          =  (DATA8*)pArray;
              pData8[Index]   =  *(DATA8*)pValue;
              DspStat         =  NOBREAK;
            }
            break;

            case DATA_16 :
            {
              pData16         =  (DATA16*)pArray;
              pData16[Index]  =  *(DATA16*)pValue;
              DspStat         =  NOBREAK;
            }
            break;

            case DATA_32 :
            {
              pData32         =  (DATA32*)pArray;
              pData32[Index]  =  *(DATA32*)pValue;
              DspStat         =  NOBREAK;
            }
            break;

            case DATA_F :
            {
              pDataF          =  (DATAF*)pArray;
              pDataF[Index]   =  *(DATAF*)pValue;
              DspStat         =  NOBREAK;
            }
            break;

          }
        }
      }
    }
  }
  if (DspStat != NOBREAK)
  {
#ifdef DEBUG
    printf("  WR ERR P=%1u H=%1u     I=%8lu\r\n",(unsigned int)TmpPrgId,(unsigned int)TmpHandle,(unsigned long)Index);
#endif
    SetDispatchStatus(DspStat);
  }
}


/*! \page cMemory
 *
 *  <hr size="1"/>
 *  <b>     opARRAY_READ (HANDLE, INDEX, VALUE)  </b>
 *
 *- Array element write\n
 *- Dispatch status can change to FAILBREAK
 *
 *  \param  (HANDLER) HANDLE    - Array handle
 *  \param  (DATA32)  INDEX     - Index of element to read
 *  \return (type)    VALUE     - Value to read - type depends on type of array
 *
 *\n
 *
 */
/*! \brief  opARRAY_READ byte code
 *
 */
void      cMemoryArrayRead(void)
{
  DSPSTAT DspStat = FAILBREAK;
  PRGID   TmpPrgId;
  HANDLER TmpHandle;
  void    *pTmp;
  DATA32  Index;
  void    *pArray;
  DATA8   *pData8;
  DATA16  *pData16;
  DATA32  *pData32;
  DATAF   *pDataF;

  TmpPrgId        =  CurrentProgramId();
  TmpHandle       =  *(HANDLER*)PrimParPointer();
  Index           =  *(DATA32*)PrimParPointer();

  if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
  {
    if ((Index >= 0) && (Index < (*(DESCR*)pTmp).Elements))
    {
      pArray      =  (*(DESCR*)pTmp).pArray;
#ifdef DEBUG
      printf("  Read   P=%1u H=%1u     I=%8lu A=%8p\r\n",(unsigned int)TmpPrgId,(unsigned int)TmpHandle,(unsigned long)Index,pArray);
#endif
      switch ((*(DESCR*)pTmp).Type)
      {
        case DATA_8 :
        {
          pData8                      =  (DATA8*)pArray;
          *(DATA8*)PrimParPointer()   =  pData8[Index];
          DspStat                     =  NOBREAK;
        }
        break;

        case DATA_16 :
        {
          pData16                     =  (DATA16*)pArray;
          *(DATA16*)PrimParPointer()  =  pData16[Index];
          DspStat                     =  NOBREAK;
        }
        break;

        case DATA_32 :
        {
          pData32                     =  (DATA32*)pArray;
          *(DATA32*)PrimParPointer()  =  pData32[Index];
          DspStat                     =  NOBREAK;
        }
        break;

        case DATA_F :
        {
          pDataF                      =  (DATAF*)pArray;
          *(DATAF*)PrimParPointer()   =  pDataF[Index];
          DspStat                     =  NOBREAK;
        }
        break;

      }
    }
  }
  if (DspStat != NOBREAK)
  {
    PrimParAdvance();
    SetDispatchStatus(DspStat);
  }
}


/*! \page cMemory
 *
 *  <hr size="1"/>
 *  <b>     opARRAY_APPEND (HANDLE, VALUE)  </b>
 *
 *- Array element append\n
 *- Dispatch status can change to FAILBREAK
 *
 *  \param  (HANDLER) HANDLE    - Array handle
 *  \param  (type)    VALUE     - Value (new element) to append - type depends on type of array\n
 *
 *\n
 *
 */
/*! \brief  opARRAY_APPEND byte code
 *
 */
void      cMemoryArrayAppend(void)
{
  DSPSTAT DspStat = FAILBREAK;
  PRGID   TmpPrgId;
  HANDLER TmpHandle;
  void    *pTmp;
  void    *pValue;
  DESCR   *pDescr;
  DATA32  Elements;
  DATA32  Index;
  void    *pArray;
  DATA8   *pData8;
  DATA16  *pData16;
  DATA32  *pData32;
  DATAF   *pDataF;

  TmpPrgId        =  CurrentProgramId();
  TmpHandle       =  *(HANDLER*)PrimParPointer();
  pValue          =  PrimParPointer();

  if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
  {
    pDescr        =  (DESCR*)pTmp;
    Index         =  (*pDescr).Elements;
    Elements      =  Index + 1;

    DspStat       =  NOBREAK;
    if (Elements > (*pDescr).Elements)
    {
      if (cMemoryResize(TmpPrgId,TmpHandle,Elements) == NULL)
      {
        DspStat   =  FAILBREAK;
      }
    }
    if (DspStat == NOBREAK)
    {
      if (cMemoryGetPointer(TmpPrgId,TmpHandle,&pTmp) == OK)
      {
        pDescr      =  (DESCR*)pTmp;
        pArray      =  (*pDescr).pArray;
#ifdef DEBUG
        printf("  Append P=%1u H=%1u     I=%8lu A=%8p",(unsigned int)TmpPrgId,(unsigned int)TmpHandle,(unsigned long)Index,pArray);
#endif
        switch ((*pDescr).Type)
        {
          case DATA_8 :
          {
#ifdef DEBUG
            printf(" V=%d",(int)*(DATA8*)pValue);
#endif
            pData8          =  (DATA8*)pArray;
            pData8[Index]   =  *(DATA8*)pValue;
            DspStat         =  NOBREAK;
          }
          break;

          case DATA_16 :
          {
#ifdef DEBUG
            printf(" V=%d",(int)*(DATA16*)pValue);
#endif
            pData16         =  (DATA16*)pArray;
            pData16[Index]  =  *(DATA16*)pValue;
            DspStat         =  NOBREAK;
          }
          break;

          case DATA_32 :
          {
#ifdef DEBUG
            printf(" V=%d",(int)*(DATA32*)pValue);
#endif
            pData32         =  (DATA32*)pArray;
            pData32[Index]  =  *(DATA32*)pValue;
            DspStat         =  NOBREAK;
          }
          break;

          case DATA_F :
          {
#ifdef DEBUG
            printf(" V=%f",*(DATAF*)pValue);
#endif
            pDataF          =  (DATAF*)pArray;
            pDataF[Index]   =  *(DATAF*)pValue;
            DspStat         =  NOBREAK;
          }
          break;

        }
#ifdef DEBUG
        printf("\r\n");
#endif
      }
    }
  }
  if (DspStat != NOBREAK)
  {
#ifdef DEBUG
    printf("  WR ERR P=%1u H=%1u     I=%8lu\r\n",(unsigned int)TmpPrgId,(unsigned int)TmpHandle,(unsigned long)Index);
#endif
    SetDispatchStatus(DspStat);
  }
}


/*! \page cMemory
 *
 *  <hr size="1"/>
 *  <b>     opMEMORY_USAGE (TOTAL, FREE)  </b>
 *
 *- Get memory usage\n
 *- Dispatch status unchanged
 *
 *  \return (DATA32)  TOTAL     - Total memory [KB]
 *  \return (DATA32)  FREE      - Free  memory [KB]
 *
 *\n
 *
 */
/*! \brief  opMEMORY_USAGE byte code
 *
 */
void      cMemoryUsage(void)
{
  DATA32  Total;
  DATA32  Free;

  cMemoryGetUsage(&Total,&Free,0);

  if (Total < 0)
  {
    Total  =  0;
  }
  if (Free < 0)
  {
    Free  =  0;
  }

  *(DATA32*)PrimParPointer()  =  Total;
  *(DATA32*)PrimParPointer()  =  Free;

}


/*! \page cMemory
 *  <hr size="1"/>
 *  <b>     opFILENAME (CMD, ....)  </b>
 *
 *- Memory filename entry\n
 *- Dispatch status unchanged
 *
 *  \param  (DATA8)   CMD               - \ref memoryfilenamesubcode
 *
 *\n
 *  - CMD = EXIST
 *\n  Test if file exists (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \return (DATA8)    FLAG        - Exist (0 = no, 1 = yes)\n
 *
 *\n
 *  - CMD = TOTALSIZE
 *\n  Calculate folder/file size (if name starts with '~','/' or '.' it is not from current folder) \n
 *    -  \param  (DATA8)    NAME        - First character in file name (character string)\n
 *    -  \return (DATA32)   FILES       - Total number of files\n
 *    -  \return (DATA32)   SIZE        - Total folder size [KB]\n
 *
 *\n
 *  - CMD = SPLIT
 *\n  Split filename into Folder, name, extension \n
 *    -  \param  (DATA8)    FILENAME    - First character in file name (character string) "../folder/subfolder/name.ext"\n
 *    -  \param  (DATA8)    LENGTH      - Maximal length for each of the below parameters\n
 *    -  \return (DATA8)    FOLDER      - First character in folder name (character string) "../folder/subfolder"\n
 *    -  \return (DATA8)    NAME        - First character in name (character string) "name"\n
 *    -  \return (DATA8)    EXT         - First character in extension (character string) ".ext"\n
 *
 *\n
 *  - CMD = MERGE
 *\n  Merge Folder, name, extension into filename\n
 *    -  \param  (DATA8)    FOLDER      - First character in folder name (character string) "../folder/subfolder"\n
 *    -  \param  (DATA8)    NAME        - First character in name (character string) "name"\n
 *    -  \param  (DATA8)    EXT         - First character in extension (character string) ".ext"\n
 *    -  \param  (DATA8)    LENGTH      - Maximal length for the below parameter\n
 *    -  \return (DATA8)    FILENAME    - First character in file name (character string) "../folder/subfolder/name.ext"\n
 *
 *\n
 *  - CMD = CHECK
 *\n  Check filename\n
 *    -  \param  (DATA8)    FILENAME    - First character in file name (character string) "../folder/subfolder/name.ext"\n
 *    -  \return (DATA8)    OK          - Filename ok (0 = FAIL, 1 = OK)\n
 *
 *\n
 *  - CMD = PACK
 *\n  Pack file or folder into "raf" container\n
 *    -  \param  (DATA8)    FILENAME    - First character in file name (character string) "../folder/subfolder/name.ext"\n
 *
 *\n
 *  - CMD = UNPACK
 *\n  Unpack "raf" container\n
 *    -  \param  (DATA8)    FILENAME    - First character in file name (character string) "../folder/subfolder/name"\n
 *
 *\n
 *  - CMD = GET_FOLDERNAME
 *\n  Get current folder name\n
 *    -  \param  (DATA8)    LENGTH      - Maximal length for the below parameter\n
 *    -  \return (DATA8)    FOLDERNAME  - First character in folder name (character string) "../folder/subfolder"\n
 *
 *\n
 *
 */
/*! \brief  opFILENAME byte code
 *
 */
void      cMemoryFileName(void)
{
  PRGID   TmpPrgId;
  DATA8   Cmd;
  DATA8   Tmp;
  struct  stat FileStatus;
  char    Filename[MAX_FILENAME_SIZE];
  char    Folder[MAX_FILENAME_SIZE];
  char    Name[MAX_FILENAME_SIZE];
  char    Ext[MAX_FILENAME_SIZE];
  char    Buffer[2 * MAX_FILENAME_SIZE + 32];
  DATA8   Length;
  DATA8   *pFilename;
  DATA8   *pFolder;
  DATA8   *pName;
  DATA8   *pExt;
  HANDLER hFilename;
  HANDLER hFolder;
  HANDLER hName;
  HANDLER hExt;
  DATA32  Lng;
  DATA32  Size;
  DATA32  Files;

  TmpPrgId      =  CurrentProgramId();
  Cmd           =  *(DATA8*)PrimParPointer();

  switch (Cmd)
  { // Function

    case EXIST :
    {
      pFilename  =  (DATA8*)PrimParPointer();
      cMemoryFilename(TmpPrgId,(char*)pFilename,"",MAX_FILENAME_SIZE,Filename);

      Tmp        =  0;
      if (stat(Filename,&FileStatus) == 0)
      {
        Tmp      =  1;
      }
#ifdef DEBUG_TRACE_FILENAME
      printf("c_memory  cMemoryFileName: EXIST   [%s] = %d\r\n",Filename,Tmp);
#endif
      *(DATA8*)PrimParPointer()  =  Tmp;
    }
    break;

    case TOTALSIZE :
    {
      pFilename  =  (DATA8*)PrimParPointer();
      cMemoryFilename(TmpPrgId,(char*)pFilename,"",MAX_FILENAME_SIZE,Filename);
      Size  =  cMemoryFindSize((char*)Filename,&Files);

      *(DATA32*)PrimParPointer()  =  Files;
      *(DATA32*)PrimParPointer()  =  Size;
    }
    break;

    case SPLIT :
    {
      pFilename   =  (DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pFolder     =  (DATA8*)PrimParPointer();
      hFolder     =  VMInstance.Handle;
      pName       =  (DATA8*)PrimParPointer();
      hName       =  VMInstance.Handle;
      pExt        =  (DATA8*)PrimParPointer();
      hExt        =  VMInstance.Handle;

      Tmp         =  Length;

      // Split pFilename
      FindName((char*)pFilename,Folder,Name,Ext);

      // Make pFolder
      Length      =  Tmp;
      if (hFolder >= 0)
      {
        Lng       =  strlen(Folder) + 1;
        if (Lng > MIN_ARRAY_ELEMENTS)
        {
          pFolder    =  (DATA8*)VmMemoryResize(hFolder,Lng);
        }
        Length       =  (DATA8)Lng;
      }
      snprintf((char*)pFolder,(int)Length,"%s",Folder);

      // Make pName
      Length      =  Tmp;
      if (hName >= 0)
      {
        Lng       =  strlen(Name) + 1;
        if (Lng > MIN_ARRAY_ELEMENTS)
        {
          pName      =  (DATA8*)VmMemoryResize(hName,Lng);
        }
        Length       =  (DATA8)Lng;
      }
      snprintf((char*)pName,(int)Length,"%s",Name);

      // Make pExt
      Length      =  Tmp;
      if (hExt >= 0)
      {
        Lng       =  strlen(Ext) + 1;
        if (Lng > MIN_ARRAY_ELEMENTS)
        {
          pExt       =  (DATA8*)VmMemoryResize(hExt,Lng);
        }
        Length       =  (DATA8)Lng;
      }
      snprintf((char*)pExt,(int)Length,"%s",Ext);

    }
    break;

    case MERGE :
    {
      pFolder     =  (DATA8*)PrimParPointer();
      pName       =  (DATA8*)PrimParPointer();
      pExt        =  (DATA8*)PrimParPointer();
      Length      =  *(DATA8*)PrimParPointer();
      pFilename   =  (DATA8*)PrimParPointer();
      hFilename   =  VMInstance.Handle;

      // Merge pFolder, pName and pExt
      snprintf(Filename,MAX_FILENAME_SIZE,"%s/%s%s",pFolder,pName,pExt);

      // Make pFilename
      if (hFilename >= 0)
      {
        Lng       =  strlen(Filename) + 1;
        if (Lng > MIN_ARRAY_ELEMENTS)
        {
          pFilename  =  (DATA8*)VmMemoryResize(hFilename,Lng);
        }
        Length       =  (DATA8)Lng;
      }
      snprintf((char*)pFilename,(int)Length,"%s",Filename);
    }
    break;

    case CHECK :
    {
      Tmp         =  0;
      pFilename   =  (DATA8*)PrimParPointer();
      if (cMemoryCheckFilename((char*)pFilename,NULL,NULL,NULL) == OK)
      {
        Tmp       =  1;
      }

      *(DATA8*)PrimParPointer()  =  Tmp;
    }
    break;

    case PACK :
    {
      pName       =  (DATA8*)PrimParPointer();

      // Split pFilename
      FindName((char*)pName,Folder,Name,Ext);

      snprintf(Buffer,2 * MAX_FILENAME_SIZE + 32,"tar -cz -f %s%s%s -C %s %s%s &> /dev/null",Folder,Name,vmEXT_ARCHIVE,Folder,Name,Ext);
      system(Buffer);
      sync();
    }
    break;

    case UNPACK :
    {
      pName       =  (DATA8*)PrimParPointer();

      // Split pFilename
      FindName((char*)pName,Folder,Name,Ext);

      snprintf(Buffer,2 * MAX_FILENAME_SIZE + 32,"tar -xz -f %s%s%s -C %s &> /dev/null",Folder,Name,vmEXT_ARCHIVE,Folder);
      system(Buffer);
      sync();
    }
    break;

    case GET_FOLDERNAME :
    {
      Length      =  *(DATA8*)PrimParPointer();
      pFilename   =  (DATA8*)PrimParPointer();
      hFilename   =  VMInstance.Handle;

      cMemoryGetResourcePath(TmpPrgId,(char*)Filename,MAX_FILENAME_SIZE);
      Lng         =  strlen(Filename);


      if (Lng > 0)
      {
        if (Filename[Lng - 1] == '/')
        {
          Lng--;
          Filename[Lng]  =  0;
        }
      }

      // Make pFilename
      if (hFilename >= 0)
      {
        Lng++;
        if (Lng > MIN_ARRAY_ELEMENTS)
        {
          pFilename  =  (DATA8*)VmMemoryResize(hFilename,Lng);
        }
        Length       =  (DATA8)Lng;
      }
      if (pFilename != NULL)
      {
        snprintf((char*)pFilename,(int)Length,"%s",Filename);
      }
    }
    break;

  }
}


//*****************************************************************************
