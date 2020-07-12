#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>


#include 		"report_log_pack.h"
#include 		"uart/UartPacket.h"
#include 		"uart/UartUnpacket.h"
#include 		"uart/socket_task_distributer.h"
#include		"uart/xxx_channel_send_thread.h"
#include    "uart/LinkLayer/UartOpration.h"
#include    "uart/LinkLayer/lora_parament.h"

#include    "socket/VirtualPortMonitor.h"
#include    "socket/pub/util.h"
#include 		"socket/pub/util_date.h"

#include    "bdb/FileManager.h"
#include 		"util/pthread_data_copy.h"

//-----	public	-------
	UReportLogReq							mUReportLogReq;
	UReportLogResp						mUReportLogResp;
	UReportLogReqSec					mUReportLogReqSec;

/***********************************************************************
 * @brief				register_online_log_pack
 * @param[in]		void *args, void	*argu
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t register_online_log_pack(void *args, void	*argu)
{
    uint8_t  							chn,header[32],rdata[32],tbuf[8],rbuf[8];
    uint8_t								s_buffer[255];uint16_t	s_length;
    uint8_t								u_buffer[255];uint8_t		u_length;
    uint8_t								idata[sizeof(UReportLogResp)];
    uint8_t								sendCnt	=	0,u_delay	=	0;
    uint32_t 							res;
    enum	Uart_pipe_Flag	pipe;
    UartPort							*pWorkChannel;    
    UartPort							*pUartPort					= (UartPort*)argu;
    struct VirtualSocket  *pVirtualSocket			=	(struct VirtualSocket*)args;
    
    UReportLogResp				*pUReportLogResp		=	(UReportLogResp*)idata;
    ReportLogResp					*pReportLogResp, iReportLogResp;

    res    = sizeof(ReportLogResp);
    pthread_data_copy((char*)&iReportLogResp,(char*)pVirtualSocket->pContext.p,(int*)&res);
    pReportLogResp				=	&iReportLogResp;

		//-------------   auto config channel ----------------------------
    chn		=	get_atuo_channel_number();
		if(  chn == 0  ){
        while(1){
	        printf("ERROR:%s,line:%d,auto register loker information error\n",__FILE__,__LINE__);
	    		sleep(5);
    		}
    }
    else  if(  chn == 0xff  ){
			pReportLogResp->iLockerStates  =	LOCK_REGISTER_FAIL ;  //keep step CONFIG CHANNEL, send para default work 1 channel
			printf("ERROR:%s,line:%d,loker auto register  channel full ...\n",__FILE__,__LINE__);
	  }
    else{
    		
    }
//- - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - 

   if( (enum LockerStates)pReportLogResp->iLockerStates  ==	LOCK_REGISTER_FAIL )
       pUReportLogResp->cmd		=	ReportLogUnauthorizeACK;
   else
       pUReportLogResp->cmd		=	ReportLogACK;
       
    pUReportLogResp->LockerNumber         =		(uint32_t)t_ntohl(pReportLogResp->Locker_Address);
    pUReportLogResp->LogType							=		pReportLogResp->iLog_Type;
    memcpy(&pUReportLogResp->RandomNumber,pReportLogResp->random.data,sizeof(pUReportLogResp->RandomNumber));
    
		get_lora_channel_Frequency(chn,&res);
	  pUReportLogResp->Frequery[0]					=		res>>16;
		pUReportLogResp->Frequery[1]					=		res>>8;
		pUReportLogResp->Frequery[2]					=		res;

    uint8_t	val8;
    get_lora_channel_Hal_AirRate(chn,&val8);
    pUReportLogResp->AirRate						=		val8;
    uint16_t	val16;
    get_lora_channel_Hal_address(chn, &val16);
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
    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);
    
    printf("%s,line:%d,lora channel[%d] ,Freq[%x],hal addr[%x]\n",__FILE__,__LINE__,chn,res,val16);//debug monitor
    
    //-------------		send command wait for ack	-----------------
 		while(pUartPort->deviceStates != UartIDLE){
				printf("%s,line:%d,uart states=%d\n",__FILE__,__LINE__,pUartPort->deviceStates);
				usleep(50000);	
    }
    select_lora_states(&pUartPort->channelNumber,0,0);    
    if(chn	== 0xff)
    	pWorkChannel = &mUartPort[FirstUartNum];	
    else
			pWorkChannel = &mUartPort[chn - 1];											//get work channel parament
    sendCnt =   0;
    do{
    		pWorkChannel->pipe	=	PIPE_SEND_REQUEST;					//request work channel receive data
    		pUartPort->pipe	=	PIPE_SEND_REQUEST;
				pUartPort->deviceStates	=	UartSendBusying;
        res = write (pUartPort->clt_uart,u_buffer,u_length);
        if((res <= 0)||(res != u_length))
				{
					while(1){
            sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("ERROR:%s",(char*)stderr);
						sleep(3);
					}
				}
				pUartPort->deviceStates	=	UartIDLE;
				u_delay	=	0	;
				do{
					if( (enum LockerStates)pReportLogResp->iLockerStates  ==	LOCK_REGISTER_FAIL )
				       pipe	=	pUartPort->pipe;
				   else
				       pipe	=	pWorkChannel->pipe;
           usleep(100000);
           u_delay	=	u_delay	+	1 ;				       
				}while( (pipe != RegisterEnsureAck) &&(u_delay <= 25 ) );
				sendCnt	+=	1;
				printf("%s,%d, [ %s ] send data %d times\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
		}while( (pipe != RegisterEnsureAck) &&(sendCnt	<	3));
		
 		uint8_t *ptr;
 		if((enum LockerStates)pReportLogResp->iLockerStates ==	LOCK_REGISTER_FAIL)				//failure data from config channel 2019-6-19
 			ptr	=	pUartPort->UartSingleReceiveBuf;	 			
 		else
			ptr	=	pWorkChannel->UartSingleReceiveBuf;
	//----------debug start -----------------
    /*
    uint8_t	buf[]={0x3f,0x00,0x00, 0xff,0xfe,0x00,0x90,0x00,0x07,0x04,0xe0,0x0a,0x07,0xf2,0x00,0x00, 0x09,0xe7};
	ptr	=	buf;
	*/
	//-----------debug end------------------    
    UReportLogReqSec	*pUReportLogReqSec	=	(UReportLogReqSec*)ptr;
    BaseRequest     iBaseRequest    ={NULL};
    ReportLogReq	iReportLogReq;
    memset((char*)&iReportLogReq,0,sizeof(ReportLogReq));
    if ((sendCnt > 3)||(pUReportLogReqSec->cmd	!=	RegisterEnsureAck)||(pUReportLogReqSec->LockerNumber != t_ntohl(pReportLogResp->Locker_Address))||(htons(pUReportLogReqSec->RespACK)	!= RegisterEnsureAck_SUCC)){
        iReportLogReq.iLockerStates		=		LOCK_REGISTER_FAIL;
        if(chn > 0 && chn < 0x05	)
        	delete_locker_update_to_var(chn);												//cancel add auto-config-num 
        else if(chn == 0xff)
        	iReportLogReq.iLockerStates		=	GATEWAY_CHANNEL_FULL;
	  }else{
        iReportLogReq.iLockerStates		=		LOCK_REGISTER_SUCC;             
        if( updata_channel_prm_to_file() != 0){                  //record lock registor
	  			printf("%s,line=%d,auto config channel error...\n",__FILE__,__LINE__);
	  		}
    }
 //----------	 report log request to server ----------------------
    iReportLogReq.base_request          = &iBaseRequest;
        //iReportLogReq.Locker_Address        = t_ntohl(pReportLogResp->Locker_Address);	//swap address 2019-5-20
    iReportLogReq.Locker_Address        = pReportLogResp->Locker_Address;	
    iReportLogReq.Last_Work_Type        = report_log_cmd;
        //iReportLogReq.Door_Status						=	pUReportLogReq->LockerStates;
    iReportLogReq.Door_Status						=	0;
        //iReportLogReq.iLog_Type							=	pUReportLogReq->LogType;
    iReportLogReq.iLog_Type							=	RegisterStatus;												//log type = 6!!!
        //iReportLogReq.power									=	pUReportLogReq->LockerPower;
    iReportLogReq.power									=	0xff;
    iReportLogReq.Channel_number				=	chn;

    memset(rdata,0x00,sizeof(rdata));
    iReportLogReq.Data.data						=	rdata;
    iReportLogReq.Data.len						=	1;		//less than 0x01,if not decode failure.2019-5-29

