//问题1:如果通过摄像头关闭风扇，那么按键无法再打开风扇，只能通过摄像头再次开启风扇，而后可以使用按键关闭
//问题2:语音模块引脚错位，需要更换或者使用新的引脚，飞线解决？




#include "stm32f10x.h"                  // Device header
#include "tinyOS.h"
#include "LED.h"
#include "OLED.h"
#include "Key.h"
#include "Timer.h"
#include "usart.h"
#include "PWM.h"
#include "Servo.h"

int Servo_compare = 1200,Motor_compare = 0;
uint8_t Key_Value = 0xff;
uint8_t detectFlag = 0xff;

int On_OFF = 0;
int Motor_compare_last = 0;

//创建消息邮箱
tMsgBox keyMsgBox;
void * keyMsgBoxBuffer[5];

//初始化应用层任务
void applicationTaskInit(void);
void hardWareInit(void); 

void OLED_ShowStatic(void);


//创建任务结构体
tTask oledShowTask;     //OLED显示任务
tTask keyDetectTask;    //按键检测任务
tTask serialParsing;    //串口解析任务

//创建任务的堆栈空间
tTaskStack oledShowTaskEnv[128];
tTaskStack keyDetectTaskEnv[128];
tTaskStack serialParsingEnv[128];


//创建空闲任务结构体和堆栈空间
tTask tTaskIdle;
tTaskStack taskIdleEnv[TINYOS_IDLETASK_STACK_SIZE];

//创建空闲任务
void taskIdleEntry(void * param)
{
	tTaskSchedLockEnable();	//使能调度锁
	
    SysTickInit(TINYOS_SYSTICK_MS);	
	
	#if TINYOS_ENABLE_SOFTTIMER == 1
		tTimerModuleInit();
	#endif
	
	applicationTaskInit();
	
	#if TINYOS_ENABLE_CPUSTATE == 1
		cpuUsagSyncWithSystick();	
	#else
		// 开启调度器，允许切换到其它任务
		tTaskSchedLockDisable();
	#endif
	
	for(;;)
	{
		uint32_t status = tTaskEnterCritical();
		{
			idleCount++;
		}
		tTaskExitCritical(status); 
		
		#if TINYOS_ENABLE_HOOKS == 1
			tHooksCpuIdle();
		#endif
	}
}

//创建应用任务
void oledShowTaskEntry(void * param)
{	
    for (;;) 
    {			
		OLED_ShowNum(56,0,On_OFF , 1, OLED_8X16);
		OLED_ShowNum(112,0,Key_Value , 3, OLED_8X16);
		OLED_ShowNum(64,16,Servo_compare , 4, OLED_8X16);
		OLED_ShowNum(64,32,Motor_compare , 4, OLED_8X16);
		OLED_ShowHexNum(112,32,detectFlag , 2, OLED_8X16);
				
		if(detectFlag == 1)
		{
			OLED_ShowString(0, 48, "DETECTED_PEOPLE", OLED_8X16);
		}
		else
		{
			OLED_ShowString(0, 48, "NO_PEOPLE_HERE ", OLED_8X16);
		}

		OLED_Update();
		tTaskDelay(10);
		
    }
}

void keyDetectTaskEntry(void* param)
{
    tMsgBoxInit(&keyMsgBox,(void *)keyMsgBoxBuffer,5);
    for (;;) 
    {	
        Key_Value = GetKey_Value(0);
        if(Key_Value != 255)
        {
			int tempKey_Value =  Key_Value;
			//将Key_Value的值作为消息发送出去
           tMsgNotify(&keyMsgBox,&tempKey_Value,tMSGNORMAL); 
        }
        if((Key_Value == KEY0_PRES) && (On_OFF == 0)) //按键0风扇启动
        {
            On_OFF = 1;
            Servo_compare = Servo_SetCompare(1200);//舵机归中
            Motor_compare = Motor_SetCompare(2600);//风扇初档位  
            Motor_compare_last = Motor_compare;
        }
        else if(Motor_compare > 0)
        {
            if(Key_Value == KEY1_PRES)//按键1每按一次风力加强
            {
                Motor_compare += 200;
                Motor_compare = Motor_SetCompare(Motor_compare); 
                Motor_compare_last = Motor_compare;
              
            }
            else if(Key_Value == KEY2_PRES)//按键2每按一次风力减弱
            {
                Motor_compare -= 200;
                Motor_compare = Motor_SetCompare(Motor_compare);
                Motor_compare_last = Motor_compare; 
            }
            else if(Key_Value == KEY3_PRES)//按键3舵机转向
            {
                Servo_compare += 100;
                Servo_compare = Servo_SetCompare(Servo_compare); 
            }
            else if(Key_Value == KEY4_PRES)//按键4舵机转向
            {
                Servo_compare -= 100;
                Servo_compare = Servo_SetCompare(Servo_compare); 
            }
            else if(On_OFF == 1 && Key_Value == KEY0_PRES ) //风扇转动时，按下按键0关闭风扇
            {
                On_OFF = 0;
                Motor_compare = Motor_SetCompare(0);    //关闭风扇
                Servo_compare = Servo_SetCompare(1200); //舵机归中 
                Motor_compare_last = Motor_compare;                
            }
        }
		tTaskDelay(5);
    }
}

