#include "Key.h"

volatile uint8_t key0_flag = 0;

KEY Key_Sacn(void)
{
    if(key0_flag==0) 
    {
        key0_flag=0;
        return KEY0;
    }
    return NO_KEY;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY0_Pin)
    {
        key0_flag = 1;
    }
}

