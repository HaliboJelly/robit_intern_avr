#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "LCD_Text.h"


int A = 1;
int B = 1;
int op_index = 0;

char operators[] = {'+', '-', '*', '/'};

#define F_A   0x01   // SW1: A 증가
#define F_OP  0x02   // SW2: 연산자 변경
#define F_B   0x04   // SW3: B 증가
#define F_EQ  0x08   // SW4: 연산 실행
volatile uint8_t sw_flag = 0;

// 남는 칸을 공백으로 채움(짧은 문자열을 쓰면 이전 글자가 남음)
static void pad16(char *s)
{
	uint8_t i = strlen(s);
	while (i < 16) s[i++] = ' ';
	s[16] = '\0';
}

static void show_preview(void)
{
	char buf[24];
	snprintf(buf, sizeof(buf), "%d %c %d = ?", A, operators[op_index], B);
	pad16(buf);
	lcdString(0, 0, buf);
}

static void show_result(void)
{
	char buf[24];
	char op = operators[op_index];

	if (op == '/') {
		if (B == 0) {
			snprintf(buf, sizeof(buf), "Error: B=0");
			} else {
			long fixed = ((long)A * 100) / B; // 소수 2자리 고정소수점
			int intpart = fixed / 100;
			int frac = fixed % 100;
			if (frac < 0) frac = -frac;
			snprintf(buf, sizeof(buf), "%d / %d = %d.%02d", A, B, intpart, frac);
		}
		} else {
		int result = 0;
		switch (op) {
			case '+': result = A + B; 
			break;
			case '-': result = A - B; 
			break;
			case '*': result = A * B; 
			break;
		}
		snprintf(buf, sizeof(buf), "%d %c %d = %d", A, op, B, result);
	}

	pad16(buf);
	lcdString(0, 0, buf);
}

// 1씩 증가
static int step_value(int v)
{
	v++;
	if (v > 99) v = 1;
	return v;
}


ISR(INT2_vect) { sw_flag |= F_B;  }   // SW3 (PD2)
ISR(INT3_vect) { sw_flag |= F_EQ; }   // SW4 (PD3)
ISR(INT4_vect) { sw_flag |= F_A;  }   // SW1 (PE4)
ISR(INT5_vect) { sw_flag |= F_OP; }   // SW2 (PE5)

static void switches_init(void)
{
	DDRD &= ~((1 << PD2) | (1 << PD3));
	DDRE &= ~((1 << PE4) | (1 << PE5));
}

static void ext_interrupt_init(void)
{
	EICRA = (EICRA & 0x0F) | (1 << ISC31) | (1 << ISC21);

	EICRB = (EICRB & 0xF0) | (1 << ISC51) | (1 << ISC41);

	EIFR = (1 << INTF2) | (1 << INTF3) | (1 << INTF4) | (1 << INTF5);

	EIMSK |= (1 << INT2) | (1 << INT3) | (1 << INT4) | (1 << INT5);
}

int main(void)
{
	uint8_t flag;

	switches_init();
	lcdInit();              // 내부에서 i2c_init()까지 수행
	lcdClear();             // lcdInit()이 남기는 "Init OK" 지움
	lcdString(1, 0, "21th_LJW");

	ext_interrupt_init();
	show_preview();
	sei();

	while (1)
	{
		if (sw_flag == 0) continue;

		// ISR이 중간에 끼어들지 않게 읽고 지우는 동안만 인터럽트를 막는다
		cli();
		flag = sw_flag;
		sw_flag = 0;
		sei();

		if (flag & F_A)  A = step_value(A);
		if (flag & F_B)  B = step_value(B);
		if (flag & F_OP) op_index = (op_index + 1) % 4;   // + - * / 순환

		if (flag & F_EQ) show_result();
		else             show_preview();   // 값이 바뀌면 결과를 다시 계산해야 함

		// 채터링이 끝나길 기다린 뒤, 그 사이 쌓인 인터럽트를 버린다
		_delay_ms(50);
		EIFR = (1 << INTF2) | (1 << INTF3) | (1 << INTF4) | (1 << INTF5);
		sw_flag = 0;
	}
}