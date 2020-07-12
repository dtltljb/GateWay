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
#include 			"util/pthread_data_copy.h"

ChangeChannelReq					mChangeChannelReq;
URemoteLoadReq						mURemoteLoadReq;
URemoteLoadResp						mURemoteLoadResp;
//struct VirtualSocket      loadVirtualSocket; /* uart_Change_Channel_pack() send signel pass context */


#define			debug_remote_load			0
#define			REMOTE_LOAD_BUF_SIZE			255

/***********************************************************************
 * @brief				uart_remote_load_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 
 **********************************************************************/
uint8_t uart_remote_load_pack(void *args, void	*argu)
{
    uint16_t			successCount=0,faileCount=0;
	  uint8_t				FrameCnt,sendCnt,tailLength,u_delay =0;
    uint8_t  			header[32];
    uint8_t				l_arrBcdtime[8];
    uint8_t				idata[sizeof(URemoteLoadReq)];
    uint8_t				u_length;
    uint8_t				u_buffer[255];
		uint8_t				s_buffer[128];
    uint16_t			s_length;
    uint16_t			fingerDataLen;
    uint16_t   		val;
		uint8_t				*ptr;
    uint32_t 			res;    
//    idata			=	(uint8_t*) malloc(sizeof(URemoteLoadReq));
//    u_buffer	=	(uint8_t*) malloc( 255 );    
    URemoteLoadResp					*pURemoteLoadResp;
    UartFrame								*pUartFrame;
    URemoteLoadReq					*pURemoteLoadReq;
    RemoteLoadReq						*pRemoteLoadReq	;
    BaseRequest 						iBaseRequest ={NULL};			
    UartPort								*pUartPort,*pWorkChannel;
    pUartPort								= (UartPort*)argu;

    struct VirtualSocket       	*pVirtualSocket; 
    if(args == NULL)
    	 printf("%s,line:%d, cancel args warning..\n",__FILE__,__LINE__);
//    res = sizeof(struct VirtualSocket);
//    pVirtualSocket				=	(struct VirtualSocket*)args;							/*	inher */
//    pthread_data_copy((char*)&iVirtualSocket, (char*)pVirtualSocket,(int*)&res); 
//    pVirtualSocket				=	&iVirtualSocket;
//    pVirtualSocket				=	&loadVirtualSocket;				//global prm, uart_Change_Channel_pack() send signel pass context 
		pVirtualSocket				=	&VirtualSocketCopyer[ConfigUartNum];

    uint8_t		controlList					=	0x55;			//default value
    pRemoteLoadReq		=		&mRemoteLoadReq;
    pURemoteLoadReq		=		(URemoteLoadReq*)idata;	

    uint16_t	iLen		=		pRemoteLoadReq->finger_context.len + sizeof(pURemoteLoadReq->cmd) + sizeof(pURemoteLoadReq->LockerNumber) 
    											+ sizeof(pURemoteLoadReq->ConType) + sizeof(controlList) + sizeof(pRemoteLoadReq->finger_Number) ;
		if ( ( iLen  % FINGER_DATA_SIZE ) != 0 )
		{
				FrameCnt		=	iLen  / FINGER_DATA_SIZE + 1;
				tailLength  =  iLen  % FINGER_DATA_SIZE;
     }else{
				FrameCnt	=	iLen  / FINGER_DATA_SIZE;
				tailLength  =  FINGER_DATA_SIZE;
			}
        printf("%s,line:%d,iVirtualSocket.sourceChannel=%02x,pVirtualSocket->sourceChannel=%02x \n",__FILE__,__LINE__,pVirtualSocket->sourceChannel,pVirtualSocket->sourceChannel);
    //-------------		send command wait for ack	-----------------   			 
		while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST)*/ ){
        printf("%s,line:%d,uart states=%d \n",__FILE__,__LINE__,pUartPort->deviceStates);
				sleep(1);
    }
		pUartPort->deviceStates	=	UartSendBusying;
    select_lora_states(&pUartPort->channelNumber,0,0);
		//-------------- send		header	frame	------------------
    pURemoteLoadReq->cmd												=		RemoteLoadFinger;
    pURemoteLoadReq->LockerNumber								=		t_ntohl(	pRemoteLoadReq->Locker_Address );
    pURemoteLoadReq->ConType										=		0x02;  //load finger

	  pUartFrame 										= (UartFrame*)header;
	  pUartFrame->dataPtr						=	idata;
    pUartFrame->Dst_Address				=	(uint16_t)mRemoteLoadReq.Locker_Address;
	  pUartFrame->frameLength				= sizeof(URemoteLoadReq);
    pUartFrame->frameTotal				=	FrameCnt;
    sendCnt	=	1;
	  pUartFrame->frameNumber				=	sendCnt;

	  pURemoteLoadReq->SingleInfo[0]	=	pRemoteLoadReq->User_Control_list;
    res   =   sizeof(pRemoteLoadReq->finger_Number) ;
    memcpy( (char*)&pURemoteLoadReq->SingleInfo+sizeof(controlList), (char*)&pRemoteLoadReq->finger_Number, sizeof(pRemoteLoadReq->finger_Number) );
    memcpy( ( (char *)&pURemoteLoadReq->SingleInfo) + sizeof(pRemoteLoadReq->finger_Number) + sizeof(controlList), 
    				pRemoteLoadReq->finger_context.data, sizeof(pURemoteLoadReq->SingleInfo)-sizeof(pRemoteLoadReq->finger_Number)-sizeof(controlList)	);
	  res = uart_finger_data_pack_func(pUartFrame, u_buffer, &u_length);
	  res = write (pUartPort->clt_uart,u_buffer,u_length);
	  if((res <= 0)||(res != u_length)){
				while(1){
            sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("ERROR:%s",(char*)stderr);
					sleep(3);
				}
			}
     sleep(1);
     usleep(5000);
			//----------------	other	frame	------------------	
