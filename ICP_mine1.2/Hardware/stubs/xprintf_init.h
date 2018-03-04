#ifndef __X_PRINTFINIT_H__
#define __X_PRINTFINIT_H__

#define X_UART_RXB	128		/* Size of Rx buffer */
#define X_UART_TXB	128		/* Size of Tx buffer */

#include "xprintf.h"

void x_printf_init(void);

#endif
