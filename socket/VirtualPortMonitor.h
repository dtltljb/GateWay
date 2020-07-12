
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(virtual_port_monitor_h)
#define virtual_port_monitor_h

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>

#include <semaphore.h>

#include "socket_protocol_thread.h"


//********* server to GateWay********
/*
#define	Login										10000
#define	LoginResp								10001
#define	ReprotLog								10002
#define	ReprotLogResp						10003
#define	EmergencyOpenDoor				65536		
#define	RemoteOpenDoor					10004
#define	RemoteOpenDoorResp			10005
#define	RemoteDeleteInfo				10006
#define	RemoteDeleteInfoResp		10007
#define	RemoteDownLoadInfor			10008
#define	RemoteDownLoadInforResp	10009
#define	KeepActiveTime					10010
#define	KeepActiveTimeResp			10011
*/

/****	DEBUG LEVEL CONTROL ******/
#define			DISABLE								0X0
#define			ENABLE								0X01
#define			Debug_Level						ENABLE
#define 		Debug_ubuntu_Level		1					//ubuntu  debug  enable

#define		sockEncryptSize		1025

/**
 * Data for a connack packet.
 */
typedef struct
{

	union
	{
		unsigned char all;	/**< all connack flags */
		struct
		{
			unsigned int reserved : 7;	/**< message type nibble */
			bool sessionPresent : 1;    /**< was a session found on the server? */
		} bits;

	} flags;	 /**< connack flags byte */
	
	char rc; /**< connack return code */
} Connack;


typedef struct {
	uint16_t	year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t   minute;
	uint8_t		second;
}ProtocolTimeFormat;

/*
typedef struct
{
	char 			local_ip[4];
	uint16_t 	local_port;
	char			server_ip[4];
	uint16_t	server_port;
	uint16_t	keep_alive;
	
}commu_para;
*/

//********* locker to GateWay********

enum	{
		semaphore_socket_receive_id		=	0x12345,
		semaphore_socket_unpack_id						,		
}semaphore_id_pack;



typedef enum{
  SOCKET_SEND_ERR	=	1,
	SOCKET_CONNECTED,
	SOCKET_DISCONNECT,
	SOCKET_RECE_ERR,
}socket_state_t;


/*
	server socket information
*/
#define		Socket_MAX_Frame			5
#define 	ReceiveBufferSize 		1050
#define 	SendBufferSize	  		1050

/*
	virtual socket struct 
*/

enum	socketStates{
	socket_send_data_err		=	0xc0,
	socket_link_fd_err						,
	
};

typedef struct
{
    void 	*p;
    int 	len;
} pVoid;

struct VirtualSocket
{
  int32_t			 			clt_sock;
	char 							*LocalIp;
	uint16_t 					localPort;
	char							*ServerIp;
	uint16_t					serverPort;
	uint32_t 					socketStates;
	
	uint32_t					Local_CSN;
	uint8_t 					*pkey,key[17];
	
	uint16_t					base_time_count;				//base	timer	count
	uint16_t					keep_alive;
	uint8_t						keep_alive_response;		//response status
	uint16_t					keep_alive_times;				//keep alive startup time
	uint8_t						history_log_response;		//response	status
	uint16_t					history_log_times;			//time 		
	uint8_t						EmergencyStateLock;			//
//- - - - - - - - - - - - - - - - - 
	uint8_t 					SingleUnpackBuf[ReceiveBufferSize];
	uint16_t 					SingleUnpackLength;
  uint8_t						MultUnpackBuf[ReceiveBufferSize*Socket_MAX_Frame];				
	uint16_t					MultUnpackLength;
	uint8_t 					SendBuffer[SendBufferSize];
	uint16_t 					SendLength;

	NSFPFixHead				last_fix_header;

//------	channel		workType	Context	--------

	uint8_t						channel;
	uint8_t						sourceChannel;
	uint8_t						workType;
  pVoid							pContext;
  void*           	protocol_para;
};

/*
struct SocketTransBuf{
	
	uint8_t 					SingleUnpackBuf[ReceiveBufferSize];
	uint16_t 					SingleUnpackLength;
	uint8_t						MultUnpackBuf[ReceiveBufferSize*Socket_MAX_Frame];				
	uint16_t					MultUnpackLength;
	uint8_t 					SendBuffer[SendBufferSize];
	uint16_t 					SendLength;
};
*/
/*****public parament ****/

pthread_mutex_t socket_receive_mutex_lock,public_data_seg_mutex_lock;


extern 	sem_t 									sem_socket_receiver_id;
extern	struct 	VirtualSocket   mVirtualSocket;
extern	struct VirtualSocket	VirtualSocketCopyer[5];

//extern	int32_t									mkeep_alive_time;
extern	  struct itimerval tv, otv;		//timer val select after tv


#endif
#ifdef __cplusplus
     }
#endif
