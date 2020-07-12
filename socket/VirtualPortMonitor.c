/*
* first 	init  env  para
* second 	log in server,get token
* third		Read socket signal post
* fourth    judge socket status
* fiveth    destroy socket、token、signal、relink server
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include 		<fcntl.h>
#include 		<termios.h>

#include 		<sys/select.h>
#include 		<sys/time.h>

#include 		<sys/sem.h>
#include		<signal.h>
#include 		<pthread.h>



#include "socket/VirtualPortMonitor.h"
#include "socket/socket_protocol_thread.h"
#include "socket/AppLayer/keep_alive_handle.h"
#include "socket/pub/util_date.h"
#include "uart/LinkLayer/UartOpration.h"
#include "uart/LinkLayer/lora_parament.h"
#include "uart/xxx_channel_send_thread.h"
#include "uart/WorkCommandAnalysis.h"

#include    "bdb/popen.h"
#include    "bdb/FileManager.h"
#include    "bdb/link_table.h"
#include    "bdb/forlder_util.h"

#include    "uart/LinkLayer/uart_listener_thread.h"


//	------------- public para----------------------
struct 	VirtualSocket   mVirtualSocket={
                                          //.LocalIp						=		.lip,
                                .LocalIp						=		"127.0.0.1",
                                .localPort					=		505,
                                          //.ServerIp						=		.sip,
                                .ServerIp						=		"39.105.206.239",
                                .serverPort					=		0x9999,
                                .Local_CSN					=		0x12345678,
                                .keep_alive					=		60,

                                         // .pkey						=	{0x32,0x32,0x67,0x28,0x45,0x37,0x26,0x15,0x42,0x38,0x76,0x29,0x75,0xC8,0xD9,0xE0,'\n'},
                                };
                                
struct VirtualSocket	VirtualSocketCopyer[5];


sem_t 									sem_socket_receiver_id;
int32_t									mkeep_alive_time = 25;

struct itimerval tv, otv;

//	------------- private ----------------------
static	int socket_read_wait_time(int fd, int sec);

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};


//static  int sem_id = 0;
//static  int semaphore_p();
//static  int semaphore_v();
//static  int set_semvalue();
//static  void del_semvalue();

//const int ServerPort = 9999;
//const char	*ServerIp = "39.105.206.239";
//const char* ServerIp = "192.168.1.108";

static uint8_t recvBuf[1050];
//static uint8_t sendBuf[1050];

__sighandler_t	  broken_pipe_signal;

void broken_pipe_handle()
{
	broken_pipe_signal =(__sighandler_t)broken_pipe_handle;					//done broken pipe 
	signal(SIGPIPE,broken_pipe_signal);
	printf("==>>> server broken connect....,socket disconnet....\n");
	mVirtualSocket.socketStates = SOCKET_DISCONNECT;
				 //how long to run the first time
			  tv.it_value.tv_sec = 0;
			  tv.it_value.tv_usec = 0;
			   //after the first time, how long to run next time
			  tv.it_interval.tv_sec = 0;		//cancel time
			  tv.it_interval.tv_usec = 0;
			  if (setitimer(ITIMER_REAL, &tv, &otv) != 0) 
					printf("setitimer signal err %d\n", errno);
	close(mVirtualSocket.clt_sock);
}


void ctrlhandler( int signo)
{
 char	key_value;
  printf("Are you sure exit process ,put ( y / n )\n");
 /* close 相关操作*/
 key_value	=	getchar();
	if(key_value == 'y')
		 exit(0);
}

//---- monitor forlder & link table is nonempty -----  
void	*monitor_process_handler(void *pvoid)
{
	
	//----------debug start -----------------

//register_log_pack(&mVirtualSocket,NULL);
//10 01 fc 01 1c 01 01 30 00 00 10 01 02 80 0e 0e 0e 0f 0f 0f 0f 20 19 05 21 11 00 08 00 ff 02 4b 80 e0 db 00 00 00 00 ee fe 
/*    
  uint8_t	buf[]={0x30, 00, 00, 0x10, 01, 02, 0x80, 0x0e, 0x0e,0x0e, 0x0f, 0x0f, 0x0f, 0x0f, 0x20, 0x19, 05, 0x21, 0x11, 00, 0x08, 00, 0xff, 02, 0x4b, 0x80, 0xe0, 0xdb, 00, 00, 00, 00 };
	mUartPort[0].UartSingleReceiveLength	=	0x1c;
  memcpy(mUartPort[0].UartSingleReceiveBuf,buf,sizeof(buf));
	mVirtualSocket.pContext.p 	 	=   (void*)buf;									// because report log pack use server response context.wait ensure
  mVirtualSocket.pContext.len  =    0x1c;
  mVirtualSocket.clt_sock   =   1;
	res	=	work_command_analysis(&mUartPort[0],&mVirtualSocket);
*/

	while(1)
	{

/*
    Context_t		iContext;
		struct node *pHead = getHead();
		iContext.channel	=	1;
		Get_CurBCDTime7((char*)iContext.date);
    memcpy( (char*)iContext.historyData,mUartPort[0].UartSingleReceiveBuf,sizeof(iContext.historyData));
		pHead	=	insertNodeToTail(pHead,(void*)&iContext);

*/
//-----------debug end------------------
	
        monitor_link_list_empty();					//in FileManager.c
				
				monitor_forlder_file_empty();				//in FileManager.c
				
				usleep(500000);				//500 ms
		}
}

