/*
 * Description : Damage Detection System for Solar Panel
 * LCD16x2_4bit.h
 * Created: 4/3/2021 3:44:16 PM
 * Author: SIHAN
 */ 



#ifndef LCD16X2_4BIT_H_
#define LCD16X2_4BIT_H_

#include <avr/io.h>
#include <util/delay.h>

#define LCD_DPRT PORTD
#define LCD_DDDR DDRD
#define LCD_RS 0
#define LCD_EN 2
#define LCD_RW 1

void lcdcommand(unsigned char cmnd);
void lcddata(unsigned char data);
void lcdinit();
void lcd_gotoxy(unsigned char x, unsigned char y);
void lcd_print(char *str);
void lcd_clear();

#endif /* LCD16X2_4BIT_H_ */