/*==============================================================================
 *   ATmega128 TWI(I2C) Master Driver
 *      MPU     : ATmega128 (16MHz)
 *      SCL     : PD0
 *      SDA     : PD1
 *==============================================================================*/

#ifndef __I2C_H
#define __I2C_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define I2C_READ        0x01    // 주소 뒤에 붙는 R/W 비트 (읽기)
#define I2C_WRITE       0x00    // 주소 뒤에 붙는 R/W 비트 (쓰기)

// SCL = F_CPU / (16 + 2*TWBR*4^TWPS),  TWPS=0(분주 1) 기준
// 16MHz / 100kHz -> TWBR = 72
#define I2C_SCL_FREQ    100000UL
#define TWBR_VAL        (((F_CPU / I2C_SCL_FREQ) - 16) / 2)

typedef unsigned char  U8;
typedef unsigned int   U16;

void i2c_init(void);
void i2c_start(U8 address);                 // address는 이미 8비트로 시프트된 값
void i2c_transmit(U8 data);
void i2c_stop(void);
void i2c_transmit_nbytes(U8 address, U8 *data, U16 length);

#endif
