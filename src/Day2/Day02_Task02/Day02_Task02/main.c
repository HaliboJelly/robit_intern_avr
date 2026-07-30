#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "LCD_Text.h"

#define LED_STEP 8   // LED 개수 = 단계 수

void adc_init(void)
{
	ADMUX = (1 << REFS0);
	// ADC 활성화
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	DDRF = 0x00;    // ADC 입력 포트
	PORTF = 0x00;   // 아날로그 입력이므로 내부 풀업 끔
}

uint16_t adc_read(uint8_t channel)
{
	// 채널 선택, 상위 REFS 비트는 유지
	ADMUX = (ADMUX & 0xE0) | (channel & 0x1F);
	ADCSRA |= (1 << ADSC);        // 변환 시작
	while (ADCSRA & (1 << ADSC)); // 변환 완료까지 대기
	return ADC; // ADCL, ADCH 합쳐진 10bit
}

// 8회
uint16_t adc_read_avg(uint8_t channel, uint8_t count)
{
	uint32_t sum = 0;
	for (uint8_t i = 0; i < count; i++) sum += adc_read(channel);
	return (uint16_t)(sum / count);
}

// ADC값에 따라 LED가 순차적으로 켜짐
void led_sequential(uint16_t adc_value)
{
	// 0~1023을 0~8로 환산 (1023/128 = 7이라 128로 나누면 PA7이 안 켜짐)
	uint8_t count = (uint8_t)((uint32_t)adc_value * LED_STEP / 1023UL);
	if (count > LED_STEP) count = LED_STEP;

	uint8_t leds = 0;
	for (uint8_t i = 0; i < count; i++)
	{
		leds |= (1 << i);   // count개만큼 LSB부터 순서대로 켤 위치 표시
	}
	PORTA = (uint8_t)~leds;   // 액티브 로우 -> 반전
}

int main(void)
{
	DDRA = 0xFF;      // PORTA 전체를 출력으로 설정
	PORTA = 0xFF;     // LED 다 끔

	char buf[17];
	uint16_t prev = 0xFFFF;   // 값 바뀔 때만 LCD 갱신

	lcdInit();        // 내부에서 i2c_init()까지 수행
	adc_init();

	lcdClear();       // lcdInit()이 남기는 "Init OK" 지움
	lcdString(0, 0, "21th_LJW");   // 인자 순서 (line, col)

	while (1)
	{
		uint16_t adc_value = adc_read_avg(0, 8);

		led_sequential(adc_value);   // ADC 값에 따라 LED 순차 점등

		if (adc_value != prev)
		{
			prev = adc_value;

			// 5000mV 기준. adc*500은 16bit를 넘으니 unsigned long으로 계산
			// avr-libc는 %f를 기본 지원 안 해서 정수 몫/나머지로 소수점 표시
			unsigned long mv = (unsigned long)adc_value * 500UL / 1023UL;
			unsigned int volts = mv / 100;
			unsigned int frac  = mv % 100;

			snprintf(buf, sizeof(buf), "%4u  %u.%02uV", adc_value, volts, frac);
			lcdString(1, 0, buf);   // 2행 0열부터 출력
		}

		_delay_ms(200);
	}
}