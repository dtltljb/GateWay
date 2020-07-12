#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#include "link_table.h"

//private parament
struct node		*currHead;
/*
*		Function name:	Create
*		brief :create link table head
*		output param: *head success, NULL failure
******************************************************************/
struct node * CreateLinkList(void)
{
	struct node * pHead=(struct node *)malloc(sizeof(struct node));
	//struct node * pHead=&nodeHead;
	if(NULL==pHead)
	{
		printf("%s,%d, assocate memry failure \n",__FILE__,__LINE__);
		return NULL;;
	}
  struct node *pTail = pHead;
  pTail->next = NULL;
  memset((char*)&pHead->mContext,0,sizeof(pHead->mContext));
  currHead	=	pHead;
	return pHead;
}

/*
*		Function name:	getHead
*		brief :get link table head
*		output param: *head success
******************************************************************/
struct node * getHead(void)
{
	return currHead;
}

/*
*		Function name:	insertNodeToTail
*		brief :insert node to link table tail
*		output param: *head success,NULL failure
******************************************************************/
struct node *insertNodeToTail(struct node *pHead,void *Context)
{
		char * ptr;
    struct node * pNode=pHead;
		for(;pNode->next	!=	NULL;pNode = pNode->next);
		
 		struct node * pNewTail=(struct node *)malloc(sizeof(struct node));
		if(NULL	==	pNewTail){
			printf("%s,%d, assocate memry failure \n",__FILE__,__LINE__);
			return NULL;
		}
		ptr = memcpy((char*)&pNode->mContext,(char*)Context,sizeof(pNode->mContext));
		if(ptr == NULL){
			printf("%s,%d, memry copy failure \n",__FILE__,__LINE__);
			return NULL;
		}
		pNode->next=pNewTail;
		pNewTail->next=NULL;
    memset((char*)&pNewTail->mContext,0,sizeof(pNewTail->mContext));
		pNode=pNewTail;
		return pNode;
}

/*
*		Function name:	deleteHeadNode
*		brief :delete link table head,shift next node
*		output param: *head success,NULL failure
******************************************************************/
struct node *deleteHeadNode(struct node *Node)
{
        //char * ptr;
		if(Node == NULL)
			return	NULL;
		struct node * pHead	=	Node->next;
		if(pHead	!=	NULL){
			free(Node);
			currHead	=	pHead;
			return pHead;
		}else{
			printf("%s,%d, list empyt delete failure \n",__FILE__,__LINE__);
			return	NULL;
		}
}

/*
*		Function name:	getHeadContext
*		brief :get link table head	context
*		output param: *head success,NULL failure
******************************************************************/
void  * getHeadContext(struct node *pHead,void *Context)
{
		char * ptr;
		if(pHead == NULL)
			return	NULL;

		if(pHead->next	!=	NULL){
			ptr = memcpy((char*)Context,(char*)&pHead->mContext,sizeof(pHead->mContext));
            if(ptr == NULL)
                return  NULL;
            else
                return Context;
		}else{
			#ifdef		Debug_Level
			printf("%s,%d, list empyt get failure \n",__FILE__,__LINE__);
			#endif
			return	NULL;
		}
}
