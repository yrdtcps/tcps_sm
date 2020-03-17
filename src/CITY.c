/***********************************
* Copyright (c) 2017,天津通卡
* All rights reserved.
* 文件名称：CITY.c
* 文件标识：
* 摘要：主要根据城市代码实现不同城市的二维码秘钥管理
*************************************/
/* ***********************************
* 作者：孟维良，修改日期：2017年9月1日
* 修改简要说明：
**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "funtool.h"
#include "VD1050.h"
#include "CITY.h"
#include "tk_log.h"
#include "cJSON.h"
#include "tk_public.h"
#include "CRC.h"
#include "LIB_TCPS_OPENSSL.h"

extern void TCPS_OPENSSL_LIB_VER(unsigned char* buf,int *len);

const unsigned int CITY_NO[CITY_MAX][2]={{0x00,0x3610},{0x01,0xfe01},{0x02,0x3172},{0x03,0x3500},{0x04,0x4000},{0x05,0x0630},{0x06,0xfe03},{0x07,0xfe04},{0x08,0x7300},{0x09,0xfe05},{0x0a,0xfe07},{0x0b,0xfe08},{0x0c,0xfe09},{0x0d,0xfe06},{0x0e,0xfe0a},{0x0f,0xfe0b},{0x10,0xfe0c}};///索引 ＋城市代码
const char CITY_LIB_NAME[CITY_MAX][100]={{"LIB_TCPS_OPENSSL.so"},{"libqrcode.so"},{"libqrcrypt.so"},{"LIB_TCPS_OPENSSL.so"},{"libjytsdk.so"},{"libxiaobusdk.so"},{"LIB_TCPS_OPENSSL.so"},{"LIB_TCPS_OPENSSL.so"},{"LIB_TCPS_OPENSSL.so"},{"libtcpscodes.so"},{"LIB_TCPS_OPENSSL.so"},{"LIB_TCPS_OPENSSL.so"},{"LIB_TCPS_OPENSSL.so"},{"libpossdk.so"},{"LIB_TCPS_OPENSSL.so"},{"LIB_TCPS_OPENSSL.so"},{"LIB_TCPS_OPENSSL.so"}};///城市 库文件名

volatile unsigned char CITY_LIB_VERSION[CITY_MAX][4];///城市对应的so库版本，通卡OPENSSL库版本（厦门）
volatile FILE_HEAD_STRUCT CITY_Key_St[CITY_MAX];//城市对应秘钥头结构体

/*************************************************
*  Function:       // Find_City_LIB
*  Description:    // 检索城市库
*  Input:          // 库名字
*                  //
*  Output:         // 无
*  Return:         // －1：未找到   >=0 : 城市代码
*  Others:         // 其它说明
*************************************************/

int Find_City_LIB(char *libname)
{
	unsigned int i= 0;

	for(i=0;i<CITY_MAX;i++)
	{

		if(strcmp(libname,CITY_LIB_NAME[i])==0)return CITY_NO[i][1];
	}
	//printf("city no:%x\n",city_no);
	return -1;
}


/*************************************************
*  Function:       // Find_City_NO
*  Description:    // 检索城市列表，确认是否包含索引城市
*  Input:          // city_no	：2字节 16进制  城市代码  例如 ：0x3610
*                  //
*  Output:         // 无
*  Return:         // －1：未找到   >=0 : 城市索引
*  Others:         // 其它说明
*************************************************/

int Find_City_NO(unsigned int city_no)
{
	unsigned int i= 0;
	PRINTF(DEBUG,"city no:%x\n",city_no);
	for(i=0;i<CITY_MAX;i++)
	{
		if(city_no==CITY_NO[i][1])return CITY_NO[i][0];
	}

	return -1;
}

/*************************************************
*  Function:       // Read_CITY_Keyfile
*  Description:    // 根据城市 索引及城市代码获取 相应秘钥信息 并赋值相应秘钥头结构体
*  Input:          // city_index：城市索引
*                  // city_no	：2字节 16进制  城市代码  例如 ：0x3610
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 其它说明
*************************************************/
int Read_CITY_Keyfile(unsigned int city_index,unsigned int city_no)
{
    FILE  *df;
	int ret;
	unsigned long file_len;
	unsigned char *file_buff;
	char str_buf[200];

	memset((unsigned char *)&CITY_Key_St[city_index].ulFileNum[0],0,sizeof(FILE_HEAD_STRUCT));

	memset(str_buf,0x00,sizeof(str_buf));
	sprintf((char*)&str_buf[0],"/usr/VD1050/CITY_%04x_KEY.txt",city_no);

	df=fopen(str_buf,"r");
	if(df==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf);
		return -1;
	}

	fseek(df,0,SEEK_SET);

	file_len=get_file_size(str_buf);
	file_buff= (unsigned char*)malloc(file_len);
	ret=fread(file_buff,file_len,1,df);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf);
		free(file_buff);
		fclose(df);
		return -1;
	}
	//file_buff[file_len]='\0';

	cJSON *Key=cJSON_Parse((char *)file_buff);
	if(Key==NULL)
	{
		A_PRINTF(ERROR,"read CITY Key Creat Json key error\n");
		free(file_buff);
		fclose(df);
		return -1;
	}
	cJSON *Item=cJSON_GetObjectItem(Key,"Key-Head");
	if(Item==NULL)
	{
		A_PRINTF(ERROR,"read CITY key Get Json key_head error\n");
		cJSON_Delete(Key);
		free(file_buff);
		fclose(df);
		return -1;
	}

	char *Head=Item->valuestring;
	if(Head==NULL)
	{
		A_PRINTF(ERROR,"read CITY Key get key_Head error\n");
		cJSON_Delete(Key);
		free(file_buff);
		fclose(df);
		return -1;
	}
	//printf("Head:%s\n",Head);
	ret=hex_string_to_bytes((char *)Head,sizeof(FILE_HEAD_STRUCT)*2,(unsigned char *)&CITY_Key_St[city_index].ulFileNum[0],sizeof(FILE_HEAD_STRUCT));
	if(ret==-1)
	{
		TCPS_LOG(LOG_ERROR,"key head hex_string_to_bytes error");
		cJSON_Delete(Key);
		free(file_buff);
		fclose(df);
		return -1;
	}

	cJSON_Delete(Key);
	free(file_buff);
	fclose(df);
	return 0;
}



