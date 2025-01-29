#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0            PCout(13)   // PC13

#define DETECT_MODULE   PAin(3)     // PA3

#define GPIO_W_12(x)		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (BitAction)(x))



void LED_Init(void);        //初始化
void Detect_Init(void);     //人体检测模块初始化
void Motor_Dir_Init(BitAction Dir);      //电机方向引脚初始化

#endif
