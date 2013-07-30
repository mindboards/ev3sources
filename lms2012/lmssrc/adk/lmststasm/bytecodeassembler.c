#include  <stdio.h>
#include  "lmstypes.h"
#include  "bytecodes.h"

#include  "prg.c"

int       main(void)
{
  FILE    *pFile;
  ULONG   FileSize;
  ULONG   *pFileSize;
  char    PrgNameBuf[255];
  char    PrgNamePath[255];
  getcwd(PrgNamePath,250);

  snprintf(PrgNameBuf,255,"prg.rbf");
  pFile = fopen (PrgNameBuf,"wb");
  if (pFile != NULL)
  {
    FileSize    =  sizeof(prg);
#ifndef OLD_HEADER
    pFileSize   =  (ULONG*)&prg[4];
    *pFileSize  =  FileSize;
#endif
    fwrite (prg,1,FileSize,pFile);
    fclose (pFile);
    printf("%d bytes\r\n",FileSize);
  }
  else
  {
    printf("\r\nImage file not created !!!!!\r\n");
  }
  
  return (0);
}
