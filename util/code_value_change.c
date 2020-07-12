
#include <stdio.h>

#include <ctype.h>
#include "code_value_change.h"


uint8_t char_to_hex(const int8_t *ch)
{
 uint8_t value = 0;
 if(*ch >= 0 && *ch <= 9)
 {
 value = *ch + 0x30;
 }
 else if(*ch >=10 && *ch <=15)
 {
 // 大写字母
 value = *ch + 0x37;
 }
 
 //printf("%s hex : 0x%02x\n", __func__, value);
 return value;
}


uint8_t hex_to_char(const uint8_t ch)
{
    uint8_t value = 0;
    if(ch >= 0x30 && ch <=0x39)
    {
        value = ch - 0x30;
    }
    else if(ch >= 0x41 && ch <= 0x46)
    {
        value = ch - 0x37;
    }
    else if(ch >= 0x61 && ch <= 0x66)
    {
        value = ch - 0x57;
    }

    //printf("%s ascii : 0x%02x\n", __func__, value);
    return value;
}

void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;


    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;


        highByte += 0x30;


        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;


        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}

/*
* input:	char* source,int sourceLen;
* output: char* dest,return destLen;
*******************************************/
uint8_t HexStrToByte(const char* source, unsigned char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);


        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;


        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        dest[i / 2] = (highByte << 4) | lowByte;
        
    }
    return i/2 + 1;
}


/*
*	 hexToDec
**********************************/
/* 返回ch字符在sign数组中的序号 */
int getIndexOfSigns(char ch)
{
    if(ch >= '0' && ch <= '9')
    {
        return ch - '0';
    }
    if(ch >= 'A' && ch <='F') 
    {
        return ch - 'A' + 10;
    }
    if(ch >= 'a' && ch <= 'f')
    {
        return ch - 'a' + 10;
    }
    return -1;
}

/* 十进制文本数 转换为十六进制数 */
int Dec_To_Hex(char *source,int ilen)
{
    int sum = 0;
    int t = 1;
    int i, len;
    len =ilen;
    for(i=len-1; i>=0; i--)
    {
        sum += t * getIndexOfSigns(*(source + i));
        t *= 10;
    }  
 
    return sum;
}



