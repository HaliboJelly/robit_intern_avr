#include "LCD_Text.h"

static U8 lcdBackState = LCD_BL;    // 백라이트 상태 (기본 ON)

//------------------------------------------------------------------------------
// 4비트(상위 니블) 1회 전송 - 초기화 과정에서만 사용
//   [데이터 니블 + 제어비트 + EN=1] -> [데이터 니블 + 제어비트 + EN=0]
//   I2C 1바이트 전송이 100kHz에서 약 90us라서 E 펄스 폭은 자동으로 충분하다.
//------------------------------------------------------------------------------
static void lcdSendNibble(U8 nibble, U8 rs)
{
    U8 buf[2];

    buf[0] = (nibble & 0xF0) | rs | lcdBackState | LCD_EN;
    buf[1] = (nibble & 0xF0) | rs | lcdBackState;

    i2c_transmit_nbytes(LCD_I2C_ADDR, buf, 2);
}

//------------------------------------------------------------------------------
// 8비트 1바이트를 상위/하위 니블로 쪼개서 총 4바이트로 전송
//------------------------------------------------------------------------------
static void lcdSendByte(U8 byte, U8 rs)
{
    U8 buf[4];
    U8 hi = byte & 0xF0;
    U8 lo = (byte << 4) & 0xF0;

    buf[0] = hi | rs | lcdBackState | LCD_EN;   // 상위 니블 래치
    buf[1] = hi | rs | lcdBackState;
    buf[2] = lo | rs | lcdBackState | LCD_EN;   // 하위 니블 래치
    buf[3] = lo | rs | lcdBackState;

    i2c_transmit_nbytes(LCD_I2C_ADDR, buf, 4);
}

//------------------------------------------------------------------------------
// LCD 초기화 함수 (HD44780 4비트 모드 초기화 절차)
//------------------------------------------------------------------------------
void lcdInit(void)
{
    i2c_init();
    _delay_ms(50);              // 전원 안정화 대기

    lcdSendNibble(0x30, 0);  _delay_ms(5);      // 8비트 모드 요청 1
    lcdSendNibble(0x30, 0);  _delay_us(150);    // 8비트 모드 요청 2
    lcdSendNibble(0x30, 0);  _delay_us(150);    // 8비트 모드 요청 3
    lcdSendNibble(0x20, 0);  _delay_us(150);    // 여기서 4비트 모드로 전환

    lcdCommand(FUNCSET);        // 4bit, 2line, 5x8 font
    lcdCommand(DISPOFF);        // display off
    lcdCommand(ALLCLR);         // clear display
    lcdCommand(ENTMODE);        // entry mode set
    lcdCommand(DISPON);         // display on, cursor off

    lcdString(0, 0, "Init OK");
}

//------------------------------------------------------------------------------
// LCD 지우기 함수
//------------------------------------------------------------------------------
void lcdClear(void)
{
    lcdCommand(ALLCLR);
}

//------------------------------------------------------------------------------
// 문자열 출력 함수
//------------------------------------------------------------------------------
void lcdString(U8 line, U8 col, char *str)
{
    char *pStr = str;

    lcdDisplayPosition(line, col);
    while (*pStr)
    {
        lcdData(*pStr++);
    }
}

//------------------------------------------------------------------------------
// 숫자 출력 함수
//------------------------------------------------------------------------------
void lcdNumber(U8 line, U8 col, int num)
{
    char buf[12] = {0, };

    sprintf(buf, "%d", num);
    lcdString(line, col, buf);
}

//------------------------------------------------------------------------------
// 백라이트 제어
//------------------------------------------------------------------------------
void lcdBacklight(U8 on)
{
    U8 data;

    lcdBackState = on ? LCD_BL : 0x00;

    data = lcdBackState;                        // 더미 쓰기로 즉시 반영
    i2c_transmit_nbytes(LCD_I2C_ADDR, &data, 1);
}

//------------------------------------------------------------------------------
// 명령 함수 (RS = 0)
//------------------------------------------------------------------------------
void lcdCommand(U8 byte)
{
    lcdSendByte(byte, 0x00);

    // Clear / Return Home 은 1.52ms 이상 필요, 나머지는 37us
    if (byte == 0x01 || byte == 0x02)   _delay_ms(2);
    else                                _delay_us(50);
}

//------------------------------------------------------------------------------
// 데이타 쓰기 함수 (RS = 1)
//------------------------------------------------------------------------------
void lcdData(U8 byte)
{
    lcdSendByte(byte, LCD_RS);
    _delay_us(50);
}

//------------------------------------------------------------------------------
//                 Display Position Setting Function
//------------------------------------------------------------------------------
void lcdDisplayPosition(U8 line, U8 col)
{
    if (line == 0)  lcdCommand(LINE1 + col);
    else            lcdCommand(LINE2 + col);
}
