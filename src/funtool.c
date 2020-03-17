#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "typedef.h"

//========= base64 datas begin===========
static const char *codes =   
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  
  
static const BYTE map[256] = {  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 253, 255,  
255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 253, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,  
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,  
255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,  
  7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  
 19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,  
255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  
 37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  
 49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  
255, 255, 255, 255 };  
//==========base64 datas end============

unsigned char chr2hex(char ch)
{
    unsigned char hex;
    if(ch >= '0' && ch <= '9')
    {
        hex = ch - '0';
    }
    else if(ch >= 'A' && ch <= 'F')
    {
        hex = ch - 'A' + 0x0A;
    }
    else if(ch >= 'a' && ch <= 'f')
    {
        hex = ch - 'a' + 0x0A;
    }
    else
    {
        hex = 0xFF;
    }
    return hex;
}

void hexstr2buf(const char* hexstr, unsigned char* buf, int* len)
{
    int slen = strlen(hexstr);
    int i;
    for(i=0; i<slen/2; i++)
    {
        buf[i] = chr2hex(hexstr[i*2]) << 4;
        buf[i] |= chr2hex(hexstr[i*2+1]);
    }
    *len = slen/2;
}

char* buf2hexstr(const unsigned char* buf, int len, char* cache)
{
    // printf("len = %d \n", len);
    int i;
    for(i=0; i<len; i++)
    {
        sprintf(cache+(i*2), "%02X", buf[i]);
// printf("buf[%d] = %02X \n", i, buf[i]);
// printf("cache = %s \n", cache);
    }
// printf("end==========\n");
    return cache;
}


/*************************************************
*  Function:       // hex_of_char
*  Description:    // 字符转 16进制
*  Input:          // c	：字符
*                  //
*  Output:         // 无
*  Return:         // 16禁止数
*  Others:         // 其它说明
*************************************************/
unsigned char hex_of_char(char c)
{
	unsigned char tmp = 0;
	if(c >= '0' && c <= '9') {
		tmp = (c - '0');
	}
	else if(c >= 'A' && c <= 'F') {
		tmp = (c - 'A' + 10);
	}
	else if(c >= 'a' && c <= 'f') {
		tmp = (c - 'a' + 10);
	}
	return tmp;
}

/*************************************************
*  Function:       // hex_string_to_bytes
*  Description:    // 字符串转 16进制数组
*  Input:          // hex_string	：字符串
*                  // hex_string_len	:字符串长度
*                  //bytes_len	:计划得到的数组长度
*  Output:         // bytes	：输出数组
*                  //
*  Return:         // －1：失败   1：成功
*  Others:         // 其它说明
*************************************************/
int hex_string_to_bytes(char* hex_string, int hex_string_len, unsigned char* bytes, int bytes_len)
{

	int i = 0;

	/**
	* 校验十六进制字符串长度必须偶数，并且目标存储空间必须足够存放转换后的二进制数据
	*/
	if((hex_string_len % 2 != 0) || (bytes_len * 2 < hex_string_len)) {
		return -1;
	}

	for(i = 0; i < hex_string_len; i += 2) {
		bytes[i/2] = ((hex_of_char(hex_string[i]) << 4) & 0xF0) |
					(hex_of_char(hex_string[i + 1]) & 0x0F);
	}
	return 1;
}


/*************************************************
*  Function:       // bytes2hex_string
*  Description:    // 16进制数组转 字符串
*  Input:          // bytes	：数组指针
*                  // len	:数组长度
*                  //
*  Output:         // print_buf	：输出字符串
*                  //
*  Return:         // －1：失败    >=0：字符串长度
*  Others:         // 其它说明
*************************************************/
int bytes2hex_string(unsigned char* print_buf,unsigned char* bytes, int len)
{
	const unsigned char hex_map_table[] = "0123456789ABCDEF";
	int i = 0;

	/**
	* 入参校验
	*/
	if(print_buf == NULL || bytes == NULL ) {
		return -1;
	}

	for(i = 0; i < len; i++) {
		print_buf[i * 2] = hex_map_table[(bytes[i] >> 4) & 0x0F];
		print_buf[i * 2 + 1] = hex_map_table[(bytes[i]) & 0x0F];
	}
	/**
	* 填充字符串结束符
	*/
	print_buf[i * 2] = '\0';
	/**
	* 返回目标地址
	*/
	return (i*2);
}
/**************************************************************************
* 函数名称： SDWORD Base64Encode(const BYTE *pucInput,DWORD dwInputLen,BYTE *pucOutput) 
* 功能描述： base64编码
* 访问的表： 
* 修改的表： 
* 输入参数： pucInput		输入数据指针
*			dwInputLen		输入数据长度
* 输出参数： pucOutput		输出数据指针
* 返 回 值： 输出长度
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
SDWORD Base64Encode(const BYTE *pucInput,DWORD dwInputLen,BYTE *pucOutput)  
{  
   DWORD i, leven;  
   BYTE *pucData;  
   /* valid output size ? */  
   //len2 = 4 * ((dwInputLen + 2) / 3);  
   pucData = pucOutput;  
   leven = 3*(dwInputLen / 3);  
   for (i = 0; i < leven; i += 3) {  
       *pucData++ = codes[pucInput[0] >> 2];  
       *pucData++ = codes[((pucInput[0] & 3) << 4) + (pucInput[1] >> 4)];  
       *pucData++ = codes[((pucInput[1] & 0xf) << 2) + (pucInput[2] >> 6)];  
       *pucData++ = codes[pucInput[2] & 0x3f];  
       pucInput += 3;  
   }  
   /* Pad it if necessary...  */  
   if (i < dwInputLen) {  
       unsigned a = pucInput[0];  
       unsigned b = (i+1 < dwInputLen) ? pucInput[1] : 0;  
       unsigned c = 0;  
  
       *pucData++ = codes[a >> 2];  
       *pucData++ = codes[((a & 3) << 4) + (b >> 4)];  
       *pucData++ = (i+1 < dwInputLen) ? codes[((b & 0xf) << 2) + (c >> 6)] : '=';  
       *pucData++ = '=';  
   }  
  
   /* append a NULL byte */  
   *pucData = '\0';  
  
   return pucData - pucOutput;  
}  
  
