/*
 * radio.h
 *
 *  Created on: 2016Äê7ÔÂ12ÈÕ
 *      Author: weycen
 */
#ifndef RADIO_H_
#define RADIO_H_
#include <stdint.h>

extern volatile bool  data_complete_flag;
extern uint8_t  receive_buffer[8];
extern uint8_t  command;
extern uint8_t command_end;
//extern uint16_t  command_last;
extern uint16_t  command_number;
//uint8_t  receive_buffer[8]  = {0};
//uint8_t  command=0;
//uint16_t command_number=1;
//uint8_t command_last=1;
extern void radio_Init(uint32_t baud1,uint32_t baud2);

#endif /* RADIO_H_ */
