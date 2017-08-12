/*
 * motor_ctl.c
 *
 *  Created on: 2017��7��7��
 *      Author: weycen
 *
 *      ֱ�ӶԵ���Ŀ���
 *      M1:PF0 dir: PA2,3
 *      M3:PF1 dir: PA6,7
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "motor_ctl.h"
#include "radio.h"


static void motor_Set1Dir(uint16_t ui16CW);
static void motor_Set3Dir(uint16_t ui16CW);
static void motor_DirInit(void);
static void motor_SetMotor1(int pwm_value);
static void motor_SetMotor3(int pwm_value);

unsigned char direction;
//*****************************************************************************
//
// MOTOR-PWM��ʼ���ú���
//
//*****************************************************************************
void motor_Init(void)
{
    //ʹ��GPIOC/GPIOE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_MOTORX);

	// ����ʱ�ӣ�����Ƶ
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    // The PWM peripheral must be enabled for use
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM_MOTORX);//ʹ��pwmX

	// �������蹦��
    GPIOPinConfigure(GPIO_PIN_MOTOR1_PWM); //MOTOR1
    GPIOPinConfigure(GPIO_PIN_MOTOR3_PWM); //MOTOR3

    // Configure the PWM function for this pin.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    GPIOPinTypePWM(GPIO_PORT_MOTORX_BASE, GPIO_PINS_MOTORX);

    // Configure the PWM to count down without synchronization.
    // ���ò�ͬ��, �ݼ�����
    PWMGenConfigure(PWM_MOTORX_BASE, PWM_GEN_MOTORX, PWM_GEN_MODE_DOWN |
                    PWM_GEN_MODE_NO_SYNC);

    // N = SysClk/f.the maximum period you can set is 2^16.
    // Ƶ������
    PWMGenPeriodSet(PWM_MOTORX_BASE, PWM_GEN_MOTORX, PWM_PERIOD);

    // ���ó�ʼռ�ձ�:1/PWM_MAXVALUE
    PWMPulseWidthSet(PWM_MOTORX_BASE, PWM_OUT_MOTOR1,
    		(PWM_PERIOD*1)/PWM_MAXVALUE);
    PWMPulseWidthSet(PWM_MOTORX_BASE, PWM_OUT_MOTOR3,
    		(PWM_PERIOD*1)/PWM_MAXVALUE);

    // Enable the PWM output signal ʹ�����
    PWMOutputState(PWM_MOTORX_BASE, PWM_OUT_MOTOR1_BIT, true);
    PWMOutputState(PWM_MOTORX_BASE, PWM_OUT_MOTOR3_BIT, true);

    // Enable the PWM generator block ʹ��pwm������
    PWMGenEnable(PWM_MOTORX_BASE, PWM_GEN_MOTORX);

    //
    motor_DirInit();
}


//*****************************************************************************
//
// ���õ��1ռ�ձ� ��Χ : 1 - PWM_MAXVALUE
//
//*****************************************************************************
static void motor_SetMotor1(int pwm_value)
{
    if(pwm_value<0)
    {
        motor_Set1Dir(0);
    }
    else
        motor_Set1Dir(1);
//	pwm_value = pwm_value < 1 ? 1 : pwm_value;
    pwm_value = pwm_value < 1 ? fabs(pwm_value)+1 : pwm_value;
	pwm_value = pwm_value > PWM_MAXVALUE ? PWM_MAXVALUE : pwm_value;

	PWMPulseWidthSet(PWM_MOTORX_BASE, PWM_OUT_MOTOR1,
			(PWM_PERIOD*pwm_value)/PWM_MAXVALUE);
}

//*****************************************************************************
//
// ���õ��3ռ�ձ� ��Χ : 1 - PWM_MAXVALUE
//
//*****************************************************************************
static void motor_SetMotor3(int pwm_value)
{
    if(pwm_value<0)
    {
        motor_Set3Dir(0);
    }
    else
        motor_Set3Dir(1);
//	pwm_value = pwm_value < 1 ? 1 : pwm_value;
    pwm_value = pwm_value < 1 ? fabs(pwm_value)+1 : pwm_value;
	pwm_value = pwm_value > PWM_MAXVALUE ? PWM_MAXVALUE : pwm_value;

	PWMPulseWidthSet(PWM_MOTORX_BASE, PWM_OUT_MOTOR3,
			(PWM_PERIOD*pwm_value)/PWM_MAXVALUE);
}

//*****************************************************************************
//
// ����X�����˶� ��Χ : (-PWM_MAXVALUE) - (+PWM_MAXVALUE)
//
//*****************************************************************************
//static void motor_Update(uint16_t pwm_m1, uint16_t pwm_m3)
//{
//
//}


// ������ƽӿں���,���û�����
void motor_Control(uint8_t *uart_data)
{
	// Save the last pwm control value to avoid the unnecessary operation.
	static int32_t val1_last = 1, val3_last = 1;
	int32_t pwmval_forward, pwmval_turn;   // ֱ��ת��
	int32_t pwmval_m1, pwmval_m3;

	pwmval_forward = -((uart_data[4]<<8) + uart_data[5] - 50);
	pwmval_turn    =  ((uart_data[6]<<8) + uart_data[7] - 50);

	pwmval_forward = pwmval_forward > 50 ? 50 :(pwmval_forward < -50 ? -50 : pwmval_forward);
	pwmval_turn = pwmval_turn > 50 ? 50 :(pwmval_turn < -50 ? -50 : pwmval_turn);
	UARTprintf("motor: %3d   %3d\n", pwmval_forward, pwmval_turn);

//	// ����
//	pwmval_forward = pwmval_forward > 5 ? 5 (pwmval_forward < -5 ? -5 : pwmval_forward);
//	pwmval_turn = pwmval_turn > 5 ? 5 (pwmval_turn < -5 ? -5 : pwmval_turn);

	if(pwmval_forward < 0) {
		pwmval_forward = -pwmval_forward;
		motor_Set1Dir(1);
		motor_Set3Dir(0);
	} else {
		motor_Set1Dir(0);
		motor_Set3Dir(1);
	}

	pwmval_m1 = pwmval_forward*2 - pwmval_turn;
	pwmval_m3 = pwmval_forward*2 + pwmval_turn;

	pwmval_m1 = pwmval_m1 < 1 ? 1 :pwmval_m1;
	pwmval_m3 = pwmval_m3 < 1 ? 1 :pwmval_m3;

	if(val1_last != pwmval_m1) {
		motor_SetMotor1(pwmval_m1);
		val1_last = pwmval_m1;    // update the value.
	}

	if(val3_last != pwmval_m3) {
		motor_SetMotor3(pwmval_m3);
		val3_last = pwmval_m3;   // update the value.
	}
}

//*****************************************************************************
//
//! Init the gpio for control motor's direction.
//!
//! \param : none.
//!
//! \return: none.
//
//*****************************************************************************
static void motor_DirInit(void)
{
    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_MOTOR1DIR);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIO_MOTOR1DIR))
        ;

    // Enable the GPIO pin output.
    GPIOPinTypeGPIOOutput(GPIO_PORT_MOTOR1DIR_BASE, GPIO_PIN_MOTOR1DIR);


    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIO_MOTOR3DIR);

    // Check if the peripheral access is enabled.
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIO_MOTOR3DIR))
        ;

    // Enable the GPIO pin output.
    GPIOPinTypeGPIOOutput(GPIO_PORT_MOTOR3DIR_BASE, GPIO_PIN_MOTOR3DIR);


}

//*****************************************************************************
//
// ���õ��1��ת����.
//
// ui16CW 1��˳ʱ��  0����ʱ��.
//
//*****************************************************************************
static void motor_Set1Dir(uint16_t ui16CW)
{
	if(ui16CW == 1) {
		// Output 1
		GPIOPinWrite(GPIO_PORT_MOTOR1DIR_BASE, GPIO_PIN_MOTOR1DIR, GPIO_PIN_MOTOR1DIR);
	} else {
		// Output 0
		GPIOPinWrite(GPIO_PORT_MOTOR1DIR_BASE, GPIO_PIN_MOTOR1DIR, 0x0);
	}
}
//*****************************************************************************
//
// ���õ��3��ת����.
//
// ui16CW 1��˳ʱ��  0����ʱ��.
//
//*****************************************************************************
static void motor_Set3Dir(uint16_t ui16CW)
{
	if(ui16CW == 0) {
		// Output 1
		GPIOPinWrite(GPIO_PORT_MOTOR3DIR_BASE, GPIO_PIN_MOTOR3DIR, GPIO_PIN_MOTOR3DIR);
	} else {
		// Output 0
		GPIOPinWrite(GPIO_PORT_MOTOR3DIR_BASE, GPIO_PIN_MOTOR3DIR, 0x0);
}
}
void car_driver(void)
{
    static int v_target=65;
    // �����ת����Ĭ��1��˳ʱ��

        switch (command)
        {
        case 0x01:                                           //��������1
//              rt_event_send(order_event, ORDER_EVENT_1);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            motor_set_speed(v_target,v_target);
            break;
        case 0x02:                                           //��������2
//              rt_event_send(order_event, ORDER_EVENT_2);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
            motor_set_speed(-v_target,-v_target);
            break;
        case 0x03:                                           //��������3
//              rt_event_send(order_event, ORDER_EVENT_3);
            if(v_target>0)
            {
                motor_set_speed(v_target,v_target-3);

            }
            else
            {
                motor_set_speed(-v_target,-(v_target-3));

            }
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
            break;
        case 0x04:                                           //��������4
//              rt_event_send(order_event, ORDER_EVENT_4);

            if(v_target>0)
            {
                motor_set_speed(v_target-3,v_target);

            }
            else
            {
                motor_set_speed(-(v_target-3),-v_target);

            }
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
            break;
        case 0x05:                                          //��������4
           // motor_set_speed(v_target-5,v_target-5);
            v_target+=1;
            motor_set_speed(v_target,v_target);
            break;
        case 0x06:
            v_target-=1;
            motor_set_speed(v_target,v_target);
            //motor_set_speed(0,0);
            break;
        case 0x07:
            motor_set_speed(0,0);
            //v_target+=1;
            break;
        case 0x08:
            //v_target-=1;
            break;
        case 0x00:
            motor_set_speed(0,0);
            break;
        }
}
void motor_set_speed(int speed_L,int speed_R)
{
    motor_SetMotor1(speed_L);
    motor_SetMotor3(speed_R);
}












