#ifndef __LCD_12232_H
#define __LCD_12232_H

#include "main.h"
#include "font.h"

#define CHIP_L	1
#define CHIP_R	0

#define FONT8		8
#define FONT16	16

void LCD_RST(void);
void LCD_Clear(uint8_t data);
void LCD_WriteData(uint8_t data);
void LCD_WriteCMD(uint8_t cmd,uint8_t cs);
void LCD_WriteRAM(uint8_t data,uint8_t cs);
void LCD_Init(void);
void Delay_us(uint32_t nus);
void LCD_refreshGram(void);

uint32_t mypow(uint8_t m,uint8_t n);
void LCD_DrawChar8(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);
void LCD_DrawString(uint8_t x,uint8_t y,char *p,uint8_t csize,uint8_t mode);
void LCD_DrawBar(uint8_t x,uint8_t y,uint8_t data,uint8_t mode);
void LCD_DrawNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t mode);
void LCD_DrawNum16(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t mode);
void LCD_DrawBmp16(uint8_t x,uint8_t y,uint8_t *pic);

#endif




