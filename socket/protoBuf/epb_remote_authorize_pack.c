
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include    "epb_remote_authorize_pack.h"
#include    "socket/VirtualPortMonitor.h"

#include    "uart/Cmd/remote_authorize_pack.h"

#define         authorize_data_SIZE     255
//uint8_t				List_DataContext[4][authorize_data_SIZE];				//hold place symble

//RemoteAutorizeReq					mRemoteAutorizeReq;
//RemoteAuthorizeResp				mRemoteAuthorizeResp;
//RemoteAuthorizeResultReq	mRemoteAuthorizeResultReq;
//RemoteAuthorizeResultResp	mRemoteAuthorizeResultResp;




//============= User authorize information unpack ==============================
#define TAG_user_control_list										0x08
#define TAG_dataType_1										0x10
#define TAG_Data_context_1											0x1a
#define TAG_dataType_2										0x20
#define TAG_Data_context_2											0x2a
#define TAG_dataType_3										0x30
#define TAG_Data_context_3											0x3a
#define TAG_dataType_4										0x40
#define TAG_Data_context_4											0x4a

User_authorize	*epb_user_authorize_context_unpack(User_authorize *UserAuthorizeContext, uint8_t *buf, int buf_len)
{
	Epb epb;
	int ret;
	const unsigned char *str;
	
	epb_unpack_init(&epb, buf, buf_len);

	if (epb_has_tag(&epb, TAG_user_control_list)){
		UserAuthorizeContext->user_control_list = epb_get_uint32(&epb, TAG_user_control_list);
	}
		
    if (epb_has_tag(&epb, TAG_dataType_1)){
		UserAuthorizeContext->dataType_1 = epb_get_uint32(&epb, TAG_dataType_1);
	}
	if (epb_has_tag(&epb, TAG_Data_context_1)) {
		str = epb_get_bytes(&epb, TAG_Data_context_1, &UserAuthorizeContext->Data_context_1.len);
        ret =  authorize_data_SIZE;
        if(UserAuthorizeContext->Data_context_1.len > ret)
            UserAuthorizeContext->Data_context_1.len =	authorize_data_SIZE;
		memcpy((void*)UserAuthorizeContext->Data_context_1.data,str,UserAuthorizeContext->Data_context_1.len);
	}

	if (epb_has_tag(&epb, TAG_dataType_2)){
		UserAuthorizeContext->dataType_2 = epb_get_uint32(&epb, TAG_dataType_2);
	}
    if (epb_has_tag(&epb, TAG_Data_context_2)) {
		str = epb_get_bytes(&epb, TAG_Data_context_2, &UserAuthorizeContext->Data_context_2.len);
        ret =  authorize_data_SIZE;
        if(UserAuthorizeContext->Data_context_2.len > ret)
            UserAuthorizeContext->Data_context_2.len =	authorize_data_SIZE;
		memcpy((void*)UserAuthorizeContext->Data_context_2.data,str,UserAuthorizeContext->Data_context_2.len);
	}	
	
	if (epb_has_tag(&epb, TAG_dataType_3)){
		UserAuthorizeContext->dataType_3 = epb_get_uint32(&epb, TAG_dataType_3);
	}
	if (epb_has_tag(&epb, TAG_Data_context_3)) {
		str = epb_get_bytes(&epb, TAG_Data_context_3, &UserAuthorizeContext->Data_context_3.len);
        ret =  authorize_data_SIZE;
        if(UserAuthorizeContext->Data_context_3.len > ret)
            UserAuthorizeContext->Data_context_3.len =	authorize_data_SIZE;
		memcpy((void*)UserAuthorizeContext->Data_context_3.data,str,UserAuthorizeContext->Data_context_3.len);
	}	

	if (epb_has_tag(&epb, TAG_dataType_4)){
		UserAuthorizeContext->dataType_4 = epb_get_uint32(&epb, TAG_dataType_4);
	}
	if (epb_has_tag(&epb, TAG_Data_context_4)) {
		str = epb_get_bytes(&epb, TAG_Data_context_4, &UserAuthorizeContext->Data_context_4.len);
        ret =  authorize_data_SIZE;
        if(UserAuthorizeContext->Data_context_4.len > ret)
            UserAuthorizeContext->Data_context_4.len =	authorize_data_SIZE;
		memcpy((void*)UserAuthorizeContext->Data_context_4.data,str,UserAuthorizeContext->Data_context_4.len);
	}
	return UserAuthorizeContext;
}

