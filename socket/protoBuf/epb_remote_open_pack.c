#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include 		"epb_remote_open_pack.h"
#include    "socket/VirtualPortMonitor.h"
#include    "uart/Cmd/remote_open_pack.h"

//---------------		public		-----	-----

//RemoteOpenReq							mRemoteOpenReq;
//RemoteOpenResp						mRemoteOpenResp;
//RemoteOpenResultReq				mRemoteOpenResultReq;
//RemoteOpenResultResp			mRemoteOpenResultResp;

//======================== remote open request unpack ============================================

#define			TAG_remote_open_request    								0x0a		//00001 010
#define			TAG_Locker_Address												0x10		//00010 000
#define			TAG_Channel_number												0x18		//00011 000
#define			TAG_Control_state													0x20		//00100 000

RemoteOpenReq *epb_remote_open_request_unpack(RemoteOpenReq *request,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
//	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_open_request)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_open_request, &tmp_len);
	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Locker_Address)){
		request->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address);
	}
	
	if (epb_has_tag(&epb, TAG_Channel_number)){
		request->Channel_number = epb_get_uint32(&epb, TAG_Channel_number);
	}
	if (epb_has_tag(&epb, TAG_Control_state)){
		request->Control_state = epb_get_uint32(&epb, TAG_Control_state);
	}
	return request;
}

//========================= remote open response pack ==========================
#define			TAG_remote_open_base_response							0x0a
#define			TAG_Locker_Address_resp										0x10
#define			TAG_Response_ACK_resp											0x18


int epb_remote_open_response_pack_size(RemoteOpenResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_remote_open_base_response, len);
	
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_resp, response->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Response_ACK_resp, response->Response_ACK, false);

	return pack_size;
}

int epb_remote_open_response_pack(RemoteOpenResp *response, uint8_t *buf, int buf_len)
{
	int ret;
	int tmp_len;
	uint8_t *tmp;
	Epb epb;
	epb_pack_init(&epb, buf, buf_len);

	tmp_len = epb_base_response_pack_size(response->base_response);
	tmp = (uint8_t *)malloc(tmp_len);
	ret = epb_pack_base_response(response->base_response, tmp, tmp_len);
	if (ret < 0) {
		free(tmp);
		return ret;
	}
	ret = epb_set_message(&epb, TAG_remote_open_base_response, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_Locker_Address_resp, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Response_ACK_resp, response->Response_ACK);
	if (ret < 0) return ret;

	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				remote_open_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_open_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
    char *iMsg  = "remote open response";
    BaseResponse				iBaseResponse;
    iBaseResponse.err_msg.str				=	(char*)iMsg;
    iBaseResponse.err_msg.len				=	strlen((char*) iMsg);
		uint8_t  			databuf[128],header[64];
//		uint8_t				l_arrBcdtime[8],idata[32];
		int16_t				dataLength;
    uint32_t 			res;
//    int64_t				random_val;

    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
		RemoteOpenResp	*pRemoteOpenResp	= (RemoteOpenResp*)args;
		
    struct VirtualSocket		*pVirtualSocket		=	&mVirtualSocket;

        pRemoteOpenResp->base_response		=	&iBaseResponse;
		
		dataLength = epb_remote_open_response_pack_size(pRemoteOpenResp);
		res	=	epb_remote_open_response_pack(pRemoteOpenResp, databuf, dataLength);
        printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0102;
		fix_head->WorkType			=	remote_open_door;
		fix_head->WorkIndex			=	0xFF;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
		
}


/*
*							remote open result request
**************************************************************************/
#define			TAG_remote_open_Result_request    								0x0a
#define			TAG_Locker_Address_Result													0x10
#define			TAG_Last_Work_Type_Result													0x18		//00011 000
#define			TAG_Locker_Status_Result													0x20		//00100 000
#define 		TAG_Locker_ACK_Result															0x28		//00101 000
#define 		TAG_Report_Time_Result														0x32		//00110 010

