/*
 * This file is part of lcd library for ssd1306/ssd1309/sh1106 oled-display.
 *
 * lcd library for ssd1306/ssd1309/sh1106 oled-display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any later version.
 *
 * lcd library for ssd1306/ssd1309/sh1106 oled-display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von lcd library for ssd1306/ssd1309/sh1106 oled-display.
 *
 * lcd library for ssd1306/ssd1309/sh1106 oled-display ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * lcd library for ssd1306/ssd1309/sh1106 oled-display wird in der Hoffnung, dass es nützlich sein wird, aber
 * OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
 * Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Details.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
 *
 *  lcd.h
 *
 *  Created by Michael Köhler on 22.12.16.
 *  Copyright 2016 Skie-Systems. All rights reserved.
 *
 *  lib for OLED-Display with ssd1306/ssd1309/sh1106-Controller
 *  first dev-version only for I2C-Connection
 *  at ATMega328P like Arduino Uno
 *
 *  at GRAPHICMODE lib needs static SRAM for display:
 *  DISPLAY-WIDTH * DISPLAY-HEIGHT + 2 bytes
 *
 *  at TEXTMODE lib need static SRAM for display:
 *  2 bytes (cursorPosition)
 */

#include "lcd.h"
#include "font.h"
#include <string.h>

#if defined SPI
#include <util/delay.h>
#endif

static struct {
    uint8_t x;
    uint8_t y;
} cursorPosition;

static uint8_t charMode = NORMALSIZE;
#if defined GRAPHICMODE
#include <stdlib.h>
static uint8_t displayBuffer[DISPLAY_HEIGHT/8][DISPLAY_WIDTH];
#elif defined TEXTMODE
#else
#error "No valid displaymode! Refer lcd.h"
#endif


