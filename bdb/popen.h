#ifndef _POPEN_H
#define _POPEN_H

#define		uart_2_gpio_to_1		"echo 1 > /sys/class/gpio/gpio_1"

#include <stdint.h>

int user_popen(void);

int EIA_DA_Contral(char	*io_port,uint8_t flag );

#endif // POPEN_H
