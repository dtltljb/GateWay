
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_KEEP_ALIVE_PACK_H)
#define EPB_KEEP_ALIVE_PACK_H

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
#include "socket/Packet.h"

typedef struct
{
	BaseRequest *base_request;
	int32_t				keep_alive_time;				//add new 
	String				TimeZone;								//add new option
	String				Language;								//add new option
	int32_t				AuthMethod;							//add new option
	Bytes					MacAddress;							//add new 			max size= 6 bytes
	String				DeviceName;							//add new       max size= 32bytes
	Bytes 				keep_alive_random;
} KeepAliveReq;												

typedef struct
{
	BaseResponse *base_response;	
	int32_t				keep_alive_time;				//add new 
	String				ManagerPhoneID;					//add new option	max size=16 bytes
	String				ServerURL;							//add new option	max size=128bytes
	String				ManufaturerURL;					//add new option	max size=128bytes
	Bytes 				keep_alive_random;
	
} KeepAliveResp;												


extern		KeepAliveReq			mKeepAliveReq;
extern		KeepAliveResp			mKeepAliveResp;

int32_t epb_keepAlive_request_pack_size(KeepAliveReq *request);
int32_t epb_pack_keepAlive_request(KeepAliveReq *request, uint8_t *buf, int buf_len);
KeepAliveResp *epb_unpack_keepAlive_response(KeepAliveResp *response,const uint8_t *buf, int buf_len);
void keepAlive_request_pack(void *args, uint8_t *r_data, uint16_t *r_len);

#endif
#ifdef __cplusplus
     }
#endif
