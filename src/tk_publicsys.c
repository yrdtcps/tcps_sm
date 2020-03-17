/***********************************
* Copyright (c) 2017,天津通卡
* All rights reserved.
* 文件名称：tk_public.c
* 文件标识：
* 摘要：主要根据城市代码实现不同城市的二维码秘钥管理
*************************************/
/* ***********************************
* 作者：孟维良，修改日期：2017年9月1日
* 修改简要说明：
**************************************/
#include <time.h>
#include <dirent.h>
#include <unistd.h>
#include "tk_public.h"

//volatile char Debug_Level=INFO;
extern volatile char Debug_Level;

/*************************************************
*  Function:       // tk_system
*  Description:    // 通过输入字符串 执行系统指令
*  Input:          // *cpCmd	:指令字符串
*                  // 
*  Output:         // 无
*  Return:         // TK_SUCCESS：成功   TK_FAILURE : 失败
*  Others:         // 其它说明
*************************************************/
tk_int32 tk_system(const tk_int8 *cpCmd)
{
       if(NULL == cpCmd)
       {
              printf("In %s, NULL == cpCmd. failed !!!!!\n", __func__);
		return TK_PARAM_NULL;
	}

   
       __sighandler_t old_handler = signal(SIGCHLD, SIG_DFL); 
       tk_int32 lRet = system(cpCmd); 

       signal(SIGCHLD, old_handler);
   
       if (-1 == lRet)  
       {  
              printf("In %s, fork failed. error error !!!!! cpCmd = %s \n!", __func__, cpCmd);  
		return TK_FAILURE;
       }  
       else  
       {  
              if (WIFEXITED(lRet))  
              {  
                      if (0 == WEXITSTATUS(lRet))  
                      {  
                             return TK_SUCCESS;
                      }  
                      else  
			 {  
                             printf("In %s, run shell script :%s failed, script exit ret = %d \n", __func__, cpCmd, WEXITSTATUS(lRet));  
				 return TK_FAILURE;
                      }  
               }  
               else  
               {  
                      printf("In %s, cpCmd = %s, exit ret = [%d] \n", __func__, cpCmd, WEXITSTATUS(lRet));  
			 return TK_FAILURE;
               }  
       }  
	   
       return TK_SUCCESS;
}


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
tk_int32 tk_popen(tk_int8 *cpCmd, tk_int32 lCmdLen, tk_int8 *cpBuf, tk_int32 *lpBufLen)
{
       if(NULL==cpCmd || NULL==cpBuf || NULL==lpBufLen)
       {
              printf("In tk_popen, NULL==cpCmd || NULL==cpBuf || NULL==lpBufLen. failed !!!!!\n");
		return TK_PARAM_NULL;
	}
	   
	tk_int32 lReadLen = -1;
	tk_int8 *cpReadBuf = (tk_int8 *)malloc(TK_READ_MAX_LEN);
	if(NULL ==  cpReadBuf)
	{
              printf("In tk_popen, malloc memory space failed . error error !!!!! \n");
		return TK_FAILURE;
	}
	memset(cpReadBuf, 0x0, TK_READ_MAX_LEN);

    
	FILE *pStream = popen(cpCmd, "r" );
	if(NULL != pStream)
	{
	   
		lReadLen = fread(cpReadBuf, sizeof(char), TK_READ_MAX_LEN, pStream); 
		pclose( pStream );
		cpReadBuf[lReadLen-1] = 0;  //
		*lpBufLen = lReadLen;
		memcpy(cpBuf, cpReadBuf, lReadLen);
		free(cpReadBuf);

		return TK_SUCCESS;
	}

	*lpBufLen = -1;
	free(cpReadBuf);

       return TK_SUCCESS;
}


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
tk_int32 tk_get_filelen(const tk_int8 *cpFile, tk_int32 *lpLen)
{
       if(NULL == cpFile || NULL == lpLen)
       {
              printf("In tk_get_filelen, NULL == cpFile || NULL == lpLen !!!!! \n");
	       return TK_PARAM_NULL;
	}

	struct stat t_Stat;
	memset(&t_Stat, 0x0, sizeof(struct stat));
       stat(cpFile, &t_Stat);  
       *lpLen = t_Stat.st_size; 

       return TK_SUCCESS;
}


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
tk_int32 tk_read_filelen(const tk_int8 * cpFile, tk_int32 *lpLen)
{
       if(NULL == cpFile || NULL == lpLen)
       {
              printf("In tk_read_filelen, NULL == cpFile || NULL == lpLen !!!!! \n");
	       return TK_PARAM_NULL;
	}

       
       FILE *fp = fopen(cpFile, "rb");  
       if(NULL == fp)
	{
              printf("In tk_read_filelen, open file = %s failed . error error !!!!! \n", cpFile); 
              return TK_FAILURE;
	}

      
	if(0 != fseek(fp, 0L, SEEK_END) )
	{
              printf("In tk_read_binary, fseek failed . error error !!!!! \n"); 
              perror("fseek");	    
	       	  fclose(fp);
              return TK_FAILURE;
	}
	
      
       *lpLen = ftell(fp);  
	   
     
       if( 0 != fclose(fp) )
       {
            printf("In tk_read_filelen, fclose file = %s failed . error error !!!!! \n", cpFile); 
			perror("fclose");
			return TK_FAILURE;
       }

       return TK_SUCCESS;
}


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
tk_int32 tk_read_binary(const tk_int8 *cpFile, tk_int32 lOffset, tk_int32 lLen, tk_int8 **cppContent)
{
    if(NULL == cpFile || NULL == *cppContent)
    {
       		printf("In tk_read_binary, NULL == cpFile || NULL == cppContent !!!!! \n");
			return TK_PARAM_NULL;
	}


	tk_int32 lRetLen = 0;
	tk_int32 lFileLen = 0;
	tk_get_filelen(cpFile, &lFileLen);
	if(lOffset > lFileLen)
	{
              printf("In tk_read_binary, lOffset > lFileLen. error error !!!!! lOffset = %d , lFileLen = %d \n", lOffset, lFileLen); 
	       return TK_FAILURE;
	}

	if(-1 == lLen)
	{
              lRetLen = lFileLen - lOffset;;
	}  	
	else
	{
              if( (lOffset+lLen) > lFileLen )
              {
                     lRetLen = lFileLen - lOffset;
              }
              else
              {
                     lRetLen = lLen;
              }
	}
	

	FILE *fp = fopen(cpFile, "rb");
	if(NULL == fp)
	{

              printf("In tk_read_binary, open cpFile = %s failed . error error !!!!! \n", cpFile); 
              return TK_FAILURE;
	}


	if(0 != fseek(fp, lOffset, SEEK_SET) )
	{
              printf("In tk_read_binary, fseek failed . error error !!!!! lOffset = %d \n", lOffset); 
              perror("fseek");
	       fclose(fp);
              return TK_FAILURE;
	}


	if( lRetLen != fread(*cppContent, 1, lRetLen, fp) )
	{
              printf("In tk_read_binary, fread failed . error error !!!!! lRetLen = %d \n", lRetLen); 
              perror("fread");
	       fclose(fp);
              return TK_FAILURE;
	}


       if( 0 != fclose(fp) )
       {
              printf("In tk_read_binary, fclose file = %s failed . error error !!!!! \n", cpFile); 
		perror("fclose");
		return TK_FAILURE;
       }
	
       return lRetLen;
}


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
tk_int32 tk_write_binary(const tk_int8 *cpFile, tk_int32 lOffset, tk_int8 *cpContent, tk_int32 lLen)
{
       if(NULL == cpFile || NULL == cpContent)
       {
              printf("In tk_write_binary, NULL == cpFile || NULL == cpContent !!!!! \n");
	       return TK_PARAM_NULL;
	}


	tk_int32 lPos = 0;
	tk_int32 lRet = 0;   
	tk_int32 lFileLen = 0;
	tk_get_filelen(cpFile, &lFileLen);


       if(-1 == lOffset)
       {
              lPos = lFileLen;
	}
	else
	{
              if(lOffset > lFileLen)
              {
                     lPos = lFileLen;
              }
              else
              {
                     lPos = lOffset;
              }
	}
	

	FILE *fp = fopen(cpFile, "ab");
	if(NULL == fp)
	{
              printf("In tk_write_binary, open file = %s failed . error error !!!!! \n", cpFile); 
              return TK_FAILURE;
	}


	if(0 != fseek(fp, lPos, SEEK_SET) )
	{
              printf("In tk_write_binary, fseek failed . error error !!!!! lPos = %d \n", lPos); 
              perror("fseek");	  
	       fclose(fp);
              return TK_FAILURE;
	}


	lRet = fwrite(cpContent, 1, lLen, fp);
	if( lRet != lLen )
	{
          printf("In tk_write_binary, fwrite failed . error error !!!!! lRet = %d, lLen = %d \n", lRet, lLen); 
          perror("fread");
	      fclose(fp);
          return TK_FAILURE;
	}


       if( 0 != fclose(fp) )
       {
              printf("In tk_write_binary, fclose file = %s failed . error error !!!!! \n", cpFile); 
		perror("fclose");
		return TK_FAILURE;
       }
	   
       return lRet;
}


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
tk_int32 tk_copy_binary(const tk_int8 *cpDstFile, const tk_int8 *cpSrcFile)
{
       if(NULL == cpDstFile || NULL == cpSrcFile)
       {
              printf("In tk_copy_binary, NULL == pDstFile || NULL == cpSrcFile !!!!! \n");
	       return TK_PARAM_NULL;
	}
	
	tk_int32 lSrcLen = 0;
       tk_int8 *cpData = (tk_int8 *)malloc(TK_MAX_DATA_LEN);
	if(NULL ==  cpData)
	{
              printf("In tk_copy_binary, malloc memory space failed . error error !!!!! \n");
		return TK_FAILURE;
	}
	

	tk_get_filelen(cpSrcFile, &lSrcLen);


	FILE *fpSrc = fopen(cpSrcFile, "rb");
	if(NULL == fpSrc)
	{
              printf("In tk_copy_binary, open src file = %s failed . error error !!!!! \n", cpSrcFile); 
              return TK_FAILURE;
	}


	FILE *fpDst = fopen(cpDstFile, "wb");
	if(NULL == cpDstFile)
	{
              printf("In tk_copy_binary, open dst file = %s failed . error error !!!!! \n", cpDstFile); 
	       fclose(fpSrc);
              return TK_FAILURE;
	}


       while(lSrcLen)
	{
		memset(cpData, 0x00, TK_MAX_DATA_LEN);
		if(TK_MAX_DATA_LEN >= lSrcLen)
		{
			fread(cpData, 1, lSrcLen, fpSrc);
			fwrite(cpData, 1, lSrcLen, fpDst);
			lSrcLen = 0;
		}
		else
		{
			fread(cpData, 1, TK_MAX_DATA_LEN, fpSrc);
			fwrite(cpData, 1, TK_MAX_DATA_LEN, fpDst);
			lSrcLen -= TK_MAX_DATA_LEN;
		}
	}


       free(cpData);
       cpData = NULL;
	   

       if( 0 != fclose(fpSrc) )
       {
              printf("In tk_copy_binary, fclose src file = %s failed . error error !!!!! \n", cpSrcFile); 
		perror("fclose");
	       fclose(fpDst);
		return TK_FAILURE;
       }


       if( 0 != fclose(fpDst) )
       {
              printf("In tk_copy_binary, fclose dst file = %s failed . error error !!!!! \n", cpDstFile); 
		perror("fclose");
		return TK_FAILURE;
       }
	   
       return TK_SUCCESS;
}


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
tk_int32 tk_copy_ascii(const tk_int8 *cpDstFile, const tk_int8 *cpSrcFile)
{
       if(NULL == cpDstFile || NULL == cpSrcFile)
       {
              printf("In tk_copy_ascii, NULL == cpDstFile || NULL == cpSrcFile !!!!! \n");
	       return TK_PARAM_NULL;
	}


	FILE *fpSrc = fopen(cpSrcFile, "r");
	if(NULL == fpSrc)
	{
              printf("In tk_copy_binary, open src file = %s failed . error error !!!!! \n", cpSrcFile); 
              return TK_FAILURE;
	}

  
	FILE *fpDst = fopen(cpDstFile, "w");
	if(NULL == cpDstFile)
	{
              printf("In tk_copy_binary, open dst file = %s failed . error error !!!!! \n", cpDstFile); 
	       fclose(fpSrc);
              return TK_FAILURE;
	}

  
	tk_int8 data = 0x00;
       while ( EOF != (data=fgetc(fpSrc)) )
       {
              fputc(data, fpDst);
	}
	   
  
       if( 0 != fclose(fpSrc) )
       {
              printf("In tk_copy_binary, fclose src file = %s failed . error error !!!!! \n", cpSrcFile); 
		perror("fclose");
	       fclose(fpDst);
		return TK_FAILURE;
       }

       if( 0 != fclose(fpDst) )
       {
              printf("In tk_copy_binary, fclose dst file = %s failed . error error !!!!! \n", cpDstFile); 
		perror("fclose");
		return TK_FAILURE;
       }
	   
       return TK_SUCCESS;
}


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
int Check_OverTime(unsigned int time,unsigned int  OverTime)
{
	struct timeval tv;
	unsigned int System_sec=0;
    gettimeofday(&tv,NULL);
	System_sec=tv.tv_sec;
	if(System_sec>=time)
	{
		if((System_sec-time)>=OverTime)
		{
			return 1;
		}
	}
	else
	{
		if((0xffffffff-time+System_sec)>=OverTime)
		{
			return 1;
		}
	}

	return 0;

}


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
int Check_OverTimeMsec(unsigned int sec,unsigned int msec,unsigned int  OverTime)
{
	struct timeval tv;
	unsigned int System_sec=0;
	unsigned int System_msec=0;
	int diff=0;
    gettimeofday(&tv,NULL);
	System_sec=tv.tv_sec;
	System_msec=tv.tv_usec/1000;

	if(System_sec>=sec)
	{
		diff=System_sec-sec;
		diff*=1000;
		diff+=(System_msec-msec);
		if(diff>=OverTime)
		{
			return 1;
		}
	}
	else
	{
		diff=sec-System_sec;
		diff*=1000;
		diff+=(msec-System_msec);
		if(diff>=OverTime)
		{
			return 1;
		}

	}

	return 0;

}









