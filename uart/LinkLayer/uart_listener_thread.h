
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(UART_LISTENER_THREAD_H)
#define UART_LISTENER_THREAD_H

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

#include      "uart/LinkLayer/lora_parament.h"

void* first_channel_Listener(void	*pvoid);
void* second_channel_Listener(void	*pvoid);
void* third_channel_Listener(void	*pvoid);
void* fourth_channel_Listener(void	*pvoid);
void* config_channel_Listener(void	*pvoid);



#endif
#ifdef __cplusplus
     }
#endif
