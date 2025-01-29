#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 	 
#include "tinyOS.h"		

#define KEY0    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) //读取按键0
#define KEY1    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)  //读取按键1
#define KEY2    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)  //读取按键2 
#define KEY3    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)  //读取按键3
#define KEY4    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)  //读取按键4

 
 
#define KEY0_PRES 	     0	//KEY0按下
#define KEY1_PRES	     1	//KEY1按下
#define KEY2_PRES	     2	//KEY2按下
#define KEY3_PRES        3	//KEY3按下
#define KEY4_PRES        4	//KEY4按下
#define KEY_NOT_PRESS    0xff

int filter(int value);
void KEY_Init(void);    //IO初始化
uint8_t GetKey_Value(uint8_t mode);  	    //按键扫描函数			

#endif
