//问题:语音模块引脚错位，需要更换或者使用新的引脚，飞线解决？
#include "fanConfigFile.h"

int Servo_compare = SERVO_MID_ANGLE;        //舵机占空比初始值
int Motor_compare = MOTOR_OFF_SPEED;        //电机占空比初始值
int Motor_compare_last = MOTOR_OFF_SPEED;   //电机占空比上一次值

//定义按键值
uint8_t Key_Value = 0xFF;

//定义各类标志位
uint8_t detectFlag = 0xFF;
int On_OFF = TURN_OFF;

//创建消息邮箱
tMsgBox keyMsgBox;
void * keyMsgBoxBuffer[KEY_MSGCOUNT_MAX];	

//初始化应用层任务
void applicationTaskInit(void);
void hardWareInit(void); 

//OLED显示静态内容
void OLED_ShowStatic(void)
{
    OLED_ShowString(0, 0, "ON/OFF", OLED_8X16);		//0-128行,0-64列，两种可选字体8X16和6X8.
    OLED_ShowString(72, 0,"Key:", OLED_8X16);
    OLED_ShowString(0, 16, "Servo:", OLED_8X16);   
    OLED_ShowString(0, 32, "Motor:", OLED_8X16); 
	
    OLED_ShowHexNum(112,0,0xFF, 2, OLED_8X16);
    OLED_ShowString(50,16,"1200", OLED_8X16);
    OLED_ShowString(50,32,"0000", OLED_8X16);
	
	OLED_Update();
}

/******************************** 任务相关变量初始化 ********************************/

//创建任务结构体
tTask oledShowTask;     //OLED显示任务
tTask keyDetectTask;    //按键检测任务
tTask serialParsing;    //串口解析任务

//创建任务的堆栈空间
tTaskStack oledShowTaskEnv[TINYOS_APPTASK_STACK_SIZE];
tTaskStack keyDetectTaskEnv[TINYOS_APPTASK_STACK_SIZE];
tTaskStack serialParsingEnv[TINYOS_APPTASK_STACK_SIZE];


//创建空闲任务结构体和堆栈空间
tTask tTaskIdle;
tTaskStack taskIdleEnv[TINYOS_IDLETASK_STACK_SIZE];

/******************************** 创建各类任务 ********************************/
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
		OLED_ShowHexNum(52,0,On_OFF , 2, OLED_8X16);
		OLED_ShowHexNum(104,0,retVal1 , 3, OLED_8X16);
		OLED_ShowNum(50,16,Servo_compare , 4, OLED_8X16);
		OLED_ShowNum(96,16,timeToCloseFan , 4, OLED_8X16);
		OLED_ShowNum(50,32,Motor_compare , 4, OLED_8X16);
		OLED_ShowHexNum(112,32,detectFlag , 2, OLED_8X16);
				
		if(detectFlag == 1)
		{
			OLED_ShowString(0, 48, "DETECTED_PEOPLE", OLED_8X16);
		}
		else
		{
			OLED_ShowString(0, 48, "NO_PEOPLE_HERE ", OLED_8X16); //NO_PEOPLE_HERE
		}

		OLED_Update();
		tTaskDelay(10);
		
    }
}

