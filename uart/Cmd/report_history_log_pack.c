#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>

#include 		"report_log_pack.h"
#include 		"uart/UartPacket.h"
#include 		"uart/UartUnpacket.h"
#include 		"uart/socket_task_distributer.h"
#include		"uart/xxx_channel_send_thread.h"
#include    "uart/LinkLayer/UartOpration.h"
#include    "uart/LinkLayer/lora_parament.h"

#include    "socket/VirtualPortMonitor.h"
#include    "socket/pub/util.h"
#include    "bdb/FileManager.h"
#include 		"util/pthread_data_copy.h"


/***********************************************************************
 * @brief				report_history_log_pack
 * @param[in]		UReportLogReq *pUReq
 *							uint8_t channel
 * @return 			void
 **********************************************************************/

void	*report_history_log_pack(UReportLogReq *pUReq,uint8_t channel)
{
	BaseRequest 		reportLogReq_baseRequest;
	ReportLogReq		iReportLogReq;
	static	uint8_t	socketSendBuf[128],rdata[32];
  uint16_t	socketSendLength;
	int32_t	res;
	
	memset( (char*)&iReportLogReq,0,sizeof(ReportLogReq) );
	
  iReportLogReq.base_request		=	&reportLogReq_baseRequest;
  iReportLogReq.Locker_Address	=	t_ntohl(pUReq->LockerNumber);
  iReportLogReq.Last_Work_Type  = report_log_cmd;
	iReportLogReq.iLockerStates		=	pUReq->LockerStates; 
  iReportLogReq.Door_Status			=	openDoor;	 													//atuo load openDoor										
  
  iReportLogReq.Locker_Log_Time.str	=	(char*)pUReq->LogTime;
	iReportLogReq.Locker_Log_Time.len	=	sizeof(pUReq->LogTime);
  memcpy((char*)iReportLogReq.Locker_Log_Time.str,pUReq->LogTime,sizeof(pUReq->LogTime));
  //iReportLogReq.iLog_Type				=	pUReq->LogType;  									//需要后台软件确认是否可以设置为  5#
  
  iReportLogReq.iLog_Type				=	HistoryLog;  									
  iReportLogReq.power						=	pUReq->LockerPower;
  iReportLogReq.Channel_number	=	channel;
  
  memset(rdata,0x00,sizeof(rdata));
  switch(pUReq->LogType)
  {
            case	LockerRegister:
                iReportLogReq.Data.data						=	rdata;
                iReportLogReq.Data.len						=	1;		//less than 0x01,if not decode failure.2019-5-29
                break;
            case	FingerVena:
            case	SwingCard:
            case	Password:
                iReportLogReq.Data.data						=	rdata;
                iReportLogReq.Data.len						=	4;
                memcpy(iReportLogReq.Data.data,pUReq->InfoEntity,4);
                break;
            case	IdentityCard:
            default:
                iReportLogReq.Data.data						=	rdata;
                iReportLogReq.Data.len						=	sizeof(pUReq->InfoEntity);
                memcpy(iReportLogReq.Data.data,pUReq->InfoEntity,sizeof(pUReq->InfoEntity));
                break;
  }
  iReportLogReq.random.data					=	pUReq->RandomNumber;
  iReportLogReq.random.len						=	sizeof(pUReq->RandomNumber);
  memcpy(iReportLogReq.random.data,pUReq->RandomNumber,sizeof(pUReq->RandomNumber));

  register_log_request_pack((void*)&iReportLogReq,0,socketSendBuf,&socketSendLength);
  if(mVirtualSocket.clt_sock == 0)
		return NULL;
  res = send(mVirtualSocket.clt_sock,socketSendBuf,socketSendLength, 0 );
	if((res <= 0)||(res != socketSendLength)){
			printf("---%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
      close(mVirtualSocket.clt_sock);
	}
	return pUReq;
}

