//
//  i2c.c
//  i2c
//
//  Created by Michael Köhler on 09.10.17.
//
//

#include "i2c.h"

#if defined (__AVR_ATmega328__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega168PA__) || defined(__AVR_ATmega88P__) || defined(__AVR_ATmega48P__) || defined(__AVR_ATmega1284P__) || defined (__AVR_ATmega324A__) || defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324PA__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega644A__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1284P__)
#if PSC_I2C != 1 && PSC_I2C != 4 && PSC_I2C != 16 && PSC_I2C != 64
#error "Wrong prescaler for TWI !"
#elif SET_TWBR < 0 || SET_TWBR > 255
#error "TWBR out of range, change PSC_I2C or F_I2C !"
#endif
void i2c_init(void){
    // set clock
    switch (PSC_I2C) {
        case 4:
            TWSR = 0x1;
            break;
        case 16:
            TWSR = 0x2;
            break;
        case 64:
            TWSR = 0x3;
            break;
        default:
            TWSR = 0x00;
            break;
    }
    TWBR = (uint8_t)SET_TWBR;
    // enable
    TWCR = (1 << TWEN);
}
void i2c_start(uint8_t i2c_addr){
    // i2c start
    TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);
    while((TWCR & (1 << TWINT)) == 0);
    // send adress
    TWDR = i2c_addr;
    TWCR = (1 << TWINT)|( 1 << TWEN);
    while((TWCR & (1 << TWINT)) == 0);
}
void i2c_stop(void){
    // i2c stop
    TWCR = (1 << TWINT)|(1 << TWSTO)|(1 << TWEN);
}
void i2c_byte(uint8_t byte){
    TWDR = byte;
    TWCR = (1 << TWINT)|( 1 << TWEN);
    while((TWCR & (1 << TWINT)) == 0);
}
uint8_t i2c_readAck(void){
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}
uint8_t i2c_readNAck(void){
    TWCR = (1<<TWINT)|(1<<TWEN);
    while ((TWCR & (1<<TWINT)) == 0);
    return TWDR;
}
#else
#error "Micorcontroller not supported now!"
#endif
