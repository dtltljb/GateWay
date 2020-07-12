
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

#include 			"uart_listener_thread.h"
#include 			"uart/UartPacket.h"
#include 			"uart/UartUnpacket.h"
#include			"uart/xxx_channel_send_thread.h"
#include 			"uart/WorkCommandAnalysis.h"
#include 			"uart/ConfigCommandAnalysis.h"
#include 			"uart/Cmd/report_log_pack.h"

#include 			"socket/protoBuf/epb_remote_authorize_pack.h"
#include 			"socket/protoBuf/epb_remote_open_pack.h"
#include 			"socket/protoBuf/epb_report_log_pack.h"
#include 			"socket/protoBuf/epb_remote_config_pack.h"
#include 			"socket/protoBuf/epb_remote_load_pack.h"
#include 			"socket/AppLayer/AysnchronousWork.h"
#include			"socket/VirtualPortMonitor.h"
#include 			"bdb/FileManager.h"

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
//    printf("->> %s,line:%d,	fd:%d,sec:%d\n",__FILE__,__LINE__,fd,sec );
    if( rc<0 )  //error
         return -1;
    return FD_ISSET(fd, &rset)  ? 1: 0;
}

/*
*		first_channel_Listener
******************************************************************/
void* first_channel_Listener(void	*pvoid)
{
	//--------------	Parament initialize	----------------
#if Debug_ubuntu_Level	== 1	
  const char *uart = "/dev/ttyUSB1";
#else
  const char *uart = "/dev/ttymxc1";
#endif
  UartPort *pUartPort		=	&mUartPort[FirstUartNum];
	pUartPort->h_io				=	uart;
	pUartPort->bps		=	9600;
	pUartPort->bit		=	8;
	pUartPort->check	=	'N';
	
  #if Debug_ubuntu_Level	== 1
  pUartPort->FreqValue				=	CHANNEL_ONE_VALUE;
  pUartPort->AirRate					=	first_chn_AirRate;
  pUartPort->GateWayAddress		=	first_chn_GateWayHalAddress;
  #endif
  
	pUartPort->channelNumber		=	FirstLoraChn;	
	pUartPort->deviceStates			=	WorkStatesIDLE;
	if ( pvoid == NULL )
     printf("%s,line:%d first_channel_Listener run... \n",__FILE__,__LINE__);
	//-----------		public part	----------------------------
  uint8_t	waitTimes	=	5;
  uint8_t		databuf[255],idataLen;
  uint8_t	res     =   strlen(uart);
  memcpy(pUartPort->iodevice,uart,res);
  
  struct VirtualSocket         *pVirtualSocket			=	&mVirtualSocket;

	while(1)
	{
		#if (Debug_Level == 1)
		printf("%s,%d,initialize_uart %s \n",__FILE__,__LINE__,uart);
		#endif
		pUartPort->deviceStates	=	UartInitialize;
		while( initialize_uart(pUartPort) != 0 ){
      sleep(1);
    }
    pUartPort->deviceStates	=	UartIDLE;   
		select_lora_states(&pUartPort->channelNumber,0,1);
		
    while(1)
    {
		      if(uart_read_wait_time(pUartPort->clt_uart, waitTimes) == 0)
					{
						#if (Debug_Level == 1)
                 //printf("%s,%d,port:%s read timeout \n",__FILE__,__LINE__,uart);
		        #endif
		        pUartPort->pipe	=	PIPE_RECE_TIMEOUT;
					}
					else
					{
						memset(databuf,0,sizeof(databuf));
		        idataLen = read(pUartPort->clt_uart,databuf,sizeof(databuf));
            if ( idataLen <= UART_FRAME_LENGTH)
		        {
              for(uint8_t i = 0;i<idataLen;i++)
                printf("%02x ",databuf[i]);
              printf("\r\n%s,%d , %s read len=%d bytes short!!\n",__FILE__,__LINE__,pUartPort->h_io,idataLen);  
							continue;
						}
						//-----------	analysis uart lora frame	---------------			
						#if Debug_Level	== 1
              for(uint8_t i = 0;i<idataLen;i++)
                printf("%02x ",databuf[i]);
              printf("\r\n%s,%d, %s receive len=%d bytes \n",__FILE__,__LINE__,pUartPort->h_io,idataLen);  
             #endif
						if(pUartPort->pipe	==	PIPE_LORA_REQUEST)	
						{	//---server-to-locker:locker reponse to sendTaskEntity.c,line=124
              memcpy(pUartPort->UartSingleReceiveBuf,databuf,idataLen);
     					pUartPort->UartSingleReceiveLength	=	idataLen;
							pUartPort->pipe	=	PIPE_RECE_SUCCESS;
							continue;
						}
						//-----------		unpacket uart frame	---------------	
            res	= uart_data_unpack_func(pUartPort, databuf, &idataLen);
						if(res	!= UartSingleReceiverSuccess){
							printf("\r\n%s,%d, %s receive exception data res=%d !!\n",__FILE__,__LINE__,pUartPort->h_io,res);  
							continue;
						}
						//-----------	analysis uart frame	---------------				
            pVirtualSocket->pContext.p 	 	=   (void*)&databuf[0];							// because report log pack use server response context.wait ensure
            pVirtualSocket->pContext.len  =    idataLen;			
						res	=	work_command_analysis(pUartPort,pVirtualSocket);
					}
			}//end while(2)	
		close(pUartPort->clt_uart);
	}
	//-----------		public part	end----------------------------
}

