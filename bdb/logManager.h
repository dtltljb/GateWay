


/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_LOG_MANAGER_H)
#define _LOG_MANAGER_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

#include 				<stdint.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <fcntl.h>

void	app_run_log_evnt_context(void	*args,void	*argu);


#endif
#ifdef __cplusplus
     }
#endif
