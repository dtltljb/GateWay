/*
* first 	init  env  para
* second 	log in server,get token
* third		Read socket signal post
* fourth    judge socket status
* fiveth    destroy socket、token、signal、relink server
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#include      "xxx_channel_send_thread.h"

#include 			"socket/socket_protocol_thread.h"
#include 			"socket/VirtualPortMonitor.h"
#include 			"socket/protoBuf/epb_keep_alive_pack.h"
#include 			"socket/protoBuf/epb_emergency_open_pack.h"
#include 			"socket/protoBuf/epb_report_log_pack.h"
#include 			"socket/protoBuf/epb_remote_open_pack.h"
#include 			"socket/protoBuf/epb_remote_load_pack.h"
#include 			"socket/protoBuf/epb_remote_authorize_pack.h"
#include 			"socket/protoBuf/epb_remote_config_pack.h"
#include 			"socket/protoBuf/epb_remote_get_config_pack.h"
#include			"socket/protoBuf/epb_set_lora_param_pack.h"
#include 			"uart/UartPacket.h"
#include 			"uart/UartUnpacket.h"
#include 			"uart/socket_task_distributer.h"
#include 			"uart/LinkLayer/uart_listener_thread.h"
#include      "uart/LinkLayer/lora_parament.h"

#include 			"uart/Cmd/emergency_open_pack.h"
#include 			"uart/Cmd/report_log_pack.h"
#include 			"uart/Cmd/remote_open_pack.h"
#include 			"uart/Cmd/remote_load_pack.h"
#include 			"uart/Cmd/remote_config_pack.h"
#include 			"uart/Cmd/remote_authorize_pack.h"
#include 			"uart/Cmd/u_set_lora_para_pack.h"

#include       "bdb/FileManager.h"
#include       "bdb/logManager.h"
#include       "util/pthread_data_copy.h"

UartPort		mUartPort[5];

/*
*		config_channel_WaitSend
******************************************************************/
void* config_channel_WaitSend(void	*pvoid)
{

 	UartPort*		pUartPort				=	&mUartPort[ConfigUartNum];
 // struct 	VirtualSocket					*pVirtualSocket			=	&mVirtualSocket;
 // struct VirtualSocket *pVirtualSocket	=	(struct VirtualSocket*)pvoid;
  struct VirtualSocket       	*pVirtualSocket, iVirtualSocket; 		/* receive signel pass context */
	if ( pvoid == NULL )
        printf("%s,line:%d config_channel_WaitSend disable pvoid warning run\n",__FILE__,__LINE__);

  int    res;
	pthread_t 	Handle_config_channel_Listener;
	res = pthread_create(&Handle_config_channel_Listener,NULL,config_channel_Listener,NULL);
	if (res ){
		fprintf(stderr,"%s,line:%d,  chn %s signal create failure \n",__FILE__,__LINE__,pUartPort->h_io);
		//exit(EXIT_FAILURE);
	}
	uint8_t WorkCmd = 0;
	
	while(1)
	{
		
		if(sem_wait(&sem_config_channel_WaitSend)==-1)
			perror("sem_config_channel wait semaphore failure");
		else
		{
			#if	Debug_Level == 1
			fprintf(stderr,"%s,line:%d,  %s channel recevie signal\n",__FILE__,__LINE__,pUartPort->h_io);
			#endif
			if ( pUartPort->deviceStates	!=	UartIDLE ){
				fprintf(stderr,"%s,line:%d,%s non Initialize\n",__FILE__,__LINE__,pUartPort->h_io);
				continue;
			}

//		printf("%s,line:%d, mVirtualSocket.channel=%02x , mVirtualSocket.sourceChannel=%02x , pVirtualSocket->workType=%x,  \n",__FILE__,__LINE__,mVirtualSocket.channel, mVirtualSocket.sourceChannel,mVirtualSocket.workType );
		printf("%s,line:%d,iVirtualSocket.channel=%02x,iVirtualSocket.sourceChannel=%02x \n",__FILE__,__LINE__,iVirtualSocket.channel,iVirtualSocket.sourceChannel);
		res = sizeof(struct VirtualSocket);
    pthread_data_copy((char*)&iVirtualSocket, (char*)&VirtualSocketCopyer[ConfigUartNum],(int*)&res);
    pVirtualSocket			=	&iVirtualSocket;		//debug test mVirtualSocket
    printf("%s,line:%d, VirtualSocketCopyer.channel=%02x , VirtualSocketCopyer.sourceChannel=%02x , VirtualSocketCopyer.workType=%x,\n",__FILE__,__LINE__,\
    									VirtualSocketCopyer[ConfigUartNum].channel, VirtualSocketCopyer[ConfigUartNum].sourceChannel,VirtualSocketCopyer[ConfigUartNum].workType);

      WorkCmd		=			pVirtualSocket->workType;
      pVirtualSocket->workType	=	0;  										//clear flag
			switch(WorkCmd)
			{
        case		report_log_cmd:
        { // todo : add register log record 
         ReportLogResp		iReportLogResp, *pReportLogResp;
         res = sizeof(iReportLogResp);
         pthread_data_copy((char*)&iReportLogResp, (char*)pVirtualSocket->pContext.p,&res);
         pReportLogResp	=	&iReportLogResp;
         iReportLogResp.random.data = iReportLogResp.entry_random;  												// pointer to entry
         pVirtualSocket->pContext.p = (char*)&iReportLogResp;

         if( (enum LockerStates)pReportLogResp->iLockerStates		!=  LOCK_REGISTER_SUCC ){
						printf("%s,line:%d,locker register unanthorize,Locker SN = [%d] response!!\n",__FILE__,__LINE__,iReportLogResp.Locker_Address);
						break;
					}
          //ret	=	register_log_pack(pVirtualSocket,pUartPort);
					res	=	register_online_log_pack(pVirtualSocket,pUartPort);          
					//printf("%s,line:%d,report_log_cmd %d response!!\n",__FILE__,__LINE__,WorkCmd);
					}
           break;
        case		remote_load_cmd:
        {
            mURemoteLoadReq.cmd		=		RemoteLoadFinger;
	          res	=	uart_remote_load_pack(pVirtualSocket,pUartPort);			//globle prm 
            }
						break;
				case		remote_config_cmd:
				{
					RemoteConfigReq		iRemoteConfigReq;
	        res = sizeof(RemoteConfigReq);
	        pthread_data_copy((char*)&iRemoteConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteConfigReq;
          iRemoteConfigReq.first_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_1;
          iRemoteConfigReq.second_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_2;
          iRemoteConfigReq.third_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_3;
					
					res	=	uart_request_remote_config_pack(pVirtualSocket,pUartPort);
				}
					break;
        case		set_lora_param_cmd:
        	{
        	SetLoraParamReq				iSetLoraParamReq;
	        res = sizeof(SetLoraParamReq);
	        pthread_data_copy((char*)&iSetLoraParamReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iSetLoraParamReq;	
            res	=	uart_set_lora_para_pack(pVirtualSocket,pUartPort);
          }
            break;
        case		remote_get_config_cmd:
        {
        	RemoteGetConfigReq		iRemoteGetConfigReq;
	        res = sizeof(RemoteGetConfigReq);
	        pthread_data_copy((char*)&iRemoteGetConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteGetConfigReq;
	        
            res	=	uart_request_remote_get_config_pack(pVirtualSocket,pUartPort);
          }
            break;
				default:
						 fprintf(stderr,"%s,line:%d,config channel do not WorkCmd %d ERROR!!\n",__FILE__,__LINE__,WorkCmd);
						 break;	
				}
			}
	}
}

/*
*		 first_channel_WaitSend
******************************************************************/

void* first_channel_WaitSend(void	*pvoid)
{
	UartPort 							*pUartPort					=	&mUartPort[FirstUartNum];		//must to thread number
  //struct VirtualSocket					*pVirtualSocket			=	&mVirtualSocket;
  //struct VirtualSocket *pVirtualSocket	=	(struct VirtualSocket*)pvoid;
	struct VirtualSocket       	*pVirtualSocket, iVirtualSocket; 		/* receive signel pass context */
	if ( pvoid == NULL )
        printf("%s,line:%d first_channel_WaitSend disable pvoid warning run\n",__FILE__,__LINE__);
  uint8_t     WorkCmd;
	int res;
	pthread_t 	Handle_first_channel_Listener;
	res = pthread_create(&Handle_first_channel_Listener,NULL,first_channel_Listener,NULL);
	if (res ){
    fprintf(stderr,"%s,line:%d,  chn %s signal create failure",__FILE__,__LINE__,pUartPort->h_io);
		exit(EXIT_FAILURE);
	}
	
	while(1)
	{
    if(sem_wait(&sem_first_channel_WaitSend)==-1)
			perror(" wait semaphore failure");
		else
		{
			#if	Debug_Level == 1
			fprintf(stderr,"%s,line:%d,   %s recevie signal\n",__FILE__,__LINE__,pUartPort->h_io);
			#endif

			if ( pUartPort->deviceStates	!=	UartIDLE ){
				fprintf(stderr,"%s,line:%d,%s non Initialize\n",__FILE__,__LINE__,pUartPort->h_io);
				continue;
			}

		//get copy paper
 		//printf("%s,line:%d,mVirtualSocket.channel=%02x , mVirtualSocket.sourceChannel=%02x  \n",__FILE__,__LINE__,mVirtualSocket.channel, mVirtualSocket.sourceChannel);
 		printf("%s,line:%d,iVirtualSocket.channel=%02x,iVirtualSocket.sourceChannel=%02x \n",__FILE__,__LINE__,iVirtualSocket.channel,iVirtualSocket.sourceChannel);
		res = sizeof(struct VirtualSocket);
    pthread_data_copy((char*)&iVirtualSocket, (char*)&VirtualSocketCopyer[FirstUartNum],(int*)&res);
    pVirtualSocket			=	&iVirtualSocket;		//debug test mVirtualSocket
      printf("%s,line:%d, VirtualSocketCopyer.channel=%02x , VirtualSocketCopyer.sourceChannel=%02x , VirtualSocketCopyer.workType=%x,\n",__FILE__,__LINE__,\
    									VirtualSocketCopyer[FirstUartNum].channel, VirtualSocketCopyer[FirstUartNum].sourceChannel,VirtualSocketCopyer[FirstUartNum].workType);

			WorkCmd		=			pVirtualSocket->workType;
			pVirtualSocket->workType	=	0;  //clear flag
			switch(WorkCmd)
			{
        case		report_log_cmd:
         {
//                UReportLogResp  iUReportLogResp;
         ReportLogResp		iReportLogResp, *pReportLogResp;
         res = sizeof(iReportLogResp);
         pthread_data_copy((char*)&iReportLogResp, (char*)pVirtualSocket->pContext.p,&res);
         pReportLogResp	=	&iReportLogResp;
         iReportLogResp.random.data = iReportLogResp.entry_random;  												// pointer to entry
         pVirtualSocket->pContext.p = (char*)&iReportLogResp;

         if( pReportLogResp->iLockerStates		!=  (int)OnLineMode ){
						app_run_log_evnt_context(NULL,pUartPort);
						break;
					}
          res	=	report_online_log_pack(pVirtualSocket,pUartPort);
            }
					break;
					
				case		emergency_open_door:
					{
					 EmergencyOpenReq				iEmergencyOpenReq;
	         res = sizeof(EmergencyOpenReq);
	         pthread_data_copy((char*)&iEmergencyOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iEmergencyOpenReq;

	         res	=	uart_emergency_control_pack((void*)pVirtualSocket,(void*)pUartPort);
        }
					break;
					
				case		remote_open_door:
					{
					 RemoteOpenReq				iRemoteOpenReq;
	         res = sizeof(RemoteOpenReq);
	         pthread_data_copy((char*)&iRemoteOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iRemoteOpenReq;						
					res	=	uart_request_remote_open_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_authorize_cmd:
				{
					RemoteAutorizeReq		iRemoteAutorizeReq;
	        res = sizeof(RemoteAutorizeReq);
	        pthread_data_copy((char*)&iRemoteAutorizeReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteAutorizeReq;
			    iRemoteAutorizeReq.iUser_authorize.Data_context_1.data		=	iRemoteAutorizeReq.m_data_context_1;
					iRemoteAutorizeReq.iUser_authorize.Data_context_2.data		=	iRemoteAutorizeReq.m_data_context_2;
					iRemoteAutorizeReq.iUser_authorize.Data_context_3.data		=	iRemoteAutorizeReq.m_data_context_3;
					iRemoteAutorizeReq.iUser_authorize.Data_context_4.data		=	iRemoteAutorizeReq.m_data_context_4;
					
					res	=	uart_request_remote_authorize_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_load_cmd:
				{
					
          res	=	uart_Change_Channel_pack(pVirtualSocket,pUartPort);
        }
					break;
					
				case		remote_config_cmd:
				{
					RemoteConfigReq		iRemoteConfigReq;
	        res = sizeof(RemoteConfigReq);
	        pthread_data_copy((char*)&iRemoteConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteConfigReq;
          iRemoteConfigReq.first_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_1;
          iRemoteConfigReq.second_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_2;
          iRemoteConfigReq.third_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_3;

					res	=	uart_request_remote_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		set_lora_param_cmd:
					{
					SetLoraParamReq				iSetLoraParamReq;
	        res = sizeof(SetLoraParamReq);
	        pthread_data_copy((char*)&iSetLoraParamReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iSetLoraParamReq;			
					res	=	uart_set_lora_para_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		remote_get_config_cmd:
				{
					RemoteGetConfigReq		iRemoteGetConfigReq;
	        res = sizeof(RemoteGetConfigReq);
	        pthread_data_copy((char*)&iRemoteGetConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteGetConfigReq;
	        
					res	=	uart_request_remote_get_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				default:
					 fprintf(stderr,"%s,line:%d,%s channel_WorkCmd %d ERROR!!\n",__FILE__,__LINE__,pUartPort->h_io,WorkCmd);
					 break;	
			} 

    } //end else
    
	}
	
}


/*
*		second_channel_WaitSend
******************************************************************/
void* second_channel_WaitSend(void	*pvoid)
{
	UartPort 							*pUartPort					=	&mUartPort[SecondUartNum];		//must to array number
	    //struct VirtualSocket					*pVirtualSocket			=	&mVirtualSocket;
    //struct VirtualSocket *pVirtualSocket	=	(struct VirtualSocket*)pvoid;	
	struct VirtualSocket       	*pVirtualSocket, iVirtualSocket; 		/* receive signel pass context */
	if ( pvoid == NULL )
         printf("%s,line:%d second_channel_WaitSend disable pvoid warning run\n",__FILE__,__LINE__);
  int	res;
	pthread_t 	Handle_second_channel_Listener;
	res = pthread_create(&Handle_second_channel_Listener,NULL,second_channel_Listener,NULL);
	if (res ){
		fprintf(stderr,"%s,line:%d,  chn %s signal create failure",__FILE__,__LINE__,pUartPort->h_io);
		exit(EXIT_FAILURE);
	}

  uint8_t     WorkCmd;

	while(1)
	{
    if(sem_wait(&sem_second_channel_WaitSend)==-1)
			perror(" wait semaphore failure");
		else
				{
			#if	Debug_Level == 1
			fprintf(stderr,"%s,line:%d,   %s recevie signal\n",__FILE__,__LINE__,pUartPort->h_io);
			#endif
			if ( pUartPort->deviceStates	!=	UartIDLE ){
				fprintf(stderr,"%s,line:%d,%s non Initialize\n",__FILE__,__LINE__,pUartPort->h_io);
				continue;
			}
		//get copy paper
 		//printf("%s,line:%d,mVirtualSocket.channel=%02x , mVirtualSocket.sourceChannel=%02x  \n",__FILE__,__LINE__,mVirtualSocket.channel, mVirtualSocket.sourceChannel);
 		printf("%s,line:%d,iVirtualSocket.channel=%02x,iVirtualSocket.sourceChannel=%02x \n",__FILE__,__LINE__,iVirtualSocket.channel,iVirtualSocket.sourceChannel);
 		
		res = sizeof(struct VirtualSocket);
    pthread_data_copy((char*)&iVirtualSocket, (char*)&VirtualSocketCopyer[SecondUartNum],(int*)&res);
    pVirtualSocket			=	&iVirtualSocket;		//debug test mVirtualSocket
    
    printf("%s,line:%d, VirtualSocketCopyer.channel=%02x , VirtualSocketCopyer.sourceChannel=%02x , VirtualSocketCopyer.workType=%x,\n",__FILE__,__LINE__,\
    									VirtualSocketCopyer[SecondUartNum].channel, VirtualSocketCopyer[SecondUartNum].sourceChannel,VirtualSocketCopyer[SecondUartNum].workType);


			WorkCmd		=			pVirtualSocket->workType;
			pVirtualSocket->workType	=	0;  //clear flag
			switch(WorkCmd)
			{
        case		report_log_cmd:
         {
//                UReportLogResp  iUReportLogResp;
         ReportLogResp		iReportLogResp, *pReportLogResp;
         res = sizeof(iReportLogResp);
         pthread_data_copy((char*)&iReportLogResp, (char*)pVirtualSocket->pContext.p,&res);
         pReportLogResp	=	&iReportLogResp;
         iReportLogResp.random.data = iReportLogResp.entry_random;  												// pointer to entry
         pVirtualSocket->pContext.p = (char*)&iReportLogResp;

         if( pReportLogResp->iLockerStates		!=  (int)OnLineMode ){
						app_run_log_evnt_context(NULL,pUartPort);
						break;
					}
          res	=	report_online_log_pack(pVirtualSocket,pUartPort);
            }
					break;
					
				case		emergency_open_door:
					{
					 EmergencyOpenReq				iEmergencyOpenReq;
	         res = sizeof(EmergencyOpenReq);
	         pthread_data_copy((char*)&iEmergencyOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iEmergencyOpenReq;
	         
	         res	=	uart_emergency_control_pack((void*)pVirtualSocket,(void*)pUartPort);
        }
					break;
					
				case		remote_open_door:
					{
					 RemoteOpenReq				iRemoteOpenReq;
	         res = sizeof(RemoteOpenReq);
	         pthread_data_copy((char*)&iRemoteOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iRemoteOpenReq;						
					res	=	uart_request_remote_open_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_authorize_cmd:
				{
					RemoteAutorizeReq		iRemoteAutorizeReq;
	        res = sizeof(RemoteAutorizeReq);
	        pthread_data_copy((char*)&iRemoteAutorizeReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteAutorizeReq;
			    iRemoteAutorizeReq.iUser_authorize.Data_context_1.data		=	iRemoteAutorizeReq.m_data_context_1;
					iRemoteAutorizeReq.iUser_authorize.Data_context_2.data		=	iRemoteAutorizeReq.m_data_context_2;
					iRemoteAutorizeReq.iUser_authorize.Data_context_3.data		=	iRemoteAutorizeReq.m_data_context_3;
					iRemoteAutorizeReq.iUser_authorize.Data_context_4.data		=	iRemoteAutorizeReq.m_data_context_4;
					
					res	=	uart_request_remote_authorize_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_load_cmd:
				{
					
          res	=	uart_Change_Channel_pack(pVirtualSocket,pUartPort);
        }
					break;
					
				case		remote_config_cmd:
				{
					RemoteConfigReq		iRemoteConfigReq;
	        res = sizeof(RemoteConfigReq);
	        pthread_data_copy((char*)&iRemoteConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteConfigReq;
          iRemoteConfigReq.first_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_1;
          iRemoteConfigReq.second_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_2;
          iRemoteConfigReq.third_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_3;

					res	=	uart_request_remote_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		set_lora_param_cmd:
					{
					SetLoraParamReq				iSetLoraParamReq;
	        res = sizeof(SetLoraParamReq);
	        pthread_data_copy((char*)&iSetLoraParamReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iSetLoraParamReq;			
					res	=	uart_set_lora_para_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		remote_get_config_cmd:
				{
					RemoteGetConfigReq		iRemoteGetConfigReq;
	        res = sizeof(RemoteGetConfigReq);
	        pthread_data_copy((char*)&iRemoteGetConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteGetConfigReq;
	        
					res	=	uart_request_remote_get_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				default:
					 fprintf(stderr,"%s,line:%d,%s channel_WorkCmd %d ERROR!!\n",__FILE__,__LINE__,pUartPort->h_io,WorkCmd);
					 break;	
			} 

    } //end else
    
	}
}

/*
*		third_channel_WaitSend
******************************************************************/
void* third_channel_WaitSend(void	*pvoid)
{
	UartPort 							*pUartPort					=	&mUartPort[ThirdUartNum];		//must to thread number
	int res;
	pthread_t 	Handle_third_channel_Listener;
	res = pthread_create(&Handle_third_channel_Listener,NULL,third_channel_Listener,NULL);
	if (res ){
		fprintf(stderr,"%s,line:%d,  chn %s signal create failure\n",__FILE__,__LINE__,pUartPort->h_io);
		exit(EXIT_FAILURE);
	}
 // struct VirtualSocket					*pVirtualSocket			=	&mVirtualSocket;
 // struct VirtualSocket *pVirtualSocket	=	(struct VirtualSocket*)pvoid;
	struct VirtualSocket       	*pVirtualSocket, iVirtualSocket; 		/* receive signel pass context */
	    if ( pvoid == NULL )
        printf("%s,line:%d third_channel_WaitSend disable pvoid warning run\n",__FILE__,__LINE__);
        	
  uint8_t     WorkCmd;

	while(1)
	{
    if(sem_wait(&sem_third_channel_WaitSend)==-1)
			perror(" wait semaphore failure");
		else
				{
			#if	Debug_Level == 1
			fprintf(stderr,"%s,line:%d,   %s recevie signal\n",__FILE__,__LINE__,pUartPort->h_io);
			#endif
			if ( pUartPort->deviceStates	!=	UartIDLE ){
				fprintf(stderr,"%s,line:%d,%s non Initialize\n",__FILE__,__LINE__,pUartPort->h_io);
				continue;
			}
		//get copy paper
 		//printf("%s,line:%d,mVirtualSocket.channel=%02x , mVirtualSocket.sourceChannel=%02x  \n",__FILE__,__LINE__,mVirtualSocket.channel, mVirtualSocket.sourceChannel);
 		printf("%s,line:%d,iVirtualSocket.channel=%02x,iVirtualSocket.sourceChannel=%02x \n",__FILE__,__LINE__,iVirtualSocket.channel,iVirtualSocket.sourceChannel);
		res = sizeof(struct VirtualSocket);
    pthread_data_copy((char*)&iVirtualSocket, (char*)&VirtualSocketCopyer[ThirdUartNum],(int*)&res);
    pVirtualSocket			=	&iVirtualSocket;		//debug test mVirtualSocket
    
     printf("%s,line:%d, VirtualSocketCopyer.channel=%02x , VirtualSocketCopyer.sourceChannel=%02x , VirtualSocketCopyer.workType=%x,\n",__FILE__,__LINE__,\
    									VirtualSocketCopyer[ThirdUartNum].channel, VirtualSocketCopyer[ThirdUartNum].sourceChannel,VirtualSocketCopyer[ThirdUartNum].workType);


			WorkCmd		=			pVirtualSocket->workType;
			pVirtualSocket->workType	=	0;  //clear flag
			switch(WorkCmd)
			{
        case		report_log_cmd:
         {
//                UReportLogResp  iUReportLogResp;
         ReportLogResp		iReportLogResp, *pReportLogResp;
         res = sizeof(iReportLogResp);
         pthread_data_copy((char*)&iReportLogResp, (char*)pVirtualSocket->pContext.p,&res);
         pReportLogResp	=	&iReportLogResp;
         iReportLogResp.random.data = iReportLogResp.entry_random;  												// pointer to entry
         pVirtualSocket->pContext.p = (char*)&iReportLogResp;

         if( pReportLogResp->iLockerStates		!=  (int)OnLineMode ){
						app_run_log_evnt_context(NULL,pUartPort);
						break;
					}
          res	=	report_online_log_pack(pVirtualSocket,pUartPort);
            }
					break;
					
				case		emergency_open_door:
					{
					 EmergencyOpenReq				iEmergencyOpenReq;
	         res = sizeof(EmergencyOpenReq);
	         pthread_data_copy((char*)&iEmergencyOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iEmergencyOpenReq;
	         res	=	uart_emergency_control_pack((void*)pVirtualSocket,(void*)pUartPort);
        }
					break;
					
				case		remote_open_door:
					{
					 RemoteOpenReq				iRemoteOpenReq;
	         res = sizeof(RemoteOpenReq);
	         pthread_data_copy((char*)&iRemoteOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iRemoteOpenReq;						
					res	=	uart_request_remote_open_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_authorize_cmd:
				{
					RemoteAutorizeReq		iRemoteAutorizeReq;
	        res = sizeof(RemoteAutorizeReq);
	        pthread_data_copy((char*)&iRemoteAutorizeReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteAutorizeReq;
			    iRemoteAutorizeReq.iUser_authorize.Data_context_1.data		=	iRemoteAutorizeReq.m_data_context_1;
					iRemoteAutorizeReq.iUser_authorize.Data_context_2.data		=	iRemoteAutorizeReq.m_data_context_2;
					iRemoteAutorizeReq.iUser_authorize.Data_context_3.data		=	iRemoteAutorizeReq.m_data_context_3;
					iRemoteAutorizeReq.iUser_authorize.Data_context_4.data		=	iRemoteAutorizeReq.m_data_context_4;
					
					res	=	uart_request_remote_authorize_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_load_cmd:
				{
					
          res	=	uart_Change_Channel_pack(pVirtualSocket,pUartPort);
        }
					break;
					
				case		remote_config_cmd:
				{
					RemoteConfigReq		iRemoteConfigReq;
	        res = sizeof(RemoteConfigReq);
	        pthread_data_copy((char*)&iRemoteConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteConfigReq;
          iRemoteConfigReq.first_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_1;
          iRemoteConfigReq.second_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_2;
          iRemoteConfigReq.third_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_3;

					res	=	uart_request_remote_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		set_lora_param_cmd:
					{
					SetLoraParamReq				iSetLoraParamReq;
	        res = sizeof(SetLoraParamReq);
	        pthread_data_copy((char*)&iSetLoraParamReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iSetLoraParamReq;			
					res	=	uart_set_lora_para_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		remote_get_config_cmd:
				{
					RemoteGetConfigReq		iRemoteGetConfigReq;
	        res = sizeof(RemoteGetConfigReq);
	        pthread_data_copy((char*)&iRemoteGetConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteGetConfigReq;
	        
					res	=	uart_request_remote_get_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				default:
					 fprintf(stderr,"%s,line:%d,%s channel_WorkCmd %d ERROR!!\n",__FILE__,__LINE__,pUartPort->h_io,WorkCmd);
					 break;	
			} 

    } //end else
	}
	
}

/*
*		fourth_channel_WaitSend
******************************************************************/
void* fourth_channel_WaitSend(void	*pvoid)
{
	UartPort 							*pUartPort					=	&mUartPort[FourthUartNum];		//must to thread number
	int res;
	pthread_t 	Handle_fourth_channel_Listener;
	res = pthread_create(&Handle_fourth_channel_Listener,NULL,fourth_channel_Listener,NULL);
	if (res ){
		fprintf(stderr,"%s,line:%d,  chn %s signal create failure\n",__FILE__,__LINE__,pUartPort->h_io);
		exit(EXIT_FAILURE);
	}
 // struct VirtualSocket					*pVirtualSocket			=	&mVirtualSocket;
//  struct VirtualSocket *pVirtualSocket	=	(struct VirtualSocket*)pvoid;
	struct VirtualSocket       	*pVirtualSocket, iVirtualSocket; 		/* receive signel pass context */
	    if ( pvoid == NULL )
        printf("%s,line:%d fourth_channel_WaitSend disable pvoid warning run\n",__FILE__,__LINE__);
        
  uint8_t     WorkCmd;

	while(1)
	{
    if(sem_wait(&sem_fourth_channel_WaitSend)==-1)
			perror(" wait semaphore failure");
		else
				{
			#if	Debug_Level == 1
			fprintf(stderr,"%s,line:%d,   %s recevie signal\n",__FILE__,__LINE__,pUartPort->h_io);
			#endif
			if ( pUartPort->deviceStates	!=	UartIDLE ){
				fprintf(stderr,"%s,line:%d,%s non Initialize\n",__FILE__,__LINE__,pUartPort->h_io);
				continue;
			}
		//get copy paper
 		//printf("%s,line:%d,mVirtualSocket.channel=%02x , mVirtualSocket.sourceChannel=%02x  \n",__FILE__,__LINE__,mVirtualSocket.channel, mVirtualSocket.sourceChannel);
 		printf("%s,line:%d,iVirtualSocket.channel=%02x,iVirtualSocket.sourceChannel=%02x \n",__FILE__,__LINE__,iVirtualSocket.channel,iVirtualSocket.sourceChannel);
 		
		res = sizeof(struct VirtualSocket);
    pthread_data_copy((char*)&iVirtualSocket, (char*)&VirtualSocketCopyer[FourthUartNum],(int*)&res);
    pVirtualSocket			=	&iVirtualSocket;		//debug test mVirtualSocket
    
		 printf("%s,line:%d, VirtualSocketCopyer.channel=%02x , VirtualSocketCopyer.sourceChannel=%02x , VirtualSocketCopyer.workType=%x,\n",__FILE__,__LINE__,\
		    									VirtualSocketCopyer[FourthUartNum].channel, VirtualSocketCopyer[FourthUartNum].sourceChannel,VirtualSocketCopyer[FourthUartNum].workType);


			WorkCmd		=			pVirtualSocket->workType;
			pVirtualSocket->workType	=	0;  //clear flag
			switch(WorkCmd)
			{
        case		report_log_cmd:
         {
//                UReportLogResp  iUReportLogResp;
         ReportLogResp		iReportLogResp, *pReportLogResp;
         res = sizeof(iReportLogResp);
         pthread_data_copy((char*)&iReportLogResp, (char*)pVirtualSocket->pContext.p,&res);
         pReportLogResp	=	&iReportLogResp;
         iReportLogResp.random.data = iReportLogResp.entry_random;  												// pointer to entry
         pVirtualSocket->pContext.p = (char*)&iReportLogResp;

         if( pReportLogResp->iLockerStates		!=  (int)OnLineMode ){
						app_run_log_evnt_context(NULL,pUartPort);
						break;
					}
          res	=	report_online_log_pack(pVirtualSocket,pUartPort);
            }
					break;
					
				case		emergency_open_door:
					{
					 EmergencyOpenReq				iEmergencyOpenReq;
	         res = sizeof(EmergencyOpenReq);
	         pthread_data_copy((char*)&iEmergencyOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iEmergencyOpenReq;
	         
	         res	=	uart_emergency_control_pack((void*)pVirtualSocket,(void*)pUartPort);
        }
					break;
					
				case		remote_open_door:
					{
					 RemoteOpenReq				iRemoteOpenReq;
	         res = sizeof(RemoteOpenReq);
	         pthread_data_copy((char*)&iRemoteOpenReq, (char*)pVirtualSocket->pContext.p,&res);
	         pVirtualSocket->pContext.p = (char*)&iRemoteOpenReq;						
					res	=	uart_request_remote_open_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_authorize_cmd:
				{
					RemoteAutorizeReq		iRemoteAutorizeReq;
	        res = sizeof(RemoteAutorizeReq);
	        pthread_data_copy((char*)&iRemoteAutorizeReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteAutorizeReq;
			    iRemoteAutorizeReq.iUser_authorize.Data_context_1.data		=	iRemoteAutorizeReq.m_data_context_1;
					iRemoteAutorizeReq.iUser_authorize.Data_context_2.data		=	iRemoteAutorizeReq.m_data_context_2;
					iRemoteAutorizeReq.iUser_authorize.Data_context_3.data		=	iRemoteAutorizeReq.m_data_context_3;
					iRemoteAutorizeReq.iUser_authorize.Data_context_4.data		=	iRemoteAutorizeReq.m_data_context_4;
					
					res	=	uart_request_remote_authorize_pack(pVirtualSocket,pUartPort);
				}
					break;
					
				case		remote_load_cmd:
				{
					
          res	=	uart_Change_Channel_pack(pVirtualSocket,pUartPort);
        }
					break;
					
				case		remote_config_cmd:
				{
					RemoteConfigReq		iRemoteConfigReq;
	        res = sizeof(RemoteConfigReq);
	        pthread_data_copy((char*)&iRemoteConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteConfigReq;
          iRemoteConfigReq.first_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_1;
          iRemoteConfigReq.second_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_2;
          iRemoteConfigReq.third_data_group.data		=	(uint8_t*)iRemoteConfigReq.data_group_3;

					res	=	uart_request_remote_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		set_lora_param_cmd:
					{
					SetLoraParamReq				iSetLoraParamReq;
	        res = sizeof(SetLoraParamReq);
	        pthread_data_copy((char*)&iSetLoraParamReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iSetLoraParamReq;			
					res	=	uart_set_lora_para_pack(pVirtualSocket,pUartPort);
				}
					break;
				case		remote_get_config_cmd:
				{
					RemoteGetConfigReq		iRemoteGetConfigReq;
	        res = sizeof(RemoteGetConfigReq);
	        pthread_data_copy((char*)&iRemoteGetConfigReq, (char*)pVirtualSocket->pContext.p,&res);
	        pVirtualSocket->pContext.p = (char*)&iRemoteGetConfigReq;
	        
					res	=	uart_request_remote_get_config_pack(pVirtualSocket,pUartPort);
				}
					break;
				default:
					 fprintf(stderr,"%s,line:%d,%s channel_WorkCmd %d ERROR!!\n",__FILE__,__LINE__,pUartPort->h_io,WorkCmd);
					 break;	
			} 

    } //end else
	}
	
}


//--------- end -----------------


