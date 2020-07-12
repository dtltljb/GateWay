/*** 
* 1) Receive signal
* 2) analysis protocol command,
*	 control command & uart Up commmand response
* 3) distribute task
* 4) control command:
*	Remote open/close door,Emergency open door,Remote down load information,Remote Delete information.
* 5) uart Up commmand response:
*	on_line_service\register_locker.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <errno.h>
#include	<signal.h>
#include <pthread.h>

#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/un.h>  
#include <sys/sem.h>
#include <arpa/inet.h>  
#include <netinet/in.h> 


//#include <semaphore.h>

#include 			"socket_protocol_thread.h"
#include 			"socket/Unpacket.h"
#include 			"socket/VirtualPortMonitor.h"
#include 			"socket/AppLayer/keep_alive_handle.h"

#include 			"protoBuf/epb_keep_alive_pack.h"
#include 			"protoBuf/epb_emergency_open_pack.h"
#include 			"protoBuf/epb_report_log_pack.h"
#include 			"protoBuf/epb_remote_open_pack.h"
#include 			"protoBuf/epb_remote_load_pack.h"
#include 			"protoBuf/epb_remote_authorize_pack.h"
#include 			"protoBuf/epb_remote_config_pack.h"
#include 			"protoBuf/epb_remote_get_config_pack.h"
#include 			"protoBuf/epb_set_lora_param_pack.h"

#include 			"uart/UartPacket.h"
#include			"uart/xxx_channel_send_thread.h"
#include 			"uart/socket_task_distributer.h"
#include 			"uart/Cmd/emergency_open_pack.h"
#include 			"uart/Cmd/report_log_pack.h"
#include 			"uart/Cmd/remote_open_pack.h"

#include 			"uart/LinkLayer/uart_listener_thread.h"
#include 			"uart/LinkLayer/UartOpration.h"
#include            "uart/LinkLayer/lora_parament.h"
#include 			"AppLayer/AysnchronousWork.h"
#include 			"bdb/FileManager.h"
#include 			"util/pthread_data_copy.h"

#define				_GNU_SOURCE					//compile evn

//	------------- public ----------------------

//sem_t sem_first_channel_WaitSend,sem_second_channel_WaitSend,sem_third_channel_WaitSend,sem_fourth_channel_WaitSend,sem_config_channel_WaitSend;	
ProtocolTimeFormat		mProtocolTimeFormat;

uint8_t					socketReceiveBuf[1050];
uint16_t				socketReceiveLength;
//	-------------- private ---------------------
static		uint8_t					socketSendBuf[1050];
static		uint16_t				socketSendLength;
//static		uint8_t					uartSendBuf[255],mUartSendLength;
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};
//static  int sem_id = 0;
//static  int semaphore_p();
//static  int semaphore_v();
//static  int set_semvalue();
//static  void del_semvalue();

pthread_mutex_t lora_prm_mutex_lock;

phandle *pFirst_handle,*pSecond_handle,*pThird_handle,*pFourth_handle,*pConfig_handle;


void* socket_protocol_thread(void *pvoid)
{

	uint32_t	debug_failure = 0,debug_success_times = 0 ; //debug use
  uint32_t	res;
	NSFPFixHead *fix_head;
//----------- initalize	para	----------------	
  struct VirtualSocket					*pVirtualSocket			=	&mVirtualSocket;
  void *StuctPtr;
  ReportLogRespPublic			iReportLogRespPublic;

	memset(&mUartPort,0,sizeof(UartPort));
  init_lora_channel_config();
  
//	updata_channel_prm_to_file();
//	socket_data_unpack_create();

  phandle *pFirst_handle  =   (phandle *)&VirtualSocketCopyer[0];
  pthread_t 	Handle_first_channel_WaitSend;
  res = pthread_create(&Handle_first_channel_WaitSend,NULL,first_channel_WaitSend,(void*)pFirst_handle);
	if (res ){
		printf("name:%s\n,line:%d\n first failure!! ",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}
	phandle *pSecond_handle  =   (phandle *)&VirtualSocketCopyer[1];
	pthread_t 	Handle_second_channel_WaitSend;
    res = pthread_create(&Handle_second_channel_WaitSend,NULL,second_channel_WaitSend,(void*)pSecond_handle);
	if (res ){
		printf("name:%s\n,line:%d\n first failure!! ",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}

	phandle *pThird_handle  =   (phandle *)&VirtualSocketCopyer[2];
	pthread_t 	Handle_three_channel_WaitSend;
    res = pthread_create(&Handle_three_channel_WaitSend,NULL,third_channel_WaitSend,(void*)pThird_handle);
	if (res ){
		printf("name:%s\n,line:%d\n first failure!! ",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}
	phandle *pFourth_handle  =   (phandle *)&VirtualSocketCopyer[3];
	pthread_t 	Handle_four_channel_WaitSend;
  res = pthread_create(&Handle_four_channel_WaitSend,NULL,fourth_channel_WaitSend,(void*)pFourth_handle);
	if (res ){
		printf("name:%s\n,line:%d\n first failure!! ",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}
	
	phandle *pConfig_handle  =   (phandle *)&VirtualSocketCopyer[4];
	pthread_t 	Handle_config_channel_WaitSend;
  res = pthread_create(&Handle_config_channel_WaitSend,NULL,config_channel_WaitSend,(void*)pConfig_handle);
	if (res ){
		printf("name:%s\n,line:%d\n first failure!! ",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
	}
	//create lora prm lock
    pthread_mutex_init(&lora_prm_mutex_lock,PTHREAD_MUTEX_TIMED_NP);
	
//    //create	semaphore_socket_receive_id
//    sem_id = semget((key_t)semaphore_socket_unpack_id, 1, 0666 | IPC_CREAT);
//    //   set semaphore
//    if(!set_semvalue()){
//                 fprintf(stderr, "Failed to initialize semaphore_socket_unpack_id\n");
//                 exit(EXIT_FAILURE);
//    }
    
    if ( pvoid == NULL )
        printf("%s,line:%d socket_protocol_thread run, disable pvoid warning \n",__FILE__,__LINE__);

	while(1)
	{
		res		=	0;
		if(sem_wait(&sem_socket_receiver_id)==-1)
			perror(" wait semaphore failure");
		else
		{
				int rc	=1;
				while(rc){
					rc	=	pthread_mutex_trylock(&socket_receive_mutex_lock);
					usleep(10000);
				}
				res = socket_data_unpack_func(pVirtualSocket,socketReceiveBuf,&socketReceiveLength);
		    pthread_mutex_unlock(&socket_receive_mutex_lock);

			#ifdef		Debug_Level
      printf("%s,line:%d ,socketReceiveLength=%d res=%d\n",__FILE__,__LINE__,socketReceiveLength,res);
			#endif
     if((res != (uint32_t)SINGLE_RECEIVE_SUCCESS)&&(res	!=	(uint32_t)MULTI_RECEIVE_SUCCESS)){			//receive frame code anliys
        if(res != SOCKET_WAIT_RECEIVING ){
	     		debug_failure =	debug_failure + 1 ;
	     		printf("\n%s,line=%d ,receive exception frame data, throw away receive data...\n",__FILE__,__LINE__);
	     					
	     	}
     		else
         	printf("\n%s,line=%d ,receive multi-frame data waiting receive other data\n",__FILE__,__LINE__);
			}else{
				debug_success_times = debug_success_times + 1;
				printf("\n%s,line:%d, receive length=%d, receive total %d times \n ",__FILE__,__LINE__,socketReceiveLength, debug_success_times);
				
				if(res == (uint32_t)SINGLE_RECEIVE_SUCCESS)
		      fix_head = (NSFPFixHead*)socketReceiveBuf;
		    else if( (res == (uint32_t)MULTI_RECEIVE_SUCCESS)&&(debug_failure == 0) ){
        	fix_head = (NSFPFixHead*)&pVirtualSocket->last_fix_header;
        }else{
        	printf("\n%s,line:%d,neither SINGLE_RECEIVE_SUCCESS nor MULTI_RECEIVE_SUCCESS, throw away..continue \n ",__FILE__,__LINE__);
        	continue;
        }
        debug_failure	=	0 ;																		//clear	failure flag
        pVirtualSocket->keep_alive_times = pVirtualSocket->base_time_count;		//update count times
        
				switch (fix_head->WorkType)
				{
					//----------------------------------------------
          case keep_alive_cmd:
					{//send keep-alive after over timer reconnect server
							uint8_t	*ptr;
							uint16_t	len;
							char*			iMsg = "keep alive ";
							BaseResponse	iBaseResponse;
              iBaseResponse.err_msg.str				=	iMsg;
              iBaseResponse.err_msg.len				=	strlen(iMsg);
							mKeepAliveResp.base_response		=	&iBaseResponse;							
              KeepAliveResp	*pKeepAliveResp			= &mKeepAliveResp;
						#ifdef		Debug_Level
                printf("%s,line:%d,keep_alive_cmd WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
							ptr		=	pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
							len		=	pVirtualSocket->SingleUnpackLength;
			        pKeepAliveResp =	epb_unpack_keepAlive_response(pKeepAliveResp, ptr, len);
			        if(pKeepAliveResp != NULL )
			           pVirtualSocket->keep_alive_response = RESPONSE;
			        if((pKeepAliveResp->keep_alive_time > 30)&&(pKeepAliveResp->keep_alive_time <= 180 ) ){
			        	mVirtualSocket.keep_alive	=	pKeepAliveResp->keep_alive_time;
 /*
								 //how long to run the first time
                tv.it_value.tv_sec = (struct itimerval)mVirtualSocket.keep_alive;
							  tv.it_value.tv_usec = 0;
							   //after the first time, how long to run next time
                tv.it_interval.tv_sec = (struct itimerval)mVirtualSocket.keep_alive;		//heart
							  tv.it_interval.tv_usec = 0;
							  if (setitimer(ITIMER_REAL, &tv, &otv) != 0) 
									printf("setitimer signal err %d\n", errno);
 */
			        }
			        res	=	keep_alive_cmd_resp_return;
			        break;
							}
					//----------------------------------------------
          case report_log_cmd:
					{
						#ifdef		Debug_Level
               printf("%s,line:%d,report_log_cmd WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
						char LogMsg[255];
						memset(LogMsg,0,sizeof(LogMsg));
						uint8_t	*ptr;
						uint16_t	len;
						BaseResponse	iBaseResponse;
						
            iBaseResponse.err_msg.str				=	LogMsg;
            iBaseResponse.err_msg.len				=	strlen(LogMsg);
						iReportLogRespPublic.base_response	=	&iBaseResponse;

						ptr		=	pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
						len		=	pVirtualSocket->SingleUnpackLength;
						StuctPtr =	epb_report_log_response_public_unpack(&iReportLogRespPublic, ptr, len);
						if(StuctPtr ==	NULL)
							break;
            if (iReportLogRespPublic.Last_Work_Type	!= report_log_cmd){
            	pVirtualSocket->workType		=	report_log_cmd;
//			      	pVirtualSocket->pContext		=	&iReportLogRespPublic;
              pVirtualSocket->pContext.p		=	(void*)&iReportLogRespPublic;
              pVirtualSocket->pContext.len	=	sizeof(iReportLogRespPublic);			      	
							AysnchronousWorkProcessFuction((void*)pVirtualSocket);
							break;
						}

						//-------- unpack common log ,send_correspond_semaphore_trigle_uart -------------
						ReportLogResp		iReportLogResp,*pReportLogResp;
						ptr		=	pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
						len		=	pVirtualSocket->SingleUnpackLength;
						iReportLogResp.base_response			=	&iBaseResponse;

            pReportLogResp =	epb_report_log_response_unpack(&iReportLogResp, ptr, len);
            if(pReportLogResp	==NULL)
            	break;
						if( (Log_Type)pReportLogResp->iLog_Type	== RegisterStatus ){		//Register Status reponse,non done
							#ifdef		Debug_Level
							printf( "locker register Status response, do not continue...");
							#endif
							break;	
						}
						if( (Log_Type)pReportLogResp->iLog_Type	== HistoryLog ){		//HistoryLog command output status
							mVirtualSocket.history_log_response		=	RESPONSE;					//output	response 	status
							#ifdef		Debug_Level
							printf( "History log response	, continue report next record...");
							#endif
							break;	
						}
		        uint8_t	chn	=	pReportLogResp->Channel_number;						
            if((chn < 1)||(chn > 5)){
            	printf("%s,line=%d channel-%d-number over rang...\n",__FILE__,__LINE__,pReportLogResp->Channel_number);
			      	res	=	report_log_cmd_resp_return_err;
			      	break;
			      }
			      else
              #ifdef		Debug_Level
              printf("%s,line:%d,chn=%d \n",__FILE__,__LINE__,pReportLogResp->Channel_number);
              #endif
        			pVirtualSocket->channel			=	chn;
              pVirtualSocket->workType		=	report_log_cmd;
//              pVirtualSocket->pContext.p		=	(void*)&iReportLogResp;
//              pVirtualSocket->pContext.len	=	sizeof(iReportLogResp);
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&iReportLogResp;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(iReportLogResp);
              send_correspond_semphore_number(chn,pVirtualSocket);
			      	res	=	report_log_cmd_resp_return;
			      	break;
					}
					//----------------------------------------------
					case	set_lora_param_cmd:
					{
						#ifdef		Debug_Level
               printf("%s,line:%d,set_lora_param_cmd WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
            BaseRequest         iBaseRequest = {NULL};
						SetLoraParamReq        *pSetLoraParamReq, iSetLoraParamReq;		
            iSetLoraParamReq.base_request   =   &iBaseRequest;
						uint8_t			*ptr    =   pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
						uint16_t		len			=		pVirtualSocket->SingleUnpackLength;  

		        pSetLoraParamReq	=	set_lora_param_unpack_Req(&iSetLoraParamReq,ptr, len);
						if	(pSetLoraParamReq ==NULL){
								#if		Debug_Level == 1
                   printf("%s,line=%d , pSetLoraParamReq is NULL!!\n",__FILE__,__LINE__);
								#endif
								/* unpack Response_ACK	fail */
								res	=	set_lora_param_cmd_return;
								break;
							}
						/* pack Response_ACK	OKAY */
            char*				iMsg = "set lora receive okay";
						BaseResponse			 iBaseResponse;
						SetLoraParamResp		imSetLoraParamResp;
						iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen(iMsg);
            imSetLoraParamResp.base_response	=	&iBaseResponse;
            //imSetLoraParamResp.set_States   =	SetLoraParamRespCode;
            if((pSetLoraParamReq->Channel_number < 1)||(pSetLoraParamReq->Channel_number > 5)){
               imSetLoraParamResp.set_States		=		RequestError;
					  }else{
                imSetLoraParamResp.set_States		=		mUartPort[pSetLoraParamReq->Channel_number-1].WorkStates;		//uart idle
              }
            
           // imSetLoraParamResp.set_States   =	set_lora_param_cmd_return;
            
            set_lora_param_response_pack((void*)&imSetLoraParamResp,socketSendBuf,&socketSendLength);
		        res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
						if((res <= 0)||(res != socketSendLength)){
              printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
                    //close(pVirtualSocket->clt_sock);
							res	=	set_lora_param_ok_return;
							break;
                 }
                 
 /*                
						if((pSetLoraParamReq->Channel_number < 1)||(pSetLoraParamReq->Channel_number > 5)){
			      	  printf("%s,line=%d channel %d number over rang...\n",__FILE__,__LINE__,pSetLoraParamReq->Channel_number);
			      	  res	=	set_lora_param_cmd_return;
			      		break;
			      	}
			     	if(mUartPort[pSetLoraParamReq->Channel_number-1].WorkStates	==	WorkStatesBusy){
			      	  printf("%s,line=%d channel %d number WorkStatesBusy...\n",__FILE__,__LINE__,pSetLoraParamReq->Channel_number);
			      	  res	=	set_lora_param_cmd_return;
			      		break;
			      	}
*/
			      	
						//--------	send_correspond_semaphore_trigle_uart -------------
			      	mUartPort[pSetLoraParamReq->Channel_number-1].WorkStates	=	WorkStatesBusy;						
              uint8_t	chn	=	pSetLoraParamReq->Channel_number;
            //  chn	=	5;			//debug config channel
              pVirtualSocket->channel			=	chn;
			      	pVirtualSocket->workType		=	set_lora_param_cmd;
//              pVirtualSocket->pContext.p		=	(void*)&iSetLoraParamReq;
//              pVirtualSocket->pContext.len	=	sizeof(iSetLoraParamReq);
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&iSetLoraParamReq;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(iSetLoraParamReq);

			      	send_correspond_semphore_number(chn,pVirtualSocket);
			      	res	= set_lora_param_cmd_return;
						break;
					}
				//----------------------------------------------
					case emergency_open_door:
					{
            BaseRequest         iBaseRequest = {NULL};
						EmergencyOpenReq        *pEmergencyOpenReq, iEmergencyOpenReq;
            iEmergencyOpenReq.base_request  =   &iBaseRequest;
						uint8_t			*ptr    =   pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
						uint16_t		len			=		pVirtualSocket->SingleUnpackLength;  
						
		        pEmergencyOpenReq	=	epb_emergency_open_unpack_Req(&iEmergencyOpenReq,ptr, len);
						if(pEmergencyOpenReq ==NULL){
                 #if		Debug_Level == 1
                   printf("%s,line=%d , pEmergencyOpenReq= NULL!!\n",__FILE__,__LINE__);
								#endif
								/* unpack Response_ACK	fail */
								res	=	emergency_open_door_return;
								break;
							}
						/* pack Response_ACK	OKAY */
						char*				iMsg = "emergency";
						BaseResponse			 iBaseResponse;
            EmergencyOpenResp       iEmergencyOpenResp;
            iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen(iMsg);
            iEmergencyOpenResp.base_response	=	&iBaseResponse;
            iEmergencyOpenResp.Locker_Address	=	0xFFFF;
            
            iEmergencyOpenResp.Response_ACK   =	 pVirtualSocket->EmergencyStateLock ;

            emergency_open_response_pack((void*)&iEmergencyOpenResp,socketSendBuf,&socketSendLength);
		        res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
/*						non response so that  cancel
						if((res <= 0)||(res != socketSendLength)){
              printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
                    //close(pVirtualSocket->clt_sock);
							res	=	emergency_open_door_return;
							break;
			     }
						if( pVirtualSocket->EmergencyStateLock == WorkStatesBusy){
								printf("%s,line=%d Emergency open door Work States Busy...\n",__FILE__,__LINE__);
			      	  res	=	emergency_open_door_return;
			      		break;
						}
*/
						
						/*		mul pthread send uart emergency open			*/
            pVirtualSocket->workType		=	emergency_open_door;
//            pVirtualSocket->pContext.p		=	(void*)&iEmergencyOpenReq;
//            pVirtualSocket->pContext.len	=	sizeof(EmergencyOpenReq);
						if( VirtualSocketCopyer[0].EmergencyStateLock != WorkStatesBusy ){				//Work States Busy..
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[0],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[0].pContext.p		=	(void*)&iEmergencyOpenReq;
					    VirtualSocketCopyer[0].pContext.len	=	sizeof(iEmergencyOpenReq);
            	sem_post(&sem_first_channel_WaitSend);
          		}
          	if( VirtualSocketCopyer[1].EmergencyStateLock != WorkStatesBusy ){
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[1].pContext.p		=	(void*)&iEmergencyOpenReq;
					    VirtualSocketCopyer[1].pContext.len	=	sizeof(iEmergencyOpenReq);            
	            usleep(1000);
	            sem_post(&sem_second_channel_WaitSend);
	          	}
	          if( VirtualSocketCopyer[2].EmergencyStateLock != WorkStatesBusy ){	
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[2],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[2].pContext.p		=	(void*)&iEmergencyOpenReq;
					    VirtualSocketCopyer[2].pContext.len	=	sizeof(iEmergencyOpenReq);            
	            usleep(1000);
	            sem_post(&sem_third_channel_WaitSend);
	          	}
	          if( VirtualSocketCopyer[3].EmergencyStateLock != WorkStatesBusy ){	
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[3],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[3].pContext.p		=	(void*)&iEmergencyOpenReq;
					    VirtualSocketCopyer[3].pContext.len	=	sizeof(iEmergencyOpenReq);            
	            usleep(1000);
	            sem_post(&sem_fourth_channel_WaitSend);
	          	}
	          	
		        break;
			   }
			   //----------------------------------------------
					case remote_open_door:
					{
            RemoteOpenReq				*pRemoteOpenReq, iRemoteOpenReq;
            BaseRequest         iBaseRequest = {NULL};
            iRemoteOpenReq.base_request   =   &iBaseRequest;
            uint8_t			*ptr    =   pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
						uint16_t		len			=		pVirtualSocket->SingleUnpackLength;            
            pRemoteOpenReq 			=	epb_remote_open_request_unpack(&iRemoteOpenReq, ptr, len);
            if(pRemoteOpenReq ==	NULL){
              res	=	remote_open_door_return;
							break;
						}
						#ifdef		Debug_Level
              printf("%s,line:%d,remote_open_door WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
						//-------- pack socket remote open response	----------
						char*				iMsg = "remote open";            
						BaseResponse				iBaseResponse;
            RemoteOpenResp              iRemoteOpenResp;
            iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen(iMsg);						
            iRemoteOpenResp.base_response	=	&iBaseResponse;

					  if((pRemoteOpenReq->Channel_number < 1)||(pRemoteOpenReq->Channel_number > 5)){
               iRemoteOpenResp.Response_ACK		=		RequestError;
					  }else{
                iRemoteOpenResp.Response_ACK		=		mUartPort[pRemoteOpenReq->Channel_number-1].WorkStates;		//uart idle
              }
            //ptr		=		(uint8_t*)	socketSendBuf;
            iRemoteOpenResp.Locker_Address	=		pRemoteOpenReq->Locker_Address;
            remote_open_response_pack(&iRemoteOpenResp,socketSendBuf,&socketSendLength);
		        res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
						if((res <= 0)||(res != socketSendLength)){
              printf("%s,line=%d, socketSendLength=%d, send error !!! \n",__FILE__,__LINE__,res);
              pVirtualSocket->socketStates		=	socket_send_data_err;
              res	=	remote_open_door_return;
							break;
			     	}
						if((pRemoteOpenReq->Channel_number < 1)||(pRemoteOpenReq->Channel_number > 5)){
			      	  printf("%s,line=%d channel-%d-number over rang...\n",__FILE__,__LINE__,pRemoteOpenReq->Channel_number);
			      	  res	=	remote_open_door_return;
			      		break;
			      	}
			     	if(mUartPort[pRemoteOpenReq->Channel_number-1].WorkStates	==	WorkStatesBusy){
			      	  printf("%s,line=%d channel-%d-number WorkStatesBusy...\n",__FILE__,__LINE__,pRemoteOpenReq->Channel_number);
			      	  res	=	remote_open_door_return;
			      		break;
			      	}
			      	mUartPort[pRemoteOpenReq->Channel_number-1].WorkStates	= WorkStatesBusy;
						//--------	send_correspond_semaphore_trigle_uart -------------
            	uint8_t	chn	=	pRemoteOpenReq->Channel_number;
			      	pVirtualSocket->channel			=	chn;
			      	pVirtualSocket->workType		=	remote_open_door;
//	            pVirtualSocket->pContext.p		=	(void*)&iRemoteOpenReq;
//	            pVirtualSocket->pContext.len	=	sizeof(iRemoteOpenReq);  	
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&iRemoteOpenReq;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(iRemoteOpenReq);	            

			      	send_correspond_semphore_number(chn,pVirtualSocket);
			      	res	= remote_open_door_return;
			       break;
						}
					//----------------------------------------------
					case remote_authorize_cmd:
					{
						RemoteAutorizeReq				*pRemoteAutorizeReq, iRemoteAutorizeReq;
            BaseRequest         iBaseRequest = {NULL};
            iRemoteAutorizeReq.base_request =   &iBaseRequest;
            uint8_t		*ptr     	=   pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
            uint16_t	 len			=		pVirtualSocket->SingleUnpackLength;
            pRemoteAutorizeReq	=		epb_remote_authorzie_request_unpack(&iRemoteAutorizeReq, ptr, len);
            if(pRemoteAutorizeReq ==	NULL){
              res	=	remote_authorize_cmd_return;
							break;
						}
						#ifdef		Debug_Level
               printf("%s,line:%d,remote_authorize_cmd WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
						//-------- pack socket remote open response	----------
            char			*iMsg = "authorize cmd receive ok";
						BaseResponse				iBaseResponse;
            RemoteAuthorizeResp         iRemoteAuthorizeResp;
						iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen((char*) iMsg);
            iRemoteAuthorizeResp.base_response	=	&iBaseResponse;

					  if((pRemoteAutorizeReq->Channel_number < 1)||(pRemoteAutorizeReq->Channel_number > 5))
                iRemoteAuthorizeResp.Response_ACK		=		RequestError;
					  else
                iRemoteAuthorizeResp.Response_ACK		=		mUartPort[pRemoteAutorizeReq->Channel_number-1].WorkStates;		//uart idle

						iRemoteAuthorizeResp.Locker_Address		=	pRemoteAutorizeReq->Locker_Address;

            remote_authorize_response_pack(&iRemoteAuthorizeResp,socketSendBuf,&socketSendLength);								//NOTE:&ptr  mode !=  &socketSendBuf mode  !!!!!!
            res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
            if((res <= 0)||(res != socketSendLength)){
              printf("%s,line=%d, socketSendLength=%d, sock error \n",__FILE__,__LINE__,res);
              pVirtualSocket->socketStates		=	socket_send_data_err;
              res	=	remote_authorize_cmd_return;
							break;
			     	}
						if((pRemoteAutorizeReq->Channel_number < 1)||(pRemoteAutorizeReq->Channel_number > 5)){
			      	printf("%s,line=%d config cmd set channel %d number over rang...\n",__FILE__,__LINE__,pRemoteAutorizeReq->Channel_number);
              res	=	remote_config_cmd_return;
			      	break;
			      	}
			     	if(mUartPort[pRemoteAutorizeReq->Channel_number-1].WorkStates	==	WorkStatesBusy){
			      	  printf("%s,line=%d channel %d work states busy,throw away data ...\n",__FILE__,__LINE__,pRemoteAutorizeReq->Channel_number);
			      	  res	=	remote_open_door_return;
			      		break;
			      	}
              mUartPort[pRemoteAutorizeReq->Channel_number-1].WorkStates	= WorkStatesBusy;
						//--------	send_correspond_semaphore_trigle_uart -------------
            	uint8_t	chn	=	pRemoteAutorizeReq->Channel_number;
			      	pVirtualSocket->channel			=	chn;
			      	pVirtualSocket->workType		=	remote_authorize_cmd;
//	            pVirtualSocket->pContext.p		=	(void*)&iRemoteAutorizeReq;
//	            pVirtualSocket->pContext.len	=	sizeof(iRemoteAutorizeReq);  
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&iRemoteAutorizeReq;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(iRemoteAutorizeReq);	  

			      	send_correspond_semphore_number(chn,pVirtualSocket);
			      	res	= remote_authorize_cmd_return;
              break;
              }
					//---------------------------------------------		
					case remote_load_cmd:
					{
            RemoteLoadReq				*pRemoteLoadReq, iRemoteLoadReq;
            BaseRequest         iBaseRequest = {NULL};
            iRemoteLoadReq.base_request  =   &iBaseRequest;
            uint8_t			*ptr    =   pVirtualSocket->MultUnpackBuf+FRAME_DATA_AREA;
						uint16_t		len			=		pVirtualSocket->MultUnpackLength;
            pRemoteLoadReq	=	epb_remote_load_request_unpack(&iRemoteLoadReq, ptr, len);
            if(pRemoteLoadReq ==	NULL){
              res	=	remote_load_cmd_return;
							break;
						}
						#ifdef		Debug_Level
            printf("%s,line:%d,remote_load_cmd WorkIndex=0x%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
						//-------- pack socket remote open response	----------
            char				*iMsg= "receive mul pack okay";
						BaseResponse				iBaseResponse;
            RemoteLoadResp              iRemoteLoadResp;
						iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen((char*) iMsg);						
            iRemoteLoadResp.base_response	=	&iBaseResponse;

					  if((pRemoteLoadReq->Channel_number < 1)||(pRemoteLoadReq->Channel_number > 5))
                 iRemoteLoadResp.Response_ACK		=		RequestError;
					  else
                iRemoteLoadResp.Response_ACK		=		mUartPort[ConfigUartNum].WorkStates;		//config channel states
            //ptr	=(uint8_t*)socketSendBuf;
            memset((char*)&iRemoteLoadResp,0,sizeof(iRemoteLoadResp));
            remote_load_response_pack(&iRemoteLoadResp,socketSendBuf,&socketSendLength);								//NOTE:&ptr  mode !=  &socketSendBuf mode  !!!!!!
		        res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
						if((res <= 0)||(res != socketSendLength)){
              printf("%s,line=%d, socketSendLength=%d, send error !!! \n",__FILE__,__LINE__,res);
							pVirtualSocket->socketStates		=	socket_send_data_err;              
              res	=	remote_open_door_return;
							break;
			     	}
					  if((pRemoteLoadReq->Channel_number < 1)||(pRemoteLoadReq->Channel_number > 5)){
              printf("%s,line=%d channel-%d-number over rang...\n",__FILE__,__LINE__,pRemoteLoadReq->Channel_number);
              res	=	remote_config_cmd_return;
			      	break;
			      	}
            if(mUartPort[ConfigUartNum].WorkStates	==	WorkStatesBusy){				//config channel states
			      	  printf("%s,line=%d channel-%d-number WorkStatesBusy...\n",__FILE__,__LINE__,pRemoteLoadReq->Channel_number);
			      	  res	=	remote_open_door_return;
			      		break;
			      	}
						//--config channel idle send_correspond_semaphore-------------
              mUartPort[ConfigUartNum].WorkStates	=	WorkStatesBusy	;						//lock config channel states
              memset( (char*)&mRemoteLoadReq,0,sizeof(mRemoteLoadReq));
              uint32_t l = sizeof(RemoteLoadReq);
              pthread_data_copy((char*)&mRemoteLoadReq, (char*)pRemoteLoadReq, (int*)&l);														//note:finger context copy

            	uint8_t	chn	=	pRemoteLoadReq->Channel_number;
			      	pVirtualSocket->channel			=	chn;
			      	pVirtualSocket->workType		=	remote_load_cmd;
				      mRemoteLoadReq.finger_context.data	=	mRemoteLoadReq.finger_data;
				      mRemoteLoadReq.finger_context.len	=	pRemoteLoadReq->finger_context.len;
//	            pVirtualSocket->pContext.p		=	(void*)&mRemoteLoadReq;
//	            pVirtualSocket->pContext.len	=	sizeof(mRemoteLoadReq);
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&mRemoteLoadReq;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(mRemoteLoadReq);	 

			      	send_correspond_semphore_number(chn,pVirtualSocket);
			      	res	= remote_load_cmd_return;
							break;
						}
					//---------------------------------------------		
					case remote_config_cmd:
					{
						RemoteConfigReq				*pRemoteConfigReq, iRemoteConfigReq;
            BaseRequest         iBaseRequest = {NULL};
            iRemoteConfigReq.base_request  =&iBaseRequest;
            uint8_t		*ptr     	=   pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
            uint16_t	 len			=		pVirtualSocket->SingleUnpackLength;
            pRemoteConfigReq	=		epb_remote_config_request_unpack(&iRemoteConfigReq, ptr, len);

            if(pRemoteConfigReq ==	NULL){
              res	=	remote_config_cmd_return;
							break;
						}
						#ifdef		Debug_Level
            printf("%s,line:%d,remote_config_cmd WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
						//-------- pack socket remote open response	----------
            char				*iMsg="remote config receive ok";
						BaseResponse				iBaseResponse;
            RemoteConfigResp            iRemoteConfigResp;
						iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen((char*) iMsg);						
						iRemoteConfigResp.base_response	=	&iBaseResponse;
					  if((pRemoteConfigReq->Channel_number < 1)||(pRemoteConfigReq->Channel_number > 5))
               iRemoteConfigResp.Response_ACK		=		RequestError;
					  else
               iRemoteConfigResp.Response_ACK		=		mUartPort[pRemoteConfigReq->Channel_number-1].WorkStates;		//uart idle
						iRemoteConfigResp.Locker_Address		=	pRemoteConfigReq->Locker_Address;
						
            remote_config_response_pack(&iRemoteConfigResp,socketSendBuf,&socketSendLength);								//NOTE:&ptr  mode !=  &socketSendBuf mode  !!!!!!
            res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
						if((res <= 0)||(res != socketSendLength)){
              printf("%s,line=%d, socketSendLength=%d, socket error !!! \n",__FILE__,__LINE__,res);
							pVirtualSocket->socketStates		=	socket_send_data_err;
              res	=	remote_open_door_return;
							break;
			     	}
						if((pRemoteConfigReq->Channel_number < 1)||(pRemoteConfigReq->Channel_number > 5)){
			      	printf("%s,line=%d, channel %d number over rang...\n",__FILE__,__LINE__, pRemoteConfigReq->Channel_number);
              res	=	remote_config_cmd_return;
			      	break;
			      	}
            if(mUartPort[pRemoteConfigReq->Channel_number-1].WorkStates	==	WorkStatesBusy){
			      	  printf("%s,line=%d, channel %d number WorkStatesBusy...\n",__FILE__,__LINE__, pRemoteConfigReq->Channel_number);
			      	  res	=	remote_open_door_return;
			      		break;
			      	}
            mUartPort[pRemoteConfigReq->Channel_number-1].WorkStates	= WorkStatesBusy;
						//--------	send_correspond_semaphore_trigle_uart -------------
            uint8_t	chn	=	pRemoteConfigReq->Channel_number;
            pVirtualSocket->channel			=	chn;
            pVirtualSocket->workType		=	remote_config_cmd;
//	          pVirtualSocket->pContext.p		=	(void*)&iRemoteConfigReq;
//	          pVirtualSocket->pContext.len	=	sizeof(iRemoteConfigReq);			      
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&iRemoteConfigReq;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(iRemoteConfigReq);	

            send_correspond_semphore_number(chn,pVirtualSocket);
			      res	= remote_config_cmd_return;
						break;
						}
//---------------------------------------------		
					case remote_get_config_cmd:
					{
						RemoteGetConfigReq				*pRemoteGetConfigReq, iRemoteGetConfigReq;
            BaseRequest         iBaseRequest = {NULL};
            iRemoteGetConfigReq.base_request  =       &iBaseRequest;
            uint8_t		*ptr     	=   pVirtualSocket->SingleUnpackBuf+FRAME_DATA_AREA;
            uint16_t	 len			=		pVirtualSocket->SingleUnpackLength;
            pRemoteGetConfigReq	=		epb_remote_get_config_request_unpack(&iRemoteGetConfigReq, ptr, len);

            if(pRemoteGetConfigReq ==	NULL){
              res	=	remote_get_config_cmd_return;
							break;
						}
						#ifdef		Debug_Level
                        printf("%s,line:%d,remote_get_config_cmd WorkIndex=%02x \n",__FILE__,__LINE__,fix_head->WorkIndex);
						#endif
						//-------- pack socket remote get config response	----------
            char				*iMsg="remote get config receive ok";
						BaseResponse				iBaseResponse;
            RemoteGetConfigResp            iRemoteGetConfigResp;
						iBaseResponse.err_msg.str				=	(char*)iMsg;
            iBaseResponse.err_msg.len				=	strlen((char*) iMsg);						
            iRemoteGetConfigResp.base_response	=	&iBaseResponse;
					  if((pRemoteGetConfigReq->Channel_number < 1)||(pRemoteGetConfigReq->Channel_number > 5))
              iRemoteGetConfigResp.Response_ACK		=		RequestError;
					  else
              iRemoteGetConfigResp.Response_ACK		=		mUartPort[pRemoteGetConfigReq->Channel_number-1].WorkStates;		//uart idle

						iRemoteGetConfigResp.Locker_Address		=	pRemoteGetConfigReq->Locker_Address;
            remote_get_config_response_pack(&iRemoteGetConfigResp,socketSendBuf,&socketSendLength);								//NOTE:&ptr  mode !=  &socketSendBuf mode  !!!!!!
                res = send(pVirtualSocket->clt_sock,socketSendBuf,socketSendLength, 0 );
						if((res <= 0)||(res != socketSendLength))
						{
              printf("%s,line=%d, socketSendLength=%d, socket error !!! \n",__FILE__,__LINE__,res);
							pVirtualSocket->socketStates		=	socket_send_data_err;
              res	=	remote_open_door_return;
							break;
			     	}
						if((pRemoteGetConfigReq->Channel_number < 1)||(pRemoteGetConfigReq->Channel_number > 5))
			      {
			      	printf("%s,line=%d channel-%d-number over rang...\n",__FILE__,__LINE__,pRemoteGetConfigReq->Channel_number);
              res	=	remote_get_config_cmd_return;
			      	break;
			      	}
            if(mUartPort[pRemoteGetConfigReq->Channel_number-1].WorkStates	==	WorkStatesBusy)
			      {
                printf("%s,line=%d channel %d number WorkStatesBusy...\n",__FILE__,__LINE__,pRemoteGetConfigReq->Channel_number);
			      	  res	=	remote_get_config_cmd_return;
			      		break;
			      	}
			      mUartPort[pRemoteGetConfigReq->Channel_number-1].WorkStates	= WorkStatesBusy;	
						//--------	send_correspond_semaphore_trigle_uart -------------
            uint8_t	chn	=	pRemoteGetConfigReq->Channel_number;
            pVirtualSocket->channel			=	chn;
            pVirtualSocket->workType		=	remote_get_config_cmd;
//	          pVirtualSocket->pContext.p		=	(void*)&iRemoteGetConfigReq;
//	          pVirtualSocket->pContext.len	=	sizeof(iRemoteGetConfigReq);	   
              res    = sizeof(struct VirtualSocket);
					    pthread_data_copy((char*)&VirtualSocketCopyer[chn-1],(char*)pVirtualSocket,(int*)&res);
					    VirtualSocketCopyer[chn-1].pContext.p		=	(void*)&iRemoteGetConfigReq;
					    VirtualSocketCopyer[chn-1].pContext.len	=	sizeof(iRemoteGetConfigReq);

            send_correspond_semphore_number(chn,pVirtualSocket);
			      res	= remote_config_cmd_return;
						break;
						}
					default:
						printf("%s,line=%d receive work type=%d rang error,throw away...\n",__FILE__,__LINE__,fix_head->WorkType);
						res	=	work_type_rang_error;
						break ;
					}
			}
		}
	}
	pthread_mutex_destroy(&lora_prm_mutex_lock);
}



//------------			semaphore		------------------
/*
 static  int set_semvalue()
{
    
    union semun sem_union;

    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return 1;
}

 static  void del_semvalue()
{
    
    union semun sem_union;

    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

 static  int semaphore_p()
{
    
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1;//P()
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_p failed\n");
        return 0;
    }
    return 1;
}

 static int semaphore_v()
{
    
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;//V()
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_v failed\n");
        return 0;
    }
    return 1;
}

*/
