//  crc32.c
//  WeChat Embedded
//
//  Created by harlliu on 14-03-03.
//  Copyright 2014 Tencent. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#include "d3des.h"


/*

 //声明在 d3des.h 文件
#define EN0	0	//* MODE == encrypt 
#define DE1	1	//* MODE == decrypt 


void exaple_des(void)
{
	unsigned char check = 0;
	unsigned char rec_buf[64];
	unsigned char  RaidoBuf[10];
	unsigned char i,j;

		for(j=0;j<10;j++)
			RaidoBuf[j] = 0;
		j=0;
		for(i=0;i<3;i++)
		{
			deskey(key, DE1);                      //解密
			des(&s_rec_buf[j+2], RaidoBuf);           //源，目标
			zyjk_memcpy(&rec_buf[j],RaidoBuf,8);
			j=j+8;
		}
			
}

*/
 
 //*************************************
unsigned char key[16] = {0x32,0x32,0x67,0x28,0x45,0x37,0x26,0x15,
						 0x42,0x38,0x76,0x29,0x75,0xC8,0xD9,0xE0};


unsigned char key1[8]={0x32,0x32,0x67,0x28,0x45,0x37,0x26,0x15};
unsigned char key2[8]={0x42,0x38,0x76,0x29,0x75,0xC8,0xD9,0xE0};

 
 
int  main (void)
{
	
	unsigned char check = 0;
	unsigned char inbuf[64];
	unsigned char outbuf[64];
	
	unsigned char RaidoBuf[8];
	unsigned char i,j;
	
	printf("************* input data *************\r\n");
	for (i=0;i<64;i++){
		inbuf[i]	=	100+i;
		printf("0x%02x ",inbuf[i]);
	}
	printf("\r\n************* input data end *************\n");
	
	printf("\n************* output data *************\r\n");
	for (i=0;i<64;i++){
		inbuf[i]	=	100+i;
		printf("0x%02x ",inbuf[i]);
	}
	printf("\r\n************* output data end *************\n");
	memset(RaidoBuf,55,sizeof(RaidoBuf));
	deskey(key, EN0);						//加密
	des(&RaidoBuf,&outbuf);

/*	
		for(j=0;j<10;j++)
			RaidoBuf[j] = 0;

		j=0;
		for(i=0;i<3;i++)
		{
			deskey(key, DE1);                     	 //解密
			des(&s_rec_buf[j+2], RaidoBuf);           //源，目标
			memcpy(&rec_buf[j],RaidoBuf,8);
			j=j+8;
		}
*/

	for(;i< sizeof(RaidoBuf);i ++)
		printf("des en Value = %d\n",outbuf[i]);
		
}