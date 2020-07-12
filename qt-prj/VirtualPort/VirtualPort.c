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
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include	<signal.h>
#include <sys/sem.h>
#include <pthread.h>



#include "socket/VirtualPortMonitor.h"
#include "socket/SocketProtocol.h"
#include "socket/AppLayer/keep_alive_handle.h"
#include "uart/LinkLayer/UartOpration.h"
#include "uart/SendTaskEntity.h"

#include    "bdb/FileManager.h"
#include    "zlog-master/src/zlog.h"
//	------------- public ----------------------
sem_t 									sem_socket_receiver_id;
VirtualSocket           mVirtualSocket;
int32_t									mkeep_alive_time = 1;


//	-------------- private ---------------------
const int ServerPort = 9999;
const char* ServerIp = "39.105.206.239";
//const int ServerPort = 61680;
//const char* ServerIp = "127.0.0.1";

static uint8_t recvBuf[1050];
static uint8_t sendBuf[1050];



union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

static  int sem_id = 0;
static  int semaphore_p();
static  int semaphore_v();
static  int set_semvalue();
static  void del_semvalue();



int main()
{
    int32_t res = 0;
  char  IP[16];
    timer_t ms;
  time_t  timep;

  memset(&mVirtualSocket,0,sizeof(VirtualSocket));
  memset(&mUartPort[0],0,sizeof(UartPort));
  memset(&mUartPort[1],0,sizeof(UartPort));
  memset(&mUartPort[2],0,sizeof(UartPort));
  memset(&mUartPort[3],0,sizeof(UartPort));
  memset(&mUartPort[4],0,sizeof(UartPort));
       
/*
  int rc;
      zlog_category_t *c;

      rc = zlog_init("bdb/zlog.conf");
      if (rc) {
          printf("init failed\n");
          return -1;
      }

      c = zlog_get_category("my_cat");
      if (!c) {
          printf("get cat fail\n");
          zlog_fini();
          return -2;
      }

      zlog_info(c, "hello, zlog");

      zlog_fini();
    while(1);
    {
        sleep(2);
         printf("%s:%d,zlog:%s !! \n",__FILE__,__LINE__,c);
    }

    res = sem_init(&sem_socket_receiver_id,0,0);
    if (res ){
        printf("--:%s:%d\n failure!! ",__FILE__,__LINE__);
        return	(0);
    }

*/

/*debug test
  mkeep_alive_time = 2;
  alarm(mkeep_alive_time);
*/
    pthread_t HandleVirtualServerMonitor;
     res = pthread_create(&HandleVirtualServerMonitor,NULL,SocketProtocol,NULL);
     if (res )
     {
         printf("name:%s,line:%d\n SocketProtocol pthread_create failure!!\n ",__FILE__,__LINE__);
         exit(EXIT_FAILURE);
     }

    //create	semaphore_socket_receive_id
    sem_id = semget((key_t)semaphore_socket_receive_id, 1, 0666 | IPC_CREAT);
    //   set semaphore
    if(!set_semvalue())
    {
                 fprintf(stderr, "Failed to initialize semaphore\n");
                 exit(EXIT_FAILURE);
             }
             
      signal(SIGALRM,keep_alive_handle);
    //signal(SIGALRM,report_log_request_handle);
    //signal(SIGALRM,report_log_response_handle);
    //signal(SIGALRM,repmote_open_result_request_handle);
    //signal(SIGALRM,repmote_load_result_request_handle);


    struct sockaddr_in addr;
  int connect_fd;
    socklen_t addr_len;
    ssize_t size,Sendsize;
   uint16_t Seq = 1;

   while(1)
     {

        uint8_t cnt=0;
    //创建套接字,即创建socket
    do{
           mVirtualSocket.clt_sock = socket(AF_INET, SOCK_STREAM, 0);
           sleep(1);
           printf("clt_sock=%d\r\n",mVirtualSocket.clt_sock);
       }while(mVirtualSocket.clt_sock < 0 );
      //绑定信息，即命名socket
      addr.sin_family = AF_INET;
      addr.sin_port = htons(ServerPort);
      //inet_addr函数将用点分十进制字符串表示的IPv4地址转化为用网络
      //字节序整数表示的IPv4地址
      addr.sin_addr.s_addr = inet_addr(ServerIp);
      //发起连接
      addr_len = sizeof(addr);
      memcpy(IP,ServerIp,strlen(ServerIp));
      IP[strlen(ServerIp)] = '\0';
      while((connect_fd = connect(mVirtualSocket.clt_sock, (struct sockaddr*)&addr, addr_len)) < 0) {
          sleep(6);
          perror("connect error:-->");
          printf("connect server..... seq = %d,IP = %s,Port = %d \n",Seq++,IP,ServerPort);

      }

            #ifdef		Debug_Level
      printf("connected server seq = %d,IP = %s,Port = %d \n",Seq++,IP,ServerPort);
            #endif


   // 	SET	ALARM TIME
            mkeep_alive_time = 3;
            if(alarm(mkeep_alive_time))		//send keep-alive-frame
                fprintf(stderr, "signal alarm initialize faile\n");

      while(1)
      {
             memset(recvBuf, '\0', sizeof(recvBuf));
             size=recv(mVirtualSocket.clt_sock,(char*)recvBuf ,sizeof(recvBuf), 0 );
             if(size <= 0)
                 {
                        fprintf(stderr, "%s,line:%d,socket read error!!\n",__FILE__,__LINE__);
                        mVirtualSocket.socketStates	=	0;
                        break;
                 }
             else
                 {
                 		 res    =   size;
                     if (!semaphore_p() )
                     {
                         printf("%s,line:%d\n semaphore_p failure!!\n ",__FILE__,__LINE__);
                     }
                        memcpy(socketReceiveBuf,recvBuf,res);
                        socketReceiveLength = res;


                        #ifdef Debug_Level
                        printf("%s,line:%d\n ",__FILE__,__LINE__);
                        for(int16_t i=0;i<res;i++)
                          printf("%#2x ",recvBuf[i]);
                        printf("\r\n%s,line:%d, receive length=%d\n ",__FILE__,__LINE__,res);
                        #endif
                        if(!semaphore_v())
                         {
                             printf("name:%s,line:%d\n semaphore_v failure!!\n ",__FILE__,__LINE__);
                         }
                        sem_post(&sem_socket_receiver_id);
               }
               
/*
                    #ifdef		Debug_Level
              time(&timep);
              printf("name:%s,line:%d\n,receive length:%d ,UTC Time= %s \n",__FILE__,__LINE__, size,ctime(&timep));
              #endif
              */
              
       }
       close(mVirtualSocket.clt_sock);

     }

        void* pthreadResult;
        res  = pthread_join(HandleVirtualServerMonitor,&pthreadResult);
        if(res)
        {
            printf("name:%s\n,line:%d\n pthread_join!! ",__FILE__,__LINE__);
            exit(EXIT_FAILURE);
        }
        #ifdef		Debug_Level
        printf("name:%s\n,line:%d\n finished!! ",__FILE__,__LINE__);
        #endif

        sem_destroy(&sem_socket_receiver_id);

        res = pthread_destroy(&HandleVirtualServerMonitor);
        exit(EXIT_SUCCESS);
}


//------------			semaphore		------------------
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



/*
 *FilePath = "../SampleRecord.dat";
    if(offset > SampleRecordMaxLine)
        return false;
    memcpy(PSampleRecord,(const void*)pInfor,sizeof(mStr));
    if((fd = open(FilePath,O_RDWR|O_CREAT|O_APPEND,0777))<0){
       fprintf(stderr, "open %s failed,%s%d \n",FilePath,__FILE__,__LINE__);
       return false;
    }

    length_w = write(fd,mStr,sizeof(struct DevRunInfor));
    if((length_w <= 0)||(length_w != sizeof(mStr))){
        fprintf(stderr, "write none length,%s%d \n",__FILE__,__LINE__);
        return false;
    }
    else{
        printf("mStr Write OK!\n");
    }
    close(fd);
*/