void keyDetectTaskEntry(void* param)
{
	//注意：msgCountMax的值不能大于数组keyMsgBoxBuffer下标的最大值
	//注意：msgCountMax的值不能大于数组keyMsgBoxBuffer下标的最大值
	//注意：msgCountMax的值不能大于数组keyMsgBoxBuffer下标的最大值
    tMsgBoxInit(&keyMsgBox,(void *)keyMsgBoxBuffer,KEY_MSGCOUNT_MAX);
    for (;;) 
    {	
        Key_Value = GetKey_Value(0);
        if(Key_Value != 0XFF)                                   //有按键按下
        {
		   int tempKey_Value =  Key_Value;
           tMsgNotify(&keyMsgBox,&tempKey_Value,tMsgNORMAL);    //将Key_Value的值作为消息发送出去
        }
        if((Key_Value == KEY0_PRES) && (On_OFF == TURN_OFF))    //按键0风扇启动
        {
            On_OFF = 1;
            Servo_compare = Servo_SetCompare(SERVO_MID_ANGLE);  //舵机归中
            Motor_compare = Motor_SetCompare(MOTOR_NORM_SPEED); //风扇初档位  
            Motor_compare_last = Motor_compare;
        }
        else if(Motor_compare > 0)
        {
            if(Key_Value == KEY1_PRES)                          //按键1每按一次风力加强
            {
                Motor_compare += MOTOR_STEP_SIZE;
                Motor_compare = Motor_SetCompare(Motor_compare); 
                Motor_compare_last = Motor_compare;
              
            }
            else if(Key_Value == KEY2_PRES)                     //按键2每按一次风力减弱
            {
                Motor_compare -= MOTOR_STEP_SIZE;
                Motor_compare = Motor_SetCompare(Motor_compare);
                Motor_compare_last = Motor_compare; 
            }
            else if(Key_Value == KEY3_PRES)                     //按键3舵机转向
            {
                Servo_compare += SERVO_STEP_SIZE;
                Servo_compare = Servo_SetCompare(Servo_compare); 
            }
            else if(Key_Value == KEY4_PRES)                     //按键4舵机转向
            {
                Servo_compare -= SERVO_STEP_SIZE;
                Servo_compare = Servo_SetCompare(Servo_compare); 
            }
            else if((On_OFF == TURN_ON && Key_Value == KEY0_PRES) || closeFanFlag == 1 )//风扇转动时，按下按键0关闭风扇
            {
                On_OFF = TURN_OFF;
                Motor_compare = Motor_SetCompare(MOTOR_OFF_SPEED); //关闭风扇
                Servo_compare = Servo_SetCompare(SERVO_MID_ANGLE); //舵机归中 
                Motor_compare_last = Motor_compare;                
            }
        }
		tTaskDelay(5);
    }
}

#ifdef  USE_USART2       
	void * msg = NULL;
 #endif
//串口数据包解析任务
void serialParsingEntry(void* param)
{
	for (;;) 
    {	
		#ifdef  USE_USART2 
            uint32_t waitTicks = 2; // //超时等待50ms
            uint32_t retVal = tMsgWait(&keyMsgBox,&msg,waitTicks);
            if(retVal == tErrorCodeNone)
            {
                uint32_t value = *(uint32_t *)msg;	//将收到的消息强制转换为uint32_t*类型，并取值
                printf("%d\r\n",value);
            }
        #endif
		if (Serial1_RxFlag == 1)	//如果接收到数据包
		{		
			if(strcmp(Serial1_RxPacket, "People") == 0)				   //如果收到有人指令
			{
				detectFlag = 1;
				On_OFF = TURN_ON;
				Motor_compare = Motor_SetCompare(Motor_compare_last); 	//风扇应该保持之前档位
			}
			else if(strcmp(Serial1_RxPacket, "None") == 0)
			{
				detectFlag = 2;
				On_OFF = TURN_OFF;  
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

/******************************** 硬件初始化 ********************************/
void hardWareInit(void)    
{
    //中断向量分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
    LED_Init();                     //程序运行状态指示灯初始化   
    KEY_Init();	                    //按键初始化
    Motor_Dir_Init(Bit_RESET);      //电机方向I/O初始化
    Motor_PWM_Init(7200,10);        //电机PWM初始化
    OLED_Init();                    //OLED初始化
	OLED_ShowStatic();	            //OLED显示静态内容
	Serial1_Init(SERIAL1_BAUDRATE); //串口1初始化,BAUDRATE = 115200
#ifdef  USE_USART2  
    Serial2_Init(SERIAL2_BAUDRATE);	//串口2初始化,BAUDRATE = 9600
#endif
    Servo_Init();                   //舵机初始化
    TIM4_Int_Init(7200,100);        //定时器4初始化，10ms中断一次	       
}

int main(void)
{
    hardWareInit();             //硬件初始化
	tTaskSchedInit();           //初始化任务调度器
	tListInit(&taskDelayList);  //初始化延时队列
	#if TINYOS_ENABLE_CPUSTATE == 1
		initCpuUsageState();
	#endif
	tTaskInit(&tTaskIdle,taskIdleEntry,(void*)0,TINYOS_PIORITY_MAX-1,taskIdleEnv,TINYOS_IDLETASK_STACK_SIZE);
	idleTask = &tTaskIdle;

	nextTask = tTaskGetReady();	//找到处于就绪态的任务

	tTaskRunFirst();	        //首次运行第一个任务
	/******************** 后面的代码执行不到 *********************/
	return 0;
}	






