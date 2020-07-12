#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include    "socket/VirtualPortMonitor.h"
#include		"epb_emergency_open_pack.h"

//--------------			public			-------------------
		//EmergencyOpenReq			mEmergencyOpenReq;
		//EmergencyOpenResp			mEmergencyOpenResp;
		
//=====================  emergency Commad request unpack =======================================
#define TAG_emergency_BaseRequest														0x0a
#define TAG_Locker_Address																	0x10
#define TAG_Channel_number																	0x18
#define TAG_random_number																		0x22 	

EmergencyOpenReq *epb_emergency_open_unpack_Req(EmergencyOpenReq *request,const uint8_t *buf, int16_t buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	uint8_t	random[8];
	request->random.data	= random;
	epb_unpack_init(&epb, buf, buf_len);
	
	if (!epb_has_tag(&epb, TAG_emergency_BaseRequest)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_emergency_BaseRequest, &tmp_len);

	request->base_request = epb_unpack_base_request(request->base_request,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Locker_Address)){
		request->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address);
	}
	if (epb_has_tag(&epb, TAG_Channel_number)) {
        request->Control_state = epb_get_uint32(&epb, TAG_Channel_number);
	}
	
	if (epb_has_tag(&epb, TAG_random_number)) {
		str = epb_get_bytes(&epb, TAG_random_number,&request->random.len);
		memcpy((void*)request->random.data,str,request->random.len);
	}
	
	return request;
}



//===========================  emergency open cmd response pack ===================================================
#define TAG_emergency_open_BaseResponse											0x0a
#define TAG_Locker_Address_Resp															0x10
#define TAG_Response_ACK_Resp												  			0x18


int epb_emergency_open_response_pack_size(EmergencyOpenResp *response)
{
	int pack_size = 0;
	int len = 0;
	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_emergency_open_BaseResponse, len);
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_Resp, response->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Response_ACK_Resp, response->Response_ACK,false);
	return pack_size;
}

int32_t epb_pack_emergency_open_response(EmergencyOpenResp *response, uint8_t *buf, int16_t buf_len)
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
	ret = epb_set_message(&epb, TAG_emergency_open_BaseResponse, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;

	ret = epb_set_int32(&epb, TAG_Locker_Address_Resp, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Response_ACK_Resp, response->Response_ACK);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}


/***********************************************************************
 * @brief				emergency_open_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 **********************************************************************/

void emergency_open_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
        uint8_t  databuf[128],header[64];
//		uint8_t				*databuf,*header;
//		databuf	=	(uint8_t*)malloc(128);
//		header	=	(uint8_t*)malloc(64);	
		
		uint8_t 	idata[8];
		int16_t	dataLength;
    uint32_t 								res;
    BaseResponse 		iBaseResponse;
    iBaseResponse.err_msg.str		=	(char*)idata;
    iBaseResponse.err_msg.len		=	0x0;
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;

		EmergencyOpenResp*	pEmergencyOpenResp	= (EmergencyOpenResp*)args;
		pEmergencyOpenResp->base_response			=	&iBaseResponse;

		dataLength = epb_emergency_open_response_pack_size(pEmergencyOpenResp);
		res	=	epb_pack_emergency_open_response(pEmergencyOpenResp, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);

    fix_head->CSN				=	mVirtualSocket.Local_CSN;
		fix_head->WorkType			=	emergency_open_door;
		fix_head->WorkIndex			=	0xff;
		fix_head->FrameTotal		=	0	;
		fix_head->FrameSequ			=	0	;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
//		free(databuf);
//		free(header);
}

