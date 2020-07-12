//标准输入输出头文件
#include <stdio.h>
#include <stdint.h>
//文件操作函数头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/file.h>

#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include 		"DataFileManager.h"

static int create_multi_dir(const char *path);

/****************************
日期：2017年11月7日   采集数据文件管理办法：
1.数据文件存放路径及文件名规则：年/月/日_SampleRecord.dat
2.数据文件存储时间段:00:00~24:00,
3.数据文件覆盖规则:
		1年覆盖,仅限于存储一年的数据，逐日进行数据文件覆盖;
		10年覆盖，数据存储满10后，逐年逐月逐日覆盖；
4.历史数据补传规则,网关自动补传上一个月至当前月，当前时间节点。
5.数据记录中、增加RecordReportFlah 标志，数据上传后修改此标识标注。
6.服务器查询历史数据，根据查询日期检索时间段的数据内容，不检查数据是否已经上报标识。
****************************/

/*********************************************************************************************************
** 函数名称: GetImageFile
** 函数功能: read file context
** 入口参数: char	*FilePath
**          uint32_t fileline
** 出口参数： uint8_t **r_data, int *r_len
** 返回值  : 成功 true  失败  false
** 说    明:

**********************************************************************************************************/


uint8_t GetImageFile(char	*FilePathName,uint32_t fileline,uint8_t **r_data, int *r_len)
{
    FILE *fd;
	struct stat buf; 
    ssize_t     length_r;
    uint16_t    readLen;
    int32_t     st;
    char    type[80];
    char 	mStr[FileTransmitSize];
    *r_len = 10;  //Error raise
    //check forld,none create forlder
    create_multi_dir(FilePathName);

    //fd = open(FilePathName,O_RDONLY,0777);
    if( (fd = open(FilePathName,O_RDONLY,0777)) <= 0){
       fprintf(stderr, "open %s failed,%s%d \n",FilePathName,__FILE__,__LINE__);
       return false;
    }
    length_r = lseek (fd,fileline*sizeof(mStr),SEEK_CUR);
    readLen = sizeof(mStr);
    readLen = read(fd,&mStr,readLen);
    if(readLen <= 0){
        fprintf(stderr, "read err len:%ld,%s line%d \n",length_r,__FILE__,__LINE__);
        close(fd);
        return false;
    }

    length_r += readLen;
//    st = fstat(fd, &buf);
//    if( st <= 0)
//	{
//        fprintf(stderr, "fstat err%s line%d \n",__FILE__,__LINE__);
//        close(fd);
//        return false;
//	}
//    if(buf.st_size == length_r){
//        *r_len = readLen;
//        memcpy(r_data,mStr,*r_len);
//        close(fd);
//        return 2;
//    }
    *r_len = readLen;
    memcpy(r_data,mStr,*r_len);
    close(fd);
    return true;
}


/*********************************************************************************************************
** 函数名称: GetDeviceSampleInfor
** 函数功能: write sample information to flash
** 入口参数: CommunicationState	*pCommState   - 	CommunicationState
**           DeviceRunInfor 		*pInfor				-   DeviceRunInfor
** 出口参数：*pCommState	 --		指针指向的结构体内容被刷新 ,*pInfor memset(,0,)
** 返回值  : 成功 true  失败  false
** 说明 :
**********************************************************************************************************/


struct DevRunInfor  SampleRecord_Array[SampleRecordMaxLine];

uint8_t GetDeviceSampleInfor(CommunicationState	*pCommState,DeviceRunInfor *pInfor)
{
    FILE * fd;
    ssize_t length_r;
	ssize_t offset = pCommState->pGWIptr->StoreInforRDNumber;
    char *Path ;
    strncpy(Path,pCommState->sys_time,sizeof(pCommState->sys_time));

    char *FilePath = "../SampleRecord.dat";
    char mStr[sizeof(struct DevRunInfor)];
    struct DevRunInfor *PSampleRecord;
    PSampleRecord = (struct DevRunInfor *)mStr;
	if(offset > SampleRecordMaxLine)
		return false;
    if((fd = open(FilePath,O_RDWR|O_CREAT,0777)) < 0){
		fprintf(stderr, "open %s failed,%s%d \n",FilePath,__FILE__,__LINE__);
		return false;
    }
    length_r = lseek (fd,offset*sizeof(struct DevRunInfor),SEEK_CUR);

    if((length_r = read(fd,(void*)mStr,sizeof(struct DevRunInfor))) < 0){
        fprintf(stderr, "read failed,%s%d \n",__FILE__,__LINE__);
		close(fd);
		return false;
    }
    if(length_r != sizeof(struct DevRunInfor)){
		fprintf(stderr, "read length none enough,%s%d \n",__FILE__,__LINE__);
		close(fd);
		return false;
    }
	//printf("ManufacturerID is %d,DeviceId is %d \n",PSampleRecord->ManufacturerID,PSampleRecord->DeviceId );
    close(fd);
    memcpy((void*)pInfor,mStr,sizeof(struct DevRunInfor));
    return true;
}


