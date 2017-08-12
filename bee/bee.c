#include "bee.h"

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

uint8_t bee_switch=0;
void bee_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
}
void  bee_on(void)
{
    bee_switch=1;
//    GPIOPinTypeGPIOOutput(GPIO_PIN_4, GPIO_PIN_4);
}
void  bee_off(void)
{
    bee_switch=0;
//    GPIOPinTypeGPIOOutput(GPIO_PIN_4, 0);
}
