#include "lcd_12232.h"
#include "font.h"  
#include "string.h"
//显存存放格式如下.
//[0]0 1 2 3 ... 122
//[1]0 1 2 3 ... 122	
//[2]0 1 2 3 ... 122	
//[3]0 1 2 3 ... 122	
	   
volatile static uint8_t LCD_Gram[122][4];


/*LCD Clear*/
void LCD_Clear(uint8_t data)
{
	uint8_t i=0,j=0;
	for(i=0;i<4;i++)  
	{  
		LCD_WriteCMD (0xB8+i,CHIP_L);    //设置页地址（0~4）
		LCD_WriteCMD (0x00,CHIP_L);      	
		for(j=0;j<61;j++)LCD_WriteRAM(data,CHIP_L);
		LCD_WriteCMD (0xB8+i,CHIP_R);    
		LCD_WriteCMD (0x00,CHIP_R);      
		for(j=0;j<61;j++)LCD_WriteRAM(data,CHIP_R);
	} 
}


//更新显存到LCD		 
void LCD_refreshGram(void)
{
	uint8_t i,j;		    
	for(i=0;i<4;i++)  
	{  
		LCD_WriteCMD (0xB8+i,CHIP_L);    
		LCD_WriteCMD (0x00,CHIP_L);      	
		for(j=0;j<61;j++)LCD_WriteRAM(LCD_Gram[j][i],CHIP_L);
		LCD_WriteCMD (0xB8+i,CHIP_R);    
		LCD_WriteCMD (0x00,CHIP_R);      
		for(j=61;j<122;j++)LCD_WriteRAM(LCD_Gram[j][i],CHIP_R);
	}   
} 
//画点 
//t:1 填充 0,清空				   
void LCD_DrawPoint(uint8_t x,uint8_t y,uint8_t t)
{
	uint8_t pos,bx,temp=0;
	if(x>121||y>31)return;//超出范围了.
	pos=y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)LCD_Gram[x][pos]|=temp;
	else LCD_Gram[x][pos]&=~temp;	    
}




/**
  * @brief Displays one character at the specified position    
**/

void LCD_DrawChar16(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode)
{
	uint8_t i, j, k, chr_p;
	uint8_t buf, ypri = y,xpri=x;
	chr_p = chr - ' ';	
	for(i=0;i<2;i++)
	{
		for(j=0;j<8;j++)
		{
			buf=font16[chr_p][i*8+j];	
			for(k=0;k<8;k++)
			{
				if(buf&0x80)LCD_DrawPoint(x,y,mode);
				else LCD_DrawPoint(x,y,!mode);
				buf<<=1;
				y++;	
			}	
			x++;
			y=ypri;				
		}
		x=xpri;
		ypri=ypri+8;
		y=ypri;
	}
}

/**
  * @brief Displays one character at the specified position    
**/

void LCD_DrawChar8(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode)
{
	uint8_t j, k, chr_p;
	uint8_t buf, ypri = y;
	chr_p = chr - ' ';	
	for(j=0;j<6;j++)
	{
		buf=font8[chr_p][j];	
		for(k=0;k<8;k++)
		{
			if(buf&0x80)LCD_DrawPoint(x,y,mode);
			else LCD_DrawPoint(x,y,!mode);
			buf<<=1;
			y++;	
		}	
		x++;
		y=ypri;				
	}
}

	
/**
  * @brief DrawString 
**/	
void LCD_DrawString(uint8_t x,uint8_t y, char *p,uint8_t csize,uint8_t mode)
{	
	while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
	{ 
		if(csize==8)
		{
			LCD_DrawChar8(x,y,*p,mode);	 
			x+=6;
			p++; 
		}
		else if(csize==16)
		{
			LCD_DrawChar16(x,y,*p,mode);	 
			x+=8;
			p++; 		
		}
	}
}


