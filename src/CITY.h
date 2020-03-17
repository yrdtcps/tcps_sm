#ifndef HEADER_CITY_H
#define HEADER_CITY_H



#ifdef  __cplusplus
extern "C" {
#endif

#if 0
#include "XiaMenQRCode.h"
#include "JNWanDaQRCode.h"
#include "TianTaiQRCode.h"
#include "FuZhouQRCode.h"
#include "ChongQingQRCode.h"
#include "XiaoBuQRCode.h"
#include "ZhengZhouBank.h"
#include "HaErBinJTB.h"
#include "LanZhouQRCode.h"
#include "TCPSQRCode.h"
#include "WeChatSTO.h"
#include "JTBQRCodeV1.h"
#include "MeiTuanQRCode.h"
#include "BestPayQRCode.h"
#include "CMPayQRCode.h"
#endif
#include "TianFuTongQRCode.h"

#define  CITY_MAX  17 // 城市个数

extern const unsigned int CITY_NO[CITY_MAX][2];///索引 ＋城市代码
extern volatile unsigned char CITY_LIB_VERSION[CITY_MAX][4];//城市对应的so库版本，通卡OPENSSL库版本（厦门）
// extern volatile FILE_HEAD_STRUCT CITY_Key_St[CITY_MAX];//城市对应秘钥头结构体

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
int Read_CITY_PARAM(unsigned int city_no);

/*************************************************
*  Function:       // Find_City_LIB
*  Description:    // 检索城市库
*  Input:          // 库名字
*                  //
*  Output:         // 无
*  Return:         // －1：未找到   >=0 : 城市代码
*  Others:         // 其它说明
*************************************************/

int Find_City_LIB(char *libname);

/*************************************************
*  Function:       // Find_City_NO
*  Description:    // 检索城市列表，确认是否包含索引城市
*  Input:          // city_no	：2字节 16进制  城市代码  例如 ：0x3610
*                  //
*  Output:         // 无
*  Return:         // －1：未找到   >=0 : 城市索引
*  Others:         // 其它说明
*************************************************/
int Find_City_NO(unsigned int city_no);

/*************************************************
*  Function:       // Get_CITY_KeyInfo
*  Description:    // 调用 Read_CITY_Keyfile 获取所有城市秘钥信息
*  Input:          // 无
*                  //
*  Output:         // 无
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
void Get_CITY_KeyInfo(void);

/*************************************************
*  Function:       // Get_CITY_QRLib
*  Description:    // 读取ALIPAY_libver 文件 ，获取每个城市对应的 so库版本信息
*  Input:          // 无
*                  //
*  Output:         // 无
*  Return:         // 0 ：成功
*  Others:         // 其它说明
*************************************************/
int Get_CITY_QRLib(void);

/*************************************************
*  Function:       // change_CITY_TcpsKeyfile
*  Description:    // 将总线上受到的通卡自由码秘钥格式数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*************************************************/
int change_CITY_TcpsKeyfile(unsigned int city_no);


/*************************************************
*  Function:       // Change_MeiTuan_NewKey
*  Description:    // 美团so秘钥文件
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  转换为 /usr/VD1050/CITY_%04x_NEWKEY.txt.txt（Json 格式存储秘钥）
*************************************************/
int Change_MeiTuan_NewKey(unsigned int city_no);


/*************************************************
*  Function:       // change_CITY_Keyfile
*  Description:    // 将总线上受到的数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*************************************************/
int change_CITY_Keyfile(unsigned int city_no) ;


/*************************************************
*  Function:       // change_CITY_Keyfile2
*  Description:    // 将总线上受到的数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*  文件格式：
*  数据长度	2
*  公钥编号	4
*  公钥证书长度	2
*  公钥证书	n
*************************************************/
int change_CITY_Keyfile2(unsigned int city_no);


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
int change_CITY_Keyfile3(unsigned int city_no);

/*************************************************
*  Function:       // change_CITY_Keyfile4
*  Description:    // 将总线上受到的通卡自由码秘钥格式数据进行转换，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/CITY_城市代码__KEY.txt（Json 格式存储秘钥）
*************************************************/
int change_CITY_Keyfile4(unsigned int city_no);



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
int change_CITY_MACfile(unsigned int city_no);

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
int change_JTB_Keyfile(unsigned int city_no);


/*************************************************
*  Function:       // change_CITY_CAfile
*  Description:    // 将总线上的数据转成CA文件，方便使用
*  Input:          // city_no	：2字节 16进制  城市代码
*                  //
*  Output:         // 无
*  Return:         // －1：失败  0 : 成功
*  Others:         // 将  文件/opt/CITY_城市代码_key_back_over.txt  转换为 /usr/VD1050/cert/xxx
*************************************************/
int change_CITY_file(unsigned int city_no);




/*************************************************
*  Function:       // GET_CITY_KEY
*  Description:    // 根据城市代码及 公钥ID 获取相应秘钥
*  Input:          //  id		:公钥ID
*                  //  city_no	:2字节 16进制  城市代码
*  Output:         //  *key	  	:公钥 字符串
*  Return:         // －1：失败  0 : 成功
*  Others:         // 公钥存储位置 /usr/VD1050/CITY_城市代码__KEY.txt
*************************************************/
int GET_CITY_KEY(int id,char *key,unsigned int city_no);




#ifdef  __cplusplus
}
#endif

#endif
