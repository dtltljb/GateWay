#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/socket.h>

#include 			"ConfigCommandAnalysis.h"

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

 /***********************************************************************
 * @brief				config_command_analysis
 * @param[in]		void *argv,	UartPort *	pUartPort ,rang 1~4 number uart
 *							void	*args,VirtualSocket	*pVirtualSocket
 * @return 			=0 ,falure
 **********************************************************************/
uint16_t config_command_analysis(void *argu,void	*args)
{
  uint32_t              ilen = 0;
  uint32_t							ret	=	0;
  UReportLogReq        	*pUReportLogReq,iUReportLogReq;
  struct VirtualSocket *pVirtualSocket = (struct VirtualSocket *)args;
  UartPort							*pUartPort	=	(UartPort*)argu;
//	struct VirtualSocket *pVirtualSocket,iVirtualSocket;
//	ilen    =   sizeof(struct VirtualSocket);
//  memset((char*)&iVirtualSocket,0,ilen);
//  pthread_data_copy((char *)&iVirtualSocket,(char*)args,(int*)&ilen);
//  pVirtualSocket			=	&iVirtualSocket;

//    uint8_t								u_buffer[255] = {0};
//  	UReportLogReq        	*pUReportLogReq;
//	pUReportLogReq			=	(UReportLogReq*)u_buffer;

  uint8_t		cmd		=	pUartPort->UartSingleReceiveBuf[0];
    switch(cmd)
    {
        case		ReportLog:
        		{
						  ilen  =   pUartPort->UartSingleReceiveLength;
						  pthread_data_copy((char *)&iUReportLogReq,(char*)pUartPort->UartSingleReceiveBuf,(int*)&ilen);
							pUReportLogReq			=	&iUReportLogReq;
	            //-------------gateway 	pack	report register locker	--------------
					#if	Debug_Level == 1            
							printf("\n\r %s,line=%d, [ %s ] receive Locker information = %02x , report server continue ....\n",__FILE__,__LINE__, pUartPort->h_io, pUReportLogReq->LockerStates);
					#endif
							iUReportLogReq.LockerStates	=	LOCK_REGISTER_SUCC;	//assigned value
	            report_log_request_pack((void*)&iUReportLogReq,pUartPort->channelNumber,pVirtualSocket->SendBuffer,&pVirtualSocket->SendLength);
				      ret = send(pVirtualSocket->clt_sock,pVirtualSocket->SendBuffer,pVirtualSocket->SendLength, 0 );
				      if((ret <= 0)||(ret != pVirtualSocket->SendLength)){
				           printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,ret);
				           pVirtualSocket->socketStates			=		socket_send_data_err;
				           break;
				      }
	            ret	=	Report_Log_analysis_cmd_ret;
          	}
            break;

        case		RemoteLoadFingerACK:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	RemoteLoadFingerACK;
            break;
        case    RemoteChangeChannelResp:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	RemoteChangeChannelResp;
            break;
        default:
        		pUartPort->deviceStates	=	UartIDLE;
            pUartPort->pipe	=	cmd;
            printf("%s,%d ,don't config channel cmd=%d error, please set channel address...\n",__FILE__,__LINE__,cmd);
            break;
    }
        return		ret;

}

