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
#include 		"socket/VirtualPortMonitor.h"
#include 		"util/code_value_change.h"

/*
*		local_ip_port_init
******************************************************************/
uint8_t	local_ip_port_init(void	)
{
        char		buffer[File_Max_Size];

    ssize_t length_r;
    //FILE * fd;
    int fd;
    char	*fileName	= "conf/ConfigFile.txt";
//    uint32_t	res;
//    uint8_t	i;
    uint32_t 	ret =   sizeof(buffer);
    if( (fd = open(fileName,O_RDWR|O_CREAT,0777))<0 ) {
        printf("open %s failed!\n",fileName);
        return	-1;
    }
    if( (length_r = read(fd,buffer,ret ) ) < 0 ){
        perror("fread error ");
        return	-1;
    }
    close(fd);
    
    char 		*iPtr;

    iPtr	=   buffer;
    iPtr 	= strstr(iPtr,"local_ip");
    if(iPtr  ==  NULL)
		{
      		printf("%s,line=%d,open %s File error\n",__FILE__,__LINE__,fileName);
      		return	-1;
    	}
    close(fd);
		
    struct VirtualSocket	*pVirtualSocket	=	&mVirtualSocket;
    iPtr = strstr(iPtr,"port=");
    if(iPtr == NULL)
    {
     	printf("%s,line=%d,write %s File error,len=%d\n",__FILE__,__LINE__,fileName,ret);
      return	-1;
    }
    char *fPtr	=	strchr(iPtr,'=');
    if ( fPtr	== NULL )
    {
        printf("%s,line=%d,search local ip error\n",__FILE__,__LINE__);
    	return	-1;
    }
    iPtr = strchr(fPtr,'/');
    if ( iPtr != NULL )
    {
        iPtr = memcpy(pVirtualSocket->LocalIp,fPtr+1,iPtr-1-fPtr);
        if ( iPtr == NULL )
    	{
    		printf("%s,line=%d,local_ip copy error\n",__FILE__,__LINE__);
    		return	-1;
    	}	
    }
    fPtr	=	strchr(iPtr,'\n');
    if ( fPtr == NULL )
    {
        printf("%s,line=%d,search local port error\n",__FILE__,__LINE__);
    	return	-1;    	
    }
    
    union u16_2x8		port;
    port.u16	=	pVirtualSocket->serverPort;
    
    HexStrToByte(iPtr+1,(unsigned char*)port.u8,fPtr-iPtr-1);

    return 1;
}

