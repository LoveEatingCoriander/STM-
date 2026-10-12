#include "Bootloader.h"

char buffer[BOOTLOADER_LEN];
volatile uint16_t actual_length;
volatile uint16_t total_length=0;
volatile uint8_t Rx_flag=0;

void BootLoader_Init(void)
{
    __HAL_UART_CLEAR_IDLEFLAG(&huart1);
    __HAL_UART_CLEAR_OREFLAG(&huart1);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)buffer,sizeof(buffer));
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
    printf("Ready\r\n");
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if(huart->Instance==USART1)
    {
        actual_length=Size;
        total_length+=actual_length;
        Rx_flag=1;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1,(uint8_t*)buffer,sizeof(buffer));
        __HAL_DMA_DISABLE_IT(&hdma_usart1_rx,DMA_IT_HT);
    }
}

