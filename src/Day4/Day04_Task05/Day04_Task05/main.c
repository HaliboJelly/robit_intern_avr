#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
volatile char line[8];	//PC에서 받은 한 줄
volatile int lineLen;
volatile char lineReady;	//1이면 처리할 줄이 있음
ISR(USART0_RX_vect)
{
	char c = UDR0;
	if (c == '\r' || c == '\n')	//엔터를 만나면 한 줄 완성
	{
		if (lineLen > 0)
		{
			line[lineLen] = 0;
			lineReady = 1;
		}
		return;
	}
	if (lineReady)	//아직 처리 안 된 줄이 있으면 무시
	{
		return;
	}
	if (lineLen < 7)
	{
		line[lineLen] = c;
		lineLen++;
	}
}
void uartInit(void)
{
	DDRE |= (1 << PE1);
	DDRE &= ~(1 << PE0);
	UBRR0H = 0;
	UBRR0L = 16;	//57600
	UCSR0A = 0x00;
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
}
void uartPutchar(char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{
		;
	}
	UDR0 = data;
}
void uartPuts(char *s)
{
	int i;
	for (i = 0; s[i] != 0; i++)
	{
		uartPutchar(s[i]);
	}
}
void servoInit(void)	//PB7은 OC1C, timer1 fast PWM
{
	DDRB |= (1 << PB7);
	TCCR1A = (1 << COM1C1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);	//분주비 8 -> 1틱 0.5us
	ICR1 = 39999;	//40000틱 = 20ms = 50Hz
}
void servoAngle(int deg)	//0도는 0.5ms, 180도는 2.4ms
{
	OCR1C = 1000 + (long)deg * 3800 / 180;
}
int main(void)
{
	int i, angle, ok;
	char buf[50];
	servoInit();
	uartInit();
	sei();
	servoAngle(90);	//전원 켜면 90도로 복귀
	_delay_ms(500);	//서보가 도착할 때까지 대기
	uartPuts(" ready 90\r\n");
	while (1)
	{
		if (!lineReady)
		{
			continue;
		}
		ok = 1;
		angle = 0;
		for (i = 0; line[i] != 0; i++)
		{
			if (line[i] < '0' || line[i] > '9')	//숫자가 아닌 문자가 섞임
			{
				ok = 0;
				break;
			}
			angle = angle * 10 + (line[i] - '0');
			if (angle > 999)	//자릿수가 넘치기 전에 끊는다
			{
				ok = 0;
				break;
			}
		}
		if (!ok)
		{
			uartPuts("only angle 0~180\r\n");
		}
		else if (angle > 180)	//범위를 벗어나면 모터를 움직이지 않는다
		{
			sprintf(buf, "%d out (0~180)\r\n", angle);
			uartPuts(buf);
		}
		else
		{
			servoAngle(angle);
			sprintf(buf, "angle = %d\r\n", angle);
			uartPuts(buf);
		}
		cli();
		lineLen = 0;
		lineReady = 0;
		sei();
	}
	return 0;
}