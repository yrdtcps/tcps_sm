#ifndef HEADER_VD1050_H
#define HEADER_VD1050_H

// #include"Data_func.h"
#include"tk_public.h"

#ifdef  __cplusplus
extern "C" {
#endif


#define DATA_BUF_SIZE    4096   ///数据总线缓存大小

#define RPT_QRCode_MAX_NO 10  //二维码缓存用于判断重复
#define RPT_QRCode_MAX_LEN  500  //二维码缓存用于判断重复
#define RPT_QRCode_ID_Len  8  //二维码ID，用于判重 



typedef struct
{
	unsigned char ulFileNum[4];
	unsigned char FileVersion[4];	
	unsigned char FileEnableDate[7];	
	unsigned char file_CRC[2];
	unsigned char OverFlag;	
}FILE_HEAD_STRUCT;


typedef struct
{
	unsigned char  QR_State;
	unsigned char  QR_CodeState;
	unsigned char  QR_CodeType;
	unsigned char  LAST_LEN[2];
	unsigned char  LAST_QR[1024*3];
	unsigned char  QRID_LEN[2];
	unsigned char  QRID[RPT_QRCode_ID_Len];
}QR_ST;




extern char Current_ID[RPT_QRCode_ID_Len+100];//当前二维码ID 收到回应后存到数据库
extern int BUS_TYPE;//票制类型   1是单票 2是多票
extern int NUMBER;//趟次号

extern volatile unsigned char RPT_QRCode[RPT_QRCode_MAX_NO][RPT_QRCode_ID_Len+RPT_QRCode_MAX_LEN];
extern volatile unsigned char RPT_QRCode_Index;
extern volatile  unsigned int RPT_Record;
//软件版本
extern const char SoftVer[4];

///KEY 文件互斥琐
extern pthread_mutex_t  mutex_FILE_KEY;

extern pthread_mutex_t  mutex_BUS_CMD;

///二维码数据锁
extern pthread_mutex_t  mutex_QRCode_Data;


////ALI PAY
extern unsigned char ALIPAY_VERSION[10];//SO 库版本

///ALI PAY秘钥文件头
extern volatile FILE_HEAD_STRUCT AppHead_St;


///WeChat
extern unsigned char WeChat_VERSION[10];//SO 库版本

//公钥头
extern volatile FILE_HEAD_STRUCT WeChatkey_St;
//MAC 秘钥头
extern volatile FILE_HEAD_STRUCT WeChatMac_St;


//二维码回应信息 
extern volatile QR_ST  QR_INFO;

//设备号
extern volatile unsigned char ID[8];

///二维码类型  1支持验证    0不支持
extern volatile unsigned char QRCode_ENABLE;
extern volatile unsigned char QR_ALI_ENABLE;
extern volatile unsigned char QR_WeChat_ENABLE;
extern volatile unsigned char QR_XiaMen_ENABLE;
extern volatile unsigned char QR_JNWanDa_ENABLE;
extern volatile unsigned char QR_TianTai_ENABLE;
extern volatile unsigned char QR_FuZhou_ENABLE;
extern volatile unsigned char QR_ChongQing_ENABLE;
extern volatile unsigned char QR_XiaoBu_ENABLE;
extern volatile unsigned char QR_ZhengZhouBank_ENABLE;
extern volatile unsigned char QR_HaErBinJTB_ENABLE;
extern volatile unsigned char QR_LanZhou_ENABLE;
extern volatile unsigned char QR_TCPS_ENABLE;
extern volatile unsigned char QR_WeChatSTO_ENABLE;
extern volatile unsigned char QR_JTBV1_ENABLE;
extern volatile unsigned char QR_MeiTuan_ENABLE;
extern volatile unsigned char QR_BestPay_ENABLE;
extern volatile unsigned char QR_CMPay_ENABLE;
extern volatile unsigned char QR_MeiTuanV2_ENABLE;
extern volatile unsigned char QR_TianFuTong_ENABLE;



int Read_Configfile(void);



/*************************************************
*  Function:       // READ_GPS_INFO
*  Description:    // 获取 定位参数信息
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其它说明
*************************************************/
int READ_GPS_INFO(void);


/*************************************************
*  Function:       // Read_ALIPAY_PARAM
*  Description:    // 获取 支付宝参数信息
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其它说明
*************************************************/
int Read_ALIPAY_PARAM(void);

/*************************************************
*  Function:       // ALIPAY_INIT
*  Description:    // 初始化支付宝参数
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其他说明
*************************************************/
int ALIPAY_INIT(void);


/*************************************************
*  Function:       // Read_WeChat_PARAM
*  Description:    // 获取 微信参数信息
*  Input:          // 无
*                  //
*  Output:         // 无
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 其它说明
*************************************************/
int Read_WeChat_PARAM(void);

/*************************************************
*  Function:       // change_Keyfile
*  Description:    // 将总线接收到的支付宝公钥文件（key_back_over.txt）转化为 json格式存储到 ALIPAY_KEY.txt
*  Input:          // 无
*                  //
*  Output:         // ALIPAY_KEY.txt
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 文件位置/usr/VD1050/
*************************************************/
int change_Keyfile(void);


/*************************************************
*  Function:       // change_WeChat_Keyfile
*  Description:    // 将总线接收到的微信公钥文件（WeChat_key_back_over.txt）转化为 json格式存储到 WeChat_KEY.txt
*  Input:          // 无
*                  //
*  Output:         // WeChat_KEY.txt
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 文件位置/usr/VD1050/
*************************************************/
int change_WeChat_Keyfile(void);


/*************************************************
*  Function:       // change_WeChat_MACfile
*  Description:    // 将总线接收到的微信公钥文件（WeChat_MAC_back_over.txt）转化为 json格式存储到 WeChat_MAC.txt
*  Input:          // 无
*                  //
*  Output:         // WeChat_MAC.txt
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 文件位置/usr/VD1050/
*************************************************/
int change_WeChat_MACfile(void);

/*************************************************
*  Function:       // change_Profile
*  Description:    // 将总线接收到的VD1050程序文件（pro_back_over） 如果CRC正确  重命名位VD1050_update
*  Input:          // 无
*                  //
*  Output:         // VD1050_update
*                  //
*  Return:         // －1：失败    0：成功
*  Others:         // 重启系统后 会自动更新文件
*************************************************/
int change_Profile(void);

/*************************************************
*  Function:       // check_qrcode_demo
*  Description:    // 本例程演示了如何使用支付宝离线安全库对二维码进行验证
*  Input:          // *qrcode	:二维码数据指针
*                  // *qrcode_len	:二维码数据长度
*  Output:         // QR_INFO	：二维码数据信息
*                  //
*  Return:         // 非0：失败    0：成功
*  Others:         // 其他说明
*************************************************/
int check_qrcode_demo(unsigned char* qrcode, int* qrcode_len);



/*************************************************
*  Function:       // check_chat_qrcode_demo
*  Description:    // 本例程演示了如何使用微信离线安全库对二维码进行验证
*  Input:          // *qrcode	:二维码数据指针
*                  // *qrcode_len	:二维码数据长度
*  Output:         // QR_INFO	：二维码数据信息
*                  //
*  Return:         // 非0：失败    0：成功
*  Others:         // 其他说明
*************************************************/
int check_chat_qrcode_demo(unsigned char* qrcode, int* qrcode_len);

int ALI_IOT_query_data(char *data,int datalen);

int ALIPAY_LOG_SET(char *json);

#ifdef  __cplusplus
}
#endif

#endif 
