
#ifndef _UTIL_DATE_H
#define _UTIL_DATE_H

#include <stdint.h>
#include <string.h>
#include <time.h>

void Get_BCDTime7(time_t i_tTime,char* o_cpBcdTime);
void Get_BCDTime6(time_t i_tTime,char* o_cpBcdTime);
void Get_CurBCDTime6(char* o_cpBcdTime);
void Get_CurBCDTime7(char* o_cpBcdTime);
void Get_CurBCDTime8(char* o_cpBcdTime);

uint8_t *ProtoTimeFormat(void);

#endif