/*************************************************
*  Function:       // BCD2HEX
*  Description:    // 
*  Input:          // ucBCD
*                  // 
*  Output:         // 无
*  Return:         // BCD
*  Others:         // 其它说明
*************************************************/
unsigned char BCD2HEX(unsigned char ucBCD)
{
     return ((ucBCD & 0xf0)>>4)*10+(ucBCD & 0x0f);
}




/*************************************************
*  Function:       // HEX2BCD
*  Description:    // 
*  Input:          // ucHEX
*                  // 
*  Output:         // 
*  Return:         // 16进制
*  Others:         // 其它说明
*************************************************/
unsigned char HEX2BCD(unsigned char ucHEX)
{
    return ((ucHEX/10)<<4)+(ucHEX%10);
}

/*************************************************
*  Function:       // GET_SYSTEM_SECOND
*  Description:    // 获取系统当前秒数
*  Input:          // 
*                  // 
*  Output:         // 无
*  Return:         // 当前秒数
*  Others:         // 其它说明
*************************************************/
unsigned int GET_SYSTEM_SECOND(void)
{
	struct timeval tv;
    gettimeofday(&tv,NULL);
	return tv.tv_sec;

}

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
void GET_SYSTEM_MSECOND(unsigned int* sec,unsigned int* msec)
{
	struct timeval tv;
    gettimeofday(&tv,NULL);
	*sec=tv.tv_sec;
	*msec=tv.tv_usec/1000;
}


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
unsigned int Time2UTC(struct rtctm RTC_St)
{
	struct tm tmDateTime;

	tmDateTime.tm_year=(RTC_St.CLKYearL>>4)*10+(RTC_St.CLKYearL & 0x0f)+100;
	tmDateTime.tm_mon=(RTC_St.CLKMonth>>4)*10+(RTC_St.CLKMonth & 0x0f)-1;
	tmDateTime.tm_mday=(RTC_St.CLKDate>>4)*10+(RTC_St.CLKDate & 0x0f);
	tmDateTime.tm_hour=(RTC_St.CLKHour>>4)*10+(RTC_St.CLKHour & 0x0f);
	tmDateTime.tm_min=(RTC_St.CLKMin>>4)*10+(RTC_St.CLKMin & 0x0f);
	tmDateTime.tm_sec=(RTC_St.CLKSec>>4)*10+(RTC_St.CLKSec & 0x0f);
	return mktime( & tmDateTime);
}

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
unsigned char UTC2Time(unsigned int ulUTC,struct rtctm *RTC_St)
{
	struct tm tmpDateTime;
    unsigned long	tempTM;
	unsigned char ucTmp;

	localtime_r((const time_t *)&ulUTC,&tmpDateTime);
	RTC_St->CLKYearH=0x20;
	RTC_St->CLKYearL=DF_HEX2BCD((tmpDateTime.tm_year-100));
	ucTmp=tmpDateTime.tm_mon+1;   
	RTC_St->CLKMonth=DF_HEX2BCD(ucTmp);
	RTC_St->CLKDate=DF_HEX2BCD(tmpDateTime.tm_mday);
	RTC_St->CLKHour=DF_HEX2BCD(tmpDateTime.tm_hour);
	RTC_St->CLKMin=DF_HEX2BCD(tmpDateTime.tm_min);
	RTC_St->CLKSec=DF_HEX2BCD(tmpDateTime.tm_sec);

	tempTM= Time2UTC((struct rtctm)*RTC_St); 
    if(tempTM != ulUTC)
    {
       return 0;
    }
    else
    {
	   return 1;
    }
}


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
int ReadFileList(char *basePath,struct FileList *FileList_ST)
{
    
    DIR *dir;
    struct dirent *ptr;
 
    if ((dir=opendir(basePath)) == NULL)
    {
		
        return -1;

    }
 
	memset(FileList_ST,0,sizeof(struct FileList));

    while ((ptr=readdir(dir)) != NULL)
    {
		if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
  		{
			continue;
		}
        else if((ptr->d_type == 8)&&((strlen(basePath)+strlen(ptr->d_name))<FileList_MAX_LEN)&&(FileList_ST->FileNum<FileList_MAX_NO))    ///file
   		{
	    	strcpy(FileList_ST->FileName[FileList_ST->FileNum],basePath);
        	strcat(FileList_ST->FileName[FileList_ST->FileNum],ptr->d_name);
			FileList_ST->FileNum++;
	    }
        else 
        {
	    	continue;
        }
    }
    closedir(dir);
    return 0;
}


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
void PRINTF_INIT(void)
{
    if(access("/opt/VD1050_DEBUG",0)==0)
	{
		Debug_Level=DEBUG;
	}
	return;

}


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
void PRINTF_BUF(int level,const char *HeadStr,const unsigned char *data,int dataLen)
{
    int datal;
	
	if(level>Debug_Level)
	{
		return;		
	}
	
    PRINTF(level,"%s",HeadStr);
    for(datal=0;datal<dataLen;datal++)
    {
        printf("%02x",data[datal]);
    }

	printf(",LEN=%d\n",dataLen);
	return;

}


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
time_t GetFileTime(const char *pcFileName,char cType)
{
	struct stat buf;
	int result;

	if(pcFileName==NULL)
	{
		return -1;
	}


	result=stat(pcFileName,&buf);

	if(result!=0)
	{
		return -1;
	}
	else
	{
		if(1==cType)
		{
			return (buf.st_ctime);
		}
		else if(2==cType)
		{
			return (buf.st_mtime);
		}
		else if(3==cType)
		{
			return (buf.st_atime);
		}
		else
		{
			return -1;
		}
	}
	
}