//======================== remote authorize request unpack  ============================================
#define			TAG_remote_authorize_request    					0x0a
#define			TAG_Locker_Address												0x10
#define			TAG_Channel_number												0x18
#define			TAG_control_type													0x20
#define			TAG_User_authorize												0x2a
//#define			TAG_User_authorize_1											0x2a
//#define			TAG_User_authorize_2											0x32
//#define			TAG_User_authorize_3											0x3a
//#define			TAG_User_authorize_4											0x42
//#define			TAG_User_authorize_5											0x4a

RemoteAutorizeReq *epb_remote_authorzie_request_unpack(RemoteAutorizeReq *request,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
    char *str;
   // memset(List_DataContext,0,sizeof(List_DataContext));

    request->iUser_authorize.Data_context_1.data	=	(uint8_t*)request->m_data_context_1;
    request->iUser_authorize.Data_context_2.data	=	(uint8_t*)request->m_data_context_2;
    request->iUser_authorize.Data_context_3.data	=	(uint8_t*)request->m_data_context_3;
    request->iUser_authorize.Data_context_4.data	=	(uint8_t*)request->m_data_context_4;
    

	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_authorize_request)) {
		return NULL;
	}
	
	tmp = epb_get_message(&epb, TAG_remote_authorize_request, &tmp_len);
	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Locker_Address)){
		request->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address);
	}
	if (epb_has_tag(&epb, TAG_Channel_number)){
		request->Channel_number = epb_get_uint32(&epb, TAG_Channel_number);
	}
	if (epb_has_tag(&epb, TAG_control_type)){
		request->iControlType = epb_get_uint32(&epb, TAG_control_type);
	}
	if (epb_has_tag(&epb, TAG_User_authorize)) {
        str = (char*)epb_get_message(&epb, TAG_User_authorize, &tmp_len);
        epb_user_authorize_context_unpack(&request->iUser_authorize,(uint8_t*)str,tmp_len);
	}
	/*
	if (epb_has_tag(&epb, TAG_User_authorize_2)) {
		str = epb_get_bytes(&epb, TAG_User_authorize_2, &tmp_len);
		epb_user_authorize_context_unpack(&request->iUser_authorize2,str,tmp_len);
 	}
	if (epb_has_tag(&epb, TAG_User_authorize_3)) {
		str = epb_get_bytes(&epb, TAG_User_authorize_3, &tmp_len);
    epb_user_authorize_context_unpack(&request->iUser_authorize3,str,tmp_len);   
	}
	if (epb_has_tag(&epb, TAG_User_authorize_4)) {
		str = epb_get_bytes(&epb, TAG_User_authorize_4, &tmp_len);
    epb_user_authorize_context_unpack(&request->iUser_authorize4,str,tmp_len);   
	}
	if (epb_has_tag(&epb, TAG_User_authorize_5)) {
		str = epb_get_bytes(&epb, TAG_User_authorize_5, &tmp_len);
    epb_user_authorize_context_unpack(&request->iUser_authorize5,str,tmp_len);   
	}
	*/
	return request;
}


//========================= remote authorize response pack ==========================
#define			TAG_remote_authorize_base_response					0x0a
#define			TAG_Locker_Address_resp											0x10
#define			TAG_Response_ACK_resp												0x18

int epb_remote_authorize_response_pack_size(RemoteAuthorizeResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_remote_authorize_base_response, len);
	
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_resp, response->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Response_ACK_resp, response->Response_ACK, false);

	return pack_size;
}

int epb_remote_authorize_response_pack(RemoteAuthorizeResp *response, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_authorize_base_response, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_Locker_Address, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Channel_number, response->Response_ACK);
	if (ret < 0) return ret;

	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				remote_authorize_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_authorize_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
		uint8_t				idata[32];
		int16_t				dataLength;
    uint32_t 			res;
//    int64_t				random_val;
    BaseResponse 		iBaseResponse;
    iBaseResponse.err_msg.str		=	(char*)idata;
    iBaseResponse.err_msg.len		=	0x0;
        
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
		RemoteAuthorizeResp	*pRemoteAuthorizeResp	= (RemoteAuthorizeResp*)args;
//		RemoteAutorizeReq		*pRemoteAutorizeReq		=	&mRemoteAutorizeReq;
        struct VirtualSocket		*pVirtualSocket		=	&mVirtualSocket;
		
		pRemoteAuthorizeResp->base_response			=	&iBaseResponse;
