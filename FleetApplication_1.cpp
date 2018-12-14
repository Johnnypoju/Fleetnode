/*
===============================================================================
 Name        : main.c
 Author      : Joonas Sumusalo	
 Version     :
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
using namespace std;
#include <cr_section_macros.h>
#include "UART.h"
#include "SerialPort.h"
#include <vector>
#include <cstring>
static volatile int counter;
static volatile uint32_t systicks;

//#ifdef __cplusplus
/*extern "C" {
#endif
// TODO: insert other include files here
void SysTick_Handler(void)
{
	systicks++;
	if(counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif

void Sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}*/

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    /* Set up SWO to PIO1_2 */
    //Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2); // Needed for SWO printf

   	/* Enable and setup SysTick Timer at a periodic rate */
   	//SysTick_Config(SystemCoreClock / 1000);
    	//creating UART objects for data receive and send
   	UART *uart1 = new UART();
   	UART *uart2 = new UART(2);
   	/* test vector
   	vector <const char*> testvec;
   	const char* test = "ABCDEFG\n";*/
while (1){
	/* send test vector build
	testvec.clear();
	for (int i = 0; i < 8; i++){
		testvec.push_back(&test[i]);
	}*/
	uart1->UART_receive();
	uart2->UART_send(uart1->UART_get());




}
    // Enter an infinite loop, just incrementing a counter
    return 0 ;
}
