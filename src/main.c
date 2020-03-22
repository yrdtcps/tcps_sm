#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
// #include "lib_tcps_openssl.h"
#include "LIB_TCPS_OPENSSL.h"
#include "funtool.h"

// #define T_SM2_SIGN 1 //国密签名
// #define T_SM2_VERIFY 2 //国密验签
// #define T_SM2_ENC 3 //国密加密
// #define T_SM2_DEC 4 //国密解密
// #define TARGE_  T_SM2_ENC

// #if TARGE_ == T_SM2_VERIFY


/*************************************************
*  Function:       // SET_SIGN_DATA
*  Description:    // 拼接签名数据
*  Input:          // *sign		:签名原始数据  签名r＋签名s
*                  // sign_len	:签名原始数据长度
*  Output:         // *SIGN		:签名拼接数据
*                  // *SIGN_len	:签名拼接数据长度
*  Return:         // 无
*  Others:         // 输出数据格式 0x30+后边数据长度＋0x02+签名r长度＋签名r＋0x02+签名s长度＋签名s
*************************************************/
void SET_SIGN_DATA(const unsigned char *sign,unsigned int sign_len,unsigned char *SIGN,unsigned int *SIGN_len)
{
	unsigned int i=0;

	i=0;
	memcpy(&SIGN[0],"\x30\x44",2);
	i+=2;
	if(sign[0]&0x80)
	{
			SIGN[i++]=0x02;
			SIGN[i++]=sign_len/2+1;
			SIGN[i++]=0x00;
			memcpy(&SIGN[i],sign,sign_len/2);
			i+=(sign_len/2);
			
	}
	else
	{
			SIGN[i++]=0x02;
			SIGN[i++]=sign_len/2;
			memcpy(&SIGN[i],sign,sign_len/2);
			i+=(sign_len/2);
	}
		
	if(sign[sign_len/2]&0x80)
	{
			SIGN[i++]=0x02;
			SIGN[i++]=sign_len/2+1;
			SIGN[i++]=0x00;
			memcpy(&SIGN[i],&sign[sign_len/2],sign_len/2);
			i+=(sign_len/2);
			
	}
	else
	{
			SIGN[i++]=0x02;
			SIGN[i++]=sign_len/2;
			memcpy(&SIGN[i],&sign[sign_len/2],sign_len/2);
			i+=(sign_len/2);
	}
	SIGN[1]=i-2;
	*SIGN_len=i;
}