//		pRemoteAuthorizeResp->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
//		pRemoteAuthorizeResp->Response_ACK			=	0x011;
	
    dataLength = epb_remote_authorize_response_pack_size(pRemoteAuthorizeResp);
    res	=	epb_remote_authorize_response_pack(pRemoteAuthorizeResp, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
		
		fix_head->CSN						=	pVirtualSocket->Local_CSN;
		fix_head->State					= 0x0102;
		fix_head->WorkType			=	remote_authorize_cmd;
		fix_head->WorkIndex			=	0xFF;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);

}

/*
*							remote authorize result request pack
**************************************************************************/
#define			TAG_remote_authorize_Result_request    						0x0a
#define			TAG_Locker_Address_Result													0x10
#define			TAG_Last_Work_Type_Result													0x18
#define			TAG_Locker_Status_result													0x20
#define 		TAG_Locker_ACK_Result															0x28
#define 		TAG_Report_Time_Result														0x32

int epb_remote_authorize_result_pack_size(RemoteAuthorizeResultReq *request)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_remote_authorize_Result_request, len);
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_Result, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type_Result, request->Last_Work_Type, false);
	
  pack_size += epb_varint32_pack_size(TAG_Locker_Status_result, request->Locker_Status, false);	
	pack_size += epb_varint32_pack_size(TAG_Locker_ACK_Result, request->Locker_ACK, false);  
	pack_size += epb_length_delimited_pack_size(TAG_Report_Time_Result, request->Report_Time.len);	
	return pack_size;
}

int epb_remote_authorize_result_pack_request(RemoteAuthorizeResultReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, TAG_remote_authorize_Result_request, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_Locker_Address_Result, request->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Last_Work_Type_Result, request->Last_Work_Type);
	if (ret < 0) return ret;
    ret = epb_set_int32(&epb, TAG_Locker_Status_result, request->Locker_Status);
	if (ret < 0) return ret;		
	ret = epb_set_int32(&epb, TAG_Locker_ACK_Result, request->Locker_ACK);
	if (ret < 0) return ret;		
	ret = epb_set_string(&epb, TAG_Report_Time_Result, request->Report_Time.str, request->Report_Time.len);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}


/***********************************************************************
 * @brief				remote_authorize_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/

void remote_authorize_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		uint8_t  			databuf[128],header[64];
//    uint8_t				l_arrBcdtime[8];
		int16_t				dataLength;
    uint32_t 			res;
 //   int64_t				random_val;
    
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
		RemoteAuthorizeResultReq		*pRemoteAuthorizeResultReq	= (RemoteAuthorizeResultReq*)args;
    struct VirtualSocket       	*pVirtualSocket				=	&mVirtualSocket;
    
//    RemoteAutorizeReq					*pRemoteAutorizeReq				=	&mRemoteAutorizeReq;
//		URemoteAuthorizeResp			*pURemoteAuthorizeResp		=	&mURemoteAuthorizeResp;
//		pRemoteAuthorizeResultReq->base_request			=	NULL;
//		pRemoteAuthorizeResultReq->Locker_Address		=	pRemoteAutorizeReq->Locker_Address;
//    pRemoteAuthorizeResultReq->Last_Work_Type		=	remote_authorize_cmd;
//		pRemoteAuthorizeResultReq->Locker_ACK				=	0x6902;
//		
//    pRemoteAuthorizeResultReq->Report_Time.str		=	(char*)l_arrBcdtime;
//    memcpy((char*)pRemoteAuthorizeResultReq->Report_Time.str,pURemoteAuthorizeResp->ProtoTime,sizeof(pURemoteAuthorizeResp->ProtoTime));
//    pRemoteAuthorizeResultReq->Report_Time.len	=	sizeof(pURemoteAuthorizeResp->ProtoTime);

	
		dataLength = epb_remote_authorize_result_pack_size(pRemoteAuthorizeResultReq);
		res	=	epb_remote_authorize_result_pack_request(pRemoteAuthorizeResultReq, databuf, dataLength);
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
						epb_remote_authorize_result_response_unpack
***************************************************************************/
#define			TAG_remote_authorize_result_base_response					0x0a
#define			TAG_Locker_Address_result_resp										0x10
#define			TAG_Last_Work_Type_result_resp										0x18
#define			TAG_curr_states_Result_resp												0x20
#define 		TAG_random_result_resp														0x2a

RemoteAuthorizeResultResp *epb_remote_config_result_unpack_response(RemoteAuthorizeResultResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	epb_unpack_init(&epb, buf, buf_len);
	if (!epb_has_tag(&epb, TAG_remote_authorize_result_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_authorize_result_base_response, &tmp_len);
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
