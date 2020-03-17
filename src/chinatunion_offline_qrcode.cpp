/************************************************************************
* 版权所有 (C)2019, 天津通卡智能网络科技股份有限公司。
*
* 文件名称： // chinatunion_offline_qrcode.cpp
* 文件标识： //
* 内容摘要： // 交通部脱机二维码刷码实现
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
#if 0
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
#include "cJSON.h"
#include "public.h"
#include "qrcode.h"
#include "chinatunion_offline_qrcode.h"
#include "tcpsrecord.h"
#include "sounddefine.h"
// #include "lib_tcps_openssl.h"
#include "LIB_TCPS_OPENSSL.h"
#include "manage.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "VD9100_QR"





CHAR  CHINATUNION_V1_OFFLINE_QRCODE_ENABLE=0;     //1 功能使能
CHAR  CHINATUNION_V1_OFFLINE_QRCODE_TYPE=0;     //
SDWORD  CHINATUNION_V1_OFFLINE_QRCODE_SHAKE=2;		//2s  防抖动间隔
SDWORD  CHINATUNION_V1_OFFLINE_QRCODE_INTERVAL=0;	//0s 刷码间隔

CHAR  CHINATUNION_V2_OFFLINE_QRCODE_ENABLE=0;     //1 功能使能
CHAR  CHINATUNION_V2_OFFLINE_QRCODE_TYPE=0;     //
SDWORD  CHINATUNION_V2_OFFLINE_QRCODE_SHAKE=2;		//2s  防抖动间隔
SDWORD  CHINATUNION_V2_OFFLINE_QRCODE_INTERVAL=0;	//0s 刷码间隔



extern ManageParam *qrcode_Manage; // 管理参数


/**************************************************************************
* 函数名称： SDWORD ChinaTunionOfflineQRCodeInit(void)
* 功能描述： 交通部离线二维码功能初始化
* 访问的表：
* 修改的表：
* 输入参数：
* 输出参数：
* 返 回 值： -1 失败  0成功
* 其它说明：
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl
**************************************************************************/
SDWORD ChinaTunionOfflineQRCodeInit(void)
{
	return 0;
}


/**************************************************************************
* 函数名称： static SDWORD ChinaTunionReadKey(const CHAR *pKeyFile,DWORD dwKeyId,CHAR *pcKey)
* 功能描述： 交通部  密钥读取
* 访问的表：
* 修改的表：
* 输入参数： pcKeyId 密钥索引
*			pKeyFile 文件路径
* 输出参数： pcKey  密钥
* 返 回 值： -1 失败  0成功
* 其它说明：
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl
**************************************************************************/
static SDWORD ChinaTunionReadKey(const CHAR *pKeyFile,const CHAR  *pcKeyId,CHAR *pcKey)
{
	FILE  *df;
	SDWORD sdwRet;

	LLONG llFileLen;
	BYTE  *pucFileBuf;

	CHAR cItem[20];


	df=fopen(pKeyFile,"r");
	if(df==NULL)
	{
		perror(pKeyFile);
		return -1;
	}

	fseek(df,0,SEEK_SET);

	llFileLen=GetFileSize(pKeyFile);
	pucFileBuf= (BYTE *)malloc(llFileLen);
	sdwRet=fread(pucFileBuf,llFileLen,1,df);
	if(sdwRet==0)
	{
		free(pucFileBuf);
		fclose(df);
		return -1;
	}

	cJSON *Key=cJSON_Parse((CHAR *)pucFileBuf);
	if(Key==NULL)
	{
		free(pucFileBuf);
		fclose(df);
		return -1;
	}


	memset(cItem,0x00,20);
	sprintf(cItem,"%s",pcKeyId);  ///debug_info  密钥存储方式

	cJSON *Item=cJSON_GetObjectItem(Key,cItem);
	if(Item==NULL)
	{
		cJSON_Delete(Key);
		free(pucFileBuf);
		fclose(df);
		return -1;
	}


	memcpy(pcKey,Item->valuestring,strlen(Item->valuestring));
	cJSON_Delete(Key);
	free(pucFileBuf);
	fclose(df);
	return 0;
}