//串口数据包解析任务
void serialParsingEntry(void* param)
{
	for (;;) 
    {	
//		void * msg = NULL;
//		uint32_t waitTicks = 5; // //超时等待50ms
//        uint32_t retVal = tMsgWait(&keyMsgBox,&msg,waitTicks);
//		if(retVal == tErrorCodeNone)
//		{
//			uint32_t value = *(uint32_t *)msg;	//将收到的消息强制转换为uint32_t*类型，并取值
//			printf("%d\r\n",value);
//		}		
		if (Serial1_RxFlag == 1)	//如果接收到数据包
		{		
			if (strcmp(Serial1_RxPacket, "People") == 0)				//如果收到有人指令
			{
				detectFlag = 1;
				Motor_compare = Motor_SetCompare(Motor_compare_last); 	//风扇应该保持之前档位  
			}
			else
			{
				detectFlag = 2;
				Motor_compare = Motor_SetCompare(0);    				//风扇停转  
			}	
			Serial1_RxFlag = 0;
		}
		tTaskDelay(10);
    }
}



//初始化应用层任务
void applicationTaskInit(void)
{
	tTaskInit(&oledShowTask,  oledShowTaskEntry,  (void *)0, 1, oledShowTaskEnv, sizeof(oledShowTaskEnv));
    tTaskInit(&keyDetectTask, keyDetectTaskEntry, (void *)0, 0, keyDetectTaskEnv,sizeof(keyDetectTaskEnv));
    tTaskInit(&serialParsing, serialParsingEntry, (void *)0, 1, serialParsingEnv,sizeof(serialParsingEnv));
}
//硬件初始化
void hardWareInit(void)    
{
    //中断向量分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    //指示灯初始化
    LED_Init();  
    
    //检测模块初始化
    Detect_Init();
    //按键初始化
    KEY_Init();
	
    //电机方向引脚初始化
    Motor_Dir_Init(Bit_RESET);
    //电机初始化
    Motor_PWM_Init(7200,10);
    
    //OLED初始化
    OLED_Init();
	OLED_ShowStatic();
	
    //串口初始化
	Serial1_Init(115200);
    Serial2_Init(9600);
	
    //舵机初始化
    Servo_Init();
    
    //定时器4初始化，10ms中断一次
    TIM4_Int_Init(7200,100);  	       
}

int main(void)
{
    hardWareInit();
	//任务调度列表初始化
	tTaskSchedInit();
	tListInit(&taskDelayList);
	#if TINYOS_ENABLE_CPUSTATE == 1
		initCpuUsageState();
	#endif
	tTaskInit(&tTaskIdle,taskIdleEntry,(void*)0,TINYOS_PIORITY_MAX-1,taskIdleEnv,TINYOS_IDLETASK_STACK_SIZE);
	idleTask = &tTaskIdle;

	nextTask = tTaskGetReady();		//找到处于就绪态的任务

	tTaskRunFirst();	//首次运行第一个任务
	/**************** 后面的代码执行不到 *****************/
	return 0;
}	

void OLED_ShowStatic(void)
{
    OLED_ShowString(0, 0, "ON/OFF", OLED_8X16);		//0-128行,0-64列，两种可选字体8X16和6X8.
    OLED_ShowString(80, 0,"Key:", OLED_8X16);
    OLED_ShowString(0, 16, "Servo:", OLED_8X16);   
    OLED_ShowString(0, 32, "Motor:", OLED_8X16); 
	
	OLED_ShowString(64,0,"0", OLED_8X16);
    OLED_ShowString(112,0,"255", OLED_8X16);
    OLED_ShowString(64,16,"1200", OLED_8X16);
    OLED_ShowString(64,32,"0000", OLED_8X16);
	
	OLED_Update();
}





