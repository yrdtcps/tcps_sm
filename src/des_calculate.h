 #ifndef __DES_H
 #define __DES_H

#ifdef __cplusplus
extern "C" {
#endif
/*	Flags controlling SingleDesOperation()  */
	#define DO_INITIAL_DES_PERMUTATION		0x10
	#define DES_ENCRYPTION_MODE				0x20
	#define DES_DECRYPTION_MODE				0
	#define DO_FINAL_DES_PERMUTATION		0x40
	
#define DES_ENCRYPT_BLOCK (DO_INITIAL_DES_PERMUTATION|DES_ENCRYPTION_MODE|DO_FINAL_DES_PERMUTATION )
#define DES_DECRYPT_BLOCK (DO_INITIAL_DES_PERMUTATION|DES_DECRYPTION_MODE|DO_FINAL_DES_PERMUTATION )
	extern unsigned char deskey[8];


// ____________________________________________________________________________
//
//  FUNCTION: SingleDesOperation
//        IN: *plaintext: 8 bytes data to be encrypt/decrypt
//                     plaintext[0] .. MSByte
//                     plaintext[7] .. LSByte
//            *key: 8 bytes key for encrypt/decrypt
//                     key[0] .. MSByte
//                     key[7] .. LSByte
//	      	  mode: 8 bits = xFEIxxKK 	
//					F = Do Final Des Permutation
//					E = Des Encryption Mode (1=Encrypt)
//					I = Do Initial Des Permutation
//					KK = Key Number (1 to 3)
//       OUT: *plaintext 8 bit encrypted/decrypted data 
//                     plaintext[0] .. MSByte
//                     plaintext[7] .. LSByte
//    RETURN: 
/*===============================================================================================
 * Name:			SingleDesOperation
 * Description:	des 加密解密
 * Parameters: 	plain--------输入 待加密数据指针
 								key----------输入 8字节密钥
 								desdata-------输出 8字节密钥
 								mode----------输入 加密解密模式
 * ReturnValue:	none.
 * Notes:      	none.
 * Author: Tiger.gao   2011-11-03 15:49:54
 *===============================================================================================*/
extern void SingleDesOperation (unsigned char *plain,unsigned char *key,unsigned char *desdata,unsigned char mode)	 ;
/*===============================================================================================
 * Name:			TripleDES
 * Description:	3 des 加密解密
 * Parameters: 	plain--------待加密数据指针
								key-----------16字节秘钥
 								desdata-------16字节秘钥
 								mode---------加密解密模式
 * ReturnValue:	none.
 * Notes:      	none.
 * Author: Tiger.gao   2011-11-03 15:49:54
 *===============================================================================================*/
extern void TripleDES(unsigned char *plain,unsigned char *key,unsigned char *desdata,unsigned char mode);

#ifdef __cplusplus
}
#endif

#endif

