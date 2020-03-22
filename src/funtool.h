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

#define HexStr2Buf(HexStr, Buf) hex_string_to_bytes((char*)HexStr, strlen(HexStr), Buf, sizeof(Buf))
#define HexStr2Buf3(HexStr, LenHexStr, Buf) hex_string_to_bytes((char*)HexStr, LenHexStr, Buf, sizeof(Buf))
#define HexStr2Buf4(HexStr, LenHexStr, Buf, LenBuf) hex_string_to_bytes((char*)HexStr, LenHexStr, Buf, LenBuf)

#define Buf2HexStr2(Buf, HexStr) bytes2hex_string((unsigned char*)HexStr, Buf, sizeof(Buf))
#define Buf2HexStr(Buf, LenBuf, HexStr) bytes2hex_string((unsigned char*)HexStr, Buf, LenBuf)

// #ifdef __cplusplus
// }
// #endif

#endif

