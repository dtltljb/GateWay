#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <dirent.h>

#include <unistd.h>

#include 		"forlder_util.h"
#include 		"FileManager.h"
#include 		"link_table.h"

#include 		"socket/AppLayer/keep_alive_handle.h"
#include 		"socket/VirtualPortMonitor.h"
#include 		"socket/pub/util.h"
#include 		"util/code_value_change.h"
#include 		"uart/socket_task_distributer.h"
#include 		"socket/VirtualPortMonitor.h"
#include 		"uart/LinkLayer/UartOpration.h"

const char *Configfile[]=
{
    "server_ip=39.105.206.239\n",
    //"server_ip=192.168.0.103\n",
    "server_port=9999\n",
    "local_ip=192.168.1.174\n",
    "local_port=5555\n",
    "Local_CSN=12345678\n",
    "keep_alive=60\n"
    "TimeZone=UTC-11\n",
};

const	char *RecordConfig[]={
	"rd_index=0\n",
	"wd_index=100\n",
};



#if Debug_ubuntu_Level	== 1
	char	*STOREPATH	=	"/home/lijiabo/U/";
#else
	char	*STOREPATH	=	"/media/mmcblk0p1/data/";
#endif

static const int ServerPort = 9999;
static const char	*ServerIp = "39.105.206.239";

/*
*		write file context
*		文件写入输出日期作为文件名,日期发生变化就创建新文件,无需特殊处理。
*		input 	para:	filename 存储文件名, context数据
*		output param: 0 success, not 0 failure
******************************************************************/

static	int32_t	write_file_context(char	*filename,void	*log)
{

   if( (filename == NULL)||(log == NULL) ){
        printf("%s,%d, input parament is null \n",__FILE__,__LINE__);
        return	-1;
		}

 	DIR *dir;
  struct dirent *catlog;

//-	-	-	-	-	-	-	-	-		-		-	-		-		-			-			-
  uint32_t length_w;
  int fd;
  
	char	file_path_name[128];
  Context_t  Log;																				//Context from link_table.h
  memcpy( (char*)&Log,log,sizeof(struct	Context_s));
/*
	pLog	=	&Log;
  memcpy( (char*)pLog->historyData,sbuf,size);
	Get_CurBCDTime7((char*)pLog->date);
  pLog->channel	=	channel;
*/
	memset(file_path_name,0,sizeof(file_path_name));
	strcpy(file_path_name,STOREPATH);
	strncat(file_path_name,filename,strlen(filename) );									//拼接字符串
	
  if((fd = open(file_path_name,O_RDWR|O_CREAT|O_APPEND,0777))<0){
       fprintf(stderr, "%s %d, open %s failed \n",__FILE__,__LINE__,file_path_name);
	   return -5;
    }
  length_w = write(fd,&Log,sizeof(struct	Context_s));
  if((length_w <= 0)||(length_w != sizeof(struct	Context_s))){
        fprintf(stderr, "%s %d,write none length \n",__FILE__,__LINE__);
        close(fd);
        return -6;
    }
  close(fd);

  return 0;    
}


/*
*		createFileName
******************************************************************/
static	int32_t		createFileName(char	*fileName)
{
		char	s[64];
		memset(s,0,sizeof(s));
    time_t tt;
    struct tm *curtime;
    time(&tt);
    curtime=localtime(&tt);
    sprintf(s,"log_%d_%d_%d.dat",curtime->tm_year+1900,curtime->tm_mon+1,curtime->tm_mday);
    strcpy(fileName,s);
    return 1;
}


/*
*		Run_Log_Context_Record
*	input:	void	*args		,VirtualSocket
*					void	*argu		,UartPort
*	output:	sucess	is 1
******************************************************************/

int32_t	Run_Log_Context_Record(void	*args,void	*argu)
{
	
   UartPort						*pUartPort		= (UartPort*)argu;
   struct VirtualSocket  *pVirtualSocket		=	(struct VirtualSocket*)args;

#if Debug_ubuntu_Level	== 1
     fprintf(stderr, "%s %d,socket value=%d \n",__FILE__,__LINE__,pVirtualSocket->clt_sock);
#else
    char s[64];
    sprintf(s, "%s %d,socket value=%d \n",__FILE__,__LINE__,pVirtualSocket->clt_sock);
#endif


  // non debug so that  disable	!!
    Context_t		iContext;
		struct node *pHead = getHead();
		iContext.channel	=	pUartPort->channelNumber;
		Get_CurBCDTime7((char*)iContext.date);
		memcpy( (char*)iContext.historyData,pUartPort->UartSingleReceiveBuf,sizeof(iContext.historyData));
		do{
		pHead	=	insertNodeToTail(pHead,(void*)&iContext);
		}while(pHead == NULL);

    return true;
}

/*
*		monitor_link_list_empty
******************************************************************/

