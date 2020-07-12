

/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_KEEP_ALIVE_HANDLE_H)
#define _KEEP_ALIVE_HANDLE_H

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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include 			"socket/pub/epb_MmBp.h"


#define	STARTUP				1//'S'
#define	REQUEST				2//'T'
#define	RESPONSE			3//'E'

void	*keep_alive_handle(int );


#endif
#ifdef __cplusplus
     }
#endif
