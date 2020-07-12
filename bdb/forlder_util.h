
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_FILE_UTIL_H)
#define _FILE_UTIL_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif
#include 				<stdint.h>

/*
*		Function name:	create_multi_dir
*		brief :create multi dir
*		input param:			char* path
*		return reslut:			0 success, non 0 failure
*		eg:path = "./2017/11/"
******************************************************************/
int create_multi_dir(const char *path);


/*
*		Function name:	GetFileFromDir
*		brief :					Get File From Dir
*		input param:			char* path
*		output param: 		char*	file,note:*file place length >= 128 bytes
*		return reslut:			0 success, non 0 failure
******************************************************************/
int GetFileFromDir(char* path,char* path_file);



#endif
#ifdef __cplusplus
     }
#endif
