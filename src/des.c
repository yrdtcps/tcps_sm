/***********************************************************************
*                                                                      *
* File:         DES.C (Source)                                         *
*                                                                      *
* Version:                                                        *
*                                                                      *
* Created:                                                     *
* Last Change:                                                 *
*                                                                      *
*                                		       *
*                                                                      *
*                                        *
*                                                                      *
*                                    *
*                                                                      *
*		   	               *
*                                                                      *
*     						                       *
***********************************************************************/


 /******************************************************************************
 *                                   DES.C                                     *
 *******************************************************************************
 *                            ENCRYPTION  PROGRAM                              *
 *******************************************************************************
 * void encrypt(unsigned char *plaintext,unsigned char *key,unsigned char mode)*
 * 1) plaintext: -in- plaintext,-out- string encrypted or decrypted; 8 bytes.  *
 * 2) key: the key string used to encrypt or todecrypt; 8 bytes.               *
 * 3) mode:  0->to encrypt,1->to decrypt; 1 bytes.                             *
 ******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
//#include <intrins.h>
#include <string.h>
//#include "Des.h"
#define DO_INITIAL_DES_PERMUTATION      0x10
#define DES_ENCRYPTION_MODE             0x20
#define DES_DECRYPTION_MODE             0
#define DO_FINAL_DES_PERMUTATION        0x40

#define DES_ENCRYPT_BLOCK (DO_INITIAL_DES_PERMUTATION|DES_ENCRYPTION_MODE|DO_FINAL_DES_PERMUTATION )
#define DES_DECRYPT_BLOCK (DO_INITIAL_DES_PERMUTATION|DES_DECRYPTION_MODE|DO_FINAL_DES_PERMUTATION )

//unsigned char deskey[8]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};

const unsigned char   IPP[64]={57,49,41,33,25,17, 9,1,  59,51,43,35,27,19,11,3,
                             61,53,45,37,29,21,13,5,  63,55,47,39,31,23,15,7,
                             56,48,40,32,24,16, 8,0,  58,50,42,34,26,18,10,2,
                             60,52,44,36,28,20,12,4,  62,54,46,38,30,22,14,6};

const unsigned char   IPN[64]={39,7,47,15,55,23,63,31,  38,6,46,14,54,22,62,30,
                             37,5,45,13,53,21,61,29,  36,4,44,12,52,20,60,28,
                             35,3,43,11,51,19,59,27,  34,2,42,10,50,18,58,26,
                             33,1,41, 9,49,17,57,25,  32,0,40, 8,48,16,56,24};//Inverse permutation

const unsigned char  Choose56[56]={56,48,40,32,24,16,8,  0,57,49,41,33,25,17, 9, 1,
                             58,50,42,34,26,18,10, 2, 59,51,43,35,62,54,46,38,
                             30,22,14, 6,61,53,45,37, 29,21,13, 5,60,52,44,36,
                             28,20,12, 4,27,19,11, 3};                    //Choosing the 56bit key from 64bit

const unsigned char  Choose48[48]={31,0,1,2, 3, 4, 3,4,   5, 6, 7, 8, 7, 8, 9,10,
                             11,12,11,12,13,14,15,16,  15,16,17,18,19,20,19,20,
                             21,22,23,24,23,24,25,26,  27,28,27,28,29,30,31, 0};//expands the right half text of 32 bits to 48 bits

const unsigned char     E[48]={13,16,10,23, 0, 4, 2,27,  14, 5,20, 9,22,18,11, 3,
                             25, 7,15, 6,26,19,12, 1,  40,51,30,36,46,54,29,39,
                             50,44,32,47,43,48,38,55,  33,52,45,41,49,35,28,31};//Compression and permutation for key

const unsigned char    PP[32]={15, 6,19,20,28,11,27,16,  0 ,14,22,25, 4,17,30, 9,
                              1, 7,23,13,31,26, 2, 8,  18,12,29, 5,21,10, 3,24};//P-box permutation

const unsigned char  S[8][64]={{14,4,13, 1, 2,15,11, 8, 3,10, 6,12, 5, 9, 0, 7,
                              0,15, 7, 4,14, 2,13, 1,10, 6,12,11, 9, 5, 3, 8,
                              4, 1,14, 8,13, 6, 2,11,15,12, 9, 7, 3,10, 5, 0,
                             15,12, 8, 2, 4, 9, 1, 7, 5,11, 3,14,10, 0, 6,13},
                            {15, 1, 8,14, 6,11, 3, 4, 9, 7, 2,13,12, 0, 5,10,
                              3,13, 4, 7,15, 2, 8,14,12, 0, 1,10, 6, 9,11, 5,
                              0,14, 7,11,10, 4,13, 1, 5, 8,12, 6, 9, 3, 2,15,
                             13, 8,10, 1, 3,15, 4, 2,11, 6, 7,12, 0, 5,14, 9},
                            {10, 0, 9,14, 6, 3,15, 5, 1,13,12, 7,11, 4, 2, 8,
                             13, 7, 0, 9, 3, 4, 6,10, 2, 8, 5,14,12,11,15, 1,
                             13, 6, 4, 9, 8,15, 3, 0,11, 1, 2,12, 5,10,14, 7,
                              1,10,13, 0, 6, 9, 8, 7, 4,15,14, 3,11, 5, 2,12},
                            { 7,13,14, 3, 0, 6, 9,10, 1, 2, 8, 5,11,12, 4,15,
                             13, 8,11, 5, 6,15, 0, 3, 4, 7, 2,12, 1,10,14, 9,
                             10, 6, 9, 0,12,11, 7,13,15, 1, 3,14, 5, 2, 8, 4,
                              3,15, 0, 6,10, 1,13, 8, 9, 4, 5,11,12, 7, 2,14},
                            { 2,12, 4, 1, 7,10,11, 6, 8, 5, 3,15,13, 0,14, 9,
                             14,11, 2,12, 4, 7,13, 1, 5, 0,15,10, 3, 9, 8, 6,
                              4, 2, 1,11,10,13, 7, 8,15, 9,12, 5, 6, 3, 0,14,
                             11, 8,12, 7, 1,14, 2,13, 6,15, 0, 9,10, 4, 5, 3},
                            {12, 1,10,15, 9, 2, 6, 8, 0,13, 3, 4,14, 7, 5,11,
                             10,15, 4, 2, 7,12, 9, 5, 6, 1,13,14, 0,11, 3, 8,
                              9,14,15, 5, 2, 8,12, 3, 7, 0, 4,10, 1,13,11, 6,
                              4, 3, 2,12, 9, 5,15,10,11,14, 1, 7, 6, 0, 8,13},
                            { 4,11, 2,14,15, 0, 8,13, 3,12, 9, 7, 5,10, 6, 1,
                             13, 0,11, 7, 4, 9, 1,10,14, 3, 5,12, 2,15, 8, 6,
                              1, 4,11,13,12, 3, 7,14,10,15, 6, 8, 0, 5, 9, 2,
                              6,11,13, 8, 1, 4,10, 7, 9, 5, 0,15,14, 2, 3,12},
                            {13, 2, 8, 4, 6,15,11, 1,10, 9, 3,14, 5, 0,12, 7,
                              1,15,13, 8,10, 3, 7, 4,12, 5, 6,11, 0,14, 9, 2,
                              7,11, 4, 1, 9,12,14, 2, 0, 6,10,13,15, 3, 5, 8,
                              2, 1,14, 7, 4,10, 8,13,15,12, 9, 0, 3, 5, 6,11}};

const unsigned char  key_round[32]={1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1,
                                  0,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};   //shift key left //shift key right

//unsigned char  key_deround[16]={0,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

const unsigned char  _bitposition[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

void _checktable (unsigned char line, unsigned char *text,
                  unsigned char *lasttext, unsigned char  *IDD)
{
    unsigned char i,j,k,temp,temp2;
  
    for( j=0,k=0; j<line; j++,k+=8 )
    {
        lasttext[j]=0;
        for( i=0; i<8; i++ ) 
        {
            temp2 = IDD[k+i];	
            temp = text[temp2/8]; // get the byte
            temp &= _bitposition[temp2&0x7]; // get the bit	
            if(temp)
                lasttext[j] |= _bitposition[i];
        }
    }
}

/**************************************************************************************************/
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
void SingleDesOperation (unsigned char *plain,unsigned char *key,unsigned char *desdata,unsigned char mode)
{ 
	static unsigned char  prevtext[8],plaintext[8];
	unsigned char  prevkey[8],Ltext[4],Rtext[4];
  	unsigned char  i,j,temp,temp1,Round;
  	for(i=0;i<8;i++)
	plaintext[i]=plain[i];
	if(mode & DO_INITIAL_DES_PERMUTATION)
   	{
	_checktable(8,plaintext,prevtext,(unsigned char *)IPP); //Initial permutation
   	}
  	for( i=0; i<4; i++ )
	{
		Ltext[i]=prevtext[i];
	  	Rtext[i]=prevtext[i+4];
	}
  	_checktable(7,key,prevkey,(unsigned char *)Choose56);

  	for( Round=0; Round<16; Round++ )
    {
		//rotate both 28bits block to left(encrypt) or right(decrypt)
		if(mode & DES_ENCRYPTION_MODE) // encrypt
		{
	  		for( j=0; j<key_round[Round]; j++ )
            {
	    		temp = prevkey[3]&0x08;
                for( i=7; i>0; i-- )
	    		{
	      			temp1 = prevkey[i-1]&0x80;
                  	prevkey[i-1]<<=1;
                  	if(temp) 
						prevkey[i-1]|=0x01;
                  	temp = temp1;
                }
                if(temp) prevkey[3]|=0x10;
                else     prevkey[3]&=0xEF;
          	}
		}
  		else // decrypt   
		{
	  		for( j=0; j<key_round[Round+16]; j++ )
            {
		    	temp=prevkey[3]&0x10;
                for( i=0; i<7; i++ )
		    	{
		      		temp1=prevkey[i]&0x01;
                    prevkey[i]>>=1;
                    if(temp) 
		        		prevkey[i]|=0x80;
                    temp=temp1;
                }
                if(temp) prevkey[3]|=0x08;
                else     prevkey[3]&=0xF7;
            }
		}
	 
      	_checktable(6,prevkey,plaintext,(unsigned char *)E); //Compression permutation
      	_checktable(6,Rtext,prevtext,(unsigned char *)Choose48); //Expansion permutation

 		//Expanded right half XOR with the subkey
		prevtext[0] ^= plaintext[0]; 
		prevtext[1] ^= plaintext[1]; 
		prevtext[2] ^= plaintext[2]; 
		prevtext[3] ^= plaintext[3]; 
		prevtext[4] ^= plaintext[4]; 
		prevtext[5] ^= plaintext[5]; 

		for( j=6,i=8; j>0; j-=3,i-=4 )	//S-Box Substitution
       	{
            plaintext[i-1] = prevtext[j-1];
		  	plaintext[i-2] = ((prevtext[j-1]>>6)&0x03)|(prevtext[j-2]<<2);
		  	plaintext[i-3] = ((prevtext[j-2]>>4)&0x0f)|(prevtext[j-3]<<4);
		  	plaintext[i-4] = prevtext[j-3]>>2;
        }        

      	for( i=0; i<8; i++ ) //Get the S-Box location 
		{
		  	temp = plaintext[i]&0x21;
            if(temp&0x01) temp = (temp&0x20)|0x10;
        	plaintext[i] = temp|((plaintext[i]>>1)&0x0F);
        }
      		
		//Get S-Box output 
		plaintext[0] = S[0][plaintext[0]]; 
		plaintext[1] = S[1][plaintext[1]]; 
		plaintext[2] = S[2][plaintext[2]]; 
		plaintext[3] = S[3][plaintext[3]]; 
		plaintext[4] = S[4][plaintext[4]]; 
		plaintext[5] = S[5][plaintext[5]]; 
		plaintext[6] = S[6][plaintext[6]]; 
		plaintext[7] = S[7][plaintext[7]]; 

      	//Combine 4-bit block to form 32-bit block
		plaintext[0] = (plaintext[0]<<4)|plaintext[1];
		plaintext[1] = (plaintext[2]<<4)|plaintext[3];
        plaintext[2] = (plaintext[4]<<4)|plaintext[5];
		plaintext[3] = (plaintext[6]<<4)|plaintext[7];

      	_checktable(4,plaintext,prevtext,(unsigned char *)PP);

      	for( i=0; i<4; i++ )
		{
		  	prevtext[i] ^= Ltext[i];
            Ltext[i] = Rtext[i]; 
		  	Rtext[i] = prevtext[i];
        }
  	}

	for( i=0; i<4; i++ )
	{
  		prevtext[i]=Rtext[i];
  		prevtext[i+4]=Ltext[i];
	}

   	if(mode & DO_FINAL_DES_PERMUTATION)
   	{
		_checktable(8,prevtext,plaintext,(unsigned char *)IPN); //Final permutation
   	}
	memcpy(desdata,plaintext,8);
}
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
void TripleDES(unsigned char *plain,unsigned char *key,unsigned char *desdata,unsigned char mode)
{
	unsigned char  ucTmpBuf[8];
	if(mode==DES_ENCRYPT_BLOCK)
	{
		SingleDesOperation(plain,key,ucTmpBuf,DES_ENCRYPT_BLOCK);
		SingleDesOperation(ucTmpBuf,key+8,ucTmpBuf,DES_DECRYPT_BLOCK);
		SingleDesOperation(ucTmpBuf,key,desdata,DES_ENCRYPT_BLOCK);
	}
	else
	{
		SingleDesOperation(plain,key,ucTmpBuf,DES_DECRYPT_BLOCK);
		SingleDesOperation(ucTmpBuf,key+8,ucTmpBuf,DES_ENCRYPT_BLOCK);
		SingleDesOperation(ucTmpBuf,key,desdata,DES_DECRYPT_BLOCK);
	}

}

