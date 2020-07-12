/***********************************************Copyright (c)*********************************************
** Author	Date					Version		Comment
** L J B	2017-04-23		1.0				Original
**
**------------------------------------------------File Info-----------------------------------------------

*********************************************************************************************************/
#ifndef __UART_OPRATION_C
#define __UART_OPRATION_C

#include 		<string.h>
#include 		<strings.h>
#include 		<stdio.h>
#include 		<stdlib.h>
#include 		<stdint.h>
#include    <time.h>
#include 		<unistd.h>
#include 		<termios.h>
#include 		<errno.h>
#include 		<fcntl.h>

#include		<signal.h>
#include 		<sys/select.h>
#include 		<sys/times.h>
#include 		<sys/sem.h>
#include 		<sys/ioctl.h>
#include 		<sys/stat.h>
#include 		<sys/types.h>



#include		"UartOpration.h"
#include		"uart/xxx_channel_send_thread.h"
#include        "uart/Cmd/u_set_lora_para_pack.h"
#include		"socket/VirtualPortMonitor.h"


 /******************************************
 * public process: select lora work mode
 *
 * ****************************************/
//static  int sem_id = 0;
//static  int semaphore_p();
//static  int semaphore_v();
//static  int set_semvalue();
//static  void del_semvalue();

int 		set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);


#define	 	CSI_DATA00__GPIO4_IO21		0
#define		CSI_DATA01__GPIO4_IO22		1
#define		CSI_DATA02__GPIO4_IO23		2
#define		CSI_DATA03__GPIO4_IO24		3
#define		CSI_DATA04__GPIO4_IO25		4
#define		CSI_DATA05__GPIO4_IO26		5
#define		CSI_DATA06__GPIO4_IO27		6
#define		CSI_DATA07__GPIO4_IO28		7
#define		CSI_VSYNC__GPIO4_IO19     8
#define		CSI_HSYNC__GPIO4_IO20			9

#define		lora_1_SETA			CSI_HSYNC__GPIO4_IO20		 
#define		lora_1_SETB			CSI_VSYNC__GPIO4_IO19	 
#define		lora_2_SETA			CSI_DATA00__GPIO4_IO21		
#define		lora_2_SETB			CSI_DATA01__GPIO4_IO22		

#define		lora_3_SETA			CSI_DATA02__GPIO4_IO23 
#define		lora_3_SETB			CSI_DATA03__GPIO4_IO24 
#define		lora_4_SETA			CSI_DATA04__GPIO4_IO25
#define		lora_4_SETB			CSI_DATA05__GPIO4_IO26

#define		lora_5_SETA			CSI_DATA06__GPIO4_IO27
#define		lora_5_SETB			CSI_DATA07__GPIO4_IO28


#define GPIO_U_IOCTL_BASE 	'x'
#define GPIOC_OPS   				_IOWR(GPIO_U_IOCTL_BASE,0,int)

#define GPIO_SET(no,state) 	( no | (state << 31))
#define GPIO_GET(val)				(val >> 31)

void gpio_set_value(int fd,int gpio_no,int state)
{
	unsigned long val;
	val = (!!state << 31) | gpio_no;

	if(ioctl(fd,GPIOC_OPS,&val) < 0){
		perror("ioctl");
	}
}

int  gpio_get_value(int fd,int gpio_no)
{
	unsigned long val = gpio_no;
	if(ioctl(fd,GPIOC_OPS,&val) < 0){
		perror("ioctl");
	}
	return val;
}

void		select_lora_states(uint8_t	*io_channel,bool seta,bool setb)
{
    //uint8_t *lora_set = "/dev/gpio";
	uint8_t	loraChannel = *io_channel;
	int gpio;
	
#if	Debug_ubuntu_Level == 0
	gpio = open("/dev/gpio",O_RDWR);
	if(gpio < 0){
		perror(" >select_lora_states /dev/gpio");
		return;
	}
#if		Debug_Level  == 0
printf("%s,line:%d,select lora channel=%d !!\n",__FILE__,__LINE__,loraChannel);
#endif
	switch(loraChannel)
	{
		case	FirstLoraChn:
			gpio_set_value(gpio,lora_1_SETA,seta);
			gpio_set_value(gpio,lora_1_SETB,setb);
			break;
		case	SecondLoraChn:
			gpio_set_value(gpio,lora_2_SETA,seta);
			gpio_set_value(gpio,lora_2_SETB,setb);
			break;
		case	ThirdLoraChn:
			gpio_set_value(gpio,lora_3_SETA,seta);
			gpio_set_value(gpio,lora_3_SETB,setb);			
			break;
		case	FourthLoraChn:
			gpio_set_value(gpio,lora_4_SETA,seta);
			gpio_set_value(gpio,lora_4_SETB,setb);			
			break;
		case	ConfigLoraChn:
			gpio_set_value(gpio,lora_5_SETA,seta);
      gpio_set_value(gpio,lora_5_SETB,setb);
			break;
		default:
			printf("%s,line:%d,select lora channel rang:1~5,over=%d !!\n",__FILE__,__LINE__,loraChannel);
			break;
	}
	close(gpio);
#endif	
    usleep(100000);
}
 /******************************************
 * public process: iodevice  read none wait
 *
 * ****************************************/
