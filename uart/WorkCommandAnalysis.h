
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(work_command_analysis_h)
#define work_command_analysis_h

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif

enum WorkCommandType{
	
	Locker_to_server_request				=	0xa0,
	locker_to_gateway_request				,
	Locker_to_server_response				,
	locker_to_gateway_response			,
		
	server_to_locker_request				,
	server_to_locker_response				,
	server_to_gateway_request				,
	server_to_gateway_response			,
	
	gateway_to_server_request				,
	gateway_to_server_response			,
	gateway_to_locker_request				,
	gateway_to_locker_response		
};


uint16_t work_command_analysis(void *argu,void	*args);

#endif
#ifdef __cplusplus
     }
#endif
