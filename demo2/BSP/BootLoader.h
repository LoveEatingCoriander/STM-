#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "usart.h"
#include "main.h"
#include "w25q128.h"

#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000)
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000)
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000)
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000)
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000)
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000)
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000)
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000)
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000)
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000)
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000)
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000)

#define BootLoader_Buf_LEN 512
#define APP_START_ADDR 0x08004000

extern char buffer[BootLoader_Buf_LEN];

void BootLoader_init(void);
static uint32_t GetSector(uint32_t addr);
void Erase_APPSector(uint32_t sector);
void BootLoader_Write_Programming(uint32_t destination_addr, uint8_t *program_data, uint32_t length);
void BootLoader_Jump_To_App(void);

#endif

