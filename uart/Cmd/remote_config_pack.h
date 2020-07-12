
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(REMOTE_CONFIG_PACK_H)
#define REMOTE_CONFIG_PACK_H

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


#include "uart/UartPacket.h"

#define  frame_max_size				255
#define  ConfigDataSize				frame_max_size - ( sizeof(UartFrame) - sizeof(uint8_t*)) -sizeof(uint8_t)*3-sizeof(uint32_t)

#define  RemoteConfig						0x3b
#define  RemoteConfigACK				0x3c

#define	RemoteConfigACK_SUCC			0x9000
#define	RemoteConfigACK_FAIL			0x6F04

#define	CONFIG_CONTROL_TYPE_READ	0x02
#define	CONFIG_CONTROL_TYPE_WRITE	0x01
enum ConfigType{
	TimeGroup					=	1,
	HoladayGroup			=	2,
	LoraPorperty			=	3
};

#define					ConfigLockerInfoReq_STRUCT_HEADER_SIZE		( sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t)*3 )

#pragma pack(push,1)
typedef	struct {
	uint8_t						cmd;
	uint32_t					LockerNumber;
	uint8_t						DataType;
  uint8_t						controlType;
	uint8_t						data[ConfigDataSize];
}__attribute__((packed))ConfigLockerInfoReq;

typedef	struct {
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							type;
	uint8_t							ProtoTime[8];
//	uint8_t							data[16]; 					//Lora \door \locker
	uint8_t							data[18]; 					//Lora-prm, locker-states, 2019-03-27 add new dist-address context
	uint16_t						RespACK;
}__attribute__((packed))ConfigLockerInfoResp;

typedef struct
{
	uint8_t						frequency[3];
	uint8_t 					AirRate;
	uint8_t 					SendPower;
	uint8_t						rate;
	uint8_t						UartCheck;
	uint8_t						WakePeriods;
	uint8_t						TrigleDelay;
	uint8_t						CarrierSenseTime;
	uint8_t						WireDataLen;
	uint16_t					HALAddress;
	uint8_t						Functionality;
}__attribute__((packed))LockerProperty;
#pragma pack(pop)

uint8_t uart_request_remote_config_pack(void *args, void	*argu);

uint8_t uart_request_remote_get_config_pack(void *args, void	*argu);

//extern			ConfigLockerInfoReq					mConfigLockerInfoReq;
//extern			ConfigLockerInfoResp				mConfigLockerInfoResp;

#endif
#ifdef __cplusplus
     }
#endif