#if 1
int main(int argc, char* argv[])
{
	unsigned char pubkey[64] = {0};
	unsigned char signature[64] = {0};
	unsigned char data[4096] = {0};
	int datalen = 0;
	char* sPubKey = NULL;
	char* sSignature = NULL;
	char* sData = NULL;

	int itmp;
	int ret;
	// unsigned char user_id[] = {"1234567812345678"};
	// unsigned int user_id_len = (unsigned int)(strlen((char *)user_id));

	int opt;

	opterr = 0;  //使getopt不行stderr输出错误信息

	while( (opt = getopt(argc, argv, "k:s:d:")) != -1 )
	{
		printf("%c=%s\n", opt, optarg);
		switch (opt)
		{
		case 'k': //pubkey
			sPubKey = optarg;
			if(strlen(sPubKey) == 130)
			{
				if(memcmp(sPubKey, "04", 2))
				{
					fprintf(stderr, "pubkey must begin with 04 when len is 65 bytes!\n");
					exit(2);
				}
				HexStr2Buf(sPubKey + 2, pubkey);
			}
			else if(strlen(sPubKey) == 128)
			{
				HexStr2Buf(sPubKey, pubkey);
			}
			else if(strlen(sPubKey) == 66)
			{
				if(memcmp(sPubKey, "02", 2) && memcmp(sPubKey, "03", 2))
				{
					fprintf(stderr, "pubkey must begin with 02 or 03 when len is 33 bytes!\n");
					exit(2);
				}
				HexStr2Buf(sPubKey + 2, pubkey);
			}
			else
			{
				fprintf(stderr, "pubkey len must be 65 or 64 or 33 bytes!\n");
				exit(2);
			}
			break;
		case 's': //signature
			sSignature = optarg;
			if(strlen(sSignature) != 128)
			{
				fprintf(stderr, "signature len must be 64 bytes!\n");
				exit(3);
			}
			// hexstr2buf(sSignature, signature, &itmp);
			HexStr2Buf(sSignature, signature);
			break;
		case 'd': //data
			sData = optarg;
			if(strlen(sSignature) < 2)
			{
				fprintf(stderr, "data len must be more than 1 byte!\n");
				exit(4);
			}
			else if (strlen(sData) > sizeof(data)*2)
			{
				fprintf(stderr, "data len too large, max %d bytes!\n", sizeof(data));
				exit(5);
			}
			
			// hexstr2buf(sData, data, &datalen);
			if(HexStr2Buf(sData, data) < 0)
			{
				fprintf(stderr, "data format err!\n");
				exit(5);
			}
			datalen = strlen(sData) / 2;
			break;
		default: /* '?' */
ShowHelp:
			fprintf(stderr, "Usage: %s -k pubkey -s signature -d data\n", argv[0]);
			fprintf(stderr, "Example: %s -k 031B120EF62E65D55C4655B660EBE784069A440E89877A968F0BC22E43F445A0AA -s 76A00BD0A6B85ECDEE7AC7E3005061CF659050B0B0062CD4FB96E726371BEA6EF657064A9E586C19B45A9A193FDBB6F089BB766B85F4648E85E13DDBC24F7D60 -d 128000000112210000010404002102BF294C021507282DE822240E3A34A4350829955502194B28202DBAA5B6315099\n", argv[0]);
			exit(1);
		}
	}

	if(!sPubKey || !sSignature || !sData)
	{
		goto ShowHelp;
	}
	// ret = sm2_verify_sig(data, datalen, user_id, user_id_len, pubkey, (SM2_SIGNATURE_STRUCT*)signature);
	char sFullPubKey[132] = {0};
	strcat(sFullPubKey, "04");
	if(strlen(sPubKey) >= 128)
	{
		Buf2HexStr(pubkey, 64, sFullPubKey + 2);
	}
	else
	{
		Buf2HexStr(pubkey, 32, sFullPubKey + 2);
		ret = TCPS_SM2_GetY(sPubKey, sFullPubKey + 66, 66);
		if(ret)
		{
			fprintf(stderr, "Calc PubKey Y failed!", argv[0]);
			exit(7);
		}
		HexStr2Buf4(sFullPubKey + 66, 64, pubkey + 32, 32);
	}

	unsigned char SIGN[100];
	unsigned int SIGN_len = 0;
	SET_SIGN_DATA(signature, 64, SIGN, &SIGN_len);

	//SM3
	unsigned char SM3[32];
	TCPS_SM3(pubkey, pubkey+32, data, datalen, SM3);
// bytes2hex_string((unsigned char*)ss, SM3, 32);
// printf("SM3[%d] = %s\n", strlen(ss), ss);

printf("sFullPubKey[%d] = %s\n", strlen(sFullPubKey), sFullPubKey);
	//Very Signature
	ret = TCPS_SM2_verify2(sFullPubKey, SM3, 32, SIGN, SIGN_len);
	if (ret)
	{
		printf("Verify SM2 signature failed!\n");
		return 7;
	}
	printf("Verify SM2 signature success.\n");
	return 0;
}

#else



