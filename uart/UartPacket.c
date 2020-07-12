#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#include    "UartPacket.h"
#include    "UartUnpacket.h"
#include    "crc/crc16.h"
#include    "socket/VirtualPortMonitor.h"
#include    "socket/pub/util.h"
#include		"util/pthread_data_copy.h"

#include    "des/d3des.h"

#define		UART_FINGER_BUF_SIZE		255

uint8_t 	Cipher_key[16]	=	{0x32,0x32,0x67,0x28,0x45,0x37,0x26,0x15,0x42,0x38,0x76,0x29,0x75,0xC8,0xD9,0xE0};
//key[0] = 0x32;key[1] = 0x32;key[2] = 0x67;key[3] = 0x28;key[4] = 0x45;key[5] = 0x37;key[6] = 0x26;key[7] = 0x15;
//key[8] = 0x42;key[9] = 0x38;key[10] = 0x76;key[11] = 0x29;key[12] = 0x75;key[13] = 0xc8;key[14] = 0xd9;key[15] = 0xe0;

/*
* 1) single pack,other process done
* 2) pack data frame
*	3) des Encrypt
* 4) crc check 
* 5) input: UartFrame *framePtr,uint8_t *ptr,uint8_t Datalen
* 6) output:uint8_t **Sbuf,uint8_t *SLen
*
*********************************************************************/

/**@brief   Function Encrpt data Area.
 *
 * @details 
 * 	input:uint8_t *r_data, int *r_len
 *	output:uint8_t *Encrpt
 */

uint8_t		UartDataEncrpt(uint8_t *Encrpt,uint8_t *r_data, uint8_t r_len)
{
	uint8_t cipher_buf[UartEncryptSize],sbuf[UartEncryptSize];
  uint8_t	packet,i,j=0;
	uint16_t len;
	
    if( ( Encrpt	== NULL )||( r_data == NULL ) ){
		
		printf("%s,line:%d  Encrpt or r_data is null, error \n",__FILE__,__LINE__);
		return 1;
	}
    len = r_len;

	if(len%8){
        printf("%s,line:%d, Encrpt data length%%8=0x%d error \n",__FILE__,__LINE__,len);
		return 1;
	}
	memset(sbuf,0,sizeof(sbuf));
	memcpy(cipher_buf, r_data, len);
	packet	=	len/8;
	for(i=0;i<packet;i++)
  {
		 deskey(Cipher_key, EN0);
     des(cipher_buf+j, sbuf+j);                    			//key1 en
     deskey(Cipher_key+8, DE1);
     des(sbuf+j,cipher_buf+j);                    		 //key2 de
		 deskey(Cipher_key, EN0);
     des(cipher_buf+j, sbuf+j);                    		//key1 en
		 j=j+8;
	}
		#if	Debug_Level == 1
//				printf("\r\n %s,line:%d,Encrpt data context:vv \n",__FILE__,__LINE__);
//        for(uint8_t n = 0;n<len;n++)
//           printf("%02x ",sbuf[n]);
				printf("\r\n %s,line:%d,Encrpt data length:%d \n",__FILE__,__LINE__,len);
		#endif
    memcpy(Encrpt,sbuf,r_len);
		return	1;
}

/********************************************************************************************************
**	@brief	 uint32_t uart_data_pack_func(void *args, uint8_t **r_data, uint16_t *r_len)
**	input:	 void *args (UartFrame);
**	output packet result:	 uint8_t **r_data, int *r_len
**	=> data length fill 8 bytes => crc32 => des encypt
********************************************************************************************************/

