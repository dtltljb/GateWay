#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>

#include 				"remote_open_pack.h"
#include 				"util/pthread_data_copy.h"
#include 				"uart/UartPacket.h"
#include    		"uart/LinkLayer/UartOpration.h"
#include        "socket/VirtualPortMonitor.h"
#include        "socket/protoBuf/epb_remote_open_pack.h"
#include        "socket/pub/util.h"
#include 				"util/pthread_data_copy.h"

#define				Disable_Ack  		1
#define				WAIT_RESPONSE_DELAY		80

RemoteOpenDoorReq							mRemoteOpenDoorReq;
SureRequestOpenDoorResp				mSureRequestOpenDoorResp;
EnsureRequestOpenDoorReq			mEnsureRequestOpenDoorReq;
RemoteOpenDoorACKResp					mRemoteOpenDoorACKResp;

/***********************************************************************
 * @brief				uart_request_remote_open_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t uart_request_remote_open_pack(void *args, void	*argu)
{
		uint8_t  			header[32],*ptr;
		uint8_t				idata[sizeof(RemoteOpenDoorReq)];
		uint8_t					u_buffer[128],u_length;
    uint8_t					s_buffer[128];
		uint16_t				s_length;
		uint8_t 			l_arrBcdtime[8] ={0};
		uint32_t 			res;
		uint8_t				u_delay	=	0,sendCnt = 0;

			EnsureRequestOpenDoorReq				*pEnsureRequestOpenDoorReq;
			SureRequestOpenDoorResp					*pSureRequestOpenDoorResp;
			
			RemoteOpenResultReq		*pRemoteOpenResultReq,iRemoteOpenResultReq;
			pRemoteOpenResultReq	=	&iRemoteOpenResultReq;
//		s_buffer	=	(uint8_t*)malloc ( 255 );
//		idata			= (uint8_t*)malloc (sizeof(RemoteOpenDoorReq));
      

    UartPort					*pUartPort					= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
    RemoteOpenReq				*pRemoteOpenReq, iRemoteOpenReq;
    res    = sizeof(RemoteOpenReq);
    pthread_data_copy((char*)&iRemoteOpenReq,(char*)pVirtualSocket->pContext.p,(int*)&res);
    pRemoteOpenReq				=	&iRemoteOpenReq;

    RemoteOpenDoorReq		*pRemoteOpenDoorReq		=	(RemoteOpenDoorReq*)idata;
    pRemoteOpenDoorReq->cmd										=		RemoteOpenDoor;
    pRemoteOpenDoorReq->LockerNumber					=		t_ntohl(pRemoteOpenReq->Locker_Address);
    pRemoteOpenDoorReq->ControlStatus					=		htons(pRemoteOpenReq->Control_state);
    Get_CurBCDTime7((char*)l_arrBcdtime);
    memcpy(&pRemoteOpenDoorReq->ProtoTime,l_arrBcdtime,sizeof(pRemoteOpenDoorReq->ProtoTime));

    UartFrame		*pUartFrame 			= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
    pUartFrame->Dst_Address				=	pRemoteOpenReq->Locker_Address;
		pUartFrame->frameLength				= sizeof(RemoteOpenDoorReq);
    pUartFrame->frameTotal				=	0x01;
    pUartFrame->frameNumber				=	0x01;
    
		//-------------		send command wait for sync single	-----------------
    pUartPort->pipe		=	PIPE_SEND_REQUEST;	
		while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST) */){
				//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
				usleep(10000);
		}

		res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
		#ifdef		Debug_Level
				printf("\n%s,line:%d,port:%s send data\n",__FILE__,__LINE__,pUartPort->h_io);
		#endif
        sendCnt = 0;
    do{
    		select_lora_states(&pUartPort->channelNumber,0,1);
				pUartPort->deviceStates	=	UartSendBusying;
				pUartPort->pipe		=	PIPE_SEND_REQUEST;				
        res = write (pUartPort->clt_uart,u_buffer,u_length);

        if((res <= 0)||(res != u_length)){
					while(1){
            //sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("\r\n %s ERROR:%s \n",pUartPort->iodevice,(char*)stderr);
						sleep(3);
					}
				}
				pUartPort->deviceStates	=	UartIDLE;
        u_delay	=	0;
				while((pUartPort->pipe != SureRequestOpenDoor)&&(u_delay <= WAIT_RESPONSE_DELAY )){
					//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
          usleep(100000);
					u_delay = u_delay + 1;
          }
				sendCnt	+=	1;
				#ifdef		Debug_Level
        printf("%s,line:%d,[ %s ] send remote open %d  times \n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
				#endif
		}while((pUartPort->pipe != SureRequestOpenDoor)&&(sendCnt	<	3));
		select_lora_states(&pUartPort->channelNumber,0,0);

		res	=	sizeof(pUartPort->ReceiveBuf);
		pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
		ptr	=	pUartPort->ReceiveBuf;
		pSureRequestOpenDoorResp = (SureRequestOpenDoorResp*)ptr;
    #if	(Disable_Ack == 1)
		if((sendCnt > 3)||(*ptr	!=	SureRequestOpenDoor))
		{//------ packet error response	to server----------------
			pRemoteOpenResultReq->base_request			=	NULL;
			pRemoteOpenResultReq->Locker_Address		=	pRemoteOpenReq->Locker_Address;
			pRemoteOpenResultReq->Last_Work_Type		=	remote_open_door;
			pRemoteOpenResultReq->Locker_Status			=	0;
			pRemoteOpenResultReq->Locker_ACK				=	remote_open_door_un_response;
	    pRemoteOpenResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
//	    memcpy((char*)pRemoteOpenResultReq->Report_Time.str,pRemoteOpenDoorACKResp->ProtoTime,sizeof(pRemoteOpenDoorACKResp->ProtoTime));
	    pRemoteOpenResultReq->Report_Time.len	=	0;		  
      remote_open_result_request_pack(pRemoteOpenResultReq,s_buffer,&s_length);
		  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
			if((res <= 0)||(res != s_length)){
					printf("---%s,line=%d, socketSendLength=%d, send error!!\n",__FILE__,__LINE__,res);
				}
			pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
			return 0;
		}
		#endif
		//----------------	EnsureRequestOpenDoorReq_pack	-----------------
		pEnsureRequestOpenDoorReq		=	(EnsureRequestOpenDoorReq*)idata;
    pSureRequestOpenDoorResp			=	(SureRequestOpenDoorResp*)pUartPort->ReceiveBuf ;
    
    pEnsureRequestOpenDoorReq->cmd									=		EnsureRequestOpenDoor;
    pEnsureRequestOpenDoorReq->LockerNumber					=		t_ntohl(pRemoteOpenReq->Locker_Address);
    memcpy(&pEnsureRequestOpenDoorReq->RandomNumber,pSureRequestOpenDoorResp->RandomNumber,sizeof(pEnsureRequestOpenDoorReq->RandomNumber));
   // Get_CurBCDTime7((char*)l_arrBcdtime);  use last time
    memcpy(&pEnsureRequestOpenDoorReq->ProtoTime,l_arrBcdtime,sizeof(pEnsureRequestOpenDoorReq->ProtoTime));
    
    pUartFrame 										= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
		pUartFrame->Dst_Address				=	pRemoteOpenReq->Locker_Address; 
		pUartFrame->frameLength				= sizeof(EnsureRequestOpenDoorReq);
    pUartFrame->frameTotal				=	0x01;
    pUartFrame->frameNumber				=	0x01;

    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
		#ifdef		Debug_Level
				printf("\n%s,line:%d,port:%s send data\n",__FILE__,__LINE__,pUartPort->h_io);
		#endif 
    sendCnt = 0;
    do{
				pUartPort->deviceStates	=	UartSendBusying;
				pUartPort->pipe		=	PIPE_SEND_REQUEST;				
        res = write (pUartPort->clt_uart,u_buffer,u_length);
        if((res <= 0)||(res != u_length))
				{
					while(1){
            //sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->iodevice);
            printf("%s,line=%d, %s ERROR:%s \n",__FILE__,__LINE__,pUartPort->iodevice,(char*)stderr);
						sleep(3);
					}
				}

				pUartPort->deviceStates	=	UartIDLE;
        u_delay	=	0;
				while((pUartPort->pipe != RemoteOpenDoorACK)&&(u_delay <= WAIT_RESPONSE_DELAY )){
					//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
					usleep(100000);
					u_delay	=	u_delay + 1 ;
				}
				sendCnt	+=	1;
				#ifdef		Debug_Level
				printf("%s,line:%d,send remote open time=%d\n",__FILE__,__LINE__,sendCnt);		
				#endif
		}while((pUartPort->pipe != RemoteOpenDoorACK)&&(sendCnt	<	3));
		
		pUartPort->deviceStates	=	UartIDLE;
		//---- unpacket	Remote Open Door ACK Response --------------
		res	=	sizeof(pUartPort->ReceiveBuf);
		pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
		ptr	=	pUartPort->ReceiveBuf;
		RemoteOpenDoorACKResp	*pRemoteOpenDoorACKResp		=	(RemoteOpenDoorACKResp*)ptr;
		res	=	*ptr;

		if ((sendCnt > 3)||(*ptr	!=	RemoteOpenDoorACK)||(pRemoteOpenDoorACKResp->LockerNumber != pEnsureRequestOpenDoorReq->LockerNumber)||(htons(pRemoteOpenDoorACKResp->RespACK)	!= RemoteOpenDoorAck_SUCC))
		{//------ packet error response	to server----------------
	  
			pRemoteOpenResultReq->base_request			=	NULL;
			pRemoteOpenResultReq->Locker_Address		=	pRemoteOpenReq->Locker_Address;
			pRemoteOpenResultReq->Last_Work_Type		=	remote_open_door;
			pRemoteOpenResultReq->Locker_Status			=	htons(pRemoteOpenDoorACKResp->RespACK);
			pRemoteOpenResultReq->Locker_ACK				=	remote_open_door_un_response;
	    pRemoteOpenResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
	    memcpy((char*)pRemoteOpenResultReq->Report_Time.str,pRemoteOpenDoorACKResp->ProtoTime,sizeof(pRemoteOpenDoorACKResp->ProtoTime));
	    pRemoteOpenResultReq->Report_Time.len	=	0;		  

		}else{
			pRemoteOpenResultReq->base_request			=	NULL;
			pRemoteOpenResultReq->Locker_Address		=	pRemoteOpenReq->Locker_Address;
			pRemoteOpenResultReq->Last_Work_Type		=	remote_open_door;
			pRemoteOpenResultReq->Locker_Status			=	pRemoteOpenDoorACKResp->LockerStatus;
			pRemoteOpenResultReq->Locker_ACK				=	htons(pRemoteOpenDoorACKResp->RespACK);
	    pRemoteOpenResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
	    memcpy((char*)pRemoteOpenResultReq->Report_Time.str,pRemoteOpenDoorACKResp->ProtoTime,sizeof(pRemoteOpenDoorACKResp->ProtoTime));
	    pRemoteOpenResultReq->Report_Time.len	=	sizeof(pRemoteOpenDoorACKResp->ProtoTime);
		}
		//pRemoteOpenResultReq->Door_Status					=	0;
		
		remote_open_result_request_pack(pRemoteOpenResultReq,s_buffer,&s_length);				
		res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
		if((res <= 0)||(res != s_length)){
					printf("---%s,line=%d, socketSendLength=%d, send error!!\n",__FILE__,__LINE__,res);
				}
    printf("%s,line:%d,[ %s ] Remote_Open_Door_response success\n",__FILE__,__LINE__,pUartPort->h_io);
		pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
		return	1;
}