/*************************************************
*  Function:       // Read_CITY_PARAM
*  Description:    // 获取 参数信息
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其它说明
*************************************************/
int Read_CITY_PARAM(unsigned int city_no)
{
	int ret=0;

   	/*if(0xfe01==city_no)
	{
		ret=Read_JNWANDA_PARAM();
	}
	else if(0x3172==city_no)
	{
		ret=Read_TTJinTou_PARAM();
	}
	else if(0x3500==city_no)
	{
		ret=Read_FuZhou_PARAM();
	}
	else if(0x4000==city_no)
	{
		ret=Read_CHONGQING_PARAM();
	}
	else if(0x0630==city_no)
	{
		ret=Read_XiaoBu_PARAM();
	}
	else if(ZHENGZHOU_BANK==city_no)
	{
		ret=Read_ZhengZhouBank_PARAM();
	}
	else if(HAERBIN_JTB==city_no)
	{
		ret=Read_HaErBinJTB_PARAM();
	}
	else if(LanZhou_CITY==city_no)
	{
		ret=Read_LanZhou_PARAM();
	}
	else if(TCPS_NO==city_no)
	{
		ret=Read_TCPS_PARAM();
	}
	else if(WeChatSTO_NO==city_no)
	{
		ret=Read_WeChatSTO_PARAM();
	}
	else if(JTBV1_NO==city_no)
	{
		ret=Read_JTBV1_PARAM();
	}
	else if(MeiTuan_QRCode==city_no)
	{
		ret=Read_MeiTuan_PARAM();
	}
	else if(BestPay_QRCode==city_no)
	{
		ret=Read_BestPay_PARAM();
	}
	else if(CMPay_QRCode==city_no)
	{
		ret=Read_CMPay_PARAM();
	}
	else */if(TianFuTong_NO==city_no)
	{
		ret=Read_TianFuTong_PARAM();
	}
   return ret;
}



/*************************************************
*  Function:       // Get_CITY_KeyInfo
*  Description:    // 调用 Read_CITY_Keyfile 获取所有城市秘钥信息
*  Input:          // 无
*                  //
*  Output:         // 无
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
void Get_CITY_KeyInfo(void)
{
	unsigned int i= 0;
	for(i=0;i<CITY_MAX;i++)
	{

		Read_CITY_Keyfile(CITY_NO[i][0],CITY_NO[i][1]);


		Read_CITY_PARAM(CITY_NO[i][1]);
	}
}

/*************************************************
*  Function:       // Get_CITY_QRLib
*  Description:    // 读取ALIPAY_libver 文件 ，获取每个城市对应的 so库版本信息
*  Input:          // 无
*                  //
*  Output:         // 无
*  Return:         // 0 ：成功
*  Others:         // 其它说明
*************************************************/
int Get_CITY_QRLib(void)
{
	unsigned char buf[200]={0};
	char str_buf[200];
	int i=0;
	unsigned int index=0;
	int pos=0;
	int j=0;
	for(i=0;i<CITY_MAX;i++)
	{

		if(memcmp(CITY_LIB_NAME[i],"LIB_TCPS_OPENSSL.so",19)==0)
		{
			memset(str_buf,0x00,sizeof(str_buf));
			TCPS_OPENSSL_LIB_VER((unsigned char*)str_buf,(int *)&index);
			index=CITY_NO[i][0];

			memcpy((unsigned char *)CITY_LIB_VERSION[index],(char *)str_buf,4);
		}
		/*else if(memcmp(CITY_LIB_NAME[i],"libpossdk.so",12)==0)//美团版本固定写死
		{
			index=CITY_NO[i][0];

			CITY_LIB_VERSION[index][0]=0;
			CITY_LIB_VERSION[index][1]=0;
			CITY_LIB_VERSION[index][2]=0;
			CITY_LIB_VERSION[index][3]=0;
			
			if(strlen(MT_VER)>0)
			{
				
				CITY_LIB_VERSION[index][0]=atoi(MT_VER);
				pos=0;	
				for(j=0;j<strlen(MT_VER)-1;j++)		
				{
					if('.'==MT_VER[j])
					{
						pos++;

						if(1==pos)
						{
							CITY_LIB_VERSION[index][1]=atoi(&MT_VER[j+1]);
						}
						else if(2==pos)
						{
							CITY_LIB_VERSION[index][2]=atoi(&MT_VER[j+1]);
						}
						else if(3==pos)
						{
							CITY_LIB_VERSION[index][3]=MT_VER[j+1];
						}
					}
					
					
					
				}
				
			}

		}*/
		else
		{
			memset(str_buf,0x00,sizeof(str_buf));
			sprintf((char*)&str_buf[0],"CITY_%04x_VERSION",CITY_NO[i][1]);
			index=CITY_NO[i][0];

			memset(buf,0,sizeof(buf));
			GetConfigFileStringValue("LIB",str_buf,NULL,(char *)buf,19,"/usr/VD1050/ECDSA/ALIPAY_libver");
			hex_string_to_bytes((char *)buf,8,(unsigned char *)CITY_LIB_VERSION[index],4);
		}



		snprintf((char *)buf,sizeof(buf),"CITY_%04x_libver=%02x%02x.%02x.%02x",CITY_NO[i][1],CITY_LIB_VERSION[index][0],CITY_LIB_VERSION[index][1],CITY_LIB_VERSION[index][2],CITY_LIB_VERSION[index][3]);
		PRINTF(INFO,"%s\n",buf);
		TCPS_LOG(LOG_INFO,(char *)buf);
	}

	return 0;
}

