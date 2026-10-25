#ifndef _APP_BOOTLOADER_H_
#define _APP_BOOTLOADER_H_

#include "BootLoader.h"
#include "Key.h"
#include "w25q128.h"
#include <stdlib.h>

#define EXTERN_FLASH_STATUS_ADDR 0x00000000U//w25q128此地址的一个字节用来查是否有新程序
#define EXTERN_FLASH_CURRENT_GROPRAM 0x00000001U//当前的程序
#define EXTERN_FLASH_LENGTH_ADDR 0x00000003U//程序长度
#define EXTERN_FLASH_START_ADDR_A 0x00001000U//w25q128从此地址来存储程序
#define EXTERN_FLASH_START_ADDR_B 0x06001000U

#define APP_START_ADDR 0x08004000

typedef enum
{
    NO_NEW_PROGRAM,
    NEW_PROGRAM,
    NEW_PROGRAM_A,
    NEW_PROGRAM_B,
    ERROR
}Boot_status;

uint8_t APP_Read_Status(void);
void App_Bootloader_Task(void);


#endif
