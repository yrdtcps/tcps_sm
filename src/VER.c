#include <stdio.h>  
#include <stdlib.h>  
#include <string.h> 
#include "LIB_TCPS_OPENSSL.h"


static unsigned char LIB_VER[6]={0x20,0x19,0x01,0x09,0x10,0x03};

void TCPS_OPENSSL_LIB_VER(unsigned char* buf,int *len)
{
	memcpy(buf,LIB_VER,6);
	*len=6;
	return ;
}

