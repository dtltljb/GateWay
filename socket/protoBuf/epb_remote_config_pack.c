#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include 		"epb_remote_config_pack.h"
#include    "socket/VirtualPortMonitor.h"

#include    "uart/Cmd/remote_config_pack.h"

//static	uint8_t									ConfigDataGroup[3][240];

//RemoteConfigReq									mRemoteConfigReq;
//RemoteConfigResp								mRemoteConfigResp;
//RemoteConfigResultReq						mRemoteConfigResultReq;
//RemoteConfigResultResp					mRemoteConfigResultResp;

//======================== remote config request unpack ============================================
#define			TAG_remote_config_request    							0x0a
#define			TAG_Locker_Address												0x10
#define			TAG_Channel_number												0x18
#define			TAG_control_type													0x20
#define			TAG_first_data_type												0x28
#define			TAG_first_data_group											0x32
#define			TAG_second_data_type											0x38
#define			TAG_second_data_group											0x42
#define			TAG_third_data_type												0x48
#define			TAG_third_data_group											0x52

RemoteConfigReq *epb_remote_config_request_unpack(RemoteConfigReq *request,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	
    request->first_data_group.data		=	(uint8_t*)request->data_group_1;
    request->second_data_group.data		=	(uint8_t*)request->data_group_2;
    request->third_data_group.data		=	(uint8_t*)request->data_group_3;

	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_config_request)) {
		return NULL;
	}
	
	tmp = epb_get_message(&epb, TAG_remote_config_request, &tmp_len);
	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Locker_Address)){
		request->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address);
	}
	if (epb_has_tag(&epb, TAG_Channel_number)){
		request->Channel_number = epb_get_uint32(&epb, TAG_Channel_number);
	}
	if (epb_has_tag(&epb, TAG_control_type)){
		request->iControl_type = epb_get_uint32(&epb, TAG_control_type);
	}
	
	if (epb_has_tag(&epb, TAG_first_data_type)){
		request->first_data_type = epb_get_uint32(&epb, TAG_first_data_type);
	}
	if (epb_has_tag(&epb, TAG_first_data_group)) {
		str = epb_get_bytes(&epb, TAG_first_data_group, &request->first_data_group.len);
		memcpy((void*)request->first_data_group.data,str,request->first_data_group.len);
	}	
	
	if (epb_has_tag(&epb, TAG_second_data_type)){
		request->second_data_type = epb_get_uint32(&epb, TAG_second_data_type);
	}
	if (epb_has_tag(&epb, TAG_second_data_group)) {
		str = epb_get_bytes(&epb, TAG_second_data_group, &request->second_data_group.len);
		memcpy((void*)request->second_data_group.data,str,request->second_data_group.len);
	}
	
	if (epb_has_tag(&epb, TAG_third_data_type)){
		request->third_data_type = epb_get_uint32(&epb, TAG_third_data_type);
	}
	if (epb_has_tag(&epb, TAG_third_data_group)) {
		str = epb_get_bytes(&epb, TAG_third_data_group, &request->third_data_group.len);
		memcpy((void*)request->third_data_group.data,str,request->third_data_group.len);
	}
	return request;
}

//========================= remote config response pack ==========================
#define			TAG_remote_config_base_response							0x0a
#define			TAG_Locker_Address_resp											0x10
#define			TAG_Response_ACK_resp												0x18

int epb_remote_cofig_response_pack_size(RemoteConfigResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_remote_config_base_response, len);
	
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_resp, response->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Response_ACK_resp, response->Response_ACK, false);

	return pack_size;
}

int epb_remote_config_response_pack(RemoteConfigResp *response, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_config_base_response, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_Locker_Address_resp, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Response_ACK_resp, response->Response_ACK);
	if (ret < 0) return ret;

	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				remote_config_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_config_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
		int16_t				dataLength;
    uint32_t 			res;
  
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    RemoteConfigResp	*pRemoteConfigResp	= (RemoteConfigResp*)args;
		struct VirtualSocket		*pVirtualSocket				=	&mVirtualSocket;

