#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<errno.h>

void *thread_function(void *arg);

pthread_mutex_t lora_prm_mutex_lock;
int globaltmp=0;
int main(){
    int res;
    pthread_t pthread_tmp;
    
    pthread_mutex_init(&lora_prm_mutex_lock,NULL);
    
    pthread_mutex_lock(&lora_prm_mutex_lock);
    res = pthread_create(&pthread_tmp,NULL,thread_function,NULL);
    if(res != 0){
        perror("thread creation failed!");
        exit(EXIT_FAILURE);
    }
    while(1){
        pthread_mutex_unlock(&lora_prm_mutex_lock);
        sleep(2);
        globaltmp++;
        printf("in the main func,globaltmp=%d\n",globaltmp);
         res = pthread_mutex_lock(&lora_prm_mutex_lock);        
    //    if(res == EDEADLK)
        {
        //    printf("it has been locked in the thread!\n");
        }
    //    printf(" main func res =%d\n",res);
        sleep(2);
    }
    res = pthread_join(pthread_tmp,NULL);
    if(res != 0 ){
        perror("thread join is failure");
        exit(EXIT_FAILURE);
    }
    printf("thread joined!\n");
    pthread_mutex_destroy(&lora_prm_mutex_lock);
    return 0;
}

void *thread_function(void *arg){
    int res ;
    res = pthread_mutex_lock(&mutex_lock);
    if(res == EDEADLK)
        printf("it has been locked in the main!\n");
    while(1)
    {
        pthread_mutex_unlock(&mutex_lock);
        sleep(1);
        globaltmp--;
        printf("in the pthread func,globaltmp=%d\n",globaltmp);
    //    printf("I am in the pthread func\n");
        res = pthread_mutex_lock(&mutex_lock);
//        printf("thread func res=%d\n",res);
    //    if(res == EDEADLK)
        //    printf("it has been locked in the main!\n");
        sleep(1);
    }
    pthread_exit(NULL);
}