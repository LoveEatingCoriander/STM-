#include "BootLoader.h"

char buffer[BootLoader_Buf_LEN] = "\0";
volatile uint16_t actual_length;
volatile uint16_t total_length = 0;
volatile uint8_t rx_flag = 0;
uint32_t flash_write_offset = 0;
uint8_t last_byte = 0xff;
uint8_t last_flag = 0;

void BootLoader_init(void)
{
    memset(buffer, 0, sizeof(buffer));
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    
    HAL_FLASH_Unlock();
    Erase_APPSector(FLASH_SECTOR_1);
    HAL_FLASH_Lock();

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)buffer, 512);

    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);

    HAL_Delay(100);
    printf("Ready\r\n");
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        actual_length = Size;
        total_length += Size;
        rx_flag = 1;

        BootLoader_Write_Programming();

        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)buffer, 512);
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
    }
}

static uint32_t GetSector(uint32_t addr)
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

void BootLoader_Write_Programming(void)
{
    HAL_FLASH_Unlock();

    // /* 1. 擦除：按扇区，去重 */
    // for (uint32_t i = 0; i < actual_length; i++)
    // {
    //     uint8_t *p = (volatile uint8_t *)(APP_START_ADDR + flash_write_offset + i);
    //     if (*p != 0xFF)
    //     {
    //         Erase_APPSector(GetSector((uint32_t)p));
    //     }
    // }

    uint16_t data16;
    uint32_t flash_addr; // ★ 改成 32 位！

    if (last_flag)
    {
        flash_addr = APP_START_ADDR + flash_write_offset - 1;
        data16 = last_byte | (buffer[0] << 8);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flash_addr, data16);
        last_flag = 0; // ★ 必须清零

        for (uint16_t i = 1; i < actual_length; i += 2)
        {
            if (i + 1 < actual_length)
            {
                flash_addr = APP_START_ADDR + flash_write_offset + i;
                data16 = buffer[i] | (buffer[i + 1] << 8);
                HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flash_addr, data16);
            }
            else
            {
                last_byte = buffer[i];
                last_flag = 1;
            }
        }
    }
    else
    {
        for (uint16_t i = 0; i < actual_length; i += 2)
        {
            if (i + 1 < actual_length)
            {
                flash_addr = APP_START_ADDR + i + flash_write_offset;
                data16 = buffer[i] | (buffer[i + 1] << 8);
                HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, flash_addr, data16);
            }
            else
            {
                last_byte = buffer[i];
                last_flag = 1;
            }
        }
    }

    HAL_FLASH_Lock();
    flash_write_offset += actual_length;
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
