#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include 		"epb_remote_config_pack.h"
#include 		"epb_remote_get_config_pack.h"

#include    "socket/VirtualPortMonitor.h"

#include    "uart/Cmd/remote_config_pack.h"


//RemoteGetConfigReq								mRemoteGetConfigReq;
//RemoteGetConfigResp								mRemoteGetConfigResp;
//RemoteGetConfigResultReq					mRemoteGetConfigResultReq;
//RemoteGetConfigResultResp					mRemoteGetConfigResultResp;

//======================== remote config request unpack ============================================
#define			TAG_remote_get_config_request    							0x0a
#define			TAG_get_config_locker_address												0x10
#define			TAG_get_config_channel_number												0x18

RemoteGetConfigReq *epb_remote_get_config_request_unpack(RemoteGetConfigReq *request,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
//	const unsigned char *str;
	int tmp_len;
	

	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_get_config_request)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_get_config_request, &tmp_len);
	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_get_config_locker_address)){
		request->Locker_Address = epb_get_uint32(&epb, TAG_get_config_locker_address);
	}
	if (epb_has_tag(&epb, TAG_get_config_channel_number)){
		request->Channel_number = epb_get_uint32(&epb, TAG_get_config_channel_number);
	}

	return request;
}

//========================= remote config response pack ==========================
#define			TAG_remote_get_config_base_response							0x0a
#define			TAG_get_locker_address_resp											0x10
#define			TAG_get_response_ACK_resp												0x18

static int epb_remote_get_cofig_response_pack_size(RemoteGetConfigResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_remote_get_config_base_response, len);
	
	pack_size += epb_varint32_pack_size(TAG_get_locker_address_resp, response->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_get_response_ACK_resp, response->Response_ACK, false);

	return pack_size;
}

static int epb_remote_get_config_response_pack(RemoteGetConfigResp *response, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_get_config_base_response, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_get_locker_address_resp, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_get_response_ACK_resp, response->Response_ACK);
	if (ret < 0) return ret;

	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				remote_get_config_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_get_config_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
//		uint8_t				l_arrBcdtime[8],idata[32];
		int16_t				dataLength;
    uint32_t 			res;
//    int64_t				random_val;
    
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
		RemoteGetConfigResp	*pRemoteGetConfigResp	= (RemoteGetConfigResp*)args;
		struct VirtualSocket		*pVirtualSocket				=	&mVirtualSocket;
		
//		RemoteGetConfigReq		*pRemoteGetConfigReq		=	&mRemoteGetConfigReq;
//		pRemoteGetConfigResp->base_response		=	NULL;
//		pRemoteGetConfigResp->Locker_Address		=	pRemoteGetConfigReq->Locker_Address;
//		pRemoteGetConfigResp->Response_ACK			=	0x011;
	
        dataLength = epb_remote_get_cofig_response_pack_size(pRemoteGetConfigResp);
        res	=	epb_remote_get_config_response_pack(pRemoteGetConfigResp, databuf, dataLength);
        printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0102;
		fix_head->WorkType			=	remote_get_config_cmd;
		fix_head->WorkIndex			=	0xFF;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
		
}

/*
*							remote get config result request pack
**************************************************************************/
#define			TAG_remote_get_config_Result_request    							0x0a
#define			TAG_get_locker_address_Result													0x10
#define			TAG_get_last_work_type_Result													0x18
#define			TAG_get_control_type_result														0x20
#define 		TAG_get_locker_ACK_Result															0x28
#define 		TAG_get_report_Time_Result														0x32
#define 		TAG_get_data_type_Result															0x38
#define 		TAG_get_data_context_Result														0x42

static int epb_remote_get_config_result_pack_size(RemoteGetConfigResultReq *request)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_remote_get_config_Result_request, len);
	pack_size += epb_varint32_pack_size(TAG_get_locker_address_Result, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_get_last_work_type_Result, request->Last_Work_Type, false);
	pack_size += epb_varint32_pack_size(TAG_get_control_type_result, request->Control_type, false);
	pack_size += epb_varint32_pack_size(TAG_get_locker_ACK_Result, request->Locker_ACK, false);
	pack_size += epb_length_delimited_pack_size(TAG_get_report_Time_Result, request->Report_Time.len);
	
	pack_size += epb_varint32_pack_size(TAG_get_data_type_Result, request->data_type, false);
	pack_size += epb_length_delimited_pack_size(TAG_get_data_context_Result, request->data_context.len);
	return pack_size;
}

static int epb_remote_get_config_result_pack_request(RemoteGetConfigResultReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_get_config_Result_request, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_get_locker_address_Result, request->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_get_last_work_type_Result, request->Last_Work_Type);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_get_control_type_result, request->Control_type);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_get_locker_ACK_Result, request->Locker_ACK);
	if (ret < 0) return ret;
		
	ret = epb_set_string(&epb, TAG_get_report_Time_Result, request->Report_Time.str, request->Report_Time.len);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_get_data_type_Result, request->data_type);
	if (ret < 0) return ret;
    ret = epb_set_bytes(&epb, TAG_get_data_context_Result, request->data_context.data, request->data_context.len);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}


/***********************************************************************
 * @brief				remote_get_config_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_get_config_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];

		int16_t				dataLength;
    uint32_t 			res;
  // int64_t				random_val;
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
		RemoteGetConfigResultReq		*pRemoteGetConfigResultReq	= (RemoteGetConfigResultReq*)args;
    struct VirtualSocket  *pVirtualSocket				=	&mVirtualSocket;
    
		pRemoteGetConfigResultReq->Last_Work_Type		=	remote_get_config_cmd;

		dataLength = epb_remote_get_config_result_pack_size(pRemoteGetConfigResultReq);
		res	=	epb_remote_get_config_result_pack_request(pRemoteGetConfigResultReq, databuf, dataLength);
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
						epb_remote_get_config_result_response_unpack
***************************************************************************/
#define			TAG_remote_get_config_result_base_response						0x0a
#define			TAG_get_locker_address_result_resp										0x10
#define			TAG_get_last_work_type_result_resp										0x18
#define			TAG_get_curr_states_Result_resp												0x20
#define 		TAG_get_random_result_resp														0x2a

RemoteGetConfigResultResp *epb_remote_get_config_result_response_unpack(RemoteGetConfigResultResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_get_config_result_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_get_config_result_base_response, &tmp_len);
	response->base_response = epb_unpack_base_response(response->base_response,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_get_locker_address_result_resp)){
        response->Locker_Address = epb_get_uint32(&epb, TAG_get_locker_address_result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_get_last_work_type_result_resp)){
        response->Last_Work_Type = epb_get_uint32(&epb, TAG_get_last_work_type_result_resp);
	}
	
    if (epb_has_tag(&epb, TAG_get_curr_states_Result_resp)){
        response->curr_states = epb_get_uint32(&epb, TAG_get_curr_states_Result_resp);
	}
	
	if (epb_has_tag(&epb, TAG_get_random_result_resp)) {
        str = epb_get_bytes(&epb, TAG_get_random_result_resp, &response->random.len);
		memcpy((void*)response->random.data,str,response->random.len);
	}

	return response;
}

