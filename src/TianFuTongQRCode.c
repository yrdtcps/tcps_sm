/***********************************
* Copyright (c) 2017,天津通卡
* All rights reserved.
* 文件名称：JTBQRCodeV1.c
* 文件标识：
* 摘要：实现天府通脱机二维码的验签
*************************************/
/* ***********************************
* 作者：杨瑞东，修改日期：2019年11月27日
* 修改简要说明：使用SM2算法进行验签
*	
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "VD1050.h"
#include "tk_log.h"
#include "cJSON.h"
#include "CITY.h"
#include "LIB_TCPS_OPENSSL.h"


time_t getUTC20170101()
{
#if 0
	struct tm sttm;
	memset(&sttm, 0x00, sizeof(struct tm));
	sttm.tm_year = 2017 - 1900; //2017年
	sttm.tm_mon = 0; //1月
	sttm.tm_mday = 1; //1日
	sttm.tm_hour = 0;
	sttm.tm_min = 0;
	sttm.tm_sec = 0;
	time_t utc;
	utc = mktime(&sttm);
	// printf("\r\n ******* 2017-1-1 00:00:00 utc = %08X **********\r\n", utc);
	return utc;
#endif
	return 0x5867D600;
}

time_t getCurrUTCBase2017()
{
	time_t now = time(NULL);
	time_t utc_2017 = getUTC20170101();
	return now - utc_2017;
}


/*************************************************
*  Function:       // Read_TianFuTong_PARAM
*  Description:    // 获取 参数信息
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其它说明
*************************************************/
int Read_TianFuTong_PARAM(void)
{  
    FILE  *df=NULL;
	int ret;
	unsigned long file_len;
	unsigned char *file_buff;


	df=fopen("/usr/VD1050/CITY_fe0c_PARAM.txt","r");
	if(df==NULL)
	{
		return -1;
	}

	fseek(df,0,SEEK_SET);

	file_len=get_file_size("/usr/VD1050/CITY_fe0c_PARAM.txt");
	file_buff= (unsigned char*)malloc(file_len);

	

	ret=fread(file_buff,file_len,1,df);
	if(ret==0)
	{
		A_PRINTF(ERROR,"Read TianFuTong PARAM error\n"); 
		fclose(df);
		free(file_buff);
		return -1;
	}

	cJSON *Param=cJSON_Parse((char *)file_buff);
	if(Param==NULL)
	{
		A_PRINTF(ERROR,"Read TianFuTong  Creat Json Param error\n");
		free(file_buff); 
		fclose(df);
		return -1;
	}

	cJSON *Item=cJSON_GetObjectItem(Param,"response");
	if(Item==NULL)
	{
		A_PRINTF(ERROR,"Read PARAM  Get Json Response type error\n");
		cJSON_Delete(Param); 
		free(file_buff); 
		fclose(df);
		return -1;
	}



	cJSON *pparam=cJSON_GetObjectItem(Param,"param");
	if(Item==NULL)
	{
		A_PRINTF(ERROR,"Read PARAM  Get Json param type error\n");
		cJSON_Delete(Param); 
		free(file_buff); 
		fclose(df);
		return -1;
	}

	Item=NULL;
	Item=cJSON_GetObjectItem(pparam,"amt");
	if(Item==NULL)
	{
		A_PRINTF(ERROR,"Read PARAM Get Json txn_amt  value error\n");
		cJSON_Delete(Param); 
		free(file_buff); 
		fclose(df);
		return -1;
	}
	//ZZB_AMT=Item->valueint;

	char* pjson=cJSON_PrintUnformatted(pparam);

	PRINTF(INFO,"Read TianFuTong PARAM succcess:param=%s\n",pjson); 
	cJSON_Delete(Param);  
	free(file_buff);
	free(pjson); 
	fclose(df);
	return 0;
}  


