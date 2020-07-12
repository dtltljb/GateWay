
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_SET_LORA_PARAM_PACK_H)
#define EPB_SET_LORA_PARAM_PACK_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#include <stdbool.h>
#include <stdint.h>


#include "socket/pub/epb_MmBp.h"
#include "socket/pub/util.h"
#include "socket/Packet.h"

#define		WRITE_LORA_PARA_CMD						1
#define		READ_LORA_PARA_CMD						0
#define		SetLoraParamRespCode					0x1000		//'o'
//-------------------------
typedef struct
{
	BaseRequest 			*base_request;
	int32_t						Channel_number;
	int32_t						Control_type;
	bool							has_loraValue;
	Bytes							loraValue;
	//- - - - - - - entry context - - - 
	uint8_t						mloraValBuf[15];							
}SetLoraParamReq;

typedef struct
{
	BaseResponse 		*base_response;
	int32_t					 set_States;

}SetLoraParamResp;  

SetLoraParamReq *set_lora_param_unpack_Req(SetLoraParamReq *request,const uint8_t *buf, uint16_t buf_len);
void set_lora_param_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);

//---------------------------------------
typedef struct
{
	BaseRequest 			*base_request;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
	int32_t						Locker_Status;  
  int32_t						Locker_ACK;
  CString						Report_Time;
  Bytes							Read_context;
  //- - - - - - - entry context - - - 
  uint8_t						m_report_time[8];
  uint8_t						m_read_context[15];
}SetLoraParaResultReq;

typedef struct
{
	BaseResponse 			*base_response;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
  int32_t						state;
	Bytes							random;
	//- - - - - - - entry context - - - 
	uint8_t						m_random[8];
}SetLoraParaResultResp;

void set_lora_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);
SetLoraParaResultResp *set_lora_para_result_unpack_response(SetLoraParaResultResp *response,const uint8_t *buf, uint16_t buf_len);

//--------------			public			-------------------
//	extern			SetLoraParamReq				mSetLoraParamReq;
//	extern			SetLoraParamResp			mSetLoraParamResp;
//	extern			SetLoraParaResultReq	mSetLoraParaResultReq;
//	extern			SetLoraParaResultResp	mSetLoraParaResultResp;		
	
#endif
#ifdef __cplusplus
     }
#endif