/*********************************************************************************************************
** 函数名称: SetDeviceSampleInfor
** 函数功能: write sample information to flash
** 入口参数: CommunicationState	*pCommState   - 	CommunicationState
**           DeviceRunInfor 		*pInfor				-   DeviceRunInfor
** 出口参数：*pCommState	 --		指针指向的结构体内容被刷新 ,*pInfor memset(,0,)
** 返回值  : 成功 true  失败  false
** 说明 :
 
**********************************************************************************************************/

uint8_t SetDeviceSampleInfor(CommunicationState	*pCommState,DeviceRunInfor *pInfor)
{
	FILE * fd;
    ssize_t length_w,offset=1;
    char *FilePath ;
	
    int ret;
    DIR *dir;
    struct dirent *catlog;
    FilePath = "./2016/10/";
//check forld,none create forlder
    create_multi_dir(FilePath);

//使用opendir函数打开目录
    dir = opendir(FilePath);
    if(dir==NULL){
        printf("opendir %s failed!\n",FilePath);
    }
    printf("opendir %s suceces！\n",FilePath);
//使用readdir读取目录argv[1]
    catlog = readdir(dir);
    if(catlog == NULL){
        printf("readdir %s failed!\n",FilePath);
    }
    printf("%s d_ino is %ld\n ",FilePath,catlog->d_ino);
//使用closedir函数关闭目录
    closedir(dir);

    char mStr[sizeof(struct DevRunInfor)];
    struct DevRunInfor *PSampleRecord;
    PSampleRecord = (struct DevRunInfor *)mStr;
    *FilePath = "../SampleRecord.dat";
	if(offset > SampleRecordMaxLine)
		return false;
	memcpy(PSampleRecord,(const void*)pInfor,sizeof(mStr));
  if((fd = open(FilePath,O_RDWR|O_CREAT|O_APPEND,0777))<0){
       fprintf(stderr, "open %s failed,%s%d \n",FilePath,__FILE__,__LINE__);
	   return false;
    }

//	length_r = lseek (fd,offset*sizeof(struct SampleRecord),SEEK_CUR);

    length_w = write(fd,mStr,sizeof(struct DevRunInfor));
    if((length_w <= 0)||(length_w != sizeof(mStr))){
        fprintf(stderr, "write none length,%s%d \n",__FILE__,__LINE__);
		return false;
    }
    else{
        printf("mStr Write OK!\n");
    }
    close(fd);
    return true;
}


/*********************************************************************************************************
** 函数名称: GetDeviceConfigInfor
** 函数功能: 获取设备eeprom 信息内容
** 入口参数: void  *parg        - 	DeviceConfigInfor*	
**           uint16 CurNumber - 设备编号 

** 出口参数：parg --获取设备信息内容
** 返回值  : 读取成功 true  失败  false
** 注意    ：
**********************************************************************************************************/

uint8_t  GetDeviceConfigInfor(void *parg,uint16_t CurNumber)
{
		uint8_t	st;
		uint16_t val,NumberLine;
		uint32_t	iEmemAddr;
		DeviceConfigInfor*	pDevInfor;
		pDevInfor 	=	(DeviceConfigInfor *)parg;
		if(CurNumber >0){
			NumberLine	= CurNumber - 1;   //eeprom address from 0 to DeviceMaxTotalCnt-1
		}
		else
			return false;
		iEmemAddr	=	NumberLine*DeviceStructSize+dev1ManufacturerID;

		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;
		pDevInfor->ManufacturerID = val;
		pDevInfor->ManufacturerID <<= 8; 
		st=	GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;
		pDevInfor->ManufacturerID |= val;

		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
				return	false;	
		pDevInfor->GroupId = val;
		
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->DeviceType = (uint8_t)val;
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->DeviceId = (uint8_t)val;	
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->CollectType = val;	
		
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->CollectPeriod = val;	
		
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->InforCode = val;	
		pDevInfor->InforCode <<= 16; 
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->InforCode |= val;	
		
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->ItemId = val;	
		
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->Unit = val;		
		
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->RegisterState = val;
		st=GetRegValue(NULL,&val,iEmemAddr++);
		if(st == false)
			return	false;	
		pDevInfor->SampleState = val;		
		return	true;
}






