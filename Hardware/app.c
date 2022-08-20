#include "app.h"
#include "lcd_12232.h"
#include "string.h"

extern char uart_rxbuf[45];
extern uint8_t uart_rx_sta;

char gpu_temp[4],cpu_usage[4],cpu_frq[6],temp[4],ram_usage[4],ram_size[6],time[10];

void RM_Desk(void)
{
	LCD_DrawBmp16(0,0,gpu_bmp);
	LCD_DrawString(20,0,"GPU-T",8,1);
	LCD_DrawString(20,8,"   C",8,1);
	LCD_DrawBmp16(0,16,temp_bmp);
	LCD_DrawString(20,16,"CPU-T",8,1);
	LCD_DrawString(20,24,"   C",8,1);
	
	LCD_DrawBmp16(56,0,cpu_bmp);
	LCD_DrawString(74,0,"CPU    %",8,1);
	LCD_DrawString(74,8,"     MHz",8,1);
	LCD_DrawBmp16(56,16,ram_bmp);
	LCD_DrawString(74,16,"RAM    %",8,1);
	LCD_DrawString(74,24,"     MB",8,1);
	LL_GPIO_SetOutputPin(GPIOA, LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin 
												|LED4_Pin);
}

void RM_Desk_Refresh(void)
{

	strncpy(gpu_temp,uart_rxbuf+2,3);
	gpu_temp[3]='\0';
	
	strncpy(cpu_usage,uart_rxbuf+8,3);
	cpu_usage[3]='\0';
	
	strncpy(cpu_frq,uart_rxbuf+14,5);
	cpu_frq[5]='\0';
	
	strncpy(temp,uart_rxbuf+22,3);
	temp[3]='\0';
	
	strncpy(ram_usage,uart_rxbuf+28,3);
	ram_usage[3]='\0';
	
	strncpy(ram_size,uart_rxbuf+34,5);
	ram_size[5]='\0';	
	
	LCD_DrawString(20,8,gpu_temp,8,1);
	LCD_DrawString(98,0,cpu_usage,8,1);
	LCD_DrawString(74,8,cpu_frq,8,1);	
	LCD_DrawString(20,24,temp,8,1);
	LCD_DrawString(98,16,ram_usage,8,1);
	LCD_DrawString(74,24,ram_size,8,1);
	LED_Show();
}


void LED_Show(void)
{
	uint8_t value;
	if(cpu_usage[0]>'0'&&cpu_usage[1]>='0'&&cpu_usage[2]>='0')
		value=(cpu_usage[0]-'0')*100+(cpu_usage[1]-'0')*10+cpu_usage[2]-'0';
	else if(cpu_usage[0]>'0'&&cpu_usage[1]>='0'&&cpu_usage[2]<'0')
		value=(cpu_usage[0]-'0')*10+cpu_usage[1]-'0';
	else if(cpu_usage[0]>='0'&&cpu_usage[1]<'0'&&cpu_usage[2]<'0')
		value=cpu_usage[0]-'0';
		
	if(value<=20)
	{
		LL_GPIO_SetOutputPin(GPIOA,LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin);
		LL_GPIO_ResetOutputPin(GPIOA,LED4_Pin);
	}
	else if(value>20&&value<=40)
	{
		LL_GPIO_SetOutputPin(GPIOA,LED0_Pin|LED1_Pin|LED2_Pin);
		LL_GPIO_ResetOutputPin(GPIOA,LED3_Pin|LED4_Pin);	
	}
	else if(value>40&&value<=60)
	{
		LL_GPIO_SetOutputPin(GPIOA,LED0_Pin|LED1_Pin);
		LL_GPIO_ResetOutputPin(GPIOA,LED2_Pin|LED3_Pin|LED4_Pin);	
	}
	else if(value>60&&value<=80)
	{
		LL_GPIO_SetOutputPin(GPIOA,LED0_Pin);
		LL_GPIO_ResetOutputPin(GPIOA,LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin);	
	}
	else if(value>80&&value<=100)
	{
		LL_GPIO_ResetOutputPin(GPIOA,LED0_Pin|LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin);
	}

}


