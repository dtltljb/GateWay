
#include 	<string.h>
#include 	<stdlib.h>
#include 	<stdint.h>
#include 	<stdio.h>

#include    "UartPacket.h"
#include 		"UartUnpacket.h"
#include     "socket/pub/util.h"

#include    "crc/crc16.h"
//#include 		"socket/socket_protocol_thread.h"
#include    "LinkLayer/UartOpration.h"
//#include    "socket/VirtualPortMonitor.h"
#include    "des/d3des.h"

//============ private ============================

/**@brief   Function decipher data Area.
 *
 * @details 
 * 	input:uint8_t *cipher, int *r_len
 *	output:uint8_t *r_data
 */
 
uint8_t		UartDataDecipher(uint8_t *r_data,uint8_t *cipher, uint8_t r_len)
{
	uint8_t cipher_buf[UartEncryptSize],sbuf[UartEncryptSize];
  uint8_t	packet,i,j=0;
	uint16_t len;
	
	if( ( cipher	== NULL )||( r_data == NULL ) ){
		printf("%s,line:%d  cipher or r_data is null, error \n",__FILE__,__LINE__);
		return 1;
	}
    len = r_len;
	if(len%8){
    printf("%s,line:%d, cipher data length%%8=0x%d error \n",__FILE__,__LINE__,len);
		return 1;
	}
	
	memset(sbuf,0,sizeof(sbuf));
	memcpy(cipher_buf, cipher, len);
	packet	=	len/8;
	for(i=0;i<packet;i++)
  {
		 deskey(Cipher_key, DE1);
     des(cipher_buf+j, sbuf+j);                    			//key1 en
  	 deskey(Cipher_key+8, EN0);
     des(sbuf+j,cipher_buf+j);                    		 //key2 de
		 deskey(Cipher_key, DE1);
     des(cipher_buf+j, sbuf+j);                    		//key1 en
		 j=j+8;
	}
		#if	Debug_Level == 1
				printf("\r\n %s,line:%d,cipher data context:vv \n",__FILE__,__LINE__);
        for(uint8_t n = 0;n<len;n++)
           printf("%02x ",sbuf[n]);
				printf("\r\n %s,line:%d,decipher data length:%d \n",__FILE__,__LINE__,len);
		#endif
		memcpy(r_data,sbuf,r_len);
		return	1;
}

/*
uint8_t UartDataDecipher(uint8_t *PlainTextBuf,uint8_t *CipherTextBuf,uint8_t EnLen)
{
    uint8_t i;
    uint8_t j=0;
    uint8_t data1[8];
    uint8_t TCipherBuf[8];
    uint8_t EnPacket = 0;

//    CipherTextBuf[0] = 0x40;
//    CipherTextBuf[1] = 0x37;
//    CipherTextBuf[2] = 0xD4;
//    CipherTextBuf[3] = 0xC0;
//    CipherTextBuf[4] = 0x68;
//    CipherTextBuf[5] = 0x73;
//    CipherTextBuf[6] = 0x42;
//    CipherTextBuf[7] = 0x39;
//    CipherTextBuf[8] = 0x08;
//    CipherTextBuf[9] = 0x53;
//    CipherTextBuf[10] = 0xFD;
//    CipherTextBuf[11] = 0x0A;
//    CipherTextBuf[12] = 0x24;
//    CipherTextBuf[13] = 0xE1;
//    CipherTextBuf[14] = 0xBA;
//    CipherTextBuf[15] = 0xBF;

    for(i=0;i<8;i++)
        TCipherBuf[i] = 0x00;

    EnPacket = (EnLen/8);
   if((EnLen%8)!=0)
    EnPacket++;
  for(i=0;i<EnPacket;i++)
  {
         memcpy(TCipherBuf,CipherTextBuf+j,8);    //è?8×??ú?÷??

         deskey(Cipher_key, DE1);
     des(TCipherBuf, data1);                    //key1 ?a?ü

     deskey(Cipher_key+8, EN0);
     des(data1,TCipherBuf);                     //key2 ?ó?ü

         deskey(Cipher_key, DE1);
     des(TCipherBuf, data1);                    //key1 ?a?ü;

         memcpy(PlainTextBuf+j,data1,8);   			// ±￡′??÷??
         j=j+8;
    }
  return 1;
}
*/

/*
* 1) Receive Uart data
* 2) crc check out
*	3) des DE result 
* 4) out put receive context
* 5) input:uint8_t **ptr,uint8_t *Datalen
* 6) output:uint8_t **Rbuf,uint16_t *RLen  
*/

