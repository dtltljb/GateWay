#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include 			"epb_keep_alive_pack.h"
#include 			"socket/VirtualPortMonitor.h"                 

//----------- public ----------------
KeepAliveReq			mKeepAliveReq;
KeepAliveResp			mKeepAliveResp;

//==================== keep alive request=======================================         
/*
send data:                                                                                                                                                                                                                                                                                
5a a5 12 34 56 78 00 00 00 00 00 00 00 01 20 19 04 03 16 23 34 00 00 3e 
0a 00 
10 0a 
1a 08 55 54 43 2d 31 31 00 65 
22 08 65 6e 00 63 68 2d 62 6a 
28 01 
32 06 67 45 8b 6b 00 00 
3a 10 63 68 2d 62 6a 73 2d 32 30 31 39 2d 31 2d 33 31 
42 08 67 45 8b 6b 00 00 00 00 
00 00 0e e0 71 c2 a7 49 5a a5 12 34 56 78 00 00 00 00 00 00 00 01 19 70 01 16 21 48 01 00 00 36 
*/                                

#define TAG_keep_alive_base_request												0x0a
#define TAG_keep_alive_Time_Interval											0x10
#define TAG_keep_alive_Time_Zone													0x1a
#define TAG_keep_alive_Language														0x22
#define TAG_keep_alive_Auth_method												0x28

#define TAG_keep_alive_MacAddress													0x32
#define TAG_keep_alive_DeviceName													0x3a
#define TAG_keep_alive_random															0x42

int32_t epb_keepAlive_request_pack_size(KeepAliveReq *request)
{
	int pack_size = 0;
	int len = 0;

	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_keep_alive_base_request, len);
	pack_size += epb_varint32_pack_size(TAG_keep_alive_Time_Interval, request->keep_alive_time, false);
	//pack_size += epb_varint32_pack_size(TAG_keep_alive_Time_Zone, request->TimeZone, false);
	pack_size += epb_length_delimited_pack_size(TAG_keep_alive_Time_Zone, request->TimeZone.len);
  //pack_size += epb_varint32_pack_size(TAG_keep_alive_Language, request->Language, false);
	pack_size += epb_length_delimited_pack_size(TAG_keep_alive_Language, request->Language.len);  
	pack_size += epb_varint32_pack_size(TAG_keep_alive_Auth_method, request->AuthMethod, false);

    pack_size += epb_length_delimited_pack_size(TAG_keep_alive_MacAddress, request->MacAddress.len);
    pack_size += epb_length_delimited_pack_size(TAG_keep_alive_DeviceName, request->DeviceName.len);
	pack_size += epb_length_delimited_pack_size(TAG_keep_alive_random, request->keep_alive_random.len);
 
	return pack_size;
}

int32_t epb_pack_keepAlive_request(KeepAliveReq *request, uint8_t *buf, int buf_len)
{
	int ret;
	int tmp_len;
	uint8_t *tmp;
	Epb epb;
	epb_pack_init(&epb, buf, buf_len);
	tmp_len = epb_base_request_pack_size(request->base_request);
	tmp = (uint8_t *)malloc(tmp_len);
	ret = epb_pack_base_request(request->base_request, tmp, tmp_len);
	if (ret < 0) {
		free(tmp);
		return ret;
	}
	
	ret = epb_set_message(&epb, TAG_keep_alive_base_request, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, (uint16_t)TAG_keep_alive_Time_Interval, request->keep_alive_time);
	if (ret < 0) return ret;
		
	//ret = epb_set_int32(&epb, (uint16_t)TAG_keep_alive_Time_Zone, request->TimeZone);
	//if (ret < 0) return ret;
	ret = epb_set_string(&epb, TAG_keep_alive_Time_Zone, request->TimeZone.str, request->TimeZone.len);
	if (ret < 0) return ret;
	//ret = epb_set_int32(&epb, (uint16_t)TAG_keep_alive_Language, request->Language);
	//if (ret < 0) return ret;
	ret = epb_set_string(&epb, TAG_keep_alive_Language, request->Language.str, request->Language.len);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, (uint16_t)TAG_keep_alive_Auth_method, request->AuthMethod);
	if (ret < 0) return ret;
		
    ret = epb_set_bytes(&epb, TAG_keep_alive_MacAddress, request->MacAddress.data, request->MacAddress.len);
	if (ret < 0) return ret;
    ret = epb_set_string(&epb, TAG_keep_alive_DeviceName, request->DeviceName.str, request->DeviceName.len);
	if (ret < 0) return ret;				
	ret = epb_set_bytes(&epb, TAG_keep_alive_random, request->keep_alive_random.data, request->keep_alive_random.len);	
	if (ret < 0) return ret;			
	return epb_get_packed_size(&epb);
}

