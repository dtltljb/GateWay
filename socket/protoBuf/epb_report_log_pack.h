
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EPB_REPORT_LOG_PACK_H)
#define EPB_REPORT_LOG_PACK_H

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

#define		DataSize					32


typedef unsigned  int Fix_32bit;

typedef	enum{
	LockerRegister		=	(uint8_t)0,			//locker register request
 	FingerVena				=	(uint8_t)1,
  SwingCard					=	(uint8_t)2,
  Password					= (uint8_t)3,
  IdentityCard  		=	(uint8_t)4,
	HistoryLog				=	(uint8_t)5,
	RegisterStatus		=	(uint8_t)6,			//locker register	status
}Log_Type;

enum LockerStates{
	OnLineMode				=	(uint8_t)2,
	OffLineMode				=	(uint8_t)3,
	unauthorizeOpen		=	(uint8_t)4,
	//---------
	UN_AUTHORIZE_USER			=	(uint8_t)0x70,				//on line open door,authorize user flag
	LOCK_REGISTER_FAIL		=	(uint8_t)0x07,				//locker  register failure
  LOCK_REGISTER_SUCC		=	(uint8_t)0x08,					//locker  register success
  GATEWAY_CHANNEL_FULL	=	(uint8_t)0x09						// register failure becaus
};

//-----Door_Status use define -----------
#define				openDoor					(uint8_t)0
#define				closeDoor					(uint8_t)1		


typedef struct
{
	BaseRequest *base_request;
	int32_t			Locker_Address;
	int32_t			Last_Work_Type;
  enum LockerStates	iLockerStates;						//require
	int32_t			Door_Status;										//require
	CString			Locker_Log_Time;
	Log_Type		iLog_Type;
	int32_t			power;
	int32_t			Channel_number;
	Bytes				Data;
	Bytes				random;
}ReportLogReq;


typedef struct
{
	BaseResponse 				*base_response;
	int32_t							Locker_Address;
	int32_t							Last_Work_Type;
	enum LockerStates		iLockerStates;
	Bytes								random;
	int32_t							Channel_number;
	Log_Type						iLog_Type;
  //-------------------------------------------
  uint8_t             entry_random[8];
}ReportLogResp;

typedef struct
{
	BaseResponse 				*base_response;
	int32_t							Locker_Address;
	int32_t							Last_Work_Type;
	enum LockerStates		iLockerStates;
	Bytes								random;

}ReportLogRespPublic;

int epb_report_log_request_pack_size(ReportLogReq *request);
int epb_report_log_request_pack(ReportLogReq *request, uint8_t *buf, int buf_len);

void report_log_request_pack(void *arg,uint8_t channel, uint8_t *r_data, uint16_t *r_len);
void register_log_request_pack(void *arg,uint8_t channel,uint8_t *r_data, uint16_t *r_len);

ReportLogResp *epb_report_log_response_unpack(ReportLogResp *response,const uint8_t *buf, int buf_len);
void *epb_report_log_response_public_unpack(ReportLogRespPublic *response,const uint8_t *buf, int buf_len);

//----------------  test part  eg -----------------------------------
void report_log_response_pack(void *args, uint8_t *r_data, uint16_t *r_len);


//----------public ------

//extern		ReportLogReq				mReportLogReq;
//extern		ReportLogResp				mReportLogResp;

#endif
#ifdef __cplusplus
     }
#endif
