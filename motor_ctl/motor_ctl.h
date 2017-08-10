/*
 * motor_ctl.h
 *
 *  Created on: 2017年7月7日
 *      Author: weycen
 */

#ifndef MOTOR_CTL_H_
#define MOTOR_CTL_H_

#define g_ui32SysClock        80000000

#define PWM_FREQUENCY         10000     //电机控制pwm频率10khz
#define PWM_PERIOD            (g_ui32SysClock / PWM_FREQUENCY)
#define PWM_MAXVALUE          100       // pwm最大值1000



//****宏定义****//
//gpio
#define SYSCTL_PERIPH_GPIO_MOTORX     SYSCTL_PERIPH_GPIOA

#define GPIO_PORT_MOTORX_BASE         GPIO_PORTA_BASE

#define GPIO_PIN_MOTOR1          	  GPIO_PIN_6
#define GPIO_PIN_MOTOR3          	  GPIO_PIN_7
#define GPIO_PINS_MOTORX          	 (GPIO_PIN_MOTOR1 | GPIO_PIN_MOTOR3)

//pwm
#define SYSCTL_PERIPH_PWM_MOTORX   SYSCTL_PERIPH_PWM1

#define PWM_MOTORX_BASE            PWM1_BASE

#define GPIO_PIN_MOTOR1_PWM        GPIO_PA6_M1PWM2
#define GPIO_PIN_MOTOR3_PWM        GPIO_PA7_M1PWM3

#define PWM_GEN_MOTORX             PWM_GEN_1

//pwm输出位 set select bit
#define PWM_OUT_MOTOR1             PWM_OUT_2
#define PWM_OUT_MOTOR3             PWM_OUT_3

// enable out bit
#define PWM_OUT_MOTOR1_BIT     	   PWM_OUT_2_BIT
#define PWM_OUT_MOTOR3_BIT    	   PWM_OUT_3_BIT


// motor1 dir
#define SYSCTL_PERIPH_GPIO_MOTOR1DIR     SYSCTL_PERIPH_GPIOE
#define GPIO_PORT_MOTOR1DIR_BASE         GPIO_PORTE_BASE
#define GPIO_PIN_MOTOR1DIR          	 GPIO_PIN_3
// motor3 dir
#define SYSCTL_PERIPH_GPIO_MOTOR3DIR     SYSCTL_PERIPH_GPIOF
#define GPIO_PORT_MOTOR3DIR_BASE         GPIO_PORTF_BASE
#define GPIO_PIN_MOTOR3DIR          	 GPIO_PIN_1

// extern function.
extern void motor_Init(void);
extern void motor_Control(uint8_t *uart_data);
extern void car_driver(uint8_t Command);
void motor_set_speed(int speed_L,int speed_R);





#endif /* MOTOR_CTL_H_ */
