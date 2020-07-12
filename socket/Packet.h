

/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_PACKET_H)
#define _PACKET_H

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

#include 			"socket_protocol_thread.h"
#include 			"pub/epb.h"
#include 			"pub/epb_MmBp.h"



#define ChangeCodeSize      2048   //send data + frame head << ChangeCodeSize
//===================


uint32_t socket_data_pack_func(void *args, uint8_t *r_data, uint16_t *r_len);

#endif
#ifdef __cplusplus
     }
#endif