//    ptr	=	pRemoteLoadReq->finger_context.data + sizeof(pURemoteLoadReq->SingleInfo) - sizeof(pRemoteLoadReq->finger_Number) - sizeof(controlList);
    select_lora_states(&pUartPort->channelNumber,0,0);				//change control status
    printf("%s,line:%d,iVirtualSocket.sourceChannel=%02x,pVirtualSocket->sourceChannel=%02x \n",__FILE__,__LINE__,pVirtualSocket->sourceChannel,pVirtualSocket->sourceChannel);
		
		fingerDataLen		=	 sizeof(pURemoteLoadReq->SingleInfo) - sizeof(pRemoteLoadReq->finger_Number) - sizeof(controlList);
    for( sendCnt=2; sendCnt <= FrameCnt; sendCnt++)
    {
    	pUartFrame 											= (UartFrame*)header;     
    	pUartFrame->dataPtr							=	idata;                                   
      pUartFrame->Dst_Address					=	(uint16_t)pRemoteLoadReq->Locker_Address;
      
      if ( sendCnt == FrameCnt )
				pUartFrame->frameLength				= tailLength;
			else
				pUartFrame->frameLength				= sizeof(URemoteLoadReq);
	    pUartFrame->frameTotal					=	FrameCnt;
	    pUartFrame->frameNumber					=	sendCnt;

      memcpy(idata, (char*)(pRemoteLoadReq->finger_context.data + fingerDataLen), pUartFrame->frameLength);
	    res = uart_finger_data_pack_func(pUartFrame,u_buffer,&u_length);
	    res = write (pUartPort->clt_uart,u_buffer,u_length);
	    if((res <= 0)||(res != u_length))
			{
						while(1){
                sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                printf("ERROR:%s",(char*)stderr);
							sleep(3);
						}
			}
			fingerDataLen	= fingerDataLen + pUartFrame->frameLength;

			
      if ( sendCnt != 14 )
      	sleep(1);
      else
      	{
      		sleep(1);
      		usleep(500000);
      	}
      //printf("%s,line=%d,address=0x%02x, send length=%d\n",__FILE__,__LINE__,(int)pRemoteLoadReq->finger_context.data, fingerDataLen);
		}
		
    printf("%s,line:%d,iVirtualSocket.sourceChannel=%02x,pVirtualSocket->sourceChannel=%02x \n",__FILE__,__LINE__,pVirtualSocket->sourceChannel,pVirtualSocket->sourceChannel);
		
		pUartPort->pipe		=	PIPE_SEND_REQUEST;
		u_delay	=	0	;
		while((pUartPort->pipe != RemoteLoadFingerACK)&&(u_delay <= 150 )){
					//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
          usleep(100000);
          u_delay	=	u_delay	+	1 ;
		}
		Get_CurBCDTime7((char*)l_arrBcdtime);		//communication failure use system time 
    printf("%s,line:%d,iVirtualSocket.sourceChannel=%02x,pVirtualSocket->sourceChannel=%02x \n",__FILE__,__LINE__,pVirtualSocket->sourceChannel,pVirtualSocket->sourceChannel);    
    ptr	=	pUartPort->UartSingleReceiveBuf ;
    pURemoteLoadResp		=	(URemoteLoadResp*)ptr;

    if ((pURemoteLoadResp->cmd	!=	RemoteLoadFingerACK)||(htons(pURemoteLoadResp->RespACK) != RemoteLoadFingerACK_SUCC)/* (pUartPort->pipe != RemoteLoadFingerACK)||*/){
			mRemoteLoadResultReq.Locker_ACK				=	remote_load_un_response;		//-------	packet faile response to server ----------
		  mRemoteLoadResultReq.base_request			=	&iBaseRequest;
      mRemoteLoadResultReq.Locker_Address		=	pRemoteLoadReq->Locker_Address;
		}else{
			mRemoteLoadResultReq.Locker_ACK				=	remote_load_ok_response;		//-------	packet faile response to server ----------
		  mRemoteLoadResultReq.base_request			=	&iBaseRequest;
      mRemoteLoadResultReq.Locker_Address		=	pRemoteLoadReq->Locker_Address;
		}
	mRemoteLoadResultReq.Last_Work_Type		=	remote_load_cmd;	
	remote_load_result_request_pack((void*)&mRemoteLoadResultReq,s_buffer,&s_length);
	res = send(pVirtualSocket->clt_sock, s_buffer, s_length, 0 );
	if((res <= 0)||(res != s_length)){
		printf("---%s,line=%d, s_length=%d, send error!!\n",__FILE__,__LINE__,res);
	}
	sleep(1);
	usleep(5000);
  //  chn = pVirtualSocket->sourceChannel;
