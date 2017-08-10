/*
 * radio.c
 *
 *  Created on: 2016年7月12日
 *      Author: weycen
 *
 *     无线串口
 */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "motor_ctl.h"
#include "radio.h"

//#define FR_HEAD1    0xAB   // 帧头1
//#define FR_HEAD2    0xBA   // 帧头2
//#define FR_LEN   	0x08   // 帧长度
//#define FR_TYPE   	0xA1   // 帧类型
#define FR_HEAD1    0xfe   // 帧头1
#define FR_HEAD2    0x80  // 帧头2
#define FR_LEN      0x08   // 帧长度
#define FR_TYPE     0xA1   // 帧类型

uint8_t  receive_buffer[8]  = {0};
uint8_t  command=0;
uint16_t command_number=1;
uint8_t command_last=1;
volatile bool data_complete_flag = false;


//*****************************************************************************
// 无线数传用The UART1 RX、RT interrupt handler.
//*****************************************************************************
static void UART1IntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);

    static uint16_t now_i = 8;

    // Loop while there are no characters in the receive FIFO.
    // UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART1_BASE));
    while(UARTCharsAvail(UART1_BASE))
    {
        command_number++;
    	receive_buffer[0] = UARTCharGetNonBlocking(UART1_BASE);

//    	if(i == 3) {
//    		if((receive_buffer[0] != FR_HEAD1) || (receive_buffer[2] != FR_HEAD2)) {
//    		    		i = 0;
//    		    	}
//    	if((receive_buffer[(now_i-3)%8] == FR_HEAD1) &&(receive_buffer[(now_i-2)%8] == FR_HEAD1)&& (receive_buffer[now_i%8] == FR_HEAD2))
//    	{
//    	    command=receive_buffer[(now_i-1)%8];
//    		data_complete_flag = true;
//    		break;
//    	}
    	if(receive_buffer[0]>0x00&&receive_buffer[0]<0x08)
    	{
    	    command=receive_buffer[0];
    	}
    }

    while(UARTCharsAvail(UART1_BASE)) {
    	UARTCharGetNonBlocking(UART1_BASE);
    }
    command_number++;
}



//********************************
//  无线串口中断配置函数
//********************************
static void UART1_Configuer(uint32_t baud)
{
	// Enable the peripherals used by this example.
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	// Set GPIO A0 and A1 as UART pins.
	GPIOPinConfigure(GPIO_PB0_U1RX);
	GPIOPinConfigure(GPIO_PB1_U1TX);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	// Configure the UART for 9600, 8-N-1 operation.
//	UARTConfigSetExpClk(UART1_BASE, 32000000, baud,
//	                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//	                         UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), baud,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
	UARTFIFOEnable(UART1_BASE);
	// Set recive fifo-level 8 bytes
	UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);

	UARTIntRegister(UART1_BASE, UART1IntHandler);

	IntEnable(INT_UART1);

	// lowest int-priority
	IntPrioritySet (INT_UART1, 0x08);

	// Enable recive、recive timeout interrupt
	UARTIntEnable(UART1_BASE, UART_INT_RX);
}


// 串口无线传参初始化
void radio_Init(uint32_t baud)
{
	UART1_Configuer(baud);
}
