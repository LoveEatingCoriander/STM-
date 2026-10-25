#ifndef _KEY_H_
#define _KEY_H_

#include "main.h"
#include "gpio.h"

typedef enum
{
    NO_KEY,
    KEY0,
}KEY;

extern volatile uint8_t key0_flag;

KEY Key_Sacn(void);

#endif
