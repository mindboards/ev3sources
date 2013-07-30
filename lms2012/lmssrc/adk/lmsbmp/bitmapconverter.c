#include  <stdio.h>

#define   ULONG       unsigned int
#define   UBYTE       unsigned char

#include  "bitmap.bmp"

int       main(void)
{
  FILE    *pFile;
  ULONG   FileSize;
  ULONG   *pFileSize;
  UBYTE	  Header[2];
  char    BmpNameBuf[255];
  char    BmpNamePath[255];
  getcwd(BmpNamePath,250);

  snprintf(BmpNameBuf,255,"bitmap.rgf");
  pFile = fopen (BmpNameBuf,"wb");
  if (pFile != NULL)
  {
    FileSize   =  sizeof(bitmap_bits);
    Header[0]  = bitmap_width;
    Header[1]  =  bitmap_height;
    fwrite (Header,1,2,pFile);
    fwrite (bitmap_bits,1,FileSize,pFile);
    fclose (pFile);
  }
  else
  {
    printf("\r\nBitmap file not created !!!!!\r\n");
  }
  
  return (0);
}
