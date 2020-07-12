
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(SOCKET_PROTOCOL_H)
#define SOCKET_PROTOCOL_H

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
#include <semaphore.h>


#pragma pack(push,1)
 typedef   struct   //__Pack	__packed
{
                 uint16_t  	bMagicNumber;   	//0x5aa5
                 uint32_t  	CSN;		 					//GW  index number
                 uint16_t  	State;						//
                 uint8_t		WorkType;					//
                 uint8_t  	WorkIndex;  			//	
                 uint8_t		FrameTotal;
                 uint8_t		FrameSequ;
                 uint16_t		Ver;
                 uint8_t		TimeStamp[8];
                 uint16_t		nLength;
								 uint8_t		*pData;						//Data area ptr
								 uint16_t		MagicTail;				//0x0EE0
}__attribute__((packed))NSFPFixHead;  
#pragma pack(pop)

#define		MAGIC_NUMBER			0x5AA5
#define		MAGIC_TAIL				0x0EE0

#define		FRAME_DATA_AREA					(int16_t)(sizeof(NSFPFixHead) - sizeof(uint16_t) - sizeof(uint8_t *) )
#define		HEADER_FIX_LENGTH				(int16_t)(sizeof(NSFPFixHead) - sizeof(uint8_t *) )
#define		NFSP_FIX_LENGTH					(int16_t)(sizeof(NSFPFixHead) - sizeof(uint16_t) - sizeof(uint8_t *) )


#define			SINGLE_RECEIVE_SUCCESS	0x01
#define			MULTI_RECEIVE_SUCCESS		0x02
#define			SOCKET_WAIT_RECEIVING		0x03

#define			errorCodeUnpackLengthMin		0x80
#define			errorCodeMagicHeader		0x81
#define			errorCodeUnpackVersion		0x82
#define			errorCodeFrameTotal		0x83
#define			errorCodeFrameNumber		0x84
#define			errorCodeMagicTail		0x85
#define			errorCodeCrcCheck		0x86
#define			errorCodeDecodeError		0x87
#define			errorWorkTypediff		0x88
/*
typedef enum		
{
//	singleReceiverSucces					=	01,
//	socketReceiverSucces					=	02,
//	socketWaitReceiveing							,
	
//	errorCodeUnpackLengthMin 			= 0x80,
//	errorCodeMagicHeader  				= 0x81,
//  errorCodeUnpackVersion  			= 0x82,
//  errorCodeFrameTotal			,
//  errorCodeFrameNumber		,
//  errorCodeMagicTail						= 0x90,
//	errorCodeCrcCheck							= 0x95,
//	errorCodeDecodeError 					= 0x97,
//	errorWorkTypediff							=	0xa0
}UnpackCode;
*/

#define 	keep_alive_cmd									(uint8_t)00
#define 	report_log_cmd									(uint8_t)01
#define 	emergency_open_door							(uint8_t)0x10
#define 	remote_open_door                (uint8_t)0x11
#define 	remote_authorize_cmd            (uint8_t)0x12
#define 	remote_load_cmd									(uint8_t)0x13
#define 	remote_config_cmd								(uint8_t)0x14
#define		set_lora_param_cmd							(uint8_t)0x15
#define		remote_get_config_cmd						(uint8_t)0x16
#define		remote_load_channel_cmd					(uint8_t)0x17

typedef enum
{
	   
		keep_alive_cmd_resp_return		 	= 0x1000,
    emergency_open_door_return 			= 0x1000,
    remote_open_door_return					= 0x1000,
    remote_authorize_cmd_return			= 0x1000,
    report_log_cmd_resp_return 			= 0x1000,
    remote_load_cmd_return					= 0x1000,
    remote_config_cmd_return				= 0x1000,
    remote_get_config_cmd_return		= 0x1000,
    set_lora_param_cmd_return       = 0x1000,
//   set_lora_param_cmd_return       = 0x00,		//if not  soft judge failure
   
    report_log_cmd_resp_return_err	= 0x1001,
    
		remote_open_door_un_response		= 0x9001,
		remote_open_door_ok_response		= 0x9000,
		
    remote_authorize_un_Finger_p		= 0x9010,
    remote_authorize_un_Card_p			= 0x9011,
    remote_authorize_un_Password_p	= 0x9012,
    remote_authorize_un_Identity_p	= 0x9013,
    remote_authorize_ok_response		= 0x9000,
    
    remote_load_un_response				  = 0x9003,
    remote_load_ok_response					= 0x9000,

    remote_channel_un_response			= 0x9007,
    remote_channel_ok_response			= 0x9500,			//
    
    remote_config_un_first_p	      = 0x9014,
    remote_config_un_second_p	      = 0x9015,
    remote_config_un_third_p	      = 0x9016,  
    remote_config_un_get_p	      = 0x9017,      
    remote_config_ok_response			  = 0x9000,

    remote_get_config_un_response	  = 0x9005,
    remote_get_config_ok_response	  = 0x9000,
   
    set_lora_param_un_return        = 0x9006,
    set_lora_param_ok_return        = 0x9000,

//	EmergencyOpenRespCode 				  = 0x2998,
		work_type_rang_error 					  = 0x2999
}NSFP_ERROR_COMMAND_ID;

typedef int*    phandle;
//	------------- public ----------------------
sem_t sem_first_channel_WaitSend,sem_second_channel_WaitSend,sem_third_channel_WaitSend,sem_fourth_channel_WaitSend,sem_config_channel_WaitSend;
extern	phandle *pFirst_handle,*pSecond_handle,*pThird_handle,*pFourth_handle,*pConfig_handle;
extern	pthread_mutex_t lora_prm_mutex_lock;

extern	uint8_t					socketReceiveBuf[1050];
extern	uint16_t					socketReceiveLength;


void* socket_protocol_thread(void *pvoid);

#endif
#ifdef __cplusplus
     }
#endif
