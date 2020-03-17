/************************************************************************
 * * 版权所有 (C)2019, 天津通卡智能网络科技股份有限公司。
 * *
 * * 文件名称： typedef.h
 * * 文件标识：
 * * 内容摘要： 定义公用数据类型
 * * 其它说明：
 * * 当前版本：
 * * 作    者：
 * * 完成日期： 2019.5.23
 * *
 * * 修改记录1：
 * *    修改日期：
 * *    版 本 号：
 * *    修 改 人：
 * *    修改内容：
 * * 修改记录2：…
 * ************************************************************************/

#ifndef _TYPE_DEF_H
#define _TYPE_DEF_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/***********************************************************
 *                       数据类型定义                      *
 ***********************************************************/
typedef void									VOID;
typedef unsigned char							BOOL;
typedef char									CHAR;
typedef unsigned char							BYTE;
typedef unsigned short int						WORD;
typedef short int								SWORD;
typedef short int								SHORT;
typedef unsigned int							DWORD;
typedef int										SDWORD;
typedef unsigned long long						LLONG;
typedef long long								SLLONG;
typedef float									FLOAT;
typedef double									DOUBLE;
typedef void									*HANDLE;
typedef signed int          SOCKET;

//for wulian code,del later wxg 0704
typedef unsigned short int  WORD16;
typedef signed short int    SWORD16;

typedef unsigned int        WORD32;
typedef signed int          SWORD32;

typedef unsigned long long  WORD64;
typedef signed long long    SWORD64;
typedef unsigned int        BOOLEAN;

#ifndef true
#define true                (!0)
#endif

#ifndef false
#define false               0
#endif

#ifndef TRUE
#define TRUE                (!0)
#endif

#ifndef FALSE
#define FALSE               0
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif  /* _TYPE_DEF_H */
