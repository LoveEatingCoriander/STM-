#include "APP_BootLoader.h"

Boot_status boot_status=NO_NEW_PROGRAM;

/*
    读取状态
    returu ：0表示没有新程序
            1表示有新程序
*/
Boot_status APP_Read_Status()
{
    uint8_t status;
    W25Q128_Read(&status,EXTERN_FLASH_STATUS_ADDR,1);
    if(status==1)
    {
        return NEW_PROGRAM;
    }
    return NO_NEW_PROGRAM;
}

void App_Bootloader_Task()
{
    boot_status=APP_Read_Status();
    switch (boot_status)
    {
    case NO_NEW_PROGRAM:
        /* code */
        break;
    
    default:
        break;
    }
}

