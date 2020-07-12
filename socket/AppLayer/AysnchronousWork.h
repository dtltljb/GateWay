
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(AYSNCHRONOUS_WORK_H)
#define AYSNCHRONOUS_WORK_H

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

void	AysnchronousWorkProcessFuction(void	*args);

#endif
#ifdef __cplusplus
     }
#endif
