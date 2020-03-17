/***********************************
* Copyright (c) 2017,天津通卡
* All rights reserved.
* 文件名称：tk_log.h
* 文件标识：
* 摘要：实现日志的维护
*************************************/
/* ***********************************
* 作者：孟维良，修改日期：2017年9月1日
* 修改简要说明：
**************************************/

#ifndef _TK_CONFIG_H_
#define _TK_CONFIG_H_


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define LOG_FATAL       0     //
#define LOG_ERROR       1     // 
#define LOG_WARN        2     //
#define LOG_INFO        3     // 
#define LOG_TRACE       4     // 
#define LOG_DEBUG       5     // 
#define LOG_ALL         6     // 

unsigned long get_file_size(const char *path);

/*************************************************
*  Function:       // GetTime
*  Description:    // 获取系统时间
*  Input:          // 无
*  Output:         // *pszTimeStr	:时间字符串
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/ 
void GetTime(unsigned char *pszTimeStr);

/*************************************************
*  Function:       // GetCompletePath
*  Description:    // 获取文件的绝对路径
*  Input:          // *FileName		：文件名
*                  // 
*  Output:         // *WholePath	:绝对路径
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
void GetCompletePath(char *FileName,char *WholePath);

/*************************************************
*  Function:       // GetConfigFileStringValue
*  Description:    // 获取 某一项关键字 的字符串值
*  Input:          // *ConfigFileName		：文件路径
*                  // *SectionName	:块名称  一个块包含若干关键字
*                  // *KeyName	:关键字名称
*                  // *pDefaultVal	:缺省值
*  Output:         // *Output	:关键字值
*                  // *OutputLen	:关键字长度
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
void GetConfigFileStringValue(char *SectionName, char *KeyName,char *pDefaultVal,char *Output, unsigned  int iOutputLen, char* ConfigFileName);

/*************************************************
*  Function:       // GetConfigFileIntValue
*  Description:    // 获取 某一项关键字 的INT 值
*  Input:          // *ConfigFileName		：文件路径
*                  // *SectionName	:块名称  一个块包含若干关键字
*                  // *KeyName	:关键字名称
*                  // *iDefaultVal	:缺省值
*  Output:         // 无
*  Return:         // 关键字值
*  Others:         // 其它说明
*************************************************/
int  GetConfigFileIntValue(char *SectionName, char *KeyName, unsigned  int iDefaultVal,char *ConfigFileName);


/*************************************************
*  Function:       // SetConfigFileStringValue
*  Description:    // 设置 某一项关键字 的字符串值
*  Input:          // *ConfigFileName		：文件路径
*                  // *SectionName	:块名称  一个块包含若干关键字
*                  // *KeyName	:关键字名称
*                  // 
*  Output:         // *Input	:字符串值
*                  // *iInputLen	:字符串长度
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
void SetConfigFileStringValue(char *SectionName, char *KeyName, char *Input, unsigned  int iInputLen,char *ConfigFileName);



/*************************************************
*  Function:       // WriteLogFile
*  Description:    // 写日志
*  Input:          // *FileName	: 文件名
*					// *FunctionName	:函数名		
*					// iCodeLine	:行号
*					// iLogLevel	:错误等级
*					// *Content		:日志内容	
*  Output:         // 无
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/  
void WriteLogFile(char *FileName,const char *FunctionName, unsigned int iCodeLine, unsigned int iLogLevel,  char* Content);





#define TCPS_LOG(level,msg)  WriteLogFile((char *)__FILE__, (char *)__FUNCTION__,  (unsigned int)__LINE__, level, (char*)msg)

#endif    /*  _TK_CONFIG_H_  */
