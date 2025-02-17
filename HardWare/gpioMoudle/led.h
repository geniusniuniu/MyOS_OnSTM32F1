#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0            PCout(13)   // PC13

#define DETECT_MODULE   PAin(3)     // PA3

#define GPIO_W_12(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(x))



void LED_Init(void);        
void Detect_Init(void);     
void Motor_Dir_Init(BitAction Dir);     

#endif
