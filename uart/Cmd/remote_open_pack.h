
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(REMOTE_OPEN_PACK_H)
#define REMOTE_OPEN_PACK_H

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

#include "socket/VirtualPortMonitor.h"


#define  RemoteOpenDoor				0x33
#define  SureRequestOpenDoor	0x43

#pragma pack(push,1)
typedef struct{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint16_t						ControlStatus;
	uint8_t							ProtoTime[8];
}__attribute__((packed))RemoteOpenDoorReq;

typedef struct{										//lock -> Gateway
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							RandomNumber[8];	
	uint8_t							ProtoTime[8];

}__attribute__((packed))SureRequestOpenDoorResp;


#define  EnsureRequestOpenDoor	0x31
#define  RemoteOpenDoorACK			0x41

typedef struct{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							RandomNumber[8];	
	uint8_t							ProtoTime[8];	
}__attribute__((packed))EnsureRequestOpenDoorReq;

typedef struct{								//lock -> Gateway
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							ProtoTime[8];
	uint8_t							LockerStatus;
	uint16_t						RespACK;
}__attribute__((packed))RemoteOpenDoorACKResp;
#pragma pack(pop)

#define  RemoteOpenDoorAck_SUCC				0x9000
#define  RemoteOpenDoorAck_FAIL				0x6F00

//extern			RemoteOpenDoorReq							mRemoteOpenDoorReq;
//extern			SureRequestOpenDoorResp				mSureRequestOpenDoorResp;
//extern			EnsureRequestOpenDoorReq			mEnsureRequestOpenDoorReq;
//extern			RemoteOpenDoorACKResp					mRemoteOpenDoorACKResp;

/***********************************************************************
 * @brief				uart_request_remote_open_pack
 * @param[in]		void *args, void	*argu
 * @param[out]	void *args,work context
 * @return 			status	=1 success
 **********************************************************************/
uint8_t uart_request_remote_open_pack(void *args, void	*argu);


#endif
#ifdef __cplusplus
     }
#endif
