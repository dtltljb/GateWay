#include <stdio.h>
//opendir,closedir,readdir函数头文件
#include <dirent.h>
#include <sys/types.h>
#include    <search.h>

#include 		"link_table.h"
#include 		"FileManager.h"

/* 
III.linux c 编程：创建一个线程，监视目录，一旦目录里出现新的文件，就将文件转上传。 
头文件 
*/ 

#define SRCPATH "../image/"
#define DSTPATH "../imageBak/"

int movefile() 
{
	DIR *dir; 
	struct dirent *dt; 
	FILE *fp1,*fp2; 
	char filename1[256],filename2[256]; 
	char buf[1024]; 
	int readsize,writesize; 

    memset(filename1,0,sizeof(filename1));
    strcpy(filename1,SRCPATH);
    memset(filename2,0,sizeof(filename2));
    strcpy(filename2,DSTPATH);

	if((dir = opendir(SRCPATH)) == NULL) 
	{
		printf("opendir %s error\n",SRCPATH); 
		return -1; 
	}

	while(1) 
	{
		while((dt = readdir(dir)) != NULL) 
		{ 
        if(strcmp(dt->d_name,".")==0||strcmp(dt->d_name,"..")==0)
		{ 
		continue; 
		} 
		//如果这个目录里 还有目录，可以在这加判断 
		//这里假设初始为空目录 
		strcat(filename1,dt->d_name); 
		strcat(filename2,dt->d_name); 
		//如果进程资源较少可以直接用linux系统命令 

		fp1 = fopen(filename1,"rb"); 
		if(fp1==NULL) 
		{
			printf("open %s failed /n",filename1); 
			return -1; 
		}

		fp2 = fopen(filename2,"wb"); 
		if(fp2==NULL) 
		{ 
			printf("open %s failed /n",filename2); 
			fclose(fp1); 
			return -1; 
		}

		while((readsize = fread(buf,sizeof(buf),1,fp1))>0) 
		{
			//total += readsize; 
			memset(buf,0,sizeof(buf)); 
			writesize = fwrite(buf,sizeof(buf),1,fp2); 
            if(writesize != readsize)
			{ 
				printf("write error"); 
				return -2; 
				fclose(fp1); 
				fclose(fp2); 
			}
		}
		fclose(fp1); 
		fclose(fp2); 
		rmdir(filename2); 
		}
	} 
}



















/*

int movefile() 
{
	DIR *dir; 
	struct dirent *dt; 
	FILE *fp1,*fp2; 
	char filename1[256],filename2[256]; 
	char buf[1024]; 
	int readsize,writesize; 

    memset(filename1,0,sizeof(filename1));
    strcpy(filename1,SRCPATH);
    memset(filename2,0,sizeof(filename2));
    strcpy(filename2,DSTPATH);

	if((dir = opendir(SRCPATH)) == NULL) 
	{
		printf("opendir %s error\n",SRCPATH); 
		return -1; 
	}

	while(1) 
	{
		while((dt = readdir(dir)) != NULL) 
		{ 
        if(strcmp(dt->d_name,".")==0||strcmp(dt->d_name,"..")==0)
		{ 
		continue; 
		} 
		//如果这个目录里 还有目录，可以在这加判断 
		//这里假设初始为空目录 
		strcat(filename1,dt->d_name); 
		strcat(filename2,dt->d_name); 
		//如果进程资源较少可以直接用linux系统命令 

		fp1 = fopen(filename1,"rb"); 
		if(fp1==NULL) 
		{
			printf("open %s failed /n",filename1); 
			return -1; 
		}

		fp2 = fopen(filename2,"wb"); 
		if(fp2==NULL) 
		{ 
			printf("open %s failed /n",filename2); 
			fclose(fp1); 
			return -1; 
		} 

		while((readsize = fread(buf,sizeof(buf),1,fp1))>0) 
		{ 
			//total += readsize; 
			memset(buf,0,sizeof(buf)); 
			writesize = fwrite(buf,sizeof(buf),1,fp2); 
            if(writesize != readsize)
			{ 
				printf("write error"); 
				return -2; 
				fclose(fp1); 
				fclose(fp2); 
			}
		}
		fclose(fp1); 
		fclose(fp2); 
		rmdir(filename2); 
		}
	} 
}
*/
//int main(int argc,char **argv)
//{
//	pthread_t id1;
//	int ret;
//	ret = pthread_create(&id1, NULL, (void*)movefile, NULL);
//	return ret;
//}