int32_t	monitor_link_list_empty(void)
{
	struct node *pHead;
	pHead	=	getHead();
	if(pHead==NULL){
		printf("%s %d,link list head is empty..\n",__FILE__,__LINE__);
		return	-1;
	}
	//-	-	-	-	-	-	-	- check link list is empty	-	-	-	-	
	Context_t		iContext,*pContext;
	pContext	=	(void*)getHeadContext(pHead,(void*)&iContext);
	if(pContext == NULL){
		return -2;
	}
	//-	-	-	-		-	-	creat file name	-		-		-		-
	char	fileName[128];
	int32_t	ret;
	memset(fileName,0,sizeof(fileName));
	createFileName(fileName);
	ret	=	write_file_context(fileName,(void*)pContext);
	if(ret != 0){
		printf("%s,%d, write_file_context  run failur...... \n",__FILE__,__LINE__);
		return	-3;
	}
	else
		deleteHeadNode(pHead);
		return 0;
}

/*
*		function:	report_history_log_file
*		para	[in]:	char	*fileName,history	log file name
*								uint32_t fileLine,	report file line	
*		para	[out]:non 0 failure, = 1 is file empty, = 0 success
******************************************************************/
int32_t	report_history_log_file(char	*fileName,uint32_t *fileLine)
{
		Context_t		iContext;							//Context_t in link_table.h	
    uint32_t 		length_r;
    uint16_t		readLen;
		int32_t		ret;
		
		struct stat buf;
    int fd;
		if(mVirtualSocket.keep_alive_response != RESPONSE)
			return	-1;

		//-	-	-	-	-	-	
    fd = open(fileName,O_RDWR,0777);
    if( fd < 0 ) {
        printf("open %s failed!\n",fileName);
        return	-2;
    }

    length_r = lseek (fd,*fileLine * sizeof(Context_t),SEEK_CUR);
    readLen = sizeof(Context_t);
    readLen = read(fd,(char*)&iContext,readLen);
    if(readLen <= 0){
        printf("%s line%d, read err len\n",__FILE__,__LINE__);
        close(fd);
        return -3;
    }
		report_history_log_pack((UReportLogReq*)&iContext.historyData,iContext.channel);

		
		length_r += readLen;
    ret = fstat(fd, &buf);
    if( ret < 0){
	        printf("fstat err%s line%d \n",__FILE__,__LINE__);
	        close(fd);
	        return -4;
		}
		close(fd);
		//	-	-	-	-	-	end	file judge	-	-	-	-	-	-	-	 
    if(length_r	>=	buf.st_size)
      return 	1;
    else
        return	0;
}

/*
*		monitor_forlder_file_empty
******************************************************************/
#define		WAIT_HISTORY_LOG_MAX_TIME	2

static char		fileUpdateFlage	=	'n';	
static	char	fileName[128];
static	uint32_t mFileLine	=	0;
	
int32_t	monitor_forlder_file_empty(void)
{
	int32_t	res;
	char	*sPtr=NULL;
	
	if(mVirtualSocket.keep_alive_response == STARTUP)		//non link to server
		return	-1;
	if( fileUpdateFlage	== 'n' ){
		memset(fileName,0,sizeof(fileName));
		res	=	GetFileFromDir(STOREPATH,fileName);
		if(res == 0){
			sPtr 	= strstr(fileName,"log_");
	    if(sPtr  ==  NULL){
				if( remove(fileName))
							perror("remove");
	       return  -2;
	    }else{
	    	#ifdef		Debug_Level
				printf("%s,%d, GetFileFromDir  run  report...... \n",__FILE__,__LINE__);
				#endif
				fileUpdateFlage	= 'y';
				mFileLine	=	0;
				mVirtualSocket.history_log_response	=	0;
			}
		}
		else{
			
		}
	}
	else{
		if(mVirtualSocket.history_log_times > mVirtualSocket.base_time_count)
        res	=	(mVirtualSocket.history_log_times - mVirtualSocket.base_time_count);
		else
        res	=	(mVirtualSocket.base_time_count - mVirtualSocket.history_log_times);
		if(mVirtualSocket.history_log_response	==	RESPONSE){
			mFileLine	+=	1;
			mVirtualSocket.history_log_response	=	0;
		}
		if((res	<	WAIT_HISTORY_LOG_MAX_TIME )&&(mVirtualSocket.history_log_response	==	REQUEST))				//wait for history log response
			return	1;
		res = report_history_log_file(fileName,&mFileLine);
		if( res == 1 ){	//file empty rm file
							fileUpdateFlage	= 'n';
							if( remove(fileName))
								 perror("remove");
		}else if(res == 0){
			
		}
		mVirtualSocket.history_log_times	=	mVirtualSocket.base_time_count;
		mVirtualSocket.history_log_response		=	REQUEST;
	}
    return  0;
}

/*
*		config_file_context_record
******************************************************************/

