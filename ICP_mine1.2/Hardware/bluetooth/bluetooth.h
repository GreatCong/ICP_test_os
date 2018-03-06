#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "stm32f4xx_hal.h"

void UsartBound_init(uint32_t baudrate);
void SetBluetoothBaud(uint32_t baudrate);

#endif
