#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>

#include 			"remote_load_pack.h"

#include      "socket/VirtualPortMonitor.h"
#include      "socket/protoBuf/epb_remote_load_pack.h"  
#include      "socket/pub/util.h"
#include			"uart/xxx_channel_send_thread.h"
#include			"uart/socket_task_distributer.h"
#include 			"uart/UartPacket.h"
#include 			"uart/LinkLayer/UartOpration.h"
#include    	"uart/LinkLayer/lora_parament.h"
#include 				"util/pthread_data_copy.h"


ChangeChannelReq					mChangeChannelReq;
URemoteLoadReq						mURemoteLoadReq;
URemoteLoadResp						mURemoteLoadResp;

#define			debug_remote_load			0
#define			REMOTE_LOAD_BUF_SIZE			255
/***********************************************************************
 * @brief			uart_request_Change_Channel_pack,non thread safe
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t uart_Change_Channel_pack(void *args, void	*argu)
{
	
    uint8_t  			idata[sizeof(ChangeChannelReq)+1],header[32];
		uint8_t					u_buffer[255],u_length;
    uint8_t					s_buffer[128];
		uint16_t				s_length;
		uint8_t	sendCnt	=	0,u_delay = 0 ;
    uint32_t 			res;
		uint8_t 			*ptr;
		BaseRequest 	iBaseRequest = {NULL};	
    UartPort						*pUartPort						= (UartPort*)argu;
    UartPort            *pConfigChannel       = &mUartPort[ConfigUartNum];
		memset(mUartPort[ConfigUartNum].UartSingleReceiveBuf,0,sizeof(mUartPort[ConfigUartNum].UartSingleReceiveBuf));
		
    struct VirtualSocket       	*pVirtualSocket				=	(struct VirtualSocket*)args;

    ChangeChannelReq				*pChangeChannelReq	=	(ChangeChannelReq*)idata;
    RemoteLoadReq						*pRemoteLoadReq 		= &mRemoteLoadReq;												//global parm

    pChangeChannelReq->cmd									=	RemoteChangeChannelReq;
    uint16_t   val;
    get_lora_channel_Hal_address(ConfigLoraChn,&val);
    pChangeChannelReq->LockerNumber							=	(uint32_t)htons(val);
    pChangeChannelReq->LockerNumber							=	t_htonl(pChangeChannelReq->LockerNumber);
    get_lora_channel_Frequency(ConfigLoraChn,&res);
    pChangeChannelReq->DstFrequency[0]						=	res>>16;
    pChangeChannelReq->DstFrequency[1]						=	res>>8;
    pChangeChannelReq->DstFrequency[2]						=	res;

    #if		(debug_remote_load == 0)
    UartFrame		*pUartFrame 			= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
    pUartFrame->Dst_Address				=	pRemoteLoadReq->Locker_Address;
    pUartFrame->frameLength				= sizeof(ChangeChannelReq);
    pUartFrame->frameTotal				=	0x01;
    pUartFrame->frameNumber				=	0x01;

    //-------------		send command wait for ack	-----------------
		while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_LORA_REQUEST)*/ ){
				//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
				usleep(10000);
		}
    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
		
    do{
    	  select_lora_states(&pUartPort->channelNumber,0,1);
        pConfigChannel->pipe    =	PIPE_SEND_REQUEST;
        pUartPort->deviceStates	=	UartSendBusying;
        res = write (pUartPort->clt_uart,u_buffer,u_length);
        if((res <= 0)||(res != u_length)){
            sprintf((char*)stderr,"%s,line=%d, write %s error,exit!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("ERROR:%s",(char*)stderr);
						return 0;	//sleep(3);
				}

				pUartPort->deviceStates	=	UartIDLE;
				u_delay	=	0	;
				while((pConfigChannel->pipe != RemoteChangeChannelResp)&&(u_delay <= 80 )){
					//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
          usleep(100000);
          u_delay	=	u_delay	+	1 ;
				}

        sendCnt	+=	1;
        printf("%s,line:%d,[%s]change channel,sendCnt=%d\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
        }while((pConfigChannel->pipe != RemoteChangeChannelResp)&&(sendCnt	<	3));
//				select_lora_states(&pUartPort->channelNumber,0,0);
		//---- unpacket uart	remote load ack Response --------------

    ptr	=	pConfigChannel->UartSingleReceiveBuf ;
		ChangeChannelResp	*pChangeChannelResp		=	(ChangeChannelResp*)ptr;
    if ((sendCnt > 3)||(pChangeChannelResp->cmd	!=	RemoteChangeChannelResp)||(htons(pChangeChannelResp->RespACK) != RemoteChangeChannelACK_SUCC))  //(pChangeChannelResp->LockerNumber != pChangeChannelReq->LockerNumber)||
		{//------ packet error response	to server----------------
      printf("%s,line=%d,  work channel Change receive exception cmd=0x%03x ,or non receive data\n",__FILE__,__LINE__, pChangeChannelResp->cmd);
      mUartPort[ConfigUartNum].WorkStates	=	WorkStatesIDLE;
		  mRemoteLoadResultReq.base_request			=	&iBaseRequest;
		  mRemoteLoadResultReq.Locker_Address		=	pChangeChannelReq->LockerNumber;
		  mRemoteLoadResultReq.Locker_ACK				=	remote_load_un_response;
      remote_load_result_request_pack((void*)&mRemoteLoadResultReq,s_buffer,&s_length);
		  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
      if((res <= 0)||(res != s_length)){
          printf("%s,line=%d, s_length=%d, send error!!\n",__FILE__,__LINE__,res);
				}
			return 0;
		}
     #endif

		//--------------	change channel success send semphore to ConfigThread ----------
		#if	(debug_remote_load == 1)
        uint8_t	chn	=	ConfigLoraChn;
		#else
        uint8_t	chn	=	ConfigLoraChn;
		#endif

		sleep(1);
		usleep(5000);

//		printf("%s,line:%d,pVirtualSocket->channel=%02x,pVirtualSocket->sourceChannel=%02x, pVirtualSocket->workType=%x \n",
//						__FILE__,__LINE__,pVirtualSocket->channel,pVirtualSocket->sourceChannel,pVirtualSocket->workType );
			//wait for mutex_lock ,rewrite mVirtualSocket  2019-03-15
//    mVirtualSocket.sourceChannel    	= pUartPort->channelNumber;       //source channel
//		mVirtualSocket.channel						=	chn;
//		mVirtualSocket.workType						=	remote_load_cmd;			//因为命令碼相同,所以没有发现数据回写不正确的问题！！！！通道切换后需要通过mVirtualSocket.workType命令码\调度程序

    res = sizeof(struct VirtualSocket);
    pthread_data_copy((char*)&VirtualSocketCopyer[ConfigUartNum], (char*)pVirtualSocket,(int*)&res);		//global parm	,2019-03-27	
    VirtualSocketCopyer[ConfigUartNum].sourceChannel    	= pUartPort->channelNumber;       
		VirtualSocketCopyer[ConfigUartNum].channel						=	chn;
		VirtualSocketCopyer[ConfigUartNum].workType						=	remote_load_cmd;
		sem_post(&sem_config_channel_WaitSend);    						//second send sigle, pVirtualSocket data lose,use global loadVirtualSocket
		printf("%s,line:%d,VirtualSocketCopyer[ConfigUartNum].channel=%02x,VirtualSocketCopyer[ConfigUartNum].sourceChannel=%02x, VirtualSocketCopyer[ConfigUartNum].workType=%x \n",\
													__FILE__,__LINE__,VirtualSocketCopyer[ConfigUartNum].channel,VirtualSocketCopyer[ConfigUartNum].sourceChannel,VirtualSocketCopyer[ConfigUartNum].workType );

		return	1;
}
