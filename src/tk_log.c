/***********************************
* Copyright (c) 2017,天津通卡
* All rights reserved.
* 文件名称：tk_log.c
* 文件标识：
* 摘要：实现日志的维护
*************************************/
/* ***********************************
* 作者：孟维良，修改日期：2017年9月1日
* 修改简要说明：
* 2017/9/18	：增加修改配置文件功能函数 SetConfigFileStringValue
**************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/time.h>
#include <time.h>
#include"tk_log.h"
#include "tk_public.h"

volatile char Debug_Level = INFO;

///log  define

unsigned int  	TCPS_LogSize=0;				//日志文件大小
unsigned int  	TCPS_LogLevel      = 0;    // 写入日志的错误等级设置
unsigned int  	TCPS_LogPosition   = 0;    // 0:不打印 位置信息   1:打印错误位置信息
unsigned char   TCPS_LogFile1[100] = {0};  // 日志文件1
unsigned char   TCPS_LogFile2[100] = {0};  // 日志文件2

unsigned char  	Read_LogConfig_Flag=0;///是否已经读取配置信息标志 1 yes   


/*************************************************
*  Function:       // get_file_size
*  Description:    // 获取文件长度
*  Input:          // *path	：文件路径
*                  // 
*                  //
*  Output:         //
*                  //
*  Return:         // －1：失败    >=0：长度
*  Others:         // 其它说明
*************************************************/
unsigned long get_file_size(const char *path)  
{  
    unsigned long filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;  
} 

