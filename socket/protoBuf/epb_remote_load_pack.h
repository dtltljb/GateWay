
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_REMOTE_LOAD_PACK_H)
#define EPB_REMOTE_LOAD_PACK_H

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
	int32_t						Channel_number;  						//required
	int32_t						User_Control_list;
	int32_t						finger_Number;						//cancel
  Bytes							finger_context;
  //------------------------------
  uint8_t						finger_data[4096];

}__attribute__((packed))RemoteLoadReq;

typedef struct
{
	BaseResponse 			*base_response;
  int32_t						Locker_Address;
  int32_t						Response_ACK;
}__attribute__((packed))RemoteLoadResp;

RemoteLoadReq *epb_remote_load_request_unpack(RemoteLoadReq *request,uint8_t *buf, uint16_t buf_len);
int epb_remote_load_response_pack_size(RemoteLoadResp *response);
int epb_remote_load_response_pack(RemoteLoadResp *response, uint8_t *buf, int buf_len);
/***********************************************************************
 * @brief				remote_load_response_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_load_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);


//-----------------------------------------------
typedef struct
{
	BaseRequest 			*base_request;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
 	int32_t						Locker_Status;
  int32_t						Locker_ACK;
  CString						Report_Time;
}RemoteLoadResultReq;

typedef struct
{
	BaseResponse 			*base_response;
  int32_t						Locker_Address;
  int32_t						Last_Work_Type;
  int32_t						curr_states;
	Bytes							random;
}RemoteLoadResultResp;

int epb_remote_load_result_pack_size(RemoteLoadResultReq *request);
int epb_remote_load_result_pack_request(RemoteLoadResultReq *request, uint8_t *buf, int buf_len);
RemoteLoadResultResp *epb_remote_load_result_unpack_response(RemoteLoadResultResp *response,const uint8_t *buf, int buf_len);


/***********************************************************************
 * @brief				remote_load_result_request_pack
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			void
 **********************************************************************/
void remote_load_result_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);


extern			RemoteLoadReq						mRemoteLoadReq;
extern			RemoteLoadResp					mRemoteLoadResp;
extern			RemoteLoadResultReq			mRemoteLoadResultReq;
extern			RemoteLoadResultResp		mRemoteLoadResultResp;

#endif
#ifdef __cplusplus
     }
#endif
