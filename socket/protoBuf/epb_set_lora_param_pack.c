#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include		"epb_set_lora_param_pack.h"
#include    "socket/VirtualPortMonitor.h"
#include 		"socket/pub/util_date.h"
#include    "uart/Cmd/remote_open_pack.h"

//--------------			public			-------------------
		//SetLoraParamReq				mSetLoraParamReq;
		//SetLoraParamResp			mSetLoraParamResp;
		//SetLoraParaResultReq	mSetLoraParaResultReq;
		//SetLoraParaResultResp	mSetLoraParaResultResp;
		
//=====================  SetLoraParam Commad request unpack =======================================
#define TAG_SetLoraParam_BaseRequest														0x0a
#define TAG_Channel_number																			0x10
#define TAG_Control_type																				0x18
#define TAG_lora_Value																					0x22

SetLoraParamReq *set_lora_param_unpack_Req(SetLoraParamReq *request,const uint8_t *buf, uint16_t buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	request->loraValue.data	= request->mloraValBuf;
	epb_unpack_init(&epb, buf, buf_len);
	
	if (!epb_has_tag(&epb, TAG_SetLoraParam_BaseRequest)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_SetLoraParam_BaseRequest, &tmp_len);

	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Channel_number)) {
        request->Channel_number = epb_get_uint32(&epb, TAG_Channel_number);
	}
	if (epb_has_tag(&epb, TAG_Control_type)) {
        request->Control_type = epb_get_uint32(&epb, TAG_Control_type);
	}
	
	if (epb_has_tag(&epb, TAG_lora_Value)) {
		str = epb_get_bytes(&epb, TAG_lora_Value,&request->loraValue.len);
		if (str != NULL ){
      request->has_loraValue = 1;
			memcpy((void*)request->loraValue.data,str,request->loraValue.len);

		}else{
            request->has_loraValue = 0;
			request->loraValue.len = 0;
		}
	}

	return request;
}

//===========================  SetLoraParam cmd response pack ===================================================
#define TAG_SetLoraParam_BaseResponse											0x0a
#define TAG_set_States_Resp												  			0x10

int epb_set_lora_response_pack_size(SetLoraParamResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_SetLoraParam_BaseResponse, len);
	pack_size += epb_varint32_pack_size(TAG_set_States_Resp, response->set_States, false);

	return pack_size;
}

int32_t epb_pack_set_lora_response(SetLoraParamResp *response, uint8_t *buf, int16_t buf_len)
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
	ret = epb_set_message(&epb, TAG_SetLoraParam_BaseResponse, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_set_States_Resp, response->set_States);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				set_lora_param_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 **********************************************************************/

void set_lora_param_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  databuf[128],header[64];
		int16_t	dataLength;

    NSFPFixHead*	fix_head = (NSFPFixHead*)header;

		SetLoraParamResp*	pSetLoraParamResp	= (SetLoraParamResp*)args;
		pSetLoraParamResp->base_response			=	NULL;

		dataLength = epb_set_lora_response_pack_size(pSetLoraParamResp);
    epb_pack_set_lora_response(pSetLoraParamResp, databuf, dataLength);
    
		fix_head->CSN						=	mVirtualSocket.Local_CSN;        
		fix_head->WorkType			=	set_lora_param_cmd;
		fix_head->WorkIndex			=	0xff;
		fix_head->FrameTotal		=	0	;
		fix_head->FrameSequ			=	0	;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
    socket_data_pack_func(&header,r_data,r_len);
}

/*
*							set_lora_para_result_pack_size
**************************************************************************/
#define			TAG_set_lora_Result_request    										0x0a
#define			TAG_Locker_Address_Result													0x10
#define			TAG_Last_Work_Type_Result													0x18		//00011 000
#define			TAG_Locker_Status_Result													0x20		//00100 000
#define 		TAG_Locker_ACK_Result															0x28		//00101 000
#define 		TAG_Report_Time_Result														0x32		//00110 010
#define 		TAG_Read_context_result														0x3a

int set_lora_para_result_pack_size(SetLoraParaResultReq *request)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_set_lora_Result_request, len);

	pack_size += epb_varint32_pack_size(TAG_Locker_Address_Result, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type_Result, request->Last_Work_Type, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_Status_Result, request->Locker_Status, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_ACK_Result, request->Locker_ACK, false);

	pack_size += epb_length_delimited_pack_size(TAG_Report_Time_Result, request->Report_Time.len);
	pack_size += epb_length_delimited_pack_size(TAG_Read_context_result, request->Read_context.len);
	return pack_size;
}


int set_lora_para_result_pack_request(SetLoraParaResultReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_set_lora_Result_request, tmp, tmp_len);
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
    ret = epb_set_bytes(&epb, TAG_Read_context_result, request->Read_context.data, request->Read_context.len);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				set_lora_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void set_lora_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
    //uint8_t				l_arrBcdtime[8];
		int16_t				dataLength;
    uint32_t 			res;
   // int64_t				random_val;
    
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
    SetLoraParaResultReq		*pSetLoraParaResultReq	= (SetLoraParaResultReq*)args;
    struct VirtualSocket    *pVirtualSocket				=	&mVirtualSocket;

//		SetLoraParaResultResp			*pSetLoraParaResultResp		=	&mSetLoraParaResultResp;
//		pSetLoraParaResultReq->base_request			=	NULL;
//		pSetLoraParaResultReq->Locker_Address		=	0x0000;
//		pSetLoraParaResultReq->Last_Work_Type		=	set_lora_param_cmd;
//		pSetLoraParaResultReq->Locker_Status		=	0x00;
//    pSetLoraParaResultReq->Locker_ACK		=	pSetLoraParaResultResp->state;
//    Get_CurBCDTime7((char*)l_arrBcdtime);
//    pSetLoraParaResultReq->Report_Time.str	=	(char*)l_arrBcdtime;
//    pSetLoraParaResultReq->Report_Time.len	=	sizeof(l_arrBcdtime);

		dataLength = set_lora_para_result_pack_size(pSetLoraParaResultReq);
		res	=	set_lora_para_result_pack_request(pSetLoraParaResultReq, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0101;
    fix_head->WorkType			=	report_log_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
}

/*
 *					set_lora_para_result_unpack_response
***************************************************************************/
#define			TAG_set_lora_result_base_response									0x0a
#define			TAG_Locker_Address_result_resp										0x10
#define			TAG_Last_Work_Type_result_resp										0x18
#define			TAG_state_Result_resp															0x20
#define 		TAG_random_result_resp														0x2a

SetLoraParaResultResp *set_lora_para_result_unpack_response(SetLoraParaResultResp *response,const uint8_t *buf, uint16_t buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_set_lora_result_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_set_lora_result_base_response, &tmp_len);
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