int epb_remote_open_result_pack_size(RemoteOpenResultReq *request)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_remote_open_Result_request, len);
	
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_Result, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type_Result, request->Last_Work_Type, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_Status_Result, request->Locker_Status, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_ACK_Result, request->Locker_ACK, false);
	
	pack_size += epb_length_delimited_pack_size(TAG_Report_Time_Result, request->Report_Time.len);	
	return pack_size;
}


int epb_remote_open_result_pack_request(RemoteOpenResultReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_open_Result_request, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, (uint16_t)TAG_Locker_Address_Result, request->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, (uint16_t)TAG_Last_Work_Type_Result, request->Last_Work_Type);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, (uint16_t)TAG_Locker_Status_Result, request->Locker_Status);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, (uint16_t)TAG_Locker_ACK_Result, request->Locker_ACK);
	if (ret < 0) return ret;		
	ret = epb_set_string(&epb, TAG_Report_Time_Result, request->Report_Time.str, request->Report_Time.len);
	if (ret < 0) return ret;

	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				remote_open_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_open_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		static uint8_t  			databuf[128],header[64];
//    uint8_t				l_arrBcdtime[8];
		int16_t				dataLength;
    uint32_t 			res;
  //  int64_t				random_val;
    
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
    RemoteOpenResultReq*	pRemoteOpenResultReq	= (RemoteOpenResultReq*)args;
    struct VirtualSocket       	*pVirtualSocket				=	&mVirtualSocket;
    
//    RemoteOpenReq					*pRemoteOpenReq				=	&mRemoteOpenReq;
//		RemoteOpenDoorACKResp			*pRemoteOpenDoorACKResp		=	&mRemoteOpenDoorACKResp;
//		pRemoteOpenResultReq->base_request			=	NULL;
//		pRemoteOpenResultReq->Locker_Address		=	pRemoteOpenReq->Locker_Address;
//		pRemoteOpenResultReq->Last_Work_Type		=	remote_open_door;
//		pRemoteOpenResultReq->Locker_Status			=	pRemoteOpenDoorACKResp->LockerStatus;
//		pRemoteOpenResultReq->Locker_ACK				=	pRemoteOpenDoorACKResp->RespACK;
//    pRemoteOpenResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
//    memcpy((char*)pRemoteOpenResultReq->Report_Time.str,pRemoteOpenDoorACKResp->ProtoTime,sizeof(pRemoteOpenDoorACKResp->ProtoTime));
//    pRemoteOpenResultReq->Report_Time.len	=	sizeof(pRemoteOpenDoorACKResp->ProtoTime);
    	
		dataLength = epb_remote_open_result_pack_size(pRemoteOpenResultReq);
		res	=	epb_remote_open_result_pack_request(pRemoteOpenResultReq, databuf, dataLength);
        printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0102;
    fix_head->WorkType			=	report_log_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
}


/*
 *					epb_remote_open_result_unpack_response
***************************************************************************/
#define			TAG_remote_open_result_base_response							0x0a
#define			TAG_Locker_Address_result_resp										0x10
#define			TAG_Last_Work_Type_result_resp										0x18
#define			TAG_state_Result_resp															0x20
#define 		TAG_random_result_resp														0x2a

RemoteOpenResultResp *epb_remote_open_result_unpack_response(RemoteOpenResultResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_open_result_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_open_result_base_response, &tmp_len);
	response->base_response = epb_unpack_base_response(response->base_response,tmp, tmp_len);


	if (epb_has_tag(&epb, TAG_Locker_Address_result_resp)){
		response->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address_result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Last_Work_Type_result_resp)){
		response->Last_Work_Type = epb_get_uint32(&epb, TAG_Last_Work_Type_result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_state_Result_resp)){
		response->state = epb_get_uint32(&epb, TAG_state_Result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_random_result_resp)) {
		str = epb_get_bytes(&epb, TAG_random_result_resp, &response->random.len);
		memcpy((void*)response->random.data,str,response->random.len);
	}

	return response;
}


