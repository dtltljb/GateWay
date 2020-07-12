
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(config_command_analysis_h)
#define config_command_analysis_h

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

uint16_t config_command_analysis(void *argu,void	*args);

#endif
#ifdef __cplusplus
     }
#endif
