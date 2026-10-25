#ifndef _W25Q128_H_
#define _W25Q128_H_

#include "spi.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* ================== CS 控制 ================== */
#define W25Q128_CS_LOW   HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_RESET)
#define W25Q128_CS_HIGH  HAL_GPIO_WritePin(W25Q128_CS_GPIO_Port, W25Q128_CS_Pin, GPIO_PIN_SET)

/* ================== 芯片参数 ================== */
#define W25Q128_PAGE_SIZE       256
#define W25Q128_SECTOR_SIZE     4096
#define W25Q128_BLOCK_SIZE      65536
#define W25Q128_PAGE_COUNT      65536
#define W25Q128_SECTOR_COUNT    4096
#define W25Q128_BLOCK_COUNT     256
#define W25Q128_TOTAL_SIZE      (16 * 1024 * 1024)   /* 16MB */

/* ================== 指令码 ================== */
#define W25Q128_CMD_WRITE_ENABLE        0x06
#define W25Q128_CMD_WRITE_DISABLE       0x04
#define W25Q128_CMD_READ_STATUS1        0x05
#define W25Q128_CMD_READ_STATUS2        0x35
#define W25Q128_CMD_WRITE_STATUS        0x01
#define W25Q128_CMD_PAGE_PROGRAM        0x02
#define W25Q128_CMD_QUAD_PAGE_PROGRAM   0x32
#define W25Q128_CMD_SECTOR_ERASE        0x20
#define W25Q128_CMD_BLOCK_ERASE_32K     0x52
#define W25Q128_CMD_BLOCK_ERASE_64K     0xD8
#define W25Q128_CMD_CHIP_ERASE          0xC7
#define W25Q128_CMD_READ_DATA           0x03
#define W25Q128_CMD_FAST_READ           0x0B
#define W25Q128_CMD_READ_JEDEC_ID       0x9F
#define W25Q128_CMD_READ_MF_DEV_ID      0x90
#define W25Q128_CMD_RELEASE_PD          0xAB
#define W25Q128_CMD_POWER_DOWN          0xB9
#define W25Q128_CMD_ENABLE_RESET        0x66
#define W25Q128_CMD_RESET_DEVICE        0x99

/* ================== 底层收发 ================== */
void    W25Q128_Tx(uint8_t *buf, uint16_t len);
void    W25Q128_Rx(uint8_t *buf, uint16_t len);
uint8_t W25Q128_TxRxByte(uint8_t tx);

/* ================== 状态寄存器 ================== */
uint8_t W25Q128_ReadStatus1(void);
uint8_t W25Q128_ReadStatus2(void);
void    W25Q128_WaitForReady(void);

/* ================== 写使能 / 禁止 ================== */
void    W25Q128_WriteEnable(void);
void    W25Q128_WriteDisable(void);

/* ================== 复位 ================== */
void    W25Q128_Reset(void);

/* ================== ID ================== */
uint32_t W25Q128_ReadJEDECID(void);
void     W25Q128_ReadID(uint8_t *mid, uint16_t *did);

/* ================== 初始化 ================== */
uint8_t W25Q128_Init(void);

/* ================== 读 ================== */
void    W25Q128_Read(uint8_t *buf, uint32_t addr, uint32_t len);
void    W25Q128_FastRead(uint8_t *buf, uint32_t addr, uint32_t len);

/* ================== 写 ================== */
void    W25Q128_PageProgram(uint8_t *buf, uint32_t addr, uint16_t len);
void    W25Q128_Write(uint8_t *buf, uint32_t addr, uint32_t len);

/* ================== 擦除 ================== */
void    W25Q128_EraseSector(uint32_t addr);
void    W25Q128_EraseBlock64K(uint32_t addr);
void    W25Q128_EraseChip(void);

/* ================== 低功耗 ================== */
void    W25Q128_PowerDown(void);
void    W25Q128_ReleasePowerDown(void);

/* ================== 测试 ================== */
uint8_t W25Q128_Test(void);

#endif /* _W25Q128_H_ */