/********************************************************************************************************
**@brief    uart_data_unpack_func
 *
 * @details unpack data used by this application.
 *
 *	input:uint8_t *r_data, int *r_len
 *
 *	output:uint8_t *r_data, int *r_len
 *	
 *	return: unpack status code
********************************************************************************************************/
uint8_t uart_data_unpack_func(void	*argv,uint8_t *r_data, uint8_t *r_len)
{
    uint8_t			res,ilen;
	uint8_t			decipherArea[255];
	uint16_t		crc,crc_r;
  UartFrame 	*pUartFrame	=	(UartFrame*)r_data;
  ilen  =   *r_len;

  
  #if Debug_Level == 1
//  uint8_t   *ptr;
//            ptr		=		r_data;
//			for(int16_t i=0;i<*r_len;i++)
//         printf(" %02x",*(ptr+i));
//		   printf("\n %s,line:%d,uart receive len=%d data ok\n",__FILE__,__LINE__, *r_len);
    #endif

    if(ilen < sizeof(UartFrame)){
			printf("%s%d  Frame Length Err\n",__FILE__,__LINE__);
			return	UnpackLengthErrorCode;
		}
	if (pUartFrame->frameLength > (_MAX_RecevieSize-sizeof(UartFrame)-sizeof(crc))){
			printf("%s,%d  Frame Length Err \n",__FILE__,__LINE__);
			return	UnpackLengthErrorCode;
		}
	if (pUartFrame->App_Layer_head != _FRAME_HEAD){
			printf("%s,%d  Frame Header Err  \n",__FILE__,__LINE__);
			return	MagicHeaderErrorCode;
		}
	if (pUartFrame->protoVer != _Portocol_VER){
			printf("%s,%d  Protocol Version Err \n",__FILE__,__LINE__);
			return	UnpackVersionErrorCode;
		}
		//---------		decipher data context	---------------
    if(pUartFrame->frameLength%8 != 0)
        ilen	=	pUartFrame->frameLength + ( 8- ((pUartFrame->frameLength%8)) );
    else
        ilen	=	pUartFrame->frameLength ;
        
   #if  disable_decipher_debug == 1
		res	=	UartDataDecipher(decipherArea,r_data+UART_FIX_LENGTH, ilen);
		if(res != 1){
			printf("%s,%d frame Cipher error\n",__FILE__,__LINE__);
			return	EncrypErrorCode;
			}
		memcpy(r_data+UART_FIX_LENGTH,decipherArea, ilen);							//decipher data rewrite r_data
		#endif
		
		//---------	crc	check data context	---------------	
    crc	=	(uint16_t)*(r_data + *r_len - 2);
    crc <<= 8;
    crc	|=	*(r_data +	*r_len - 1);
    
    crc_r	= 	crc16(r_data+LORA_ADDRESS_SIZE,(*r_len - sizeof(crc)-LORA_ADDRESS_SIZE)) ;
    if (crc != crc_r){
            printf("%s,%d  Frame crc Check failure,recv_crc:%02x,cal_crc:%02x\n",__FILE__,__LINE__,crc,crc_r);
			return	CrcCheckErrorCode;
		}
		
		if (pUartFrame->frameTotal > _MAX_Frame){
			printf("%s,%d  FrameTotalErr  failure\n",__FILE__,__LINE__);
			return	FrameTotalErrorCode;
		}
    else if (pUartFrame->frameTotal != 1){//mul frame data
				#if	Debug_Level == 1
				printf("%s,%d  mutl frame failure\n",__FILE__,__LINE__);
				#endif
			return	FrameTotalErrorCode;
		}else{
			UartPort*		pUartPort		=	(UartPort*)argv;
      memcpy(pUartPort->UartSingleReceiveBuf,(r_data+UART_FIX_LENGTH),pUartFrame->frameLength);
      pUartPort->UartSingleReceiveLength	=	pUartFrame->frameLength;
     // memcpy((uint8_t*)&pUartPort->last_fix_header,r_data,sizeof(UART_FIX_LENGTH));
//				#if	Debug_Level == 1
//				printf("%s,line:%d  flag\n",__FILE__,__LINE__);
//				#endif
			return UartSingleReceiverSuccess;
		}
}


/*
uint8_t Cipher_key[16];
uint8_t g_Send_Buf[16];
uint8_t   g_Receive_Buf[16];

Cipher_key[0] = 0x32;
        Cipher_key[1] = 0x32;
        Cipher_key[2] = 0x67;
        Cipher_key[3] = 0x28;
        Cipher_key[4] = 0x45;
        Cipher_key[5] = 0x37;
        Cipher_key[6] = 0x26;
        Cipher_key[7] = 0x15;

        Cipher_key[8] = 0x42;
        Cipher_key[9] = 0x38;
        Cipher_key[10] = 0x76;
        Cipher_key[11] = 0x29;
        Cipher_key[12] = 0x75;
        Cipher_key[13] = 0xc8;
        Cipher_key[14] = 0xd9;
        Cipher_key[15] = 0xe0;



    g_Send_Buf[0] = 0x40;
    g_Send_Buf[1] = 0x37;
    g_Send_Buf[2] = 0xD4;
    g_Send_Buf[3] = 0xC0;
    g_Send_Buf[4] = 0x68;
    g_Send_Buf[5] = 0x73;
    g_Send_Buf[6] = 0x42;
    g_Send_Buf[7] = 0x39;
    g_Send_Buf[8] = 0x08;
    g_Send_Buf[9] = 0x53;
    g_Send_Buf[10] = 0xFD;
    g_Send_Buf[11] = 0x0A;
    g_Send_Buf[12] = 0x24;
    g_Send_Buf[13] = 0xE1;
    g_Send_Buf[14] = 0xBA;
    g_Send_Buf[15] = 0xBF;

    UartDataDecipher(g_Receive_Buf,g_Send_Buf,16);
    */

