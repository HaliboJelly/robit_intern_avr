#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int main(void)
{
	DDRA  = 0xFF;
	DDRE &= ~((1 << PE4) | (1 << PE5));   // 스위치 0,1			PE 4,5에 할당 PE 4번부터 외부 인터럽트라서
	DDRD &= ~((1 << PD2) | (1 << PD3));   // 스위치 2,3			PD 0,1에 할당
	
	PORTA = ~0xFF;//꺼진상태
	
	EIMSK = (1 << INT2) | (1 << INT3);
	EICRA = (1 << ISC21) | (1 << ISC31);              // INT2, INT3 

	sei();

	while (1)
	{	
		if ((!(PINE & (1<<PINE4))) && (!(PINE & (1<<PINE5))))//만약 0번 1번 동시 되면 다 켜짐
		{
			PORTA = 0x00;
			_delay_ms(500);
		}
		else if (!(PINE & (1<<PINE4)))//만약 0번 스위치에 입력이 들어오면 4,5,6,7 led켜짐
		{
			PORTA = ~((1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
			_delay_ms(500);
		}
		else if (!(PINE & (1<<PINE5)))//만약 1번 스위치에 입력이 들어오면 0,1,2,3 led 켜짐
		{
			PORTA = ~((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3));
			_delay_ms(500);
		}
		else			//아닐때 0.5초 마다 깜빡이기
		{
			PORTA = 0xFF;
			_delay_ms(500);
			PORTA = 0x00;
			_delay_ms(500);
			
		}
		
	}
}

ISR(INT2_vect)  //스위치  3번
{
	PORTA = ~(1 << PA7);   // PA=7만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA6);   // PA=6만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA5);   // PA=5만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA4);   // PA=4만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA3);   // PA=3만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA2);   // PA=2만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA1);   // PA=1만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA0);   // PA=0만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
}

ISR(INT3_vect)	//스위치 4번
{
	PORTA = ~(1 << PA0);   // PA=0만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA1);   // PA=1만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA2);   // PA=2만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA3);   // PA=3만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA4);   // PA=4만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA5);   // PA=5만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA6);   // PA=6만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
	PORTA = ~(1 << PA7);   // PA=7만 0(켜짐), 나머지는 전부 1(꺼짐)
	_delay_ms(100);
}
