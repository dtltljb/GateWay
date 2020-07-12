
#ifndef _UTIL_H
#define _UTIL_H

#include <stdint.h>
#include <string.h>


#define PRODUCT_TYPE_UNKNOWN 					0
#define PRODUCT_TYPE_MPBLEDEMO2 			1

#define REGISTER(NAME)	\
		do {								\
				data_handler *tmp = &NAME##_data_handler;	\
				tmp->next = first_handler.next; \
				first_handler.next = tmp;	\
		} while(0)

#define ARGS_ITEM_SET(ARGS_TYPE, ARGS_POINTER, ITEM_NAME, ITEM_VALUE)	\
		do {	\
				ARGS_TYPE *tmp = (ARGS_TYPE *)ARGS_POINTER;	\
				tmp->ITEM_NAME = ITEM_VALUE;	\
		} while(0)

#define BigLittleSwap16(A)  ((((uint16_t)(A) & 0xff00) >> 8) | \
                            (((uint16_t)(A) & 0x00ff) << 8))

 
#define BigLittleSwap32(A)  ((((uint32_t)(A) & 0xff000000) >> 24) | \
                            (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                            (((uint32_t)(A) & 0x0000ff00) << 8) | \
                            (((uint32_t)(A) & 0x000000ff) << 24))

unsigned short htons(unsigned short val);
unsigned short ntohs(unsigned short val);
unsigned long t_htonl(unsigned long h);
unsigned long t_ntohl(unsigned long n);

#endif