/**
  * @brief Draw Bar
**/
void LCD_DrawBar(uint8_t x,uint8_t y,uint8_t data,uint8_t mode)
{
	uint8_t i, j;
	uint8_t buf, ypri = y;
	for(i=0;i<4;i++)
	{
		buf=bar[0][i];	
		for(j=0;j<8;j++)
		{
			if(buf&0x80)LCD_DrawPoint(x,y,mode);
			else LCD_DrawPoint(x,y,!mode);
			buf<<=1;
			y++;	
		}	
		x++;
		y=ypri;	
	}
	if(data>100)data=100;
	data = data*0.8+4;
	for(i=4;i<data;i++)
	{
		buf=0x38;	
		for(j=0;j<8;j++)
		{
			if(buf&0x80)LCD_DrawPoint(x,y,mode);
			else LCD_DrawPoint(x,y,!mode);
			buf<<=1;
			y++;	
		}	
		x++;
		y=ypri;				
	}
	for(i=data;i<84;i++)
	{
		buf=0x10;	
		for(j=0;j<8;j++)
		{
			if(buf&0x80)LCD_DrawPoint(x,y,mode);
			else LCD_DrawPoint(x,y,!mode);
			buf<<=1;
			y++;	
		}	
		x++;
		y=ypri;				
	}	
	for(i=84;i<96;i++)
	{
		buf=bar[0][i];	
		for(j=0;j<8;j++)
		{
			if(buf&0x80)LCD_DrawPoint(x,y,mode);
			else LCD_DrawPoint(x,y,!mode);
			buf<<=1;
			y++;	
		}	
		x++;
		y=ypri;				
	}
}	

//m^n函数
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//f_w:字宽
//f_h:字高
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void LCD_DrawNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t mode)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_DrawChar8(x+6*t,y,' ',mode);
				continue;
			}
			else 
				enshow=1; 
		}
	 	LCD_DrawChar8(x+6*t,y,temp+'0',mode); 
	}
}

void LCD_DrawNum16(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t mode)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_DrawChar16(x+8*t,y,'0',mode);
				continue;
			}
			else 
				enshow=1; 
		}
	 	LCD_DrawChar16(x+8*t,y,temp+'0',mode); 
	}
}

void LCD_DrawBmp16(uint8_t x,uint8_t y,uint8_t *pic)
{
	uint8_t i,j,k;
	uint8_t x_pri=x,y_pri=y;
	for(k=0;k<2;k++)
	{
		for(j=0;j<16;j++)
		{
			uint8_t buf=pic[j+k*16];
			for(i=0;i<8;i++)
			{
				if(buf&0x80)LCD_DrawPoint(x,y,1);
				else LCD_DrawPoint(x,y,0);
				buf<<=1;
				y++;				
			}
			y=y_pri;
			x++;
		}
		x=x_pri;
		y_pri=y_pri+8;
		y=y_pri;
	}
}



void LCD_RST(void)
{
	LL_GPIO_ResetOutputPin(LCD_RST_GPIO_Port,LCD_RST_Pin);
	HAL_Delay(100);
	LL_GPIO_SetOutputPin(LCD_RST_GPIO_Port,LCD_RST_Pin);
	HAL_Delay(100);
}

/*send data*/
void LCD_WriteData(uint8_t data)
{
	uint32_t reg_data;
	reg_data=LL_GPIO_ReadOutputPort(GPIOB);
	LL_GPIO_WriteOutputPort(GPIOB,(reg_data&0xFF00)|data);
}

/*write command*/
void LCD_WriteCMD(uint8_t cmd,uint8_t cs)
{
	if(cs)
	{
		LL_GPIO_SetOutputPin(LCD_EN1_GPIO_Port,LCD_EN1_Pin);
		LL_GPIO_ResetOutputPin(LCD_EN2_GPIO_Port,LCD_EN2_Pin);
	}
	else 
	{
		LL_GPIO_ResetOutputPin(LCD_EN1_GPIO_Port,LCD_EN1_Pin);
		LL_GPIO_SetOutputPin(LCD_EN2_GPIO_Port,LCD_EN2_Pin);		
	}
//	Delay_us(5);
	LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port,LCD_DC_Pin);
	LL_GPIO_ResetOutputPin(LCD_RW_GPIO_Port,LCD_RW_Pin);
