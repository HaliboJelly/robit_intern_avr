#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define BIT_DELAY()         _delay_us(104)//보더레이트 9600일때 104 마이크로세컨드 간격으로 보내기 

#define TX_HIGH()   (PORTD |=  (1 << PD3))	//pd이기 떄문에 유아트1 쪽 핀 이용. pd3는 tx로 데이터시트에 나옴
#define TX_LOW()    (PORTD &= ~(1 << PD3))

static void swUartPutchar(char data)
{
    char i;

    //start

    TX_LOW();
    BIT_DELAY();
	
	//data
    for (i = 0; i < 8; i++)
    {
        if (data & 0x01){
			TX_HIGH();
		}
        else{
			TX_LOW();
		}

        data >>= 1;         // 다음 비트를 최하위로 끌어내린다
        BIT_DELAY();
    }

    //stop
    TX_HIGH();
    BIT_DELAY();
}

int main(void)
{
	DDRD  |= (1 << PD3);      // 출력
	TX_HIGH();                

    while (1)
    {
        swUartPutchar(72);      // 'H'
        swUartPutchar(101);     // 'e'
        swUartPutchar(108);     // 'l'
        swUartPutchar(108);     // 'l'
        swUartPutchar(111);     // 'o'
        swUartPutchar(87);      // 'W'
        swUartPutchar(111);     // 'o'
        swUartPutchar(114);     // 'r'
        swUartPutchar(108);     // 'l'
        swUartPutchar(100);     // 'd'
        swUartPutchar(33);      // '!'

        swUartPutchar(13);      // \r 커서를 줄 맨 앞으로
        swUartPutchar(10);      // \n 다음 줄로

        _delay_ms(1000);        // 1초마다
    }

    return 0;
}