/************************************************************************
* 版权所有 (C)2019, 天津通卡智能网络科技股份有限公司。
* 
* 文件名称： // chinatunion_offline_qrcode.h
* 文件标识： // 
* 内容摘要： // 交通部脱机二维码头文件
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
#ifndef _ChinaTunion_OFFLINE_QRCODE_H_
#define _ChinaTunion_OFFLINE_QRCODE_H_



#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "cJSON.h"
#include "typedef.h"
#include "directory.h"

/**************************************************************************
 *                        常量                                            *
 **************************************************************************/
//#define  CHINATUNION_OFFLINE_QRCODE_ENABLE 1     //功能使能
//#define  CHINATUNION_OFFLINE_QRCODE_SHAKE 2		//2s  防抖动间隔
//#define  CHINATUNION_OFFLINE_QRCODE_INTERVAL 60	//60s 刷码间隔

#define  CHINATUNION_V1_OFFLINE_QRCODE_KEY_PATH   (KEY_DIR "ChinaTunionV1OfflineQRCode.txt")  // 交通部版本1二维码密钥存储文件
#define  CHINATUNION_V2_OFFLINE_QRCODE_KEY_PATH   (KEY_DIR "ChinaTunionV2OfflineQRCode.txt")  // 交通部版本2二维码密钥存储文件


extern CHAR  CHINATUNION_V1_OFFLINE_QRCODE_ENABLE;     //1 功能使能
extern CHAR  CHINATUNION_V1_OFFLINE_QRCODE_TYPE;     //
extern SDWORD  CHINATUNION_V1_OFFLINE_QRCODE_SHAKE;		//2s  防抖动间隔
extern SDWORD  CHINATUNION_V1_OFFLINE_QRCODE_INTERVAL;	//0s 刷码间隔

extern CHAR  CHINATUNION_V2_OFFLINE_QRCODE_ENABLE;     //1 功能使能
extern CHAR  CHINATUNION_V2_OFFLINE_QRCODE_TYPE;     //
extern SDWORD  CHINATUNION_V2_OFFLINE_QRCODE_SHAKE;		//2s  防抖动间隔
extern SDWORD  CHINATUNION_V2_OFFLINE_QRCODE_INTERVAL;	//0s 刷码间隔



/**************************************************************************
 *                        全局函数原型                                     *
 **************************************************************************/


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
SDWORD ChinaTunionOfflineQRCodeInit(void);

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
SDWORD ChinaTunionOfflineQRCode(const BYTE *pucQRCode, SDWORD sdwQRCodeLen,cJSON *OUT);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif 
