#ifndef __KEY_H
#define __KEY_H	 

#include "sys.h" 	 
#include "tinyOS.h"		

#define KEY0    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) //读取按键0
#define KEY1    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)  //读取按键1
#define KEY2    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)  //读取按键2
#define KEY3    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)  //读取按键3
#define KEY5    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)  //读取按键4 
#define KEY4    GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12) //读取按键2

 
#define KEY0_PRES 	     0	//KEY0按下
#define KEY1_PRES	     1	//KEY1按下
#define KEY2_PRES	     2	//KEY2按下
#define KEY3_PRES        3	//KEY3按下
#define KEY4_PRES        4	//KEY4按下
#define KEY5_PRES        5	//KEY5按下
#define KEY_NOT_PRESS    0xFF

#define     SHORT_KEY_DELAY     15		// 短按延时
#define     LONG_KEY_DELAY      200		// 长按延时
#define     DOUBLE_KEY_DELAY  	30		// 双击两次触发之间的最大延时
#define     CNTINUS_KEY_DELAY  	20		// 连续触发延时


typedef enum {
    KEY_NONE = 0x0A,
    KEY5_SHORT,
    KEY5_LONG,
    KEY5_CNTINUS,
    KEY5_DOUBLE,
}key_val_t;

// 按键扫描结构体
typedef struct 
{
    uint8_t scan_flag;          // 按键扫描标志,每10ms被定时中断置1,在key_scan函数中扫描按键之后清零
    uint32_t cnt;               // 按键按下计数器,单位:扫描周期(10ms);当键值按下时加1,按键弹起时清零
    uint32_t last_input;        // 上次输入
    uint8_t id;                 // 当前有效按键在key_info_t信息表中的ID
    uint8_t pressed;            // 按键按下生效标志位,非0表示持续按下时长已经达到短按延时
	uint8_t waitLongTimeout;// 长按按键超时
    uint8_t wait_double_flag;   // 有任意一个按键在等待双击时该标志=1,没有按键在等待双击时=0
    uint8_t wait_double;        // 等待双击标志位.若某按键支持双击,则第一次单击后,将该标志置1.
    uint16_t double_timeout;    // 等待双击超时,若某按键支持双击,则第一次单击后,将该值设为DOUBLE_KEY_DELAY.
                                // 每次扫描自减1,减到0的时刻为超时,若此时仍未等到第二次按下,则返回短按值.
                                // 返回短按值或双击键值时清零
}key_scan_t;

// 按键信息结构体
typedef struct {
    uint16_t hw_input_val;      // 当按键按下时，get_key_input()的返回值

    uint8_t short_key_val;      // 短按有效时,返回的键值,不支持则写KEY_NONE
    uint8_t long_key_val;       // 长按有效时,返回的键值,不支持则写KEY_NONE
    uint8_t cntinus_key_val;    // 连续按有效时,返回的键值,不支持则写KEY_NONE
    uint8_t double_key_val;     // 双击有效时,返回的键值,不支持则写KEY_NONE

    uint16_t long_cnt;          // 长按有效时间判定长度,单位:扫描周期(10ms)
    
}key_info_t;

extern key_scan_t timerKey;

void KEY_Init(void);    //IO初始化
uint8_t timerKeyScan(void);
uint8_t key_release_handle(void);
uint8_t GetKey_Value(uint8_t mode);  	    //按键扫描函数			

#endif