int main(int argc, char* argv[])
{
	// printf("sizeof(long)=%d, sizeof(int)=%d\n", sizeof(long), sizeof(int));
	// return 0;
	unsigned char temp[256];
	unsigned char SIGN[100];
	unsigned int SIGN_len = 0;

	//公钥HEX字符串
	// char Public_Key_Str[500] = "041B120EF62E65D55C4655B660EBE784069A440E89877A968F0BC22E43F445A0AAFD9FFABBD1A7E87A8DF69522B95AD95176F3B12FDFA88E5791CF1F2EDF3575E7";
	char Public_Key_Str[500] = "04BF294C021507282DE822240E3A34A4350829955502194B28202DBAA5B6315099FA51ECC8D576C3F3E7DBC3A976D8C0E24448D282B50ACD6654A36C8B994FFDF8";
	// char Public_Key_Str[500] = "042AAB965EB8242BF1819096B10F04B9FFD6DAE308E7940520B350041ACF9B0091CCA471038B81E84867ECAC74500540F4D119219A8F8AE3FEDAC404A50BCA790C";
	// char Public_Key_Str[500] = "0466ad71afb4e4780a056fe6d2e8faf723262e141deed6cdf142670ad5d06fb6f4d38d59e67d1d90d3c77c9daecaca8da19d15dca0c5cffe84770d2686e5f20a6e";
	unsigned char PubKeyBytes[200];

	//明文HEX字符串
	// char* sPlain = "128000000112210000010404002102BF294C021507282DE822240E3A34A4350829955502194B28202DBAA5B6315099";
	char* sPlain = "240101000001128000000112210000010404002102BF294C021507282DE822240E3A34A4350829955502194B28202DBAA5B631509976A00BD0A6B85ECDEE7AC7E3005061CF659050B0B0062CD4FB96E726371BEA6EF657064A9E586C19B45A9A193FDBB6F089BB766B85F4648E85E13DDBC24F7D6031303031303030303030303230383430808080800000000000018000000120000002010000C803874B31DFE5A58969ECFE06188A7E8F5CDBDAD7B7DE5F210E5286A99DD2EDE8D25E726545007800";
	unsigned int len_plain = strlen(sPlain) / 2;
	unsigned char plain[256];

	//签名HEX字符串
	// char* sSign = "76A00BD0A6B85ECDEE7AC7E3005061CF659050B0B0062CD4FB96E726371BEA6EF657064A9E586C19B45A9A193FDBB6F089BB766B85F4648E85E13DDBC24F7D60";
	char* sSign = "20A9A96EEA8BAA6F8E6C26F8BBAFB23E25151A0E33286BAA795FF7D1715DA969A9B301593C6009B592C7BC0E1588C1889296C5791D8793EBED6D38A4251FAE5C";
	// char* sSign = "703DF9DB9C6EF6CC1346701B100D96FE03C00CA8DCED2FA0996C042B5B91A692884696967F2C40E71C1EBCDE1553C80765068276FC2C211AD773B7FBBC2F5CF8";
	// char* sSign = "e88517c6c11f37f0ee214bf9e7a81be783270eb59ee453cab23a4f5742ad2aebf7321155fc78c02543e0de244766d0eeeeda9258fbe74b5930eaefd0145e6542";


	unsigned char SM3[256];
	int ret;
	char ss[500];

	if(0)
	{
		char* sKeyX = "02BF294C021507282DE822240E3A34A4350829955502194B28202DBAA5B6315099";
		char sKeyY[66] = {0};
		ret = TCPS_SM2_GetY(sKeyX, sKeyY, sizeof(sKeyY));
		printf("TCPS_SM2_GetY, ret = %d, sKeyY = %s\n", ret, sKeyY);
		return 0;
	}

	//公钥字节序列
	hex_string_to_bytes(Public_Key_Str+2, strlen(Public_Key_Str)-2, PubKeyBytes, (strlen(Public_Key_Str)-2)/2);

	//明文字节序列
	// hex_string_to_bytes((char*)sPlain, len_plain*2, plain, len_plain);
	hex_string_to_bytes((char*)sPlain, len_plain*2, plain, len_plain);

	//签名字节序列
	hex_string_to_bytes(sSign, 128, temp, 64);
bytes2hex_string((unsigned char*)ss, temp, 64);
printf("signature[%d] = %s\n", strlen(ss), ss);
	SET_SIGN_DATA(temp, 64, SIGN, &SIGN_len);
bytes2hex_string((unsigned char*)ss, SIGN, SIGN_len);
printf("SIGN[%d] = %s\n", strlen(ss), ss);

bytes2hex_string((unsigned char*)ss, plain, len_plain);
printf("plain[%d] = %s\n", strlen(ss), ss);
bytes2hex_string((unsigned char*)ss, PubKeyBytes, 64);
printf("PubKeyBytes[%d] = %s\n", strlen(ss), ss);
	//SM3
	TCPS_SM3(PubKeyBytes, PubKeyBytes+32, plain, len_plain, SM3);
bytes2hex_string((unsigned char*)ss, SM3, 32);
printf("SM3[%d] = %s\n", strlen(ss), ss);

printf("Public_Key_Str[%d] = %s\n", strlen(Public_Key_Str), Public_Key_Str);
	//Very Signature
	ret = TCPS_SM2_verify2(Public_Key_Str, SM3, 32, SIGN, SIGN_len);
	if(ret)
	{
		printf("sm2 verify error!\n");
	}
	else
	{
		printf("sm2 verify OK.\n");
	}


	return 0;
}


#endif
