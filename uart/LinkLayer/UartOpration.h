
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(UART_OPRATION_H)
#define UART_OPRATION_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#define			uart_receive_size				255
#define			uart_send_size					255
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
/*
	uart port parament
*/

#include "uart/UartPacket.h"

enum	Uart_States_Flag{
				UartError					=	0x00,
				UartFrameError		,
				
			  UartInitialize		=	0x05,
				UartSendBusying		,
				UartReceBusying		,
				UartIDLE					
	
};

enum	uart_WorkStates{
        WorkStatesIDLE		=	00,
				WorkStatesBusy		= 101,
				RequestError			= 102,
};

enum	Uart_pipe_Flag{
	PIPE_SEND_REQUEST	=	'S',
	PIPE_RECE_TIMEOUT	=	'T',
	PIPE_RECE_SUCCESS	=	'R',
	PIPE_LORA_REQUEST	=	'L'
};

typedef struct 
{

		int8_t 							iodevice[64];
		const	char								*h_io;
		uint16_t							bps;
		uint8_t								bit;
    uint8_t								check;
		
		int32_t						 				clt_uart;
		uint8_t										deviceStates;
		enum	uart_WorkStates			WorkStates;
		enum	Uart_pipe_Flag			pipe;
		
		uint8_t								channelNumber;					// 1byte rang:1~5 channel
		uint32_t							FreqValue;
		uint8_t								AirRate;
		uint16_t							GateWayAddress;
		uint8_t								DeviceTotal;
		
		uint8_t								workType;
    void*									pContext;
		uint8_t  							ReceiveBuf[uart_send_size];
		uint16_t 							ReceiveLength;
		
		uint8_t								UartSingleReceiveBuf[uart_receive_size];
		uint8_t								UartSingleReceiveLength;
		UartFrame							current_fix_header;
		
		//----		log	context	---------------
		uint8_t								runLogStates;
		uint8_t								Topic[128];
		uint8_t								runTime[8];

}UartPort;



uint8_t initialize_uart(UartPort*	argu);
//int 		uart_read_wait_time(int fd, int sec);
void		select_lora_states(uint8_t	*io_channel,bool seta,bool setb);

#endif
#ifdef __cplusplus
     }
#endif