/*************************************************
*  Function:       // change_CITY_TcpsKeyfile
*  Description:    // 将总线上受到的通卡自由码秘钥格式数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*************************************************/
int change_CITY_TcpsKeyfile(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len,data_len;
	unsigned char *file_buff=NULL;
	unsigned char string_buff[200];
	char item[20];

	char str_buf1[200];
	char str_buf2[200];

	cJSON *Key=NULL;
	char* json=NULL;

	ret=-1;

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		goto err;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		goto err;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		goto err;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		ret=-1;
		TCPS_LOG(LOG_ERROR,str_buf1);
		goto err;
	}
	ret=-1;



	Key=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		goto err;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));


	while(offset<file_len)
	{

		offset+=2;// total len
		offset+=8;//
		key_id=(file_buff[offset]<<24)|(file_buff[offset+1]<<16)|(file_buff[offset+2]<<8)|file_buff[offset+3];
		offset+=4;

		key_len=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=2;
		if(key_len>100)
		{
			TCPS_LOG(LOG_ERROR,"key len error");

			goto err;
		}

		memset(string_buff,0x00,200);
		bytes2hex_string(string_buff,&file_buff[offset],key_len);

		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		cJSON_AddItemToObject(Key,item,cJSON_CreateString((char *)string_buff));

		offset+=key_len;

		data_len=(file_buff[offset]<<8)|file_buff[offset+1];// pos key len
		offset+=2;
		offset+=data_len;
	}


	json=cJSON_PrintUnformatted(Key);
	fprintf(newf,"%s",json);



	fflush(newf);

	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x _key success!\n",city_no);
	Get_CITY_KeyInfo();
	ret=0;
err:
	if(oldf!=NULL)
	{
		fclose(oldf);
	}

	if(newf!=NULL)
	{
		fclose(newf);
	}
	if(file_buff!=NULL)
	{
		free(file_buff);
	}
	if(Key!=NULL)
	{
		cJSON_Delete(Key);
	}
	if(json!=NULL)
	{
		free(json);
	}
	return ret;
}



/*************************************************
*  Function:       // Change_MeiTuan_NewKey
*  Description:    // 美团so秘钥文件
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  转换为 /usr/VD1050/CITY_%04x_NEWKEY.txt（Json 格式存储秘钥）
*************************************************/
int Change_MeiTuan_NewKey(unsigned int city_no)
{
    FILE  *oldf=NULL;
	FILE  *newf=NULL;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len,data_len;
	unsigned char *file_buff=NULL;
	unsigned char string_buff[200];
	char item[20];

	char str_buf1[200];
	char str_buf2[200];

	cJSON *Key=NULL;
	cJSON *NewKey=NULL;
	cJSON *Item=NULL;
	char* json=NULL;

	ret=-1;

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/usr/VD1050/CITY_%04x_KEY.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_NEWKEY.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		goto err;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		goto err;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		goto err;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		ret=-1;
		TCPS_LOG(LOG_ERROR,str_buf1);
		goto err;
	}
	ret=-1;


	Key=cJSON_Parse((char *)file_buff);
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		goto err;
	}
	NewKey=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY NewKey Creat Json key error\n");
		cJSON_Delete(Key);
		goto err;
	}

	key_id=0;
	while(key_id<256)
	{

		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		Item=cJSON_GetObjectItem(Key,item);
		if(Item==NULL)
		{
			key_id++;
			continue;
		}

		memset(item,0x00,20);
		sprintf(item,"%02d",key_id);

		if(strlen(Item->valuestring)==130)
		{
			cJSON_AddItemToObject(NewKey,item,cJSON_CreateString((Item->valuestring)+2));
		}
		else
		{
			cJSON_AddItemToObject(NewKey,item,cJSON_CreateString(Item->valuestring));
		}
		key_id++;
	}


	json=cJSON_PrintUnformatted(NewKey);
	fprintf(newf,"%s",json);



	fflush(newf);


	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_NEWKEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_NEWKEY.txt  /usr/VD1050/CITY_%04x_NEWKEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x _newkey success!\n",city_no);
	//Get_CITY_KeyInfo();
	ret=0;
