


/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_FILE_MANAGER_H)
#define _FILE_MANAGER_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#include 				<stdint.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <fcntl.h>

#include 		"uart/Cmd/report_log_pack.h"

#define		File_Max_Size				2048


typedef	struct{
	UReportLogReq				logContext;
	char								logTime[20];
	char								logChannel[32];
}LogRecord_S;

extern char	*STOREPATH;			//log data record store path,globle
/*
*		Run_Log_Context_Record
*	input:	void	*args		,VirtualSocket
*					void	*argu		,UartPort
*	output:	sucess	is 1
******************************************************************/
int32_t	Run_Log_Context_Record(void	*args,void	*argu);


/*
*		monitor_link_list_empty
*	brief:,;
*				,.
******************************************************************/
int32_t	monitor_link_list_empty(void);


/*
*		function:	report_history_log_file
*		para	[in]:	char	*fileName,history	log file name
*								uint32_t fileLine,	report file line	
*		para	[out]:non 0 failure, = 1 is file empty, = 0 success
******************************************************************/
int32_t	report_history_log_file(char	*fileName,uint32_t *fileLine);

int32_t	monitor_forlder_file_empty(void);

/*
*		config_file_context_record
*		,,
*		./conf/AutoManger.txt
*	input:	void	*args		,VirtualSocket
*	output:	sucess	is 1
******************************************************************/
uint8_t	config_file_context_record(void *args);

#endif
#ifdef __cplusplus
     }
#endif