/*************************************************
*  Function:       // GetCompletePath
*  Description:    // 获取文件的绝对路径
*  Input:          // *FileName		：文件名
*                  // 
*  Output:         // *WholePath	:绝对路径
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
void GetCompletePath(char *FileName,char *WholePath)
{
    char  CurPath[256] = {0};
	char  sWholePath[256] = {0};
    // 先对输入参数进行异常判断
    if (FileName == NULL || WholePath == NULL)
    {
        printf("GetCompletePath: input parameter(s) is NULL!\n");
        return;
    }

    getcwd(CurPath,sizeof(CurPath));

    // 拼装文件路径
    snprintf(sWholePath,sizeof(sWholePath)-1,"%s/%s", CurPath, FileName);
	strncpy(WholePath, sWholePath, strlen(sWholePath));
}


/*************************************************
*  Function:       // GetStringContentValue
*  Description:    // 获取 某一项关键字值
*  Input:          // *fp		：文件句柄
*                  // *SectionName	:块名称  一个块包含若干关键字
*                  // *KeyName	:关键字名称
*  Output:         // *Output	:关键字值
*                  // *OutputLen	:关键字长度
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
int GetStringContentValue(FILE *fp, char *SectionName,char *KeyName, char *Output,unsigned int OutputLen)
{
	char  sSectionName[100]    = {0};
   	char  sKeyName[100] = {0};
   	char  Line[256]    = {0};
   	char  LineBak[256] = {0};
    unsigned int 	iLineLen       = 0;
    unsigned int 	iPosFlag       = 0;

    // 
    if (fp == NULL || SectionName == NULL || KeyName == NULL || Output == NULL)
    {
        printf("GetStringContentValue: input parameter(s) is NULL!\n");
        return -1;
    }

	
	sprintf(sSectionName, "[%s]", SectionName);
    strcpy(sKeyName, KeyName);

    while (feof(fp) == 0)
    {
        memset(Line, 0x00, sizeof(Line));
        fgets((char *)Line, sizeof(Line), fp);      // read line

        // check the line head
        if (Line[0] == '#' || Line[0] == '\r' || Line[0] == '\n' || Line[0] == '\0')
        {
            continue;
        }

		//find sectionname
		if (strncasecmp((char *)sSectionName, Line, strlen((char *)sSectionName)) == 0)     
		{
			 while (feof(fp) == 0)
			{

				memset(Line, 0x00, sizeof(Line));
				memset(LineBak, 0x00, sizeof(LineBak));
        		fgets((char *)Line, sizeof(Line), fp);      // read line

				if (Line[0] == '#' || Line[0] == '\r' || Line[0] == '\n' || Line[0] == '\0')
				{
				    continue;
				}
         		memcpy((char *)LineBak, (char *)Line, strlen(Line));

				 //find keyname
				if (strncasecmp((char *)sKeyName, LineBak, strlen((char *)sKeyName)) == 0)     
				{
					iLineLen = strlen(Line);
					for (iPosFlag = strlen((char *)sKeyName); iPosFlag <= iLineLen; iPosFlag++)
				    {
				    	if (Line[iPosFlag] == ' ')
				   		{
				      		continue;
				    	}
				     	if (Line[iPosFlag] == '=')
				     	{
				     		break;
				   		}

				  		iPosFlag = iLineLen + 1;
				  		break;
				     }

		   			iPosFlag = iPosFlag + 1;    // skip =

					if (iPosFlag > iLineLen)
				 	{
						continue;
		   			}

				    memset(Line, 0x00, sizeof(Line));
				    strcpy(Line, LineBak + iPosFlag);

				    
				   	for (iPosFlag = 0; iPosFlag < strlen(Line); iPosFlag ++)
			   		{
				  		if (Line[iPosFlag] == '\r' || Line[iPosFlag] == '\n' || Line[iPosFlag] == '\0')
				      	{
				      		Line[iPosFlag] = '\0';
				       		break;
				  		}
				  	}

			   
					if(strlen(Line)>=OutputLen)
					{
			  			strncpy((char *)Output, Line, OutputLen-1);
					}	
					else
					{
			  			strncpy((char *)Output, Line, strlen(Line));
						Output[strlen(Line)]=0;
					}
					return 0;
			 		
			 	}
				else if (Line[0] == '[')
				{
				    continue;
				}
			}
			break;
		}

    }
	return -1;
}


/*************************************************
*  Function:       // SetStringContentValue
*  Description:    // 设置某一项关键字值
*  Input:          // *fp		：文件句柄
*                  // *SectionName	:块名称  一个块包含若干关键字
*                  // *KeyName	:关键字名称
*  Output:         // *Input	:关键字值
*                  // *InputLen	:关键字长度
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/
int SetStringContentValue(FILE *fp, char *SectionName,char *KeyName, char *Input,unsigned int InputLen)
{
	char  sSectionName[100]    = {0};
   	char  sKeyName[100] = {0};
   	char  Line[100][100];
   	char  LineBak[100] = {0};
    unsigned int 	iLineLen= 0;
    unsigned int 	iPosFlag= 0;

	FILE *wp=NULL;
	int fd=-1;
	unsigned int line_max=0;//总行数  不超过100
	unsigned int line_pos=0;//查找行定位

    if(fp == NULL  || SectionName == NULL || KeyName == NULL || Input == NULL)
	{
        printf("SetStringContentValue: input parameter(s) is NULL!\n");
        return -1;
    }
	
	sprintf(sSectionName,"[%s]",SectionName);
    strcpy(sKeyName,KeyName);

    while (feof(fp) == 0)
    {
        memset(Line[line_max], 0x00, 100);
        fgets((char *)Line[line_max], 100, fp);      // read line
		

        // check the line head
        if (Line[line_max][0] == '#' || Line[line_max][0] == '\r' || Line[line_max][0] == '\n' || Line[line_max][0] == '\0')
        {
			line_max++;
            continue;
        }

		//find sectionname
		if (strncasecmp((char *)sSectionName, Line[line_max], strlen((char *)sSectionName)) == 0)     
		{
			line_pos=line_max;
			line_max++;
			while (feof(fp) == 0)
			{
				memset(Line[line_max], 0x00, 100);
        		fgets((char *)Line[line_max], 100, fp);      // read line
				
				memset(LineBak, 0x00, sizeof(LineBak));
        		

				if (Line[line_max][0] == '#' || Line[line_max][0] == '\r' || Line[line_max][0] == '\n' || Line[line_max][0] == '\0')
				{
					line_max++;
				    continue;
				}
         		memcpy((char *)LineBak, (char *)Line[line_max], strlen(Line[line_max]));

				 //find keyname
				if (strncasecmp((char *)sKeyName, LineBak, strlen((char *)sKeyName)) == 0)     
				{
					iLineLen = strlen(Line[line_max]);
					for (iPosFlag = strlen((char *)sKeyName); iPosFlag <= iLineLen; iPosFlag++)
				    {
				    	if (LineBak[iPosFlag] == ' ')
				   		{
				      		continue;
				    	}
				     	if (LineBak[iPosFlag] == '=')
				     	{
				     		break;
				   		}

				  		iPosFlag = iLineLen + 1;
				  		break;
				     }

		   			iPosFlag = iPosFlag + 1;    // skip =

					if (iPosFlag > iLineLen)
				 	{
						line_max++;
						continue;
		   			}

				   	line_pos=line_max;
					line_max++;
					continue;
			 		
			 	}
				else if (Line[line_max][0] == '[')
				{
					line_max++;
				    continue;
				}
				line_max++;
				
			}
			break;
		}
		line_max++;

    }
	//printf("linemax:%d,pos:%d\n",line_max,line_pos);
	if((line_pos!=0)&&(line_max!=0))
	{

		wp=fopen("/opt/LIB_CONFIG.txt", "w+");
		if (wp == NULL)
		{
		    printf("SetStringContentValue: write LIB_CONFIG.txt failed!\n");
		    return -1;
		}
		fseek(fp,0,SEEK_SET);/// 清空文件

		for(iPosFlag=0;iPosFlag<line_max;iPosFlag++)
		{
			if(iPosFlag==line_pos)
			{
				if(strncasecmp((char *)sSectionName, Line[iPosFlag], strlen((char *)sSectionName)) == 0)  
				{
					//printf("write line %d:%s\n",iPosFlag,Line[iPosFlag]);
					fwrite(Line[iPosFlag],strlen(Line[iPosFlag]),1,wp);

					if((iPosFlag+1)==line_max)fwrite("\n",1,1,wp);//最后一行加换行符

					memset(LineBak,0,sizeof(LineBak));
					sprintf(LineBak,"%s=%s\n",(char *)sKeyName,Input);
					//printf("add new line %d\n",strlen(LineBak));
					fwrite(LineBak,strlen(LineBak),1,wp);
					continue;
				}
				else if (strncasecmp((char *)sKeyName, Line[iPosFlag], strlen((char *)sKeyName)) == 0)  
				{
					memset(Line[iPosFlag],0,100);
					sprintf(Line[iPosFlag],"%s=%s\n",(char *)sKeyName,Input);
				}
			}
			//printf("write line %d\n",iPosFlag);
			fwrite(Line[iPosFlag],strlen(Line[iPosFlag]),1,wp);
			
		}
		fflush(wp);

    	fclose(wp);
		return 0;
	}
	else if(line_pos==0)
	{

		wp=fopen("/opt/LIB_CONFIG.txt", "w+");
		if (wp == NULL)
		{
		    printf("SetStringContentValue: write LIB_CONFIG.txt failed!\n");
		    return -1;
		}
		fseek(fp,0,SEEK_SET);/// 清空文件

		for(iPosFlag=0;iPosFlag<line_max;iPosFlag++)
		{
			if(iPosFlag==line_pos)
			{
				if(strncasecmp((char *)sSectionName, Line[iPosFlag], strlen((char *)sSectionName)) == 0)  
				{
					//printf("write line %d:%s\n",iPosFlag,Line[iPosFlag]);
					fwrite(Line[iPosFlag],strlen(Line[iPosFlag]),1,wp);

					if((iPosFlag+1)==line_max)fwrite("\n",1,1,wp);//最后一行加换行符

					memset(LineBak,0,sizeof(LineBak));
					sprintf(LineBak,"%s=%s\n",(char *)sKeyName,Input);
					//printf("add new line %d\n",strlen(LineBak));
					fwrite(LineBak,strlen(LineBak),1,wp);
					line_pos=1;
					continue;
				}
				else if (strncasecmp((char *)sKeyName, Line[iPosFlag], strlen((char *)sKeyName)) == 0)  
				{
					memset(Line[iPosFlag],0,100);
					sprintf(Line[iPosFlag],"%s=%s\n",(char *)sKeyName,Input);
				}
			}
			//printf("write line %d\n",iPosFlag);
			fwrite(Line[iPosFlag],strlen(Line[iPosFlag]),1,wp);
			
		}

		if(line_pos == 0)  
		{
			fwrite("\n",1,1,wp);//最后一行加换行符
			memset(LineBak,0,sizeof(LineBak));
			sprintf(LineBak,"%s\n",(char *)sSectionName);
			fwrite(LineBak,strlen(LineBak),1,wp);

			memset(LineBak,0,sizeof(LineBak));
			sprintf(LineBak,"%s=%s\n",(char *)sKeyName,Input);
			fwrite(LineBak,strlen(LineBak),1,wp);
			
		}


		fflush(wp);

    	fclose(wp);
		return 0;

	}
	return -1;
}



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
void GetConfigFileStringValue(char *SectionName, char *KeyName, char *pDefaultVal, char *Output, unsigned  int iOutputLen,char *ConfigFileName)
{
    FILE  *fp                    = NULL;
    char  szWholePath[256]      = {0};

    // 
    if (SectionName == NULL || KeyName == NULL || Output == NULL || ConfigFileName == NULL)
    {
        printf("GetConfigFileStringValue: input parameter(s) is NULL!\n");
        return;
    }

    // 
    if (pDefaultVal == NULL)
    {
        strcpy(Output, "");
    }
    else
    {
        strcpy(Output, pDefaultVal);
    }

    //
    //GetCompletePath(pszConfigFileName, szWholePath);
    fp = fopen(ConfigFileName, "r");
    if (fp == NULL)
    {
        printf("GetConfigFileStringValue: open %s failed!\n", szWholePath);
        return;
    }

    // 
    GetStringContentValue(fp, SectionName, KeyName, Output, iOutputLen);
	
    // 
	fflush(fp);

    fclose(fp);
    fp = NULL;

}



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
void SetConfigFileStringValue(char *SectionName, char *KeyName, char *Input, unsigned  int iInputLen,char *ConfigFileName)
{
    FILE  *fp                    = NULL;
    char  szWholePath[256]      = {0};
	int ret=-1;
	int fd=-1;
    // 
    if (SectionName == NULL || KeyName == NULL || Input == NULL || ConfigFileName == NULL)
    {
        printf("SetConfigFileStringValue: input parameter(s) is NULL!\n");
        return;
    }

	if(-1==access(ConfigFileName,0))
	{
		memset(szWholePath,0,sizeof(szWholePath));
		sprintf(szWholePath,":>/opt/LIB_CONFIG.txt");
		tk_system(szWholePath);

		memset(szWholePath,0,sizeof(szWholePath));
		sprintf(szWholePath,"wr cp -f /opt/LIB_CONFIG.txt  %s",ConfigFileName);
		tk_system(szWholePath);
	}
	
    //
    //GetCompletePath(pszConfigFileName, szWholePath);
    fp = fopen(ConfigFileName, "r");
    if (fp == NULL)
    {
        printf("SetConfigFileStringValue: open %s failed!\n", ConfigFileName);
        return;
    }

    // 
    ret=SetStringContentValue(fp, SectionName, KeyName, Input, iInputLen);
	
    // 
	fflush(fp);

    fclose(fp);
    fp = NULL;

	if(ret==0)
	{
		memset(szWholePath,0,sizeof(szWholePath));
		sprintf(szWholePath,"wr cp -f /opt/LIB_CONFIG.txt  %s",ConfigFileName);
		tk_system(szWholePath);
	}
}




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
int GetConfigFileIntValue(char *SectionName, char *KeyName, unsigned  int iDefaultVal,char *ConfigFileName)
{
    char  GetValue[512] = {0};
    int  iGetValue       = 0;


    if (SectionName == NULL|| KeyName == NULL|| ConfigFileName == NULL)
    {
        printf("GetIntContentValue: input parameter(s) is NULL!\n");
        return -1;
    }

    GetConfigFileStringValue(SectionName,KeyName,NULL,GetValue,512-1,ConfigFileName);   

    if (GetValue[0] == '\0' || GetValue[0] == ';'|| GetValue[0] == '#')    //
    {
        iGetValue = iDefaultVal;
    }
    else
    {
        iGetValue = atoi((char *)GetValue);
    }

    return iGetValue;
}


/*************************************************
*  Function:       // Get_LOGConfigValue
*  Description:    // 读取 LOG 配置信息
*  Input:          // 无
*  Output:         // 无
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/ 
void Get_LOGConfigValue(void)
{
    unsigned char  LogDir[256] = {0};

    //
    TCPS_LogLevel = GetConfigFileIntValue("LOG", "LogLevel", 5, "/usr/VD1050/TCPS_VD1050.config");

    //
    TCPS_LogPosition = GetConfigFileIntValue("LOG", "LogPosition", 0, "/usr/VD1050/TCPS_VD1050.config");

	TCPS_LogSize = GetConfigFileIntValue("LOG", "LogSize", 1, "/usr/VD1050/TCPS_VD1050.config");
	if(TCPS_LogSize>=10)
	{
		TCPS_LogSize=10;
	}
	TCPS_LogSize*=1024*1024;
	
	//printf("LOG_CONFIG: level=%d,size=%d,posztion=%d\n",TCPS_LogLevel,TCPS_LogSize,TCPS_LogPosition);
    //
    GetConfigFileStringValue((char *)"LOG", (char *)"LogDir", (char *)"/opt/log", (char *)LogDir, sizeof(LogDir), (char *)"/usr/VD1050/TCPS_VD1050.config");
    snprintf((char *)TCPS_LogFile1, sizeof(TCPS_LogFile1)-1, "%s/TCPS_SYSLOG.log", LogDir);
	snprintf((char *)TCPS_LogFile2, sizeof(TCPS_LogFile2)-1, "%s/TCPS_SYSLOG2.log", LogDir);
	Read_LogConfig_Flag=1;
}


/*************************************************
*  Function:       // LogLevel
*  Description:    // 判断 log 错误等级
*  Input:          // iLogLevel	: 等级值
*  Output:         // 无
*  Return:         // 登记描述的字符串
*  Others:         // 其它说明
*************************************************/ 
char *LogLevel(unsigned int iLogLevel)
{
    switch (iLogLevel)
    {
        case LOG_FATAL:
        {
            return (char *)"FATAL";
        }

        case LOG_ERROR:   
        {
            return (char *)"ERROR";
        }

        case LOG_WARN :
        {
            return (char *)"WARN";
        }

        case LOG_INFO :
        {
            return (char *)"INFO";
        }

        case LOG_TRACE:   
        {
            return (char *)"TRACE";
        }

        case LOG_DEBUG:   
        {
            return (char *)"DEBUG";
        }

        case LOG_ALL:   
        {
            return (char *)"ALL";
        }

        default: 
        {
            return (char *)"OTHER";
        }
    }
}