err:
	if(oldf!=NULL)
	{
		fclose(oldf);
	}

	if(newf!=NULL)
	{
		fclose(newf);
	}
	if(file_buff!=NULL)
	{
		free(file_buff);
	}

	if(Key!=NULL)
	{
		cJSON_Delete(Key);
	}
	if(NewKey!=NULL)
	{
		cJSON_Delete(NewKey);
	}

	if(json!=NULL)
	{
		free(json);
	}
	return ret;
}


/*************************************************
*  Function:       // change_CITY_Keyfile
*  Description:    // 将总线上受到的数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*  公钥长度	1
*  公钥编号	1
*  公钥   	n
*************************************************/
int change_CITY_Keyfile(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len;
	unsigned char *file_buff;
	unsigned char string_buff[500];
	char item[20];

	char str_buf1[500];
	char str_buf2[500];


	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		return -1;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		fclose(oldf);
		return -1;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}



	cJSON *Key;
	Key=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));


	while(offset<file_len)
	{

		key_len=file_buff[offset]-1;
		offset++;
		key_id=file_buff[offset];
		offset++;

		if(key_len>(sizeof(string_buff)/2))
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}

		if((key_len+offset)>file_len)
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}

		memset(string_buff,0x00,sizeof(string_buff));
		bytes2hex_string(string_buff,&file_buff[offset],key_len);

		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		cJSON_AddItemToObject(Key,item,cJSON_CreateString((char *)string_buff));

		offset+=key_len;
	}


	char* json=cJSON_PrintUnformatted(Key);
	fprintf(newf,"%s",json);

	cJSON_Delete(Key);
	free(file_buff);
	free(json);

	fflush(newf);
	fclose(newf);
	fclose(oldf);
	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x _key success!\n",city_no);
	Get_CITY_KeyInfo();
	return 0;
}

/*************************************************
*  Function:       // change_CITY_Keyfile2
*  Description:    // 将总线上受到的数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（）
*  文件格式：
*  数据长度	2
*  公钥编号	4
*  有效期		2
*  公钥x长度	2
*  公钥x		n
*  公钥x长度	2
*  公钥x		n
*************************************************/
int change_CITY_Keyfile2(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len;
	unsigned char *file_buff;
	unsigned char string_buff[500];
	char item[20];
	int i=0;
	int j=0;
	int date=0;
	char str_buf1[500];
	char str_buf2[500];


	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		return -1;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		fclose(oldf);
		return -1;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}



	cJSON *Key;
	Key=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));


	while(offset<file_len)
	{


		offset+=2;
		key_id=(file_buff[offset]<<24)|(file_buff[offset+1]<<16)|(file_buff[offset+2]<<8)|file_buff[offset+3];
		offset+=4;
		date=(file_buff[offset]<<8)|file_buff[offset+1];//有效期
		offset+=2;
		key_len=(file_buff[offset]<<8)|file_buff[offset+1];//公钥x
		offset+=2;

		if(key_len>(sizeof(string_buff)/2))
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}

		memset(string_buff,0x00,sizeof(string_buff));
		j=0;

		string_buff[j++]=((date&0x0000f000)>>12)+'0';
		string_buff[j++]=((date&0x00000f00)>>8)+'0';
		string_buff[j++]=((date&0x000000f0)>>4)+'0';
		string_buff[j++]=(date&0x0000000f)+'0';

		string_buff[j++]='0';
		string_buff[j++]='4';
		i=key_len%2;
		if(i)
		{
			string_buff[j++]='0';
			string_buff[j++]='0';
		}
		bytes2hex_string(&string_buff[j],&file_buff[offset],key_len);
		j+=(key_len*2);
		offset+=key_len;

		key_len=(file_buff[offset]<<8)|file_buff[offset+1];//公钥y
		offset+=2;

		if(key_len>(sizeof(string_buff)/2))
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}


		i=key_len%2;
		if(i)
		{
			string_buff[j++]='0';
			string_buff[j++]='0';
		}
		bytes2hex_string(&string_buff[j],&file_buff[offset],key_len);





		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		cJSON_AddItemToObject(Key,item,cJSON_CreateString((char *)string_buff));

		offset+=key_len;
	}


	char* json=cJSON_PrintUnformatted(Key);
	fprintf(newf,"%s",json);

	cJSON_Delete(Key);
	free(file_buff);
	free(json);

	fflush(newf);
	fclose(newf);
	fclose(oldf);
	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);
	PRINTF(INFO,"load CITY_%04x _key success!\n",city_no);
	Get_CITY_KeyInfo();
	return 0;
}



