#ifndef __PWM_H
#define __PWM_H
#include "sys.h"

#define MOTOR_IO1   PCout(15) //PC15 ���������һ������Ϊ0����һ�����PWM����PA0��

#define SPEED_MAX   4400
#define SPEED_DISTRICT(value) ((value) > (SPEED_MAX) ? (SPEED_MAX) : (value))

void Motor_PWM_Init(int Period,int Prescaler);
uint16_t Motor_SetCompare(uint16_t Compare);
void Motor_SetPrescale(uint16_t Prescale);

#endif
