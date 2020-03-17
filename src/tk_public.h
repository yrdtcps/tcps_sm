
#ifndef _TK_PUBLIC_H_
#define _TK_PUBLIC_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "tk_types.h"

enum{
	ERROR=0,
	WARN,
	INFO,
	DEBUG
};

extern volatile char Debug_Level;
//#define Debug_Level  INFO

#define F_OUT printf("VD1050_DEBUG: %s==>  ", __FUNCTION__);fflush(stdout);
#define L_OUT printf("VD1050_DEBUG: %s:%d==>  ", __FILE__, __LINE__);fflush(stdout);
#define A_OUT printf("VD1050_DEBUG: %s:%d:%s==>  ", __FILE__, __LINE__, __FUNCTION__);fflush(stdout);
#define D_OUT printf("VD1050_DEBUG: ");fflush(stdout);

#define F_PRINTF(level,fmt, arg...)  if(level<=Debug_Level){F_OUT; printf(fmt, ##arg);}
#define L_PRINTF(level,fmt, arg...)  if(level<=Debug_Level){L_OUT; printf(fmt, ##arg);}
#define A_PRINTF(level,fmt, arg...)  if(level<=Debug_Level){A_OUT; printf(fmt, ##arg);}
#define PRINTF(level,fmt, arg...)    if(level<=Debug_Level){D_OUT; printf(fmt, ##arg);}






//#define F_PERROR(fmt) F_OUT perror(fmt)
//#define L_PERROR(fmt) L_OUT perror(fmt)
//#define A_PERROR(fmt) A_OUT perror(fmt)
//#define PERROR(fmt) D_OUT perror(fmt)


struct rtctm
{
	unsigned char CLKYearH;
	unsigned char CLKYearL;
	unsigned char CLKMonth;
	unsigned char CLKDate;
	unsigned char CLKHour;
	unsigned char CLKMin;
	unsigned char CLKSec;
	unsigned char CLKWeek;
};

#define FileList_MAX_NO  50  //最大获取文件数
#define FileList_MAX_LEN 100 //文件名最大长度

struct FileList
{
	char FileName[FileList_MAX_NO][FileList_MAX_LEN];
	int  FileNum;
};


#define DF_HEX2BCD(x) (((( (x)/10)<<4) & 0xf0)+(((x)%10) & 0x0f))


#define TK_MAX_DATA_LEN    1024*1024
#define TK_CMD_MAX_LEN      128//
#define TK_READ_MAX_LEN    128//

/*************************************************
*  Function:       // tk_system
*  Description:    // 通过输入字符串 执行系统指令
*  Input:          // *cpCmd	:指令字符串
*                  //
*  Output:         // 无
*  Return:         // TK_SUCCESS：成功   TK_FAILURE : 失败
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_system(const tk_int8 *cpCmd);

/*************************************************
*  Function:       // tk_popen
*  Description:    // 通过创建管道 执行系统指令
*  Input:          // *cpCmd	:指令字符串
*                  // lCmdLen	:指令字符串长度
*                  // cpBuf		:指令执行结果
*                  // lpBufLen	:指令执行结果长度
*  Output:         // 无
*  Return:         // TK_SUCCESS：成功   TK_FAILURE : 失败
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_popen(tk_int8 *cpCmd, tk_int32 lCmdLen, tk_int8 *cpBuf, tk_int32 *lpBufLen);

/*************************************************
*  Function:       // tk_get_filelen
*  Description:    // 获取文件长度
*  Input:          // *cpFile	:文件路径
*                  // *lpLen	:文件长度
*                  //
*  Output:         // 无
*  Return:         // TK_SUCCESS：成功   TK_FAILURE : 失败
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_get_filelen(const tk_int8 *cpFile, tk_int32 *lpLen);


/*************************************************
*  Function:       // tk_read_filelen
*  Description:    // 读取文件长度
*  Input:          // *cpFile	:文件路径
*                  // *lpLen	:文件长度
*                  //
*  Output:         // 无
*  Return:         // TK_SUCCESS：成功   TK_FAILURE : 失败
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_read_filelen(const tk_int8 * pFile, tk_int32 *pLen);


/*************************************************
*  Function:       // tk_read_binary
*  Description:    // 读取二进制文件
*  Input:          // *cpFile	:文件路径
*                  // lOffset	:便宜量
*                  // lLen		:读取长度
*                  //
*  Output:         // **cppContent	:读取到的数据
*  Return:         // TK_PARAM_NULL：参数错误   TK_FAILURE : 失败		lRet：返回数据长度
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_read_binary(const tk_int8 *cpFile, tk_int32 lOffset, tk_int32 lLen, tk_int8 **cppContent);


/*************************************************
*  Function:       // tk_write_binary
*  Description:    // 写二进制文件
*  Input:          // *cpFile	:文件路径
*                  // lOffset	:便宜量
*                  // lLen		:写长度
*                  // **cppContent	:输入数据
*  Output:         //无
*  Return:         // TK_PARAM_NULL：参数错误   TK_FAILURE : 失败		lRet：返回写数据长度
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_write_binary(const tk_int8 *cpFile, tk_int32 lOffset, tk_int8 *cpContent, tk_int32 lLen);


/*************************************************
*  Function:       // tk_copy_binary
*  Description:    // 拷贝二进制文件
*  Input:          // *cpDstFile	:目的文件路径
*                  // *cpSrcFile	:源文件路径
*                  //
*  Output:         //无
*  Return:         // TK_PARAM_NULL：参数错误   TK_FAILURE : 失败		TK_SUCCESS：成功
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_copy_binary(const tk_int8 *cpDstFile, const tk_int8 *cpSrcFile);


/*************************************************
*  Function:       // tk_copy_ascii
*  Description:    // 拷贝文件
*  Input:          // *cpDstFile	:目的文件路径
*                  // *cpSrcFile	:源文件路径
*                  //
*  Output:         //无
*  Return:         // TK_PARAM_NULL：参数错误   TK_FAILURE : 失败		TK_SUCCESS：成功
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_copy_ascii(const tk_int8 *cpDstFile, const tk_int8 *cpSrcFile);

/*************************************************
*  Function:       // Check_OverTime
*  Description:    // 超时检测
*  Input:          // time	:超时的相对时间  秒数（UTC）
*                  // OverTime	:超时时间 单位秒
*                  //
*  Output:         //无
*  Return:         // 1：超时时间到	0：未超时
*  Others:         // 其它说明
*************************************************/
int Check_OverTime(unsigned int time,unsigned int  OverTime);