/**************************************************************************
* 函数名称： SDWORD Base64Decode(const BYTE *pucInput,BYTE *pucOutput) 
* 功能描述： base64解码
* 访问的表： 
* 修改的表： 
* 输入参数： pucInput		输入数据指针
*			
* 输出参数： pucOutput		输出数据指针
* 返 回 值： 输出长度
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
SDWORD Base64Decode(const BYTE *pucInput,BYTE *pucOutput) 
{  
    unsigned long t, x, y, z;  
    unsigned char c;  
    int g = 3;  
  
    for (x = y = z = t = 0; pucInput[x]!=0;) {  
        c = map[pucInput[x++]];  
        if (c == 255) return -1;  
        if (c == 253) continue;  
        if (c == 254) { c = 0; g--; }  
        t = (t<<6)|c;  
        if (++y == 4) {  
//          if (z + g > *outlen) { return CRYPT_BUFFER_OVERFLOW; }  
            pucOutput[z++] = (unsigned char)((t>>16)&255);  
            if (g > 1) pucOutput[z++] = (unsigned char)((t>>8)&255);  
            if (g > 2) pucOutput[z++] = (unsigned char)(t&255);  
            y = t = 0;  
        }  
    }  
//  if (y != 0) {  
//      return -1;  
//  }  
    return z;  
}  





/**************************************************************************
* 函数名称： void SetSignData(const BYTE *pucSign,DWORD dwSignLen,BYTE *pucOutSign,DWORD *pwdOutLsen)
* 功能描述： 签名数据组包
* 访问的表： 
* 修改的表： 
* 输入参数： pucSign		:签名原始数据  签名r＋签名s
*           dwSignLen		:签名原始数据长度
*			
* 输出参数： pucOutSign		:签名拼接数据
*           pwdOutLsen		:签名拼接数据长度
* 返 回 值： 
* 其它说明： 输出数据格式 0x30+后边数据长度＋0x02+签名r长度＋签名r＋0x02+签名s长度＋签名s
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
void SetSignData(const BYTE *pucSign,DWORD dwSignLen,BYTE *pucOutSign,DWORD *pwdOutLsen)
{
	DWORD i=0;

	i=0;
	memcpy(&pucOutSign[0],"\x30\x44",2);
	i+=2;
	if(pucSign[0]&0x80)
	{
			pucOutSign[i++]=0x02;
			pucOutSign[i++]=dwSignLen/2+1;
			pucOutSign[i++]=0x00;
			memcpy(&pucOutSign[i],pucSign,dwSignLen/2);
			i+=(dwSignLen/2);
			
	}
	else
	{
			pucOutSign[i++]=0x02;
			pucOutSign[i++]=dwSignLen/2;
			memcpy(&pucOutSign[i],pucSign,dwSignLen/2);
			i+=(dwSignLen/2);
	}
		
	if(pucSign[dwSignLen/2]&0x80)
	{
			pucOutSign[i++]=0x02;
			pucOutSign[i++]=dwSignLen/2+1;
			pucOutSign[i++]=0x00;
			memcpy(&pucOutSign[i],&pucSign[dwSignLen/2],dwSignLen/2);
			i+=(dwSignLen/2);
			
	}
	else
	{
			pucOutSign[i++]=0x02;
			pucOutSign[i++]=dwSignLen/2;
			memcpy(&pucOutSign[i],&pucSign[dwSignLen/2],dwSignLen/2);
			i+=(dwSignLen/2);
	}
	pucOutSign[1]=i-2;
	*pwdOutLsen=i;
}

#if 0
void main(int argc, char* argv[])
{
    unsigned char buf[100] = "\x12\x34\x56\x78\x90\xab\xcd\xef";
    char ss[200] = {0};
    int len;

    printf("buf = %s\n", buf2hexstr(buf, 90, ss));
    hexstr2buf("aabbccddeeff11223344", buf, &len);
    printf("buf = %s\n", buf2hexstr(buf, len, ss));
}
#endif