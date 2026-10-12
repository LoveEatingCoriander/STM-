#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "usart.h"

#define BOOTLOADER_LEN 512


extern char buffer[BOOTLOADER_LEN];
extern volatile uint16_t actual_length;
extern volatile uint16_t total_length;
extern volatile uint8_t Rx_flag;

void BootLoader_Init(void);

#endif