//=============================================================================
#define TAG_keep_alive_base_response												0x0a
#define TAG_keep_alive_TimeInterval_resp										0x10
#define TAG_keep_alive_ManagerPhoneID_resp									0x1a
#define TAG_keep_alive_ServerURL_resp												0x22
#define TAG_keep_alive_ManufaturerURL_resp									0x2a
#define TAG_keep_alive_random_resp													0x32

//================================ unpack KeepAliveResp ================================

KeepAliveResp *epb_unpack_keepAlive_response(KeepAliveResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const char *str;
	int tmp_len;
  uint8_t	ManagerPhoneID[16],ServerURL[128],ManufaturerURL[128],random[8];
  response->ManagerPhoneID.str     	=  (char*) ManagerPhoneID;
  response->ServerURL.str     			= ( char*)  ServerURL;
  response->ManufaturerURL.str     	=  ( char*) ManufaturerURL;
	response->keep_alive_random.data 	=   random;

	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_keep_alive_base_response)) {
		return 0;
	}
	
	tmp = epb_get_message(&epb, TAG_keep_alive_base_response, &tmp_len);
	response->base_response = epb_unpack_base_response(response->base_response,tmp, tmp_len);
	
	if (epb_has_tag(&epb, TAG_keep_alive_TimeInterval_resp)){
		response->keep_alive_time = epb_get_uint32(&epb, TAG_keep_alive_TimeInterval_resp);
	}
	if (epb_has_tag(&epb, TAG_keep_alive_ManagerPhoneID_resp)) {
		str = epb_get_string(&epb, TAG_keep_alive_ManagerPhoneID_resp, &response->ManagerPhoneID.len);
		memcpy((void*)response->ManagerPhoneID.str,str,response->ManagerPhoneID.len);
	}
	if (epb_has_tag(&epb, TAG_keep_alive_ServerURL_resp)) {
		str = epb_get_string(&epb, TAG_keep_alive_ServerURL_resp, &response->ServerURL.len);
		memcpy((void*)response->ServerURL.str,str,response->ServerURL.len);
	}
	if (epb_has_tag(&epb, TAG_keep_alive_ManufaturerURL_resp)) {
		str = epb_get_string(&epb, TAG_keep_alive_ManufaturerURL_resp, &response->ManufaturerURL.len);
		memcpy((void*)response->ManufaturerURL.str,str,response->ManufaturerURL.len);
	}
	if (epb_has_tag(&epb, TAG_keep_alive_ManufaturerURL_resp)){
		tmp = epb_get_bytes(&epb, TAG_keep_alive_random_resp, &response->keep_alive_random.len);
		memcpy((void*)response->keep_alive_random.data,tmp,response->keep_alive_random.len);
	}
    return response;
}

void epb_unpack_auth_response_free(KeepAliveResp *response)
{
	epb_unpack_base_response_free(response->base_response);
	free(response);
}


/***********************************************************************
 * @brief				keepAlive_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 **********************************************************************/

void keepAlive_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
	char	*TimeZone	= "UTC-11",*Language = "en",*DeviceName = "ch-bjs-2019-1-31";
        uint8_t  			databuf[128],header[64];
        //uint8_t				*databuf,*header;
//        databuf	=	(uint8_t*)malloc(128);
//        header	=	(uint8_t*)malloc(64);
		int16_t				dataLength;
    uint32_t 			res;
    int64_t				random_val;
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;

		KeepAliveReq*	pKeepAliveReq	= (KeepAliveReq*)args;
		pKeepAliveReq->base_request			=	NULL;
		pKeepAliveReq->keep_alive_time		=	10;
		
		//memcpy(pKeepAliveReq->TimeZone.str,TimeZone,sizeof(TimeZone));
		pKeepAliveReq->TimeZone.str				=	TimeZone;
		pKeepAliveReq->TimeZone.len				=	sizeof(TimeZone);
		//memcpy(pKeepAliveReq->Language.str,Language,sizeof(Language));
		pKeepAliveReq->Language.str				=	Language;
		pKeepAliveReq->Language.len				=	sizeof(Language);
		
		pKeepAliveReq->AuthMethod					=	0x01;
		pKeepAliveReq->MacAddress.data	=	(uint8_t*)&random_val;
		pKeepAliveReq->MacAddress.len		=	6;
		//memcpy(pKeepAliveReq->DeviceName.str,DeviceName,sizeof(DeviceName));
		pKeepAliveReq->DeviceName.str		=		DeviceName;
    pKeepAliveReq->DeviceName.len		=		strlen(DeviceName);
		
		random_val	=	 random();
		//random_val	=	0Xcc345678123456dd ; 
		pKeepAliveReq->keep_alive_random.data	=	(uint8_t*)&random_val;
		pKeepAliveReq->keep_alive_random.len	=	8;

		dataLength = epb_keepAlive_request_pack_size(pKeepAliveReq);
		res	=	epb_pack_keepAlive_request(pKeepAliveReq, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
    fix_head->CSN						=	mVirtualSocket.Local_CSN;
   // fix_head->State					= 0x8005;
    fix_head->State					= 0x00;
    fix_head->WorkType			=	keep_alive_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
//		free(databuf);
//		free(header);
}


