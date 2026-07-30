#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
int main(void)
{
	DDRA  = 0xFF;
	DDRD &= ~((1 << PD2) | (1 << PD3));    // 스위치 2,3
	DDRE &= ~((1 << PE4) | (1 << PE5));    // 스위치 0,1
	PORTA = 0xFF;                          // 카운터 0
	EIMSK = (1 << INT2) | (1 << INT3) | (1 << INT4) | (1 << INT5);
	EICRA = (1 << ISC21) | (1 << ISC31);   // INT2, INT3
	EICRB = (1 << ISC41) | (1 << ISC51);   // INT4, INT5
	sei();
	while (1)
	{
		if (PORTA == 0x00)  PORTA = 0xFF;  // 다 세면 0으로 되돌리기
		else                PORTA--;       // 감소 = 표시값 증가
		_delay_ms(100);
	}
}
ISR(INT4_vect)    // SW0: LED 3개 우측 이동
{
	int16_t temp = 0b11100000;
	for (int i = 0; i < 6; i++)
	{
		PORTA = ~temp;
		temp = temp >> 1;
		_delay_ms(300);
	}
}
ISR(INT5_vect)    // SW1: LED 3개 좌측 이동
{
	int16_t temp = 0b00000111;
	for (int i = 0; i < 6; i++)
	{
		PORTA = ~temp;
		temp = temp << 1;
		_delay_ms(300);
	}
}
ISR(INT2_vect)    // SW2: 1개 좌측 이동 후 우측 이동
{
	int16_t temp = 0b00000001;
	for (int i = 0; i < 8; i++)
	{
		PORTA = ~temp;
		temp = temp << 1;
		_delay_ms(100);
	}
	for (int i = 0; i < 8; i++)
	{
		temp = temp >> 1;
		PORTA = ~temp;
		_delay_ms(100);
	}
}
ISR(INT3_vect)    // SW3: 카운터 초기화
{
	PORTA = 0xFF;
}
