#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>

#include 			"WorkCommandAnalysis.h"

#include 			"uart/UartPacket.h"
#include 			"uart/UartUnpacket.h"
#include			"uart/xxx_channel_send_thread.h"

#include			"uart/Cmd/remote_authorize_pack.h"
#include			"uart/Cmd/remote_config_pack.h"
#include			"uart/Cmd/remote_load_pack.h"
#include			"uart/Cmd/remote_open_pack.h"
#include			"uart/Cmd/report_log_pack.h"
#include			"socket/protoBuf/epb_report_log_pack.h"
#include			"util/pthread_data_copy.h"
#include            "bdb/FileManager.h"

 /***********************************************************************
 * @brief				work_command_analysis
 * @param[in]		void *argv,	UartPort *	pUartPort ,rang 1~4 number uart
 *							void	*args,VirtualSocket	*pVirtualSocket
 * @return 			=0 ,falure
 **********************************************************************/
uint16_t work_command_analysis(void *argu,void	*args)
{
  uint32_t              ilen;
  uint32_t							res,ret	=	0;
	UReportLogReq        	*pUReportLogReq,iUReportLogReq;
	struct VirtualSocket *pVirtualSocket,iVirtualSocket;
	UartPort							*pUartPort	=	(UartPort*)argu;
  ilen    =   sizeof(struct VirtualSocket);
  memset((char*)&iVirtualSocket,0,ilen);
  pthread_data_copy((char *)&iVirtualSocket,(char*)args,(int*)&ilen);
  pVirtualSocket			=	&iVirtualSocket;
	//uint8_t								u_buffer[255] = {0};  
//  ilen  =   pUartPort->UartSingleReceiveLength;
//  pthread_data_copy((char *)u_buffer,(char*)pUartPort->UartSingleReceiveBuf,(int*)&ilen);
//	pUReportLogReq			=	(UReportLogReq*)u_buffer;

  uint8_t		cmd		=	pUartPort->UartSingleReceiveBuf[0];

    switch(cmd)
    {
        case		ReportLog:
        	{
					  ilen  =   pUartPort->UartSingleReceiveLength;
					  pthread_data_copy((char *)&iUReportLogReq,(char*)pUartPort->UartSingleReceiveBuf,(int*)&ilen);
						pUReportLogReq			=	&iUReportLogReq;
            //-------------gateway 	pack	report log ack	--------------
            if( (pVirtualSocket->clt_sock == 0) /*|| (pUReportLogReq->LogType == LogRegister)*/ ){//--gateway 	pack	report log err ack	--
            	    res	=	report_offline_log_pack(pVirtualSocket,pUartPort,ReportLogOffLineACK);
                  //-------------	off line	record	run		log		-----------------
                  if(res){
    							Run_Log_Context_Record(pVirtualSocket,pUartPort);
    						}
            }else if (pUReportLogReq->LockerStates ==	OffLineMode){
                  res	=	report_offline_log_pack(pVirtualSocket,pUartPort,ReportLogACK);
            }else{
            	printf("\n\r %s,line=%d, [ %s ] receive Locker information = %02x , report server continue ....\n",__FILE__,__LINE__, pUartPort->h_io, pUReportLogReq->LockerStates);
            }
			      //----------	 report log request to server ----------------------
//			      ReportLogReq	*pReportLogReq		=		&mReportLogReq;
//			      pReportLogReq->Channel_number		=		pUartPort->channelNumber;
//			      pReportLogReq->Door_Status			=		pUReportLogReq->LockerStates;
//						pReportLogReq->iLog_Type				=		pUReportLogReq->LogType;
            report_log_request_pack((void*)&iUReportLogReq,pUartPort->channelNumber,pVirtualSocket->SendBuffer,&pVirtualSocket->SendLength);
			      res = send(pVirtualSocket->clt_sock,pVirtualSocket->SendBuffer,pVirtualSocket->SendLength, 0 );
			      if((res <= 0)||(res != pVirtualSocket->SendLength)){
			           printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
			           pVirtualSocket->socketStates			=		socket_send_data_err;
			           break;
			      }
            ret	=	Report_Log_analysis_cmd_ret;
          }
            break;
				case	RegisterEnsureAck:
						pUartPort->deviceStates	=	UartIDLE;
						pUartPort->pipe	=	RegisterEnsureAck;
					break;
        //-------------	server request to locker,data pipe response to sender -------

        case		SureRequestOpenDoor:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	SureRequestOpenDoor;
            ret	=	locker_to_gateway_request;
            break;

        case		RemoteOpenDoorACK:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	RemoteOpenDoorACK;
            ret	=	Locker_to_server_response;
            break;

        case		RemoteLoadFingerACK:											//change work channel cmd response
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	RemoteLoadFingerACK;
            ret	=	Locker_to_server_response;
            break;

        case		RemoteConfigACK:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	RemoteConfigACK;
            ret	=	Locker_to_server_response;
            break;

        case		RemoteAuthorizeACK:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	RemoteAuthorizeACK;
            ret	=	Locker_to_server_response;
            break;
//        case        RemoteChangeChannelResp:
//            pUartPort->pipe	=	RemoteChangeChannelResp;
//            break;
        default:
        		pUartPort->deviceStates	=	UartIDLE;
            printf("%s,%d , %s receive exception cmd=%d error,throw away continue...\n",__FILE__,__LINE__, pUartPort->h_io, cmd);
            ret	=	0;
            break;
    }
        return		ret;
}

