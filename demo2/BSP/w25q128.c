#include "w25q128.h"

/* ============================================================
 *                        底层收发
 * ============================================================ */

/**
 * @brief  发送数据
 */
void W25Q128_Tx(uint8_t *buf, uint16_t len)
{
    HAL_SPI_Transmit(&hspi1, buf, len, HAL_MAX_DELAY);
}

/**
 * @brief  接收数据
 */
void W25Q128_Rx(uint8_t *buf, uint16_t len)
{
    HAL_SPI_Receive(&hspi1, buf, len, HAL_MAX_DELAY);
}

/**
 * @brief  收发一个字节
 */
uint8_t W25Q128_TxRxByte(uint8_t tx)
{
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

/* ============================================================
 *                        状态寄存器
 * ============================================================ */

/**
 * @brief  读 Status Register-1（S7~S0）
 *         bit0 = BUSY, bit1 = WEL
 */
uint8_t W25Q128_ReadStatus1(void)
{
    uint8_t cmd = W25Q128_CMD_READ_STATUS1;
    uint8_t status = 0;

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Rx(&status, 1);
    W25Q128_CS_HIGH;

    return status;
}

/**
 * @brief  读 Status Register-2（S15~S8）
 *         bit1 = QE
 */
uint8_t W25Q128_ReadStatus2(void)
{
    uint8_t cmd = W25Q128_CMD_READ_STATUS2;
    uint8_t status = 0;

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Rx(&status, 1);
    W25Q128_CS_HIGH;

    return status;
}

/**
 * @brief  等待 BUSY 位清零
 */
void W25Q128_WaitForReady(void)
{
    uint32_t timeout = 0xFFFFFF;
    while ((W25Q128_ReadStatus1() & 0x01) && (timeout--));
}

/* ============================================================
 *                       写使能 / 禁止
 * ============================================================ */

/**
 * @brief  写使能：置 WEL=1
 *         擦除、编程、写状态寄存器前必须调用
 */
void W25Q128_WriteEnable(void)
{
    uint8_t cmd = W25Q128_CMD_WRITE_ENABLE;
    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;
}

/**
 * @brief  写禁止：清 WEL=0
 */
void W25Q128_WriteDisable(void)
{
    uint8_t cmd = W25Q128_CMD_WRITE_DISABLE;
    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;
}

/* ============================================================
 *                          复位
 * ============================================================ */

/**
 * @brief  软件复位（66h + 99h）
 */
void W25Q128_Reset(void)
{
    uint8_t cmd;

    cmd = W25Q128_CMD_ENABLE_RESET;
    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;

    cmd = W25Q128_CMD_RESET_DEVICE;
    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;

    HAL_Delay(1);
}

/* ============================================================
 *                           ID
 * ============================================================ */

/**
 * @brief  读 JEDEC ID
 *         W25Q128BV 应为 0xEF4018
 */
uint32_t W25Q128_ReadJEDECID(void)
{
    uint8_t cmd = W25Q128_CMD_READ_JEDEC_ID;
    uint8_t buf[3] = {0};

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Rx(buf, 3);
    W25Q128_CS_HIGH;

    return ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
}

/**
 * @brief  读 Manufacturer ID 和 Device ID
 *         mid 应为 0xEF，did 应为 0x17（W25Q128BV 为 0x4018 时看 JEDEC）
 */
void W25Q128_ReadID(uint8_t *mid, uint16_t *did)
{
    uint8_t cmd = W25Q128_CMD_READ_MF_DEV_ID;
    uint8_t addr[3] = {0x00, 0x00, 0x00};
    uint8_t buf[4] = {0};

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Tx(addr, 3);
    W25Q128_Rx(buf, 4);
    W25Q128_CS_HIGH;

    if (mid) *mid = buf[0];
    if (did) *did = ((uint16_t)buf[1] << 8) | buf[2];
}

/* ============================================================
 *                          初始化
 * ============================================================ */

/**
 * @brief  初始化并检测芯片
 * @return 0=成功，1=失败
 */
uint8_t W25Q128_Init(void)
{
    W25Q128_CS_HIGH;
    HAL_Delay(10);

    W25Q128_Reset();
    W25Q128_WaitForReady();

    if (W25Q128_ReadJEDECID() == 0xEF4018) {
        return 0;
    }
    return 1;
}

/* ============================================================
 *                            读
 * ============================================================ */

/**
 * @brief  标准读（03h），最高约 33MHz
 */
void W25Q128_Read(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint8_t cmd = W25Q128_CMD_READ_DATA;
    uint8_t addr_buf[3];

    addr_buf[0] = (addr >> 16) & 0xFF;
    addr_buf[1] = (addr >> 8)  & 0xFF;
    addr_buf[2] =  addr        & 0xFF;

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Tx(addr_buf, 3);
    W25Q128_Rx(buf, (uint16_t)len);
    W25Q128_CS_HIGH;
}

/**
 * @brief  快速读（0Bh），加 8 个 dummy clocks，最高 104MHz
 */
void W25Q128_FastRead(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint8_t cmd = W25Q128_CMD_FAST_READ;
    uint8_t addr_buf[3];
    uint8_t dummy = 0x00;

    addr_buf[0] = (addr >> 16) & 0xFF;
    addr_buf[1] = (addr >> 8)  & 0xFF;
    addr_buf[2] =  addr        & 0xFF;

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Tx(addr_buf, 3);
    W25Q128_Tx(&dummy, 1);          /* 8 dummy clocks */
    W25Q128_Rx(buf, (uint16_t)len);
    W25Q128_CS_HIGH;
}

/* ============================================================
 *                            写
 * ============================================================ */

/**
 * @brief  页编程，最多 256 字节，不能跨页
 */
void W25Q128_PageProgram(uint8_t *buf, uint32_t addr, uint16_t len)
{
    uint8_t cmd = W25Q128_CMD_PAGE_PROGRAM;
    uint8_t addr_buf[3];

    if (len == 0 || len > W25Q128_PAGE_SIZE) return;

    addr_buf[0] = (addr >> 16) & 0xFF;
    addr_buf[1] = (addr >> 8)  & 0xFF;
    addr_buf[2] =  addr        & 0xFF;

    W25Q128_WriteEnable();

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Tx(addr_buf, 3);
    W25Q128_Tx(buf, len);
    W25Q128_CS_HIGH;

    W25Q128_WaitForReady();
}

/**
 * @brief  写入任意长度数据（调用前需确保目标区域已擦除）
 *         自动按页拆分
 */
void W25Q128_Write(uint8_t *buf, uint32_t addr, uint32_t len)
{
    uint32_t page_remain;

    while (len > 0) {
        page_remain = W25Q128_PAGE_SIZE - (addr % W25Q128_PAGE_SIZE);
        if (page_remain > len) page_remain = len;

        W25Q128_PageProgram(buf, addr, (uint16_t)page_remain);

        addr += page_remain;
        buf  += page_remain;
        len  -= page_remain;
    }
}

/* ============================================================
 *                           擦除
 * ============================================================ */

/**
 * @brief  扇区擦除（4KB），地址需 4KB 对齐
 */
void W25Q128_EraseSector(uint32_t addr)
{
    uint8_t cmd = W25Q128_CMD_SECTOR_ERASE;
    uint8_t addr_buf[3];

    addr_buf[0] = (addr >> 16) & 0xFF;
    addr_buf[1] = (addr >> 8)  & 0xFF;
    addr_buf[2] =  addr        & 0xFF;

    W25Q128_WriteEnable();

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Tx(addr_buf, 3);
    W25Q128_CS_HIGH;

    W25Q128_WaitForReady();
}

/**
 * @brief  64KB 块擦除，地址需 64KB 对齐
 */
void W25Q128_EraseBlock64K(uint32_t addr)
{
    uint8_t cmd = W25Q128_CMD_BLOCK_ERASE_64K;
    uint8_t addr_buf[3];

    addr_buf[0] = (addr >> 16) & 0xFF;
    addr_buf[1] = (addr >> 8)  & 0xFF;
    addr_buf[2] =  addr        & 0xFF;

    W25Q128_WriteEnable();

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_Tx(addr_buf, 3);
    W25Q128_CS_HIGH;

    W25Q128_WaitForReady();
}

/**
 * @brief  整片擦除（约 25~40 秒）
 */
void W25Q128_EraseChip(void)
{
    uint8_t cmd = W25Q128_CMD_CHIP_ERASE;

    W25Q128_WriteEnable();

    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;

    W25Q128_WaitForReady();
}

/* ============================================================
 *                          低功耗
 * ============================================================ */

/**
 * @brief  进入深度掉电模式
 */
void W25Q128_PowerDown(void)
{
    uint8_t cmd = W25Q128_CMD_POWER_DOWN;
    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;
    HAL_Delay(1);
}

/**
 * @brief  退出深度掉电模式
 */
void W25Q128_ReleasePowerDown(void)
{
    uint8_t cmd = W25Q128_CMD_RELEASE_PD;
    W25Q128_CS_LOW;
    W25Q128_Tx(&cmd, 1);
    W25Q128_CS_HIGH;
    HAL_Delay(1);
}


uint8_t W25Q128_Test(void)
{
    uint8_t  mid = 0;
    uint16_t did = 0;
    uint32_t jedec;
    uint8_t  tx[256];
    uint8_t  rx[256];

    printf("\r\n========== W25Q128 Test Start ==========\r\n");

    /* ---------- 1. 初始化 ---------- */
    if (W25Q128_Init() != 0) {
        printf("[FAIL] W25Q128 not detected!\r\n");
        return 1;
    }
    printf("[ OK ] W25Q128 detected\r\n");

    /* ---------- 2. 读 ID ---------- */
    jedec = W25Q128_ReadJEDECID();
    W25Q128_ReadID(&mid, &did);
    printf("[INFO] JEDEC ID = 0x%06lX (expect 0xEF4018)\r\n", jedec);
    printf("[INFO] MID = 0x%02X, DID = 0x%04X\r\n", mid, did);

    if (jedec != 0xEF4018) {
        printf("[FAIL] JEDEC ID mismatch!\r\n");
        return 1;
    }
    printf("[ OK ] JEDEC ID OK\r\n");

    /* ---------- 3. 擦除扇区 0 ---------- */
    printf("[INFO] Erasing sector 0 ...\r\n");
    W25Q128_EraseSector(0x000000);
    W25Q128_WaitForReady();

    W25Q128_Read(rx, 0x000000, 16);
    for (int i = 0; i < 16; i++) {
        if (rx[i] != 0xFF) {
            printf("[FAIL] Erase check failed at %d: 0x%02X\r\n", i, rx[i]);
            return 1;
        }
    }
    printf("[ OK ] Sector erase OK\r\n");

    /* ---------- 4. 单页写入 / 回读 ---------- */
    printf("[INFO] Test 1: single page write/read (256 bytes)\r\n");
    for (int i = 0; i < 256; i++) tx[i] = (uint8_t)i;

    W25Q128_Write(tx, 0x000000, 256);
    W25Q128_WaitForReady();

    memset(rx, 0, sizeof(rx));
    W25Q128_Read(rx, 0x000000, 256);
    for (int i = 0; i < 256; i++) {
        if (rx[i] != tx[i]) {
            printf("[FAIL] Test 1 mismatch at %d: tx=0x%02X rx=0x%02X\r\n",
                   i, tx[i], rx[i]);
            return 1;
        }
    }
    printf("[ OK ] Test 1 passed\r\n");

    /* ---------- 5. 跨页写入 / 回读 ---------- */
    printf("[INFO] Erasing sector 0 again ...\r\n");
    W25Q128_EraseSector(0x000000);
    W25Q128_WaitForReady();

    printf("[INFO] Test 2: cross-page write/read (500 bytes)\r\n");
    for (int i = 0; i < 500; i++) tx[i] = (uint8_t)(i & 0xFF);

    W25Q128_Write(tx, 0x000000, 500);
    W25Q128_WaitForReady();

    memset(rx, 0, sizeof(rx));
    W25Q128_Read(rx, 0x000000, 500);
    for (int i = 0; i < 500; i++) {
        if (rx[i] != tx[i]) {
            printf("[FAIL] Test 2 mismatch at %d: tx=0x%02X rx=0x%02X\r\n",
                   i, tx[i], rx[i]);
            return 1;
        }
    }
    printf("[ OK ] Test 2 passed\r\n");

    /* ---------- 6. Fast Read ---------- */
    printf("[INFO] Test 3: fast read\r\n");
    memset(rx, 0, sizeof(rx));
    W25Q128_FastRead(rx, 0x000000, 500);
    if (memcmp(rx, tx, 500) == 0) {
        printf("[ OK ] Test 3 passed\r\n");
    } else {
        printf("[FAIL] Test 3 failed\r\n");
        return 1;
    }

    printf("========== W25Q128 Test End ==========\r\n\r\n");
    return 0;
}
