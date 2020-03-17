/************************************************************************
* 版权所有 (C)2019, 天津通卡智能网络科技股份有限公司。
* 
* 文件名称： // tcps_openssl_sha.cpp
* 文件标识： // 
* 内容摘要： // 哈希算法实现
* 其它说明： // 
* 当前版本： // 
* 作    者： // mwl
* 完成日期： // 2019/05/24
* 
* 修改记录1：// 
*    修改日期：
*    版 本 号：
*    修 改 人：
*    修改内容： 
* 修改记录2：…
************************************************************************/
#include <openssl/sha.h>   
#include <openssl/crypto.h>  // OPENSSL_cleanse  
#include <string.h>  
// #include "lib_tcps_openssl.h"
#include "LIB_TCPS_OPENSSL.h"


/**************************************************************************
* 函数名称： void sha1(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)   
* 功能描述： sha1计算
* 访问的表： 
* 修改的表： 
* 输入参数： pucData         数据指针
*           iDataLen        数据长度
* 输出参数： pucSha         哈希值
* 返 回 值： 无
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
void sha1(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)  
{  
    SHA_CTX c;  
    SHA1_Init(&c);  
    SHA1_Update(&c, pucData, iDataLen);  
    SHA1_Final(pucSha, &c);  
    OPENSSL_cleanse(&c, sizeof(c));   
}  
  
/**************************************************************************
* 函数名称： void sha224(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)   
* 功能描述： sha224计算
* 访问的表： 
* 修改的表： 
* 输入参数： pucData         数据指针
*           iDataLen        数据长度
* 输出参数： pucSha         哈希值
* 返 回 值： 无
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
void sha224(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)   
{  
    SHA256_CTX c;  

    SHA224_Init(&c);  
    SHA224_Update(&c, pucData,iDataLen);  
    SHA224_Final(pucSha, &c);  
    OPENSSL_cleanse(&c, sizeof(c));  
}  
  
/**************************************************************************
* 函数名称： void sha256(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)   
* 功能描述： sha256计算
* 访问的表： 
* 修改的表： 
* 输入参数： pucData         数据指针
*           iDataLen        数据长度
* 输出参数： pucSha         哈希值
* 返 回 值： 无
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
void sha256(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)  
{  
    SHA256_CTX c;  

    SHA256_Init(&c);  
    SHA256_Update(&c, pucData,iDataLen);  
    SHA256_Final(pucSha, &c);  
    OPENSSL_cleanse(&c, sizeof(c));  
 
}  
  
/**************************************************************************
* 函数名称： void sha384(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)   
* 功能描述： sha384计算
* 访问的表： 
* 修改的表： 
* 输入参数： pucData         数据指针
*           iDataLen        数据长度
* 输出参数： pucSha         哈希值
* 返 回 值： 无
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
void sha384(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)  
{  
    SHA512_CTX c;  

    SHA384_Init(&c);  
    SHA384_Update(&c, pucData, iDataLen);  
    SHA384_Final(pucSha, &c);  
    OPENSSL_cleanse(&c, sizeof(c));  

}  
  
/**************************************************************************
* 函数名称： void sha512(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)   
* 功能描述： sha512计算
* 访问的表： 
* 修改的表： 
* 输入参数： pucData         数据指针
*           iDataLen        数据长度
* 输出参数： pucSha         哈希值
* 返 回 值： 无
* 其它说明： 
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl	      
**************************************************************************/
void sha512(const unsigned char *pucData,unsigned int iDataLen,unsigned char *pucSha)  
{  
    SHA512_CTX c;  

    SHA512_Init(&c);  
    SHA512_Update(&c, pucData, iDataLen);  
    SHA512_Final(pucSha, &c);  
    OPENSSL_cleanse(&c, sizeof(c));  

}  
   