/**************************************************************************
* 函数名称： SDWORD ChinaTunionOfflineQRCodeV1(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT)
* 功能描述： 交通部离线二维码验签
* 访问的表：
* 修改的表：
* 输入参数： pucQRCode 		二维码
*			sdwQRCodeLen   二维码长度
* 输出参数： OUT			结果输出
* 返 回 值： 错误类型
* 其它说明：
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl
**************************************************************************/
SDWORD ChinaTunionOfflineQRCodeV1(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT)
{

	static  BYTE s_ucLastSha[20]={0};
	BYTE ucNowSha[20]={0};
	CHAR cSound[300]={0};
	CHAR cUserId[30]={0};
	SDWORD sdwRet = 0;
	SDWORD i = 0;
	SDWORD sdwLength=0;
	SDWORD sdwSec=0;
	DWORD  dwKeyId=0;

	BYTE ucSign[100];
	SDWORD  sdwSignLen=0;
	BYTE ucSm3Digest[32]={0};
	CHAR cPublicKeyStr[500];
	BYTE ucPublicKeyByte[300];
	CHAR cKey[200];
	CHAR cY[100];
	BYTE ucXY[300];
	

	time_t now;
	struct tm *tm_now;
	struct timeval tv;

	if(0==CHINATUNION_V1_OFFLINE_QRCODE_ENABLE)
	{
		PRINTF(ERROR,"交通部V1二维码功能未启用\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码功能未启用");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_wuxiaoma);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}

	if(sdwQRCodeLen!=226)
	{
		PRINTF(ERROR,"交通部V1二维码长度错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码长度错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}
	if((pucQRCode[0]<0x80)&&(pucQRCode[0]!=0x40))
	{
		PRINTF(ERROR,"交通部V1二维码版本错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码版本错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}
	if((pucQRCode[3]!=0x00)&&(pucQRCode[3]!=0x01))
	{
		PRINTF(ERROR,"交通部V1二维码类型错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码类型错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}



	time(&now);
	tm_now=localtime(&now);


	memset(cUserId,0,sizeof(cUserId));
	BytesToHexString(cUserId,sizeof(cUserId),(BYTE  *)&pucQRCode[34],10);
	cJSON_ReplaceStr(OUT,"CardNo",cUserId);


	gettimeofday(&tv,NULL);

	//支付帐号授权时间有效性判断
	/*
	sdwSec=(pucQRCode[84]<<24)|(pucQRCode[85]<<16)|(pucQRCode[86]<<8)|pucQRCode[87];
	if(sdwSec<tv.tv_sec)
	{
		PRINTF(ERROR,"交通部V1二维码授权时间错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码授权时间错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_OVERDUE;
	}
*/

	//二维码生成时间判断
	sdwSec=(pucQRCode[157]<<24)|(pucQRCode[158]<<16)|(pucQRCode[159]<<8)|pucQRCode[160];
	if((sdwSec-20)>tv.tv_sec)
	{
		PRINTF(ERROR,"交通部V1二维码时间错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码时间错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_OVERDUE;
	}

	//二维码有效期判断
	if((sdwSec+(pucQRCode[88]<<8)+pucQRCode[89])<tv.tv_sec)
	{
		PRINTF(ERROR,"交通部V1二维码已过期 pucQRCode time:%d ,%d nowtime:%d\n",sdwSec,((pucQRCode[88]<<8)+pucQRCode[89]),(int)tv.tv_sec);
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码已过期");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_OVERDUE;
	}



	memset(cY,0,sizeof(cY));
	memset(cPublicKeyStr,0,sizeof(cPublicKeyStr));
	memset(ucPublicKeyByte,0,sizeof(ucPublicKeyByte));


	//dwKeyId=(pucQRCode[4]<<24)|(pucQRCode[5]<<16)|(pucQRCode[6]<<8)|pucQRCode[7];
	CHAR keyid[20]={0};
	snprintf(keyid,sizeof(keyid),"%02x%02x%02x%02x",pucQRCode[4],pucQRCode[5],pucQRCode[6],pucQRCode[7]);
	sdwRet=ChinaTunionReadKey(CHINATUNION_V1_OFFLINE_QRCODE_KEY_PATH,keyid,cPublicKeyStr);
	if(sdwRet!=0)
	{
		PRINTF(ERROR,"交通部V1二维码密钥错误%s\n",keyid);
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码密钥错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}




	//签名认证
	HexStringToBytes(cPublicKeyStr,strlen(cPublicKeyStr),ucPublicKeyByte,strlen(cPublicKeyStr)/2);

	SetSignData(&pucQRCode[91],64,(BYTE  *)ucSign,(unsigned int *)&sdwSignLen);

	TcpsSm3(&ucPublicKeyByte[0],&ucPublicKeyByte[32],&pucQRCode[10],80,ucSm3Digest);

	memset(cKey,0,sizeof(cKey));
	cKey[0]='0';
	cKey[1]='4';
	memcpy(&cKey[2],&cPublicKeyStr[0],128);

	sdwRet=Sm2Verify2(&cKey[0],ucSm3Digest,32,ucSign,sdwSignLen);
	if(sdwRet!=0)
	{
		PRINTF(ERROR,"交通部V1二维码发卡机构签名错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码发卡机构签名错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}



	memset(cPublicKeyStr,0,sizeof(cPublicKeyStr));
	memset(cY,0,sizeof(cY));
	BytesToHexString((char  *)cPublicKeyStr,sizeof(cPublicKeyStr),&pucQRCode[51],33);



	Sm2GetY(cPublicKeyStr,&cY[0],sizeof(cY));
	i=strlen(cY);
	if((i>64)||(i<60))
	{
		PRINTF(ERROR,"交通部V1二维码发用户公钥错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码发用户公钥错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}

	memset(&cPublicKeyStr[66],0x30,64);
	memcpy(&cPublicKeyStr[66+64-i],&cY[0],i);
	cPublicKeyStr[0]='0';
	cPublicKeyStr[1]='4';


	//签名认证
	SetSignData(&pucQRCode[162],64,(BYTE  *)ucSign,(unsigned int *)&sdwSignLen);

	HexStringToBytes(&cPublicKeyStr[2],128,ucXY,64);

	TcpsSm3(&ucXY[0],&ucXY[32],&pucQRCode[8],153,ucSm3Digest);

	sdwRet=Sm2Verify2(cPublicKeyStr,ucSm3Digest,32,ucSign,sdwSignLen);
	if(sdwRet!=0)
	{
		PRINTF(ERROR,"交通部V1二维码用户签名错误\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码用户签名错误");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}

	
	sdwRet=SearchQRCodeRecord("TK_EWMPAY_TRADE_TODAY",cUserId,CHINATUNION_V1_OFFLINE_QRCODE_SHAKE);
	if(1==sdwRet)
	{
		PRINTF(ERROR,"交通部V1二维码抖动连刷\n");
		return ERROR_QRCODE_FIND;
	}

	sha1(pucQRCode,sdwQRCodeLen,ucNowSha);
	if(memcmp(ucNowSha,s_ucLastSha,20)==0)
	{
		PRINTF(ERROR,"交通部V1二维码重复\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码重复");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_DUPLICATED;
	}

	memcpy(s_ucLastSha,ucNowSha,20);

	sdwRet=SearchQRCodeRecord("TK_EWMPAY_TRADE_TODAY",cUserId,CHINATUNION_V1_OFFLINE_QRCODE_INTERVAL);
	if(1==sdwRet)
	{
		PRINTF(ERROR,"交通部V1二维码禁止带人\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V1二维码禁止带人");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_RFU;
	}


	CHAR cPay[10];
	memset(cPay,0,sizeof(cPay));
	snprintf(cPay,sizeof(cPay),"%d",qrcode_Manage->BasePrice);
	cJSON_ReplaceStr(OUT,"PayType","6");
	cJSON_ReplaceStr(OUT,"Pay",cPay);
	cJSON_ReplaceStr(OUT,"DispSwitch","1");
	cJSON_ReplaceStr(OUT,"DispInfo","SUCCESS");
	memset(cSound,0,sizeof(cSound));
	snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"800\",\"Break\":\"0\"}]",SOUNDFILE_shuamachenggong);
	cJSON_ReplaceArray(OUT,"Sound",cSound);

	
	TKRecord record;
    T_RecordStruct table = record.GetRecordStruct((char *)"TK_EWMPAY_TRADE_TODAY");
    record.TagValue(&table, (CHAR *)"CARD_NO", cUserId);
	record.TagValue(&table, (CHAR *)"ewm_type", "0");
    record.TagValue(&table, (CHAR *)"ewmpay_data1", pucQRCode, sdwQRCodeLen);
    record.TagValue(&table, (CHAR *)"ewm_type", "0");
    record.TagDate(&table, (CHAR *)"RIDING_TIME", tv.tv_sec);
    record.TagValue(&table, (CHAR *)"PAID", cPay);
	record.TagValue(&table, (CHAR *)"region_code", qrcode_Manage->RegionNo.c_str());     // 消费区域号    REGION_CODE    NUMBER    2
    record.TagValue(&table, (CHAR *)"voca_code", qrcode_Manage->IndustryNo.c_str());     // 消费行业号    VOCA_CODE    NUMBER    2
    record.TagValue(&table, (CHAR *)"dept_no", qrcode_Manage->CompanyNo.c_str());        // 消费公司号    DEPT_NO    NUMBER    8
    record.TagValue(&table, (CHAR *)"group_no", qrcode_Manage->TeamNo.c_str());          // 消费路队号    GROUP_NO    NUMBER    6
    record.TagValue(&table, (CHAR *)"line_no", qrcode_Manage->LineNo.c_str());           // 消费线路号    LINE_NO    NUMBER    6
    record.TagValue(&table, (CHAR *)"bus_no", qrcode_Manage->BusNo.c_str());             // 消费车辆号    BUS_NO    NUMBER    8
    record.TagValue(&table, (CHAR *)"mach_no", qrcode_Manage->PosID.c_str());            // 消费设备号    MACH_NO    NUMBER    8
   	record.TagValue(&table, (CHAR *)"condcard_no", qrcode_Manage->SellerSerial.c_str()); // 售票员卡号    CONDCARD_NO    VARCHAR2    16  
	record.TagValue(&table, (CHAR *)"opcard_no", qrcode_Manage->DriverSerial.c_str());   // 司机卡号    OPCARD_NO    VARCHAR2    16
    record.TagDate(&table, (CHAR *)"reach_time", qrcode_Manage->s_SignedUtc);            // 司机上班时间    REACH_TIME    DATE
	record.TagValue(&table, (CHAR *)"charge_type", "0");
    record.InsertRecord(table);
	

	PRINTF(INFO,"ChinaTunion V1 QRCode check successed\n");
	PRINTF(INFO,"===========End================\n");

	return QRCODE_SUCCESS;
}



/**************************************************************************
* 函数名称： SDWORD ChinaTunionOfflineQRCodeV2(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT)
* 功能描述： 交通部离线二维码验签
* 访问的表：
* 修改的表：
* 输入参数： pucQRCode 		二维码
*			sdwQRCodeLen   二维码长度
* 输出参数： OUT			结果输出
* 返 回 值： 错误类型
* 其它说明：
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl
**************************************************************************/
SDWORD ChinaTunionOfflineQRCodeV2(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT)
{

	static  BYTE s_ucLastSha[20]={0};
	BYTE ucNowSha[20]={0};
	CHAR cSound[300]={0};
	CHAR cUserId[30]={0};
	SDWORD sdwRet = 0;
	SDWORD i = 0;
	SDWORD sdwLength=0;
	SDWORD sdwSec=0;

	BYTE ucSign[100];
	SDWORD  sdwSignLen=0;
	BYTE ucSm3Digest[32]={0};
	CHAR cPublicKeyStr[500];
	BYTE ucPublicKeyByte[300];
	CHAR cKey[200];
	CHAR cY[100];
	BYTE ucXY[300];

	time_t now;
	struct tm *tm_now;
	struct timeval tv;

	if(0==CHINATUNION_V2_OFFLINE_QRCODE_ENABLE)
	{
		PRINTF(ERROR,"交通部V2二维码功能未启用\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V2二维码功能未启用");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_wuxiaoma);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}


	if(sdwQRCodeLen>364)
	{
		PRINTF(ERROR,"交通部二维码错误0005498\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码错误0005498");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}
	if(sdwQRCodeLen<332)
	{
		PRINTF(ERROR,"交通部二维码错误0005728\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码错误0005728");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}


	if(pucQRCode[0]<0x80)
	{
		PRINTF(ERROR,"交通部二维码格式错误0005499\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码格式错误0005499");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}
	sdwLength=((pucQRCode[1]<<8)|pucQRCode[2]);

	if((sdwLength+3)!=sdwQRCodeLen)
	{
		PRINTF(ERROR,"交通部二维码格式错误0005500\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码格式错误0005500");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_FORMAT;
	}

	if(pucQRCode[3]!=0x24)
	{
		PRINTF(ERROR,"交通部二维码发卡机构证书错误0005586\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构证书错误0005586");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_RFU;
	}

	time(&now);
	tm_now=localtime(&now);


	if((tm_now->tm_year+1900)>(2000+((pucQRCode[15]&0xf0)>>4)*10+(pucQRCode[15]&0x0f)))
	{
		PRINTF(ERROR,"交通部二维码发卡机构证书错误0005586\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构证书错误0005586");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_RFU;
	}
	else if((tm_now->tm_year+1900)==(2000+((pucQRCode[15]&0xf0)>>4)*10+(pucQRCode[15]&0x0f)))
	{
		if((tm_now->tm_mon+1)>(((pucQRCode[14]&0xf0)>>4)*10+(pucQRCode[14]&0x0f)))
		{
			PRINTF(ERROR,"交通部二维码发卡机构证书错误0005586\n");
			cJSON_ReplaceStr(OUT,"DispSwitch","1");
			cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构证书错误0005586");
			memset(cSound,0,sizeof(cSound));
			snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
			cJSON_ReplaceArray(OUT,"Sound",cSound);

			return ERROR_QRCODE_RFU;
		}
	}


	memset(cUserId,0,sizeof(cUserId));
	BytesToHexString(cUserId,sizeof(cUserId),(BYTE  *)&pucQRCode[136],10);
	cJSON_ReplaceStr(OUT,"CardNo",cUserId);



	memset(cY,0,sizeof(cY));
	memset(cPublicKeyStr,0,sizeof(cPublicKeyStr));
	memset(ucPublicKeyByte,0,sizeof(ucPublicKeyByte));

	CHAR keyid[20]={0};
	snprintf(keyid,sizeof(keyid),"%d",pucQRCode[8]);

	sdwRet=ChinaTunionReadKey(CHINATUNION_V2_OFFLINE_QRCODE_KEY_PATH,keyid,cPublicKeyStr);
	if(sdwRet!=0)
	{
		PRINTF(ERROR,"交通部二维码密钥错误%s\n",keyid);
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码密钥错误0005586");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}

	if(strlen(cPublicKeyStr)>130)
	{
		//检查根公钥有效性
		HexStringToBytes(cPublicKeyStr,strlen(cPublicKeyStr),ucPublicKeyByte,strlen(cPublicKeyStr)/2);

		PRINTF(DEBUG,"SYS_TIME:%04d-%02d-%02d %02d:%02d:%02d\n",tm_now->tm_year+1900,tm_now->tm_mon+1,tm_now->tm_mday,tm_now->tm_hour,tm_now->tm_min,tm_now->tm_sec);

		if(DF_HEX2BCD((tm_now->tm_year+1900)%100)>ucPublicKeyByte[12])
		{
			PRINTF(ERROR,"交通部二维码根CA过期0005586\n");
			cJSON_ReplaceStr(OUT,"DispSwitch","1");
			cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码根CA过期0005586");
			memset(cSound,0,sizeof(cSound));
			snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
			cJSON_ReplaceArray(OUT,"Sound",cSound);
			return ERROR_QRCODE_OVERDUE;

		}
		else if((DF_HEX2BCD((tm_now->tm_year+1900)%100)==ucPublicKeyByte[12])&&(DF_HEX2BCD(tm_now->tm_mon+1)>ucPublicKeyByte[11]))
		{
			PRINTF(ERROR,"交通部二维码根CA过期0005586\n");
			cJSON_ReplaceStr(OUT,"DispSwitch","1");
			cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码根CA过期0005586");
			memset(cSound,0,sizeof(cSound));
			snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
			cJSON_ReplaceArray(OUT,"Sound",cSound);
			return ERROR_QRCODE_OVERDUE;
		}


		//签名认证

		SetSignData(&pucQRCode[3+53],64,(BYTE  *)ucSign,(unsigned int *)&sdwSignLen);

		TcpsSm3(&ucPublicKeyByte[16],&ucPublicKeyByte[16+32],&pucQRCode[9],47,ucSm3Digest);

		memset(cKey,0,sizeof(cKey));
		cKey[0]='0';
		cKey[1]='4';
		memcpy(&cKey[2],&cPublicKeyStr[32],128);

		sdwRet=Sm2Verify2(&cKey[0],ucSm3Digest,32,ucSign,sdwSignLen);
		if(sdwRet!=0)
		{
			PRINTF(ERROR,"交通部二维码发卡机构证书错误0005586\n");
			cJSON_ReplaceStr(OUT,"DispSwitch","1");
			cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构证书错误0005586");
			memset(cSound,0,sizeof(cSound));
			snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
			cJSON_ReplaceArray(OUT,"Sound",cSound);

			return ERROR_QRCODE_VERIFY;
		}


	}
	else
	{
		//签名认证
		HexStringToBytes(cPublicKeyStr,strlen(cPublicKeyStr),ucPublicKeyByte,strlen(cPublicKeyStr)/2);

		SetSignData(&pucQRCode[3+53],64,(BYTE  *)ucSign,(unsigned int *)&sdwSignLen);

		TcpsSm3(&ucPublicKeyByte[0],&ucPublicKeyByte[32],&pucQRCode[9],47,ucSm3Digest);

		memset(cKey,0,sizeof(cKey));
		cKey[0]='0';
		cKey[1]='4';
		memcpy(&cKey[2],&cPublicKeyStr[0],128);

		sdwRet=Sm2Verify2(&cKey[0],ucSm3Digest,32,ucSign,sdwSignLen);
		if(sdwRet!=0)
		{
			PRINTF(ERROR,"交通部二维码发卡机构证书错误0005586\n");
			cJSON_ReplaceStr(OUT,"DispSwitch","1");
			cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构证书错误0005586");
			memset(cSound,0,sizeof(cSound));
			snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
			cJSON_ReplaceArray(OUT,"Sound",cSound);

			return ERROR_QRCODE_VERIFY;
		}

	}

	for(i=0;i<16;i++)
	{
		if((pucQRCode[120+i]<0x20)||(pucQRCode[120+i]>0x7e))
		{
			PRINTF(ERROR,"交通部二维码支付账户错误0005573\n");
			cJSON_ReplaceStr(OUT,"DispSwitch","1");
			cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码支付账户错误0005573");
			memset(cSound,0,sizeof(cSound));
			snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
			cJSON_ReplaceArray(OUT,"Sound",cSound);

			return ERROR_QRCODE_RFU;
		}

	}


	if(pucQRCode[154]==0)
	{
		PRINTF(ERROR,"交通部二维码账户类型错误0005577\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码账户类型错误0005577");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_RFU;
	}



	gettimeofday(&tv,NULL);

	//支付帐号授权时间有效性判断
	sdwSec=(pucQRCode[191]<<24)|(pucQRCode[192]<<16)|(pucQRCode[193]<<8)|pucQRCode[194];
	if(sdwSec<tv.tv_sec)
	{
		PRINTF(ERROR,"交通部二维码授权时间错误0005580\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码授权时间错误0005580");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_OVERDUE;
	}

	sdwLength=pucQRCode[197];
	if(sdwLength>32)
	{
		PRINTF(ERROR,"交通部二维码扩展域错误0005581\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码扩展域错误0005581");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_RFU;
	}
	if((197+sdwLength+135)!=sdwQRCodeLen)
	{
		PRINTF(ERROR,"交通部二维码扩展域错误0005582\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码扩展域错误0005582");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_RFU;
	}

	if(((pucQRCode[195]<<8)+pucQRCode[196])>60*20)
	{
		PRINTF(ERROR,"交通部二维码有效期错误0005501\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码有效期错误0005501");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_RFU;
	}

	//二维码生成时间判断
	sdwLength=pucQRCode[197];
	sdwSec=(pucQRCode[198+sdwLength+65]<<24)|(pucQRCode[198+sdwLength+66]<<16)|(pucQRCode[198+sdwLength+67]<<8)|pucQRCode[198+sdwLength+68];
	if((sdwSec-20)>tv.tv_sec)
	{
		PRINTF(ERROR,"交通部二维码时间错误0005584\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码时间错误0005584");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_OVERDUE;
	}

	//二维码有效期判断
	if((sdwSec+(pucQRCode[195]<<8)+pucQRCode[196])<tv.tv_sec)
	{
		PRINTF(ERROR,"交通部V2二维码已过期 pucQRCode time:%d ,%d nowtime:%d\n",sdwSec,((pucQRCode[195]<<8)+pucQRCode[196]),(int)tv.tv_sec);
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码已过期0005584");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_OVERDUE;
	}


	memset(cPublicKeyStr,0,sizeof(cPublicKeyStr));
	memset(cY,0,sizeof(cY));
	BytesToHexString((char  *)cPublicKeyStr,sizeof(cPublicKeyStr),&pucQRCode[23],33);



	Sm2GetY(cPublicKeyStr,&cY[0],sizeof(cY));
	i=strlen(cY);
	if((i>64)||(i<60))
	{
		PRINTF(ERROR,"交通部二维码发卡机构签名错误0005586\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构签名错误0005586");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}

	memset(&cPublicKeyStr[66],0x30,64);
	memcpy(&cPublicKeyStr[66+64-i],&cY[0],i);
	cPublicKeyStr[0]='0';
	cPublicKeyStr[1]='4';


	sdwLength=pucQRCode[197];

	//签名认证
	SetSignData(&pucQRCode[198+sdwLength+1],64,(BYTE  *)ucSign,(unsigned int *)&sdwSignLen);

	HexStringToBytes(&cPublicKeyStr[2],128,ucXY,64);

	TcpsSm3(&ucXY[0],&ucXY[32],&pucQRCode[3],195+sdwLength,ucSm3Digest);

	sdwRet=Sm2Verify2(cPublicKeyStr,ucSm3Digest,32,ucSign,sdwSignLen);
	if(sdwRet!=0)
	{
		PRINTF(ERROR,"交通部二维码发卡机构签名错误0005583\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码发卡机构签名错误0005583");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);

		return ERROR_QRCODE_VERIFY;
	}


	memset(cPublicKeyStr,0,sizeof(cPublicKeyStr));
	memset(cY,0,sizeof(cY));
	BytesToHexString((char  *)cPublicKeyStr,sizeof(cPublicKeyStr),&pucQRCode[158],33);

	Sm2GetY(cPublicKeyStr,&cY[0],sizeof(cY));
	i=strlen(cY);
	if((i>64)||(i<60))
	{
		PRINTF(ERROR,"交通部二维码用户签名错误0005579\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码用户签名错误0005579");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_VERIFY;
	}

	memset(&cPublicKeyStr[66],0x30,64);
	memcpy(&cPublicKeyStr[66+64-i],&cY[0],i);
	cPublicKeyStr[0]='0';
	cPublicKeyStr[1]='4';

	sdwLength=pucQRCode[197];

	//签名认证
	SetSignData(&pucQRCode[198+sdwLength+70],64,(BYTE  *)ucSign,(unsigned int *)&sdwSignLen);

	HexStringToBytes(&cPublicKeyStr[2],128,ucXY,64);

	TcpsSm3(&ucXY[0],&ucXY[32],&pucQRCode[0],198+sdwLength+69,ucSm3Digest);

	sdwRet=Sm2Verify2(cPublicKeyStr,ucSm3Digest,32,ucSign,sdwSignLen);
	if(sdwRet!=0)
	{
		PRINTF(ERROR,"交通部二维码用户签名错误0005585\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部二维码用户签名错误0005585");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_VERIFY;
	}



	
	sdwRet=SearchQRCodeRecord("TK_EWMPAY_TRADE_TODAY",cUserId,CHINATUNION_V2_OFFLINE_QRCODE_SHAKE);
	if(1==sdwRet)
	{
		PRINTF(ERROR,"交通部V2二维码抖动连刷\n");
		return ERROR_QRCODE_FIND;
	}

	sha1(pucQRCode,sdwQRCodeLen,ucNowSha);
	if(memcmp(ucNowSha,s_ucLastSha,20)==0)
	{
		PRINTF(ERROR,"交通部V2二维码重复\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V2二维码重复");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingshuaxin);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_DUPLICATED;
	}

	memcpy(s_ucLastSha,ucNowSha,20);

	sdwRet=SearchQRCodeRecord("TK_EWMPAY_TRADE_TODAY",cUserId,CHINATUNION_V2_OFFLINE_QRCODE_INTERVAL);
	if(1==sdwRet)
	{
		PRINTF(ERROR,"交通部V2二维码禁止带人\n");
		cJSON_ReplaceStr(OUT,"DispSwitch","1");
		cJSON_ReplaceStr(OUT,"DispInfo","交通部V2二维码禁止带人");
		memset(cSound,0,sizeof(cSound));
		snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"500\",\"Break\":\"0\"}]",SOUNDFILE_qingtoubi);
		cJSON_ReplaceArray(OUT,"Sound",cSound);
		return ERROR_QRCODE_RFU;
	}


	CHAR cPay[10];
	memset(cPay,0,sizeof(cPay));
	snprintf(cPay,sizeof(cPay),"%d",qrcode_Manage->BasePrice);
	cJSON_ReplaceStr(OUT,"PayType","6");
	cJSON_ReplaceStr(OUT,"Pay",cPay);
	cJSON_ReplaceStr(OUT,"DispSwitch","1");
	cJSON_ReplaceStr(OUT,"DispInfo","SUCCESS");
	memset(cSound,0,sizeof(cSound));
	snprintf(cSound,sizeof(cSound),"[{\"Name\":\"%s\",\"Delay\":\"800\",\"Break\":\"0\"}]",SOUNDFILE_shuamachenggong);
	cJSON_ReplaceArray(OUT,"Sound",cSound);

	
	TKRecord record;
    T_RecordStruct table = record.GetRecordStruct((char *)"TK_EWMPAY_TRADE_TODAY");
    record.TagValue(&table, (CHAR *)"CARD_NO", cUserId);
	record.TagValue(&table, (CHAR *)"ewm_type", "0");
    record.TagValue(&table, (CHAR *)"ewmpay_data1", pucQRCode, sdwQRCodeLen);
    record.TagValue(&table, (CHAR *)"ewm_type", "0");
    record.TagDate(&table, (CHAR *)"RIDING_TIME", tv.tv_sec);
    record.TagValue(&table, (CHAR *)"PAID", cPay);
	record.TagValue(&table, (CHAR *)"region_code", qrcode_Manage->RegionNo.c_str());     // 消费区域号    REGION_CODE    NUMBER    2
    record.TagValue(&table, (CHAR *)"voca_code", qrcode_Manage->IndustryNo.c_str());     // 消费行业号    VOCA_CODE    NUMBER    2
    record.TagValue(&table, (CHAR *)"dept_no", qrcode_Manage->CompanyNo.c_str());        // 消费公司号    DEPT_NO    NUMBER    8
    record.TagValue(&table, (CHAR *)"group_no", qrcode_Manage->TeamNo.c_str());          // 消费路队号    GROUP_NO    NUMBER    6
    record.TagValue(&table, (CHAR *)"line_no", qrcode_Manage->LineNo.c_str());           // 消费线路号    LINE_NO    NUMBER    6
    record.TagValue(&table, (CHAR *)"bus_no", qrcode_Manage->BusNo.c_str());             // 消费车辆号    BUS_NO    NUMBER    8
    record.TagValue(&table, (CHAR *)"mach_no", qrcode_Manage->PosID.c_str());            // 消费设备号    MACH_NO    NUMBER    8
   	record.TagValue(&table, (CHAR *)"condcard_no", qrcode_Manage->SellerSerial.c_str()); // 售票员卡号    CONDCARD_NO    VARCHAR2    16  
	record.TagValue(&table, (CHAR *)"opcard_no", qrcode_Manage->DriverSerial.c_str());   // 司机卡号    OPCARD_NO    VARCHAR2    16
    record.TagDate(&table, (CHAR *)"reach_time", qrcode_Manage->s_SignedUtc);            // 司机上班时间    REACH_TIME    DATE
	record.TagValue(&table, (CHAR *)"charge_type", "0");
    record.InsertRecord(table);
	

	PRINTF(INFO,"ChinaTunion V2 QRCode check successed\n");
	PRINTF(INFO,"===========End================\n");

	return QRCODE_SUCCESS;
}



/**************************************************************************
* 函数名称： SDWORD ChinaTunionOfflineQRCode(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT)
* 功能描述： 交通部离线二维码验签
* 访问的表：
* 修改的表：
* 输入参数： pucQRCode 		二维码
*			sdwQRCodeLen   二维码长度
* 输出参数： OUT			结果输出
* 返 回 值： 错误类型
* 其它说明：
* 修改日期    版本号     修改人	     修改内容
* -----------------------------------------------
* 2019/05/24	     V1.0	    mwl
**************************************************************************/
SDWORD ChinaTunionOfflineQRCode(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT)
{
	SDWORD sdwAck=ERROR_QRCODE_FIND;

	sdwAck=ChinaTunionOfflineQRCodeV1(pucQRCode,sdwQRCodeLen,OUT);//验证是否是交通部版本1二维码
    if(sdwAck==ERROR_QRCODE_FORMAT)
	{
		sdwAck=ChinaTunionOfflineQRCodeV2(pucQRCode,sdwQRCodeLen,OUT);//验证是否是交通部版本2二维码
	}
	return sdwAck;
}
#endif