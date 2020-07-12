
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(LORA_PARAMENT_H)
#define LORA_PARAMENT_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif
/*
*			MaxChannelNumber:hal lora channel total,reduce cost maybe replace 2,must Correspond hal lora
*			ChannelMaxDevice:channel load locker total,replace 30~50 lockers
*/
#define         MaxChannelNumber            5
#define         ChannelMaxDevice            25

#define	_FRAME_HEAD					0XFC
#define	_Portocol_VER				0X01
#define	_MAX_Frame					20
#define	_MAX_RecevieSize		0xFF

#include <stdbool.h>
#include <stdint.h>
#include <signal.h>

#include "socket/pub/epb_MmBp.h"

 enum{
 			first_chn_AirRate										=	10,
 			second_chn_AirRate									=	10,
 			third_chn_AirRate										=	10,
 			fourth_chn_AirRate									=	10,
 			config_chn_AirRate									=	10,			 			 			 			
     first_chn_GateWayHalAddress         =       0x0110,
     second_chn_GateWayHalAddress        =       0x0210,
     third_chn_GateWayHalAddress         =       0x0310,
     fourth_chn_GateWayHalAddress        =       0x0410,
     config_chn_GateWayHalAddress        =       0x0010
 };

enum	{
    CHANNEL_ONE_VALUE										=	460000,
    CHANNEL_TWO_VALUE										=	470000,
    CHANNEL_THREE_VALUE                 =	480000,
    CHANNEL_FOUR_VALUE                  =	490000,
    CHANNEL_CONF_VALUE                  =	450000,

}channel_configure_value;

typedef struct {
	uint8_t		chn;
	uint8_t		DeviceTotal;
	uint32_t	FreqValue;
  uint16_t  GateWayHalAddress;
  uint8_t		AirRate;
}ChannelCorrespond;

typedef	struct {
  uint8_t        init_flag;		//init_flag =	1; init okay
  uint8_t        autoConfigNum;
	ChannelCorrespond	chn1;
	ChannelCorrespond	chn2;
	ChannelCorrespond	chn3;
	ChannelCorrespond	chn4;
	ChannelCorrespond	configChn;
}ChannelValueList;

//-------	public -------
extern	ChannelValueList	mChannelValueList;

int8_t	init_lora_channel_config(void);

int8_t	updata_channel_prm_to_file(void);

/***********************************************************************
 * @brief				get_atuo_channel_number
 * @param[in]		void
 * @param[out]
 * @return 			channel number, 0 is faile,0xff is full error
 **********************************************************************/
uint8_t	get_atuo_channel_number(void);

/***********************************************************************
 * @brief				get_lora_channel_Hal_address
 * @param[in]		uint8_t	chn,uint32_t	*addr
 * @return 			1 success
 **********************************************************************/
 uint8_t	get_lora_channel_Hal_address(uint8_t	chn,uint16_t	*addr);
/***********************************************************************
  * @brief				get_lora_channel_Hal_AirRate
  * @param[in]		uint8_t	chn,uint32_t	*addr
  * @return 			1 success
  **********************************************************************/
  uint8_t	get_lora_channel_Hal_AirRate(uint8_t	chn,uint8_t	*AirRate);
  
/***********************************************************************
 * @brief				get_lora_channel_Frequency
 * @param[in]		uint8_t	chn,uint32_t	*FreVal
 * @return 			1 success
 **********************************************************************/
uint8_t	get_lora_channel_Frequency(uint8_t	chn,uint32_t	*FreVal);

uint8_t	get_lora_channel_ByFrequency(uint8_t	*chn,uint32_t	FreVal);

int8_t	delete_locker_update_to_var(uint8_t chn);

#endif
#ifdef __cplusplus
     }
#endif
