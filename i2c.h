//
//  i2c.h
//  i2c
//
//  Created by Michael Köhler on 09.10.17.
//
//

#ifndef i2c_h
#define i2c_h

/* TODO: setup i2c/twi */
#define F_I2C			400000UL// clock i2c
#define PSC_I2C			1		// prescaler i2c
#define SET_TWBR		(F_CPU/F_I2C-16UL)/(PSC_I2C*2UL)

#include <stdio.h>
#include <avr/io.h>

void i2c_init(void);				// init hw-i2c
void i2c_start(uint8_t i2c_addr);	// send i2c_start_condition
void i2c_stop(void);				// send i2c_stop_condition
void i2c_byte(uint8_t byte);		// send data_byte

uint8_t i2c_readAck(void);          // read byte with ACK
uint8_t i2c_readNAck(void);         // read byte with NACK

#endif /* i2c_h */