//	Delay_us(5);
	LCD_WriteData(cmd);
//	Delay_us(10);
	if(cs)LL_GPIO_ResetOutputPin(LCD_EN1_GPIO_Port,LCD_EN1_Pin);
	else LL_GPIO_ResetOutputPin(LCD_EN2_GPIO_Port,LCD_EN2_Pin);
	LL_GPIO_SetOutputPin(LCD_RW_GPIO_Port,LCD_RW_Pin);
}


/*write data to lcd ram*/
void LCD_WriteRAM(uint8_t data,uint8_t cs)
{
	if(cs)
	{
		LL_GPIO_SetOutputPin(LCD_EN1_GPIO_Port,LCD_EN1_Pin);
		LL_GPIO_ResetOutputPin(LCD_EN2_GPIO_Port,LCD_EN2_Pin);
	}
	else 
	{
		LL_GPIO_ResetOutputPin(LCD_EN1_GPIO_Port,LCD_EN1_Pin);
		LL_GPIO_SetOutputPin(LCD_EN2_GPIO_Port,LCD_EN2_Pin);		
	}
	Delay_us(10);
	LL_GPIO_SetOutputPin(LCD_DC_GPIO_Port,LCD_DC_Pin);
	LL_GPIO_ResetOutputPin(LCD_RW_GPIO_Port,LCD_RW_Pin);
	Delay_us(2);
	LCD_WriteData(data);
	Delay_us(10);
	if(cs)LL_GPIO_ResetOutputPin(LCD_EN1_GPIO_Port,LCD_EN1_Pin);
	else LL_GPIO_ResetOutputPin(LCD_EN2_GPIO_Port,LCD_EN2_Pin);
	Delay_us(10);
//	LL_GPIO_ResetOutputPin(LCD_DC_GPIO_Port,LCD_DC_Pin);
	LL_GPIO_SetOutputPin(LCD_RW_GPIO_Port,LCD_RW_Pin);
}





void LCD_Init(void)
{
	LCD_RST();
	
	LCD_WriteCMD(0xAE,CHIP_L);/*Display OFF*/	
	LCD_WriteCMD(0xA4,CHIP_L);/*Static Drive OFF*/	
	LCD_WriteCMD(0xE2,CHIP_L);/*Reset LCD*/
	LCD_WriteCMD(0xA9,CHIP_L);/*Static Duty=1/32*/	
	LCD_WriteCMD(0xA0,CHIP_L);/*Select ADC Colckwise forward*/
	LCD_WriteCMD(0xE0,CHIP_L);/*Read Modify Write*/
	LCD_WriteCMD(0xEE,CHIP_L);/*END*/	
	
	LCD_WriteCMD(0xAE,CHIP_R);/*Display OFF*/	
	LCD_WriteCMD(0xA4,CHIP_R);/*Static Drive OFF*/	
	LCD_WriteCMD(0xE2,CHIP_R);/*Reset LCD*/
	LCD_WriteCMD(0xA9,CHIP_R);/*Static Duty=1/32*/	
	LCD_WriteCMD(0xA0,CHIP_R);/*Select ADC Colckwise forward*/
	LCD_WriteCMD(0xE0,CHIP_R);/*Read Modify Write*/
	LCD_WriteCMD(0xEE,CHIP_R);/*END*/		

	LCD_WriteCMD(0xAF,CHIP_L);/*Display ON*/
	LCD_Clear(0x00);
	LCD_WriteCMD(0xAF,CHIP_R);/*Display ON*/
}


void Delay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				
	ticks=nus*48; 					
	told=SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;		
		}  
	};
}





