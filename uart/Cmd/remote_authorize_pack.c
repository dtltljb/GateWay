
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>

#include 				"remote_authorize_pack.h"
#include 				"uart/UartPacket.h"
#include    		"uart/LinkLayer/UartOpration.h"

#include        "socket/VirtualPortMonitor.h"
#include        "socket/protoBuf/epb_remote_authorize_pack.h"
#include        "socket/pub/util.h"
#include 				"util/pthread_data_copy.h"

#define					Disable_auth_ack		0

URemoteAuthorizeReq					mURemoteAuthorizeReq;
URemoteAuthorizeResp				mURemoteAuthorizeResp;

/***********************************************************************
 * @brief				uart_request_remote_authorize_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
 
 #define	WAIT_DELETE_ALL_TIME 	250			//
 
 #define	WAIT_OTHER_CMD_TIME 	80

uint8_t uart_request_remote_authorize_pack(void *args, void	*argu)
{
	  UartPort										*pUartPort					= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
		uint8_t 				*ptr;
		uint16_t				ilen;
		uint8_t					sendCnt	=	0;
		uint16_t				u_delay = 0,wait_time_value =	WAIT_OTHER_CMD_TIME;
		uint32_t 								res;
    BaseRequest 		iBaseRequest = {NULL} ;
		User_authorize				*pUserAuthorize;
	  uint8_t					l_arrBcdtime[8];
		uint8_t  				header[32];
		uint8_t  				idata[sizeof(URemoteAuthorizeReq)];
		uint8_t					u_buffer[255],u_length;
		uint8_t					s_buffer[128];
		uint16_t				s_length;
	//	uint8_t					iData_context_1[255],iData_context_2[255],iData_context_3[255],iData_context_4[255];
    //-------------------		free data space    	-	-	-
		RemoteAuthorizeResultReq		iRemoteAuthorizeResultReq,*pRemoteAuthorizeResultReq;
		pRemoteAuthorizeResultReq	=	&iRemoteAuthorizeResultReq;
		URemoteAuthorizeResp	*pURemoteAuthorizeResp;
		
    res    = sizeof(RemoteAutorizeReq);
    RemoteAutorizeReq						iRemoteAutorizeReq;																	
    pthread_data_copy((char*)&iRemoteAutorizeReq,(char*)pVirtualSocket->pContext.p,(int*)&res);
    RemoteAutorizeReq						*pRemoteAutorizeReq				=	(RemoteAutorizeReq*)&iRemoteAutorizeReq;

    iRemoteAutorizeReq.iUser_authorize.Data_context_1.data		=	iRemoteAutorizeReq.m_data_context_1;
		iRemoteAutorizeReq.iUser_authorize.Data_context_2.data		=	iRemoteAutorizeReq.m_data_context_2;
		iRemoteAutorizeReq.iUser_authorize.Data_context_3.data		=	iRemoteAutorizeReq.m_data_context_3;
		iRemoteAutorizeReq.iUser_authorize.Data_context_4.data		=	iRemoteAutorizeReq.m_data_context_4;
    pUserAuthorize		=	&iRemoteAutorizeReq.iUser_authorize;
    
 		URemoteAuthorizeReq					*pURemoteAuthorizeReq			=	(URemoteAuthorizeReq*)idata;
		pURemoteAuthorizeReq->cmd										=		RemoteAuthorize;
		pURemoteAuthorizeReq->LockerNumber					=		t_ntohl(pRemoteAutorizeReq->Locker_Address);
    pURemoteAuthorizeReq->controlType						=		pRemoteAutorizeReq->iControlType;
   // pURemoteAuthorizeReq->controlType						=		02;  //debug use 2019-02-25
   
  	//---------------------------------------------
   // uint8_t data[]={0xab,0xf1,0x86,0x0f,0,0,0,0,0,0,0,0,0};  //debug
     if((pUserAuthorize->dataType_1	>=	Finger_Number)&&(pUserAuthorize->dataType_1	<=	Identity_Card)&&(pUserAuthorize->Data_context_1.len > 3))
		 {
        if ((pUserAuthorize->dataType_1 == Finger_Number) && (pRemoteAutorizeReq->iControlType == CREATEUSER) )
		 		{
           printf("%s,line:%d,remote authorize do not finger ven Create User, controlType do not =%d\n",__FILE__,__LINE__,pUserAuthorize->user_control_list);
		 		}else{
		 			
			    pURemoteAuthorizeReq->holidayControl				=		pUserAuthorize->user_control_list;
			    pURemoteAuthorizeReq->dataType							=		pUserAuthorize->dataType_1;
					ptr			=	pUserAuthorize->Data_context_1.data;
          if ( pUserAuthorize->Data_context_1.len > (int)sizeof(pURemoteAuthorizeReq->context))
						ilen		=	sizeof(pURemoteAuthorizeReq->context);
					else	
						ilen		=	pUserAuthorize->Data_context_1.len;
          memcpy(pURemoteAuthorizeReq->context,ptr,ilen);
          
		 			if(pRemoteAutorizeReq->iControlType != DELETEALL)
		 				wait_time_value =	WAIT_OTHER_CMD_TIME;
		 			else
		 				wait_time_value =	WAIT_DELETE_ALL_TIME;          
          //-------------	pack -> send -> wait -> unpack -> ack	-----------------
          UartFrame		*pUartFrame 			= (UartFrame*)header;
          pUartFrame->dataPtr						=	idata;
          pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pURemoteAuthorizeReq->LockerNumber);
          pUartFrame->frameLength				= ilen+URemoteAuthorizeReq_STRUCT_HEADER_SIZE;
          pUartFrame->frameTotal				=	0x01;
          pUartFrame->frameNumber				=	0x01;
					//-------------		send command wait for sync single	-----------------
			    pUartPort->pipe		=	PIPE_SEND_REQUEST;	
					while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST)*/ ){
							//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
							usleep(10000);
					}
          res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
          memset(pUartPort->UartSingleReceiveBuf,0,sizeof(pUartPort->UartSingleReceiveBuf));		//clear recive data space
          sendCnt = 0;
          do{
          		select_lora_states(&pUartPort->channelNumber,0,1);
              pUartPort->deviceStates	=	UartSendBusying;
              pUartPort->pipe		=	PIPE_SEND_REQUEST;                      
              res = write (pUartPort->clt_uart,u_buffer,u_length);
           		if((res <= 0)||(res != u_length)){
                 while(1){
                  sprintf((char*)stderr,"%s,line=%d,[ %s ] write  error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                  printf("ERROR:%s",(char*)stderr);
                  sleep(3);
                }
              }

              pUartPort->deviceStates	=	UartIDLE;
							u_delay	=	0	;
							while((pUartPort->pipe != RemoteAuthorizeACK)&&(u_delay <= wait_time_value )){
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
               sendCnt	+=	1;
               #ifdef		Debug_Level
               printf("%s,line:%d,[ %s ] send Authorize %d times \n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
               #endif
           }while((pUartPort->pipe != RemoteAuthorizeACK)&&(sendCnt	<	3));

					//select_lora_states(&pUartPort->channelNumber,0,0);             
              //---- unpacket	Remote Open Door ACK Response --------------
              
						res	=	sizeof(pUartPort->ReceiveBuf);
						pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
						ptr	=	pUartPort->ReceiveBuf;              
           #if	(Disable_auth_ack == 0)
					pURemoteAuthorizeResp		=	(URemoteAuthorizeResp*)ptr;					
					
					//res	=	*ptr;
					if ((sendCnt > 3)||(*ptr	!=	RemoteAuthorizeACK)||(htons(pURemoteAuthorizeResp->RespACK)	!= RemoteAuthorizeACK_SUCC)
																													 ||(pURemoteAuthorizeResp->LockerNumber != pURemoteAuthorizeReq->LockerNumber))
					{//------ packet error response	to server----------------
					
						pRemoteAuthorizeResultReq->base_request			=	&iBaseRequest;
						pRemoteAuthorizeResultReq->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
				    pRemoteAuthorizeResultReq->Last_Work_Type		=	remote_authorize_cmd;
				    pRemoteAuthorizeResultReq->Locker_Status		=	0x0;
						pRemoteAuthorizeResultReq->Locker_ACK				=	remote_authorize_un_Finger_p;

				    pRemoteAuthorizeResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteAuthorizeResultReq->Report_Time.str,pURemoteAuthorizeResp->ProtoTime,sizeof(pURemoteAuthorizeResp->ProtoTime));
				    pRemoteAuthorizeResultReq->Report_Time.len	=	0;

            remote_authorize_result_request_pack((void*)&iRemoteAuthorizeResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("%s,line=%d, s_length=%d, sock send error!!\n",__FILE__,__LINE__,res);
							}
						//pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						//return 0;
					}
					#endif
				}
					
				}
			//----------------- loop end -----------------------
			
        if((pUserAuthorize->dataType_2	>=	Finger_Number)&&(pUserAuthorize->dataType_2	<=	Identity_Card)&&(pUserAuthorize->Data_context_2.len > 3))
				{
          if ((pUserAuthorize->dataType_2 == Finger_Number) && (pRemoteAutorizeReq->iControlType == CREATEUSER) )
          	{
                 printf("%s,line:%d,remote authorize do not finger ven Create User, controlType do not =%d\n",__FILE__,__LINE__,pUserAuthorize->user_control_list);
					 	}else{
			    pURemoteAuthorizeReq->holidayControl				=		pUserAuthorize->user_control_list;
			    pURemoteAuthorizeReq->dataType							=		pUserAuthorize->dataType_2;
					ptr			=	pUserAuthorize->Data_context_2.data;
					ilen		=	pUserAuthorize->Data_context_2.len;
			    memcpy(pURemoteAuthorizeReq->context,ptr,ilen);		
		 			if(pRemoteAutorizeReq->iControlType != DELETEALL)
		 				wait_time_value =	WAIT_OTHER_CMD_TIME;
		 			else
		 				wait_time_value =	WAIT_DELETE_ALL_TIME;
 					//-------------	pack -> send -> wait -> unpack -> ack	-----------------

			    UartFrame		*pUartFrame 			= (UartFrame*)header;
			    pUartFrame->dataPtr						=	idata;
					pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pURemoteAuthorizeReq->LockerNumber); 
					pUartFrame->frameLength				= ilen+URemoteAuthorizeReq_STRUCT_HEADER_SIZE;
			    pUartFrame->frameTotal				=	0x01;
			    pUartFrame->frameNumber				=	0x01;
					//-------------		send command wait for sync single	-----------------
			    pUartPort->pipe		=	PIPE_SEND_REQUEST;	
					while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST)*/ ){
							//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
							usleep(10000);
					}
			    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
			    memset(pUartPort->UartSingleReceiveBuf,0,sizeof(pUartPort->UartSingleReceiveBuf));		//clear recive data space
          sendCnt = 0;
			    do{
			    		select_lora_states(&pUartPort->channelNumber,0,1);
							pUartPort->deviceStates	=	UartSendBusying;
							pUartPort->pipe		=	PIPE_SEND_REQUEST;							
			        res = write (pUartPort->clt_uart,u_buffer,u_length);
			        if((res <= 0)||(res != u_length))
							{
								while(1)
								{
                  sprintf((char*)stderr,"%s,line=%d,[%s] write error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                  printf("ERROR:%s",(char*)stderr);
									sleep(3);
								}
							}

							pUartPort->deviceStates	=	UartIDLE;
							u_delay	=	0	;
							while((pUartPort->pipe != RemoteAuthorizeACK)&&(u_delay <= wait_time_value ))
							{
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
							sendCnt	+=	1;	
							#ifdef		Debug_Level
							printf("%s,line:%d,[%s] send Authorize %d times \n",__FILE__,__LINE__,pUartPort->h_io,sendCnt);
							#endif									
           }while((pUartPort->pipe != RemoteAuthorizeACK)&&(sendCnt	<	3));
					//select_lora_states(&pUartPort->channelNumber,0,0);			
					//---- unpacket	Remote Open Door ACK Response --------------
					ptr	=	pUartPort->UartSingleReceiveBuf;
					
					#if	(Disable_auth_ack == 0)
					pURemoteAuthorizeResp		=	(URemoteAuthorizeResp*)ptr;					
					res	=	*ptr;
					if ((sendCnt > 3)||(*ptr	!=	RemoteAuthorizeACK)||(htons(pURemoteAuthorizeResp->RespACK)	!= RemoteAuthorizeACK_SUCC)
																													 ||(pURemoteAuthorizeResp->LockerNumber != pURemoteAuthorizeReq->LockerNumber))
					{//------ packet error response	to server----------------
						pRemoteAuthorizeResultReq->base_request			=	&iBaseRequest;
						pRemoteAuthorizeResultReq->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
				    pRemoteAuthorizeResultReq->Last_Work_Type		=	remote_authorize_cmd;
				    pRemoteAuthorizeResultReq->Locker_Status		=	0x0;
						pRemoteAuthorizeResultReq->Locker_ACK				=	remote_authorize_un_Card_p;
						
				    pRemoteAuthorizeResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteAuthorizeResultReq->Report_Time.str,pURemoteAuthorizeResp->ProtoTime,sizeof(pURemoteAuthorizeResp->ProtoTime));
				    pRemoteAuthorizeResultReq->Report_Time.len	=	0;
				    
            remote_authorize_result_request_pack((void*)&iRemoteAuthorizeResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("---%s,line=%d, s_length=%d, sock send error!!\n",__FILE__,__LINE__,res);
							}
						//pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						//return 0;
					}
					#endif
				}
								    		
				}
				//----------------- loop end -----------------------
				
        if((pUserAuthorize->dataType_3	>=	Finger_Number)&&(pUserAuthorize->dataType_3	<=	Identity_Card)&&(pUserAuthorize->Data_context_3.len > 3))
				{
          if ((pUserAuthorize->dataType_3 == Finger_Number) && (pRemoteAutorizeReq->iControlType == CREATEUSER) )
					{
              printf("%s,line:%d,remote authorize do not finger ven Create User, controlType do not =%d\n",__FILE__,__LINE__,pUserAuthorize->user_control_list);
					}else{
			    pURemoteAuthorizeReq->holidayControl				=		pUserAuthorize->user_control_list;
			    pURemoteAuthorizeReq->dataType							=		pUserAuthorize->dataType_3;
					ptr			=	pUserAuthorize->Data_context_3.data;
					ilen		=	pUserAuthorize->Data_context_3.len;
			    memcpy(pURemoteAuthorizeReq->context,ptr,ilen);
		 			if(pRemoteAutorizeReq->iControlType != DELETEALL)
		 				wait_time_value =	WAIT_OTHER_CMD_TIME;
		 			else
		 				wait_time_value =	WAIT_DELETE_ALL_TIME;
			   //-------------	pack -> send -> wait -> unpack -> ack	-----------------
			    UartFrame		*pUartFrame 			= (UartFrame*)header;
			    pUartFrame->dataPtr						=	idata;
					pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pURemoteAuthorizeReq->LockerNumber); 
					pUartFrame->frameLength				= ilen+URemoteAuthorizeReq_STRUCT_HEADER_SIZE;
			    pUartFrame->frameTotal				=	0x01;
			    pUartFrame->frameNumber				=	0x01;
					//-------------		send command wait for sync single	-----------------
			    pUartPort->pipe		=	PIPE_SEND_REQUEST;	
					while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST)*/ ){
							//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
							usleep(10000);
					}			  	
			    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
			    memset(pUartPort->UartSingleReceiveBuf,0,sizeof(pUartPort->UartSingleReceiveBuf));		//clear recive data space
			    sendCnt = 0;
			    do{
							select_lora_states(&pUartPort->channelNumber,0,1);
							pUartPort->deviceStates	=	UartSendBusying;
							pUartPort->pipe		=	PIPE_SEND_REQUEST;
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
							while((pUartPort->pipe != RemoteAuthorizeACK)&&(u_delay <= wait_time_value )){
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
						sendCnt	+=	1;
						#ifdef		Debug_Level
						printf("%s,line:%d,[%s] send Authorize %d times\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
						#endif										
					}while((pUartPort->pipe != RemoteAuthorizeACK)&&(sendCnt	<	3));

					//select_lora_states(&pUartPort->channelNumber,0,0);			
					//---- unpacket	Remote Open Door ACK Response --------------
					ptr	=	pUartPort->UartSingleReceiveBuf ;
					
					#if	(Disable_auth_ack == 0)					
					pURemoteAuthorizeResp		=	(URemoteAuthorizeResp*)ptr;
					res	=	*ptr;
					if ((sendCnt > 3)||(*ptr	!=	RemoteAuthorizeACK)||(htons(pURemoteAuthorizeResp->RespACK)	!= RemoteAuthorizeACK_SUCC)
																													 ||(pURemoteAuthorizeResp->LockerNumber != pURemoteAuthorizeReq->LockerNumber))
					{//------ packet error response	to server----------------
						pRemoteAuthorizeResultReq->base_request			=	&iBaseRequest;
						pRemoteAuthorizeResultReq->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
				    pRemoteAuthorizeResultReq->Last_Work_Type		=	remote_authorize_cmd;
				    pRemoteAuthorizeResultReq->Locker_Status		=	0x0;
						pRemoteAuthorizeResultReq->Locker_ACK				=	remote_authorize_un_Password_p;
						
				    pRemoteAuthorizeResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteAuthorizeResultReq->Report_Time.str,pURemoteAuthorizeResp->ProtoTime,sizeof(pURemoteAuthorizeResp->ProtoTime));
				    pRemoteAuthorizeResultReq->Report_Time.len	=	0;
				    
            remote_authorize_result_request_pack((void*)&iRemoteAuthorizeResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("---%s,line=%d, s_length=%d, sock send error!!\n",__FILE__,__LINE__,res);
							}
						//pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						//return 0;
					}
					#endif
				}
						    				
				}
			//----------------- loop end -----------------------
			
       if((pUserAuthorize->dataType_4	>=	Finger_Number)&&(pUserAuthorize->dataType_4	<=	Identity_Card)&&(pUserAuthorize->Data_context_4.len > 3))
				{
           if ((pUserAuthorize->dataType_4 == Finger_Number) && (pRemoteAutorizeReq->iControlType == CREATEUSER) )
					 {
              printf("%s,line:%d,remote authorize do not finger ven Create User, controlType do not =%d\n",__FILE__,__LINE__,pUserAuthorize->user_control_list);
					 }else{					
					
			    pURemoteAuthorizeReq->holidayControl				=		pUserAuthorize->user_control_list;
			    pURemoteAuthorizeReq->dataType							=		pUserAuthorize->dataType_4;
					ptr			=	pUserAuthorize->Data_context_4.data;
					ilen		=	pUserAuthorize->Data_context_4.len;
			    memcpy(pURemoteAuthorizeReq->context,ptr,ilen);
		 			if(pRemoteAutorizeReq->iControlType != DELETEALL)
		 				wait_time_value =	WAIT_OTHER_CMD_TIME;
		 			else
		 				wait_time_value =	WAIT_DELETE_ALL_TIME;
			   //-------------	pack -> send -> wait -> unpack -> ack	-----------------

			    UartFrame		*pUartFrame 			= (UartFrame*)header;
			    pUartFrame->dataPtr						=	idata;
					pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pURemoteAuthorizeReq->LockerNumber); 
					pUartFrame->frameLength				= ilen+URemoteAuthorizeReq_STRUCT_HEADER_SIZE;
			    pUartFrame->frameTotal				=	0x01;
			    pUartFrame->frameNumber				=	0x01;
					//-------------		send command wait for sync single	-----------------
			    pUartPort->pipe		=	PIPE_SEND_REQUEST;	
					while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST)*/ ){
							//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
							usleep(10000);
					}
			    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
			    memset(pUartPort->UartSingleReceiveBuf,0,sizeof(pUartPort->UartSingleReceiveBuf));		//clear recive data space
			    sendCnt	=	0;
			    do{
							select_lora_states(&pUartPort->channelNumber,0,1);
							pUartPort->deviceStates	=	UartSendBusying;
							pUartPort->pipe		=	PIPE_SEND_REQUEST;							
			        res = write (pUartPort->clt_uart,u_buffer,u_length);
			        if((res <= 0)||(res != u_length)){
								while(1){
			            //sprintf(stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
			            printf("ERROR:%s",(char*)stderr);
									sleep(3);
								}
							}
							pUartPort->deviceStates	=	UartIDLE;
							u_delay	=	0	;
              while((pUartPort->pipe != RemoteAuthorizeACK)&&(u_delay <= wait_time_value )){
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
							sendCnt	+=	1;			
						#ifdef		Debug_Level
						printf("%s,line:%d,[%s] send Authorize %d times \n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
						#endif							
					}while((pUartPort->pipe != RemoteAuthorizeACK)&&(sendCnt	<	3));
//					select_lora_states(&pUartPort->channelNumber,0,0);			
					//---- unpacket	Remote Open Door ACK Response --------------
					ptr	=	pUartPort->UartSingleReceiveBuf ;
					#if	(Disable_auth_ack == 0)
					pURemoteAuthorizeResp		=	(URemoteAuthorizeResp*)ptr;
					res	=	*ptr;
					if ((sendCnt > 3)||(*ptr	!=	RemoteAuthorizeACK)||(htons(pURemoteAuthorizeResp->RespACK)	!= RemoteAuthorizeACK_SUCC)
																													 ||(pURemoteAuthorizeResp->LockerNumber != pURemoteAuthorizeReq->LockerNumber))
					{//------ packet error response	to server----------------
						pRemoteAuthorizeResultReq->base_request			=	&iBaseRequest;
						pRemoteAuthorizeResultReq->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
				    pRemoteAuthorizeResultReq->Last_Work_Type		=	remote_authorize_cmd;
				    pRemoteAuthorizeResultReq->Locker_Status		=	0x0;
						pRemoteAuthorizeResultReq->Locker_ACK				=	remote_authorize_un_Identity_p;
						
				    pRemoteAuthorizeResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteAuthorizeResultReq->Report_Time.str,pURemoteAuthorizeResp->ProtoTime,sizeof(pURemoteAuthorizeResp->ProtoTime));
				    pRemoteAuthorizeResultReq->Report_Time.len	=	0;
				    
            remote_authorize_result_request_pack((void*)&iRemoteAuthorizeResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("%s,line=%d, s_length=%d, sock send error!!\n",__FILE__,__LINE__,res);
							}
						pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c								
						return 0;
					}
					#endif
				}
				}
		//----------------- loop end -----------------------
		
		//--------------	packet success response to server ----------
		pRemoteAuthorizeResultReq->base_request			=	&iBaseRequest;
		pRemoteAuthorizeResultReq->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
		pRemoteAuthorizeResultReq->Last_Work_Type		=	remote_authorize_cmd;
		pRemoteAuthorizeResultReq->Locker_Status		=	0x0;
		pRemoteAuthorizeResultReq->Locker_ACK				=	remote_authorize_ok_response;
		
		pRemoteAuthorizeResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
		memcpy((char*)pRemoteAuthorizeResultReq->Report_Time.str,pURemoteAuthorizeResp->ProtoTime,sizeof(pURemoteAuthorizeResp->ProtoTime));
		pRemoteAuthorizeResultReq->Report_Time.len	=	sizeof(pURemoteAuthorizeResp->ProtoTime);
		
    remote_authorize_result_request_pack((void*)&iRemoteAuthorizeResultReq,s_buffer,&s_length);
		res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
		if((res <= 0)||(res != s_length)){
				printf("---%s,line=%d, s_length=%d, sock send error!!\n",__FILE__,__LINE__,res);
			}
    printf("%s,line:%d,Remote authorize result do success\n",__FILE__,__LINE__);
    pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
		return	1;
}
