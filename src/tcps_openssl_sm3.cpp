#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sm3.h"
// #include "lib_tcps_openssl.h"
#include "LIB_TCPS_OPENSSL.h"


/**************************************************************************
* 函数名称： int TcpsSm3( const unsigned char *X, const unsigned char *Y,const unsigned char *datain,int inlen,unsigned char *dataout)
* 功能描述： sm3 摘要计算
* 访问的表： 
* 修改的表： 
* 输入参数： X	--- 公钥X
*			Y	--- 公钥Y
*			datain  数据
*			inlen	数据长度
* 输出参数： dataout	摘要存储地址
* 返 回 值： -1  失败  0成功
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
int TcpsSm3( const unsigned char *X, const unsigned char *Y,const unsigned char *datain,int inlen,unsigned char *dataout)
{
	//unsigned char input[78] = "\x12\x31\x06\x22\x80\x12\x30\x00\x00\x01\x04\x00\x11\x40\xA8\x8B\xCD\xF9\x81\x22\x60\x8F\x18\xB0\x0E\xB0\x3A\x41\x0C\xA1\xCD\x6D\x7E\x41\x24\x83\x2F\x4B\xC6\x63\x86\x1C\x45\xFE\x5D\x31\x90\xBE\xE3\x75\x9C\x25\xA2\x99\xEF\x39\x7C\x87\xF6\x9A\x42\x1C\xE0\xD9\x32\x5F\x36\xFC\x0F\x4F\xA0\x02\x7B\x30\x12\xF8\xAB\xA0";
	unsigned char head[18+1]="\x00\x80\x31\x32\x33\x34\x35\x36\x37\x38\x31\x32\x33\x34\x35\x36\x37\x38";

	unsigned char a[32+1] = "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC";
	unsigned char b[32+1] = "\x28\xE9\xFA\x9E\x9D\x9F\x5E\x34\x4D\x5A\x9E\x4B\xCF\x65\x09\xA7\xF3\x97\x89\xF5\x15\xAB\x8F\x92\xDD\xBC\xBD\x41\x4D\x94\x0E\x93";
	unsigned char x[32+1] = "\x32\xC4\xAE\x2C\x1F\x19\x81\x19\x5F\x99\x04\x46\x6A\x39\xC9\x94\x8F\xE3\x0B\xBF\xF2\x66\x0B\xE1\x71\x5A\x45\x89\x33\x4C\x74\xC7";
	unsigned char y[32+1] = "\xBC\x37\x36\xA2\xF4\xF6\x77\x9C\x59\xBD\xCE\xE3\x6B\x69\x21\x53\xD0\xA9\x87\x7C\xC6\x2A\x47\x40\x02\xDF\x32\xE5\x21\x39\xF0\xA0";
	//unsigned char px[32] = "\x16\x0E\x12\x89\x7D\xF4\xED\xB6\x1D\xD8\x12\xFE\xB9\x67\x48\xFB\xD3\xCC\xF4\xFF\xE2\x6A\xA6\xF6\xDB\x95\x40\xAF\x49\xC9\x42\x32";
	//unsigned char py[32] = "\x4A\x7D\xAD\x08\xBB\x9A\x45\x95\x31\x69\x4B\xEB\x20\xAA\x48\x9D\x66\x49\x97\x5E\x1B\xFC\xF8\xC4\x74\x1B\x78\xB4\xB2\x23\x00\x7F";

	unsigned char *data;
	int len;
	unsigned char output[32];
	int i;

	data = (unsigned char *)malloc(inlen + 300);
	if(data==NULL)
	{
		return -1;	
	}


	memset((char *)data,0,300);
	memcpy(data,head,18);
	memcpy(data+18,a,32);
	memcpy(data+50,b,32);
	memcpy(data+82,x,32);
	memcpy(data+114,y,32);
	memcpy(data+146,X,32);
	memcpy(data+178,Y,32);
	sm3(data, 210, output);

	memset(data,0,300);
	memcpy(data,output,32);
	memcpy(&data[32],datain,inlen);
	sm3(data, 32+inlen, output);

	memcpy(dataout,output,32);

	free(data);
	return 0;
}

/**************************************************************************
* 函数名称： int TcpsSm3XiaoMen(const unsigned char *datain,int inlen,unsigned char *dataout)
* 功能描述： sm3 摘要计算  厦门专用
* 访问的表： 
* 修改的表： 
* 输入参数： datain  数据
*			inlen	数据长度
* 输出参数： dataout	摘要存储地址
* 返 回 值： -1  失败  0成功
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
int TcpsSm3XiaoMen(const unsigned char *datain,int inlen,unsigned char *dataout)
{

	unsigned char output[32];

	sm3((unsigned char *)datain,inlen, output);
	memcpy(dataout,output,32);

	return 0;
}





/**************************************************************************
* 函数名称： int TcpsIDSm3( const unsigned char *X, const unsigned char *Y,const unsigned char *datain,int inlen,const unsigned char *usrid,int usridlen,unsigned char *dataout)
* 功能描述： sm3 摘要计算  ID 外部输入
* 访问的表： 
* 修改的表： 
* 输入参数： X	--- 公钥X
*			Y	--- 公钥Y
*			datain  数据
*			inlen	数据长度
*			usrid   id数据
*			usridlen	id数据长度
* 输出参数： dataout	摘要存储地址
* 返 回 值： -1  失败  0成功
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
int TcpsIDSm3( const unsigned char *X, const unsigned char *Y,const unsigned char *datain,int inlen,const unsigned char *usrid,int usridlen,unsigned char *dataout)
{
	//unsigned char input[78] = "\x12\x31\x06\x22\x80\x12\x30\x00\x00\x01\x04\x00\x11\x40\xA8\x8B\xCD\xF9\x81\x22\x60\x8F\x18\xB0\x0E\xB0\x3A\x41\x0C\xA1\xCD\x6D\x7E\x41\x24\x83\x2F\x4B\xC6\x63\x86\x1C\x45\xFE\x5D\x31\x90\xBE\xE3\x75\x9C\x25\xA2\x99\xEF\x39\x7C\x87\xF6\x9A\x42\x1C\xE0\xD9\x32\x5F\x36\xFC\x0F\x4F\xA0\x02\x7B\x30\x12\xF8\xAB\xA0";
	unsigned char a[32+1] = "\xFF\xFF\xFF\xFE\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFC";
	unsigned char b[32+1] = "\x28\xE9\xFA\x9E\x9D\x9F\x5E\x34\x4D\x5A\x9E\x4B\xCF\x65\x09\xA7\xF3\x97\x89\xF5\x15\xAB\x8F\x92\xDD\xBC\xBD\x41\x4D\x94\x0E\x93";
	unsigned char x[32+1] = "\x32\xC4\xAE\x2C\x1F\x19\x81\x19\x5F\x99\x04\x46\x6A\x39\xC9\x94\x8F\xE3\x0B\xBF\xF2\x66\x0B\xE1\x71\x5A\x45\x89\x33\x4C\x74\xC7";
	unsigned char y[32+1] = "\xBC\x37\x36\xA2\xF4\xF6\x77\x9C\x59\xBD\xCE\xE3\x6B\x69\x21\x53\xD0\xA9\x87\x7C\xC6\x2A\x47\x40\x02\xDF\x32\xE5\x21\x39\xF0\xA0";
	//unsigned char px[32] = "\x16\x0E\x12\x89\x7D\xF4\xED\xB6\x1D\xD8\x12\xFE\xB9\x67\x48\xFB\xD3\xCC\xF4\xFF\xE2\x6A\xA6\xF6\xDB\x95\x40\xAF\x49\xC9\x42\x32";
	//unsigned char py[32] = "\x4A\x7D\xAD\x08\xBB\x9A\x45\x95\x31\x69\x4B\xEB\x20\xAA\x48\x9D\x66\x49\x97\x5E\x1B\xFC\xF8\xC4\x74\x1B\x78\xB4\xB2\x23\x00\x7F";

	unsigned char *data;
	int datalen=0;
	int len=0;

	unsigned char output[32];
	int i;

	datalen=inlen + usridlen+300;
	data = (unsigned char *)malloc(datalen);
	if(data==NULL)
	{
		return -1;	
	}

	len=usridlen*8;

	memset(data,0,datalen);
	data[0]=len>>8;
	data[1]=len;

	memcpy((char *)&data[2],(char *)usrid,usridlen);
	i=usridlen+2;

	memcpy(data+i,a,32);
	i+=32;
	memcpy(data+i,b,32);
	i+=32;
	memcpy(data+i,x,32);
	i+=32;
	memcpy(data+i,y,32);
	i+=32;
	memcpy(data+i,X,32);
	i+=32;
	memcpy(data+i,Y,32);
	i+=32;
	sm3(data, i, output);

	memset(data,0,datalen);
	memcpy(data,output,32);
	memcpy(&data[32],datain,inlen);
	sm3(data, 32+inlen, output);

	memcpy(dataout,output,32);

	free(data);
	return 0;
}

