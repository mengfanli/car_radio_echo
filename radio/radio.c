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
#include "bee.h"

//#define FR_HEAD1    0xAB   // 帧头1
//#define FR_HEAD2    0xBA   // 帧头2
//#define FR_LEN   	0x08   // 帧长度
//#define FR_TYPE   	0xA1   // 帧类型
#define FR_HEAD1    0xfe   // 帧头1
#define FR_HEAD2    0x80  // 帧头2
#define FR_LEN      0x08   // 帧长度
#define FR_TYPE     0xA1   // 帧类型

//uint8_t  receive_buffer[8]  = {0};
//uint8_t  command=0;
//uint16_t command_number=1;
//uint8_t command_last=1;
volatile bool data_complete_flag = false;


//*****************************************************************************
// 无线数传用The UART1 RX、RT interrupt handler.
//*****************************************************************************
static void UART1IntHandler(void)
{
//    IntDisable(INT_TIMER0A);
    uint32_t ui32Status;

    ui32Status = UARTIntStatus(UART1_BASE, true);
    UARTIntClear(UART1_BASE, ui32Status);

    // Loop while there are no characters in the receive FIFO.
    // UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART1_BASE));
    while(UARTCharsAvail(UART1_BASE))
    {
        command_number++;
    	receive_buffer[0] = UARTCharGetNonBlocking(UART1_BASE);
    	data_complete_flag = true;
    	if(receive_buffer[0]>0x00&&receive_buffer[0]<0x08)
    	{
    	    command=receive_buffer[0];
    	}
    	if(receive_buffer[0]==0xac)
    	{
    	    UARTCharPutNonBlocking(UART3_BASE,0xac);
    	    UARTCharPutNonBlocking(UART3_BASE,0xac);
    	}
    }
//    IntEnable(INT_TIMER0A);
    command_end=1;
}
static void UART3IntHandler(void)
{

    uint32_t ui32Status;
    uint8_t receive_buffer2[2]={0};
    ui32Status = UARTIntStatus(UART3_BASE, true);
    UARTIntClear(UART3_BASE, ui32Status);

    // Loop while there are no characters in the receive FIFO.
    // UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART1_BASE));
    while(UARTCharsAvail(UART3_BASE))
    {
//        receive_buffer2[0] = UARTCharGetNonBlocking(UART2_BASE);
        receive_buffer2[0] =UARTCharGet(UART3_BASE);
        if(receive_buffer2[0] ==0xef){
//            receive_buffer2[1]=UARTCharGetNonBlocking(UART2_BASE);
            receive_buffer2[1] =UARTCharGet(UART3_BASE);
            switch (receive_buffer2[1]){
            case 0xe1:
                bee_on();
                break;
            case 0xe0:
                bee_off();
            }
        }
        else
            UARTCharPutNonBlocking(UART1_BASE,*receive_buffer2);
//        UARTCharPutNonBlocking(UART1_BASE,UARTCharGetNonBlocking(UART2_BASE));
    }
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
//	UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
	UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

	UARTIntRegister(UART1_BASE, UART1IntHandler);

	IntEnable(INT_UART1);

	// higth-priority
	IntPrioritySet (INT_UART1, 0x00);

	// Enable recive、recive timeout interrupt
	UARTIntEnable(UART1_BASE, UART_INT_RX);
}

static void UART3_Configuer(uint32_t baud)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    GPIOPinConfigure(GPIO_PC6_U3RX);
    GPIOPinConfigure(GPIO_PC7_U3TX);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    // Configure the UART for 9600, 8-N-1 operation.
    UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), baud,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    UARTFIFOEnable(UART3_BASE);
    // Set recive fifo-level 8 bytes
//  UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
    UARTFIFOLevelSet(UART3_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

    UARTIntRegister(UART3_BASE, UART3IntHandler);

    IntEnable(INT_UART3);

    IntPrioritySet (INT_UART3, 0x00);

    // Enable recive、recive timeout interrupt
    UARTIntEnable(UART3_BASE, UART_INT_RX|UART_INT_RT);//|

}


// 串口无线传参初始化
void radio_Init(uint32_t baud1,uint32_t baud2)
{
	UART1_Configuer(baud1);
	UART3_Configuer(baud2);
	bee_init();
}
