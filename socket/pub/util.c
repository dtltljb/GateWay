
#include "util.h"

#define  BigLittleMode			1
#define  LittleBigMode			0

uint32_t checkCPUendian()
{
       union{
              uint32_t i;
              uint8_t s[4];
       }c;

       c.i = 0x12345678;
//			 if(0x12 == c.s[0])
//				 return BigLittleMode;
//			 else
//				 return LittleBigMode;
       return (0x12 == c.s[0]);
}

unsigned long t_htonl(unsigned long h)
{
       return checkCPUendian() ? h : BigLittleSwap32(h);
}
 
unsigned long t_ntohl(unsigned long n)
{

       return checkCPUendian() ? n : BigLittleSwap32(n);
}

unsigned short htons(unsigned short h)
{
       return checkCPUendian() ? h : BigLittleSwap16(h);
}
 
unsigned short ntohs(unsigned short n)
{
//	uint16_t Var = checkCPUendian();
//	if (Var)
//		return n;
//	else
//		return BigLittleSwap16(n);
       return checkCPUendian() ? n : BigLittleSwap16(n);
}

/*turn an unsigned short value to big-endian value					*/
/*for example 0x1234 in the memory of X86 is 0x34 and 0x12	*/
/*then turn it to Network Byte Order is 0x12 and 0x34				*/

