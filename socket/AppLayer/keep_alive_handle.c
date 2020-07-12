
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include	<signal.h>

 
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/un.h>  
#include <sys/time.h>
#include <arpa/inet.h>  
#include <netinet/in.h> 

#include 			"keep_alive_handle.h"
#include 			"socket/VirtualPortMonitor.h"
#include 			"socket/protoBuf/epb_keep_alive_pack.h"
#include 			"socket/protoBuf/epb_report_log_pack.h"
#include 			"socket/protoBuf/epb_remote_open_pack.h"
#include 			"socket/protoBuf/epb_remote_load_pack.h"
#include    	"socket/pub/util.h"
#include 			"uart/Cmd/report_log_pack.h"

#define				_GNU_SOURCE					//compile evn
//------------------- private -------------------
#define				RESPONSE_MAX_TIME						5
ReportLogReq				mReportLogReq;
/***********************************************************************
 * @brief				keep_alive_process
 *							心跳基准定时器频率1Hz,接收到应当或通信链路其他数据,顺延计时点;保证链路通讯期间
 *							不会发送心跳数据,而是在无数据通讯期间发送心跳数据。保持socket在线连接能力。
 * @param[in]		int32_t second
 * @return 			non
 **********************************************************************/
void	keep_alive_process(void)
{

	BaseRequest 		iKeepAliveReqBaseReq = {NULL};
	uint8_t		socketSendBuf[256];
  uint16_t	socketSendLength;
	int32_t	res;

  if( (mVirtualSocket.socketStates != SOCKET_CONNECTED)||(mVirtualSocket.clt_sock <= 0) ){
  	#ifdef		Debug_Level
    printf("%s,line:%d ,sockID disconected quit heart process.. \n",__FILE__,__LINE__);
    #endif
    return ;
    }

	if(mVirtualSocket.keep_alive_times > mVirtualSocket.base_time_count)
        res	=	(mVirtualSocket.keep_alive_times - mVirtualSocket.base_time_count);
	else
        res	=	(mVirtualSocket.base_time_count - mVirtualSocket.keep_alive_times);
	//----------------heart time out judge------------------	
	if( (mVirtualSocket.keep_alive_response == REQUEST)&&(res > RESPONSE_MAX_TIME) ){
		    printf("%s,line=%d, non receive keep alive response, socket close....\n",__FILE__,__LINE__);
        close(mVirtualSocket.clt_sock);
        return;
	}
	//-----------------first create link not startup jugde-------------------	
    if( (mVirtualSocket.keep_alive_response != STARTUP)&&(res < mVirtualSocket.keep_alive) ){
        return ;
	}
	#ifdef		Debug_Level
//	printf("%s,line:%d,response=%d,alive=%d,times=%d,count=%d\n",__FILE__,__LINE__,mVirtualSocket.keep_alive_response,mVirtualSocket.keep_alive,mVirtualSocket.keep_alive_times,mVirtualSocket.base_time_count);
	#endif
	
	mKeepAliveReq.base_request	=	&iKeepAliveReqBaseReq;
  keepAlive_request_pack((void*)&mKeepAliveReq,socketSendBuf,&socketSendLength);
  res = send(mVirtualSocket.clt_sock,socketSendBuf,socketSendLength, 0 );
	if((res <= 0)||(res != socketSendLength)){
        printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
        close(mVirtualSocket.clt_sock);
        return;
	}
	
	mVirtualSocket.keep_alive_response = REQUEST;
	mVirtualSocket.keep_alive_times	= mVirtualSocket.base_time_count;
	
	
	
/*	
	   //how long to run the first time
   tv.it_value.tv_sec =  (struct itimerval)mVirtualSocket.keep_alive;
   tv.it_value.tv_usec = 0;
   //after the first time, how long to run next time
   tv.it_interval.tv_sec =  (struct itimerval)mVirtualSocket.keep_alive;
   tv.it_interval.tv_usec = 0;
   if (setitimer(ITIMER_REAL, &tv, &otv) != 0) 
		printf("setitimer signal err %d\n", errno);
*/
  //printf("%s,line:%d,socket id=%d,sendLen=%d\n",__FILE__,__LINE__,mVirtualSocket.clt_sock,socketSendLength);
  
  return	;
}

