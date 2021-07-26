#include "led.h"
#include "key.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "includes.h"
#include "timer.h"
#include "i2c.h"
#include "oled0561.h"
#include "lm75a.h"


/************************************************
 ALIENTEK战舰STM32开发板UCOS实验
 例4-1 UCOSIII UCOSIII移植
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

//UCOSIII中以下优先级用户程序不能使用，ALIENTEK
//将这些优先级分配给了UCOSIII的5个系统内部任务
//优先级0：中断服务服务管理任务 OS_IntQTask()
//优先级1：时钟节拍任务 OS_TickTask()
//优先级2：定时任务 OS_TmrTask()
//优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
//优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


/********************************************************************************
							START 任务的相关宏定义
********************************************************************************/
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);



/********************************************************************************
							OLED 任务的相关宏定义
********************************************************************************/
//任务优先级
#define	OLED_TASK_PRIO		5
//任务堆栈大小	
#define OLED_STK_SIZE 		128
//任务控制块
OS_TCB OledTaskTCB;
//任务堆栈	
CPU_STK OLED_TASK_STK[OLED_STK_SIZE];
void oled_task(void *p_arg);


/********************************************************************************
							LED0 任务的相关宏定义
********************************************************************************/
//任务优先级
#define LED0_TASK_PRIO		6
//任务堆栈大小	
#define LED0_STK_SIZE 		128
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);



/********************************************************************************
							LED1 任务的相关宏定义
********************************************************************************/
//任务优先级
#define LED1_TASK_PRIO		5
//任务堆栈大小	
#define LED1_STK_SIZE 		128
//任务控制块
OS_TCB Led1TaskTCB;
//任务堆栈	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *p_arg);


/********************************************************************************
							FLOAT 任务的相关宏定义
********************************************************************************/
//任务优先级
#define FLOAT_TASK_PRIO		6
//任务堆栈大小
#define FLOAT_STK_SIZE		128
//任务控制块
OS_TCB	FloatTaskTCB;
//任务堆栈
__align(8) CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
//任务函数
void float_task(void *p_arg);


/********************************************************************************
							ADC 任务的相关宏定义
********************************************************************************/
//任务优先级
#define ADC_TASK_PRIO		7
//任务堆栈大小
#define ADC_STK_SIZE		128
//任务控制块
OS_TCB	AdcTaskTCB;
//任务堆栈
__align(8) CPU_STK	ADC_TASK_STK[ADC_STK_SIZE];
//任务函数
void adc_task(void *p_arg);


/********************************************************************************
							KEY 任务的相关宏定义
********************************************************************************/
//任务优先级
#define KEY_TASK_PRIO		8
//任务堆栈大小
#define KEY_STK_SIZE		128
//任务控制块
OS_TCB	KeyTaskTCB;
//任务堆栈
__align(8) CPU_STK	KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *p_arg);

/********************************************************************************
							PWM 任务的相关宏定义
********************************************************************************/
//任务优先级
#define PWM_TASK_PRIO		9
//任务堆栈大小
#define PWM_STK_SIZE		128
//任务控制块
OS_TCB	PwmTaskTCB;
//任务堆栈
__align(8) CPU_STK	PWM_TASK_STK[PWM_STK_SIZE];
//任务函数
void pwm_task(void *p_arg);


/*********************************************************************************/

/*********************************************************************************/




int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();       //延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断优先级分组
	uart_init(115200);    //串口波特率设置
	LED_Init();         //LED初始化
	KEY_Init();			//key init
	Adc_Init();
	TIM3_PWM_Init(2200,0); // 不分频 ,PWM 频率 =72000/900=80KHz
		I2C_Configuration();//I2C初始化
		OLED0561_Init(); //OLED0516初始化
		OLED_DISPLAY_LIT(100);//亮度





	
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}



