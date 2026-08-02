//기본적으로 과제3번 배경

#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include <stdio.h>
volatile int msCnt;
volatile char measure;	//1이면 측정할 차례
ISR(TIMER0_OVF_vect)	//1ms 마다
{
	TCNT0 = 6;
	msCnt++;
	if (msCnt < 100)	//측정 주기 100ms
	{
		return;
	}
	msCnt = 0;
	measure = 1;
}
void timer0Init(void)
{
	TCCR0 = (1 << CS02);
	TCNT0 = 6;
	TIMSK |= (1 << TOIE0);
}
void uartInit(void)
{
	DDRE |= (1 << PE1);
	DDRE &= ~(1 << PE0);
	UBRR0H = 0;
	UBRR0L = 16;	//57600
	UCSR0A = 0x00;
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
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
void adcInit(void)
{
	ADMUX = (1 << REFS0) | 1;	//PF1
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
int movingAvg(int adc)	//최근 8개의 평균(배열을 이용하여 8개의 값을 받아낸 뒤에 그 값으로 평균을 낸다)
{
	static int buf[8];	//가장 오래된 값을 새 값으로 덮어쓴다
	static int idx, sum, cnt;
	sum -= buf[idx];
	buf[idx] = adc;
	sum += adc;
	idx++;
	if (idx >= 8)
	{
		idx = 0;
	}
	if (cnt < 8)	//처음에는 채워진 개수로만 나눈다
	{
		cnt++;
	}
	return sum / cnt;
}
int main(void)
{
	int raw, filtered, mm;
	char buf[60];
	DDRF &= ~(1 << PF1);	//PSD 출력 입력으로
	PORTF &= ~(1 << PF1);
	adcInit();
	uartInit();
	timer0Init();
	sei();
	uartPuts("PSD start\r\n");
	while (1)
	{
		if (!measure)
		{
			continue;
		}
		measure = 0;
		raw = adcRead();
		filtered = movingAvg(raw);
		if (filtered > 568)	//20cm보다 가까우면 판별 불가.  센서의 한계(제조사에서 제공해준 그래프를 보면 20센치 이하일 때 그래프가 뒤집힘)
		{
			sprintf(buf, "RAW:%4d / FILTERED:%4d / TOO CLOSE\r\n", raw, filtered);
		}
		else if (filtered < 104)	//150cm보다 멀면 측정 범위 밖(150센치 이상일때 스펙상으로 이용불가. 그래프에도 150센치 이상 안 나와 있어서 보정 불가)
		{
			sprintf(buf, "RAW:%4d / FILTERED:%4d / TOO FAR\r\n", raw, filtered);
		}
		else
		{
			mm = 377000 * pow(filtered, -1.189);	//데이터시트 곡선을 근사한 식(제조사에서 제공한 오차 데이터를 ai에게 입력 후 식을 구함)
			sprintf(buf, "RAW:%4d / FILTERED:%4d / DISTANCE:%4d.%dcm\r\n", raw, filtered, mm/10, mm%10);
		}
		uartPuts(buf);
	}
	return 0;
}