/*
 * SerialPort.cpp
 *
 *  Created on: 10.2.2016
 *      Author: krl
 */
#include "SerialPort.h"


//#define LPC_USART1       LPC_USART1
//#define LPC_USART2		 LPC_USART2
#define LPC_IRQNUM1		UART1_IRQn
#define LPC_IRQNUM2		UART2_IRQn
#define LPC_UARTHNDLR1   UART1_IRQHandler
#define LPC_UARTHNDLR2  UART2_IRQHandler

/* Enable this define to use integer clocking instead of the fractional baud
   rate generator */
#define USE_INTEGER_CLOCK

static RINGBUFF_T *rxring11;
static RINGBUFF_T *txring11;
static RINGBUFF_T *rxring22;
static RINGBUFF_T *txring22;

extern "C" {
/**
 * @brief	UART interrupt handler using ring buffers
 * @return	Nothing
 */

void LPC_UARTHNDLR1(void)
{
	/* Want to handle any errors? Do it here. */

	/* Use default ring buffer handler. Override this with your own
	   code if you need more capability. */
	Chip_UART_IRQRBHandler(LPC_USART1, rxring11, txring11);
}
void LPC_UARTHNDLR2(void)
{
	Chip_UART_IRQRBHandler(LPC_USART2, rxring22, txring22);
}
}