/*************************************************
*  Function:       // change_CITY_Keyfile3
*  Description:    // 将总线上受到的数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*  文件格式：
*  数据长度	2
*  公钥json	n
*************************************************/
int change_CITY_Keyfile3(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	unsigned long json_len;
	int ret,offset;
	int key_id,key_len;
	unsigned char *file_buff=NULL;
	unsigned char *json_buff=NULL;
	unsigned char string_buff[500];
	char item[20];

	char str_buf1[500];
	char str_buf2[500];


	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		return -1;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		fclose(oldf);
		return -1;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	file_buff= (unsigned char*)malloc(file_len);
	if(file_buff==NULL)
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}




	offset=0;
	offset+=sizeof(FILE_HEAD_STRUCT);
	json_len=(file_buff[offset]<<8)|file_buff[offset+1];

	if(((json_len+offset+2)>file_len)||(json_len>1024*2*1024))
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	json_buff= (unsigned char*)malloc(json_len+2);
	if(json_buff==NULL)
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}
	memset(json_buff,0x00,json_len+2);
	memcpy(json_buff,&file_buff[offset+2],json_len);
	cJSON *Key=cJSON_Parse((char *)json_buff);
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		free(file_buff);
		free(json_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));

 	offset+=json_len+2;

	if((offset+2)<=file_len)
	{

		key_id=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=2;

		memset(item,0x00,20);
		sprintf(item,"Time");

		cJSON_AddItemToObject(Key,item,cJSON_CreateNumber(key_id));

	}
	else
	{

		key_id=20000;

		memset(item,0x00,20);
		sprintf(item,"Time");

		cJSON_AddItemToObject(Key,item,cJSON_CreateNumber(key_id));

	}



	char* json=cJSON_PrintUnformatted(Key);
	fprintf(newf,"%s",json);

	cJSON_Delete(Key);
	free(file_buff);
	free(json_buff);
	free(json);

	fflush(newf);
	fclose(newf);
	fclose(oldf);
	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x _key success!\n",city_no);
	Get_CITY_KeyInfo();
	return 0;
}

/*************************************************
*  Function:       // change_CITY_Keyfile4
*  Description:    // 将总线上受到的通卡自由码秘钥格式数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*************************************************/
int change_CITY_Keyfile4(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len,data_len;
	unsigned char *file_buff=NULL;
	unsigned char string_buff[200];
	char item[20];

	char str_buf1[200];
	char str_buf2[200];

	cJSON *Key=NULL;
	char* json=NULL;

	ret=-1;

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		goto err;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		goto err;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		goto err;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		ret=-1;
		TCPS_LOG(LOG_ERROR,str_buf1);
		goto err;
	}
	ret=-1;



	Key=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		goto err;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));


	while(offset<file_len)
	{

		offset+=2;// total len		offset+=8;//
		key_id=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=12;

		key_len=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=2;
		if(key_len>100)
		{
			TCPS_LOG(LOG_ERROR,"key len error");

			goto err;
		}

		memset(string_buff,0x00,200);
		bytes2hex_string(string_buff,&file_buff[offset],key_len);

		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		cJSON_AddItemToObject(Key,item,cJSON_CreateString((char *)string_buff));

		offset+=key_len;

		data_len=(file_buff[offset]<<8)|file_buff[offset+1];// pos key len
		offset+=2;
		offset+=data_len;
	}


	json=cJSON_PrintUnformatted(Key);
	fprintf(newf,"%s",json);



	fflush(newf);

	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x _key success!\n",city_no);
	Get_CITY_KeyInfo();
	ret=0;
err:
	if(oldf!=NULL)
	{
		fclose(oldf);
	}

	if(newf!=NULL)
	{
		fclose(newf);
	}
	if(file_buff!=NULL)
	{
		free(file_buff);
	}
	if(Key!=NULL)
	{
		cJSON_Delete(Key);
	}
	if(json!=NULL)
	{
		free(json);
	}
	return ret;
}


/*************************************************
*  Function:       // change_CITY_MACfile
*  Description:    // 将总线上受到的数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*  公钥长度	1
*  公钥编号	1
*  公钥   	n
*************************************************/
int change_CITY_MACfile(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len;
	unsigned char *file_buff;
	unsigned char string_buff[8192];
	char item[20];

	char str_buf1[500];
	char str_buf2[500];


	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		return -1;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		fclose(oldf);
		return -1;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}



	cJSON *Key;
	Key=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY key Creat Json key error\n");
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));


	while(offset<file_len)
	{

		key_len=file_buff[offset]*256+file_buff[offset+1]-1;
		offset+=2;
		key_id=file_buff[offset];
		offset++;

		if(key_len>sizeof(string_buff))
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}

		if((key_len+offset)>file_len)
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}

		memset(string_buff,0x00,sizeof(string_buff));
		//bytes2hex_string(string_buff,&file_buff[offset],key_len);
		memcpy(string_buff,&file_buff[offset],key_len);

		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		cJSON_AddItemToObject(Key,item,cJSON_CreateString((char *)string_buff));

		offset+=key_len;
	}


	char* json=cJSON_PrintUnformatted(Key);
	fprintf(newf,"%s",json);

	cJSON_Delete(Key);
	free(file_buff);
	free(json);

	fflush(newf);
	fclose(newf);
	fclose(oldf);
	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x_KEY success!\n",city_no);
	Get_CITY_KeyInfo();
	return 0;
}



