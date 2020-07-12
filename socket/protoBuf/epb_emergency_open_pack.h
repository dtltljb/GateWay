
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_EMERGENCY_OPEN_PACK_H)
#define EPB_EMERGENCY_OPEN_PACK_H

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
 
//-------------------------
typedef struct
{
	BaseRequest 			*base_request;
	int32_t						Locker_Address;
	int32_t						Control_state;			//update  of Channel_number
	Bytes							random;							
}EmergencyOpenReq;

typedef struct
{
	BaseResponse 		*base_response;
	int32_t					Locker_Address;
	int32_t					Response_ACK;

}EmergencyOpenResp;  

EmergencyOpenReq *epb_emergency_open_unpack_Req(EmergencyOpenReq *request,const uint8_t *buf, int16_t buf_len);
int32_t epb_pack_emergency_open_response(EmergencyOpenResp *response, uint8_t *buf, int16_t buf_len);
void emergency_open_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);


//--------------			public			-------------------
//extern	EmergencyOpenReq			mEmergencyOpenReq;
//extern	EmergencyOpenResp			mEmergencyOpenResp;
		
#endif
#ifdef __cplusplus
     }
#endif
