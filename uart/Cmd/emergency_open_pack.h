
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(EMERGENCY_OPEN_PACK_H)
#define EMERGENCY_OPEN_PACK_H

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


#define  EmergencyOpen		0x32 				/* non ACK*/

#pragma pack(push,1)
typedef struct{
	uint8_t							cmd;
	uint32_t						LockerNumber;
	uint8_t							ProtoTime[8];
	uint16_t						ControlStatus;				//add new	
}__attribute__((packed))EmergencyControl;
#pragma pack(pop)

//** LockerNumber == 0xFFFF & Dst_Address == 0x0000
uint8_t uart_emergency_control_pack(void *args, void	*argu);

extern	EmergencyControl			mEmergencyControl;			//uart

#endif
#ifdef __cplusplus
     }
#endif
