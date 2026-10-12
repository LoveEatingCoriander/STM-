#include "Key.h"

volatile uint8_t key0_flag = 0;

uint8_t Key_Sacn(void)
{
    

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY0_Pin)
    {
        key0_flag = 1;
    }
}

