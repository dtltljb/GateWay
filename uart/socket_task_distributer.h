
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(SOCKET_TASK_DISTRIBUTER_H)
#define SOCKET_TASK_DISTRIBUTER_H

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




 /***********************************************************************
 * @brief				send_correspond_semphore_number
 * @param[in]		uint8_t	channel,void *args
 * @return 			=0 ,falure 
 **********************************************************************/
uint8_t	send_correspond_semphore_number(uint8_t	channel,void *args);



#endif
#ifdef __cplusplus
     }
#endif
