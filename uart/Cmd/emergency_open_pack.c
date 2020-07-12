#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include 		"emergency_open_pack.h"
#include 		"uart/UartPacket.h"
#include    "uart/LinkLayer/UartOpration.h"
#include 				"util/pthread_data_copy.h"

#include    "socket/VirtualPortMonitor.h"
#include    "socket/protoBuf/epb_emergency_open_pack.h"
#include    "socket/pub/util_date.h"

EmergencyControl			mEmergencyControl;			//uart

/***********************************************************************
 * @brief				uart_emergency_control_pack
 * @param[in]		void *args, void	*argu
 * @return 			void
 **********************************************************************/
uint8_t uart_emergency_control_pack(void *args, void	*argu)
{
    UartPort							*pUartPort					= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
 		uint8_t  			header[32],idata[sizeof(EmergencyControl)],s_buffer[255];
		uint8_t			sendCnt	=	0,s_length;
		uint8_t 			l_arrBcdtime[8] ={0};
    int 			res;

    EmergencyOpenReq        *pEmergencyOpenReq, iEmergencyOpenReq;
    res = sizeof(EmergencyOpenReq);

    pthread_data_copy((char*)&iEmergencyOpenReq, (char*)pVirtualSocket->pContext.p,(int*)&res);

    pEmergencyOpenReq	=	&iEmergencyOpenReq;
   //------------ pack -> send -> delay -> send ---------------------
    EmergencyControl				*pEmergencyControl		=	(EmergencyControl*)idata;
//    EmergencyOpenReq				*pEmergencyOpenReq		=	(EmergencyOpenReq*)pVirtualSocket->pContext.p;
   
    pEmergencyControl->cmd								=	EmergencyOpen;
    //pEmergencyControl->LockerNumber				=	t_ntohl(pEmergencyOpenReq->Locker_Address);
    pEmergencyControl->LockerNumber				=	0xffffffff;
    pEmergencyControl->ControlStatus			=	pEmergencyOpenReq->Control_state;
    Get_CurBCDTime7((char*)l_arrBcdtime);
    memcpy(pEmergencyControl->ProtoTime,l_arrBcdtime,sizeof(pEmergencyControl->ProtoTime));

    UartFrame		*pUartFrame 			= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
    pUartFrame->Dst_Address				=	0x0000;												//note!!!!
    pUartFrame->frameLength				= sizeof(EmergencyControl);
    pUartFrame->frameTotal				=	0x01;
    pUartFrame->frameNumber				=	0x01;
    res = uart_data_pack_func(pUartFrame,s_buffer,&s_length);
    select_lora_states(&pUartPort->channelNumber,0,1);
    while(pUartPort->deviceStates != UartIDLE){
                //printf("%s,line:%d,uart states=%d\n",__FILE__,__LINE__,pUartPort->h_io);
                usleep(10000);
    }

  do{
        pUartPort->deviceStates	=	UartSendBusying;
        res = write (pUartPort->clt_uart,s_buffer,s_length);
        if((res <= 0)||(res != s_length)){
					while(1){
            sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("ERROR:%s",(char*)stderr);
						sleep(3);
					}
				}
				sendCnt	+=	1;
				printf("%s,line:%d, %s emergency open send %d times!!!\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
        sleep(5);
        }while((pUartPort->pipe != PIPE_RECE_SUCCESS)&&(sendCnt	<	1));
				pUartPort->deviceStates	=	UartIDLE;
        select_lora_states(&pUartPort->channelNumber,0,0);
        pVirtualSocket->EmergencyStateLock = WorkStatesIDLE ;			//unlock state
        
		return	1;
}