/***********************************************************************
 * @brief				exception_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 **********************************************************************/
#define			FRAME_ERROE_ENCODER		0x9001

void exception_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
	char	*TimeZone	= "UTC-11",*Language = "en",*DeviceName = "ch-bjs-2019-1-31";
        uint8_t  			databuf[128],header[64];
        //uint8_t				*databuf,*header;
//        databuf	=	(uint8_t*)malloc(128);
//        header	=	(uint8_t*)malloc(64);
		int16_t				dataLength;
    uint32_t 			res;
    int64_t				random_val;
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;

		KeepAliveReq*	pKeepAliveReq			= (KeepAliveReq*)args;
		pKeepAliveReq->base_request			=	NULL;
		pKeepAliveReq->keep_alive_time	=	10;
		
		//memcpy(pKeepAliveReq->TimeZone.str,TimeZone,sizeof(TimeZone));
		pKeepAliveReq->TimeZone.str				=	TimeZone;
		pKeepAliveReq->TimeZone.len				=	sizeof(TimeZone);
		//memcpy(pKeepAliveReq->Language.str,Language,sizeof(Language));
		pKeepAliveReq->Language.str				=	Language;
		pKeepAliveReq->Language.len				=	sizeof(Language);
		
		pKeepAliveReq->AuthMethod					=	0x01;
		pKeepAliveReq->MacAddress.data	=	(uint8_t*)&random_val;
		pKeepAliveReq->MacAddress.len		=	6;
		//memcpy(pKeepAliveReq->DeviceName.str,DeviceName,sizeof(DeviceName));
		pKeepAliveReq->DeviceName.str		=		DeviceName;
    pKeepAliveReq->DeviceName.len		=		strlen(DeviceName);
		
		random_val	=	 random();
		//random_val	=	0Xcc345678123456dd ; 
		pKeepAliveReq->keep_alive_random.data	=	(uint8_t*)&random_val;
		pKeepAliveReq->keep_alive_random.len	=	8;

		dataLength = epb_keepAlive_request_pack_size(pKeepAliveReq);
		res	=	epb_pack_keepAlive_request(pKeepAliveReq, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
    fix_head->CSN						=	mVirtualSocket.Local_CSN;
    fix_head->State					= FRAME_ERROE_ENCODER;
    fix_head->WorkType			=	keep_alive_cmd;
		fix_head->WorkIndex			=	0xFF;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
//		free(databuf);
//		free(header);
}

/***********************************************************************
 * @brief				debug_reslut_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 **********************************************************************/
/*
void debug_reslut_pack(void *args)
{

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

//---- unpacket uart	remote load ack Response --------------


		pConfigLockerInfoResp		=	(ConfigLockerInfoResp*)ptr;
		
		 memcpy((char*)&iConfigLockerInfoResp,ptr,sizeof(ConfigLockerInfoResp));
     pConfigLockerInfoResp =  &iConfigLockerInfoResp;
     iRemoteGetConfigResultReq.base_request			=	&iBaseRequest;
     iRemoteGetConfigResultReq.Locker_Address    =	pConfigLockerInfoReq->LockerNumber;
     iRemoteGetConfigResultReq.Locker_ACK				=	remote_load_ok_response;
     iRemoteGetConfigResultReq.data_type					=	LoraPorperty;
     iRemoteGetConfigResultReq.data_context.len				=		sizeof(pConfigLockerInfoResp->data);
     iRemoteGetConfigResultReq.data_context.data       =   pConfigLockerInfoResp->data;
     iRemoteGetConfigResultReq.Report_Time.len         =   sizeof(pConfigLockerInfoResp->ProtoTime);
     iRemoteGetConfigResultReq.Report_Time.str         =   (char*)pConfigLockerInfoResp->ProtoTime;
		
    remote_get_config_result_request_pack((void*)&iRemoteGetConfigResultReq,s_buffer,&s_length);

		res = send(pVirtualSocket->clt_sock,s_buffer,s_length, 0 );
		if((res <= 0)||(res != s_length)){
					printf("---%s,line=%d, s_length=%d, send error!!\n",__FILE__,__LINE__,res);
		}
		pUartPort->WorkStates	=	 WorkStatesIDLE;								// unlock  channel,lock in file socket_protocol_thread.c	
		return	1;
}
*/
