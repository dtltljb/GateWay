
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(REMOTE_AUTHORIZE_PACK_H)
#define REMOTE_AUTHORIZE_PACK_H

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

#define  conf_frame_max_size			255
#define  AuthorizeDataSize				conf_frame_max_size - ( sizeof(UartFrame) - sizeof(uint8_t*)) -sizeof(uint8_t)*4-sizeof(uint32_t)

#define  RemoteAuthorize				0x35
#define  RemoteAuthorizeACK			0x36


#define  RemoteAuthorizeACK_SUCC				0X9000
#define  RemoteAuthorizeACK_FAIL				0X6F02		

/*
enum DataType{
	_CardID				=	1,
	_Password			=	2,
	_VenaFinger		=	3,
	_IdentityCard	=	4
};
enum ControlType{
	DeleteAll			=	1,
	DeleteUser		=	2,
	NewCreate			=	3
};
*/


union DownLoadContext{
	uint32_t		Password;
	uint32_t		CardID;
	uint8_t			IdentityCard[32];
};

#define					URemoteAuthorizeReq_STRUCT_HEADER_SIZE		( sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t)*3 )
#pragma pack(push,1)
typedef struct{
	uint8_t									cmd;
	uint32_t								LockerNumber;
	uint8_t									controlType;
	uint8_t									dataType;
	uint8_t									holidayControl;
	uint8_t									context[AuthorizeDataSize];		
}__attribute__((packed))URemoteAuthorizeReq;

typedef struct{
	uint8_t									cmd;
	uint32_t								LockerNumber;
	uint8_t									dataType;	
  uint8_t									ProtoTime[8];
	uint16_t								RespACK;
}__attribute__((packed))URemoteAuthorizeResp;

#pragma pack(pop)

uint8_t uart_request_remote_authorize_pack(void *args, void	*argu);

//extern				URemoteAuthorizeReq					mURemoteAuthorizeReq;
//extern				URemoteAuthorizeResp				mURemoteAuthorizeResp;

#endif
#ifdef __cplusplus
     }
#endif
