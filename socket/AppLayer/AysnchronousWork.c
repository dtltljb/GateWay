//标准输入输出头文件
#include <stdio.h>
#include <stdint.h>
//文件操作函数头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/file.h>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define			Aysn_Debug_Level			1

#include 			"AysnchronousWork.h"
#include 			"socket/socket_protocol_thread.h"
#include 			"socket/Unpacket.h"
#include 			"socket/VirtualPortMonitor.h"
#include 			"socket/protoBuf/epb_report_log_pack.h"
#include 			"socket/protoBuf/epb_remote_open_pack.h"
#include 			"socket/protoBuf/epb_remote_load_pack.h"
#include 			"socket/protoBuf/epb_remote_authorize_pack.h"
#include 			"socket/protoBuf/epb_remote_config_pack.h"

#include 			"uart/Cmd/report_log_pack.h"
#include 			"uart/Cmd/remote_open_pack.h"

void	AysnchronousWorkProcessFuction(void	*args)
{
	static	uint8_t			cmd;
    struct VirtualSocket *pVirtualSocket			=	args;
      ReportLogRespPublic			*pReportLogRespPublic		=	pVirtualSocket->pContext.p;

		#if		(Aysn_Debug_Level	==	1 )
		printf("%s,line:%d,last_Work_type=%d \n",__FILE__,__LINE__,pReportLogRespPublic->Last_Work_Type);
		#endif
		cmd			=	pReportLogRespPublic->Last_Work_Type;
    switch(cmd)
    {
    	case		remote_open_door:
    		break;
    	case		remote_authorize_cmd:
    		break;
    	case		remote_load_cmd:
    		break;
    	case		remote_config_cmd:
    		break;    		    		 
    	default:
    		break;   		
    }
	
}
