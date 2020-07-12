
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include 			"Packet.h"
#include 			"VirtualPortMonitor.h"
#include 			"pub/epb_MmBp.h"
#include 			"pub/util.h"
#include 			"pub/util_date.h"
#include 			"crc/crc32.h"

/********************************************************************************************************
**@brief		void ChangeDefine(uint8_t *r_data, int *r_len)
**input		:	 uint8_t *r_data, int *r_len
**output	:	 uint8_t *r_data, int *r_len
**	
**	
********************************************************************************************************/

void ChangeDefine(uint8_t *r_data, uint32_t *r_len)
{
	uint32_t d,len;
  uint8_t *ptr,ibuf[ChangeCodeSize];;
	len = *r_len;
	ptr = ibuf;
//	ptr= (uint8_t *)malloc(len);
	memcpy(ptr,r_data,len);
	
	for(d=0;d<len;d++)
	{
		if(*ptr==0x7d)
		{
			*r_data++ = 0x7d;
			*r_data++ = 0x01;
			*r_len += 1;
		}
		else if(*ptr==0xfc)
		{
			*r_data++ = 0x7d;
			*r_data++ = 0x02;
			*r_len += 1;
		}
		else
		{
			*r_data++ = *ptr;
		}
		ptr++;
	}
//	free(ptr);
}

/********************************************************************************************************
**	@brief	 uint32_t socket_data_pack_func(void *args, uint8_t **r_data, uint16_t *r_len)
**	input:	 void *args (NSFPFixHead);
**	output packet result:	 uint8_t **r_data, int *r_len
**	=> data length fill 8 bytes => crc32 => des encypt
********************************************************************************************************/

uint32_t socket_data_pack_func(void *args, uint8_t *r_data, uint16_t *r_len)
{
    uint32_t  	Crc32;
    uint8_t			val,buf[1050];
    uint8_t 		l_arrBcdtime[7] ={0};
 
    NSFPFixHead *fix_head = (NSFPFixHead *)args;   
    uint16_t		iLength		=	fix_head->nLength;
		fix_head->bMagicNumber = htons(MAGIC_NUMBER);
		fix_head->CSN					=	t_ntohl(fix_head->CSN); 
		fix_head->State				= htons(fix_head->State);
		fix_head->Ver 				= htons(0x0001);
		Get_CurBCDTime7((char*)l_arrBcdtime);
		memcpy(fix_head->TimeStamp,l_arrBcdtime,sizeof(fix_head->TimeStamp));
		fix_head->nLength		=	htons(fix_head->nLength);
  	fix_head->MagicTail = htons(MAGIC_TAIL);
  	
		memcpy(buf, fix_head,NFSP_FIX_LENGTH);
		val	=	iLength % 8 ;
		if(val !=	0)
		{
			val = 8-val;
      memset((fix_head->pData+iLength),	0,	val);
			}
		memcpy(&buf[NFSP_FIX_LENGTH], fix_head->pData, iLength+val);
		buf[NFSP_FIX_LENGTH+iLength+val]	=	(int8_t)fix_head->MagicTail ;
		buf[NFSP_FIX_LENGTH+iLength+val+1]	=	(int8_t)(fix_head->MagicTail>>8) ;
		
		Crc32	=	t_ntohl(	crc32(CRC32InitVal, buf,NFSP_FIX_LENGTH+iLength+val+sizeof(int16_t)));
		//Crc32	=		crc32(CRC32InitVal, buf,NFSP_FIX_LENGTH+iLength+val+sizeof(int16_t));
		memcpy(&buf[NFSP_FIX_LENGTH+iLength+val+sizeof(int16_t)], (char *)&Crc32, sizeof(Crc32));
		*r_len	=	NFSP_FIX_LENGTH	+	iLength +	val + sizeof(uint16_t)	+	sizeof(Crc32);
       // memcpy(*r_data,buf,*r_len);
       memcpy(r_data,buf,*r_len);
  	#ifdef Debug_Level
  	printf("send data:\n");
		for(int16_t i=0;i<*r_len;i++)
            printf("%02x ",r_data[i]);
    printf("\n%s,line:%d ,val=%d,crc32=%4X,*r_len=%d\n",__FILE__,__LINE__,val,Crc32,*r_len);        
		#endif
		
		return	1;
}
