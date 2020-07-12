
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "util_date.h"
/***************************************************************************
*Char2Bcd
*BCD
*-buf:
*
*  
*
***************************************************************************/
char Char2Bcd(char i_cHexData)
{
   char i;
   i=((i_cHexData%100)/10)*16+(i_cHexData%10);
   return i;
}
 
/***************************************************************************
*Get_BCDTime7
*BCD
*-*time:
*
*  
*
***************************************************************************/
void Get_BCDTime7(time_t i_tTime,char* o_cpBcdTime)
{
    struct tm *time;
    time=localtime(&i_tTime);
    o_cpBcdTime[0]=Char2Bcd((time->tm_year+1900)/100);
    o_cpBcdTime[1]=Char2Bcd((time->tm_year+1900)%100);
    o_cpBcdTime[2]=Char2Bcd(time->tm_mon+1);
    o_cpBcdTime[3]=Char2Bcd(time->tm_mday);
    o_cpBcdTime[4]=Char2Bcd(time->tm_hour);
    o_cpBcdTime[5]=Char2Bcd(time->tm_min);
    o_cpBcdTime[6]=Char2Bcd(time->tm_sec);
}
 
/***************************************************************************
*Get_CurBCDTime6 
*BCD
*-*time:
*
*  
*
***************************************************************************/
void Get_BCDTime6(time_t i_tTime,char* o_cpBcdTime)
{
    struct tm *time;
    time=localtime(&i_tTime);
    o_cpBcdTime[0]=(unsigned char)((((time->tm_year-100)/10)<<4)|((time->tm_year-100)%10));
    o_cpBcdTime[1]=(unsigned char)((((1+time->tm_mon)/10)<<4)|((1+time->tm_mon)%10));
    o_cpBcdTime[2]=(unsigned char)((((time->tm_mday)/10)<<4)|((time->tm_mday)%10));
    o_cpBcdTime[3]=(unsigned char)((((time->tm_hour)/10)<<4)|((time->tm_hour)%10));
    o_cpBcdTime[4]=(unsigned char)((((time->tm_min)/10)<<4)|((time->tm_min)%10));
    o_cpBcdTime[5]=(unsigned char)((((time->tm_sec)/10)<<4)|((time->tm_sec)%10));
}
 
/***************************************************************************
*Get_CurBCDTime6
*BCD
*-*time:
*
*  
*add by lianzihao 2016-09-13
***************************************************************************/
void Get_CurBCDTime6(char* o_cpBcdTime)
{
        time_t lt=0;   
        lt = time(NULL);
        struct tm * pCurTime = localtime(&lt);
        o_cpBcdTime[0]=(unsigned char)((((pCurTime->tm_year-100)/10)<<4)|((pCurTime->tm_year-100)%10));
        o_cpBcdTime[1]=(unsigned char)((((1+pCurTime->tm_mon)/10)<<4)|((1+pCurTime->tm_mon)%10));
        o_cpBcdTime[2]=(unsigned char)((((pCurTime->tm_mday)/10)<<4)|((pCurTime->tm_mday)%10));
        o_cpBcdTime[3]=(unsigned char)((((pCurTime->tm_hour)/10)<<4)|((pCurTime->tm_hour)%10));
        o_cpBcdTime[4]=(unsigned char)((((pCurTime->tm_min)/10)<<4)|((pCurTime->tm_min)%10));
        o_cpBcdTime[5]=(unsigned char)((((pCurTime->tm_sec)/10)<<4)|((pCurTime->tm_sec)%10));
}
/***************************************************************************
*Get_CurBCDTime7
*BCD
*-*time:
*
*  
*
***************************************************************************/
void Get_CurBCDTime7(char* o_cpBcdTime)
{
    time_t tt;
    struct tm *curtime;
    time(&tt);
    curtime=localtime(&tt);
    o_cpBcdTime[0]=Char2Bcd((curtime->tm_year+1900)/100);
    o_cpBcdTime[1]=Char2Bcd((curtime->tm_year+1900)%100);
    o_cpBcdTime[2]=Char2Bcd(curtime->tm_mon+1);
    o_cpBcdTime[3]=Char2Bcd(curtime->tm_mday);
    o_cpBcdTime[4]=Char2Bcd(curtime->tm_hour);
    o_cpBcdTime[5]=Char2Bcd(curtime->tm_min);
    o_cpBcdTime[6]=Char2Bcd(curtime->tm_sec);
}
 

uint8_t*	ProtoTimeFormat(void)
{
    static uint8_t timeString[20];  //process end release memery,must be use static
    struct tm *tblock;
    time_t timep;

    time(&timep);
    tblock = localtime(&timep);
    sprintf((char*)timeString,"%d%d%d%d%d%d",(tblock->tm_year+1900),tblock->tm_mon+1,tblock->tm_mday,\
            tblock->tm_hour,tblock->tm_min,tblock->tm_sec);
    return timeString;
}
 

//***************************************************************************/
void Get_CurBCDTime8(char* o_cpBcdTime)
{
    time_t tt;    uint8_t week;
    struct tm *curtime;
    time(&tt);
    curtime=localtime(&tt);
    o_cpBcdTime[0]=Char2Bcd((curtime->tm_year+1900)/100);
    o_cpBcdTime[1]=Char2Bcd((curtime->tm_year+1900)%100);
    o_cpBcdTime[2]=Char2Bcd(curtime->tm_mon+1);
    o_cpBcdTime[3]=Char2Bcd(curtime->tm_mday);
    if(curtime->tm_wday == 0)
       week    =   7;
    else
       week    =   curtime->tm_wday;

    o_cpBcdTime[4]=Char2Bcd(week);
    o_cpBcdTime[5]=Char2Bcd(curtime->tm_hour);
    o_cpBcdTime[6]=Char2Bcd(curtime->tm_min);
    o_cpBcdTime[7]=Char2Bcd(curtime->tm_sec);
}
 
 
 
 /*
#define LEN        7
int main(void)
{
    int i = 0;
    char l_arrBcdtime[LEN] ={0};
    Get_CurBCDTime7(l_arrBcdtime);
    printf("Get_CurBCDTime7 :");
    for(i=0;i<LEN;i++)
    {
        printf("%02x",l_arrBcdtime[i]);
    }
 
    Get_CurBCDTime6(l_arrBcdtime);
    
    printf("\nGet_CurBCDTime6 :");
    for(i=0;i<LEN-1;i++)
    {
        printf("%02x",l_arrBcdtime[i]);
    }
    printf("\n");
    return 0;
}
*/