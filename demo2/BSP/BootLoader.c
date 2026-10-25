#include "BootLoader.h"

char buffer[BootLoader_Buf_LEN] = "\0";
uint32_t flash_write_offset = 0;
uint8_t last_byte = 0xff;
uint8_t last_flag = 0;

void BootLoader_init(void)
{
    HAL_FLASH_Unlock();
    Erase_APPSector(GetSector(APP_START_ADDR));  
    HAL_FLASH_Lock();
}


uint32_t GetSector(uint32_t addr)
{
    if (addr < ADDR_FLASH_SECTOR_1)
        return FLASH_SECTOR_0;
    if (addr < ADDR_FLASH_SECTOR_2)
        return FLASH_SECTOR_1;
    if (addr < ADDR_FLASH_SECTOR_3)
        return FLASH_SECTOR_2;
    if (addr < ADDR_FLASH_SECTOR_4)
        return FLASH_SECTOR_3;
    if (addr < ADDR_FLASH_SECTOR_5)
        return FLASH_SECTOR_4;
    if (addr < ADDR_FLASH_SECTOR_6)
        return FLASH_SECTOR_5;
    if (addr < ADDR_FLASH_SECTOR_7)
        return FLASH_SECTOR_6;
    if (addr < ADDR_FLASH_SECTOR_8)
        return FLASH_SECTOR_7;
    if (addr < ADDR_FLASH_SECTOR_9)
        return FLASH_SECTOR_8;
    if (addr < ADDR_FLASH_SECTOR_10)
        return FLASH_SECTOR_9;
    if (addr < ADDR_FLASH_SECTOR_11)
        return FLASH_SECTOR_10;
    return FLASH_SECTOR_11;
}

void Erase_APPSector(uint32_t sector)
{
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t sector_err = 0;
    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;
    erase.Sector = sector;
    erase.NbSectors = 1;
    HAL_FLASHEx_Erase(&erase, &sector_err);
}

void BootLoader_Write_Programming(uint32_t destination_addr, uint8_t *program_data, uint32_t length)
{
    uint32_t i;
    uint32_t write_addr = destination_addr;
    uint64_t data64;   // F407 内部 Flash 按双字(64bit)编程

    HAL_FLASH_Unlock();

    /* 清除之前的错误标志，避免误判 */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP   | FLASH_FLAG_OPERR |
                           FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
                           FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    /*
     * F407 内部 Flash 编程粒度为双字(8 字节)，
     * 所以按 8 字节一组写入，最后一组不足 8 字节补 0xFF。
     */
    for (i = 0; i < length; i += 8)
    {
        /* 组装 8 字节数据，不足部分补 0xFF（Flash 擦除态） */
        data64  = 0xFFFFFFFFFFFFFFFFULL;
        uint8_t *p = (uint8_t *)&data64;

        if (length - i >= 8)
        {
            memcpy(p, &program_data[i], 8);
        }
        else
        {
            /* 最后不足 8 字节，逐字节填充，剩余保持 0xFF */
            uint32_t remain = length - i;
            for (uint32_t j = 0; j < remain; j++)
            {
                p[j] = program_data[i + j];
            }
        }

        /* 写入一个双字 */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, write_addr, data64) != HAL_OK)
        {
            /* 写入失败，可在此加入错误处理 */
            break;
        }

        write_addr += 8;
    }

    HAL_FLASH_Lock();
}

typedef void (*pFunction)(void);

void BootLoader_Jump_To_App(void)
{
    uint32_t app_stack = *(volatile uint32_t *)APP_START_ADDR;
    uint32_t app_reset = *(volatile uint32_t *)(APP_START_ADDR + 4);
    pFunction jump = (pFunction)app_reset;

    // 检查栈顶是否合法
    if ((app_stack & 0x2FFE0000) != 0x20000000)
    {
        printf("tranmit data is error");
        return;
    }
    printf("start jump\r\n");

    __disable_irq();
    HAL_DeInit();
    __set_MSP(app_stack);

    jump();
}
