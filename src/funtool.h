#ifndef __FUNTOOL_H__
#define __FUNTOOL_H__

#include "typedef.h"

// #ifdef __cplusplus
// extern "C" {
// #endif

void hexstr2buf(const char* hexstr, unsigned char* buf, int* len);
char* buf2hexstr(const unsigned char* buf, int len, char* cache);

int hex_string_to_bytes(char* hex_string, int hex_string_len, unsigned char* bytes, int bytes_len);
int bytes2hex_string(unsigned char* print_buf,unsigned char* bytes, int len);

SDWORD Base64Encode(const BYTE *pucInput,DWORD dwInputLen,BYTE *pucOutput);
SDWORD Base64Decode(const BYTE *pucInput,BYTE *pucOutput);
void SetSignData(const BYTE *pucSign,DWORD dwSignLen,BYTE *pucOutSign,DWORD *pwdOutLsen);

// #ifdef __cplusplus
// }
// #endif

#endif