uint8_t	config_file_context_record(void* args	)
{

   // UartPort							*pUartPort					= (UartPort*)argu;
    //printf("%s,%d,[ %s ] Run_Log_Context_Record \n",__FILE__,__LINE__,pUartPort->h_io);
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
    //printf("%s,%d,[ %s ] Run_Log_Context_Record \n",__FILE__,__LINE__,pVirtualSocket->LocalIp);
    //char 	*FilePath ;
    
    create_multi_dir(STOREPATH);				//data file create initilize
//  dir = opendir(STOREPATH);
//    if(dir==NULL){
//        printf("opendir %s failed!\n",STOREPATH);
//        return  -3;
//    }
//    catlog = readdir(dir);
//    if(catlog == NULL){
//        printf("readdir %s failed!\n",STOREPATH);
//        return -4;
//    }
//    closedir(dir);
    
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


    char	buffer[4096];

    uint32_t length_w,length_r;
   // ssize_t offset=1;

    //FILE * fd;
    int fd;
    char	*fileName	= "conf/ConfigFile.txt";
    int	cnt;
    char 		*sPtr,*ePtr;

    uint32_t 	ret =   sizeof(buffer);
    fd = open(fileName,O_RDWR|O_CREAT,0777);
    if( fd < 0 ) {
        printf("open %s failed!\n",fileName);
        return	-1;
    }
    length_r = read(fd,buffer,ret );
    close(fd);
		sPtr	=   buffer; //input prm

    if(length_r < sizeof(Configfile)){
         printf("%s,line=%d,configContextFile is empty or error \n",__FILE__,__LINE__);
         fd = open(fileName,O_RDWR|O_CREAT,0777);
         if(fd < 0)
         {
               printf("%s,line=%d,open %s File error\n",__FILE__,__LINE__,fileName);
               return	-1;
         }
         cnt		=		sizeof(Configfile)/sizeof(const char*);
         for(uint8_t i=0;i < cnt;i++)                                           //init prm okay
         {
           ret	=	strlen(Configfile[i]);
           length_w = write( fd,Configfile[i],ret );
           if(length_w < ret ){
                printf("%s,line=%d,write %s File error,len=%d\n",__FILE__,__LINE__,fileName,ret);
                return	-1;
            }
           }
         close(fd);
         memset(buffer,0,sizeof(buffer));

         fd = open(fileName,O_RDWR|O_CREAT,0777);
               if( fd < 0 ) {
                    printf("open %s failed!\n",fileName);
                    return	-1;
                }
               ret	=	sizeof(buffer);
			   length_r = read(fd,buffer,ret );
			   close(fd);
				 sPtr	=   buffer; //input prm
    }

    sPtr 	= strstr(sPtr,"server_ip=");
    if(sPtr  ==  NULL){
        strcpy(pVirtualSocket->ServerIp,ServerIp);
        pVirtualSocket->serverPort = ServerPort;
        return  -3;
    }else{
        sPtr	+=	sizeof("server_ip=")-1;
    		ePtr	=	strchr(sPtr,'\n');
    		if(ePtr == NULL){
    			printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
      		return	-4;
    		}
        strncpy(pVirtualSocket->ServerIp,sPtr,ePtr-sPtr);
    }
    //---- --- port -- --
    sPtr = strstr(sPtr,"server_port=")-1;
    if(sPtr == NULL){
    	printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
      return	-5;
    }else{
    	sPtr	+=	sizeof("server_port=");
	    ePtr	=	strchr(sPtr,'\n');
	    if ( ePtr == NULL ){
	      printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
	    	return	-6;
	    }

      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      pVirtualSocket->serverPort = ret;				//get port

    }
	    
    //---- --- Local CSN -- --
    sPtr = strstr(sPtr,"Local_CSN=")-1;
    if(sPtr == NULL){
    	printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
      return	-6;
    }else{
    	sPtr	+=	sizeof("Local_CSN=");
	    ePtr	=	strchr(sPtr,'\n');
	    if ( ePtr == NULL ){
	      printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
	    	return	-6;
	    }

      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      pVirtualSocket->Local_CSN = ret;				//get port

    }
    
    //---- --- keep alive -- --
    sPtr = strstr(sPtr,"keep_alive=")-1;
    if(sPtr == NULL){
    	printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
      return	-6;
    }else{
    	sPtr	+=	sizeof("keep_alive=");
	    ePtr	=	strchr(sPtr,'\n');
	    if ( ePtr == NULL ){
	      printf("%s,line=%d,config ip error.process exit..\n",__FILE__,__LINE__);
	    	return	-6;
	    }
      ret   =   Dec_To_Hex(sPtr,ePtr-sPtr);
      pVirtualSocket->keep_alive = ret;				//get port
      
    }
    
		//mVirtualSocket.keep_alive	=	60 ;// 60 s   
 
    //mVirtualSocket.Local_CSN    =   0x11223344;
  	
  	
    return 1;
}
