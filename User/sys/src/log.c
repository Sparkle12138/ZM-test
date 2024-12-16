#include "log.h"
#include <stdio.h>
#include <stdarg.h>

static const char *__LOG_LEVEL_NAME__[5] = 
	{"LOG-CLOSE","LOG-DEBUG","LOG-INFOR","LOG-WARNI","LOG-ERROR"};
static char __log_message_buff__[LOG_MESSAGE_BUFF_SIZE]={0};

/**
 *@function log output
 *@param 	const LOG_LEVEL level	��־�ȼ�
 *			const char *function	������־�ĺ�����
 *			const int  line			������־��Դ�ļ��е�����
 *			const char *format		��Ϣ��ʽ
 *@return 	void
 */	
void jmy_log(const LOG_LEVEL level,const char *function,
					const int line,const char *format,...)
{
	va_list arg = {0};
	if((unsigned int)level >= (unsigned int)LOG_OUTPUT_FILTER_LEVEL)
	{
		//compound log message
		unsigned int length = sprintf(__log_message_buff__,"[%s]-[%s:%d]:",
								__LOG_LEVEL_NAME__[level],function,line);
		va_start(arg,format);
		length += vsnprintf(&__log_message_buff__[length],
					LOG_MESSAGE_BUFF_SIZE-length,format,arg);			
		*(unsigned int*)(__log_message_buff__+length)= 0x0A0D;
		va_end(arg);
		
		//log message output 
		//getLogUsartHandle()->sendMsg(__log_message_buff__);
	}
}
