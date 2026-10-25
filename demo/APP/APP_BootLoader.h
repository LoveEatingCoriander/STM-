#ifndef _APP_BOOTLOADER_H_
#define _APP_BOOTLOADER_H_

#include "BootLoader.h"
#include "Key.h"
#include "w25q128.h"

#define EXTERN_FLASH_STATUS_ADDR 0x00000000U//w25q128此地址的一个字节用来查看程序状态
#define EXTERN_FLASH_START_ADDR 0x00001000U//w25q128从此地址来存储程序

typedef enum
{
    NO_NEW_PROGRAM,
    NEW_PROGRAM,
    START_RECV_PROGRAM,
    END_RECV_PROGRAM,
}Boot_status;

uint8_t APP_Read_Status();
void App_Bootloader_Task();


#endif
