
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_REMOTE_GET_CONFIG_PACK_H)
#define EPB_REMOTE_GET_CONFIG_PACK_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#include <stdbool.h>
#include <stdint.h>
#include <signal.h>

#include "socket/pub/epb_MmBp.h"
#include "epb_report_log_pack.h"


typedef struct
{// data_type = 0 non data
	BaseRequest 						*base_request;
  int32_t									Locker_Address;
	int32_t									Channel_number;  

}RemoteGetConfigReq;

typedef struct
{
	BaseResponse 			*base_response;
	int32_t						Locker_Address;
  int32_t						Response_ACK;	
}RemoteGetConfigResp;



/***********************************************************************
 * @brief				remote_config_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_config_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);


/*
*	remote Config Result Request
*********************************************/

typedef struct
{
	BaseRequest 			*base_request;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
	int32_t						Control_type; 
  int32_t						Locker_ACK;  
	CString						Report_Time;	 
  int32_t						data_type;
	Bytes							data_context;		//new add:response data context
}RemoteGetConfigResultReq;

typedef struct
{
	BaseResponse *base_response;
	int32_t			Locker_Address;
	int32_t			Last_Work_Type;
	int32_t			curr_states;
	Bytes				random;
}RemoteGetConfigResultResp;


RemoteGetConfigReq *epb_remote_get_config_request_unpack(RemoteGetConfigReq *request,const uint8_t *buf, int buf_len);
void remote_get_config_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);

void remote_get_config_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);
RemoteGetConfigResultResp *epb_remote_get_config_result_response_unpack(RemoteGetConfigResultResp *response,const uint8_t *buf, int buf_len);


//extern				RemoteGetConfigReq					mRemoteGetConfigReq;
//extern				RemoteGetConfigResp					mRemoteGetConfigResp;
//extern				RemoteGetConfigResultReq		mRemoteGetConfigResultReq;
//extern				RemoteGetConfigResultResp		mRemoteGetConfigResultResp;


#endif
#ifdef __cplusplus
     }
#endif



