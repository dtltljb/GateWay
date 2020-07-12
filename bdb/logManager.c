#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/file.h>

#include <dirent.h>
#include <unistd.h>

#include 		"logManager.h"
#include		"socket/VirtualPortMonitor.h"
#include		"uart/LinkLayer/UartOpration.h"

/*
*			app_run_log_record_context
******************************************************************/

void	app_run_log_evnt_context(void	*args,void	*argu)
{

    UartPort							*pUartPort		= (UartPort*)argu;
    struct VirtualSocket       	*pVirtualSocket			=	(struct VirtualSocket*)args;
/*
    DIR *dir;
    struct dirent *catlog;
    char	*dataFile = "./conf/";
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
*/
        printf("%s,%d,[ %s ] record log information \n",__FILE__,__LINE__,pUartPort->h_io);
      //  printf("%s,%d,[ %s ] record log information \n",__FILE__,__LINE__,pVirtualSocket->LocalIp);


}
