//UART로 LED 제어. SW1 누르면 리셋
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
volatile char rxBuf[16];	//수신 링 버퍼
volatile int rxHead;	//isr이 쓰는 위치
volatile int rxTail;	//main이 읽는 위치
char ledIndex;	//켜져 있는 led 번호
char ledOn;	//0이면 전부 꺼짐
ISR(USART0_RX_vect)
{
	char c = UDR0;	//반드시 읽어야 rxc0 플래그가 내려간다
	int next = (rxHead + 1) % 16;
	if (next == rxTail)	//버퍼가 꽉 차면 버린다
	{
		return;
	}
	rxBuf[rxHead] = c;
	rxHead = next;
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
char uartAvailable(void)
{
	return (rxHead != rxTail);
}
char uartGetchar(void)
{
	char c = rxBuf[rxTail];
	rxTail = (rxTail + 1) % 16;
	return c;
}
void ledUpdate(void)
{
	if (ledOn)
	{
		PORTA = ~(1 << ledIndex);	//액티브 로우라 켤 자리만 0
	}
	else
	{
		PORTA = 0xFF;	//전부 1이면 소등
	}
}
void resetAll(void)
{
	ledIndex = 0;
	ledOn = 0;
	ledUpdate();
	uartPuts("reset\r\n");
}
void handleCommand(char cmd)
{
	char buf[20];
	if (cmd >= '0' && cmd <= '7')
	{
		ledIndex = cmd - '0';	//문자를 숫자로
		ledOn = 1;
		ledUpdate();
		sprintf(buf, "%d led oon\r\n", ledIndex);
		uartPuts(buf);
	}
	else if (cmd == '8')
	{
		if (!ledOn)
		{
			uartPuts("led nono\r\n");
			return;
		}
		ledIndex = (ledIndex + 1) & 0x07;	//PA7 다음은 pa0
		ledUpdate();
		uartPuts("LEFT\r\n");
	}
	else if (cmd == '9')
	{
		if (!ledOn)
		{
			uartPuts("led nono\r\n");
			return;
		}
		ledIndex = (ledIndex - 1) & 0x07;	//pa0 이전은 pa7
		ledUpdate();
		uartPuts("right\r\n");
	}
	else if (cmd == '\r' || cmd == '\n')	//터미널이 붙여 보내는 개행문자는 무시
	{
		;
	}
	else
	{
		uartPuts("error\r\n");
	}
}
void swPoll(void)	//눌리는 순간에만 1회 동작
{
	static char prev = 1;
	char now;
	if (PINE & (1 << PE4))
	{
		now = 1;
	}
	else
	{
		now = 0;
	}
	if (prev == 1 && now == 0)
	{
		_delay_ms(20);	//채터링 제거
		if (!(PINE & (1 << PE4)))
		{
			resetAll();
		}
	}
	prev = now;
}
int main(void)
{
	DDRA = 0xFF;
	PORTA = 0xFF;	//시작은 전부 소등
	DDRE &= ~(1 << PE4);	//sw1
	PORTE |= (1 << PE4);
	DDRE |= (1 << PE2);	//max485 re+de, 하이로 두면 ro가 rxd0을 놓아준다
	PORTE |= (1 << PE2);
	uartInit();
	sei();
	uartPuts("ready\r\n");
	while (1)
	{
		swPoll();
		while (uartAvailable())
		{
			handleCommand(uartGetchar());
		}
		_delay_ms(5);
	}
	return 0;
}