//    iReportLogReq.Locker_Log_Time.str	=	(char*)pUReportLogReq->LogTime;
//		iReportLogReq.Locker_Log_Time.len	=	sizeof(pUReportLogReq->LogTime);
//    memcpy((char*)iReportLogReq.Locker_Log_Time.str,pUReportLogReq->LogTime,sizeof(pUReportLogReq->LogTime));
    iReportLogReq.Locker_Log_Time.str	=	(const char *)tbuf;
		Get_CurBCDTime7((char*)iReportLogReq.Locker_Log_Time.str);
		iReportLogReq.Locker_Log_Time.len	=	8;
		
    iReportLogReq.random.data						=	rbuf;
    //iReportLogReq.random.len						=	pReportLogResp->random.len;
    iReportLogReq.random.len						=	8;
    memcpy(iReportLogReq.random.data,pReportLogResp->random.data,iReportLogReq.random.len);
    register_log_request_pack((void*)&iReportLogReq,pUartPort->channelNumber,s_buffer,&s_length);

		res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
		if((res <= 0)||(res != s_length)){
		              printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
		              pVirtualSocket->socketStates			=		socket_send_data_err;
                  return 0;
		      }
    return	1;
}

/***********************************************************************
 * @brief				register_log_pack
 * @param[in]		void *args, 		VirtualSocket
 *							void	*argu,		UartPort
 * work channel receive ack
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t register_log_pack(void *args, void	*argu)
{
    uint8_t  							chn,header[32],rdata[32];
    uint8_t								s_buffer[128];uint16_t	s_length;
    uint8_t								u_buffer[255];uint8_t		u_length;
    uint8_t								idata[sizeof(UReportLogResp)];
    //uint8_t idataLen,databuf[255];
    uint8_t					sendCnt	=	0,u_delay	=	0;
    uint32_t 				res;
    uint16_t        val;
    
    UartPort							*pUartPort,*pWorkChannel;
    pUartPort					= (UartPort*)argu;
    struct VirtualSocket  *pVirtualSocket			=	(struct VirtualSocket*)args;

    UReportLogResp				*pUReportLogResp		=	(UReportLogResp*)idata;
    UReportLogReq					*pUReportLogReq			=	(UReportLogReq*)pUartPort->UartSingleReceiveBuf;
    //----------debug start -----------------
    /*
    uint8_t	REQ[]={30, 00, 00, 0xff, 0xfe, 00, 70, 0x9b, 80, 67, 67, 63, 61, 61, 20, 19, 04, 20, 10, 25, 33, 00, 0Xff, 02, 10};
    pUReportLogReq  =  (UReportLogReq*)REQ;
    */
    //-----------debug end------------------

   if ( pUReportLogReq->LogType != LockerRegister ){
            printf("%s,line:%d,lock register %s type==%d error !\n",__FILE__,__LINE__,pUartPort->h_io, pUReportLogReq->LogType);
			return	0;
		}
    pUReportLogResp->cmd									=		ReportLogACK;
    pUReportLogResp->LockerNumber					=		pUReportLogReq->LockerNumber;
    pUReportLogResp->LogType							=		pUReportLogReq->LogType;
    memcpy(&pUReportLogResp->RandomNumber,pUReportLogReq->RandomNumber,sizeof(pUReportLogResp->RandomNumber));
    
		//-------------   auto config channel ----------------------------
    chn		=	get_atuo_channel_number();
		if( ( chn != 0 ) && ( chn != 0xff ) )
		{
			//pUartPort->channelNumber		=		chn;
    	get_lora_channel_Frequency(chn,&res);					
    }else{
        while(1){
	        printf("ERROR:%s,line:%d,auto register loker information error\n",__FILE__,__LINE__);
	    		sleep(5);
    	}
    }
    pUReportLogResp->Frequery[0]					=		res>>16;
    pUReportLogResp->Frequery[1]					=		res>>8;
    pUReportLogResp->Frequery[2]					=		res;
    get_lora_channel_Hal_AirRate(chn,&sendCnt);
    pUReportLogResp->AirRate						=		sendCnt;
    get_lora_channel_Hal_address(chn, &val);
    pUReportLogResp->GateWayAddress     =	val;//	htons(val);


    UartFrame		*pUartFrame 			= (UartFrame*)header;
    pUartFrame->dataPtr						=	idata;
    //pUartFrame->Dst_Address				=	(uint16_t)t_ntohl(pUReportLogReq->LockerNumber);
    pUartFrame->Dst_Address				=	(uint16_t)(pUReportLogReq->LockerNumber);
		pUartFrame->frameLength				= sizeof(UReportLogResp);
    pUartFrame->frameTotal				=	0x00;
    pUartFrame->frameNumber				=	0x00;
    res = uart_data_pack_func(pUartFrame,u_buffer,&u_length);

    //-------------		send command wait for ack	-----------------
 		while(pUartPort->deviceStates != UartIDLE){
				printf("%s,line:%d,uart states=%d\n",__FILE__,__LINE__,pUartPort->deviceStates);
				usleep(50000);	
    }
    select_lora_states(&pUartPort->channelNumber,0,0);    
		pWorkChannel = &mUartPort[chn - 1];											//get work channel parament	  
    sendCnt =   0;
    do{
    		pWorkChannel->pipe	=	PIPE_SEND_REQUEST;					//request work channel receive data
				pUartPort->deviceStates	=	UartSendBusying;
        res = write (pUartPort->clt_uart,u_buffer,u_length);
        if((res <= 0)||(res != u_length))
				{
					while(1)
					{
            sprintf((char*)stderr,"%s,line=%d, write %s error!!\n",__FILE__,__LINE__,pUartPort->h_io);
            printf("ERROR:%s",(char*)stderr);
						sleep(3);
					}
				}
				pUartPort->deviceStates	=	UartIDLE;

					u_delay	=	0	;
					while((pWorkChannel->pipe != RegisterEnsureAck)&&(u_delay <= 25 ))
					{
						//printf("%s,line:%d,wait...pUartPort->pipe=%d\n",__FILE__,__LINE__,pUartPort->pipe);
           usleep(100000);
           u_delay	=	u_delay	+	1 ;
					}
				sendCnt	+=	1;	
				printf("%s,%d, [ %s ] send data %d times\n",__FILE__,__LINE__,pUartPort->h_io, sendCnt);
		}while((pWorkChannel->pipe != RegisterEnsureAck)&&(sendCnt	<	3));

    uint8_t *ptr;    
    ptr	=	pWorkChannel->UartSingleReceiveBuf;
	//----------debug start -----------------
    /*
    uint8_t	buf[]={0x3f,0x00,0x00, 0xff,0xfe,0x00,0x90,0x00,0x07,0x04,0xe0,0x0a,0x07,0xf2,0x00,0x00, 0x09,0xe7};
	ptr	=	buf;
	*/
	//-----------debug end------------------    
    UReportLogReqSec	*pUReportLogReqSec	=	(UReportLogReqSec*)ptr;
    BaseRequest     iBaseRequest    ={NULL};
    ReportLogReq	iReportLogReq;
    memset((char*)&iReportLogReq,0,sizeof(ReportLogReq));
    if ((sendCnt > 3)||(pUReportLogReqSec->cmd	!=	RegisterEnsureAck)||(pUReportLogReqSec->LockerNumber != pUReportLogReq->LockerNumber)||(htons(pUReportLogReqSec->RespACK)	!= RegisterEnsureAck_SUCC)){
            iReportLogReq.iLockerStates		=		LOCK_REGISTER_FAIL;
            mChannelValueList.autoConfigNum				-=	1;				//cancel add auto-config-num 
	  }else{
        iReportLogReq.iLockerStates		=		LOCK_REGISTER_SUCC;             
        if( updata_channel_prm_to_file() != 0){                  //record lock registor
	  			printf("%s,line=%d,auto config channel error...\n",__FILE__,__LINE__);
	  		}
    }
 //----------	 report log request to server ----------------------
        iReportLogReq.base_request          = &iBaseRequest;
        iReportLogReq.Locker_Address        = t_ntohl(pUReportLogReqSec->LockerNumber);	//swap address 2019-5-20
        //iReportLogReq.Locker_Address        = pUReportLogReqSec->LockerNumber;	
        iReportLogReq.Last_Work_Type        = report_log_cmd;
        //iReportLogReq.iLockerStates					=	pUReportLogReq->LockerStates;
        iReportLogReq.Door_Status						=	pUReportLogReq->LockerStates;
        iReportLogReq.iLog_Type							=	pUReportLogReq->LogType;
        iReportLogReq.power									=	pUReportLogReq->LockerPower;
        iReportLogReq.Channel_number				=	chn;
        
        memset(rdata,0x00,sizeof(rdata));
        switch(iReportLogReq.iLog_Type)
        {
            case	LockerRegister:
                iReportLogReq.Data.data						=	rdata;
                iReportLogReq.Data.len						=	1;		//less than 0x01,if not decode failure.2019-5-29
                break;
            case	FingerVena:
            case	SwingCard:
            case	Password:
                
                iReportLogReq.Data.data						=	rdata;
                iReportLogReq.Data.len						=	4;
                memcpy(iReportLogReq.Data.data,pUReportLogReq->InfoEntity,4);
                break;
            case	IdentityCard:
            default:
                iReportLogReq.Data.data						=	rdata;
                iReportLogReq.Data.len						=	sizeof(pUReportLogReq->InfoEntity);
                memcpy(iReportLogReq.Data.data,pUReportLogReq->InfoEntity,sizeof(pUReportLogReq->InfoEntity));
                break;
        }
        
    iReportLogReq.Locker_Log_Time.str	=	(char*)pUReportLogReq->LogTime;
		iReportLogReq.Locker_Log_Time.len	=	sizeof(pUReportLogReq->LogTime);
    memcpy((char*)iReportLogReq.Locker_Log_Time.str,pUReportLogReq->LogTime,sizeof(pUReportLogReq->LogTime));

    iReportLogReq.random.data					=	pUReportLogReq->RandomNumber;
    iReportLogReq.random.len						=	sizeof(pUReportLogReq->RandomNumber);
    memcpy(iReportLogReq.random.data,pUReportLogReq->RandomNumber,sizeof(pUReportLogReq->RandomNumber));

    register_log_request_pack((void*)&iReportLogReq,pUartPort->channelNumber,s_buffer,&s_length);
  //  register_log_request_pack((void*)&iReportLogReq,0x1,s_buffer,&s_length);

		res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
		if((res <= 0)||(res != s_length)){
		              printf("%s,line=%d, res=%d, send error !!! socket close....\n",__FILE__,__LINE__,res);
		              pVirtualSocket->socketStates			=		socket_send_data_err;
                  return 0;
		      }
		return	1;
}
