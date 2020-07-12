#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include    "socket/pub/util.h"
#include 		"epb_report_log_pack.h"
#include    "socket/VirtualPortMonitor.h"
#include    "uart/Cmd/report_log_pack.h"


#define				_GNU_SOURCE					//compile evn

//----------public ------
//		ReportLogReq				mReportLogReq;
//		ReportLogResp				mReportLogResp;

//=========================== report Log context pack=========================================

#define TAG_report_Log_BaseRequest							0x0a
#define TAG_Locker_Address											0x10
#define TAG_Last_Work_Type											0x18
#define TAG_Locker_status												0x20
#define TAG_Door_Status													0x28

#define TAG_Locker_Log_Time											0x32
#define TAG_iLog_Type														0x38 
#define TAG_power																0x40 //01000 000
#define TAG_Channel_number											0x48 //01001 000 
#define TAG_Data																0x52 //01010 010
#define TAG_random															0x5a //01011 010

int epb_report_log_request_pack_size(ReportLogReq *request)
{
	int pack_size = 0;
	int len = 0;

	len = epb_base_request_pack_size(request->base_request);
	pack_size += epb_length_delimited_pack_size(TAG_report_Log_BaseRequest, len);
	pack_size += epb_varint32_pack_size(TAG_Locker_Address, request->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type, request->Last_Work_Type, false);
	pack_size += epb_varint32_pack_size(TAG_Locker_status, request->iLockerStates, false);
	pack_size += epb_varint32_pack_size(TAG_Door_Status, request->Door_Status, false);
  
	pack_size += epb_length_delimited_pack_size(TAG_Locker_Log_Time, request->Locker_Log_Time.len);	
	pack_size += epb_varint32_pack_size(TAG_iLog_Type, request->iLog_Type, false);
	pack_size += epb_varint32_pack_size(TAG_power, request->power, false);		
	pack_size += epb_varint32_pack_size(TAG_Channel_number, request->Channel_number, false);
	pack_size += epb_length_delimited_pack_size(TAG_Data, request->Data.len);	
	pack_size += epb_length_delimited_pack_size(TAG_random, request->random.len);		
	
	return pack_size;
}

int epb_report_log_request_pack(ReportLogReq *request, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, (uint16_t)TAG_report_Log_BaseRequest, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Locker_Address, request->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Last_Work_Type, request->Last_Work_Type);
	if (ret < 0) return ret;
  ret = epb_set_int32(&epb, TAG_Locker_status, request->iLockerStates);
	if (ret < 0) return ret;		
	ret = epb_set_int32(&epb, TAG_Door_Status, request->Door_Status);
	if (ret < 0) return ret;

	ret = epb_set_string(&epb, TAG_Locker_Log_Time, request->Locker_Log_Time.str, request->Locker_Log_Time.len);
	if (ret < 0) return ret;

	ret = epb_set_enum(&epb, TAG_iLog_Type, request->iLog_Type);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_power, request->power);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Channel_number, request->Channel_number);
	if (ret < 0) return ret;
	ret = epb_set_bytes(&epb, TAG_Data, request->Data.data, request->Data.len);
	if (ret < 0) return ret;		
	ret = epb_set_bytes(&epb, TAG_random, request->random.data, request->random.len);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}


/***********************************************************************
 * @brief				report_log_request_pack
 * @param[in]		void *args, uint8_t **r_data, int16_t *r_len
 * @return 			void
 **********************************************************************/