/*************************************************
*  Function:       // check_TianFuTong_qrcode_demo
*  Description:    // 验证交通部版本1二维码
*  Input:          // *qrcode	:二维码数据
*                  // *qrcode_len	:二维码数据长度
*  Output:         // QR_INFO  二维码结果
*  Return:         // －2：二维码结构错误  －1:失败  0 : 成功
*  Others:         // 其它说明
*************************************************/
int check_TianFuTong_qrcode_demo(unsigned char* qrcode, int* qrcode_len)
{
#if 0
	int ret = 0;
	int i=0;
	unsigned char SIGN[100];
	unsigned int  SIGN_len=0;
	
	unsigned int key_id=0;
	int index=0;

	unsigned char qrdata[500];
	int  qrlen=0;
	unsigned char Public_Key[200]={0};
	unsigned int key_len=0;
	unsigned char XY[200]={0};
	struct timeval tv;
	unsigned char sm3_digest[32]={0};
	unsigned char Y[100];
	// unsigned char MMYY[10];
	time_t now;
	// struct tm *tm_now;
	long valid_secs = 0;
	long birth_utc = 0;
	long expire_utc = 0;

	QR_INFO.QR_CodeType=0xff;
	QR_INFO.LAST_LEN[0]=(*qrcode_len)>>8;
	QR_INFO.LAST_LEN[1]=(*qrcode_len);
	memcpy((char *)&QR_INFO.LAST_QR[0],qrcode,*qrcode_len);

	if(QR_TianFuTong_ENABLE==0)
	{
		A_PRINTF(ERROR,"QR TianFuTong NOT ENABLE\n");
		QR_INFO.QR_CodeState=1;
		return -2;
	}

	if(strlen((char *)&qrcode[0])<(*qrcode_len))
	{
		return -2;
	}

	memset(qrdata,0,sizeof(qrdata));
    memcpy(qrdata,qrcode,*qrcode_len);
	qrlen=*qrcode_len;

	qrlen=base64_decode((unsigned char *)qrdata,(unsigned char *)qrdata);
	PRINTF(DEBUG,"base64_decode ret=%d\n",qrlen);
	if(qrlen<0)return -2;

	if(qrdata[0]>=0x80)
	{
		A_PRINTF(ERROR,"qrdata head  err  %02x\n",qrdata[0]); 
		return -2;
	}
	if((qrdata[1]!=0x12)&&(qrdata[1]!=0x21)) //0x12.非对称密钥脱机码 0x21.对称密钥联机码
	{
		A_PRINTF(ERROR,"qrdata type   err  %02x\n",qrdata[1]); 
		return -2;
	}
	if(0x12 == qrdata[1]) //脱机码
	{
		if(qrdata[177] + 243 != qrlen)
		{
			A_PRINTF(ERROR,"qrdata len  err  %d\n",qrlen); 
			return -2;
		}
	}
	else //联机码
	{
		if(qrdata[42] + 51 != qrlen)
		{
			A_PRINTF(ERROR,"qrdata len  err  %d\n",qrlen); 
			return -2;
		}
		//天府通联机码直接吐给卡机处理
		QR_INFO.QR_CodeState=1;
		return -1;
	}

	//获取公钥

	key_id=(qrdata[2]<<8)|qrdata[3];
	memset(Public_Key,0,sizeof(Public_Key));
	ret=GET_CITY_KEY(key_id,(char *)Public_Key,TianFuTong_NO);
	if(ret!=0)
	{
		A_PRINTF(ERROR,"get key  err  %d\n",key_id); 
		QR_INFO.QR_CodeState=1;/// 
		return -2;
	}


	QR_INFO.QR_CodeType=0x15;

	// time(&now);
	// tm_now=localtime(&now);
	// hex_string_to_bytes((char *)&Public_Key[0],4,MMYY,2);

	// //printf("key==%s \n",Public_Key); 
	// if((tm_now->tm_year+1900)>(2000+((MMYY[1]&0xf0)>>4)*10+(MMYY[1]&0x0f)))
	// {
	// 	PRINTF(INFO,"key  overdue %02x%02x \n",MMYY[0],MMYY[1]); 
	// 	QR_INFO.QR_CodeState=2;/// 
	// 	return -1;
	// }
	// else if((tm_now->tm_year+1900)==(2000+((MMYY[1]&0xf0)>>4)*10+(MMYY[1]&0x0f)))
	// {
	// 	if((tm_now->tm_mon+1)>=(((MMYY[0]&0xf0)>>4)*10+(MMYY[0]&0x0f)))
	// 	{
	// 		PRINTF(INFO,"key  overdue %02x%02x \n",MMYY[0],MMYY[1]);  
	// 		QR_INFO.QR_CodeState=2;/// 
	// 		return -1;
	// 	}
	// }


	now = getCurrUTCBase2017();
	if(now <= 0)
	{
		A_PRINTF(ERROR,"sys clock err\n"); 
		QR_INFO.QR_CodeState=9;/// 
		return -1;
	}

	gettimeofday(&tv,NULL);
	PRINTF(DEBUG,"TIME1:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));

	//有效时间 二维码生成后的有效秒数
	valid_secs = (qrdata[16] << 8) | qrdata[17];
	//生成时间
	birth_utc = (qrdata[165] << 24) | (qrdata[166] << 16) | (qrdata[167] << 8) | qrdata[168];
	//过期时间
	expire_utc = (qrdata[88] << 24) | (qrdata[89] << 16) | (qrdata[90] << 8) | qrdata[91];

	if(now > expire_utc || now < birth_utc || now > birth_utc+valid_secs)
	{
		A_PRINTF(ERROR,"qr time err\n"); 
		QR_INFO.QR_CodeState=6;/// 
		return -1;
	}
	
	//签名认证

	//SET_SIGN_DATA(&qrdata[93],64,(unsigned char *)SIGN,(unsigned int *)&SIGN_len);

	hex_string_to_bytes((char *)&Public_Key[6],128,XY,64);
	TCPS_SM3(&XY[0],&XY[32],&qrdata[4],88,sm3_digest);

	ret=TCPS_SM2_verify2((char *)&Public_Key[4],sm3_digest,32,SIGN,SIGN_len);
	if(ret==0)
	{
		A_PRINTF(ERROR,"STEP 1 SM2_verify  OK\n"); 
	}
	else
	{
		A_PRINTF(ERROR,"STEP 1 SM2_verify  fail\n"); 
		QR_INFO.QR_CodeState=3;/// 
		return -1;
	}



	// memset(Public_Key,0,sizeof(Public_Key));
	// memset(Y,0,sizeof(Y));
	// bytes2hex_string((unsigned char *)Public_Key,&qrcode[51],33);
	
	// TCPS_SM2_GetY((char *)Public_Key,(char *)&Y[0],sizeof(Y));
	// i=strlen((char *)Y);
	// if((i>64)||(i<60))
	// {
	// 	A_PRINTF(ERROR,"STEP 2 get Y  fail %s\n",Y); 
	// 	QR_INFO.QR_CodeState=4;/// 
	// 	return -1;
	// }

	// memset(&Public_Key[66],0x30,64);
	// memcpy(&Public_Key[66+64-i],&Y[0],i);
	// Public_Key[0]='0';
	// Public_Key[1]='4';

	memset(Public_Key,0,sizeof(Public_Key));
	bytes2hex_string((unsigned char *)Public_Key+2,&qrdata[24],64);
	Public_Key[0]='0';
	Public_Key[1]='4';


	//签名认证
	//SET_SIGN_DATA(&qrdata[qrdata[177]+179],64,(unsigned char *)SIGN,(unsigned int *)&SIGN_len);

	hex_string_to_bytes((char *)&Public_Key[2],128,XY,64);

	TCPS_SM3(&XY[0],&XY[32],&qrdata[157],21+qrdata[177],sm3_digest);

	ret=TCPS_SM2_verify2((char *)Public_Key,sm3_digest,32,SIGN,SIGN_len);
	if(ret==0)
	{
		A_PRINTF(ERROR,"STEP 2 SM2_verify  OK\n"); 
	}
	else
	{
		A_PRINTF(ERROR,"STEP 2 SM2_verify  fail\n"); 
		QR_INFO.QR_CodeState=5;/// 
		return -1;
	}




	if(*qrcode_len<RPT_QRCode_MAX_LEN)
	{
						
		memcpy((char *)&RPT_QRCode[RPT_QRCode_Index][RPT_QRCode_ID_Len],(char *)qrcode,*qrcode_len);

						
	}
	else
	{
		memcpy((char *)&RPT_QRCode[RPT_QRCode_Index][RPT_QRCode_ID_Len],(char *)qrcode,RPT_QRCode_MAX_LEN);
	}

	RPT_Record++;

	RPT_QRCode[RPT_QRCode_Index][0]=tv.tv_sec>>24;
	RPT_QRCode[RPT_QRCode_Index][1]=tv.tv_sec>>16;
	RPT_QRCode[RPT_QRCode_Index][2]=tv.tv_sec>>8;
	RPT_QRCode[RPT_QRCode_Index][3]=tv.tv_sec;
	RPT_QRCode[RPT_QRCode_Index][4]=RPT_Record>>24;
	RPT_QRCode[RPT_QRCode_Index][5]=RPT_Record>>16;
	RPT_QRCode[RPT_QRCode_Index][6]=RPT_Record>>8;
	RPT_QRCode[RPT_QRCode_Index][7]=RPT_Record;

	QR_INFO.QRID_LEN[0]=RPT_QRCode_ID_Len>>8;
	QR_INFO.QRID_LEN[1]=RPT_QRCode_ID_Len;
	memcpy((char *)&QR_INFO.QRID[0],(const char *)&RPT_QRCode[RPT_QRCode_Index][0],RPT_QRCode_ID_Len);

	RPT_QRCode_Index=RPT_QRCode_Index>=(RPT_QRCode_MAX_NO-1)?0:RPT_QRCode_Index+1;




	gettimeofday(&tv,NULL);
	PRINTF(DEBUG,"TIME2:%lds:%ldms\n",tv.tv_sec,(tv.tv_usec/1000));
	QR_INFO.QR_CodeState=0;
	PRINTF(INFO,"TianFuTong QRCode check successed\n");
	PRINTF(INFO,"===========End================\n");
#endif
	return 0;
}



