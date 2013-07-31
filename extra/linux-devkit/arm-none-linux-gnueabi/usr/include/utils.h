#include <stdlib.h>
#include <endian.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

extern long buffering_write(int outf, char *buffer, long num);
extern int buffering_close(int fd);

/* I wonder how many alignment issues this is gonna trip in the
   future...  it shouldn't trip any...  I guess we'll find out :) */

static inline int bigendianp(void){
  int test=1;
  char *hack=(char *)(&test);
  if(hack[0])return(0);
  return(1);
}

static inline int32_t swap32(int32_t x){
  return((((u_int32_t)x & 0x000000ffU) << 24) | 
	 (((u_int32_t)x & 0x0000ff00U) <<  8) | 
	 (((u_int32_t)x & 0x00ff0000U) >>  8) | 
	 (((u_int32_t)x & 0xff000000U) >> 24));
}

static inline int16_t swap16(int16_t x){
  return((((u_int16_t)x & 0x00ffU) <<  8) | 
	 (((u_int16_t)x & 0xff00U) >>  8));
}

#if BYTE_ORDER == LITTLE_ENDIAN

static inline int32_t be32_to_cpu(int32_t x){
  return(swap32(x));
}

static inline int16_t be16_to_cpu(int16_t x){
  return(swap16(x));
}

static inline int32_t le32_to_cpu(int32_t x){
  return(x);
}

static inline int16_t le16_to_cpu(int16_t x){
  return(x);
}

#else

static inline int32_t be32_to_cpu(int32_t x){
  return(x);
}

static inline int16_t be16_to_cpu(int16_t x){
  return(x);
}

static inline int32_t le32_to_cpu(int32_t x){
  return(swap32(x));
}

static inline int16_t le16_to_cpu(int16_t x){
  return(swap16(x));
}


#endif

static inline int32_t cpu_to_be32(int32_t x){
  return(be32_to_cpu(x));
}

static inline int32_t cpu_to_le32(int32_t x){
  return(le32_to_cpu(x));
}

static inline int16_t cpu_to_be16(int16_t x){
  return(be16_to_cpu(x));
}

static inline int16_t cpu_to_le16(int16_t x){
  return(le16_to_cpu(x));
}

static inline char *copystring(const char *s){
  if(s){
    char *ret=malloc((strlen(s)+1)*sizeof(char));
    strcpy(ret,s);
    return(ret);
  }
  return(NULL);
}

static inline char *catstring(char *buff,const char *s){
  if(s){
    if(buff)
      buff=realloc(buff,strlen(buff)+strlen(s)+1);
    else
      buff=calloc(strlen(s)+1,1);
    strcat(buff,s);
  }
  return(buff);
}

