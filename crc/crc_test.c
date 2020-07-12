//  crc32.c
//  WeChat Embedded
//
//  Created by harlliu on 14-03-03.
//  Copyright 2014 Tencent. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "crc32.h"
#include "crc16.h"



int  main (void)
{
	uint8_t	buffer[1000];
	uint16_t crcValue;
	uint32_t crc32Value;
	memset(buffer,55,sizeof(buffer));
	crcValue = crc16(buffer,sizeof(buffer));
	printf("crc 16 Value = %d\n",crcValue);
	
	memset(buffer,55,sizeof(buffer));
	crc32Value = crc32(CRC32InitVal,buffer,sizeof(buffer));
	printf("crc 32 Value = %d\n",crc32Value);
		
}