int8_t uart_data_pack_func(UartFrame *argv,uint8_t *r_data, uint8_t *r_len)
{
	uint8_t	buf[255],EncrptBuf[250],val=0;
	uint8_t *ptr;
	uint16_t	crc,ilen;
	UartFrame	*pUartFrame = (UartFrame*)argv;

	pUartFrame->Dst_Address				= htons(pUartFrame->Dst_Address);
	pUartFrame->App_Layer_head 		= _FRAME_HEAD;
	pUartFrame->protoVer					=	_Portocol_VER;
	memset(buf,0,sizeof(buf));
	memcpy(buf, pUartFrame,UART_FIX_LENGTH);
	
	val	=	pUartFrame->frameLength % 8 ;
	if(val !=	0)
	{
			val = 8-val;
			//memset((pUartFrame->dataPtr+*pUartFrame->dataPtr),	0,	val);
			}
	//memcpy(&buf[UART_FIX_LENGTH], pUartFrame->dataPtr, pUartFrame->frameLength+val);
	
	memcpy(&buf[UART_FIX_LENGTH], pUartFrame->dataPtr, pUartFrame->frameLength);
  crc	=	crc16(buf+LORA_ADDRESS_SIZE, UART_FIX_LENGTH+pUartFrame->frameLength+val-LORA_ADDRESS_SIZE);
	memcpy(&buf[UART_FIX_LENGTH+pUartFrame->frameLength+val], (char *)&crc, sizeof(crc));
  ilen	=	UART_FIX_LENGTH+pUartFrame->frameLength+val	+	sizeof(crc);
  *r_len = ilen;
	//--- Encrpt data---
  #if  disable_decipher_debug == 1
	UartDataEncrpt(EncrptBuf,&buf[UART_FIX_LENGTH],pUartFrame->frameLength+val);
	memcpy(&buf[UART_FIX_LENGTH],EncrptBuf,pUartFrame->frameLength+val);						//rewrite Encrpt data to buf
	#endif
	
  memcpy(r_data,buf,ilen);

  #ifdef Debug_Level
  ptr = r_data;
  printf("uart packet data out_len=%d\n",ilen);
	for(int16_t i=0;i<*r_len;i++)
           printf("%02x ",*(ptr+i) );
	printf("\n\r%s,line:%d ,val=%d,crc=%4X,*r_len=%d \n",__FILE__,__LINE__,val,crc,*r_len);           
	#endif
	return 1;
}



/********************************************************************************************************
**	@brief	 uint32_t uart_finger_data_pack_func(void *args, uint8_t *r_data, uint16_t *r_len)
**	input:	 void *args (UartFrame);
**	output packet result:	 uint8_t **r_data, int *r_len
**	=> data length fill 8 bytes => crc32 => des encypt
********************************************************************************************************/

int8_t uart_finger_data_pack_func(UartFrame *argv,uint8_t *r_data, uint8_t *r_len)
{
    uint8_t	buf[255];  //question input
//	uint8_t	*buf = malloc(UART_FINGER_BUF_SIZE);
//	if(buf == NULL){
//		printf("\n %s,line:%d ,malloc faile ,*buf=null\n",__FILE__,__LINE__);
//		return 0;
//	}
	uint8_t	val=0;
	uint16_t	crc;
	UartFrame	*pUartFrame = (UartFrame*)argv;

	pUartFrame->Dst_Address				= htons(pUartFrame->Dst_Address);
	pUartFrame->App_Layer_head 		= _FRAME_HEAD;
	pUartFrame->protoVer					=	_Portocol_VER;

    memset(buf,0,UART_FINGER_BUF_SIZE);
	memcpy(buf, pUartFrame,UART_FIX_LENGTH);
 
	val	=	pUartFrame->frameLength % 8 ;
	if(val !=	0)
	{
			val = 8-val;
			//memset((pUartFrame->dataPtr+*pUartFrame->dataPtr),	0,	val);
			}
	//memcpy(&buf[UART_FIX_LENGTH], pUartFrame->dataPtr, pUartFrame->frameLength+val);
	
	memcpy(&buf[UART_FIX_LENGTH], pUartFrame->dataPtr, pUartFrame->frameLength);
  crc	=	crc16(buf+LORA_ADDRESS_SIZE, UART_FIX_LENGTH+pUartFrame->frameLength+val-LORA_ADDRESS_SIZE);
	memcpy(&buf[UART_FIX_LENGTH+pUartFrame->frameLength+val], (char *)&crc, sizeof(crc));
  *r_len	=	UART_FIX_LENGTH+pUartFrame->frameLength+val	+	sizeof(crc);

  memcpy(r_data,buf,*r_len);
		//--- wait en des---
  #ifdef Debug_Level
  printf("uart packet finger data len=%d\n",*r_len);
	for(int16_t i=0;i<*r_len;i++)
           printf("%02x ",r_data[i]);
	printf("\n\r%s,line:%d ,val=%d,crc=%4X,*r_len=%d \n",__FILE__,__LINE__,val,crc,*r_len);           
	#endif
  //free(buf);
	return 1;
}
