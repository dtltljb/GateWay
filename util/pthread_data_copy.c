#include 	<unistd.h>
#include	<signal.h>
#include 	<pthread.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>

#include	"pthread_data_copy.h"
#include "socket/VirtualPortMonitor.h"




int pthread_data_copy( char *argd,char *args,int *Length)
{
						int rc	=1;
						while(rc){
							rc	=	pthread_mutex_trylock(&public_data_seg_mutex_lock);
							usleep(10000);
						}

						memcpy(argd,args,*Length);

		        pthread_mutex_unlock(&public_data_seg_mutex_lock);
		        return 0;
}



/*

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
};

static	int pthread_data_copy_run_flag = 0;
static int sem_id = 0;

static int set_semvalue();
static void del_semvalue();
static int semaphore_p();
static int semaphore_v();

int pthread_data_copy( char *argd,char *args,int *Length)
{

    if(pthread_data_copy_run_flag < 1)
    {
    	 //创建信号量
       sem_id = semget((key_t)public_semaphore_protect_seg, 1, 0666 | IPC_CREAT);
        //程序第一次被调用，初始化信号量
       if(!set_semvalue()){
            fprintf(stderr, "Failed to initialize semaphore\n");
            return	 -1;   
       }
       pthread_data_copy_run_flag	+=	1;
    }
         //进入临界区
  if(!semaphore_p())
            return	 -1;

	memcpy(argd,args,*Length);
	
 	if(!semaphore_v())
            return	 -1;
	pthread_data_copy_run_flag	-=	1;
   if(pthread_data_copy_run_flag < 1){
        //如果程序是第一次被调用，则在退出前删除信号量
      del_semvalue();
    }
			return	 0;
}

static int set_semvalue()
{
    //用于初始化信号量，在使用信号量前必须这样做
    union semun sem_union;

    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1)
        return 0;
    return 1;
}

static void del_semvalue()
{
    //删除信号量
    union semun sem_union;

    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p()
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