const uint8_t init_sequence [] PROGMEM = {    // Initialization Sequence
    LCD_DISP_OFF,    // Display OFF (sleep mode)
    0x20, 0b00,      // Set Memory Addressing Mode
    // 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
    // 10=Page Addressing Mode (RESET); 11=Invalid
    0xB0,            // Set Page Start Address for Page Addressing Mode, 0-7
    0xC8,            // Set COM Output Scan Direction
    0x00,            // --set low column address
    0x10,            // --set high column address
    0x40,            // --set start line address
    0x81, 0x3F,      // Set contrast control register
    0xA1,            // Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
    0xA6,            // Set display mode. A6=Normal; A7=Inverse
    0xA8, DISPLAY_HEIGHT-1, // Set multiplex ratio(1 to 64)
    0xA4,            // Output RAM to Display
					 // 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
    0xD3, 0x00,      // Set display offset. 00 = no offset
    0xD5,            // --set display clock divide ratio/oscillator frequency
    0xF0,            // --set divide ratio
    0xD9, 0x22,      // Set pre-charge period
		     // Set com pins hardware configuration
#if DISPLAY_HEIGHT==64
    0xDA, 0x12,      
#elif DISPLAY_HEIGHT==32
    0xDA, 0x02,
#endif
    0xDB,            // --set vcomh
    0x20,            // 0x20,0.77xVcc
    0x8D, 0x14,      // Set DC-DC enable
    
    
};
#pragma mark LCD COMMUNICATION
void lcd_command(uint8_t cmd[], uint8_t size) {
#if defined I2C
    i2c_start((LCD_I2C_ADR << 1) | 0);
    i2c_byte(0x00);    // 0x00 for command, 0x40 for data
    for (uint8_t i=0; i<size; i++) {
        i2c_byte(cmd[i]);
    }
    i2c_stop();
#elif defined SPI
	LCD_PORT &= ~(1 << CS_PIN);
	LCD_PORT &= ~(1 << DC_PIN);
	for (uint8_t i=0; i<size; i++) {
        SPDR = cmd[i];
        while(!(SPSR & (1<<SPIF)));
    }
    LCD_PORT |= (1 << CS_PIN);
#endif
}
void lcd_data(uint8_t data[], uint16_t size) {
#if defined I2C
    i2c_start((LCD_I2C_ADR << 1) | 0);
    i2c_byte(0x40);    // 0x00 for command, 0x40 for data
    for (uint16_t i = 0; i<size; i++) {
        i2c_byte(data[i]);
    }
    i2c_stop();
#elif defined SPI
	LCD_PORT &= ~(1 << CS_PIN);
	LCD_PORT |= (1 << DC_PIN);
	for (uint16_t i = 0; i<size; i++) {
        SPDR = data[i];
        while(!(SPSR & (1<<SPIF)));
    }
    LCD_PORT |= (1 << CS_PIN);
#endif
}
#pragma mark -
#pragma mark GENERAL FUNCTIONS
void lcd_init(uint8_t dispAttr){
#if defined I2C
    i2c_init();
#elif defined SPI
	DDRB |= (1 << PB2)|(1 << PB3)|(1 << PB5);
    SPCR = (1 << SPE)|(1<<MSTR)|(1<<SPR0);
    LCD_DDR |= (1 << CS_PIN)|(1 << DC_PIN)|(1 << RES_PIN);
    LCD_PORT |= (1 << CS_PIN)|(1 << DC_PIN)|(1 << RES_PIN);
    LCD_PORT &= ~(1 << RES_PIN);
    _delay_ms(10);
    LCD_PORT |= (1 << RES_PIN);
#endif

    uint8_t commandSequence[sizeof(init_sequence)+1];
    for (uint8_t i = 0; i < sizeof (init_sequence); i++) {
        commandSequence[i] = (pgm_read_byte(&init_sequence[i]));
    }
    commandSequence[sizeof(init_sequence)]=(dispAttr);
    lcd_command(commandSequence, sizeof(commandSequence));
    lcd_clrscr();
}
void lcd_gotoxy(uint8_t x, uint8_t y){
    x = x * sizeof(FONT[0]);
    lcd_goto_xpix_y(x,y);
}
void lcd_goto_xpix_y(uint8_t x, uint8_t y){
    if( x > (DISPLAY_WIDTH) || y > (DISPLAY_HEIGHT/8-1)) return;// out of display
    cursorPosition.x=x;
    cursorPosition.y=y;
#if defined (SSD1306) || defined (SSD1309)
    uint8_t commandSequence[] = {0xb0+y, 0x21, x, 0x7f};
#elif defined SH1106
    uint8_t commandSequence[] = {0xb0+y, 0x21, 0x00+((2+x) & (0x0f)), 0x10+( ((2+x) & (0xf0)) >> 4 ), 0x7f};
#endif
    lcd_command(commandSequence, sizeof(commandSequence));
}
void lcd_clrscr(void){
#ifdef GRAPHICMODE
    for (uint8_t i = 0; i < DISPLAY_HEIGHT/8; i++){
        memset(displayBuffer[i], 0x00, sizeof(displayBuffer[i]));
        lcd_gotoxy(0,i);
        lcd_data(displayBuffer[i], sizeof(displayBuffer[i]));
    }
#elif defined TEXTMODE
    uint8_t displayBuffer[DISPLAY_WIDTH];
    memset(displayBuffer, 0x00, sizeof(displayBuffer));
    for (uint8_t i = 0; i < DISPLAY_HEIGHT/8; i++){
        lcd_gotoxy(0,i);
        lcd_data(displayBuffer, sizeof(displayBuffer));
    }
#endif
    lcd_home();
}
void lcd_home(void){
    lcd_gotoxy(0, 0);
}
void lcd_invert(uint8_t invert){
    uint8_t commandSequence[1];
    if (invert != YES) {
        commandSequence[0] = 0xA6;
    } else {
        commandSequence[0] = 0xA7;
    }
    lcd_command(commandSequence, 1);
}
void lcd_sleep(uint8_t sleep){
    uint8_t commandSequence[1];
    if (sleep != YES) {
        commandSequence[0] = 0xAF;
    } else {
        commandSequence[0] = 0xAE;
    }
    lcd_command(commandSequence, 1);
}
void lcd_set_contrast(uint8_t contrast){
    uint8_t commandSequence[2] = {0x81, contrast};
    lcd_command(commandSequence, sizeof(commandSequence));
}
void lcd_putc(char c){
    switch (c) {
        case '\b':
            // backspace
            lcd_gotoxy(cursorPosition.x-charMode, cursorPosition.y);
            lcd_putc(' ');
            lcd_gotoxy(cursorPosition.x-charMode, cursorPosition.y);
            break;
        case '\t':
            // tab
            if( (cursorPosition.x+charMode*4) < (DISPLAY_WIDTH/ sizeof(FONT[0])-charMode*4) ){
                lcd_gotoxy(cursorPosition.x+charMode*4, cursorPosition.y);
            }else{
                lcd_gotoxy(DISPLAY_WIDTH/ sizeof(FONT[0]), cursorPosition.y);
            }
            break;
        case '\n':
            // linefeed
            if(cursorPosition.y < (DISPLAY_HEIGHT/8-1)){
                lcd_gotoxy(cursorPosition.x, cursorPosition.y+charMode);
            }
            break;
        case '\r':
            // carrige return
            lcd_gotoxy(0, cursorPosition.y);
            break;
        default:
            // char doesn't fit in line
            if( (cursorPosition.x >= DISPLAY_WIDTH-sizeof(FONT[0])) || (c < ' ') ) break;
            // mapping char
            c -= ' ';
            if (c >= pgm_read_byte(&special_char[0][1]) ) {
                char temp = c;
                c = 0xff;
                for (uint8_t i=0; pgm_read_byte(&special_char[i][1]) != 0xff; i++) {
                    if ( pgm_read_byte(&special_char[i][0])-' ' == temp ) {
                        c = pgm_read_byte(&special_char[i][1]);
                        break;
                    }
                }
                if ( c == 0xff ) break;
            }
            // print char at display
#ifdef GRAPHICMODE
            if (charMode == DOUBLESIZE) {
                uint16_t doubleChar[sizeof(FONT[0])];
                uint8_t dChar;
                if ((cursorPosition.x+2*sizeof(FONT[0]))>DISPLAY_WIDTH) break;
                
                for (uint8_t i=0; i < sizeof(FONT[0]); i++) {
                    doubleChar[i] = 0;
                    dChar = pgm_read_byte(&(FONT[(uint8_t)c][i]));
                    for (uint8_t j=0; j<8; j++) {
                        if ((dChar & (1 << j))) {
                            doubleChar[i] |= (1 << (j*2));
                            doubleChar[i] |= (1 << ((j*2)+1));
                        }
                    }
                }
                for (uint8_t i = 0; i < sizeof(FONT[0]); i++)
                {
                    // load bit-pattern from flash
                    displayBuffer[cursorPosition.y+1][cursorPosition.x+(2*i)] = doubleChar[i] >> 8;
                    displayBuffer[cursorPosition.y+1][cursorPosition.x+(2*i)+1] = doubleChar[i] >> 8;
                    displayBuffer[cursorPosition.y][cursorPosition.x+(2*i)] = doubleChar[i] & 0xff;
                    displayBuffer[cursorPosition.y][cursorPosition.x+(2*i)+1] = doubleChar[i] & 0xff;
                }
                cursorPosition.x += sizeof(FONT[0])*2;
            } else {
            	if ((cursorPosition.x+sizeof(FONT[0]))>DISPLAY_WIDTH) break;
            	
                for (uint8_t i = 0; i < sizeof(FONT[0]); i++)
                {
                    // load bit-pattern from flash
                    displayBuffer[cursorPosition.y][cursorPosition.x+i] =pgm_read_byte(&(FONT[(uint8_t)c][i]));
                }
                cursorPosition.x += sizeof(FONT[0]);
            }
#elif defined TEXTMODE
            if (charMode == DOUBLESIZE) {
                uint16_t doubleChar[sizeof(FONT[0])];
                uint8_t dChar;
                if ((cursorPosition.x+2*sizeof(FONT[0]))>DISPLAY_WIDTH) break;
                
                for (uint8_t i=0; i < sizeof(FONT[0]); i++) {
                    doubleChar[i] = 0;
                    dChar = pgm_read_byte(&(FONT[(uint8_t)c][i]));
                    for (uint8_t j=0; j<8; j++) {
                        if ((dChar & (1 << j))) {
                            doubleChar[i] |= (1 << (j*2));
                            doubleChar[i] |= (1 << ((j*2)+1));
                        }
                    }
                }
                uint8_t data[sizeof(FONT[0])*2];
                for (uint8_t i = 0; i < sizeof(FONT[0]); i++)
                {
                    // print font to ram, print 6 columns
                    data[i<<1]=(doubleChar[i] & 0xff);
                    data[(i<<1)+1]=(doubleChar[i] & 0xff);
                }
                lcd_data(data, sizeof(FONT[0])*2);
                
#if defined (SSD1306) || defined (SSD1309)
                uint8_t commandSequence[] = {0xb0+cursorPosition.y+1,
                    0x21,
                    cursorPosition.x,
                    0x7f};
#elif defined SH1106
                uint8_t commandSequence[] = {0xb0+cursorPosition.y+1,
                    0x21,
                    0x00+((2+cursorPosition.x) & (0x0f)),
                    0x10+( ((2+cursorPosition.x) & (0xf0)) >> 4 ),
                    0x7f};
#endif
                lcd_command(commandSequence, sizeof(commandSequence));
                
                for (uint8_t i = 0; i < sizeof(FONT[0]); i++)
                {
                    // print font to ram, print 6 columns
                    data[i<<1]=(doubleChar[i] >> 8);
                    data[(i<<1)+1]=(doubleChar[i] >> 8);
                }
                lcd_data(data, sizeof(FONT[0])*2);
                
                commandSequence[0] = 0xb0+cursorPosition.y;
#if defined (SSD1306) || defined (SSD1309)
                commandSequence[2] = cursorPosition.x+(2*sizeof(FONT[0]));
#elif defined SH1106
                commandSequence[2] = 0x00+((2+cursorPosition.x+(2*sizeof(FONT[0]))) & (0x0f));
                commandSequence[3] = 0x10+( ((2+cursorPosition.x+(2*sizeof(FONT[0]))) & (0xf0)) >> 4 );
#endif
                lcd_command(commandSequence, sizeof(commandSequence));
                cursorPosition.x += sizeof(FONT[0])*2;
            } else {
                uint8_t data[sizeof(FONT[0])];
                if ((cursorPosition.x+sizeof(FONT[0]))>DISPLAY_WIDTH) break;
                
            	for (uint8_t i = 0; i < sizeof(FONT[0]); i++)
                {
                    // print font to ram, print 6 columns
                    data[i]=(pgm_read_byte(&(FONT[(uint8_t)c][i])));
                }
                lcd_data(data, sizeof(FONT[0]));
                cursorPosition.x += sizeof(FONT[0]);
            }
#endif
            break;
    }
    
}
void lcd_charMode(uint8_t mode){
    charMode = mode;
}
void lcd_flip(uint8_t flipping){
	uint8_t command[2] = {0xC8, 0xA1};
	switch(flipping){
		case 0:
			// normal mode default at init (needs to be reload data to display)
			command[0] = 0xC8;
			command[1] = 0xA1;
			lcd_command(command, sizeof(command));
			break;
		case 1:
			// flip horizontal && vertical (needs to be reload data to display)
			command[0] = 0xC0;
			command[1] = 0xA0;
			lcd_command(command, sizeof(command));
			break;
		case 2:
			// flip vertical (immediate without reload data to display)
			command[0] = 0xC0;
			lcd_command(command, sizeof(command));
			break;
		case 3:
			// flip horizontal (needs to be reload data to display)
			command[1] = 0xA0;
			lcd_command(command, sizeof(command));
		default:
			// do nothing
			break;
	}
}
void lcd_puts(const char* s){
    while (*s) {
        lcd_putc(*s++);
    }
}
void lcd_puts_p(const char* progmem_s){
    register uint8_t c;
    while ((c = pgm_read_byte(progmem_s++))) {
        lcd_putc(c);
    }
}
#ifdef GRAPHICMODE
#pragma mark -
#pragma mark GRAPHIC FUNCTIONS
uint8_t lcd_drawPixel(uint8_t x, uint8_t y, uint8_t color){
    if( x > DISPLAY_WIDTH-1 || y > (DISPLAY_HEIGHT-1)) return 1; // out of Display
    
    if( color == WHITE){
        displayBuffer[(y / 8)][x] |= (1 << (y % 8));
    } else {
        displayBuffer[(y / 8)][x] &= ~(1 << (y % 8));
    }
    
    return 0;
}
uint8_t lcd_drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color){
	uint8_t result;
	
    int dx =  abs(x2-x1), sx = x1<x2 ? 1 : -1;
    int dy = -abs(y2-y1), sy = y1<y2 ? 1 : -1;
    int err = dx+dy, e2; /* error value e_xy */
    
    while(1){
        result = lcd_drawPixel(x1, y1, color);
        if (x1==x2 && y1==y2) break;
        e2 = 2*err;
        if (e2 > dy) { err += dy; x1 += sx; } /* e_xy+e_x > 0 */
        if (e2 < dx) { err += dx; y1 += sy; } /* e_xy+e_y < 0 */
    }
    
    return result;
}
uint8_t lcd_drawRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color){
    uint8_t result;
    
    result = lcd_drawLine(px1, py1, px2, py1, color);
    result = lcd_drawLine(px2, py1, px2, py2, color);
    result = lcd_drawLine(px2, py2, px1, py2, color);
    result = lcd_drawLine(px1, py2, px1, py1, color);
    
    return result;
}
uint8_t lcd_fillRect(uint8_t px1, uint8_t py1, uint8_t px2, uint8_t py2, uint8_t color){
    uint8_t result;
    
    if( px1 > px2){
        uint8_t temp = px1;
        px1 = px2;
        px2 = temp;
        temp = py1;
        py1 = py2;
        py2 = temp;
    }
    for (uint8_t i=0; i<=(py2-py1); i++){
        result = lcd_drawLine(px1, py1+i, px2, py1+i, color);
    }
    
    return result;
}
uint8_t lcd_drawCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color){
    uint8_t result;
    
    int16_t f = 1 - radius;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;
    
    result = lcd_drawPixel(center_x  , center_y+radius, color);
    result = lcd_drawPixel(center_x  , center_y-radius, color);
    result = lcd_drawPixel(center_x+radius, center_y  , color);
    result = lcd_drawPixel(center_x-radius, center_y  , color);
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        result = lcd_drawPixel(center_x + x, center_y + y, color);
        result = lcd_drawPixel(center_x - x, center_y + y, color);
        result = lcd_drawPixel(center_x + x, center_y - y, color);
        result = lcd_drawPixel(center_x - x, center_y - y, color);
        result = lcd_drawPixel(center_x + y, center_y + x, color);
        result = lcd_drawPixel(center_x - y, center_y + x, color);
        result = lcd_drawPixel(center_x + y, center_y - x, color);
        result = lcd_drawPixel(center_x - y, center_y - x, color);
    }
    return result;
}
uint8_t lcd_fillCircle(uint8_t center_x, uint8_t center_y, uint8_t radius, uint8_t color) {
    uint8_t result;
    for(uint8_t i=0; i<= radius;i++){
        result = lcd_drawCircle(center_x, center_y, i, color);
    }
    return result;
}
uint8_t lcd_drawBitmap(uint8_t x, uint8_t y, const uint8_t *picture, uint8_t width, uint8_t height, uint8_t color){
    uint8_t result,i,j, byteWidth = (width+7)/8;
    for (j = 0; j < height; j++) {
        for(i=0; i < width;i++){
            if(pgm_read_byte(picture + j * byteWidth + i / 8) & (128 >> (i & 7))){
                result = lcd_drawPixel(x+i, y+j, color);
            } else {
                result = lcd_drawPixel(x+i, y+j, !color);
            }
        }
    }
    return result;
}
void lcd_display() {
#if defined (SSD1306) || defined (SSD1309)
    lcd_gotoxy(0,0);
    lcd_data(&displayBuffer[0][0], DISPLAY_WIDTH*DISPLAY_HEIGHT/8);
#elif defined SH1106
    for (uint8_t i = 0; i < DISPLAY_HEIGHT/8; i++){
        lcd_gotoxy(0,i);
        lcd_data(displayBuffer[i], sizeof(displayBuffer[i]));
    }
#endif
}
void lcd_clear_buffer() {
    for (uint8_t i = 0; i < DISPLAY_HEIGHT/8; i++){
        memset(displayBuffer[i], 0x00, sizeof(displayBuffer[i]));
    }
}
uint8_t lcd_check_buffer(uint8_t x, uint8_t y) {
    if( x > DISPLAY_WIDTH-1 || y > (DISPLAY_HEIGHT-1)) return 0; // out of Display
    return displayBuffer[(y / (DISPLAY_HEIGHT/8))][x] & (1 << (y % (DISPLAY_HEIGHT/8)));
}
void lcd_display_block(uint8_t x, uint8_t line, uint8_t width) {
    if (line > (DISPLAY_HEIGHT/8-1) || x > DISPLAY_WIDTH - 1){return;}
    if (x + width > DISPLAY_WIDTH) { // no -1 here, x alone is width 1
        width = DISPLAY_WIDTH - x;
    }
    lcd_goto_xpix_y(x,line);
    lcd_data(&displayBuffer[line][x], width);
}
#endif
