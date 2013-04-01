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

volatile char response[1000];
int a;
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
		UARTCharPut(UART0_BASE, cChar);

	}
}

//*****************************************************************************
//
// The UART0 interrupt handler.
//
//****************************************************************************
//void
//UART0IntHandler(void)
//{
//	unsigned long ulStatus;
//	//tBoolean bRc;
//
//	// Get the interrupt status.
//	ulStatus = UARTIntStatus(UART0_BASE, true);
//
//	// Clear the asserted interrupts.
//	UARTIntClear(UART0_BASE, ulStatus);
//
//	// Mientras haya data en el UART1
//	UARTReceive(UART0_BASE, command);
//
//	//Print to terminal, device response.
//
//}


//////////////////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
//
// The UART1 interrupt handler.
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
	volatile long int i=1000000;
	volatile long int seg=1000000;//3000000 funciona con 38400 baudrate.
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
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
					UART_CONFIG_PAR_NONE));


	IntEnable(INT_UART1);
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);

	// Put a character to show start of example. This will display on the terminal.
	UARTCharPut(UART0_BASE, '!');
	//strlen((const char*)command)
	// Send to UART0 using UARTSend. Testing purposes.
	//	i=seg;
//	while(i--){}
//	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT", 2);
//	UARTCharPut(UART1_BASE, 13);
//	UARTCharPut(UART1_BASE, 26);

	/*
	 * Prints out possible baud rates.
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT+IPR=?",8);
	//	UARTCharPut(UART1_BASE, 13);
	//	UARTCharPut(UART1_BASE, 26);

	/*
	 *Creates context 1.
	 *APN: carspy.ece.uprm.edu
	 *IP address: dynamically assigned by the ISP
	 *Packet Data Protocol type: Internet Protocol (IP)
	 *Data compression: OFF
	 *Header compression: OFF
	 *
	 */
		i=seg;
		while(i--){}
		UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT+CGDCONT=1,IP,ibox.tim.it,0.0.0.0,0,0 <cr>",44);//52 with carspy.ece...
		UARTCharPut(UART1_BASE, 13);
		UARTCharPut(UART1_BASE, 26);

	/*
	 * The minimum quality of service requested parameters represent the
	 * boundary under which the connection quality is not anymore
	 * acceptable and will be terminated
	 * CID: 1
	 * Precedence: 0 (subcribed) Applied when network has a heavy duty and user precedence must be followed.
	 * Delay: 0 Maximum allowable time delay class between the sending and the reception of a packet
	 * Reliability: 0 connection reliability requested, the higher is the number the less reliable is the data exchanged
	 * Peak: 0 Peak data transfer throughput
	 * Mean: 0 Mean data transfer throughput
	 * @returns OK/ERROR
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT+CGQMIN=1,0,0,0,0,0",21);
	//	UARTCharPut(UART1_BASE, 13);
	//	UARTCharPut(UART1_BASE, 26);

	/*
	 *
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT+CGQREQ=1,0,0,3,0,0",21);
	//	UARTCharPut(UART1_BASE, 13);
	//	UARTCharPut(UART1_BASE, 26);

	/*
	 *Telit module enters Data Traffic Mode (Socket Mode)
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "ATD*99***1#",11);
	//	UARTCharPut(UART1_BASE, 13);
	//	UARTCharPut(UART1_BASE, 26);
	//
	//	SysCtlDelay(200000000);

	/*
	 * HAS to be sent after a connection has been established. It requires +++ to exit Data Traffic Mode (Socket Mode)
	 */
	//	i=seg;
	//	while(i--){}
	//	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "+++",3);
	//	SysCtlDelay(100000000); //Needs 2 second delay between the command and the LF and CR characters for correct reading of pause command.
	//	UARTCharPut(UART1_BASE, 13);
	//	UARTCharPut(UART1_BASE, 26);

	i=seg;
	while(i--){}
	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT#SCFG=1,1,512,30,300,100", 26);
	UARTCharPut(UART1_BASE, 13);
	UARTCharPut(UART1_BASE, 26);

	i=seg;
	while(i--){}
	UARTSend((unsigned long)UART1_BASE, (unsigned char *) "AT#SD=1,0,80,www.telit.com", 26);
	UARTCharPut(UART1_BASE, 13);
	UARTCharPut(UART1_BASE, 26);
	
	// DELAY??
	//	 Send to UART1 using UARTSend
	//	UARTSend((unsigned long)UART1_BASE, (unsigned char*)"AT+CREG", 7);
	//	UARTCharPut(UART1_BASE, 13);
	while(1){}
	return 0;
}