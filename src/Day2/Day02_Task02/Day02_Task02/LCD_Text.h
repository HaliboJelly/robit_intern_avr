/*==============================================================================
 *
 *   CLCD (I2C 버전)
 *      LCD Driver      : KS0066U / HD44780 호환
 *      I2C Expander    : PCF8574  (Slave Address 0x27)
 *      File Name       : LCD_Text.h
 *      MPU_Type        : ATmega128 (16MHz)
 *
 *   기존 병렬(PORTC) 버전과 함수 이름/인자를 동일하게 맞췄기 때문에
 *   main.c는 고칠 필요 없이 이 파일들만 교체하면 된다.
 *
==============================================================================*/

#ifndef __Text_Lcd_H
#define __Text_Lcd_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "i2c.h"

//------------------------------------------------------------------------------
//  I2C 슬레이브 주소
//  PCF8574  -> 0x27 , PCF8574A -> 0x3F 인 경우가 많다.
//  TWI에는 7비트 주소를 왼쪽으로 1칸 민 값을 넣어야 한다. (0x27 << 1 = 0x4E)
//------------------------------------------------------------------------------
#define LCD_I2C_ADDR    (0x27 << 1)

//------------------------------------------------------------------------------
//  PCF8574 출력 핀 <-> LCD 핀 연결 (I2C 백팩 모듈 표준 배선)
//  Bit      7    6    5    4     3    2    1    0
//  LCD Pin  DB7  DB6  DB5  DB4   BL   E    R/W  RS
//------------------------------------------------------------------------------
#define LCD_RS          0x01    // 0 = 명령, 1 = 데이터
#define LCD_RW          0x02    // 항상 0 (쓰기 전용)
#define LCD_EN          0x04    // Enable
#define LCD_BL          0x08    // Backlight

// Text Lcd 명령어 셋팅
#define FUNCSET     0x28    // Function set     0010 1000 (4bit, 2line, 5x8)
#define ENTMODE     0x06    // Entry Mode Set   0000 0110
#define ALLCLR      0x01    // All Clear        0000 0001
#define DISPOFF     0x08    // Display Off      0000 1000
#define DISPON      0x0C    // Display On       0000 1100
#define LINE1       0x80    // 1st line Move    1000 0000
#define LINE2       0xC0    // 2nd line Move    1100 0000

//------------------------------------------------------------------------------
//                       ===== Funtion Prototype =====
//------------------------------------------------------------------------------
void lcdInit(void);
void lcdClear(void);
void lcdString(U8 line, U8 col, char *str);
void lcdNumber(U8 line, U8 col, int num);
void lcdBacklight(U8 on);                   // 1 = 켜기, 0 = 끄기

void lcdCommand(U8 byte);
void lcdData(U8 byte);
void lcdDisplayPosition(U8 line, U8 col);

#endif
