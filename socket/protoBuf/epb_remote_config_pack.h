
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_REMOTE_CONFIG_PACK_H)
#define EPB_REMOTE_CONFIG_PACK_H

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

 enum control_type{
                        type_write = 0,
                        type_read 	= 1,
		};
		
 enum data_type{
											timer_group = 1,
											holiday_group,
											Lora_property
		};
		
typedef struct
{// data_type = 0 non data
	BaseRequest 						*base_request;
  int32_t									Locker_Address;
	int32_t									Channel_number;  
	enum control_type				iControl_type;
	enum data_type					first_data_type;
	Bytes										first_data_group;
	enum data_type					second_data_type;	
	Bytes										second_data_group;
	enum data_type					third_data_type;
	Bytes										third_data_group;
	//-- -- - entry -----
	int8_t		data_group_1[240];
	int8_t		data_group_2[240];
	int8_t		data_group_3[240];
}RemoteConfigReq;

typedef struct
{
	BaseResponse 			*base_response;
	int32_t						Locker_Address;
  int32_t						Response_ACK;	
}RemoteConfigResp;

RemoteConfigReq *epb_remote_config_request_unpack(RemoteConfigReq *request,const uint8_t *buf, int buf_len);
int epb_remote_cofig_response_pack_size(RemoteConfigResp *response);
int epb_remote_config_response_pack(RemoteConfigResp *response, uint8_t *buf, int buf_len);
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
	Bytes							dataContext;		//new add:response data context
}RemoteConfigResultReq;

typedef struct
{
	BaseResponse *base_response;
	int32_t			Locker_Address;
	int32_t			Last_Work_Type;
	int32_t			curr_states;
//	Log_Type		iLog_Type;
	Bytes				random;
}RemoteConfigResultResp;

int epb_remote_config_result_pack_size(RemoteConfigResultReq *request);
int epb_remote_config_result_pack_request(RemoteConfigResultReq *request, uint8_t *buf, int buf_len);
RemoteConfigResultResp *epb_remote_config_result_response_unpack(RemoteConfigResultResp *response,const uint8_t *buf, int buf_len);
/***********************************************************************
 * @brief				remote_config_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_config_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);

//extern				RemoteConfigReq					mRemoteConfigReq;
//extern				RemoteConfigResp				mRemoteConfigResp;
//extern				RemoteConfigResultReq		mRemoteConfigResultReq;
//extern				RemoteConfigResultResp	mRemoteConfigResultResp;


#endif
#ifdef __cplusplus
     }
#endif



