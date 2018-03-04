#include "xprintf_init.h"
#include "usart.h"

static volatile struct {
	uint16_t	tri, twi, tct;
	uint16_t	rri, rwi, rct;
	uint8_t		tbuf[X_UART_TXB];
	uint8_t		rbuf[X_UART_RXB];
} x_Fifo;



//void USART1_IRQHandler (void)
//{
//	uint32_t sr = USART1_SR;	/* Interrupt flags */
//	uint8_t d;
//	int i;


//	if (sr & _BV(5)) {	/* RXNE is set: Rx ready */
//		d = USART1_DR;	/* Get received byte */
//		i = Fifo1.rct;
//		if (i < UART1_RXB) {	/* Store it into the rx fifo if not full */
//			Fifo1.rct = ++i;
//			i = Fifo1.rwi;
//			Fifo1.rbuf[i] = d;
//			Fifo1.rwi = ++i % UART1_RXB;
//		}
//	}

//	if (sr & _BV(7)) {	/* TXE is set: Tx ready */
//		i = Fifo1.tct;
//		if (i--) {	/* There is any data in the tx fifo */
//			Fifo1.tct = (uint16_t)i;
//			i = Fifo1.tri;
//			USART1_DR = Fifo1.tbuf[i];
//			Fifo1.tri = ++i % UART1_TXB;
//		} else {	/* No data in the tx fifo */
//			USART1_CR1 &= ~_BV(7);		/* Clear TXEIE - Disable TXE irq */
//		}
//	}
//}



int x_uart_test (void)
{
	return x_Fifo.rct;
}



//uint8_t x_uart_getc (void)
//{
//	uint8_t d;
//	int i;

//	/* Wait while rx fifo is empty */
//	while (!x_Fifo.rct) ;

//	i = x_Fifo.rri;			/* Get a byte from rx fifo */
//	d = x_Fifo.rbuf[i];
//	x_Fifo.rri = ++i % X_UART_RXB;
//	__disable_irq();
//	x_Fifo.rct--;
//	__enable_irq();

//	return d;
//}



//void x_uart_putc (uint8_t d)
//{
//	int i;

//	/* Wait for tx fifo is not full */
//	while (x_Fifo.tct >= X_UART_TXB) ;

//	i = x_Fifo.twi;		/* Put a byte into Tx fifo */
//	x_Fifo.tbuf[i] = d;
//	x_Fifo.twi = ++i % X_UART_TXB;
//	__disable_irq();
//	x_Fifo.tct++;
//	USART1_CR1 |= _BV(7);	/* Set TXEIE - Enable TXE irq */
//	__enable_irq();
//}

void x_uart_putc (uint8_t d){
	
 	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
	USART6->DR = (uint8_t) d;      

}

uint8_t x_uart_getc (void){
  return 0;
}

void x_printf_init(void){
  xdev_out(x_uart_putc);
	xdev_in(x_uart_getc);
}
