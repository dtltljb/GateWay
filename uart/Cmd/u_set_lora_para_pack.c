#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <pthread.h>

#include 				"u_set_lora_para_pack.h"
#include 				"uart/UartPacket.h"
#include    		"uart/LinkLayer/UartOpration.h"
#include        "socket/VirtualPortMonitor.h"
#include        "socket/protoBuf/epb_set_lora_param_pack.h"
#include 				"socket/pub/util_date.h"
#include 				"util/pthread_data_copy.h"

 /******************************************
 * public process: iodevice  read none wait
 *
 * ****************************************/


static int uart_read_wait_time(int fd, int sec)
{
	  fd_set rset;
    struct timeval tv;
    int    rc;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    //return select(fd+1, &rset, NULL, NULL, &tv);
    rc	=	select(fd+1, &rset, NULL, NULL, &tv);
    printf("->> %s,line:%d,	fd:%d,sec:%d\n",__FILE__,__LINE__,fd,sec );
    if( rc<0 )  //error
     return -1;
    return FD_ISSET(fd, &rset)  ? 1: 0;
}


/***********************************************************************
 * @brief				uart_request_remote_config_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/

uint8_t uart_set_lora_para_pack(void *args, void	*argu)
{
		struct	SetLoraFrame mSetLoraFrame={
			  .megicCode	=	{0xff,0x56,0xae,0x35,0xa9,0x55,0x90},
			  .address			=	02,
			  .length				=	14
			};
		
		struct	GetLoraFrame mGetLoraFrame={
			  .megicCode	=	{0xff,0x56,0xae,0x35,0xa9,0x55,0xf0},
			  .address			=	02,
			  .length				=	14
			};

    uint8_t				u_buffer[128],u_length;
    uint8_t					s_buffer[128];
		uint16_t				s_length;
    uint8_t				sendCnt	=	0,*ptr;
    //uint8_t				loraDefaultVal[]={07,04,0xE0,0x0A, 0x16, 0x03, 0x00, 0x50, 0x01, 0x01, 0x40, 0x10, 0x01, 0x18};
    
    uint8_t 			l_arrBcdtime[8] ={0};
		BaseRequest 		iBaseRequest = {NULL};
		SetLoraParaResultReq		iSetLoraParaResultReq;
		
    uint32_t 								res;
    UartPort										*pUartPort					= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
    uint8_t			lora_para[15];  //loraChannel

    //-	-	-	-	-	-	free data space	-	-	-	-	-	-	-	    
    LoraProperty			*pLoraProperty;
    uint8_t		databuf[sizeof(LoraProperty)];
    SetLoraParamReq		iSetLoraParamReq;
//    RemoteOpenDoorReq				*pRemoteOpenDoorReq		=	(RemoteOpenDoorReq*)idata;
//    RemoteOpenReq						*pRemoteOpenReq				=	(RemoteOpenReq*)pVirtualSocket->pContext.p;
    res    = sizeof(SetLoraParamReq);
    pthread_data_copy((char*)&iSetLoraParamReq,(char*)pVirtualSocket->pContext.p,(int*)&res);
    SetLoraParamReq		*pSetLoraParamReq					=	&iSetLoraParamReq;

    memcpy(lora_para,iSetLoraParamReq.loraValue.data,iSetLoraParamReq.loraValue.len);

    if ( pSetLoraParamReq->Control_type	!= READ_LORA_PARA_CMD )
    {
//    // - - - - - debug lora para - - - -  - -
//      pSetLoraParamReq->loraValue.data = loraDefaultVal;
//   
		memcpy(databuf, pSetLoraParamReq->loraValue.data, pSetLoraParamReq->loraValue.len);
		pSetLoraParamReq->loraValue.data			=		databuf;
    pLoraProperty		=	(LoraProperty*)databuf;

		pUartPort->pipe		=	PIPE_LORA_REQUEST;
		
		while( (pUartPort->deviceStates != UartIDLE)&&(pUartPort->pipe	!=	PIPE_LORA_REQUEST) ){
				//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
				usleep(10000);
		}
		
		//------------ pack -> send -> wait -> unpack -> ack ---------------------    
		  	
     memcpy( u_buffer,&mSetLoraFrame,sizeof(struct SetLoraFrame) );
     memcpy( (u_buffer+ sizeof(struct SetLoraFrame) ),databuf,iSetLoraParamReq.loraValue.len);
     u_length	=	iSetLoraParamReq.loraValue.len	+ sizeof(struct  SetLoraFrame);

      #if (Debug_Level == 1)
			printf("uart packet data len=%d\n",u_length);
		  for(int16_t i=0;i<u_length;i++)
		         printf(" %02x",u_buffer[i]);
		  printf("\n %s,line:%d ,[ %s ] set lora prm \n",__FILE__,__LINE__,pUartPort->h_io);
		  #endif
		  select_lora_states(&pUartPort->channelNumber,1,1);
          //loraChannel = 1 ;
			do{
					pUartPort->deviceStates	=	UartSendBusying; 
					pUartPort->pipe		=	PIPE_LORA_REQUEST;

			    res = write (pUartPort->clt_uart,u_buffer,u_length);

			    if((res <= 0)||(res != u_length))
					{
						while(1){
			        printf("%s,line=%d,[ %s ] write error...\n",__FILE__,__LINE__,pUartPort->h_io);
			        //printf("ERROR:%s",stderr);
							sleep(3);
						}
					}
					pUartPort->deviceStates	=	UartIDLE;
					while((pUartPort->pipe != PIPE_RECE_SUCCESS)&&(pUartPort->pipe != PIPE_RECE_TIMEOUT))
					{
						usleep(10000);	
					}
					sendCnt	+=	1;
					#ifdef		Debug_Level
					printf("%s,line:%d,[ %s ] set lora set %d times\n",__FILE__,__LINE__,pUartPort->h_io,sendCnt);
					#endif
			}while((pUartPort->pipe != PIPE_RECE_SUCCESS)&&(sendCnt	<	3));

			//---- unpacket lora	Response frame header is 0x24 --------------
			select_lora_states(&pUartPort->channelNumber,0,0);
			ptr	=	pUartPort->UartSingleReceiveBuf;
			res	=	*ptr;

      if ((sendCnt > 3)||( res	!=	0x24 ) )
      {
				iSetLoraParaResultReq.base_request			=	&iBaseRequest;
				iSetLoraParaResultReq.Locker_Address		=	0x0000;
				iSetLoraParaResultReq.Last_Work_Type		=	set_lora_param_cmd;
				iSetLoraParaResultReq.Locker_Status			=	0x00;
		    iSetLoraParaResultReq.Locker_ACK				=	set_lora_param_un_return;
		    Get_CurBCDTime7((char*)l_arrBcdtime);
		    iSetLoraParaResultReq.Report_Time.str		=	(char*)l_arrBcdtime;
		    iSetLoraParaResultReq.Report_Time.len		=	sizeof(l_arrBcdtime);   
		    iSetLoraParaResultReq.Read_context.data	=	NULL;  
				iSetLoraParaResultReq.Read_context.len	=	0x0;
			}else{
				int res	=1;
				while(res){
				res	=	pthread_mutex_trylock(&lora_prm_mutex_lock);
				usleep(10000);
			}
//				pthread_mutex_lock(&lora_prm_mutex_lock);
//-	-	-	-	- update	lora	parament	-	-	-	-	

				pUartPort->FreqValue											=	pLoraProperty->frequency[0];
				pUartPort->FreqValue											=	pUartPort->FreqValue<<8;
				pUartPort->FreqValue											|=	pLoraProperty->frequency[1];
				pUartPort->FreqValue											=	pUartPort->FreqValue<<8;
				pUartPort->FreqValue											|=	pLoraProperty->frequency[2];
				pUartPort->AirRate												=	pLoraProperty->AirRate;
        pUartPort->GateWayAddress									=	pLoraProperty->GateWayAddress;        
        
        pthread_mutex_unlock(&lora_prm_mutex_lock);
        
 				#ifdef Debug_Level
          printf("%s, pUartPort->FreqValue = 0x%02x \n", pUartPort->h_io, pUartPort->FreqValue);
          printf("%s, pUartPort->AirRate = 0x%02x \n",pUartPort->h_io, pUartPort->AirRate);
          printf("%s, pUartPort->GateWayAddress = 0x%02x \n",pUartPort->h_io, htons(pUartPort->GateWayAddress));
        #endif

				iSetLoraParaResultReq.base_request			=	&iBaseRequest;
				iSetLoraParaResultReq.Locker_Address		=	0x0000;
				iSetLoraParaResultReq.Last_Work_Type		=	set_lora_param_cmd;
				iSetLoraParaResultReq.Locker_Status			=	0x00;
		    iSetLoraParaResultReq.Locker_ACK				=	set_lora_param_ok_return;
		    Get_CurBCDTime7((char*)l_arrBcdtime);
		    iSetLoraParaResultReq.Report_Time.str		=	(char*)l_arrBcdtime;
		    iSetLoraParaResultReq.Report_Time.len		=	sizeof(l_arrBcdtime);   
				iSetLoraParaResultReq.Read_context.data	=	NULL;		      
				iSetLoraParaResultReq.Read_context.len	=	0x0;
        #ifdef		Debug_Level
				printf("%s,line:%d,[ %s ] set lora parament okay \n",__FILE__,__LINE__,pUartPort->h_io);
				#endif
			}
          //ptr = s_buffer;
      set_lora_result_request_pack((void*)&iSetLoraParaResultReq,s_buffer,&s_length);
		  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
			if((res <= 0)||(res != s_length)){
					printf(" %s,line=%d, s_length=%d, sock send error \n",__FILE__,__LINE__,res);
				}
			pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  config channel
	    printf("%s,line:%d,Remote_Open_Door_response success\n",__FILE__,__LINE__);
	    pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
			return	1;
		}else{
			
    pLoraProperty		=	(LoraProperty*)databuf;
		while( (pUartPort->deviceStates != UartIDLE)&&(pUartPort->pipe	!=	PIPE_SEND_REQUEST))
		{
				//printf("%s,line:%d,uart states=%c\n",__FILE__,__LINE__,pUartPort->deviceStates);
				usleep(10000);
		}
		//------------ pack -> send -> wait -> unpack -> ack ---------------------    
		  	
     memcpy( u_buffer,&mGetLoraFrame,sizeof(struct GetLoraFrame) );
     u_length	=	sizeof(struct  GetLoraFrame) ;

      #if (Debug_Level == 1)
			printf("uart packet data len=%d\n",u_length);
		  for(int16_t i=0;i<u_length;i++)
		         printf(" %02x",u_buffer[i]);
		  printf("\n\r%s,line:%d \n",__FILE__,__LINE__);
		  #endif
     select_lora_states(&pUartPort->channelNumber,1,1);
			do{
					pUartPort->deviceStates	=	UartSendBusying; 
					pUartPort->pipe		=	PIPE_LORA_REQUEST;
			    res = write (pUartPort->clt_uart,u_buffer,u_length);
			    if((res <= 0)||(res != u_length))
					{
						while(1){
			        printf("%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
			        //printf("ERROR:%s",stderr);
							sleep(3);
						}
					}
					pUartPort->deviceStates	=	UartIDLE;
					while((pUartPort->pipe != PIPE_RECE_SUCCESS)&&(pUartPort->pipe != PIPE_RECE_TIMEOUT)){
						usleep(10000);	
					}
					sendCnt	+=	1;
					#ifdef		Debug_Level
					printf("%s,line:%d,[%s] send write lora command times=%d\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
					#endif
			}while((pUartPort->pipe != PIPE_RECE_SUCCESS)&&(sendCnt	<	3));
			//---- unpacket lora	Response frame header is 0x24 --------------
      select_lora_states(&pUartPort->channelNumber,0,0);
			ptr	=	pUartPort->UartSingleReceiveBuf;
			res	=	*ptr;
      if ((sendCnt > 3)||( res	!=	0x24 ) )
      {
      	iSetLoraParaResultReq.base_request			=	&iBaseRequest;
				iSetLoraParaResultReq.Locker_Address		=	0x0000;
				iSetLoraParaResultReq.Last_Work_Type		=	set_lora_param_cmd;
				iSetLoraParaResultReq.Locker_Status			=	0x00;
		    iSetLoraParaResultReq.Locker_ACK				=	set_lora_param_un_return;
		    Get_CurBCDTime7((char*)l_arrBcdtime);
		    iSetLoraParaResultReq.Report_Time.str		=	(char*)l_arrBcdtime;
		    iSetLoraParaResultReq.Report_Time.len		=	sizeof(l_arrBcdtime);   
		    iSetLoraParaResultReq.Read_context.data	=	NULL;  
				iSetLoraParaResultReq.Read_context.len	=	0x0;
			}else{
				
				iSetLoraParaResultReq.base_request			=	&iBaseRequest;
				iSetLoraParaResultReq.Locker_Address		=	0x0000;
				iSetLoraParaResultReq.Last_Work_Type		=	set_lora_param_cmd;
				iSetLoraParaResultReq.Locker_Status			=	0x00;
		    iSetLoraParaResultReq.Locker_ACK				=	set_lora_param_ok_return;
		    Get_CurBCDTime7((char*)l_arrBcdtime);
		    iSetLoraParaResultReq.Report_Time.str		=	(char*)l_arrBcdtime;
		    iSetLoraParaResultReq.Report_Time.len		=	sizeof(l_arrBcdtime);   
				iSetLoraParaResultReq.Read_context.data	=	ptr;		      
				iSetLoraParaResultReq.Read_context.len	=	LORA_PARA_LEN;
			//-	-	-	-	-	update lora parament -	-	-	-	-	
				pLoraProperty		=	(LoraProperty*)( ptr + 1) ;
				pUartPort->FreqValue											=	pLoraProperty->frequency[0];
				pUartPort->FreqValue											=	pUartPort->FreqValue<<8;
				pUartPort->FreqValue											|=	pLoraProperty->frequency[1];
				pUartPort->FreqValue											=	pUartPort->FreqValue<<8;
				pUartPort->FreqValue											|=	pLoraProperty->frequency[2];
				pUartPort->AirRate												=	pLoraProperty->AirRate;
        pUartPort->GateWayAddress									=	pLoraProperty->GateWayAddress;
				#ifdef Debug_Level
          printf("%s, pUartPort->FreqValue = 0x%02x \n", pUartPort->h_io, pUartPort->FreqValue);
          printf("%s, pUartPort->AirRate = 0x%02x \n",pUartPort->h_io, pUartPort->AirRate);
          printf("%s, pUartPort->GateWayAddress = 0x%02x \n",pUartPort->h_io, htons(pUartPort->GateWayAddress));
        #endif
				
			  #ifdef		Debug_Level
							printf("%s,line:%d,[ %s ]  read lora parament okay \n",__FILE__,__LINE__,pUartPort->h_io);
				#endif
			}

      set_lora_result_request_pack((void*)&iSetLoraParaResultReq,s_buffer,&s_length);
		  res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
			if((res <= 0)||(res != s_length)){
					printf("%s,line=%d, s_length=%d, socket error!!\n",__FILE__,__LINE__,res);
				}
			pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  config channel
	    printf("%s,line:%d,Remote set lora response success\n",__FILE__,__LINE__);
	    pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
			return	1;
		}
}


uint8_t uart_get_lora_prm(void	*argu)
{
		struct	GetLoraFrame mGetLoraFrame={
			  .megicCode	=	{0xff,0x56,0xae,0x35,0xa9,0x55,0xf0},
			  .address			=	02,
			  .length				=	14
			};
    uint32_t 			res;
		UartPort			*pUartPort		= (UartPort*)argu;
    uint8_t				u_buffer[128],u_length;
    LoraProperty			*pLoraProperty;
    uint8_t		databuf[sizeof(LoraProperty)+1],idataLen;		//because property non cmd byte,so add 1 bytes
    //SetLoraParamReq		iSetLoraParamReq;

    while( (pUartPort->deviceStates != UartIDLE) ){
				//printf("%s,line:%d, [ %s ] uart states busying...\n",__FILE__,__LINE__,pUartPort->h_io);
		usleep(10000);
		}
		
		//------------ pack -> send -> wait -> unpack -> ack ---------------------    
     memcpy( u_buffer,&mGetLoraFrame,sizeof(struct GetLoraFrame) );
     u_length	=	sizeof(struct  GetLoraFrame) ;

      #if (Debug_Level == 1)
			printf("uart packet data len=%d\n",u_length);
		  for(int16_t i=0;i<u_length;i++)
		     printf(" %02x",u_buffer[i]);
		  printf("\n\r%s,line:%d \n",__FILE__,__LINE__);
		  #endif
		 uint8_t sendCnt	= 0 ;
     select_lora_states(&pUartPort->channelNumber,1,1);
			do{
					pUartPort->deviceStates	=	UartSendBusying; 
					pUartPort->pipe		=	PIPE_LORA_REQUEST;
			    res = write (pUartPort->clt_uart,u_buffer,u_length);
			    if((res <= 0)||(res != u_length))
					{
						while(1){
			        printf("%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
			        //printf("ERROR:%s",stderr);
							sleep(3);
						}
					}
					memset(databuf,0,sizeof(databuf));
          if(uart_read_wait_time(pUartPort->clt_uart, 2) == 0)
					{
						pUartPort->pipe	=	PIPE_RECE_TIMEOUT;
					}else{
		      idataLen = read(pUartPort->clt_uart,databuf,sizeof(databuf));
          if ( idataLen <= UART_FRAME_LENGTH){
              for(uint8_t i = 0;i<idataLen;i++)
                printf("%02x ",databuf[i]);
              printf("\r\n%s,%d , %s read len=%d bytes short!!\n",__FILE__,__LINE__,pUartPort->h_io,idataLen);  
							continue;
						}
						//-----------	analysis uart lora frame	---------------			
				 	#if Debug_Level	== 1
         	for(uint8_t i = 0;i<idataLen;i++)
            printf("%02x ",databuf[i]);
          printf("\r\n %s,%d, %s receive len=%d bytes \n",__FILE__,__LINE__,pUartPort->h_io,idataLen);  
          #endif
          pUartPort->pipe	=	PIPE_RECE_SUCCESS;
					}
					sendCnt	+=	1;
					#ifdef		Debug_Level
          printf("%s,line:%d,[%s] send read lora command %d times \n",__FILE__,__LINE__,pUartPort->h_io,sendCnt);
					#endif
			}while((pUartPort->pipe != PIPE_RECE_SUCCESS)&&(sendCnt	<	3));
			//---- unpacket lora	Response frame header is 0x24 --------------
      select_lora_states(&pUartPort->channelNumber,0,0);

      if ((sendCnt > 3)||( databuf[0]	!=	0x24 ) ){
			  #ifdef		Debug_Level
					printf("%s,line:%d,[ %s ]  get lora parament failure!! \n",__FILE__,__LINE__,pUartPort->h_io);
				#endif
				return	1;
			}else{
				pLoraProperty		=	(LoraProperty*)&databuf[1];
				
				pUartPort->FreqValue											=	pLoraProperty->frequency[0];
				pUartPort->FreqValue											=	pUartPort->FreqValue<<8;
				pUartPort->FreqValue											|=	pLoraProperty->frequency[1];
				pUartPort->FreqValue											=	pUartPort->FreqValue<<8;
				pUartPort->FreqValue											|=	pLoraProperty->frequency[2];
				pUartPort->AirRate												=	pLoraProperty->AirRate;
        pUartPort->GateWayAddress									=	pLoraProperty->GateWayAddress;
				#ifdef Debug_Level
          printf("%s, pUartPort->FreqValue = 0x%02x \n", pUartPort->h_io, pUartPort->FreqValue);
          printf("%s, pUartPort->AirRate = 0x%02x \n",pUartPort->h_io, pUartPort->AirRate);
          printf("%s, pUartPort->GateWayAddress = 0x%02x \n",pUartPort->h_io, htons(pUartPort->GateWayAddress));
        #endif

			  #ifdef		Debug_Level
					printf("%s,line:%d,[ %s ]  get lora parament okay \n",__FILE__,__LINE__,pUartPort->h_io);
				#endif
				return	0;
			}
}



