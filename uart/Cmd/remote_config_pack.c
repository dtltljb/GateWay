#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>

#include 				"remote_config_pack.h"
#include 				"uart/UartPacket.h"
#include		"uart/xxx_channel_send_thread.h"
#include    		"uart/LinkLayer/UartOpration.h"
#include    "uart/LinkLayer/lora_parament.h"

#include        "socket/VirtualPortMonitor.h"
#include        "socket/protoBuf/epb_remote_config_pack.h"
#include        "socket/protoBuf/epb_remote_get_config_pack.h"
#include        "socket/pub/util.h"
#include 				"util/pthread_data_copy.h"

#define					Disable_config_ack			0

ConfigLockerInfoReq					mConfigLockerInfoReq;
ConfigLockerInfoResp				mConfigLockerInfoResp;


/***********************************************************************
 * @brief				uart_request_remote_config_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t uart_request_remote_config_pack(void *args, void	*argu)
{
		UartPort				*pChangeChannel;
	  UartPort							*pUartPort						= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
		BaseRequest 		iBaseRequest = {NULL};
    uint32_t 				res;
    uint8_t				iLen,*ptr;
    uint8_t				l_arrBcdtime[8];
		uint8_t  			header[32];
		uint8_t  			idata[sizeof(ConfigLockerInfoReq)];
		uint8_t				s_buffer[128];		uint16_t	s_length;
    uint8_t				u_buffer[255],u_length;
		uint8_t				u_delay	=	0,sendCnt = 0;
    uint8_t				chn,ret;
            
		RemoteConfigResultReq		iRemoteConfigResultReq,*pRemoteConfigResultReq;
		pRemoteConfigResultReq	= &iRemoteConfigResultReq;
    ConfigLockerInfoResp	*pConfigLockerInfoResp;
    //-	-	-	-	-	-	free data space	-	-	-	-	-	-	-	  
    res    = sizeof(RemoteConfigReq);  
		RemoteConfigReq							iRemoteConfigReq;
    pthread_data_copy((char*)&iRemoteConfigReq,(char*)pVirtualSocket->pContext.p,(int*)&res);
    RemoteConfigReq							*pRemoteConfigReq					=	&iRemoteConfigReq;
  	
    iRemoteConfigReq.first_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_1;
    iRemoteConfigReq.second_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_2;
    iRemoteConfigReq.third_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_3;

    ConfigLockerInfoReq *pConfigLockerInfoReq		=		(ConfigLockerInfoReq*)idata;
		pConfigLockerInfoReq->cmd										=		RemoteConfig;
		pConfigLockerInfoReq->LockerNumber					=		t_ntohl(pRemoteConfigReq->Locker_Address);

		pConfigLockerInfoReq->controlType						=		CONFIG_CONTROL_TYPE_WRITE;					//


        
        if((pRemoteConfigReq->first_data_type != 0)&&(pRemoteConfigReq->first_data_type <= (enum data_type)LoraPorperty)&&(pRemoteConfigReq->first_data_group.len > 3))
	  		{
	  			//-	-	-	-	-	-	-	public		part	-	-	-	-	-	-	-	
          if ( pRemoteConfigReq->first_data_group.len > (int)sizeof(ConfigLockerInfoReq) ){
	  				printf("%s,line:%d,[%s] first_data_context over length=%d!\n",__FILE__,__LINE__,pUartPort->h_io, pRemoteConfigReq->first_data_group.len);
	  			}
	  			else
	  			{
		  		//------------ pack -> send -> wait -> unpack -> ack ---------------------    

			    pConfigLockerInfoReq->DataType	=		pRemoteConfigReq->first_data_type;
			    iLen		=		pRemoteConfigReq->first_data_group.len;
					memcpy(pConfigLockerInfoReq->data,pRemoteConfigReq->first_data_group.data,iLen);
			    UartFrame		*pUartFrame 			= (UartFrame*)header;
			    pUartFrame->dataPtr						=	idata;
					pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pConfigLockerInfoReq->LockerNumber); 
					pUartFrame->frameLength				= iLen +	ConfigLockerInfoReq_STRUCT_HEADER_SIZE;
			    pUartFrame->frameTotal				=	0x01;
			    pUartFrame->frameNumber				=	0x01;
					//-------------		send command wait for sync single	-----------------
			    pUartPort->pipe		=	PIPE_SEND_REQUEST;	
					while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST) */){
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
			        if((res <= 0)||(res != u_length))
							{
								while(1){
                        sprintf((char*)stderr,"%s,line=%d, [ %s ] write error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                        printf("ERROR:%s",(char*)stderr);
									sleep(3);
								}
							}
							pUartPort->deviceStates	=	UartIDLE;
							u_delay	=	0	;
							while((pUartPort->pipe != RemoteConfigACK)&&(u_delay <= 80 ))
							{
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
							sendCnt	+=	1;
							#ifdef		Debug_Level
              printf("%s,line:%d,[ %s ] send config  count=%d\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
							#endif
					}while((pUartPort->pipe != RemoteConfigACK)&&(sendCnt	<	3));
					//select_lora_states(&pUartPort->channelNumber,0,0);
					//---- unpacket uart	remote load ack Response --------------
						res	=	sizeof(pUartPort->ReceiveBuf);
						pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
						ptr	=	pUartPort->ReceiveBuf;

					#if	(	Disable_config_ack == 0 )
					pConfigLockerInfoResp		=	(ConfigLockerInfoResp*)ptr;
          if ((sendCnt > 3)||(pConfigLockerInfoResp->cmd	!=	RemoteConfigACK)||(pConfigLockerInfoResp->LockerNumber != pConfigLockerInfoReq->LockerNumber)||(htons(pConfigLockerInfoResp->RespACK)	!= RemoteConfigACK_SUCC))
					{//------ packet error response	to server----------------
						pRemoteConfigResultReq->base_request			=	&iBaseRequest;
						pRemoteConfigResultReq->Locker_Address		=	pRemoteConfigReq->Locker_Address;
						pRemoteConfigResultReq->Last_Work_Type		=	remote_config_cmd;
						pRemoteConfigResultReq->Locker_ACK				=	remote_config_un_first_p;
				    pRemoteConfigResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteConfigResultReq->Report_Time.str,pConfigLockerInfoResp->ProtoTime,sizeof(pConfigLockerInfoResp->ProtoTime));
				    pRemoteConfigResultReq->Report_Time.len	=	0;

            remote_config_result_request_pack((void*)&iRemoteConfigResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("---%s,line=%d, s_length=%d, socket error!!\n",__FILE__,__LINE__,res);
							}
						//pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						//return 0;
					}
          #endif
					usleep(1000000);
					}
				}
      if((pRemoteConfigReq->second_data_type != 0)&&(pRemoteConfigReq->second_data_type <= (enum data_type)LoraPorperty)&&(pRemoteConfigReq->second_data_group.len > 3))
			{
				//-	-	-	-	-	-	-	public		part	-	-	-	-	-	-	-	
          if ( pRemoteConfigReq->second_data_group.len > (int)sizeof(ConfigLockerInfoReq) ){
	  				printf("%s,line:%d,first_data_context over length=%d!\n",__FILE__,__LINE__,pRemoteConfigReq->second_data_group.len);
	  			}else{
		  		//------------ pack -> send -> wait -> unpack -> ack ---------------------    

			    pConfigLockerInfoReq->DataType	=		pRemoteConfigReq->second_data_type;
			    iLen		=		pRemoteConfigReq->second_data_group.len;
					memcpy(pConfigLockerInfoReq->data,pRemoteConfigReq->second_data_group.data,iLen);
			    UartFrame		*pUartFrame 			= (UartFrame*)header;
			    pUartFrame->dataPtr						=	idata;
					pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pConfigLockerInfoReq->LockerNumber); 
					pUartFrame->frameLength				= iLen +	ConfigLockerInfoReq_STRUCT_HEADER_SIZE;
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
                        sprintf((char*)stderr,"%s,line=%d, [ %s ] write error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                        printf("ERROR:%s",(char*)stderr);
									sleep(3);
								}
							}
			        
							pUartPort->deviceStates	=	UartIDLE;
							u_delay	=	0	;
							while((pUartPort->pipe != RemoteConfigACK)&&(u_delay <= 80 )){
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
							
							sendCnt	+=	1;
							#ifdef		Debug_Level
							printf("%s,line:%d, [%s] send remote config %d times\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
							#endif
					}while((pUartPort->pipe != RemoteConfigACK)&&(sendCnt	<	3));
					//select_lora_states(&pUartPort->channelNumber,0,0);	
					//---- unpacket uart	remote load ack Response --------------
						res	=	sizeof(pUartPort->ReceiveBuf);
						pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
						ptr	=	pUartPort->ReceiveBuf;

					#if	(	Disable_config_ack == 0 )
					pConfigLockerInfoResp		=	(ConfigLockerInfoResp*)ptr;
					if ((sendCnt > 3)||(pConfigLockerInfoResp->cmd	!=	RemoteConfigACK)||(pConfigLockerInfoResp->LockerNumber != pConfigLockerInfoReq->LockerNumber)||(htons(pConfigLockerInfoResp->RespACK)	!= RemoteConfigACK_SUCC))
					{//------ packet error response	to server----------------
						pRemoteConfigResultReq->base_request			=	&iBaseRequest;
						pRemoteConfigResultReq->Locker_Address		=	pRemoteConfigReq->Locker_Address;
						pRemoteConfigResultReq->Last_Work_Type		=	remote_config_cmd;
						pRemoteConfigResultReq->Locker_ACK				=	remote_config_un_second_p;
				    pRemoteConfigResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteConfigResultReq->Report_Time.str,pConfigLockerInfoResp->ProtoTime,sizeof(pConfigLockerInfoResp->ProtoTime));
				    pRemoteConfigResultReq->Report_Time.len	=	0;

            remote_config_result_request_pack((void*)pRemoteConfigResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("%s,line=%d, s_length=%d, sock send error!!\n",__FILE__,__LINE__,res);
							}
						//pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						//return 0;
					}
          #endif				
				usleep(1000000);
			}
     }
      if((pRemoteConfigReq->third_data_type != 0)&&(pRemoteConfigReq->third_data_type <= (enum data_type)LoraPorperty)&&(pRemoteConfigReq->third_data_group.len > 3))
			{
				//-	-	-	-	-	-	-	public		part	-	-	-	-	-	-	-	
           if ( pRemoteConfigReq->second_data_group.len > (int)sizeof(ConfigLockerInfoReq) ){
	  				printf("%s,line:%d,first_data_context over length=%d!\n",__FILE__,__LINE__,pRemoteConfigReq->second_data_group.len);
	  			}
	  			else
	  			{
		  		//------------ pack -> send -> wait -> unpack -> ack ---------------------    
					uint32_t Fre;

			    pConfigLockerInfoReq->DataType	=		pRemoteConfigReq->third_data_type;
			    iLen		=		pRemoteConfigReq->third_data_group.len;
					memcpy(pConfigLockerInfoReq->data,pRemoteConfigReq->third_data_group.data,iLen);
					
			    UartFrame		*pUartFrame 			= (UartFrame*)header;
			    pUartFrame->dataPtr						=	idata;
					pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pConfigLockerInfoReq->LockerNumber); 
					pUartFrame->frameLength				= iLen +	ConfigLockerInfoReq_STRUCT_HEADER_SIZE;
			    pUartFrame->frameTotal				=	0x01;
			    pUartFrame->frameNumber				=	0x01;
			    // - - - - - - - get lora change channel  - - - - -
			    LockerProperty	*pLockerProperty;
			    pLockerProperty	=	(LockerProperty*)pConfigLockerInfoReq->data;
			    
			    Fre											=		pLockerProperty->frequency[0];
					Fre											<<=	8;
					Fre											|=	pLockerProperty->frequency[1];
					Fre											<<=	8;
					Fre											|=	pLockerProperty->frequency[2];		
			    ret = get_lora_channel_ByFrequency(&chn,Fre);
			    printf("%s,line:%d, channel=%d,Fre=0x%x \n",__FILE__,__LINE__,chn,Fre);
			    
			    if ( ret != 1){	//------ packet error response	to server----------------
						pRemoteConfigResultReq->base_request			=	&iBaseRequest;
						pRemoteConfigResultReq->Locker_Address		=	pRemoteConfigReq->Locker_Address;
						pRemoteConfigResultReq->Last_Work_Type		=	remote_config_cmd;
						pRemoteConfigResultReq->Locker_ACK				=	remote_config_un_third_p;
				    pRemoteConfigResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteConfigResultReq->Report_Time.str,pConfigLockerInfoResp->ProtoTime,sizeof(pConfigLockerInfoResp->ProtoTime));
				    pRemoteConfigResultReq->Report_Time.len	=	0;

            remote_config_result_request_pack((void*)pRemoteConfigResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("---%s,line=%d, s_length=%d, socket error!!\n",__FILE__,__LINE__,res);
							}
						pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						return 0;
			    }
			    pChangeChannel	=	&mUartPort[chn - 1];											//get change channel states	  
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
							
							//pChangeChannel->pipe		=	PIPE_SEND_REQUEST;							
			        res = write (pUartPort->clt_uart,u_buffer,u_length);
			        if((res <= 0)||(res != u_length)){
								while(1){
                        sprintf((char*)stderr,"%s,line=%d,  %s write error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                        printf("ERROR:%s",(char*)stderr);
									sleep(3);
								}
							}

							pUartPort->deviceStates	=	UartIDLE;
							u_delay	=	0	;
							while((pChangeChannel->pipe != RemoteConfigACK)&&(u_delay <= 80 )){
								//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
                usleep(100000);
                u_delay	=	u_delay	+	1 ;
							}
							sendCnt	+=	1;
							#ifdef		Debug_Level
							printf("%s,line:%d,[ %s ] send remote config time=%d\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
							#endif
					}while((pChangeChannel->pipe != RemoteConfigACK)&&(sendCnt	<	3));
					//select_lora_states(&pUartPort->channelNumber,0,0);
					//---- unpacket uart	remote load ack Response --------------
						res	=	sizeof(pUartPort->ReceiveBuf);
						//pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
						pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pChangeChannel->UartSingleReceiveBuf,(int*)&res);
						ptr	=	pUartPort->ReceiveBuf;
						
					#if	(	Disable_config_ack == 0 )
					pConfigLockerInfoResp		=	(ConfigLockerInfoResp*)ptr;
					if ((sendCnt > 3)||(pConfigLockerInfoResp->cmd	!=	RemoteConfigACK)||(htons(pConfigLockerInfoResp->RespACK)	!= RemoteConfigACK_SUCC) /*||(pConfigLockerInfoResp->LockerNumber != pConfigLockerInfoReq->LockerNumber) because replace Number */ )
					{//------ packet error response	to server----------------
						pRemoteConfigResultReq->base_request			=	&iBaseRequest;
						pRemoteConfigResultReq->Locker_Address		=	pRemoteConfigReq->Locker_Address;
						pRemoteConfigResultReq->Last_Work_Type		=	remote_config_cmd;
						pRemoteConfigResultReq->Locker_ACK				=	remote_config_un_third_p;
				    pRemoteConfigResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
				    memcpy((char*)pRemoteConfigResultReq->Report_Time.str,pConfigLockerInfoResp->ProtoTime,sizeof(pConfigLockerInfoResp->ProtoTime));
				    pRemoteConfigResultReq->Report_Time.len	=	0;

            remote_config_result_request_pack((void*)pRemoteConfigResultReq,s_buffer,&s_length);
					  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
						if((res <= 0)||(res != s_length)){
								printf("---%s,line=%d, s_length=%d, socket error!!\n",__FILE__,__LINE__,res);
							}
						pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c		
						return 0;
					}
          #endif				
			}
     }
		//--------------	packet success response to server ----------
			pRemoteConfigResultReq->base_request			=	&iBaseRequest;
			pRemoteConfigResultReq->Locker_Address		=	pRemoteConfigReq->Locker_Address;
			pRemoteConfigResultReq->Last_Work_Type		=	remote_config_cmd;
			pRemoteConfigResultReq->Locker_ACK				=	remote_config_ok_response;
			pRemoteConfigResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
      memcpy((char*)pRemoteConfigResultReq->Report_Time.str,pConfigLockerInfoResp->ProtoTime,sizeof(pConfigLockerInfoResp->ProtoTime));
			pRemoteConfigResultReq->Report_Time.len	=	sizeof(pConfigLockerInfoResp->ProtoTime);

      remote_config_result_request_pack((void*)pRemoteConfigResultReq,s_buffer,&s_length);
		  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
			if((res <= 0)||(res != s_length)){
					printf("%s,line=%d, s_length=%d, socket error!!\n",__FILE__,__LINE__,res);
				}
    printf("%s,line:%d,[ %s] Remote config  success\n",__FILE__,__LINE__,pUartPort->h_io);
    pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
		return	1;
}



