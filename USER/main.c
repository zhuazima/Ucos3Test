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
 ALIENTEKս��STM32������UCOSʵ��
 ��4-1 UCOSIII UCOSIII��ֲ
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//UCOSIII���������ȼ��û�������ʹ�ã�ALIENTEK
//����Щ���ȼ��������UCOSIII��5��ϵͳ�ڲ�����
//���ȼ�0���жϷ������������� OS_IntQTask()
//���ȼ�1��ʱ�ӽ������� OS_TickTask()
//���ȼ�2����ʱ���� OS_TmrTask()
//���ȼ�OS_CFG_PRIO_MAX-2��ͳ������ OS_StatTask()
//���ȼ�OS_CFG_PRIO_MAX-1���������� OS_IdleTask()
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


/********************************************************************************
							START �������غ궨��
********************************************************************************/
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);



/********************************************************************************
							OLED �������غ궨��
********************************************************************************/
//�������ȼ�
#define	OLED_TASK_PRIO		5
//�����ջ��С	
#define OLED_STK_SIZE 		128
//������ƿ�
OS_TCB OledTaskTCB;
//�����ջ	
CPU_STK OLED_TASK_STK[OLED_STK_SIZE];
void oled_task(void *p_arg);


/********************************************************************************
							LED0 �������غ궨��
********************************************************************************/
//�������ȼ�
#define LED0_TASK_PRIO		6
//�����ջ��С	
#define LED0_STK_SIZE 		128
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);



/********************************************************************************
							LED1 �������غ궨��
********************************************************************************/
//�������ȼ�
#define LED1_TASK_PRIO		5
//�����ջ��С	
#define LED1_STK_SIZE 		128
//������ƿ�
OS_TCB Led1TaskTCB;
//�����ջ	
CPU_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *p_arg);


/********************************************************************************
							FLOAT �������غ궨��
********************************************************************************/
//�������ȼ�
#define FLOAT_TASK_PRIO		6
//�����ջ��С
#define FLOAT_STK_SIZE		128
//������ƿ�
OS_TCB	FloatTaskTCB;
//�����ջ
__align(8) CPU_STK	FLOAT_TASK_STK[FLOAT_STK_SIZE];
//������
void float_task(void *p_arg);


/********************************************************************************
							ADC �������غ궨��
********************************************************************************/
//�������ȼ�
#define ADC_TASK_PRIO		7
//�����ջ��С
#define ADC_STK_SIZE		128
//������ƿ�
OS_TCB	AdcTaskTCB;
//�����ջ
__align(8) CPU_STK	ADC_TASK_STK[ADC_STK_SIZE];
//������
void adc_task(void *p_arg);


/********************************************************************************
							KEY �������غ궨��
********************************************************************************/
//�������ȼ�
#define KEY_TASK_PRIO		8
//�����ջ��С
#define KEY_STK_SIZE		128
//������ƿ�
OS_TCB	KeyTaskTCB;
//�����ջ
__align(8) CPU_STK	KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *p_arg);

/********************************************************************************
							PWM �������غ궨��
********************************************************************************/
//�������ȼ�
#define PWM_TASK_PRIO		9
//�����ջ��С
#define PWM_STK_SIZE		128
//������ƿ�
OS_TCB	PwmTaskTCB;
//�����ջ
__align(8) CPU_STK	PWM_TASK_STK[PWM_STK_SIZE];
//������
void pwm_task(void *p_arg);


/*********************************************************************************/

/*********************************************************************************/




int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	delay_init();       //��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�ж����ȼ�����
	uart_init(115200);    //���ڲ���������
	LED_Init();         //LED��ʼ��
	KEY_Init();			//key init
	Adc_Init();
	TIM3_PWM_Init(2200,0); // ����Ƶ ,PWM Ƶ�� =72000/900=80KHz
		I2C_Configuration();//I2C��ʼ��
		OLED0561_Init(); //OLED0516��ʼ��
		OLED_DISPLAY_LIT(100);//����





	
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}



/********************************************************************************
							��ʼ����
********************************************************************************/
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif
	
#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���

	//����LED0����
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
				 
	//����LED1����
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
			 
	//���������������
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

	// ����ADC��������
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

	//����KEY��������
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

	//����PWM��������
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


	//����OLED��������
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




	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�˳��ٽ���
}

/********************************************************************************
							PWM ����
********************************************************************************/
void oled_task(void *p_arg)
{
	u8 buffer[3];

	OLED_DISPLAY_PIC1();//��ʾͼƬ
	delay_ms(5000); //��ʱ
	OLED_DISPLAY_CLEAR();
	OLED_DISPLAY_8x16_BUFFER(0,"   YoungTalk ");
	OLED_DISPLAY_8x16_BUFFER(6,"  Temp:");

	OLED_DISPLAY_16x16(2,2*16,0);
	OLED_DISPLAY_16x16(2,3*16,1);
	OLED_DISPLAY_16x16(2,4*16,2);
	OLED_DISPLAY_16x16(2,5*16,3);

	while(1){
		LM75A_GetTemp(buffer); //�¶�ֵ
			
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
								led0������
********************************************************************************/
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED0 = 0;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		LED0 = 1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms		 

	}
}
/********************************************************************************
								led1������
********************************************************************************/
void led1_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		LED1 = ~LED1;
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms		 

	}
}
/********************************************************************************
							�����������
********************************************************************************/

void float_task(void *p_arg)
{
	static float float_num = 0.00;
	OS_ERR err;

	p_arg = p_arg;
	
	while(1)
	{
		float_num += 0.01f;
		printf("float_num��ֵΪ: %.4f\r\n",float_num);
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms

	}
}


/********************************************************************************
							ADC ����
********************************************************************************/
void adc_task(void *p_arg)
{
	OS_ERR err;
	u16 adcx;

	p_arg = p_arg;

	while(1)
	{
		adcx=Get_Adc_Average(ADC_Channel_2,10);
		printf("����ADC��ֵ��: %x\r\n",adcx);

		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
	}
}




/********************************************************************************
							KEY ����
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
							PWM ����
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







