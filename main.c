/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */


#include <stdlib.h>
#include <avr/io.h>
#include "i2c.h"
#include "lcd.h"

int main(void)
{
    /* init I2C-Bus, for settings look at i2c.h */
    i2c_init();
    /* init display, after init turn on */
    lcd_init(LCD_DISP_ON);
    /* clear screen */
    lcd_clrscr();
    /* put string from flash, cursor is at position x=0, y=0, left, top corner */
    lcd_puts_p(PSTR("OLED Display "));
    /* set cursor at begin from next line */
    lcd_gotoxy(0,1);
    /* put string from sram */
    lcd_puts_p("with I2C ");
    
    for(;;){
        /* insert your main loop code here */
       
    }
    return 0;   /* never reached */
}
