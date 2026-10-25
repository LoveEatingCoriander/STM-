#include "APP_BootLoader.h"

Boot_status boot_status=NO_NEW_PROGRAM;

Boot_status APP_Read_Status(void)
{
    uint8_t status;
    W25Q128_Read(&status,EXTERN_FLASH_STATUS_ADDR,1);
    if(status==1)
    {
        uint8_t program;
        W25Q128_Read(&program,EXTERN_FLASH_CURRENT_GROPRAM,1);
        if(program==1)
        {
            return NEW_PROGRAM_B;
        }
        else if(program==2)
        {
            return NEW_PROGRAM_A;
        }
    }
    return NO_NEW_PROGRAM;
}

void APP_Write_Status(Boot_status status)
{   
    uint8_t data;
    if(status==NO_NEW_PROGRAM)
    {
        data=0;
    }
    else if(status==NEW_PROGRAM)
    {
        data=1;
    }
    W25Q128_Write(&data,EXTERN_FLASH_STATUS_ADDR,1);
}

void APP_Set_CurrentProgram(Boot_status status)
{
    uint8_t data;
    if(status==EXTERN_FLASH_START_ADDR_A)
    {
        data=1;
    }
    else if(status==EXTERN_FLASH_START_ADDR_B)
    {
        data=2;
    }
    W25Q128_Write(&data,EXTERN_FLASH_STATUS_ADDR,1);
}


uint32_t APP_Read_Length(void)
{
    uint32_t length;
    uint8_t res[4];
    W25Q128_Read(res,EXTERN_FLASH_LENGTH_ADDR,sizeof(res));
    length=(res[0]<<24)|(res[1]<<16)|(res[2]<<8)|(res[3]<<0);
    return length;
}

void APP_Load_Program(uint32_t original_addr)
{
    uint32_t length=APP_Read_Length();
    uint8_t* program_data=(uint8_t*)malloc(sizeof(uint8_t)*length);
    W25Q128_Read(program_data,original_addr,length);
    BootLoader_Write_Programming(APP_START_ADDR,program_data,length);
}

void App_Bootloader_Task(void)
{
    boot_status=APP_Read_Status();
    switch (boot_status)
    {
    case NO_NEW_PROGRAM:
        BootLoader_Jump_To_App();
        break;
    case NEW_PROGRAM_A:
        APP_Load_Program(EXTERN_FLASH_START_ADDR_A);
        W25Q128_EraseSector(EXTERN_FLASH_STATUS_ADDR);
        APP_Write_Status(NO_NEW_PROGRAM);
        APP_Set_CurrentProgram(NEW_PROGRAM_A);
        BootLoader_Jump_To_App();
    case NEW_PROGRAM_B:
        APP_Load_Program(EXTERN_FLASH_START_ADDR_B);
        W25Q128_EraseSector(EXTERN_FLASH_STATUS_ADDR);
        APP_Write_Status(NO_NEW_PROGRAM);
        APP_Set_CurrentProgram(NEW_PROGRAM_B);
        BootLoader_Jump_To_App();
    default:
        break;
    }
}

