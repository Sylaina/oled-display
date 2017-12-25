/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */


#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "lcd.h"

int main(void)
{
    /* insert your hardware initialization here */
    i2c_init();
    
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_puts_p(PSTR("OLED Display "));
    lcd_gotoxy(0,1);
    lcd_puts_p(PSTR("with I2C "));
    
    sei();
    
    for(;;){
        /* insert your main loop code here */
       
    }
    return 0;   /* never reached */
}
