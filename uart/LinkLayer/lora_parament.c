
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <dirent.h>

#include      "lora_parament.h"
#include 			"bdb/forlder_util.h"
#include 			"util/code_value_change.h"
#include 			"socket/socket_protocol_thread.h"
#include			"uart/xxx_channel_send_thread.h"

const char *channel_prm[]={
	  "auto_config_number=0\n"
    "device_total_chn_1=0\n",
    "device_total_chn_2=0\n",
    "device_total_chn_3=0\n",
    "device_total_chn_4=0\n",
};

//-------	public -------
ChannelValueList	mChannelValueList;

/***********************************************************************
 * @brief				get_lora_channel_number
 * @param[in]		void *args, uint8_t **r_data, int *r_len
 * @return 			0 success , non zero  faile
 **********************************************************************/
int8_t	init_lora_channel_config(void)
{
		if( mChannelValueList.init_flag		==	1 )
			return	0;
		DIR *dir;
    struct dirent *catlog;
    char	*FilePath = "./conf/";
    create_multi_dir(FilePath);
  
    dir = opendir(FilePath);
    if(dir==NULL){
        printf("opendir %s failed!\n",FilePath);
        return  -1;
    }
    catlog = readdir(dir);
    if(catlog == NULL){
        printf("readdir %s failed!\n",FilePath);
        return -1;
    }
    closedir(dir);
    
    char		buffer[4096];
    int 		fd;
    char		*fileName	= "conf/AutoConfigPara.txt";
    int			cnt;
    char 		*sPtr,*ePtr;
    uint32_t length_w,length_r;
    
    uint32_t 	ret =   sizeof(buffer);
    fd = open(fileName,O_RDWR|O_CREAT,0777);
    if( fd < 0 ) {
        printf("open %s failed!\n",fileName);
        return	-1;
    }
    length_r = read(fd,buffer,ret );
    close(fd);
		sPtr	=   buffer; //input prm

    if(length_r < sizeof(channel_prm)){
         printf("%s,line=%d,configContextFile is empty or error \n",__FILE__,__LINE__);
         fd = open(fileName,O_RDWR|O_CREAT,0777);
         if(fd < 0){
               printf("%s,line=%d,open %s File error\n",__FILE__,__LINE__,fileName);
               return	-1;
         }
         cnt	=	sizeof(channel_prm)/sizeof(const char*);
         for(uint8_t i=0;i < cnt;i++)                                           //init prm okay
         {
           ret	=	strlen(channel_prm[i]);
           length_w = write( fd,channel_prm[i],ret );
           if(length_w < ret ){
                printf("%s,line=%d,write %s File error,len=%d\n",__FILE__,__LINE__,fileName,ret);
                return	-1;
            }
         }
     		 close(fd);
         
         fd = open(fileName,O_RDWR|O_CREAT,0777);
			   if( fd < 0 ) {
			        printf("open %s failed!\n",fileName);
			        return	-1;
			    }
			   length_r = read(fd,buffer,ret );
			   close(fd);
				 sPtr	=   buffer; //input prm
    }
    
    sPtr 	= strstr(sPtr,"number=");
    if(sPtr  ==  NULL){
				mChannelValueList.autoConfigNum	=	0;
        return  -3;
    }else{
        sPtr	+=	sizeof("number=")-1;
    		ePtr	=	strchr(sPtr,'\n');
    		if(ePtr == NULL){
    			printf("%s,line=%d,auto config chn prm error.process exit..\n",__FILE__,__LINE__);
      		return	-4;
    		}
      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      mChannelValueList.autoConfigNum	=	ret;
    }

    sPtr 	= strstr(ePtr,"chn_1=");
    if(sPtr  ==  NULL){
				mChannelValueList.chn1.DeviceTotal	=	0;
        return  -5;
    }else{
        sPtr	+=	sizeof("chn_1=")-1;
    		ePtr	=	strchr(sPtr,'\n');
    		if(ePtr == NULL){
    			printf("%s,line=%d,chn 1 prm error.process exit..\n",__FILE__,__LINE__);
      		return	-6;
    		}
      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      mChannelValueList.chn1.DeviceTotal	=	ret;
    }
    
    sPtr 	= strstr(ePtr,"chn_2=");
    if(sPtr  ==  NULL){
				mChannelValueList.chn2.DeviceTotal	=	0;
        return  -7;
    }else{
        sPtr	+=	sizeof("chn_2=")-1;
    		ePtr	=	strchr(sPtr,'\n');
    		if(ePtr == NULL){
    			printf("%s,line=%d,chn 2 prm error.process exit..\n",__FILE__,__LINE__);
      		return	-8;
    		}
      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      mChannelValueList.chn2.DeviceTotal	=	ret;
    }

    sPtr 	= strstr(ePtr,"chn_3=");
    if(sPtr  ==  NULL){
				mChannelValueList.chn3.DeviceTotal	=	0;
        return  -9;
    }else{
        sPtr	+=	sizeof("chn_3=")-1;
    		ePtr	=	strchr(sPtr,'\n');
    		if(ePtr == NULL){
    			printf("%s,line=%d,chn 3 prm error.process exit..\n",__FILE__,__LINE__);
      		return	-10;
    		}
      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      mChannelValueList.chn3.DeviceTotal	=	ret;
    }

    sPtr 	= strstr(ePtr,"chn_4=");
    if(sPtr  ==  NULL){
				mChannelValueList.chn4.DeviceTotal	=	0;
        return  -11;
    }else{
        sPtr	+=	sizeof("chn_4=")-1;
    		ePtr	=	strchr(sPtr,'\n');
    		if(ePtr == NULL){
    			printf("%s,line=%d,chn 4 prm error.process exit..\n",__FILE__,__LINE__);
      		return	-12;
    		}
      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      mChannelValueList.chn4.DeviceTotal	=	ret;
    }
	
//	  memset(&mChannelValueList,0x0,sizeof(ChannelValueList));
		mChannelValueList.init_flag							=	1;
		return	0;
}

 /***********************************************************************
   * @brief				update_channel_prm_to_file
   * @param[in]		void
   * @param[out]
   * @return 			channel number, 0 is faile,0xff is full error
   **********************************************************************/
