/*
 * main.c
 *
 *  Created on: 2017年7月17日
 *      Author: weycen
 */
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"

#include "driverlib/fpu.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"

#include "radio.h"
#include "motor_ctl.h"


volatile uint8_t ADCDataReadyFlag = 0;

uint16_t ch0_value, ch1_value;
//uint8_t data_to_car[8] = {0};


void ConfigureUART(uint32_t ui32Baud);
void timer0_Configure(uint32_t ui32Period_Xms, void (*pfnHandler)(void));



//*****************************************************************************
//  Timer0 intrrupt handler
//*****************************************************************************
void timer0_IntHandler(void)
{
	// 清中断标志位
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // 计数器
    static uint16_t led_counter = 0;  // led
    static uint16_t main_counter = 0; // 主程序

    //////////////////////////////////
    switch (main_counter)
    {
    case 10: {
    	main_counter = 0;
    } break;
    default: {
    	main_counter++;
    } break;
    }


    //////// control the led. //////
    switch (led_counter)
    {
    case 0: {
        // turn on led.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
        led_counter ++ ;
        if(command_last==command_number&&command_number>1)
        {
            command=0;
            data_complete_flag=true;//给修改权限
            command_number=0;
        }
        command_last=command_number;
    }  break;
    case 500: {
        // turn off led.
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);
        led_counter ++ ;
        if(command_last==command_number&&command_number>1)
        {
            command=0;
            data_complete_flag=true;
            command_number=0;
        }
        command_last=command_number;
    }  break;
    case 1000: {
    	// led计数器清零
    	led_counter = 0;
        if(command_last==command_number&&command_number>1)
        {
            command=0;
            data_complete_flag=true;
            command_number=0;
        }
        command_last=command_number;
    } break;
    default: {
    	led_counter ++ ;
    } break;
    }
}



void main(void)
{
    //
    // configure system clock, divide _2_5_, run at 80MHz.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // enable the float-process-unit.
    //
    FPUEnable();
    FPULazyStackingEnable();

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    // 关总中断
    IntMasterDisable();

    //
    // Configure the uart0 for printf.
    //
    ConfigureUART( 115200 );

    //
    // 初始化无线串口
    //
    radio_Init(115200);

    //
    // 初始化定时器
    //
    timer0_Configure(1, timer0_IntHandler);

    //
    // 初始化pwm
    //
    motor_Init();

    // 开总中断
    IntMasterEnable();


    while(1)
    {
    	if(data_complete_flag == true) {
    		data_complete_flag = false;

    		// 更新电机pwm
//    		motor_Control(receive_buffer);
    		car_driver(command);

    	}
    }
}



//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void ConfigureUART(uint32_t ui32Baud)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, ui32Baud, 16000000);
}

//*****************************************************************************
//
// Configure the timer0 and its pins.
//  ui32Period -- interrupt period, Xms.
//*****************************************************************************
void timer0_Configure(uint32_t ui32Period_Xms, void (*pfnHandler)(void))
{
    //
    // enable the peripheral timer0.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //
    // configuer count.
    //
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    //
    // Set interrupt period.
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period_Xms *
            ( SysCtlClockGet() / 1000) );

    //
    // Registe the inthandler.
    //
    TimerIntRegister(TIMER0_BASE,TIMER_A, pfnHandler);

    //
    // Enable the time-out interrupt.
    //
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    //
    // Enable timer0A interrupt.
    //
    IntEnable(INT_TIMER0A); //0xE0 0xD0 0xC0

    //
    // set the interrut priority
    //
    IntPrioritySet (INT_TIMER0A, 0x0); //优先级最高highest

    //
    // Turn on timer0A.
    //
    TimerEnable(TIMER0_BASE, TIMER_A); //使能timer0
}
