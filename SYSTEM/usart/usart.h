/*
 * @Author: your name
 * @Date: 2021-06-21 22:10:32
<<<<<<< HEAD
 * @LastEditTime: 2021-07-30 18:28:20
=======
 * @LastEditTime: 2021-07-30 18:20:40
>>>>>>> test
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \UCOS3_TEST\SYSTEM\usart\usart.h
 */
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//Mini STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.csom
//�޸�����:2011/6/14
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);


#define EN_USART3   1                           //usart3 ʹ�ܱ��
#define USART3_REC_LEN  			200  	    //����uart3�������ֽ�
extern u8  USART3_RX_BUF[USART3_REC_LEN];       //���ջ���
extern u16 USART3_RX_STA;         		        //����״̬���
void USART3_Init(u32 bound);                    //usart3��ʼ��
void USART3_printf(char* fmt,...);              //usart3��ӡ����

#endif


