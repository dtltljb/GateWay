#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include 		"epb_remote_load_pack.h"
#include    "socket/VirtualPortMonitor.h"
#include    "uart/Cmd/remote_load_pack.h"

//uint8_t									mfinger_context[4000];

RemoteLoadReq						mRemoteLoadReq;
RemoteLoadResp					mRemoteLoadResp;
RemoteLoadResultReq			mRemoteLoadResultReq;
RemoteLoadResultResp		mRemoteLoadResultResp;

//======================== remote load pack request ============================================
#define			TAG_remote_load_request    								0x0a
#define			TAG_Locker_Address												0x10
#define			TAG_Channel_number												0x18
#define			TAG_User_Control_list											0x20
#define			TAG_finger_Number													0x28
#define			TAG_finger_context												0x32

RemoteLoadReq *epb_remote_load_request_unpack(RemoteLoadReq *request, uint8_t *buf, uint16_t buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	
  request->finger_context.data	=	request->finger_data;
  
	epb_unpack_init(&epb, buf, buf_len);
	
	if (!epb_has_tag(&epb, TAG_remote_load_request)) {
		return NULL;
	}
	
	tmp = epb_get_message(&epb, TAG_remote_load_request, &tmp_len);
	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Locker_Address)){
		request->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address);
	}
	
	if (epb_has_tag(&epb, TAG_Channel_number)){
		request->Channel_number = epb_get_uint32(&epb, TAG_Channel_number);
//		request->has_Channel_number = true;
	}
	
	if (epb_has_tag(&epb, TAG_User_Control_list)){
		request->User_Control_list = epb_get_uint32(&epb, TAG_User_Control_list);
//		request->has_finger_Number = true;
	}
	
	if (epb_has_tag(&epb, TAG_finger_Number)){
		request->finger_Number = epb_get_uint32(&epb, TAG_finger_Number);
//		request->has_finger_Number = true;
	}
	if (epb_has_tag(&epb, TAG_finger_context)) {
		str = epb_get_bytes(&epb, TAG_finger_context, &request->finger_context.len);
		memcpy((void*)request->finger_context.data,str,request->finger_context.len);
	}
	return request;
}

//========================= remote load response pack ==========================
#define			TAG_remote_load_base_response							0x0a
#define			TAG_Locker_Address_resp										0x10
#define			TAG_Response_ACK_resp											0x18

int epb_remote_load_response_pack_size(RemoteLoadResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);

	pack_size += epb_length_delimited_pack_size(TAG_remote_load_base_response, len);

	pack_size += epb_varint32_pack_size(TAG_Locker_Address_resp, response->Locker_Address, false);

	pack_size += epb_varint32_pack_size(TAG_Response_ACK_resp, response->Response_ACK, false);

	return pack_size;
}

int epb_remote_load_response_pack(RemoteLoadResp *response, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_load_base_response, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_Locker_Address_resp, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Response_ACK_resp, response->Response_ACK);
	if (ret < 0) return ret;

	return epb_get_packed_size(&epb);
}


/***********************************************************************
 * @brief				remote_load_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_load_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
    uint8_t				idata[32];
		int16_t				dataLength;
    uint32_t 			res;
  //  int64_t				random_val;
    BaseResponse 		iBaseResponse;
    iBaseResponse.err_msg.str		=	(char*)idata;
    iBaseResponse.err_msg.len		=	0x0;
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
		RemoteLoadResp	*pRemoteLoadResp	= (RemoteLoadResp*)args;
		RemoteLoadReq		*pRemoteLoadReq		=	&mRemoteLoadReq;
    struct VirtualSocket		*pVirtualSocket		=	&mVirtualSocket;
		
		pRemoteLoadResp->base_response		=	&iBaseResponse;

		pRemoteLoadResp->Locker_Address		=	pRemoteLoadReq->Locker_Address;
//		pRemoteLoadResp->Response_ACK			=	0x011;
	
		dataLength = epb_remote_load_response_pack_size(pRemoteLoadResp);

		res	=	epb_remote_load_response_pack(pRemoteLoadResp, databuf, dataLength);
        printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0102;
		fix_head->WorkType			=	remote_load_cmd;
		fix_head->WorkIndex			=	0xFF;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
}
/*
*							remote loa result request
**************************************************************************/
#define			TAG_remote_load_Result_request    								0x0a
#define			TAG_Locker_Address_Result													0x10
#define			TAG_Last_Work_Type_Result													0x18
#define			TAG_Locker_Status_Result													0x20		//00100 000
#define 		TAG_Locker_ACK_Result															0x28
#define			TAG_Report_Time_Result														0x32



