
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(XXXX_CHANNEL_SEND_THREAD_H)
#define XXXX_CHANNEL_SEND_THREAD_H

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

#include		"LinkLayer/UartOpration.h"

extern	UartPort		mUartPort[5];


#define				FirstUartNum					0
#define				SecondUartNum					1
#define				ThirdUartNum					2
#define				FourthUartNum					3
#define				ConfigUartNum					4

#define				FirstLoraChn					(FirstUartNum+1)
#define				SecondLoraChn					(SecondUartNum+1)
#define				ThirdLoraChn					(ThirdUartNum+1)
#define				FourthLoraChn					(FourthUartNum+1)
#define				ConfigLoraChn					(ConfigUartNum+1)

void* first_channel_WaitSend(void		*pvoid);
void* second_channel_WaitSend(void	*pvoid);
void* third_channel_WaitSend(void		*pvoid);
void* fourth_channel_WaitSend(void	*pvoid);
void* config_channel_WaitSend(void	*pvoid);

#endif
#ifdef __cplusplus
     }
#endif
