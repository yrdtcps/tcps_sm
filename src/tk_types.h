

#ifndef _TK_TYPES_H_
#define _TK_TYPES_H_

typedef unsigned char 		TK_U8;
typedef unsigned short		TK_U16;
typedef unsigned int		TK_U32;
typedef unsigned long long	TK_U64;

typedef char				TK_S8;
typedef short				TK_S16;
typedef int				TK_S32;
typedef long long			TK_S64;

typedef char				TK_CHAR;
typedef char*			TK_PCHAR;
typedef const char*		TK_CPCHAR;

typedef float				TK_FLOAT;
typedef double			TK_DOUBLE;
typedef void				TK_VOID;
typedef void*			TK_PVOID;
typedef void**			TK_PPVOID;
typedef long				TK_TIME_T;

typedef char tk_int8;
typedef unsigned char tk_uint8;
typedef short tk_int16;
typedef unsigned short tk_uint16;
typedef int tk_int32;
typedef unsigned int tk_uint32; 
typedef long long tk_int64;
typedef unsigned long long tk_uint64; 

typedef volatile char tk_vint8;
typedef volatile unsigned char tk_vuint8;
typedef volatile short tk_vint16;
typedef volatile unsigned short tk_vuint16;
typedef volatile long tk_vint32;
typedef volatile unsigned long tk_vuint32;
typedef volatile long long tk_vint64;
typedef volatile unsigned long long tk_vuint64; 


typedef enum
{
	TK_FALSE 	= 0,
	TK_TRUE 	= 1,
}TK_BOOL;

#define TK_PARAM_NULL   1             //
#define TK_SUCCESS		0		//
#define TK_FAILURE		-1		//
#define TK_BREAK		-2		//
#define TK_NOPS			-3		//

#endif    /*  _TK_TYPES_H_  */

