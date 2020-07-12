	
#ifndef __Unpacket_H
#define __Unpacket_H

#include <stdbool.h>
#include "pub/epb.h"
#include "pub/epb_MmBp.h"
#include "pub/util.h"
#include  "crc/crc32.h"
#include "VirtualPortMonitor.h"
#include "socket_protocol_thread.h"

#define  DeviceMaxTotalCnt       (40204-40180)

#define		UnpackBufSize				1050	*	Socket_MAX_Frame


//void		socket_data_unpack_create(void);

uint32_t socket_data_unpack_func(void *args, uint8_t *data, uint16_t *len);
//---------			public		---------------
//extern		Unpack_info			mUnpack_info;

#endif