/*************************************************
*  Function:       // change_JTB_Keyfile
*  Description:    // 将总线上收到的数据进行转换，方便使用（交通部）
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*  文件格式：
*  公钥证书长度	2
*  公钥证书	n
*************************************************/
int change_JTB_Keyfile(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int key_id,key_len;
	unsigned char *file_buff;
	unsigned char string_buff[500];
	char item[20];

	char str_buf1[500];
	char str_buf2[500];

	unsigned char SIGN[100];
	unsigned int  SIGN_len=0;
	unsigned char sm3_digest[32]={0};
	char KEY[200];


	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_key_back_over.txt",city_no);

	memset(str_buf2,0x00,sizeof(str_buf2));
	sprintf((char*)&str_buf2[0],"/opt/CITY_%04x_key_update.txt",city_no);

	oldf=fopen(str_buf1,"rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		return -1;
	}

	newf=fopen(str_buf2,"w+");
	if(newf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf2);
		fclose(oldf);
		return -1;
	}


	fseek(oldf,0,SEEK_SET);
	fseek(newf,0,SEEK_SET);
	file_len=get_file_size(str_buf1);

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*2*1024))
	{
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	file_buff= (unsigned char*)malloc(file_len);


	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}



	cJSON *Key;
	Key=cJSON_CreateObject();
 	if(Key==NULL)
	{
		A_PRINTF(ERROR,"update CITY Key Creat Json key error\n");
		free(file_buff);
		fclose(newf);
		fclose(oldf);
		return -1;
	}

	//write head
	offset=0;
	bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));
	offset+=sizeof(FILE_HEAD_STRUCT);
	cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));


	while(offset<file_len)
	{

		key_len=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=2;
		key_id=file_buff[offset+10];



		if(key_len>(sizeof(string_buff)/2))
		{
			TCPS_LOG(LOG_ERROR,"key len error");
			cJSON_Delete(Key);
			free(file_buff);
			fclose(newf);
			fclose(oldf);
			return -1;
		}

		//检测根公钥合法性
		if(0x21!=file_buff[offset])
		{
			A_PRINTF(ERROR,"JTB CA head error\n");
			offset+=key_len;
			continue;
		}

		if(0x04!=file_buff[offset+13])
		{
			A_PRINTF(ERROR,"JTB CA  isn't  SM2\n");
			offset+=key_len;
			continue;
		}

		if(0x40!=file_buff[offset+15])
		{
			A_PRINTF(ERROR,"JTB CA  len %d \n",file_buff[offset+15]);
			offset+=key_len;
			continue;
		}

		//签名认证

		//SET_SIGN_DATA(&file_buff[offset+80],64,(unsigned char *)SIGN,(unsigned int *)&SIGN_len);

		TCPS_SM3(&file_buff[offset+16],&file_buff[offset+16+32],&file_buff[offset],80,sm3_digest);

		memset(KEY,0,sizeof(KEY));
		KEY[0]='0';
		KEY[1]='4';
		bytes2hex_string((unsigned char *)&KEY[2],&file_buff[offset+16],64);

		ret=TCPS_SM2_verify2(&KEY[0],sm3_digest,32,SIGN,SIGN_len);
		if(ret!=0)
		{
			A_PRINTF(ERROR,"JTB CA  verify failed\n");
			offset+=key_len;
			continue;
		}



		memset(string_buff,0x00,sizeof(string_buff));
		bytes2hex_string(string_buff,&file_buff[offset],key_len);

		memset(item,0x00,20);
		sprintf(item,"PubKey_%d",key_id);

		cJSON_AddItemToObject(Key,item,cJSON_CreateString((char *)string_buff));

		offset+=key_len;
	}


	char* json=cJSON_Print(Key);
	fprintf(newf,"%s",json);

	cJSON_Delete(Key);
	free(file_buff);
	free(json);

	fflush(newf);
	fclose(newf);
	fclose(oldf);
	remove(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/CITY_%04x_KEY.txt",city_no);

	tk_system(str_buf1);

	memset(str_buf1,0x00,sizeof(str_buf1));
	sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_key_update.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

	tk_system(str_buf1);

	remove(str_buf2);

	tk_system("sync");
	PRINTF(INFO,"load CITY_%04x _key success!\n",city_no);
	Get_CITY_KeyInfo();
	return 0;
}






