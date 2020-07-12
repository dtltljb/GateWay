
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_REMOTE_OPEN_PACK_H)
#define EPB_REMOTE_OPEN_PACK_H

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
#include "socket/pub/util_date.h"
#include "socket/Packet.h"

typedef struct
{
	BaseRequest 			*base_request;
  int32_t						Locker_Address;
	int32_t						Channel_number;  
  int32_t						Control_state;
}RemoteOpenReq;

typedef struct
{
	BaseResponse 			*base_response;
  int32_t						Locker_Address;
  int32_t						Response_ACK;
}RemoteOpenResp;

RemoteOpenReq *epb_remote_open_request_unpack(RemoteOpenReq *request,const uint8_t *buf, int buf_len);

//int epb_remote_open_response_pack_size(RemoteOpenResp *response);
//int epb_remote_open_response_pack(RemoteOpenResp *response, uint8_t *buf, int buf_len);

/***********************************************************************
 * @brief				remote_open_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_open_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);



//---------------------------------------
typedef struct
{
	BaseRequest 			*base_request;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
	int32_t						Locker_Status;  
  int32_t						Locker_ACK;
  CString						Report_Time;
}RemoteOpenResultReq;

typedef struct
{
	BaseResponse 			*base_response;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
  int32_t						state;
	Bytes							random;
}RemoteOpenResultResp;

//int epb_remote_open_result_pack_size(RemoteOpenResultReq *request);
//int epb_remote_open_result_pack_request(RemoteOpenResultReq *request, uint8_t *buf, int buf_len);

RemoteOpenResultResp *epb_remote_open_result_unpack_response(RemoteOpenResultResp *response,const uint8_t *buf, int buf_len);



/***********************************************************************
 * @brief				remote_open_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_open_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);


//---------------		public		-----	-----
//extern			RemoteOpenReq							mRemoteOpenReq;
//extern			RemoteOpenResp						mRemoteOpenResp;
//extern			RemoteOpenResultReq				mRemoteOpenResultReq;
//extern			RemoteOpenResultResp			mRemoteOpenResultResp;

#endif
#ifdef __cplusplus
     }
#endif
