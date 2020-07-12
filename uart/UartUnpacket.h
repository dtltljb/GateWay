	
#ifndef __Unpacket_H
#define __Unpacket_H

#include <stdbool.h>
#include <stdint.h>

#include "UartPacket.h"


#define         LORA_ADDRESS_SIZE       0x02

enum	UartUnpackCode
{
	UartSingleReceiverSuccess					=	01,
	UartMultReceiverSuccess						=	02,
	UartWaitReceiveing							,

	UnpackLengthErrorCode 				= 0x80,
	MagicHeaderErrorCode  		,
  UnpackVersionErrorCode  	,
  FrameTotalErrorCode				,
  FrameNumberErrorCode			,
  MagicTailErrorCode				,
	CrcCheckErrorCode					,
	DecodeErrorCode 					,
	EncrypErrorCode				
};



void		uart_data_unpack_create(void	*argv)	;
uint8_t uart_data_unpack_func(void	*argv, uint8_t *r_data, uint8_t *r_len);

#endif
