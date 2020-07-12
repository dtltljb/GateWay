#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include 			"socket_task_distributer.h"
#include 			"socket/socket_protocol_thread.h"
#include 			"socket/VirtualPortMonitor.h"
#include			"uart/xxx_channel_send_thread.h"
#include      "LinkLayer/UartOpration.h"
#include      "LinkLayer/lora_parament.h"

 /***********************************************************************
 * @brief				send_correspond_semphore_number
 * @param[in]		uint8_t	channel,void *args
 * @return 			=0 ,falure 
 **********************************************************************/
 uint8_t	send_correspond_semphore_number(uint8_t	channel,void *args)
 {
 	  
    struct	VirtualSocket			*pVirtualSocket		=	(struct VirtualSocket*)args;
 	
    uint8_t	chn = channel;
    switch(chn)
 	{
 		case 1:

      pVirtualSocket->channel =   chn;
			mUartPort[FirstUartNum].channelNumber	=	chn;
 			sem_post(&sem_first_channel_WaitSend);
 			break;
 		case	2:

      pVirtualSocket->channel =   chn;
			mUartPort[SecondUartNum].channelNumber	=	chn;
 			sem_post(&sem_second_channel_WaitSend);
 			break;
 		case	3:

      pVirtualSocket->channel =   chn;
			mUartPort[ThirdUartNum].channelNumber	=	chn;
 			sem_post(&sem_third_channel_WaitSend);
 			break;
 		case	4:

      pVirtualSocket->channel =   chn;
      mUartPort[FourthUartNum].channelNumber	=	chn;
 			sem_post(&sem_fourth_channel_WaitSend);	
 			break;
 		case	5:	//configure channel

      pVirtualSocket->channel =   chn;
 			mUartPort[ConfigUartNum].channelNumber	=	chn;
 			sem_post(&sem_config_channel_WaitSend);	
 			break;
 		default:
      printf("ERROR:%s,line:%d,lora_channel_error!!!sure set context:%d !!\n",__FILE__,__LINE__,chn);
      chn	=	1;
 			sem_post(&sem_first_channel_WaitSend);
 			break;	
 	}
 	return	chn;
}
 
	
