
#include 	<string.h>
#include 	<stdlib.h>
#include 	<stdint.h>
#include 	<stdio.h>

//#include 	"Packet.h"

#include 	"Unpacket.h"



//============ public ============================



//------------------	private		--------------
static		uint16_t				mReceiveLength = 0;

//static		uint8_t					mSingleUnpackBuf[1050];
//static		uint8_t					mMultUnpackBuf[UnpackBufSize];
//static		socket_Unpack_info			mUnpack_info;

/**@brief   socket_data_unpack_create
 *
 * @details clear mUnpack_info entry information
 * 	input:void
 *	output:void
 */
/*
void		socket_data_unpack_create(void)
{
	memset((int8_t*)&mUnpack_info,0,sizeof(mUnpack_info));
	mUnpack_info.SingleUnpackBuf	=	mSingleUnpackBuf;
	mUnpack_info.MultUnpackBuf	=	mMultUnpackBuf;
}
*/

/**@brief   Function for the light initialization.
 *
 * @details Initializes all lights used by this application.
 */

int32_t mpbledemo2_get_md5(void)
{
	int32_t error_code = 0;
#if defined EAM_md5AndNoEnrypt || EAM_md5AndAesEnrypt
	char device_type[] = DEVICE_TYPE;
	char device_id[] = DEVICE_ID;
	char argv[sizeof(DEVICE_TYPE) + sizeof(DEVICE_ID) - 1];
	memcpy(argv,device_type,sizeof(DEVICE_TYPE));
/*when add the DEVICE_ID to DEVICE_TYPE, the offset shuld -1 to overwrite '\0'  at the end of DEVICE_TYPE */
	memcpy(argv + sizeof(DEVICE_TYPE)-1,device_id,sizeof(DEVICE_ID));
#ifdef CATCH_LOG	
	printf ( "\r\nDEVICE_TYPE and DEVICE_ID:%s\r\n",argv);
#endif
	error_code = md5(argv, md5_type_and_id);
#ifdef CATCH_LOG
	printf ( "\r\nMD5:");
	for ( uint8_t i = 0; i < 16; i++ )
	printf ( " %02x", md5_type_and_id[i] );
	putchar ( '\n' );
#endif
#endif
	return error_code;
}

/**@brief   Function decipher data Area.
 *
 * @details 
 * 	input:uint8_t *r_data, int *r_len
 *	output:uint8_t *r_data, int *r_len
 */


int32_t		decipherDataArea(uint8_t *r_data, uint16_t *r_len)
{
		
		uint8_t ibuf[sockEncryptSize],*ptr;

		uint16_t len;
		len = *r_len;
		ptr = ibuf;
		memcpy(ptr,r_data,len);
    return *r_len;
}

/********************************************************************************************************
**@brief   void UnChangeDefine(uint8_t *r_data, int *r_len)
 *
 * @details change code used by this application.
 *
 *	input:uint8_t *r_data, int *r_len
 *
 *	output:uint8_t *r_data, int *r_len
********************************************************************************************************/
#define		unChangeCodeSize		1050

void UnChangeDefine(uint8_t *r_data, uint32_t *r_len)
{
	uint32_t d,len;
  uint8_t *ptr,ibuf[unChangeCodeSize];
	len = *r_len;
	ptr = ibuf;
//	ptr= (uint8_t *)malloc(len);
//	if ( ptr == NULL )
//	{free(ptr);return;}
	memcpy(ptr,r_data,len);
	
	for(d=0;d<len;d++)
	{
		if(*ptr == 0x7d)
			{
				ptr++;
				switch(*ptr)
				{
					case 0x01:
						*r_data++ = 0x7d;
									*r_len -= 1;
						break;
					case 0x02:
						*r_data++ = 0xfc;
									*r_len -= 1;
						break;
					default:
						
						break;
				}
			}
		else
			{
				*r_data++ = *ptr;
			}
		ptr++;
	}
	//free(ptr);
}


/********************************************************************************************************
**@brief    socket_data_unpack_func
 *
 * @details unpack data used by this application.
 *
 *	input:uint8_t *r_data, int *r_len
 *
 *	output:uint8_t *r_data =>fix_header, void	*args=>Unpack_info
 *	
 *	return: unpack status code
********************************************************************************************************/
#define			enable_printf_receive_data		1