void report_log_request_pack(void *arg,uint8_t channel,uint8_t *r_data, uint16_t *r_len)
{
    uint8_t  			databuf[255],header[64];
    uint8_t				l_arrBcdtime[8],randomBuf[8];
		int16_t				dataLength;
    uint32_t 			res;

    memset(header,0,sizeof(header));
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
		//ReportLogReq		*pReportLogReq	= (ReportLogReq*)args;
		ReportLogReq		*pReportLogReq,iReportLogReq;
    UReportLogReq		*pUReportLogReq	=	(UReportLogReq*)arg;
		pReportLogReq	=	&iReportLogReq;
		pReportLogReq->base_request			=	NULL;
		//pReportLogReq->Locker_Address		=	pUReportLogReq->LockerNumber;
		pReportLogReq->Locker_Address		=	t_ntohl(pUReportLogReq->LockerNumber);	//swap address 2019-5-22
		
		pReportLogReq->Last_Work_Type		=	report_log_cmd;
		pReportLogReq->iLog_Type				=	pUReportLogReq->LogType;
		pReportLogReq->iLockerStates		=	pUReportLogReq->LockerStates;
		pReportLogReq->Door_Status			=	0;		
		pReportLogReq->power						=	pUReportLogReq->LockerPower;
    pReportLogReq->Channel_number       =   channel;
    pReportLogReq->Locker_Log_Time.str	=	(char*)l_arrBcdtime;
		pReportLogReq->Locker_Log_Time.len	=	sizeof(pUReportLogReq->LogTime);
    memcpy((char*)pReportLogReq->Locker_Log_Time.str,pUReportLogReq->LogTime,sizeof(pUReportLogReq->LogTime));
    switch(pReportLogReq->iLog_Type)
    {
                case	LockerRegister:
                    pReportLogReq->Data.data						=	pUReportLogReq->InfoEntity;
                    pReportLogReq->Data.len							=	04;
                    memcpy(pReportLogReq->Data.data,pUReportLogReq->InfoEntity,4);
                    break;
                case	FingerVena:
                case	SwingCard:
                case	Password:
                    pReportLogReq->Data.data						=	pUReportLogReq->InfoEntity;
                    pReportLogReq->Data.len							=	4;
                    memcpy(pReportLogReq->Data.data,pUReportLogReq->InfoEntity,4);
                    break;
                case	IdentityCard:
                default:
                    pReportLogReq->Data.data						=	pUReportLogReq->InfoEntity;
                    pReportLogReq->Data.len							=	sizeof(pUReportLogReq->InfoEntity);
                    memcpy(pReportLogReq->Data.data,pUReportLogReq->InfoEntity,sizeof(pUReportLogReq->InfoEntity));
                    break;
    }
    pReportLogReq->random.data					=	pUReportLogReq->RandomNumber;
		pReportLogReq->random.len						=	sizeof(randomBuf);
		memcpy(pReportLogReq->random.data,pUReportLogReq->RandomNumber,sizeof(randomBuf));
		
		dataLength = epb_report_log_request_pack_size(pReportLogReq);
		res	=	epb_report_log_request_pack(pReportLogReq, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
    fix_head->CSN						=	mVirtualSocket.Local_CSN;;
		fix_head->State					= 0x0102;
    fix_head->WorkType			=	report_log_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf;
		res = socket_data_pack_func(&header,r_data,r_len);
}

/***********************************************************************
 * @brief				register_log_request_pack
 * @param[in]		void *args, uint8_t **r_data, int16_t *r_len
 * @return 			void
 **********************************************************************/

void register_log_request_pack(void *arg,uint8_t channel,uint8_t *r_data, uint16_t *r_len)
{
    uint8_t  			databuf[255],header[64];
//    uint8_t				l_arrBcdtime[8],randomBuf[8];
		int16_t				dataLength;
    uint32_t 			res;

    memset(header,0,sizeof(header));
    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
    
		ReportLogReq		*pReportLogReq	= (ReportLogReq*)arg;
		//ReportLogReq		*pReportLogReq,iReportLogReq;
    
		dataLength = epb_report_log_request_pack_size(pReportLogReq);
		res	=	epb_report_log_request_pack(pReportLogReq, databuf, dataLength);
    printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);
    fix_head->CSN						=	mVirtualSocket.Local_CSN;;
		fix_head->State					= 0x0102;
    fix_head->WorkType			=	report_log_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf;
		res = socket_data_pack_func(&header,r_data,r_len);
}

/*
*						epb_report_log_response_unpack
***************************************************************************/
#define			TAG_remote_Log_base_response							0x0a
#define			TAG_Locker_Address_resp										0x10
#define			TAG_Last_Work_Type_resp										0x18
#define			TAG_Locker_States_resp										0x20
#define 		TAG_random_resp														0x2a
#define			TAG_Channel_number_resp										0x30
#define			TAG_iLog_Type_resp												0x38


ReportLogResp *epb_report_log_response_unpack(ReportLogResp *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
    //uint8_t	random[8];
    response->random.data     	=   response->entry_random; //  self memroy
	epb_unpack_init(&epb, buf, buf_len);
	
	if (!epb_has_tag(&epb, TAG_remote_Log_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_Log_base_response, &tmp_len);
	response->base_response = epb_unpack_base_response(response->base_response,tmp, tmp_len);

	if (epb_has_tag(&epb, TAG_Locker_Address_resp)){
		response->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Last_Work_Type_resp)){
		response->Last_Work_Type = epb_get_uint32(&epb, TAG_Last_Work_Type_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Locker_States_resp)){
		response->iLockerStates = epb_get_enum(&epb, TAG_Locker_States_resp);
	}	
	
	if (epb_has_tag(&epb, TAG_random_resp)) {
		str = epb_get_bytes(&epb, TAG_random_resp, &response->random.len);
		memcpy((void*)response->random.data,str,response->random.len);
	}
	
	if (epb_has_tag(&epb, TAG_iLog_Type_resp)){
		response->iLog_Type = epb_get_enum(&epb, TAG_iLog_Type_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Channel_number_resp)){
		response->Channel_number = epb_get_uint32(&epb, TAG_Channel_number_resp);
	}
	return response;
}
/*					asys receive done
*						epb_report_log_response_public_unpack
***************************************************************************/
void *epb_report_log_response_public_unpack(ReportLogRespPublic *response,const uint8_t *buf, int buf_len)
{
	Epb epb;
	const uint8_t *tmp;
	const unsigned char *str;
	int tmp_len;
	uint8_t	random[8];
	response->random.data     	=   random;
	epb_unpack_init(&epb, buf, buf_len);
	
	if (!epb_has_tag(&epb, TAG_remote_Log_base_response)) {
		return NULL;
	}
	tmp = epb_get_message(&epb, TAG_remote_Log_base_response, &tmp_len);
	response->base_response = epb_unpack_base_response(response->base_response,tmp, tmp_len);


	if (epb_has_tag(&epb, TAG_Locker_Address_resp)){
		response->Locker_Address = epb_get_uint32(&epb, TAG_Locker_Address_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Last_Work_Type_resp)){
		response->Last_Work_Type = epb_get_uint32(&epb, TAG_Last_Work_Type_resp);
	}
	
	if (epb_has_tag(&epb, TAG_Locker_States_resp)){
		response->iLockerStates = epb_get_enum(&epb, TAG_Locker_States_resp);
	}	
	
	if (epb_has_tag(&epb, TAG_random_resp)) {
		str = epb_get_bytes(&epb, TAG_random_resp, &response->random.len);
		memcpy((void*)response->random.data,str,response->random.len);
	}

	return response;
}


