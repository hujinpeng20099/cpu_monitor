#ifndef __PSU_THREAD_H
#define	__PSU_THREAD_H

#include "rtthread.h"
#include "main.h"
#include "lcd_12232.h"


#define ADC_CONV_TIMEOUT				200

#define THREAD_PRIORITY         12
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        10


extern UART_HandleTypeDef dbg_uart;
extern UART_HandleTypeDef pd_uart;
extern int8_t encoder_value;
extern uint8_t ec11_btn;


void Out_Value_Update(uint16_t* adc_buf);
void set_value_update(void);
void set_couser_pos(uint8_t cur_pos);



void F071_Peripheral_Init(void);
void PSU_Thread_Creat(void);
void MX_ADC_Init(void);
void MX_DAC_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);


#endif