int8_t	updata_channel_prm_to_file(void)
{
//	  char		buffer[4096];
	  char		icontext[128];
    int 		fd;
    char		*fileName	= "conf/AutoConfigPara.txt";
    int			cnt;
    //char 		*sPtr,*ePtr;
    uint32_t length_w,ret;
    
	 	fd = open(fileName,O_RDWR|O_CREAT,0777);
    if(fd < 0){
          printf("%s,line=%d,open %s File error\n",__FILE__,__LINE__,fileName);
          return	-1;
    }
    
    cnt	=	sizeof(channel_prm)/sizeof(const char*);
    for(uint8_t i=0;i < cnt;i++){
           memset(icontext,0,sizeof(icontext));
           switch(i){
           	case 0:
                sprintf(icontext,"auto_config_number=%d\n",mChannelValueList.autoConfigNum);
           		ret	=	strlen(icontext);
           		break;
           	case 1:
                sprintf(icontext,"device_total_chn_1=%d\n",mChannelValueList.chn1.DeviceTotal);
							ret	=	strlen(icontext);
           		break;         
           	case 2:
                sprintf(icontext,"device_total_chn_2=%d\n",mChannelValueList.chn2.DeviceTotal);
           		ret	=	strlen(icontext);           		
           		break;
           	case 3:
           		sprintf(icontext,"device_total_chn_3=%d\n",mChannelValueList.chn3.DeviceTotal);
           		ret	=	strlen(icontext);           		
           		break;
           	case 4:
           		sprintf(icontext,"device_total_chn_4=%d\n",mChannelValueList.chn4.DeviceTotal);
           		ret	=	strlen(icontext);           		
           		break;
           	default:
           		sprintf(icontext,"auto config index error=%d\n",i);
           		printf("%s,line=%d,auto config index error=%d\n",__FILE__,__LINE__,i);
           		ret	=	strlen(icontext);
           		break;
          }
      length_w = write( fd,icontext,ret );
      if(length_w < ret ){
                printf("%s,line=%d,write %s File error,len=%d\n",__FILE__,__LINE__,fileName,ret);
                return	-1;
      }
    }
    
    close(fd);
    return	0;
}

