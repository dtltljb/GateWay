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

#include 		"forlder_util.h"

/*
*		Function name:	create_multi_dir
*		brief :create multi dir
*		input param:			char* path
*		return reslut:			0 success, non 0 failure
*		eg:path = "./2017/11/"
******************************************************************/
int create_multi_dir(const char *path)
{
        int i, len,ret;

        len = strlen(path);
        char dir_path[len+1];
        dir_path[len] = '\0';

        strncpy(dir_path, path, len);

        for (i=0; i<len; i++)
        {
                if (dir_path[i] == '/' && i > 0)
                {
                        dir_path[i]='\0';
                        if( (ret=access(dir_path, F_OK) )< 0)
                        {
                                if (mkdir(dir_path, 0755) < 0)
                                {
                                        printf("mkdir=%s:msg=%s\n", dir_path, strerror(errno));
                                        return -1;
                                }
                        }
                        dir_path[i]='/';
                }
        }

        return 0;
}

/*
*		Function name:	GetFileFromDir
*		brief :					Get File From Dir
*		input param:			char* path
*		output param: 		char*	file,note:*file place length >= 128 bytes
*		return reslut:			0 success, non 0 failure
******************************************************************/
int GetFileFromDir(char* path,char* path_file) 
{
	DIR *dir;
	struct dirent *dt;
	char filename1[128];
    //path_file	=	NULL;
	
  memset(filename1,0,sizeof(filename1));
  strcpy(filename1,path);

	if((dir = opendir(path)) == NULL)
	{
		printf("opendir %s error\n",path); 
		return -1; 
	}

	while((dt = readdir(dir)) != NULL){ 
    if(strcmp(dt->d_name,".")==0||strcmp(dt->d_name,"..")==0){ 
				continue; 
		}
		//如果这个目录里 还有目录，可以在这加判断 
		strcat(filename1,dt->d_name);
		strcpy(path_file,filename1);
		return	0;
	}
	return	-2;
}

int forlder(void)
{
//    //
//    struct _finddata_t fileinfo;
//    //
//    long fHandle;
//    //
//    int i = 0;
//    if( (fHandle=_findfirst( "./test/*.txt", &fileinfo )) == -1L )
//    {
//        printf( "txt\n");
//        return 0;
//    }
//    else{
//        do{
//            i ++;
//            printf( ":%s,%d\n", fileinfo.name,fileinfo.size);
//        }while( _findnext(fHandle,&fileinfo)==0);
//    }
//    //
//    _findclose( fHandle );
//    printf("%d\n",i);
    return 0;

}

