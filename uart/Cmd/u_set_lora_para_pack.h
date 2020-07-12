
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(U_SET_LORA_PARA_PACK_H)
#define U_SET_LORA_PARA_PACK_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#include <stdbool.h>
#include <stdint.h>

#include "uart/UartPacket.h"

#define	SetLoraParaACK_SUCC			0x9000
#define	SetLoraParaACK_FAIL			0x6F05
#define	LORA_PARA_LEN						14

struct	SetLoraFrame{
	uint8_t		megicCode[7];
	uint8_t		address;
	uint8_t		length;
};

struct	GetLoraFrame{
	uint8_t		megicCode[7];
	uint8_t		address;
	uint8_t		length;
};
#pragma pack(push,1)
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
	uint16_t					GateWayAddress;
	uint8_t						Functionality;
}__attribute__((packed))LoraProperty;
#pragma pack(pop)


uint8_t 		uart_set_lora_para_pack(void *args, void	*argu);

uint8_t 		uart_get_lora_prm(void	*argu);

#endif
#ifdef __cplusplus
     }
#endif
