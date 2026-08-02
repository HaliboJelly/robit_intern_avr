#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "LCD_Text.h"
#define SW1 4	//PE4 확정
#define SW2 5	//PE5 시작
volatile int year = 2026;
volatile char month = 1, day = 1;
volatile char hour, minute, sec, csec;	//csec은 1/100초
volatile char msCnt;
volatile char run;	//1이면 시간 흐름
char lastDay(int y, char m)	//그 달의 마지막 날
{
	char d[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	if (m == 2 && ((y%4 == 0 && y%100 != 0) || y%400 == 0))
	{
		return 29;
	}
	return d[m-1];
}
ISR(TIMER0_OVF_vect)	//1ms 마다
{
	TCNT0 = 6;
	if (!run)
	{
		return;
	}
	msCnt++;
	if (msCnt < 10)
	{
		return;
	}
	msCnt = 0;
	csec++;
	if (csec < 100)
	{
		return;
	}
	csec = 0;
	sec++;
	if (sec < 60)
	{
		return;
	}
	sec = 0;
	minute++;
	if (minute < 60)
	{
		return;
	}
	minute = 0;
	hour++;
	if (hour < 24)
	{
		return;
	}
	hour = 0;
	day++;
	if (day <= lastDay(year, month))	//윤년/월별 일수 반영
	{
		return;
	}
	day = 1;
	month++;
	if (month <= 12)
	{
		return;
	}
	month = 1;
	year++;
}
void timer0Init(void)
{
	TCCR0 = (1 << CS02);	
	TCNT0 = 6;
	TIMSK |= (1 << TOIE0);
}
void adcInit(void)
{
	ADMUX = (1 << REFS0);	//채널 ADC0 (PF0)
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
int adcRead(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	{
		;
	}
	return ADCW;
}
char swPushed(char bit)	//눌리는 순간 1, 뗄 때까지 대기
{
	if (PINE & (1 << bit))
	{
		return 0;
	}
	if (PINE & (1 << bit))
	{
		return 0;
	}
	while (!(PINE & (1 << bit)))
	{
		;
	}
	return 1;
}
int setValue(char *name, int lo, int hi)	//가변저항으로 고르고 SW1로 확정
{
	int v;
	char buf[20];
	while (1)
	{
		v = lo + (long)adcRead() * (hi - lo + 1) / 1024;
		if (v > hi)	//1023일 때 넘어가는 것 방지
		{
			v = hi;
		}
		sprintf(buf, "%-5s %4d ", name, v);
		lcdString(0, 0, buf);
		if (swPushed(SW1))
		{
			return v;
		}
	}
}
int main(void)
{
	int y;
	char mo, d, h, mi, s, c, prevDay = 0;
	char buf[20];
	DDRE &= ~((1 << SW1) | (1 << SW2));
	PORTE |= (1 << SW1) | (1 << SW2);
	DDRF &= ~(1 << PF0);
	PORTF &= ~(1 << PF0);
	adcInit();
	lcdInit();
	timer0Init();
	sei();
	lcdClear();
	lcdString(1, 0, "SW1 to fix");
	year = 2000 + setValue("YEAR", 0, 99);
	month = setValue("MONTH", 1, 12);
	day = setValue("DAY", 1, lastDay(year, month));	//월에 맞는 범위만 허용
	hour = setValue("HOUR", 0, 23);
	minute = setValue("MIN", 0, 59);
	sec = setValue("SEC", 0, 59);
	lcdClear();
	lcdString(0, 0, "push SW2");
	while (!swPushed(SW2))
	{
		;
	}
	lcdClear();
	run = 1;
	while (1)
	{
		cli();	//ISR이 도중에 값을 바꾸지 않도록 한번에 복사
		y = year; mo = month; d = day;
		h = hour; mi = minute; s = sec; c = csec;
		sei();
		if (d != prevDay)	//날짜는 바뀔 때만 다시 그린다
		{
			sprintf(buf, "%02d%02d%02d", y%100, mo, d);
			lcdString(0, 0, buf);
			prevDay = d;
		}
		sprintf(buf, "%02d:%02d:%02d.%02d", h, mi, s, c);
		lcdString(1, 0, buf);
	}
	return 0;
}