/*
* brief:main funtion
*/
#define	CONNECT_MAX_COUNT		5
int main()
//int VirtualPortMonitor()
{
  int32_t res = 0;
  struct sockaddr_in addr;
  int connect_fd;
  socklen_t addr_len;
  ssize_t size;
  uint16_t Seq = 1;
  time_t timep;

  memset((char*)&mVirtualSocket,0,sizeof(mVirtualSocket));
  static	char  sIp[20],lIp[20];
  mVirtualSocket.ServerIp	=	sIp;
  mVirtualSocket.LocalIp	=	lIp;
  config_file_context_record((void*)&mVirtualSocket);
  printf("name:%s,line:%d\n test!!\n ",__FILE__,__LINE__);

  struct node *p	=	CreateLinkList();
  if(p == NULL)
    printf("name:%s,line:%d CreateLinkList failure!!\n ",__FILE__,__LINE__);

  pthread_t HandleVirtualServerMonitor;
  res = pthread_create(&HandleVirtualServerMonitor,NULL,socket_protocol_thread,NULL);
  if (res ){
         printf("name:%s,line:%d\n SocketProtocol pthread_create failure!!\n ",__FILE__,__LINE__);
         exit(EXIT_FAILURE);
     }
	//create lora prm lock
  pthread_mutex_init(&socket_receive_mutex_lock,PTHREAD_MUTEX_TIMED_NP);
 	//create public data segment mutex lock
	pthread_mutex_init(&public_data_seg_mutex_lock,PTHREAD_MUTEX_TIMED_NP);

	broken_pipe_signal =(__sighandler_t)broken_pipe_handle;//done broken pipe 
	signal(SIGPIPE,broken_pipe_signal);

//create	semaphore_socket_receive_id
//  sem_id = semget((key_t)semaphore_socket_receive_id, 1, 0666 | IPC_CREAT);
//  if(!set_semvalue()){
//     fprintf(stderr, "Failed to initialize semaphore\n");
//     printf("name:%s,line:%d\n semaphore_socket_receive_id !!\n ",__FILE__,__LINE__);
//     exit(EXIT_FAILURE);
//   }

    //config	timer and signal 
  __sighandler_t	  heartHandle =(__sighandler_t)keep_alive_handle;//report_log_request_handle;
	signal(SIGALRM,heartHandle);		//send keep-alive-frame
	//-	-	-	-	-	- ctrl + c -	-	-	-	- - -
	__sighandler_t	ctrl_handler	=	(__sighandler_t)ctrlhandler;
 	signal(SIGINT,ctrl_handler);
  
  pthread_t HandleMonitor;
  res = pthread_create(&HandleMonitor,NULL,monitor_process_handler,NULL);
  if (res ){
         printf("name:%s,line:%d\n SocketProtocol pthread_create failure!!\n ",__FILE__,__LINE__);
         exit(EXIT_FAILURE);
     }

  printf("name:%s,line:%d\n SocketProtocol pthread_create failure!!\n ",__FILE__,__LINE__);
  
   while(1)
     {
pool_main:     	
     	 mVirtualSocket.keep_alive_response = STARTUP;		//initional heart flag
     	 mVirtualSocket.history_log_response		=	STARTUP;
     	 Seq = 0 ;
    //创建套接字,即创建socket
    do{
           mVirtualSocket.clt_sock = socket(AF_INET, SOCK_STREAM, 0);
           sleep(1);
           printf("%s, line:%d,ip=%s,port=%d connect...\n",__FILE__,__LINE__,mVirtualSocket.ServerIp,mVirtualSocket.serverPort);
       }while(mVirtualSocket.clt_sock < 0 );

       bzero(&addr,sizeof(struct sockaddr_in));
      //绑定信息，即命名socket
      addr.sin_family = AF_INET;
      addr.sin_port = htons(mVirtualSocket.serverPort);
      //addr.sin_port = htons(ServerPort);
      //inet_addr函数将用点分十进制字符串表示的IPv4地址转化为用网络
      //字节序整数表示的IPv4地址
      addr.sin_addr.s_addr = inet_addr(mVirtualSocket.ServerIp);
      //addr.sin_addr.s_addr = inet_addr(ServerIp);
      //发起连接
      addr_len = sizeof(addr);
      while((connect_fd = connect(mVirtualSocket.clt_sock, (struct sockaddr*)&addr, addr_len)) < 0) {
      		mVirtualSocket.socketStates = SOCKET_DISCONNECT;
          sleep(1);
          perror("connect error:-->");
          printf("connect server..... seq = %d,sock_id=%02x, sIp = %s,Port = %d \n",Seq++, mVirtualSocket.clt_sock, mVirtualSocket.ServerIp,mVirtualSocket.serverPort);
          if( Seq > CONNECT_MAX_COUNT ){
              goto pool_main;
            }
      }

			mVirtualSocket.socketStates	=	SOCKET_CONNECTED;
				 //how long to run the first time
				tv.it_value.tv_sec = 1;
			  tv.it_value.tv_usec = 0;
			   //after the first time, how long to run next time
			  tv.it_interval.tv_sec = 1;		//heart base time
				//tv.it_interval.tv_sec = mVirtualSocket.keep_alive;		//heart
			  tv.it_interval.tv_usec = 0;
			if (setitimer(ITIMER_REAL, &tv, &otv) != 0) 
					printf("setitimer signal err %d\n", errno);

      #ifdef		Debug_Level
      printf("connected server seq = %d,sIp = %s,Port = %d \n",Seq,mVirtualSocket.ServerIp,mVirtualSocket.serverPort);
      #endif

      while(1)
      {
      	 /*****loop start***/ 
        if( socket_read_wait_time(mVirtualSocket.clt_sock,1) == 0 ){
      		time(&timep);
      		//printf("%s,%d,read %s/%d timeout.%s\n",__FILE__,__LINE__,ServerIp,ServerPort,ctime(&timep));
      	}else{
        memset(recvBuf, '\0', sizeof(recvBuf));
        size=recv(mVirtualSocket.clt_sock,(char*)recvBuf ,sizeof(recvBuf), 0 );
        if(size <= 0){
        	perror("recevie error=>");
          //fprintf(stderr, "%s,line:%d,socket read error!!\n",__FILE__,__LINE__);
          printf("%s,line:%d socket err Info=> %s,read size =%d,reconnect server \n",__FILE__,__LINE__,(char*)stderr,(uint32_t)size);
          mVirtualSocket.socketStates	=	SOCKET_RECE_ERR;
          break;
         }else{

//             if (!semaphore_p() ){
//                 printf("%s,line:%d\n semaphore_p failure!!\n ",__FILE__,__LINE__);
//             }
//             res    =   size;
//             memcpy(socketReceiveBuf,recvBuf,res);
//             socketReceiveLength = res;
//             if(!semaphore_v()){
//                  printf("name:%s,line:%d\n semaphore_v failure!!\n ",__FILE__,__LINE__);
//              }

						int rc	=1;
						while(rc){
							rc	=	pthread_mutex_trylock(&socket_receive_mutex_lock);
							usleep(10000);
						}
            res    =   size;
            memcpy(socketReceiveBuf,recvBuf,res);
            socketReceiveLength = res;
            pthread_mutex_unlock(&socket_receive_mutex_lock);
           sem_post(&sem_socket_receiver_id);										//send semphore to socket done process
           }
          }
        

					
        /*****loop end***/ 
       }
       printf("%s,line:%d,sock=%d\n ",__FILE__,__LINE__,mVirtualSocket.clt_sock);
       close(mVirtualSocket.clt_sock);
     }

        void* pthreadResult;
        res  = pthread_join(HandleVirtualServerMonitor,&pthreadResult);
        if(res)
        {
            printf("name:%s\n,line:%d\n pthread_join!! ",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
        #if		Debug_Level==1
        printf("name:%s\n,line:%d\n finished!! ",__FILE__,__LINE__);
        #endif

        sem_destroy(&sem_socket_receiver_id);
       // res = pthread_destroy(&HandleVirtualServerMonitor);

        exit(EXIT_SUCCESS);
}


//------------			semaphore		------------------
/*
 static  int set_semvalue()
{
    //用于初始化信号量，在使用信号量前必须这样做
    union semun sem_union;

    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return 1;
}

 static  void del_semvalue()
{
    //删除信号量
    union semun sem_union;

    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

 static  int semaphore_p()
{
    //对信号量做减1操作，即等待P（sv）
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1;//P()
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_p failed\n");
        return 0;
    }
    return 1;
}

 static int semaphore_v()
{
    //这是一个释放操作，它使信号量变为可用，即发送信号V（sv）
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1;//V()
    sem_b.sem_flg = SEM_UNDO;
    if(semop(sem_id, &sem_b, 1) == -1)
    {
        fprintf(stderr, "semaphore_v failed\n");
        return 0;
    }
    return 1;
}
*/
/******************************************
 * public process: iodevice  read none wait
 *
 * ****************************************/
static	int socket_read_wait_time(int fd, int sec)
{
    fd_set rset;
    struct timeval tv;
		int rc;
    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    rc	= select(fd+1, &rset, NULL, NULL, &tv);
   // printf("->> %s,line:%d,	fd:%d,sec:%d\n",__FILE__,__LINE__,fd,sec );
    if( rc<0 )  //error
         return -1;
    return FD_ISSET(fd, &rset)  ? 1: 0;
}

