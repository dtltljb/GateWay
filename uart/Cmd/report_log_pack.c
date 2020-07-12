#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>


#include 		"report_log_pack.h"
#include 		"uart/UartPacket.h"
#include 		"uart/socket_task_distributer.h"
#include    "uart/LinkLayer/UartOpration.h"
#include    "uart/LinkLayer/lora_parament.h"

#include    "socket/VirtualPortMonitor.h"
#include    "socket/pub/util.h"


#include 				"util/pthread_data_copy.h"
//-----	public	-------
	UReportLogReq							mUReportLogReq;
	UReportLogResp						mUReportLogResp;
	UReportLogReqSec					mUReportLogReqSec;
/***********************************************************************
 * @brief				report_online_log_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t report_online_log_pack(void *args, void	*argu)
{
		uint8_t  							header[32];
    uint8_t								s_buffer[255],idata[sizeof(UReportLogResp)];
//    uint8_t						*s_buffer,*idata;
//    s_buffer	=	(uint8_t*)malloc(255);
//    idata			=	(uint8_t*)malloc(sizeof(UReportLogResp));
    uint8_t s_length;
    uint32_t 								res;
    UartPort							*pUartPort					= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
    
    UReportLogResp				*pUReportLogResp		=	(UReportLogResp*)idata;
    ReportLogResp					*pReportLogResp, iReportLogResp;
    res    = sizeof(ReportLogResp);
    pthread_data_copy((char*)&iReportLogResp,(char*)pVirtualSocket->pContext.p,(int*)&res);
    pReportLogResp				=	&iReportLogResp;


   if( pReportLogResp->iLockerStates  ==	(enum LockerStates)UN_AUTHORIZE_USER )
       pUReportLogResp->cmd		=	ReportLogUnauthorizeACK;
   else
       pUReportLogResp->cmd		=	ReportLogACK;

    pUReportLogResp->LockerNumber         =		(uint32_t)t_ntohl(pReportLogResp->Locker_Address);
    pUReportLogResp->LogType							=		pReportLogResp->iLog_Type;

    memcpy(&pUReportLogResp->RandomNumber,pReportLogResp->random.data,sizeof(pUReportLogResp->RandomNumber));
    
    get_lora_channel_Frequency(pUartPort->channelNumber,&res);
    pUReportLogResp->Frequery[0]					=		res>>16;
    pUReportLogResp->Frequery[1]					=		res>>8;
    pUReportLogResp->Frequery[2]					=		res;
    
    uint8_t	val8;
    get_lora_channel_Hal_AirRate(pUartPort->channelNumber,&val8);
    pUReportLogResp->AirRate						=		val8;
    uint16_t	val16;
    get_lora_channel_Hal_address(pUartPort->channelNumber, &val16);
    pUReportLogResp->GateWayAddress     =		val16;		//htons(val16);
    
//	-		-		-		-	check time	-		-		-		-
#if	disable_locker_check_time	== 1
     Get_CurBCDTime8((char*)pUReportLogResp->LogTime);
#endif     
//	-		-		-		-		-		-		-		-  -  -  - -

    UartFrame		*pUartFrame 			= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
    //pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pReportLogResp->Locker_Address);
    pUartFrame->Dst_Address				=	(uint16_t)(pReportLogResp->Locker_Address);
		pUartFrame->frameLength				= sizeof(UReportLogResp);
    pUartFrame->frameTotal				=	0x01;
    pUartFrame->frameNumber				=	0x01;
    res = uart_data_pack_func(pUartFrame,s_buffer,&s_length);

    //-------------		send command wait for ack	-----------------
 		while(pUartPort->deviceStates != UartIDLE){
				printf("%s,line:%d,uart states=%c\n",__FILE__,__LINE__,pUartPort->deviceStates);
				usleep(50000);	
    }
    select_lora_states(&pUartPort->channelNumber,0,0);
		pUartPort->deviceStates	=	UartSendBusying;
    res = write (pUartPort->clt_uart,s_buffer,s_length);
    pUartPort->deviceStates	=	UartIDLE;
    if((res <= 0)||(res != s_length)){
        sprintf((char*)stderr,"%s,line=%d, write %s error...\n",__FILE__,__LINE__, (char*)pUartPort->h_io);
        printf("ERROR:%s",(char*)stderr);
        pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
        return 0;
			}
//	  free(s_buffer);
//	  free(idata);
		//-------------		record	run		log		file	-----------------
		pUartPort->deviceStates	=	UartIDLE;

    printf("\n\r%s,line:%d,uart =%s response\n",__FILE__,__LINE__,pUartPort->h_io);    
    pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	     
    return	1;
}

/***********************************************************************
 * @brief				report_offline_log_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t report_offline_log_pack(void *args, void	*argu,uint8_t	cmd)
{
    uint8_t  							header[32];
    uint8_t								s_buffer[255],idata[sizeof(UReportLogResp)];
    uint8_t 							s_length;
    
    uint32_t 								res;
    UartPort							*pUartPort					= (UartPort*)argu;
    struct VirtualSocket  *pVirtualSocket			=	(struct VirtualSocket*)args;

    UReportLogResp				*pUReportLogResp		=	(UReportLogResp*)idata;
    
    UReportLogReq					*pUReportLogReq, iUReportLogReq;
    res    = sizeof(UReportLogReq);
    pthread_data_copy((char*)&iUReportLogReq,(char*)pUartPort->UartSingleReceiveBuf,(int*)&res);
    pUReportLogReq				=	&iUReportLogReq;

    pUReportLogResp->cmd		=	cmd;

    pUReportLogResp->LockerNumber         =		pUReportLogReq->LockerNumber;
    pUReportLogResp->LogType							=		pUReportLogReq->LogType;

    memcpy(&pUReportLogResp->RandomNumber,pUReportLogReq->RandomNumber,sizeof(pUReportLogReq->RandomNumber));

    get_lora_channel_Frequency(pUartPort->channelNumber,&res);
    pUReportLogResp->Frequery[0]					=		res>>16;
    pUReportLogResp->Frequery[1]					=		res>>8;
    pUReportLogResp->Frequery[2]					=		res;

    uint8_t	val8;
    get_lora_channel_Hal_AirRate(pUartPort->channelNumber,&val8);
    pUReportLogResp->AirRate						=		val8;
    uint16_t	val16;
    get_lora_channel_Hal_address(pUartPort->channelNumber, &val16);
    pUReportLogResp->GateWayAddress     =		val16;//htons(val16);
    
//	-		-		-		-	check time	-		-		-		-
#if	disable_locker_check_time	== 1
     Get_CurBCDTime8((char*)pUReportLogResp->LogTime);
#endif     
//	-		-		-		-		-		-		-		-  -  -  - -

    UartFrame		*pUartFrame 			= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
    //pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pUReportLogReq->LockerNumber);
    val16				=	(uint16_t)(pUReportLogReq->LockerNumber>>16);				//data format:e8 03 00 00 
    pUartFrame->Dst_Address				=	htons(val16);											//output:03 e8
    pUartFrame->frameLength				= sizeof(UReportLogResp);
    pUartFrame->frameTotal				=	0x01;
    pUartFrame->frameNumber				=	0x01;
    res = uart_data_pack_func(pUartFrame,s_buffer,&s_length);

    //-------------		send command wait for ack	-----------------
    while(pUartPort->deviceStates != UartIDLE){
         printf("%s,line:%d,[ %s ]uart states=%c\n",__FILE__,__LINE__,pUartPort->h_io, pUartPort->deviceStates);
         usleep(50000);
    }
    select_lora_states(&pUartPort->channelNumber,0,0);
    pUartPort->deviceStates	=	UartSendBusying;
    res = write (pUartPort->clt_uart,s_buffer,s_length);
    pUartPort->deviceStates	=	UartIDLE;
    if((res <= 0)||(res != s_length)){
        sprintf((char*)stderr,"%s,line=%d, write %s error...\n",__FILE__,__LINE__, (char*)pUartPort->h_io);
        printf("ERROR:%s",(char*)stderr);
        pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
        return 0;
       }
		pUartPort->deviceStates	=	UartIDLE;

    printf("\n\r %s,line:%d,uart %s response\n",__FILE__,__LINE__,pUartPort->h_io);
    pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
    return	1;
}
