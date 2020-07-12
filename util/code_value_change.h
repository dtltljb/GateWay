
/// @cond EXCLUDE
#if defined(__cplusplus)
 extern "C" {
#endif

#if !defined(_CODE_VALUE_CHANGE_H)
#define _CODE_VALUE_CHANGE_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

union u16_2x8{
	
		uint16_t	u16;
		uint8_t		u8[2];
		};

uint8_t char_to_hex(const int8_t *ch);

uint8_t hex_to_char(const uint8_t ch);

void ByteToHexStr(const unsigned char* source, char* dest, int sourceLen);

/*
* input:	char* source,int sourceLen;
* output: char* dest,return destLen;
*******************************************/

uint8_t HexStrToByte(const char* source, unsigned char* dest, int sourceLen);


int Dec_To_Hex(char *source,int ilen);

#endif
#ifdef __cplusplus
     }
#endif