/********************************************************************************
							开始任务
********************************************************************************/
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区

	//创建LED0任务
	// OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
	// 			 (CPU_CHAR	* )"led0 task", 		
    //              (OS_TASK_PTR )led0_task, 			
    //              (void		* )0,					
    //              (OS_PRIO	  )LED0_TASK_PRIO,     
    //              (CPU_STK   * )&LED0_TASK_STK[0],	
    //              (CPU_STK_SIZE)LED0_STK_SIZE/10,	
    //              (CPU_STK_SIZE)LED0_STK_SIZE,		
    //              (OS_MSG_QTY  )0,					
    //              (OS_TICK	  )0,					
    //              (void   	* )0,					
    //              (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
    //              (OS_ERR 	* )&err);				
				 
	//创建LED1任务
	OSTaskCreate((OS_TCB 	* )&Led1TaskTCB,		
				 (CPU_CHAR	* )"led1 task", 		
                 (OS_TASK_PTR )led1_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED1_TASK_PRIO,     	
                 (CPU_STK   * )&LED1_TASK_STK[0],	
                 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED1_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	
			 
	//创建浮点测试任务
	OSTaskCreate((OS_TCB 	* )&FloatTaskTCB,		
				 (CPU_CHAR	* )"float test task", 		
                 (OS_TASK_PTR )float_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )FLOAT_TASK_PRIO,     	
                 (CPU_STK   * )&FLOAT_TASK_STK[0],	
                 (CPU_STK_SIZE)FLOAT_STK_SIZE/10,	
                 (CPU_STK_SIZE)FLOAT_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);								 

	// 创建ADC测试任务
	OSTaskCreate((OS_TCB 	* )&AdcTaskTCB,		
				 (CPU_CHAR	* )"adc test task", 		
                 (OS_TASK_PTR )adc_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ADC_TASK_PRIO,     	
                 (CPU_STK   * )&ADC_TASK_STK[0],	
                 (CPU_STK_SIZE)ADC_STK_SIZE/10,	
                 (CPU_STK_SIZE)ADC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	

	//创建KEY测试任务
	OSTaskCreate((OS_TCB 	* )&KeyTaskTCB,		
				 (CPU_CHAR	* )"key test task", 		
                 (OS_TASK_PTR )key_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )KEY_TASK_PRIO,     	
                 (CPU_STK   * )&KEY_TASK_STK[0],	
                 (CPU_STK_SIZE)KEY_STK_SIZE/10,	
                 (CPU_STK_SIZE)KEY_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	

	//创建PWM测试任务
	OSTaskCreate((OS_TCB 	* )&PwmTaskTCB,		
				 (CPU_CHAR	* )"pwm test task", 		
                 (OS_TASK_PTR )pwm_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )PWM_TASK_PRIO,     	
                 (CPU_STK   * )&PWM_TASK_STK[0],	
                 (CPU_STK_SIZE)PWM_STK_SIZE/10,	
                 (CPU_STK_SIZE)PWM_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	


	//创建OLED测试任务
	OSTaskCreate((OS_TCB 	* )&OledTaskTCB,		
				 (CPU_CHAR	* )"oled test task", 		
                 (OS_TASK_PTR )oled_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )OLED_TASK_PRIO,     	
                 (CPU_STK   * )&OLED_TASK_STK[0],	
                 (CPU_STK_SIZE)OLED_STK_SIZE/10,	
                 (CPU_STK_SIZE)OLED_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);	




	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}

/********************************************************************************
							PWM 任务
********************************************************************************/
void oled_task(void *p_arg)
{
	u8 buffer[3];

	OLED_DISPLAY_PIC1();//显示图片
	delay_ms(5000); //延时
	OLED_DISPLAY_CLEAR();
	OLED_DISPLAY_8x16_BUFFER(0,"   YoungTalk ");
	OLED_DISPLAY_8x16_BUFFER(6,"  Temp:");

	OLED_DISPLAY_16x16(2,2*16,0);
	OLED_DISPLAY_16x16(2,3*16,1);
	OLED_DISPLAY_16x16(2,4*16,2);
	OLED_DISPLAY_16x16(2,5*16,3);

	while(1){
		LM75A_GetTemp(buffer); //温度值
			
		if(buffer[0])OLED_DISPLAY_8x16(6,7*8,'-'); //
		OLED_DISPLAY_8x16(6,8*8,buffer[1]/10+0x30);//
		OLED_DISPLAY_8x16(6,9*8,buffer[1]%10+0x30);//
		OLED_DISPLAY_8x16(6,10*8,'.');//
		OLED_DISPLAY_8x16(6,11*8,buffer[2]/10+0x30);//
		OLED_DISPLAY_8x16(6,12*8 ,buffer[2]%10+0x30);//
		OLED_DISPLAY_8x16(6,13*8,'C');//

		delay_ms(200);
	}

}



/********************************************************************************
								led0任务函数
********************************************************************************/
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED0 = 0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED0 = 1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms		 

	}
}
/********************************************************************************
								led1任务函数
********************************************************************************/
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms		 

	}
}
/********************************************************************************
							浮点测试任务
********************************************************************************/

void float_task(void *p_arg)
{
	static float float_num = 0.00;
	OS_ERR err;

	p_arg = p_arg;
	
	while(1)
	{
		float_num += 0.01f;
		printf("float_num的值为: %.4f\r\n",float_num);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms

	}
}


/********************************************************************************
							ADC 任务
********************************************************************************/
void adc_task(void *p_arg)
{
	OS_ERR err;
	u16 adcx;

	p_arg = p_arg;

	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_2,10);
		printf("测试ADC的值是: %x\r\n",adcx);

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}




/********************************************************************************
							KEY 任务
********************************************************************************/
void key_task(void *p_arg)
{
	u8  key = 0;
	
	OS_ERR err;
	err = err;
	p_arg = p_arg;

	// OS_TaskSuspend((OS_TCB  *)&Led0TaskTCB,&err);
	// OS_TaskSuspend((OS_TCB  *)&Led1TaskTCB,&err);


	while(1)
	{
		key = KEY_Scan(0);	
	   	if(key)
		{						   
			switch(key)
			{				 
				case KEY1_PRES:		 
					LED1 = !LED1;
					break;
				case KEY0_PRES:	
					LED0 = !LED0;
					break;
				default:
					break;
			}
		}else delay_ms(10); 
	}	 
}



/********************************************************************************
							PWM 任务
********************************************************************************/
void pwm_task(void *p_arg)
{


 	u16 led0pwmval=0;
	u8 dir=1;	

	OS_ERR err;
	err = err;
	p_arg = p_arg;


	while(1)
	{
		delay_ms(10);	
		 
		if(dir)
		{
			led0pwmval += 40;
		}
		else 
		{
			led0pwmval -= 40;
		}

 		if(led0pwmval > 2100)	dir=0;
		if(led0pwmval == 0)		dir=1;		

		TIM_SetCompare3(TIM3,led0pwmval);
	}	 
}