/***********************************************************************
   * @brief				get_atuo_channel_number
   * @param[in]		void
   * @param[out]
   * @return 			channel number, 0 is faile,0xff is full error
   **********************************************************************/

uint8_t	get_atuo_channel_number(void)
   {
      		ChannelCorrespond		*pChannelCorrespond ;

          if ( mChannelValueList.init_flag		!=	1 )
              return 0;
          do{
              switch(mChannelValueList.autoConfigNum)
              {
                  case	1:
                      pChannelCorrespond	=	&mChannelValueList.chn1	;
                      break;
                  case	2:

                      pChannelCorrespond	=	&mChannelValueList.chn2	;
                      break;
                  case	3:

                      pChannelCorrespond	=	&mChannelValueList.chn3	;
                      break;
                  case	4:
                      pChannelCorrespond	=	&mChannelValueList.chn4	;
                      break;

                  default:
                      mChannelValueList.autoConfigNum				=	1;
                      pChannelCorrespond	=	&mChannelValueList.chn1	;
                      break;
              }
              
              if ( pChannelCorrespond->DeviceTotal <	ChannelMaxDevice){
              		pChannelCorrespond->DeviceTotal	+=	1;
                  return	mChannelValueList.autoConfigNum;
                }
              else
                  mChannelValueList.autoConfigNum				+=	1;
                  
          }while( mChannelValueList.autoConfigNum < MaxChannelNumber );

          return	0xff;

  }


/***********************************************************************
 * @brief				get_lora_channel_Frequency
 * @param[in]		uint8_t		chn
 * @param[out]	uint32_t	*FreVal
 * @return 			1 success
 **********************************************************************/
 uint8_t	get_lora_channel_Frequency(uint8_t	chn,uint32_t	*FreVal)
 {
    ChannelValueList *list	=	&mChannelValueList;
  	if(list->init_flag != 1)
 		{
 			printf("ERROR:%s,line:%d,non init lora channel parament!!,init_flag=%d\n",__FILE__,__LINE__,list->init_flag);
 			*FreVal		=	mUartPort[0].FreqValue;
 			return 0x0;
 		}
 		switch(chn)
 		{
 			case	1:
 			case	2:
 			case	3:
 			case	4:
 			case	5:
 				*FreVal		=	mUartPort[chn-1].FreqValue;
 				break;
 			default:
 				printf("ERROR:%s,line:%d,get_lora_channel_Frequency , sure set channel:%d !!\n",__FILE__,__LINE__,chn);
 				*FreVal		=	mUartPort[0].FreqValue;
 				return 0x0;
 		}
 		return	1;
}