/***********************************************************************
 * @brief				keep_alive_handle
 * @param[in]		int32_t second
 * @return 			non
 **********************************************************************/

void	*keep_alive_handle(int argv)
{
    mVirtualSocket.base_time_count	+=1;
	keep_alive_process();
	
//	monitor_link_list_empty();					//in FileManager.c
//				
//	monitor_forlder_file_empty();				//in FileManager.c
	
  //printf("%s,line:%d,ptr=%0x02 keep alive handle triger \n",__FILE__,__LINE__,argv);
  return	NULL;
}

/***********************************************************************
 * @brief				report_log_response_handle
 * @param[in]		int32_t second
 * @return 			void
 **********************************************************************/
/*
void	report_log_response_handle(void *pvoid)
{
	BaseResponse 		reportLogRespBase ;
	static	uint8_t	socketSendBuf[100];
    uint16_t	socketSendLength;
	int32_t	res;
	

    mReportLogResp.base_response	=	&reportLogRespBase;
    report_log_response_pack((void*)&mReportLogResp,socketSendBuf,&socketSendLength);
	

	#ifdef Debug_Level
	printf("socketSendBuf:\n");
	for(int16_t i=0;i<socketSendLength;i++)
		printf(" %#2x",socketSendBuf[i]);
	printf("\n\r socketSendBuf over!!");		
	#endif
	
	
    if(mVirtualSocket.clt_sock == 0)
		return;
    res = send(mVirtualSocket.clt_sock,socketSendBuf,socketSendLength, 0 );
	if((res <= 0)||(res != socketSendLength))
	{
			printf("---%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
        close(mVirtualSocket.clt_sock);
		}


}
*/

/***********************************************************************
 * @brief				repmote_open_result_request_handle
 * @param[in]		int32_t second
 * @return 			void
 **********************************************************************/
/*
void	repmote_open_result_request_handle(void *pvoid)
{
  BaseRequest 		iBaseRequest = {NULL};
	static	uint8_t	socketSendBuf[100];
    uint16_t	socketSendLength;
	int32_t	res;
	

  mRemoteOpenResultReq.base_request	=	&iBaseRequest;
    remote_open_result_request_pack((void*)&mRemoteOpenResultReq,socketSendBuf,&socketSendLength);
	

	#ifdef Debug_Level
	printf("socketSendBuf:\n");
	for(int16_t i=0;i<socketSendLength;i++)
		printf(" %#2x",socketSendBuf[i]);
	printf("\n\r socketSendBuf over!!");		
	#endif
	
	
  if(mVirtualSocket.clt_sock == 0)
		return;
  res = send(mVirtualSocket.clt_sock,socketSendBuf,socketSendLength, 0 );
	if((res <= 0)||(res != socketSendLength))
	{
			printf("---%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
        close(mVirtualSocket.clt_sock);
		}


}
*/

/***********************************************************************
 * @brief				repmote_load_result_request_handle
 * @param[in]		int32_t second
 * @return 			void
 **********************************************************************/
/*
void	repmote_load_result_request_handle(void *pvoid)
{
    BaseResponse 		iBaseResponse={
    	.err_msg.str	=	NULL,
    	.err_msg.len	= 0,
    	.err_code		=	0,
    };
	static	uint8_t	socketSendBuf[100];
    uint16_t	socketSendLength;
	int32_t	res;
	

    mRemoteLoadResp.base_response	=	&iBaseResponse;
     mRemoteLoadResp.base_response->err_msg.len =0;
     
    remote_load_response_pack(&mRemoteLoadResp,socketSendBuf,&socketSendLength);
	

	#ifdef Debug_Level
	printf("socketSendBuf:\n");
	for(int16_t i=0;i<socketSendLength;i++)
		printf(" %#2x",socketSendBuf[i]);
	printf("\n\r socketSendBuf over!!");		
	#endif
	
	
    if(mVirtualSocket.clt_sock == 0)
		return;
    res = send(mVirtualSocket.clt_sock,socketSendBuf,socketSendLength, 0 );
	if((res <= 0)||(res != socketSendLength))
	{
			printf("---%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
        close(mVirtualSocket.clt_sock);
		}

}

*/