/*************************************************
*  Function:       // change_CITY_CAfile
*  Description:    // 将总线上的数据转成CA文件，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/cert/xxx
*************************************************/
int change_CITY_file(unsigned int city_no)
{
    FILE  *oldf,*newf;
	unsigned long file_len;
	int ret,offset;
	int body_len;
	int name_len;
	int file_no,i;
	unsigned char *file_buff;
	unsigned char *file_name[100];
	unsigned char string_buff[200];
	unsigned short File_CRC=0;
	unsigned short CRC=0;
	char str_buf1[200];
	char str_buf2[200];
	char Version[20]={0};

	if(access("/usr/VD1050/cert/",0)==-1)
	{
		A_PRINTF(ERROR,"mkdir cert \n");
		tk_system("wr mkdir /usr/VD1050/cert/");
	}


	oldf=fopen("/opt/CITY_file_back_over.txt","rb");
	if(oldf==NULL)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		return -1;
	}


	fseek(oldf,0,SEEK_SET);

	file_len=get_file_size("/opt/CITY_file_back_over.txt");

	if((file_len<=sizeof(FILE_HEAD_STRUCT))||(file_len>1024*20*1024))
	{
		fclose(oldf);
		return -1;
	}

	file_buff=NULL;
	file_buff= (unsigned char*)malloc(file_len);
	if(file_buff==NULL)
	{
		fclose(oldf);
		return -1;
	}

	ret=fread(file_buff,file_len,1,oldf);
	if(ret==0)
	{
		TCPS_LOG(LOG_ERROR,str_buf1);
		free(file_buff);
		fclose(oldf);
		return -1;
	}




	offset=0;
	offset+=sizeof(FILE_HEAD_STRUCT);

	file_no=0;

	while(offset<file_len)
	{
		name_len=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=2;

		file_name[file_no]=NULL;
		file_name[file_no]= (unsigned char*)malloc(name_len+2);
		if(file_buff==NULL)
		{
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			return -1;
		}
		file_no++;




		memset(file_name[file_no-1],0x00,name_len+2);
		memcpy(file_name[file_no-1],&file_buff[offset],name_len);
		offset+=name_len;

		if(name_len!=strlen((char *)file_name[file_no-1])) //判断文件名长度是否正确
		{
			A_PRINTF(ERROR,"File name len error\n");
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			return -1;
		}


	 	memset(str_buf2,0x00,sizeof(str_buf2));
		sprintf((char*)&str_buf2[0],"/opt/%s",file_name[file_no-1]);

		newf=fopen(str_buf2,"wb+");
		if(newf==NULL)
		{
			TCPS_LOG(LOG_ERROR,str_buf2);
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			return -1;
		}
		fseek(newf,0,SEEK_SET);

		body_len=(file_buff[offset]<<8)|file_buff[offset+1];
		offset+=2;///跳过体长度


		if(body_len>(file_len-offset)) //判断文件体长度是否正确
		{
			A_PRINTF(ERROR,"File body len error\n");
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			fclose(newf);
			return -1;
		}
		File_CRC=0;
		File_CRC=Crc162(File_CRC,(unsigned char *)&file_buff[offset],body_len-2);
		CRC=(unsigned short)((file_buff[offset+body_len-2]<<8)|file_buff[offset+body_len-1]);
		if(File_CRC!=CRC)
		{
			A_PRINTF(ERROR,"File body crc error\n");
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			fclose(newf);
			return -1;
		}


		fwrite((unsigned char *)&file_buff[offset],body_len-2,1,newf);
		offset+=body_len;

		fflush(newf);
		fclose(newf);

	}

	fclose(oldf);
	remove("/opt/CITY_file_back_over.txt");//删除临时文件

	if((file_name[0]!=NULL)&&((strstr((char *)file_name[0],".cer")!=NULL)||(strstr((char *)file_name[0],".crt")!=NULL)||(strstr((char *)file_name[0],".pem")!=NULL)))
	{////KEY 文件 版本更新
		//printf("update  ker version\n");
		memset(str_buf1,0x00,sizeof(str_buf1));
		sprintf((char*)&str_buf1[0],"/opt/CITY_%04x_KEY.txt",city_no);


		oldf=fopen(str_buf1,"w+");
		if(oldf==NULL)
		{
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			return -1;
		}
		fseek(oldf,0,SEEK_SET);

		cJSON *Key;
		Key=cJSON_CreateObject();
	 	if(Key==NULL)
		{
			A_PRINTF(ERROR,"update CITY File Creat Json key error\n");
			free(file_buff);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			return -1;
		}

		//write head

		bytes2hex_string(string_buff,file_buff,sizeof(FILE_HEAD_STRUCT));

		cJSON_AddItemToObject(Key,"Key-Head",cJSON_CreateString((char *)string_buff));
		cJSON_AddItemToObject(Key,"FileName",cJSON_CreateString((char *)file_name[0]));
		char* json=cJSON_PrintUnformatted(Key);
		if(json==NULL)
		{
			A_PRINTF(ERROR,"update CITY File Creat Json error\n");
			free(file_buff);
			cJSON_Delete(Key);
			for(i=0;i<file_no;i++)
			{
				free(file_name[i]);
			}
			fclose(oldf);
			return -1;
		}
		fprintf(oldf,"%s",json);

		cJSON_Delete(Key);
		free(json);
		fflush(oldf);
		fclose(oldf);


		memset(str_buf1,0x00,sizeof(str_buf1));
		sprintf((char*)&str_buf1[0],"wr cp -f /opt/CITY_%04x_KEY.txt  /usr/VD1050/CITY_%04x_KEY.txt",city_no,city_no);

		tk_system(str_buf1);
	}
	else if((file_name[0]!=NULL)&&(strstr((char *)file_name[0],".so")!=NULL))
	{
		////SDK 文件 版本更新
		memset(str_buf1,0x00,sizeof(str_buf1));

		sprintf((char*)&str_buf1[0],"CITY_%04x_VERSION",city_no);

		bytes2hex_string((unsigned char *)Version,file_buff+4,4);

		SetConfigFileStringValue("LIB",str_buf1,Version,8,"/usr/VD1050/ECDSA/ALIPAY_libver");
	}





	free(file_buff);


	for(i=0;i<file_no;i++)
	{
		if((strstr((char *)file_name[i],".cer")!=NULL)||(strstr((char *)file_name[i],".crt")!=NULL)||(strstr((char *)file_name[0],".pem")!=NULL))
		{
			//memset(str_buf1,0x00,sizeof(str_buf1));
			//sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/cert/%s",file_name[i]);

			//tk_system(str_buf1);

			memset(str_buf1,0x00,sizeof(str_buf1));
			sprintf((char*)&str_buf1[0],"wr cp -f /opt/%s  /usr/VD1050/cert/%s",file_name[i],file_name[i]);
			//printf("%s \n",str_buf1);
			tk_system(str_buf1);
		}
		else if(strstr((char *)file_name[i],".so")!=NULL)
		{
			memset(str_buf1,0x00,sizeof(str_buf1));
			sprintf((char*)&str_buf1[0],"wr rm  /usr/VD1050/ECDSA/%s",file_name[i]);
			tk_system(str_buf1);


			memset(str_buf1,0x00,sizeof(str_buf1));
			sprintf((char*)&str_buf1[0],"wr cp  /opt/%s  /usr/VD1050/ECDSA/%s",file_name[i],file_name[i]);
			tk_system(str_buf1);

		}
		else if(strstr((char *)file_name[i],"KEY.txt")!=NULL)
		{
			//memset(str_buf1,0x00,sizeof(str_buf1));
			//sprintf((char*)&str_buf1[0],"wr rm -f /usr/VD1050/%s",file_name[i]);

			//tk_system(str_buf1);
			//pthread_mutex_lock(&mutex_FILE_KEY);
			memset(str_buf1,0x00,sizeof(str_buf1));
			sprintf((char*)&str_buf1[0],"wr cp -f /opt/%s  /usr/VD1050/%s",file_name[i],file_name[i]);
			tk_system(str_buf1);
			//pthread_mutex_unlock(&mutex_FILE_KEY);
		}


		memset(str_buf2,0x00,sizeof(str_buf2));
		sprintf((char*)&str_buf2[0],"/opt/%s",file_name[i]);
		remove(str_buf2);
		PRINTF(INFO,"load %s success!\n",file_name[i]);

		free(file_name[i]);
	}

	tk_system("sync");
	Get_CITY_KeyInfo();
	return 0;
}




