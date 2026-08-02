//가변저항으로 다이나믹셀 위치, PC 숫자로 속도. lcd에 표시
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include "LCD_Text.h"
volatile char rx_digit;	//pc에서 받은 마지막 숫자
volatile char rx_new;	//1이면 새 값이 들어옴
const unsigned int crc_table[256] PROGMEM = {	//다이나믹셀 프로토콜 2.0 표준 표
	0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
	0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
	0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
	0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
	0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
	0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
	0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
	0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
	0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
	0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
	0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
	0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
	0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
	0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
	0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
	0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
	0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
	0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
	0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
	0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
	0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
	0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
	0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
	0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
	0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
	0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
	0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
	0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
	0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
	0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
	0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
	0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};
ISR(USART0_RX_vect)
{
	char c = UDR0;	//반드시 읽어야 rxc0가 내려간다
	if (c < '0' || c > '9')	//숫자가 아니면 무시
	{
		return;
	}
	rx_digit = c - '0';
	rx_new = 1;
}
void uart_init(void)
{
	DDRE |= (1 << PE1);
	DDRE &= ~(1 << PE0);
	UBRR0H = 0;
	UBRR0L = 16;	//57600, 다이나믹셀 기본값과 같다
	UCSR0A = 0x00;
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
}
void uart_putchar(char data)
{
	while (!(UCSR0A & (1 << UDRE0)))
	{
		;
	}
	UDR0 = data;
}
void adc_init(void)
{
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
int adc_read(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	{
		;
	}
	return ADCW;
}
int adc_avg(void)	//떨림이 심해서 16번 평균(왜냐면 가변저항 쓸 떄 충분히 값이 안정화 되지 못해서 평균으로 하는게 안정적임.  ps.가변저항 값이 금요일에는 안정적이었는데 주말에 시연할 때 회로를 조금 수정하다보니 값이 조금씩 바뀌어서 추가함. )
{
	int i;
	long sum = 0;
	for (i = 0; i < 16; i++)
	{
		sum += adc_read();
	}
	return sum / 16;
}
unsigned int update_crc(unsigned int crc, char *data, int len)
{
	int i, idx;
	for (i = 0; i < len; i++)
	{
		idx = ((crc >> 8) ^ data[i]) & 0xFF;
		crc = (crc << 8) ^ pgm_read_word(&crc_table[idx]);
	}
	return crc;
}
void dxl_write(int addr, char *data, int len)	//쓰기 명령 패킷 송신
{
	char packet[16];
	int idx = 0;
	int i;
	int length = 1 + 2 + len + 2;	//명령1+주소2+데이터n+crc2
	unsigned int crc;
	packet[idx++] = 0xFF;
	packet[idx++] = 0xFF;
	packet[idx++] = 0xFD;
	packet[idx++] = 0x00;	//예약
	packet[idx++] = 1;	//다이나믹셀 id
	packet[idx++] = length & 0xFF;
	packet[idx++] = length >> 8;
	packet[idx++] = 0x03;	//write 명령
	packet[idx++] = addr & 0xFF;
	packet[idx++] = addr >> 8;
	for (i = 0; i < len; i++)
	{
		packet[idx++] = data[i];
	}
	crc = update_crc(0, packet, idx);
	packet[idx++] = crc & 0xFF;
	packet[idx++] = crc >> 8;
	for (i = 0; i < idx; i++)
	{
		uart_putchar(packet[i]);
	}
}
void dxl_write1(int addr, char value)
{
	dxl_write(addr, &value, 1);
}
void dxl_write4(int addr, long value)	//리틀 엔디안
{
	char d[4];
	d[0] = value;
	d[1] = value >> 8;
	d[2] = value >> 16;
	d[3] = value >> 24;
	dxl_write(addr, d, 4);
}
void lcd_show(int speed, int pos)
{
	char buf[20];
	sprintf(buf, "Speed:%4d", speed);	//폭을 고정해서 잔상 방지
	lcdString(0, 0, buf);
	sprintf(buf, "Pos  :%4d", pos);
	lcdString(1, 0, buf);
}
int main(void)
{
	int goal_pos, goal_speed = 0;
	int last_pos = -1;	//마지막으로 보낸 위치
	DDRE |= (1 << PE2);
	PORTE |= (1 << PE2);	//max485 re+de, 계속 high로 두고 송신만 한다
	DDRF &= ~(1 << PF0);	//가변저항
	PORTF &= ~(1 << PF0);
	uart_init();
	adc_init();
	lcdInit();	//안에서 i2c_init까지 해준다
	lcdClear();
	sei();
	_delay_ms(500);	//다이나믹셀 전원 안정화 대기
	dxl_write1(68, 1);	//쓰기에 응답 안 하게 해서 버스 충돌 방지
	_delay_ms(10);	//다이나믹셀이 앞 패킷을 처리할 시간
	dxl_write1(64, 1);	//토크
	_delay_ms(10);
	while (1)
	{
		goal_pos = adc_avg();	//0에서1023을 목표 위치로
		if (last_pos < 0 || goal_pos - last_pos >= 15 || last_pos - goal_pos >= 15)	//목표가 계속 바뀌면 모터가 궤적을 다시 짜느라 못 움직인다(따라서 가변저항 값을 안정화 시키는게 가장 중요하다.)
		{
			dxl_write4(116, goal_pos);	//목표위치
			last_pos = goal_pos;
			lcd_show(goal_speed, last_pos);
		}
		if (rx_new)
		{
			rx_new = 0;
			goal_speed = rx_digit * 300 / 9;	//0에서9를 0에서300에 대응
			dxl_write4(112, goal_speed);
			lcd_show(goal_speed, last_pos);
		}
		_delay_ms(100);
	}
	return 0;
}