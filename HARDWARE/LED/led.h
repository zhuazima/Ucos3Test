/*
 * @Author: your name
 * @Date: 2021-04-29 22:35:44
 * @LastEditTime: 2021-05-18 20:52:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \��4-1 UCOSIII��ֲ\HARDWARE\LED\led.h
 */
#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PBout(0)// PB0
#define LED1 PBout(1)// PB1

void LED_Init(void);//��ʼ��

		 				    
#endif