/***********************************************************************
 * @brief				get_lora_channel_ByFrequency
 * @param[in]		uint32_t	FreVal
 * @param[in]		uint8_t		*chn
 * @return 			1 success
 **********************************************************************/
 uint8_t	get_lora_channel_ByFrequency(uint8_t	*chn,uint32_t	FreVal)
 {
    ChannelValueList *list	=	&mChannelValueList;
  	if(list->init_flag != 1){
 			printf("ERROR:%s,line:%d,non init lora channel parament!!,init_flag=%d\n",__FILE__,__LINE__,list->init_flag);
 			*chn		=	0x01;			//first channel
 			return 0x0;
 		}
 		if( FreVal		==	mUartPort[0].FreqValue )
 			*chn		=	0x01;			//first channel
 		else
 		if( FreVal		==	mUartPort[1].FreqValue )
 			*chn		=	0x02;
 		else
 		if( FreVal		==	mUartPort[2].FreqValue )
 			*chn		=	0x03;
 		else
 		if( FreVal		==	mUartPort[3].FreqValue )
 			*chn		=	0x04;
 		else
 		if( FreVal		==	mUartPort[4].FreqValue )
 			*chn		=	0x05;
 		else{
 			*chn		=	0x01;			//first channel
 			return 0;
 			}
 		return	1;
}

 /***********************************************************************
  * @brief				get_lora_channel_Hal_address
  * @param[in]		uint8_t	chn,uint32_t	*addr
  * @return 			1 success
  **********************************************************************/
  uint8_t	get_lora_channel_Hal_address(uint8_t	chn,uint16_t	*addr)
  {
     ChannelValueList *list	=	&mChannelValueList;
     if(list->init_flag != 1){
             printf("ERROR:%s,line:%d,non init lora channel parament!!,init_flag=%d\n",__FILE__,__LINE__,list->init_flag);
            *addr		=	mUartPort[0].GateWayAddress;
             return 0x0;
         }
         switch(chn)
         {
             case	1:
             case	2:
             case	3:
             case	4:
             case	5:
                 *addr		=	mUartPort[chn-1].GateWayAddress;
                 break;
             default:
                 printf("ERROR:%s,line:%d,get_lora_channel_Hal_address, sure set channel:%d !!\n",__FILE__,__LINE__,chn);
                 *addr		=	mUartPort[0].GateWayAddress;
                 return 0x0;
         }
         return	1;
 }
/***********************************************************************
  * @brief				get_lora_channel_Hal_AirRate
  * @param[in]		uint8_t	chn,uint32_t	*addr
  * @return 			1 success
  **********************************************************************/
  uint8_t	get_lora_channel_Hal_AirRate(uint8_t	chn,uint8_t	*AirRate)
  {
     ChannelValueList *list	=	&mChannelValueList;
     if(list->init_flag != 1)
         {
             printf("ERROR:%s,line:%d,non init lora channel parament!!,init_flag=%d\n",__FILE__,__LINE__,list->init_flag);
             *AirRate		=	mUartPort[0].AirRate;
             return 0x0;
         }
         switch(chn)
         {
             case	1:

             case	2:

             case	3:

             case	4:

             case	5:
                 *AirRate		=	mUartPort[chn-1].AirRate;
                 break;
             default:
                 printf("ERROR:%s,line:%d,lora_channel_set_error!!!sure set channel:%d !!\n",__FILE__,__LINE__,chn);
                 *AirRate		=	mUartPort[0].AirRate;
                 return 0x0;
         }
         return	1;
 }
 
/***********************************************************************
   * @brief				delete_locker_update_to_var
   * @param[in]		uint8_t chn //channel number
   * @param[out]
   * @return 			0 is success,non 0 is failure
   **********************************************************************/
int8_t	delete_locker_update_to_var(uint8_t chn)
{

    int8_t			cnt;
    cnt	=	0x00;
    switch(chn){
           	case 1:
           		if(mChannelValueList.chn1.DeviceTotal > 1)
           				mChannelValueList.chn1.DeviceTotal -=1;
           		else
           			cnt	=	-1;
           	break;
           	case 2:
           		if(mChannelValueList.chn2.DeviceTotal > 1)
           				mChannelValueList.chn2.DeviceTotal -=1;
	           	else
           				cnt	=	-2;
           		break;
           	case 3:
           		if(mChannelValueList.chn3.DeviceTotal > 1)
           			mChannelValueList.chn3.DeviceTotal -=1;           		
	           	else           	
           			cnt	=	-3;
           	break;
           	case 4:
           		if(mChannelValueList.chn4.DeviceTotal > 1)
           			mChannelValueList.chn4.DeviceTotal -=1;
           		else
           			cnt	=	-4;
           		break;
           	default:
           		cnt	=	-5;
           		printf("%s,line=%d,delete locker channel error rang\n",__FILE__,__LINE__);
           		break;
     }

		 if( cnt != 0x00 )
			return	cnt;
		else
    	return	0;
}

