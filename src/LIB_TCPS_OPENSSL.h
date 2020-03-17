#ifndef __LIB_TCPS_OPENSSL_H__
#define __LIB_TCPS_OPENSSL_H__

#ifdef __cplusplus
extern "C" {
#endif

//===========================================
//Function:	TCPS_OPENSSL_LIB_VER
//Description:
//Input:	no
//
//Output:	*ver		--- version buff
//			len			--- versionlength
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_OPENSSL_LIB_VER(unsigned char* ver,int *len);



////***********************////
////**********SHA**********////
////***********************////

//===========================================
//Function:	TCPS_SHA
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 20(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_SHA(unsigned char *datain,unsigned int inlen,unsigned char *md);

//===========================================
//Function:	TCPS_SHA1
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 20(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_SHA1(unsigned char *datain,unsigned int inlen,unsigned char *md); 
 
//===========================================
//Function:	TCPS_SHA224
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 28(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_SHA224(unsigned char *datain,unsigned int inlen,unsigned char *md);
  
//===========================================
//Function:	TCPS_SHA256
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 32(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_SHA256(unsigned char *datain,unsigned int inlen,unsigned char *md);
  
//===========================================
//Function:	TCPS_SHA384
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 48(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_SHA384(unsigned char *datain,unsigned int inlen,unsigned char *md);
  
//===========================================
//Function:	TCPS_SHA512
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 64(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_SHA512(unsigned char *datain,unsigned int inlen,unsigned char *md);

////***********************////
////**********MD***********////
////***********************////

//===========================================
//Function:	TCPS_MD4
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 16(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_MD4(unsigned char *datain,unsigned int inlen,unsigned char *md);  

//===========================================
//Function:	TCPS_MD5
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//Output:	*md			--- out buff (out length 16(BYTE))
//			
//Return:	
//Author:	mwl	
//Date:		2017-6-27
//===========================================
void TCPS_MD5(unsigned char *datain,unsigned int inlen,unsigned char *md);  

////***********************////
////**********RSA**********////
////***********************////

//===========================================
//Function:	TCPS_RSA_GENERATE_KEY
//Description:
//Input:	keylen		--- key length
//			e		 	--- index (3 or 65537 )
//			*privatefile		--- private key file absolute path
//			*publicfile		 	--- public key file absolute path
//Output:	private key save to *privatefile
//			public key save to *publicfile
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_GENERATE_KEY(unsigned long keylen,unsigned long e,const char *privatefile,const char *publicfile);

//===========================================
//Function:	TCPS_RSA_GET_PUBKEY_LEN
//Description:
//Input:	*pubkey_path		 	--- public key file absolute path
//Output:	no
//			
//Return:	public key length
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_GET_PUBKEY_LEN(const char *pubkey_path);

//===========================================
//Function:	TCPS_RSA_GET_PRIKEY_LEN
//Description:
//Input:	*pubkey_path		 	--- private key file absolute path
//Output:	no
//			
//Return:	private key length
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_GET_PRIKEY_LEN(const char *prikey_path);
 
//===========================================
//Function:	TCPS_RSA_encrypt
//Description:
//Input:	*datain		--- data buff
//			inlen		--- data length
//			*publicfile	--- public key file absolute path
//			mode		--- RSA_PKCS1_PADDING	1
//							RSA_SSLV23_PADDING	2
//							RSA_NO_PADDING		3   ()
//							RSA_PKCS1_OAEP_PADDING	4
//							RSA_X931_PADDING	5
//							RSA_PKCS1_PSS_PADDING	6
//Output:	*out			--- out buff (out length ==key length)
//			
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_encrypt(unsigned char *datain,unsigned int inlen, const char *pubkey_path,int mode,char *out);
 
//===========================================
//Function:	TCPS_RSA_decrypt
//Description:
//Input:	*datain		--- data buff( leng ==private key length)
//			*prikey_path	--- private key file absolute path
//			mode		--- RSA_PKCS1_PADDING	1
//							RSA_SSLV23_PADDING	2
//							RSA_NO_PADDING		3   ()
//							RSA_PKCS1_OAEP_PADDING	4
//							RSA_X931_PADDING	5
//							RSA_PKCS1_PSS_PADDING	6
//Output:	*out			--- out buff (out length )
//			
//Return:	-1: fail  >0 out length
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_decrypt(unsigned char *datain, const char *prikey_path,int mode,char *out);
 
//===========================================
//Function:	TCPS_RSA_verify
//Description:
//Input:	mode   	--- NID_md5				4
//						NID_sha				41
//						NID_sha1			64
//			*in		--- data buff
//			inlen	--- data length
//			 *key_path	--- key file absolute path
//			*in2		--- sign data buff
//			len	--- sign data length
//Output:	no
//			
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_verify(int mode,char *in,int inlen, const char *key_path, char* in2, int len);

//===========================================
//Function:	TCPS_RSA_sign
//Description:
//Input:	mode   	--- NID_md5				4
//						NID_sha				41
//						NID_sha1			64
//			*in		--- data buff
//			inlen	--- data length
//			 *key_path	--- key file absolute path
//			*out		--- sign data buff
//			*plen		--- sign data length
//Output:	no
//			
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_sign(int mode,char *in, int inlen,char *key_path, char* out, int* plen);

//===========================================
//Function:	TCPS_RSA_PEM_TO_BIGNUM
//Description:
//Input:	*filename	--- key file absolute path
//			
//Output:	*n			--- string buff of key module
//			*e 			--- index
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_PEM_TO_BIGNUM(const char *filename,char *n,unsigned long *e);  

//===========================================
//Function:	TCPS_RSA_BIGNUM_TO_PEM
//Description:
//Input:	*n			--- string buff of key module
//			e 			--- index (3 or 65537)
//			
//Output:	*filename	--- key file absolute path
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_RSA_BIGNUM_TO_PEM(char *n,unsigned long e,const char *filename); 


////***********************////
////**********DES**********////
////***********************////
//===========================================
//Function:	TCPS_DES_ECB_encrypt
//Description:
//Input:	*key			--- key buff   8 BYTE
//			*datain 		--- data buff  8 BYTE
//			mode 			---DES_ENCRYPT or  DES_DECRYPT
//Output:	*dataout 		--- out buff  8 BYTE
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_DES_ECB_encrypt(unsigned char *key, unsigned char *datain,unsigned char *dataout,char mode);

//===========================================
//Function:	TCPS_DES_CBC_encrypt
//Description:
//Input:	*key			--- key buff   8 BYTE
//			*initVC			--- initial vector   8 BYTE
//			*datain 		--- data buff  
//			*inlen 			--- data length
//			mode 			---DES_ENCRYPT or  DES_DECRYPT
//Output:	*dataout 		--- out buff  8 BYTE
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_DES_CBC_encrypt(unsigned char *key, unsigned char *initVC,unsigned char *datain,unsigned int inlen,unsigned char *dataout,char mode);


//===========================================
//Function:	TCPS_DES_ECB_encrypt
//Description:
//Input:	*key			--- key buff   16 BYTE
//			*datain 		--- data buff  16 BYTE
//			mode 			---DES_ENCRYPT or  DES_DECRYPT
//Output:	*dataout 		--- out buff  8 BYTE
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_3DES_ECB_encrypt(unsigned char *key, unsigned char *datain,unsigned char *dataout,char mode);



//===========================================
//Function:	TCPS_DES_CBC_encrypt
//Description:
//Input:	*key			--- key buff   16 BYTE
//			*initVC			--- initial vector   8 BYTE
//			*datain 		--- data buff  
//			*inlen 			--- data length
//			mode 			---DES_ENCRYPT or  DES_DECRYPT
//Output:	*dataout 		--- out buff  8 BYTE
//Return:	0:success  -1:fail
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_3DES_CBC_encrypt(unsigned char *key, unsigned char *initVC,unsigned char *datain,unsigned int inlen,unsigned char *dataout,char mode);


////***********************////
////*********ECDSA*********////
////***********************////


//===========================================
//Function:	lib_ecdsa_verify
//Description:
//Input:	length: ec秘钥长度
//			public_key_hex_str: hex格式公钥字符串
//			data: 被签名数据 
//			data_len: 被签名数据长度
//			sig: 签名数据
//			sig_len: 签名数据长度
//Output:	
//
//Return:	 1 成功， 非1 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int lib_ecdsa_verify(int length,const char* public_key_hex_str, const unsigned char* data, int data_len, const unsigned char* sig, int sig_len);


//===========================================
//Function:	lib_ecdsa_sign
//Description:
//Input:	length: ec秘钥长度
//			private_key_hex_str: hex格式私钥字符串
//			data: 被签名数据 
//			data_len: 被签名数据长度
//			
//Output:	result: 存储签名结果数据, 请确保空间足够
//			result_len: 签名结果数据长度, 输入为result长度，输出为签名结果长度
//Return:	 1 成功， 非1 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int lib_ecdsa_sign(int length,const char* private_key_hex_str, const unsigned char* data, int data_len,unsigned char* result,int* result_len);



//===========================================
//Function:	lib_ecdsa_sign
//Description:
//Input:	length: ec秘钥长度
//Output:	public_key: 存储生成的公钥数据, 请确保空间足够
//			public_key_size: 公钥数据长度, 输入为public_key长度，输出为公钥数据长度
//			private_key: 存储生成的私钥数据, 请确保空间足够
//			private_key_size: 私钥数据长度, 输入为private_key长度，输出为私钥数据长度
//Return:	 1 成功， 非1 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int generate_key(int length,unsigned char* public_key,int* public_key_size,unsigned char* private_key, int* private_key_size);



////***********************////
////**********SM2**********////
////***********************////

//===========================================
//Function:	TCPS_SM2_verify
//Description:
//Input:	*prikey	--- private key  (string)
//			*digest		--- digest  (32 BYTE )  by SM3
//			digestlen	--- digest length
//			*Sign		--- Sign  (70---72 BYTE     30440220......0220......)
//			*Signlen	--- Sign length
//Output:	no
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_SM2_signature(char *prikey,unsigned char *digest,int digestlen,unsigned char *Sign,int *Signlen);



//===========================================
//Function:	TCPS_SM2_verify
//Description:
//Input:	*pubkeyX	--- public key x (string)
//			*pubkeyY	--- public key y (string)
//			*digest		--- digest  (32 BYTE )  by SM3
//			digestlen	--- digest length
//			*Sign		--- Sign  (70---72 BYTE     30440220......0220......)
//			Signlen	--- Sign length
//Output:	no
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_SM2_verify(char *pubkeyX,char *pubkeyY,unsigned char *digest,int digestlen,unsigned char *Sign,int Signlen);

int TCPS_SM2_verify2(char *pubkey,unsigned char *digest,int digestlen,unsigned char *Sign,int Signlen);

//===========================================
//Function:	TCPS_SM2_GetY
//Description:
//Input:	*pubkey	--- 压缩公钥
//			*Y		--- 公钥y
//			len		--- 存放公钥y的buf长度
//Output:	no
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2018-4-27
//===========================================
int TCPS_SM2_GetY(char *pubkey,char *Y,int len);

////***********************//// 
////**********SM3**********////
////***********************////

//===========================================
//Function:	TCPS_SM3
//Description:
//Input:	*X		--- public key x (32BYTE)
//			*Y		--- public key y (32BYTE)
//			*datain		--- data buff
//			inlen		--- data length
//
//Output:	*dataout	--- digest out  (32BYTE)
//
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_SM3( unsigned char *X, unsigned char *Y,unsigned char *datain,int inlen,unsigned char *dataout);

int TCPS_SM3_XiaMen(unsigned char *datain,int inlen,unsigned char *dataout);

int TCPS_SM3_2( unsigned char *X, unsigned char *Y,unsigned char *datain,int inlen,unsigned char *usrid,int usridlen,unsigned char *dataout);
////***********************////
////**********SM4**********////
////***********************////

//===========================================
//Function:	TCPS_SM4_encrypt
//Description:
//Input:	*key		---  key (16BYTE)
//			*datain		--- data buff
//			inlen		--- data length
//
//Output:	*dataout	--- digest out  (outlen ==inlen)
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_SM4_encrypt(unsigned char *key,unsigned char *datain,int inlen,unsigned char *output);

//===========================================
//Function:	TCPS_SM4_decrypt
//Description:
//Input:	*key		---  key (16BYTE)
//			*datain		--- data buff
//			inlen		--- data length
//
//Output:	*dataout	--- digest out  (outlen ==inlen)
//Return:	 0 成功， 非0 失败
//Author:	mwl	
//Date:		2017-6-27
//===========================================
int TCPS_SM4_decrypt(unsigned char *key,unsigned char *datain,int inlen,unsigned char *output);


#ifdef __cplusplus
}
#endif
#endif   

 
