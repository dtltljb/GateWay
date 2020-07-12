
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_LINK_TABLE_H)
#define _LINK_TABLE_H

#if defined(WIN32) || defined(WIN64)
  #define DLLImport __declspec(dllimport)
  #define DLLExport __declspec(dllexport)
#else
  #define DLLImport extern
  #define DLLExport __attribute__ ((visibility ("default")))
#endif


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include 		"uart/Cmd/report_log_pack.h"

#define			HistoryDataSize		sizeof(UReportLogReq)

typedef	struct	Context_s{
    uint8_t			channel;
    uint8_t			historyData[HistoryDataSize];
    uint8_t     date[8];		//yyyy mm dd hh:mm:ss per/10ms
}Context_t;

typedef struct node{  
    struct node * next;						//数据入口
    struct node * head;						//数据出口  
    Context_t	mContext;
} node_t;

struct node * CreateLinkList(void);

struct node * getHead(void);

struct node *insertNodeToTail(struct node *pTail,void *Context);

struct node *deleteHeadNode(struct node *Node);

void  * getHeadContext(struct node *pHead,void *Context);

#endif
#ifdef __cplusplus
     }
#endif
