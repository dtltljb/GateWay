
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_REMOTE_AUTHORIZE_PACK_H)
#define EPB_REMOTE_AUTHORIZE_PACK_H

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

//repair 

//#define		DELETEALL			0
//#define		DELETEUSER			1
//#define		CREATEUSER			2


enum ControlType{
			DELETEALL 	= 0,
      DELETEUSER 	=1,
      CREATEUSER        =2
		};

enum Data_type{
			Finger_Number=1,
			Smart_Card ,
			Password_Card,
			Identity_Card
		};

typedef struct
{ // Data_type = 0 ,non data context
	int32_t						user_control_list;
	enum Data_type		dataType_1;
	Bytes							Data_context_1;
	enum Data_type		dataType_2;
	Bytes							Data_context_2;	
	enum Data_type		dataType_3;
	Bytes							Data_context_3;
	enum Data_type		dataType_4;
	Bytes							Data_context_4;
	
}__attribute__((packed))User_authorize;


typedef struct
{
	BaseRequest 										*base_request;
  int32_t													Locker_Address;
	int32_t													Channel_number;  
	enum ControlType								iControlType;
  User_authorize									iUser_authorize;
	
//	bool					has_user_authorize_1;
//	User_authorize									iUser_authorize1;
//	bool					has_user_authorize_2;
//	User_authorize									iUser_authorize2;
//	bool					has_user_authorize_3;
//	User_authorize									iUser_authorize3;
//	bool					has_user_authorize_4;
//	User_authorize									iUser_authorize4;
//	bool					has_user_authorize_5;
//	User_authorize									iUser_authorize5;
	//---- entry ---------
	uint8_t						m_data_context_1[255];
	uint8_t						m_data_context_2[255];
	uint8_t						m_data_context_3[255];
	uint8_t						m_data_context_4[255];
	
}RemoteAutorizeReq;


typedef struct
{
	BaseResponse 			*base_response;
  int32_t						Locker_Address;
  int32_t						Response_ACK;

}RemoteAuthorizeResp;

RemoteAutorizeReq *epb_remote_authorzie_request_unpack(RemoteAutorizeReq *request,const uint8_t *buf, int buf_len);
int epb_remote_authorize_response_pack_size(RemoteAuthorizeResp *response);
int epb_remote_authorize_response_pack(RemoteAuthorizeResp *response, uint8_t *buf, int buf_len);
/***********************************************************************
 * @brief				remote_authorize_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_authorize_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);

//******************************

typedef struct
{
	BaseRequest 			*base_request;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
  int32_t						Locker_Status;	
  int32_t						Locker_ACK; 	
 	CString						Report_Time;
}RemoteAuthorizeResultReq;

typedef struct
{
	BaseResponse 			*base_response;
	int32_t						Locker_Address;
	int32_t						Last_Work_Type;
	int32_t						curr_states;
	//Log_Type					iLog_Type;
	Bytes							random;
}RemoteAuthorizeResultResp;

int epb_remote_authorize_result_pack_size(RemoteAuthorizeResultReq *request);
int epb_remote_authorize_result_pack_request(RemoteAuthorizeResultReq *request, uint8_t *buf, int buf_len);
RemoteAuthorizeResultResp *epb_remote_config_result_unpack_response(RemoteAuthorizeResultResp *response,const uint8_t *buf, int buf_len);
/***********************************************************************
 * @brief				remote_authorize_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_authorize_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);

//extern			uint8_t				User_List_DataContext[5][4][32];
//extern			RemoteAutorizeReq					mRemoteAutorizeReq;
//extern			RemoteAuthorizeResp				mRemoteAuthorizeResp;
//extern			RemoteAuthorizeResultReq	mRemoteAuthorizeResultReq;
//extern			RemoteAuthorizeResultResp	mRemoteAuthorizeResultResp;

#endif
#ifdef __cplusplus
     }
#endif