/***********************************************************************
 * @brief				test part  eg
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 *	eg:
 0x5a 0xa5 0x12 0x34 0x56 0x78 0x1 0x2 0x1 0x0 0x0 0x0 0x0 0x1 0x20 0x19 0x1 0x22 0x16 0x52 0x19 0x0 0x0 
 0x16 0xa  0 0x10 0xf8 0xac 0xd1 0x91 0x1 0x18 0x1 0x20 0x4 0x2a 0x8 0xbb 0x56 0x34 0x12 0x78 0x56 0x34 0xaa
 0x0  0x0 0xe 0xe0 0x15 0xd1 0xb3 0xa8  
************************************************************************/
int epb_report_log_response_pack_size(ReportLogResp *response)
{
	int pack_size = 0;
	int len = 0;

	len = epb_base_response_pack_size(response->base_response);
	pack_size += epb_length_delimited_pack_size(TAG_remote_Log_base_response, len);
	pack_size += epb_varint32_pack_size(TAG_Locker_Address_resp, response->Locker_Address, false);
	pack_size += epb_varint32_pack_size(TAG_Last_Work_Type_resp, response->Last_Work_Type, false);
	
	pack_size += epb_varint32_pack_size(TAG_iLog_Type_resp, response->iLog_Type, false);
	pack_size += epb_length_delimited_pack_size(TAG_random_resp, response->random.len);		
	return pack_size;
}

int epb_report_log_response_pack(ReportLogResp *response, uint8_t *buf, int buf_len)
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
	ret = epb_set_message(&epb, (uint16_t)TAG_remote_Log_base_response, tmp, tmp_len);
	free(tmp);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Locker_Address_resp, response->Locker_Address);
	if (ret < 0) return ret;
	ret = epb_set_int32(&epb, TAG_Last_Work_Type_resp, response->Last_Work_Type);
	if (ret < 0) return ret;
	ret = epb_set_enum(&epb, TAG_iLog_Type_resp, response->iLog_Type);
	if (ret < 0) return ret;
	ret = epb_set_bytes(&epb, TAG_random_resp, response->random.data, response->random.len);
	if (ret < 0) return ret;
	return epb_get_packed_size(&epb);
}

/***********************************************************************
 * @brief				report_log_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			int
 *	eg:

 **********************************************************************/

 void report_log_response_pack(void *args, uint8_t *r_data, uint16_t *r_len)
{
		static uint8_t  	databuf[128],header[64];
//		uint8_t				l_arrBcdtime[8],idata[32];
		int16_t				dataLength;
    uint32_t 			res;
    int64_t				random_val;

    NSFPFixHead*	fix_head = (NSFPFixHead*)header;
		ReportLogResp*	pReportLogResp	= (ReportLogResp*)args;
		pReportLogResp->base_response			=	NULL;
		pReportLogResp->Locker_Address		=	0x12345678;
		pReportLogResp->Last_Work_Type		=	0x01;
		pReportLogResp->iLog_Type						=	IdentityCard;

		random_val	=	 random();
		//random_val	=	0Xaa345678123456bb ; 		
		pReportLogResp->random.data					=	(uint8_t*)&random_val;
		pReportLogResp->random.len						=	8;

		dataLength = epb_report_log_response_pack_size(pReportLogResp);
		res	=	epb_report_log_response_pack(pReportLogResp, databuf, dataLength);
        printf("%s,line:%d,len=%d,epb result=%d \n",__FILE__,__LINE__,dataLength,res);

//		fix_head->CSN						=	0x12345678; 
//		fix_head->State					= 0x0102;
    fix_head->WorkType			=	report_log_cmd;
		fix_head->WorkIndex			=	0x00;
		fix_head->FrameTotal		=	0x00;
		fix_head->FrameSequ			=	0x00;
		fix_head->nLength				=	dataLength;
		fix_head->pData					=	databuf	;
		res = socket_data_pack_func(&header,r_data,r_len);
}