int epb_remote_load_result_pack_size(RemoteLoadResultReq *request)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_remote_load_Result_request, len);
	
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_Result, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type_Result, request->Last_Work_Type, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_Status_Result, request->Locker_Status, false);	
	pack_size += epb_length_delimited_pack_size(TAG_Report_Time_Result, request->Report_Time.len);	
	pack_size += epb_varint32_pack_size(TAG_Locker_ACK_Result, request->Locker_ACK, false);
	
	return pack_size;
}


int epb_remote_load_result_pack_request(RemoteLoadResultReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_load_Result_request, tmp, tmp_len);
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
 * @brief				remote_load_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_load_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		static uint8_t  			databuf[128],header[64];

		int16_t				dataLength;
    uint32_t 			res;
 //   int64_t				random_val;
    
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
		RemoteLoadResultReq		*pRemoteLoadResultReq	= (RemoteLoadResultReq*)args;
		
    struct VirtualSocket       	*pVirtualSocket				=	&mVirtualSocket;
//    RemoteLoadReq					*pRemoteLoadReq				=	&mRemoteLoadReq;
//		URemoteLoadResp			*pURemoteLoadResp		=	&mURemoteLoadResp;
		
		pRemoteLoadResultReq->base_request			=	NULL;
		//pRemoteLoadResultReq->Locker_Address		=	pRemoteLoadReq->Locker_Address;
		//pRemoteLoadResultReq->Last_Work_Type		=	remote_load_cmd;
		//pRemoteLoadResultReq->Locker_ACK				=	0x6902;

//    pRemoteLoadResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
//    memcpy((char*)pRemoteLoadResultReq->Report_Time.str,pURemoteLoadResp->ProtoTime,sizeof(pURemoteLoadResp->ProtoTime));
//    pRemoteLoadResultReq->Report_Time.len	=	sizeof(pURemoteLoadResp->ProtoTime);

		dataLength = epb_remote_load_result_pack_size(pRemoteLoadResultReq);
		res	=	epb_remote_load_result_pack_request(pRemoteLoadResultReq, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= pVirtualSocket->socketStates;
		fix_head->WorkType			=	report_log_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
}


/*
						epb_remote_load_result_unpack_response
***************************************************************************/
#define			TAG_remote_load_result_base_response							0x0a
#define			TAG_Locker_Address_result_resp										0x10
#define			TAG_Last_Work_Type_result_resp										0x18
#define			TAG_curr_states_Result_resp												0x20
#define 		TAG_random_result_resp														0x2a

RemoteLoadResultResp *epb_remote_load_result_unpack_response(RemoteLoadResultResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_load_result_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_load_result_base_response, &tmp_len);
	response->base_response = epb_unpack_base_response(response->base_response,tmp, tmp_len);


	if (epb_has_tag(&epb, TAG_Locker_Address_result_resp)){
		response->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address_result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Last_Work_Type_result_resp)){
		response->Last_Work_Type = epb_get_uint32(&epb, TAG_Last_Work_Type_result_resp);
	}
	
    if (epb_has_tag(&epb, TAG_curr_states_Result_resp)){
		response->curr_states = epb_get_uint32(&epb, TAG_curr_states_Result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_random_result_resp)) {
		str = epb_get_bytes(&epb, TAG_random_result_resp, &response->random.len);
		memcpy((void*)response->random.data,str,response->random.len);
	}

	return response;
}
