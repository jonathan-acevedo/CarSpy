#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "string.h"
#include "stdio.h"

volatile char response[100];
volatile int a;
char cChar;

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(unsigned long ulBase, const unsigned char *pucBuffer, unsigned long ulCount)
{
  //
	// Loop while there are more characters to send.
	//
	while(ulCount--)
	{
		//
		// Write the next character to the UART.
		//
		UARTCharPut(ulBase, *pucBuffer++);
	}
}



//*****************************************************************************
//
// Receive a string from the UART.
//
//*****************************************************************************
void
UARTReceive(unsigned long ulBase, volatile char pucBuffer[])
{
	//
	// Loop while there are more characters to send.
	//
	while(UARTCharsAvail(ulBase))
	{
		// Read the next character from the UART
		cChar = UARTCharGet(ulBase);
		pucBuffer[a++] = cChar;

		//Print device response to terminal
		UARTCharPut(UART0_BASE, cChar);

	}
}


//*****************************************************************************
//
// The UART interrupt handler.
//
//****************************************************************************
void
UART1IntHandler(void)
{
	unsigned long ulStatus;
	//tBoolean bRc;

	// Get the interrupt status.
	ulStatus = UARTIntStatus(UART1_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(UART1_BASE, ulStatus);

	// Mientras haya data en el UART1
	UARTReceive(UART1_BASE, response);

}


//////////////////////////////////////////////////////////////////////////////////////////

int main(void)
{
	unsigned char bytes[5];


	// Setear clock
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	//Configuracion de los puertos UART que se van a usar en el micro
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	// Enable processor interrupts
	IntMasterEnable();

	// Configure the GPIO pin muxing for the UART function.
	GPIOPinConfigure(0x00000001);//GPIO_PA0_U0RX
	GPIOPinConfigure(0x00000401);//GPIO_PA1_U0TX
	GPIOPinConfigure(0x00010001);//GPIO_PB0_U0RX
	GPIOPinConfigure(0x00010401);//GPIO_PB1_U0TX

	// Since GPIO A0 and A1 are used for the UART function, they must be
	// configured for use as a peripheral function (instead of GPIO).
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 38400,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));

	// Enable interrupt
	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	// Put a character to show start of example. This will display on the terminal.
	UARTCharPut(UART0_BASE, '!');

	// Reset Command
	bytes[0] = 86;
	bytes[1] = 0;
	bytes[2] = 38;
	bytes[3] = 0;
	//bytes[4] = '\0';

	// Send to UART0 using UARTSend
	UARTSend((unsigned long)UART0_BASE, bytes, sizeof(bytes));

	// DELAY??

	// Send to UART1 using UARTSend
	UARTSend((unsigned long)UART1_BASE, bytes, sizeof(bytes));

	while(1){}

	return 0;
}




