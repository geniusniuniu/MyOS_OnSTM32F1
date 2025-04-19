#ifndef FANCONFIGFILE_H
#define FANCONFIGFILE_H

#include <stdlib.h>
#include "stm32f10x.h"                  // Device header
#include "tinyOS.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"
#include "usart.h"
#include "PWM.h"
#include "Servo.h"

#define SERVO_MID_ANGLE     1200        //舵机角度中值
#define SERVO_STEP_SIZE     100         //舵机步进大小
#define MOTOR_OFF_SPEED     0           //电机停转速度
#define MOTOR_NORM_SPEED    2600        //电机正常转速
#define MOTOR_STEP_SIZE     200         //电机步进大小                

#define TURN_ON             1           //风扇开启标志
#define TURN_OFF            0           //风扇关闭标志

//作为上位机测试接口开关，如需调试请取消注释
#define USE_USART2

#define SERIAL1_BAUDRATE    	115200
#ifdef  USE_USART2
    #define SERIAL2_BAUDRATE    9600
#endif


#define KEY_MSGCOUNT_MAX		10


#endif




