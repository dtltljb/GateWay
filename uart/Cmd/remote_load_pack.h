
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(REMOTE_LOAD_PACK_H)
#define REMOTE_LOAD_PACK_H

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

#define  STATIC_FINGER_SIZE			(249 - ( sizeof(UartFrame) - sizeof(uint8_t*) ) - ( sizeof(uint8_t)*2 + sizeof(uint32_t) ) )
#define  FINGER_DATA_SIZE			( 249	- ( sizeof(UartFrame)- sizeof(uint8_t*)  )  )


typedef enum {
	ChangeChannel				=	1,			
	DownLoadSingle			=	2	
}LoadType;

#define  RemoteChangeChannelReq						0x38
#define  RemoteChangeChannelResp					0x34
#define  RemoteChangeChannelACK_SUCC					0x9000

typedef struct {
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							DstFrequency[3];
}__attribute__((packed))ChangeChannelReq;

typedef struct {
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint16_t						RespACK;
}__attribute__((packed))ChangeChannelResp;


#define  RemoteLoadFinger									0x39
#define  RemoteLoadFingerACK							0x3a
#define  RemoteLoadFingerACK_SUCC					0x9000

#pragma pack(push,1)
typedef struct
{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							ConType;
	uint8_t							SingleInfo[STATIC_FINGER_SIZE];
}__attribute__((packed))URemoteLoadReq;

typedef struct
{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							ConType;
	uint8_t							ProtoTime[8];	
	uint16_t						RespACK;
}__attribute__((packed))URemoteLoadResp;
#pragma pack(pop)

extern				ChangeChannelReq					mChangeChannelReq;
extern				URemoteLoadReq						mURemoteLoadReq;
extern				URemoteLoadResp						mURemoteLoadResp;
//extern				struct VirtualSocket      loadVirtualSocket; /* uart_Change_Channel_pack() send signel pass context */

uint8_t uart_Change_Channel_pack(void *args, void	*argu);

uint8_t uart_remote_load_pack(void *args, void	*argu);


#endif
#ifdef __cplusplus
     }
#endif
