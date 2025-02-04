#ifndef __SERIAL_H
#define __SERIAL_H
#include <stdio.h>

extern char Serial1_RxPacket[];
extern uint8_t Serial1_RxFlag;

void Serial1_Init(uint32_t Baund);
void Serial2_Init(uint32_t Baund);
void Serial1_SendByte(uint8_t Byte);
void Serial2_SendByte(uint8_t Byte);


#endif