//		RemoteConfigReq		*pRemoteConfigReq		=	&mRemoteConfigReq;		
//		pRemoteConfigResp->base_response		=	NULL;
//		pRemoteConfigResp->Locker_Address		=	pRemoteConfigReq->Locker_Address;
//		pRemoteConfigResp->Response_ACK			=	0x011;
	
    dataLength = epb_remote_cofig_response_pack_size(pRemoteConfigResp);
    res	=	epb_remote_config_response_pack(pRemoteConfigResp, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0102;
		fix_head->WorkType			=	remote_config_cmd;
		fix_head->WorkIndex			=	0xFF;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
		
}

/*
*							remote config result request pack
**************************************************************************/
#define			TAG_remote_config_Result_request    							0x0a
#define			TAG_Locker_Address_Result													0x10
#define			TAG_Last_Work_Type_Result													0x18
#define			TAG_Control_type_result														0x20
#define 		TAG_Locker_ACK_Result															0x28
#define 		TAG_Report_Time_Result														0x32

int epb_remote_config_result_pack_size(RemoteConfigResultReq *request)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_remote_config_Result_request, len);
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_Result, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type_Result, request->Last_Work_Type, false);
	pack_size += epb_varint32_pack_size(TAG_Control_type_result, request->Control_type, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_ACK_Result, request->Locker_ACK, false);
	
	pack_size += epb_length_delimited_pack_size(TAG_Report_Time_Result, request->Report_Time.len);	
	return pack_size;
}

int epb_remote_config_result_pack_request(RemoteConfigResultReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_config_Result_request, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Locker_Address_Result, request->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Last_Work_Type_Result, request->Last_Work_Type);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Control_type_result, request->Control_type);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Locker_ACK_Result, request->Locker_ACK);
	if (ret < 0) return ret;
		
	ret = epb_set_string(&epb, TAG_Report_Time_Result, request->Report_Time.str, request->Report_Time.len);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}


/***********************************************************************
 * @brief				remote_config_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_config_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
    //uint8_t				l_arrBcdtime[8];
		int16_t				dataLength;
    uint32_t 			res;
   // int64_t				random_val;
    
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
		RemoteConfigResultReq		*pRemoteConfigResultReq	= (RemoteConfigResultReq*)args;
    struct VirtualSocket       	*pVirtualSocket				=	&mVirtualSocket;
    
//    RemoteConfigReq				*pRemoteConfigReq			=	&mRemoteConfigReq;
//		ConfigLockerInfoResp	*pConfigLockerInfoResp		=	&mConfigLockerInfoResp;
//		pRemoteConfigResultReq->base_request			=	NULL;
//		pRemoteConfigResultReq->Locker_Address		=	pRemoteConfigReq->Locker_Address;
//		pRemoteConfigResultReq->Last_Work_Type		=	remote_config_cmd;
//		//pRemoteConfigResultReq->Locker_ACK				=	0x6902;
//    pRemoteConfigResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
//    memcpy((char*)pRemoteConfigResultReq->Report_Time.str,pConfigLockerInfoResp->ProtoTime,sizeof(pConfigLockerInfoResp->ProtoTime));
//    pRemoteConfigResultReq->Report_Time.len	=	sizeof(pConfigLockerInfoResp->ProtoTime);

	
		dataLength = epb_remote_config_result_pack_size(pRemoteConfigResultReq);
		res	=	epb_remote_config_result_pack_request(pRemoteConfigResultReq, databuf, dataLength);
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
						epb_remote_config_result_response_unpack
***************************************************************************/
#define			TAG_remote_config_result_base_response						0x0a
#define			TAG_Locker_Address_result_resp										0x10
#define			TAG_Last_Work_Type_result_resp										0x18
#define			TAG_curr_states_Result_resp												0x20
#define 		TAG_random_result_resp														0x2a

RemoteConfigResultResp *epb_remote_config_result_response_unpack(RemoteConfigResultResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_config_result_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_config_result_base_response, &tmp_len);
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