uint8_t initialize_uart(UartPort	*argu)
{
	uint32_t res;
	int32_t		fd;

  UartPort*		pUartPort		=	argu;

	if(pUartPort->iodevice	==	NULL){
		fprintf(stderr,"%s,line:%d,open %s is failed!!\n",__FILE__,__LINE__,pUartPort->iodevice);
		return	-1;
	}
	
  if((fd = open((char*)pUartPort->iodevice, O_RDWR|O_NOCTTY))<0){
      fprintf(stderr,"%s,line:%d,open %s is failed!!\n",__FILE__,__LINE__,pUartPort->iodevice);
      return	-1;
		}
		
  res	=	set_opt(fd, 9600, 8, 'n', 1);
	if(	res	!=	0)
	{
    fprintf(stderr,"%s,line:%d,open %s is failed!!\n",__FILE__,__LINE__,pUartPort->iodevice);
		return	-1;
	}
	else
	{
			pUartPort->clt_uart		=	fd;
			pUartPort->deviceStates		=	UartIDLE;
      printf("%s,line:%d,	open %s success,bps:%d,bit:%d,check:%d\n",__FILE__,__LINE__,pUartPort->iodevice,pUartPort->bps, pUartPort->bit, pUartPort->check);
  #if (Debug_ubuntu_Level	== 0)						//debug  disable
			uart_get_lora_prm(pUartPort);  
	#endif
			return	0;
		}
}


/*************************************************
** @brief	uart init 
**
**************************************************/
int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio,oldtio;
	if  ( tcgetattr( fd,&oldtio)  !=  0) { 
		perror("SetupSerial 1");
		return -1;
	}
	bzero( &newtio, sizeof( newtio ) );
	newtio.c_cflag  |=  CLOCAL | CREAD;
	newtio.c_cflag &= ~CSIZE;

	switch( nBits )
	{
	case 7:
		newtio.c_cflag |= CS7;
		break;
	case 8:
		newtio.c_cflag |= CS8;
		break;
	}

	switch( nEvent )
	{
	case 'O':
	case 'o':
		newtio.c_cflag |= PARENB;
		newtio.c_cflag |= PARODD;
		newtio.c_iflag |= INPCK ;
		//newtio.c_iflag |= (INPCK | ISTRIP);
		break;
	case 'E':
	case 'e':	 
		//newtio.c_iflag |= (INPCK | ISTRIP);
		newtio.c_iflag |= INPCK  ;
		newtio.c_cflag |= PARENB;
		newtio.c_cflag &= ~PARODD;
		break;
	case 'N':
	case 'n':	
		newtio.c_cflag &= ~PARENB;
		newtio.c_iflag &= ~INPCK;
		
//    newtio.c_cflag &= ~CRTSCTS;			
//		newtio.c_iflag |= IGNBRK;
		break;
	}

	switch( nSpeed )
	{
	case 2400:
		cfsetispeed(&newtio, B2400);
		cfsetospeed(&newtio, B2400);
		break;
	case 4800:
		cfsetispeed(&newtio, B4800);
		cfsetospeed(&newtio, B4800);
		break;
	case 9600:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	case 115200:
		cfsetispeed(&newtio, B115200);
		cfsetospeed(&newtio, B115200);
		break;
	case 460800:
		cfsetispeed(&newtio, B460800);
		cfsetospeed(&newtio, B460800);
		break;
	default:
		cfsetispeed(&newtio, B9600);
		cfsetospeed(&newtio, B9600);
		break;
	}
	if( nStop == 1 )
		newtio.c_cflag &=  ~CSTOPB;
	else if ( nStop == 2 )
		newtio.c_cflag |=  CSTOPB;

	newtio.c_cc[VTIME]  = 1;
	newtio.c_cc[VMIN] = 255;
	tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0)
	{
		perror("com set error");
		return -1;
	}
//	printf("set done!\n\r");
	return 0;
}


    
    
#endif
/*********************************************************************************************************
**  End
**********************************************************************************************************/