uint32_t socket_data_unpack_func(void	*args, uint8_t *data, uint16_t *r_len)
{
    uint8_t *ptr;
    uint16_t 			ilen, len,val16;
    uint32_t  		Crc32,val;
    uint8_t			u8;
    struct VirtualSocket*	pVirtualSocket	=	(struct VirtualSocket*)args;
		NSFPFixHead 	*fix_head;	// = (NSFPFixHead*)data;
		ilen = *r_len;
		
    #if enable_printf_receive_data == 1
    printf("%s,line:%d,socket receive data: \n ",__FILE__,__LINE__);
    for(int16_t i=0;i<ilen;i++)
      printf("%02x ",*(data + i) );
    printf("\n %s,line:%d,socket receive length=%d \n ",__FILE__,__LINE__,ilen);
    #endif
		   
    if(ilen < sizeof(NSFPFixHead)){
        printf("%s,line=%d, errorCodeUnpackLengthMin,length=%d!!\n",__FILE__,__LINE__,ilen);
        return errorCodeUnpackLengthMin;
      }
//- - - - - 
		ptr	=	data;
		fix_head = (NSFPFixHead*)ptr;
		while(fix_head->bMagicNumber != htons(MAGIC_NUMBER)){
			if( (--ilen) < sizeof(NSFPFixHead)){
				printf("%s,line=%d, search magic code error, length=%d!!\n",__FILE__,__LINE__,ilen);
        return errorCodeUnpackLengthMin;
			}
			fix_head = (NSFPFixHead*)(++ptr);
		}
      
    if(fix_head->bMagicNumber != htons(MAGIC_NUMBER)){
        printf("%s,line=%d , errorCodeMagicHeader=%d !\n",__FILE__,__LINE__,errorCodeMagicHeader);
        return errorCodeMagicHeader;
      }
    if(fix_head->Ver !=htons( 0x0001)){
				printf("---%s,line=%d , errorCodeUnpackVersion=%d    !!\n",__FILE__,__LINE__,errorCodeUnpackVersion);
        return errorCodeUnpackVersion;
      }
      
    len = htons(fix_head->nLength);
    if(len < 6){
				printf("---%s,line=%d , errorCodeUnpackLengthMin=%d !!\n",__FILE__,__LINE__,errorCodeUnpackLengthMin);
    		return errorCodeUnpackLengthMin;
    	}
		u8 = len % 8;
		if (u8 != 0 )			
			u8 = 8 - u8;
    val16 = *(uint16_t*)(ptr + NFSP_FIX_LENGTH + len + u8);
    if( val16 != htons(MAGIC_TAIL) ){
				printf("---%s,line=%d , length error non find magic tail=%d !!\n",__FILE__,__LINE__,u8);
    		return errorCodeMagicTail;	
    	}
		//=====  decipher  data area ====
    ilen =   len + u8;
    decipherDataArea( ptr + NFSP_FIX_LENGTH, &ilen);  										//decipher data
		
		//=====  check  crc32 ====
		Crc32=crc32(CRC32InitVal, ptr, HEADER_FIX_LENGTH + len + u8);							//data include 0xfc;
		//==== get crc value =====
		
    val = *(ptr +  HEADER_FIX_LENGTH + len + u8  );
	  val <<= 8;
    val |= *(ptr + HEADER_FIX_LENGTH + len + u8  + 1 );
	  val <<= 8;
    val |= *(ptr + HEADER_FIX_LENGTH + len + u8  + 2 );
    val <<= 8;
    val |= *(ptr + HEADER_FIX_LENGTH + len + u8  + 3 );

    if(Crc32 != val){
        printf("---%s,line=%d ,ilen=%d,Crc32=%#8x\n",__FILE__,__LINE__,ilen,Crc32);
            return	errorCodeCrcCheck;
        }

		/* mul frame 
		*/
		
     if (fix_head->FrameTotal > 1)
		{//mul frame data
			if (fix_head->FrameTotal > Socket_MAX_Frame){
          printf("%s,%d , frame total over rang error \n",__FILE__,__LINE__);
					return	errorCodeFrameTotal;
				}
			if (fix_head->FrameSequ > fix_head->FrameTotal){
        printf("%s,%d , frame total number error \n",__FILE__,__LINE__);
				return	errorCodeFrameNumber;
			}
/*	,CRC32
      if((pVirtualSocket->last_fix_header.WorkType != fix_head->WorkType)&&(fix_head->FrameSequ != 1)){		//work type diffrent error,non receive data
          printf("%s,%d ,mult frame Work Type diff  error \n",__FILE__,__LINE__);
					return errorWorkTypediff;  //next work manager destroy socket_data_unpack_create		
				}
*/
			if(fix_head->FrameSequ == fix_head->FrameTotal){
				//ilen	=	htons(fix_head->nLength);
				memcpy((pVirtualSocket->MultUnpackBuf + mReceiveLength), (ptr + FRAME_DATA_AREA), ilen);
				mReceiveLength += ilen;
				pVirtualSocket->MultUnpackLength = mReceiveLength;
				//memset(&pVirtualSocket->last_fix_header,0,sizeof(pVirtualSocket->last_fix_header));
				#ifdef	Debug_Level
				printf("%s,line:%d  frameTotal=%d,frameNumber=%d,len=%d ,receive okay..\n",__FILE__,__LINE__,fix_head->FrameTotal,fix_head->FrameSequ,mReceiveLength);
				#endif
				mReceiveLength	=	0	;
        return MULTI_RECEIVE_SUCCESS;
			}
      if(fix_head->FrameSequ == 1)				//error frame lose data!!!
			{
				mReceiveLength	=	0;
				memcpy((pVirtualSocket->MultUnpackBuf + mReceiveLength),ptr,ilen+FRAME_DATA_AREA);
				mReceiveLength += ilen+FRAME_DATA_AREA;
				memcpy((char*)&(pVirtualSocket->last_fix_header), (char*)fix_head,NFSP_FIX_LENGTH);		//non MagicTail
				#ifdef	Debug_Level
				printf("%s,line:%d  frameTotal=%d,frameNumber=%d,len=%d\n",__FILE__,__LINE__,fix_head->FrameTotal,fix_head->FrameSequ,mReceiveLength);
				#endif
        return SOCKET_WAIT_RECEIVING;
			}else{
				memcpy((pVirtualSocket->MultUnpackBuf + mReceiveLength),(ptr + FRAME_DATA_AREA),ilen);
				mReceiveLength += ilen;
				memcpy((char*)&(pVirtualSocket->last_fix_header), (char*)fix_head,NFSP_FIX_LENGTH);		//non MagicTail
				#ifdef	Debug_Level
				printf("%s,line:%d  frameTotal=%d,frameNumber=%d,len=%d\n",__FILE__,__LINE__,fix_head->FrameTotal,fix_head->FrameSequ,mReceiveLength);
				#endif
        return SOCKET_WAIT_RECEIVING;
			}
		}
		else{//single	frame
			/*
         pVirtualSocket->SingleUnpackLength =htons(fix_head->nLength);
         memcpy((pVirtualSocket->SingleUnpackBuf),(ptr+FRAME_DATA_AREA),pVirtualSocket->SingleUnpackLength);
         memcpy((char*)&(pVirtualSocket->last_fix_header), (char*)fix_head,NFSP_FIX_LENGTH);		//non MagicTail
         */
         //ptr = (int8_t*)data;
         pVirtualSocket->SingleUnpackLength = len ;
         memcpy((pVirtualSocket->SingleUnpackBuf),(char*)ptr,( HEADER_FIX_LENGTH + len + u8 + sizeof(Crc32)) );
         //memcpy((char*)&(pVirtualSocket->last_fix_header), (char*)fix_head,NFSP_FIX_LENGTH);		//non MagicTail
         //memcpy((pVirtualSocket->SingleUnpackBuf),(char*)ptr,*r_len);
					#ifdef	Debug_Level
					printf("%s,line:%d,receive length=%d, data length=%d,receive okay.. \n",__FILE__,__LINE__,*r_len,len);
					#endif
          return SINGLE_RECEIVE_SUCCESS;
		}
	
	}
