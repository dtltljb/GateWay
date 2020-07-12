
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(REPORT_LOG_PACK_H)
#define REPORT_LOG_PACK_H

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


//#include    "../LinkLayer/VirtualUartLink.h"
//#include    "socket/VirtualPortMonitor.h"

#include    "socket/protoBuf/epb_report_log_pack.h"

#define	 disable_locker_check_time		 1				//	0 disable check,	1 enable check

#define  ReportLog												0x30
#define  ReportLogACK                   	0x31
#define  ReportLogOffLineACK              0x3D
#define  ReportLogUnauthorizeACK          0x3E

#define  Report_Log_analysis_cmd_ret			0x130


#define  ReportLogACK_FAIL                   0x6F08
#define  ReportLogACK_SUSS                   0x9000

typedef union {
	uint32_t  		FingerNuber;
	uint32_t			Password;
	uint32_t			CardID;
	uint8_t				IdentityCard[32];
}RespInfoType;

#pragma pack(push,1)
typedef	struct{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							LogType;
	uint8_t							RandomNumber[8];
	uint8_t							LogTime[8];
	uint8_t							LockerPower;
	uint8_t							LockerStates;
	uint8_t							InfoEntity[32];
}__attribute__((packed))UReportLogReq;

typedef	struct{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							LogType;
	uint8_t							RandomNumber[8];
	uint8_t							Frequery[3];
	uint8_t							AirRate;
	uint16_t						GateWayAddress;
	//- - - - - - - check clock - - - - -
#if	disable_locker_check_time	== 1	
	uint8_t							LogTime[8];		//
#endif		
}__attribute__((packed))UReportLogResp;



#define  RegisterEnsureAck								0x3F

#define  RegisterEnsureAck_SUCC								0x9000

typedef	struct{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							LogType;	
	uint16_t						RespACK;
	uint8_t							Frequery[3];
	uint8_t							AirRate;
	uint16_t						GateWayAddress;
}__attribute__((packed))UReportLogReqSec;
#pragma pack(pop)

//uint8_t uart_response_report_log_pack(void *args,void	*argu);
uint8_t report_online_log_pack(void *arg, void	*argu);
uint8_t report_offline_log_pack(void *args, void	*argu,uint8_t	cmd);

/***********************************************************************
 * @brief				register_log_pack,	锁体注册
 * @param[in]		void *args, 		VirtualSocket
 *							void	*argu,		UartPort
 * work channel receive ack
 * @return 			0 faile,1 success
 **********************************************************************/
uint8_t register_log_pack(void *args, void	*argu);
uint8_t register_online_log_pack(void *args, void	*argu);
/***********************************************************************
 * @brief				report_history_log_pack
 * @param[in]		UReportLogReq *pUReq,	struct ReportLogReq
 *							uint8_t channel,			
 * @return 			void
 **********************************************************************/
void	*report_history_log_pack(UReportLogReq *pUReq,uint8_t channel);



#endif
#ifdef __cplusplus
     }
#endif