/*************************************************
*  Function:       // GET_CITY_KEY
*  Description:    // 根据城市代码及 公钥ID 获取相应秘钥
*  Input:          //  id		:公钥ID
*                  //  city_no	:2字节 16进制  城市代码
*  Output:         //  *key	  	:公钥 字符串
*  Return:         // －1：失败  0 : 成功
*  Others:         // 公钥存储位置 /usr/VD1050/CITY_城市代码__KEY.txt
*************************************************/
int GET_CITY_KEY(int id,char *key,unsigned int city_no)
{
	FILE  *df;
	int ret;
	char str_buf[200];
	unsigned long file_len;
	unsigned char *file_buff;

	char item[20];

	memset(str_buf,0x00,sizeof(str_buf));
	sprintf((char*)&str_buf[0],"/usr/VD1050/CITY_%04x_KEY.txt",city_no);



	df=fopen(str_buf,"r");
	if(df==NULL)
	{
		perror(str_buf);
		return -1;
	}

	fseek(df,0,SEEK_SET);

	file_len=get_file_size(str_buf);
	file_buff= (unsigned char*)malloc(file_len);
	ret=fread(file_buff,file_len,1,df);
	if(ret==0)
	{
		perror(str_buf);
		free(file_buff);
		fclose(df);
		return -1;
	}

	cJSON *Key=cJSON_Parse((char *)file_buff);
	if(Key==NULL)
	{
		A_PRINTF(ERROR,"Get CITY Key Creat Json key error\n");
		free(file_buff);
		fclose(df);
		return -1;
	}


	memset(item,0x00,20);
	sprintf(item,"PubKey_%d",id);

	cJSON *Item=cJSON_GetObjectItem(Key,item);
	if(Item==NULL)
	{
		A_PRINTF(ERROR,"read CITY Key %s error\n",item);
		cJSON_Delete(Key);
		free(file_buff);
		fclose(df);
		return -1;
	}

	char* json=Item->valuestring;

	memcpy(key,json,strlen(json));

	cJSON_Delete(Key);
	free(file_buff);
	fclose(df);
	//free(json);
	return 0;
}