/*************************************************
*  Function:       // Check_OverTimeMsec
*  Description:    // 超时检测
*  Input:          	// sec	:超时的相对时间  秒数
*					// msec	:超时的相对时间  毫数
*                  // OverTime	:超时时间 单位毫秒
*                  //
*  Output:         //无
*  Return:         // 1：超时时间到	0：未超时
*  Others:         // 其它说明
*************************************************/
int Check_OverTimeMsec(unsigned int sec,unsigned int msec,unsigned int  OverTime);

/*************************************************
*  Function:       // GET_SYSTEM_SECOND
*  Description:    // 获取系统当前秒数
*  Input:          //
*                  //
*  Output:         // 无
*  Return:         // 当前秒数
*  Others:         // 其它说明
*************************************************/
unsigned int GET_SYSTEM_SECOND(void);

/*************************************************
*  Function:       // GET_SYSTEM_MSECOND
*  Description:    // 获取系统当前秒数和毫妙
*  Input:          //
*                  //
*  Output:         	// sec
*					// msec
*  Return:         // 无
*  Others:         // 其它说明

*************************************************/
void GET_SYSTEM_MSECOND(unsigned int* sec,unsigned int* msec);


/*************************************************

*  Function:       // BCD2HEX
*  Description:    //
*  Input:          // ucBCD
*                  //
*  Output:         // 无
*  Return:         // BCD
*  Others:         // 其它说明
*************************************************/
unsigned char BCD2HEX(unsigned char ucBCD);



/*************************************************

*  Function:       // HEX2BCD
*  Description:    //
*  Input:          // ucHEX
*                  //
*  Output:         //
*  Return:         // 16进制
*  Others:         // 其它说明
*************************************************/
unsigned char HEX2BCD(unsigned char ucHEX);



/*************************************************
*  Function:       // Time2UTC
*  Description:    // 把时间转成UTC
*  Input:          //  RTC_St	：时间结构体

*                  //
*  Output:         //  无
*                  //
*  Return:         //  UTC时间

*  Others:         //
*************************************************/
unsigned int Time2UTC(struct rtctm RTC_St);

/*************************************************
*  Function:       // UTCt2Time
*  Description:    // 把UTC转成时间
*  Input:          //  ulUTC	：UTC时间
*                  //
*  Output:         //  RTC_St	：时间结构体
*                  //
*  Return:         // 1：成功   0：失败
*  Others:         //
*************************************************/
unsigned char UTC2Time(unsigned int ulUTC,struct rtctm *RTC_St);

/*************************************************
*  Function:       // ReadFileList
*  Description:    // 获取目录下文件列表
*  Input:          // *basePath	：目录
*                  // FileList_ST: 文件列表
*  Output:         //
*                  //
*  Return:         // 0：成功   -1：失败
*  Others:         //
*************************************************/
int ReadFileList(char *basePath,struct FileList *FileList_ST);

//===========================================
//Function:	PRINTF_BUF
//Description:	打印一个数组
//Input:		HeadStr			----字符串头
//				data			----数组
//				dataLen			----数组长度
//
//Output:	无
//Return:
//Author:	mwl
//Date:		2018-10-23
//===========================================
void PRINTF_BUF(int level,const char *HeadStr,const unsigned char *data,int dataLen);


//===========================================
//Function:	PRINTF_INIT
//Description:	打印等级初始化
//Input:		
//				
//Output:	无
//Return:	
//Author:	mwl	
//Date:		2018-10-23
//===========================================
void PRINTF_INIT(void);

//===========================================
//Function:	GetFileTime
//Description:	获取文件时间信息
//Input:		pcFileName			文件名（全路径）
//				cType  1 创建时间  2 最后修改时间  3 访问时间
//				
//Output:	无
//Return:	－1 错误  否则返回时间
//Author:	mwl	
//Date:		2018-10-23
//===========================================
time_t GetFileTime(const char *pcFileName,char cType);




#endif    /*  _TK_PUBLIC_H_  */
