
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(UART_PACKET_H)
#define UART_PACKET_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#define	_FRAME_HEAD					0XFC
#define	_Portocol_VER				0X01
#define	_MAX_RecevieSize		0xFF



#include <stdbool.h>
#include <stdint.h>

#define		UartEncryptSize					248
#define	 	disable_decipher_debug  0 		// = 0 disable ciphering, =1 enable ciphering
extern 		uint8_t 	Cipher_key[16];

#pragma pack(push,1)
typedef struct 
{
	uint16_t		Dst_Address;	
	uint8_t 		App_Layer_head;
  uint8_t 		protoVer;
  uint8_t 		frameLength;
	uint8_t 		frameTotal;
	uint8_t 		frameNumber;
	uint8_t			*dataPtr;
	uint16_t		crc16;
}__attribute__((packed))UartFrame;	
#pragma pack(pop)


#define	_MAX_Frame					20
#define					UART_FIX_LENGTH		sizeof(UartFrame) - sizeof(uint8_t*)- sizeof(uint16_t)
#define         UART_FRAME_LENGTH       sizeof(UartFrame) - sizeof(uint8_t*)


int8_t uart_data_pack_func(UartFrame *argv,uint8_t *r_data, uint8_t *r_len);

int8_t uart_finger_data_pack_func(UartFrame *argv,uint8_t *r_data, uint8_t *r_len);

#endif
#ifdef __cplusplus
     }
#endif