SerialPort::SerialPort() {
	UFlag = false;
	/* UART signals on pins PIO0_13 (FUNC0, U0_TXD) and PIO0_18 (FUNC0, U0_RXD) */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 19, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 10, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
	//Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 29, (IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // RTS (for direction control)

	/* UART signal muxing via SWM */
	Chip_SWM_MovablePortPinAssign(SWM_UART1_RXD_I, 1, 10);
	Chip_SWM_MovablePortPinAssign(SWM_UART1_TXD_O, 1, 9);
	//Chip_SWM_MovablePortPinAssign(SWM_UART1_RTS_O, 0, 29);


	/* Before setting up the UART, the global UART clock for USARTS 1-4
	   must first be setup. This requires setting the UART divider and
	   the UART base clock rate to 16x the maximum UART rate for all
	   UARTs. */
#if defined(USE_INTEGER_CLOCK)
	/* Use main clock rate as base for UART baud rate divider */
	Chip_Clock_SetUARTBaseClockRate(Chip_Clock_GetMainClockRate(), false);

#else
	/* Use 128x expected UART baud rate for fractional baud mode. */
	Chip_Clock_SetUARTBaseClockRate((115200 * 128), true);
#endif

	/* Setup UART */
	Chip_UART_Init(LPC_USART1);
	Chip_UART_ConfigData(LPC_USART1, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
	Chip_UART_SetBaud(LPC_USART1, 9600);

	//LPC_USART1->CFG |= (1 << 20); // enable rs485 mode
	//LPC_USART->CFG |= (1 << 18); // OE turnaraound time
	//LPC_USART->CFG |= (1 << 21);// driver enable polarity (active high)

	/* Optional for low clock rates only:
	Chip_UART_SetBaudWithRTC32K(LPC_USART, 300); */
	Chip_UART_Enable(LPC_USART1);
	Chip_UART_TXEnable(LPC_USART1);

	/* Before using the ring buffers, initialize them using the ring
	   buffer init function */
	RingBuffer_Init(&rxring1, rxbuff, 1, UART_RB_SIZE);
	RingBuffer_Init(&txring1, txbuff, 1, UART_RB_SIZE);

	/* set the fixed pointers for ISR
	 * a fixed ISR binds this object to a single UART, which is not proper OO programming
	 */
	rxring11 = &rxring1;
	txring11 = &txring1;

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(LPC_USART1, UART_INTEN_RXRDY);
	Chip_UART_IntDisable(LPC_USART1, UART_INTEN_TXRDY);	/* May not be needed */

	/* Enable UART interrupt */
	NVIC_EnableIRQ(LPC_IRQNUM1);

}

SerialPort::SerialPort(int sport) {

	if (sport == 1) {
		UFlag = false;
		/* UART signals on pins PIO0_13 (FUNC0, U0_TXD) and PIO0_18 (FUNC0, U0_RXD) */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 19, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 10, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		//Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 29, (IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // RTS (for direction control)

		/* UART signal muxing via SWM */
		Chip_SWM_MovablePortPinAssign(SWM_UART1_RXD_I, 1, 10);
		Chip_SWM_MovablePortPinAssign(SWM_UART1_TXD_O, 1, 9);
		//Chip_SWM_MovablePortPinAssign(SWM_UART1_RTS_O, 0, 29);


		/* Before setting up the UART, the global UART clock for USARTS 1-4
		   must first be setup. This requires setting the UART divider and
		   the UART base clock rate to 16x the maximum UART rate for all
		   UARTs. */
	#if defined(USE_INTEGER_CLOCK)
		/* Use main clock rate as base for UART baud rate divider */
		Chip_Clock_SetUARTBaseClockRate(Chip_Clock_GetMainClockRate(), false);

	#else
		/* Use 128x expected UART baud rate for fractional baud mode. */
		Chip_Clock_SetUARTBaseClockRate((115200 * 128), true);
	#endif

		/* Setup UART */
		Chip_UART_Init(LPC_USART1);
		Chip_UART_ConfigData(LPC_USART1, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
		Chip_UART_SetBaud(LPC_USART1, 9600);

		//LPC_USART1->CFG |= (1 << 20); // enable rs485 mode
		//LPC_USART->CFG |= (1 << 18); // OE turnaraound time
		//LPC_USART->CFG |= (1 << 21);// driver enable polarity (active high)

		/* Optional for low clock rates only: Chip_UART_SetBaudWithRTC32K(LPC_USART, 300); */
		Chip_UART_Enable(LPC_USART1);
		Chip_UART_TXEnable(LPC_USART1);

		/* Before using the ring buffers, initialize them using the ring
		   buffer init function */
		RingBuffer_Init(&rxring1, rxbuff, 1, UART_RB_SIZE);
		RingBuffer_Init(&txring1, txbuff, 1, UART_RB_SIZE);

		/* set the fixed pointers for ISR
		 * a fixed ISR binds this object to a single UART, which is not proper OO programming
		 */
		rxring11 = &rxring1;
		txring11 = &txring1;

		/* Enable receive data and line status interrupt */
		Chip_UART_IntEnable(LPC_USART1, UART_INTEN_RXRDY);
		Chip_UART_IntDisable(LPC_USART1, UART_INTEN_TXRDY);	/* May not be needed */

		/* Enable UART interrupt */
		NVIC_EnableIRQ(LPC_IRQNUM1);

	}
	else if (sport == 2){
		UFlag = true;
		/* UART signals on pins PIO0_13 (FUNC0, U0_TXD) and PIO0_18 (FUNC0, U0_RXD) */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 12, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 15, (IOCON_MODE_INACT | IOCON_DIGMODE_EN));
		//Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 29, (IOCON_MODE_INACT | IOCON_DIGMODE_EN)); // RTS (for direction control)

		/* UART signal muxing via SWM */
		Chip_SWM_MovablePortPinAssign(SWM_UART2_RXD_I, 0, 10);
		Chip_SWM_MovablePortPinAssign(SWM_UART2_TXD_O, 0, 9);
		//Chip_SWM_MovablePortPinAssign(SWM_UART1_RTS_O, 0, 29);


		/* Before setting up the UART, the global UART clock for USARTS 1-4
		   must first be setup. This requires setting the UART divider and
		   the UART base clock rate to 16x the maximum UART rate for all
		   UARTs. */
	#if defined(USE_INTEGER_CLOCK)
		/* Use main clock rate as base for UART baud rate divider */
		Chip_Clock_SetUARTBaseClockRate(Chip_Clock_GetMainClockRate(), false);

	#else
		/* Use 128x expected UART baud rate for fractional baud mode. */
		Chip_Clock_SetUARTBaseClockRate((115200 * 128), true);
	#endif

		/* Setup UART */
		Chip_UART_Init(LPC_USART2);
		Chip_UART_ConfigData(LPC_USART2, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1);
		Chip_UART_SetBaud(LPC_USART2, 9600);

		//LPC_USART2->CFG |= (1 << 20); // enable rs485 mode
		//LPC_USART->CFG |= (1 << 18); // OE turnaraound time
		//LPC_USART->CFG |= (1 << 21);// driver enable polarity (active high)

		/* Optional for low clock rates only: Chip_UART_SetBaudWithRTC32K(LPC_USART, 300); */
		Chip_UART_Enable(LPC_USART2);
		Chip_UART_TXEnable(LPC_USART2);

		/* Before using the ring buffers, initialize them using the ring
		   buffer init function */
		RingBuffer_Init(&rxring2, rxbuff, 1, UART_RB_SIZE);
		RingBuffer_Init(&txring2, txbuff, 1, UART_RB_SIZE);

		/* set the fixed pointers for ISR
		 * a fixed ISR binds this object to a single UART, which is not proper OO programming
		 */
		rxring22 = &rxring2;
		txring22 = &txring2;

		/* Enable receive data and line status interrupt */
		Chip_UART_IntEnable(LPC_USART2, UART_INTEN_RXRDY);
		Chip_UART_IntDisable(LPC_USART2, UART_INTEN_TXRDY);	/* May not be needed */

		/* Enable UART interrupt */
		NVIC_EnableIRQ(LPC_IRQNUM2);

	}
}

SerialPort::~SerialPort() {
	/* DeInitialize UART peripheral */
	NVIC_DisableIRQ(LPC_IRQNUM1);
	NVIC_DisableIRQ(LPC_IRQNUM2);
	Chip_UART_DeInit(LPC_USART1);
	Chip_UART_DeInit(LPC_USART2);
}

int SerialPort::available() {
	if (UFlag == false) {
		return RingBuffer_GetCount(&rxring1);
	}
	else if (UFlag == true){
		return RingBuffer_GetCount(&rxring2);
	}
}

void SerialPort::begin(int speed) {
	if (UFlag == false){
		Chip_UART_SetBaud(LPC_USART1, speed);
	}
	else if (UFlag == true){
		Chip_UART_SetBaud(LPC_USART2, speed);
	}
}

int SerialPort::read() {
	uint8_t byte;
	int value;
	if (UFlag == false){
		value = Chip_UART_ReadRB(LPC_USART1, &rxring1, &byte, 1);
		if(value > 0) return (byte);
	}
	else if (UFlag == true){
		value = Chip_UART_ReadRB(LPC_USART2, &rxring2, &byte, 1);
		if(value > 0) return (byte);
	}
	return -1;
}
int SerialPort::write(const char* buf, int len) {
	if (UFlag == false) {
		Chip_UART_SendRB(LPC_USART1, &txring1, buf, len);
	}
	else if (UFlag ==true){
		Chip_UART_SendRB(LPC_USART2, &txring2, buf, len);
	}
	return len;
}

int SerialPort::print(int val, int format) {
	uint8_t byte = val;
	if (UFlag == false){
		Chip_UART_SendRB(LPC_USART1, &txring1, &byte, 1);
	}
	else if (UFlag == true){
		Chip_UART_SendRB(LPC_USART2, &txring2, &byte, 1);
	}
	return (0);
}

void SerialPort::flush() {
	if (UFlag == false){
		while(RingBuffer_GetCount(&txring1)>0) __WFI();
	}
	else if (UFlag == true) {
		while(RingBuffer_GetCount(&txring2)>0) __WFI();
	}
}
