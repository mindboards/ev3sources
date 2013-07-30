#include  <stdio.h>
#include  <string.h>

#define   ULONG       unsigned int
#define   UBYTE       unsigned char

int       main(int argc,char *argv[])
{
  FILE    *pFileIn;
  FILE    *pFileOut;
  char    Buffer[255];
  int     State;
  int     Datas;
  int     Data;
  int     Bytes;
  ULONG   Result;
  float   *pData;
  UBYTE   Byte;


  if (argc > 2)
  {
    pFileIn = fopen(argv[1],"rb");
    if (pFileIn != NULL)
    {
      pFileOut = fopen(argv[2],"wb");
      if (pFileOut != NULL)
      {
        State  =  0;
        while (fread(&Byte,1,1,pFileIn) == 1)
        {
          switch (State)
          {
            case 0 :
            {
              fwrite(&Byte,1,1,pFileOut);
              if (Byte == '\n')
              {
                Datas  =  0;
                State++;
              }
            }
            break;
            
            case 1 :
            {
              fwrite(&Byte,1,1,pFileOut);
              if (Byte == '\t')
              {
                Datas++;
              }
              if (Byte == '\n')
              {
                Datas++;
                Data   =  0;
                Bytes  =  0;
                State++;
              }
            }
            break;
            
            default :
            {
              if (Bytes == 0)
              {
                Result  =  (ULONG)Byte;
              }
              else
              {
                Result |=  (ULONG)Byte << (8 * Bytes);
              }
              if (++Bytes >= 4)
              {
                if (Result != 0xFFFFFFFF)
                {
                  pData  =  (float*)&Result;
                  if (Data == 0)
                  {
                    sprintf(Buffer,"%08.0f",*pData);
                  }
                  else
                  {
                    sprintf(Buffer,"%.1f",*pData);
                  }
                  fwrite(Buffer,strlen(Buffer),1,pFileOut);
                  if (++Data >= Datas)
                  {
                    fwrite("\r\n",2,1,pFileOut);
                    Data  =  0;
                  }
                  else
                  {
                    fwrite("\t",1,1,pFileOut);
                  }
                }
                else
                {
                  if (++Data >= 2)
                  {
                    fwrite("******************************************************************************************\r\n",92,1,pFileOut);
                    State  =  0;
                  }
                }
                Bytes  =  0;
              }
            }
            break;

          }
        }
        fclose(pFileOut);
      } 
      fclose(pFileIn);
    }
  }
  else
  {
    printf("\r\nUsage cnvlog filein fileout\r\n\n");
  }
  return (0);
}