/***********************************************************************
 * @brief				uart_request_remote_config_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/

uint8_t uart_request_remote_get_config_pack(void *args, void	*argu)
{
	  UartPort							*pUartPort					= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
		uint8_t				*ptr;				
    uint8_t				sendCnt	=	0,u_delay = 0 ;
		BaseRequest 		iBaseRequest = {NULL};
    uint32_t 							res;
  
		uint8_t  			header[32],idata[sizeof(ConfigLockerInfoReq)];

		uint8_t				s_buffer[128];
		uint16_t			s_length;
		uint8_t				u_buffer[255],u_length =0;

    //-	-	-	-	-	-	free data space	-	-	-	-	-	-	-	    
    ConfigLockerInfoResp	iConfigLockerInfoResp,*pConfigLockerInfoResp;
    RemoteGetConfigResultReq        iRemoteGetConfigResultReq;
    memset(&iRemoteGetConfigResultReq,0,sizeof(RemoteGetConfigResultReq));
    
    RemoteGetConfigReq							iRemoteGetConfigReq,*pRemoteGetConfigReq;
    res    = sizeof(RemoteGetConfigReq);     
    pthread_data_copy((char*)&iRemoteGetConfigReq,(char*)pVirtualSocket->pContext.p,(int*)&res);
    pRemoteGetConfigReq					=	&iRemoteGetConfigReq;

	 	//------------ pack -> send -> wait -> unpack -> ack ---------------------   				
	 	
    ConfigLockerInfoReq *pConfigLockerInfoReq		=		(ConfigLockerInfoReq*)idata;
		pConfigLockerInfoReq->cmd										=		RemoteConfig;
		pConfigLockerInfoReq->LockerNumber					=		t_ntohl(pRemoteGetConfigReq->Locker_Address);
		pConfigLockerInfoReq->DataType							=		LoraPorperty;
		pConfigLockerInfoReq->controlType						=		CONFIG_CONTROL_TYPE_READ;   

    UartFrame		*pUartFrame 				= (UartFrame*)header;
    pUartFrame->dataPtr							=	idata;
		pUartFrame->Dst_Address					=	(uint16_t)t_ntohl(pConfigLockerInfoReq->LockerNumber); 
    pUartFrame->frameLength					= 07;				//ConfigLockerInfoReq_STRUCT_HEADER_SIZE;
		pUartFrame->frameTotal					=	0x01;
		pUartFrame->frameNumber					=	0x01;
		//-------------		send command wait for sync single	-----------------
		
		pUartPort->pipe		=	PIPE_SEND_REQUEST;	
		while( (pUartPort->deviceStates != UartIDLE)/*&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST)*/ ){
				//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
				usleep(10000);
		}
		res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
    sendCnt	=	0;
		do{
				select_lora_states(&pUartPort->channelNumber,0,1);
				pUartPort->deviceStates	=	UartSendBusying;
				pUartPort->pipe		=	PIPE_SEND_REQUEST;
			  res = write (pUartPort->clt_uart,u_buffer,u_length);
			  if((res <= 0)||(res != u_length)){
					while(1){
                  sprintf((char*)stderr,"%s,line=%d, write [ %s ] error!!\n",__FILE__,__LINE__,pUartPort->h_io);
                  printf("ERROR:%s",(char*)stderr);
						sleep(3);
					}
				}

				pUartPort->deviceStates	=	UartIDLE;
				u_delay	=	0	;
				while((pUartPort->pipe != RemoteConfigACK)&&(u_delay <= 80)){
					//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
          usleep(100000);
          u_delay	=	u_delay	+	1 ;
				}
				sendCnt	+=	1;
				#ifdef		Debug_Level
         printf("%s,line:%d, [%s] send config  count=%d\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
				#endif
		}while((pUartPort->pipe != RemoteConfigACK)&&(sendCnt	<	3));
		select_lora_states(&pUartPort->channelNumber,0,0);
		//---- unpacket uart	remote load ack Response --------------
		res	=	sizeof(pUartPort->ReceiveBuf);
		pthread_data_copy((char*)pUartPort->ReceiveBuf,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
		ptr	=	pUartPort->ReceiveBuf;

		pConfigLockerInfoResp		=	(ConfigLockerInfoResp*)ptr;
    if ((sendCnt > 3)||(pConfigLockerInfoResp->cmd	!=	RemoteConfigACK)||(pConfigLockerInfoResp->LockerNumber != pConfigLockerInfoReq->LockerNumber)||(htons(pConfigLockerInfoResp->RespACK)	!= RemoteConfigACK_SUCC))
		{					//------ packet error response	to server----------------
                iRemoteGetConfigResultReq.base_request				=	&iBaseRequest;
                iRemoteGetConfigResultReq.Locker_Address			=	pRemoteGetConfigReq->Locker_Address;
                iRemoteGetConfigResultReq.Locker_ACK					=	remote_config_un_get_p;
                iRemoteGetConfigResultReq.data_type						=	LoraPorperty;
                iRemoteGetConfigResultReq.data_context.len		=	0;
                iRemoteGetConfigResultReq.Report_Time.len     = 0;
		}else{		//--------------	packet success response to server ----------
          memcpy((char*)&iConfigLockerInfoResp,ptr,sizeof(ConfigLockerInfoResp));
          pConfigLockerInfoResp =  &iConfigLockerInfoResp;
          iRemoteGetConfigResultReq.base_request			=	&iBaseRequest;
          iRemoteGetConfigResultReq.Locker_Address    =	pRemoteGetConfigReq->Locker_Address;
          iRemoteGetConfigResultReq.Locker_ACK				=	remote_load_ok_response;
          iRemoteGetConfigResultReq.data_type					=	LoraPorperty;
          iRemoteGetConfigResultReq.data_context.len				=		sizeof(pConfigLockerInfoResp->data);
          iRemoteGetConfigResultReq.data_context.data       =   pConfigLockerInfoResp->data;
          iRemoteGetConfigResultReq.Report_Time.len         =   sizeof(pConfigLockerInfoResp->ProtoTime);
          iRemoteGetConfigResultReq.Report_Time.str         =   (char*)pConfigLockerInfoResp->ProtoTime;
		}

    remote_get_config_result_request_pack((void*)&iRemoteGetConfigResultReq,s_buffer,&s_length);

		res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
		if((res <= 0)||(res != s_length)){
					printf("---%s,line=%d, s_length=%d, send error!!\n",__FILE__,__LINE__,res);
		}
		pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
		return	1;
}