/*************************************************
*  Function:       // GetTime
*  Description:    // 获取系统时间
*  Input:          // 无
*  Output:         // *pszTimeStr	:时间字符串
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/ 
void GetTime(unsigned char *pszTimeStr)
{
    struct tm      tSysTime     = {0};
    struct timeval tTimeVal     = {0};
    time_t         tCurrentTime = {0};

    unsigned char  szUsec[20] = {0};    // 微秒
    unsigned char  szMsec[20] = {0};    // 毫秒

    if (pszTimeStr == NULL)
    {
        return;
    }

    tCurrentTime = time(NULL);
    localtime_r(&tCurrentTime, &tSysTime);   // localtime_r

    gettimeofday(&tTimeVal, NULL);    
    sprintf((char *)szUsec, "%06d", (int)tTimeVal.tv_usec);  // 
    strncpy((char *)szMsec, (char *)szUsec, 3);                // 

    sprintf((char *)pszTimeStr, "[%04d.%02d.%02d %02d:%02d:%02d.%3.3s]", tSysTime.tm_year+1900, tSysTime.tm_mon+1, tSysTime.tm_mday,tSysTime.tm_hour, tSysTime.tm_min, tSysTime.tm_sec, szMsec);
}

/*************************************************
*  Function:       // LOGFILE_SIZE
*  Description:    // 判断LOG1文件大小，如果超过设置大小，log1重命名为log2，删除log1
*  Input:          // 无
*  Output:         // 无
*  Return:         // 无
*  Others:         // 其它说明
*************************************************/ 
void LOGFILE_SIZE(void)
{
	unsigned long file_len;
    file_len=get_file_size((char *)TCPS_LogFile1);
	if(file_len>TCPS_LogSize)
	{
                remove((char *)TCPS_LogFile2);
                rename((char *)TCPS_LogFile1,(char *)TCPS_LogFile2);
                remove((char *)TCPS_LogFile1);
	}
}



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
void WriteLogFile(char *FileName, const char *FunctionName, unsigned int iCodeLine, unsigned int iLogLevel,char *Content) 
{
    FILE  *fp                 = NULL;
    char  szLogContent[2048] = {0};
    char  szTimeStr[128]     = {0};
	int fd=-1;

	if(Read_LogConfig_Flag!=1)
	{
		Get_LOGConfigValue();
	}
	LOGFILE_SIZE();


    if (FileName == NULL || Content == NULL|| TCPS_LogSize==0)
    {
        return;
    }

	
    // 
    if (iLogLevel > TCPS_LogLevel)
    {
        return;
    }

    fp = fopen((char *)TCPS_LogFile1, "at+");      // 
    if (fp == NULL)
    {
        return;
    }

    // 
    GetTime((unsigned char *)szTimeStr);
    fputs(szTimeStr, fp);

    // 
    if (TCPS_LogPosition == 1)    // 
    {
        snprintf(szLogContent, sizeof(szLogContent)-1, "[%s][%s][%04d][%s]%s\n", FileName, FunctionName, iCodeLine, LogLevel(iLogLevel), Content);
    }
    else    // 
    {
        snprintf(szLogContent, sizeof(szLogContent)-1, "[%s]%s\n", LogLevel(iLogLevel), Content);
    }
    fputs(szLogContent, fp);

    fflush(fp);     // 
	///同步到磁盘
	fd=fileno(fp);

	if(fd!=-1)
	{
		fsync(fd);
	}


    fclose(fp);     // 
    fp = NULL;      //

    return;
}