//done_change_channel:
   //-	-	-	-	-	-	-	-	-	
    #if(debug_remote_load == 0)
    //    RemoteOpenDoorReq				*pRemoteOpenDoorReq		=	(RemoteOpenDoorReq*)idata;
    pURemoteLoadReq->cmd											=		RemoteLoadFinger;
    get_lora_channel_Hal_address(pVirtualSocket->sourceChannel, &val);
    pURemoteLoadReq->LockerNumber             =		(uint32_t)htons(val);				//t_ntohl(val);
    pURemoteLoadReq->LockerNumber							=	t_htonl(pURemoteLoadReq->LockerNumber);
    pURemoteLoadReq->ConType									=		0x01;  									//change channel
    get_lora_channel_Frequency(pVirtualSocket->sourceChannel, &res);
    pURemoteLoadReq->SingleInfo[0]	=	0x00;					//protocol holiday list
    pURemoteLoadReq->SingleInfo[1]	=	res >> 16;
    pURemoteLoadReq->SingleInfo[2]	=	res >> 8;
    pURemoteLoadReq->SingleInfo[3]	=	res ;
		printf("%s,line:%d,pVirtualSocket->sourceChannel =0x%02x,channel_Frequency=%02x \n",__FILE__,__LINE__, \
								pVirtualSocket->sourceChannel,res);
	  pUartFrame 										= (UartFrame*)header;
	  pUartFrame->dataPtr						=	idata;
    pUartFrame->Dst_Address				=	(uint16_t)pRemoteLoadReq->Locker_Address;
	  pUartFrame->frameLength				= 1 + 4 + 1 + 4;		
	  pUartFrame->frameTotal				=	01;
	  pUartFrame->frameNumber				=	01;
	  //res = uart_finger_data_pack_func(pUartFrame, u_buffer, &u_length);
	  res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);							//encypt transfer
    select_lora_states(&pUartPort->channelNumber,0,0);	  									//change lora control
	  printf("%s,line:%d,pVirtualSocket->sourceChannel =0x%02x \n",__FILE__,__LINE__, pVirtualSocket->sourceChannel);
		pWorkChannel = &mUartPort[pVirtualSocket->sourceChannel-1];											//get work channel parament	  
    sendCnt	=	0;
    do{
    		pWorkChannel->pipe	=	PIPE_SEND_REQUEST;					//request work channel receive data
				pUartPort->deviceStates	=	UartSendBusying;
        res = write (pUartPort->clt_uart,u_buffer,u_length);
        if((res <= 0)||(res != u_length)){
					while(1){
            sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("ERROR:%s",(char*)stderr);
						sleep(3);
					}
				}

			pUartPort->deviceStates	=	UartIDLE;
			u_delay	=	0	;
			while((pWorkChannel->pipe != RemoteLoadFingerACK)&&(u_delay <= 150 )){
						//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
	          usleep(100000);
	          u_delay	=	u_delay	+	1 ;
					}
			sendCnt	+=	1;
			printf("%s,line:%d,RemoteLoadFinger cmd sendCnt=%d times,pWorkChannel->pipe=0x%02x \n",__FILE__,__LINE__, sendCnt, pWorkChannel->pipe);
		}while((pWorkChannel->pipe != RemoteLoadFingerACK)&&(sendCnt	<	3));
		
	 // select_lora_states(&pUartPort->channelNumber,0,0);			disable status control
	 //---- unpacket uart	remote load ack Response --------------

    ptr	=	pWorkChannel->UartSingleReceiveBuf;
    pURemoteLoadResp		=	(URemoteLoadResp*)ptr;
		if ((sendCnt > 3)||(pURemoteLoadResp->cmd	!=	RemoteLoadFingerACK)||(htons(pURemoteLoadResp->RespACK)	!= RemoteLoadFingerACK_SUCC)){
		  mRemoteLoadResultReq.Locker_ACK				=	remote_channel_un_response;		//-------	packet failure response to server ----------
		  res	=	1;
    	faileCount+=1;
      printf("failure Count=%d,total=%d\n",faileCount,successCount+faileCount);
		}else{
    	mRemoteLoadResultReq.Locker_ACK				=	remote_channel_ok_response;		//------- packet success response	to server----------------
    	res	=	0;
    	successCount+=1;
    	memcpy(l_arrBcdtime,pURemoteLoadResp->ProtoTime,sizeof(pURemoteLoadResp->ProtoTime));
      printf("success Count=%d,total=%d\n",successCount,successCount+faileCount);
    }
   mRemoteLoadResultReq.Last_Work_Type		=	remote_load_channel_cmd;
	 mRemoteLoadResultReq.base_request			=	&iBaseRequest;
   mRemoteLoadResultReq.Locker_Address		=	pRemoteLoadReq->Locker_Address;
      
    mRemoteLoadResultReq.Report_Time.str	=	(char*)l_arrBcdtime;
    mRemoteLoadResultReq.Report_Time.len	=	sizeof(pURemoteLoadResp->ProtoTime);
      
      remote_load_result_request_pack((void*)&mRemoteLoadResultReq, s_buffer, &s_length);
		  res = send(pVirtualSocket->clt_sock, s_buffer, s_length, 0 );
			if((res <= 0)||(res != s_length)){
					printf("---%s,line=%d, s_length=%d, send error!!\n",__FILE__,__LINE__,res);
				}
     #endif

      pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  config  channel,lock in file socket_protocol_thread.c
      return (uint8_t)res;

}

