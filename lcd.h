/*
 * This file is part of lcd library for ssd1306/sh1106 oled-display.
 *
 * lcd library for ssd1306/sh1106 oled-display is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any later version.
 *
 * lcd library for ssd1306/sh1106 oled-display is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Diese Datei ist Teil von lcd library for ssd1306/sh1106 oled-display.
 *
 * lcd library for ssd1306/sh1106 oled-display ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder jeder späteren
 * veröffentlichten Version, weiterverbreiten und/oder modifizieren.
 *
 * lcd library for ssd1306/sh1106 oled-display wird in der Hoffnung, dass es nützlich sein wird, aber
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
 *  lib for OLED-Display with ssd1306/sh1106-Controller
 *  first dev-version only for I2C-Connection
 *  at ATMega328P like Arduino Uno
 ****************************************************
 *  For other Atmegas/Attinys: GFX-Lib needs a
 *  minimum of 1029 byte SRAM!
 ****************************************************
 *
 */

#ifndef LCD_H
#define LCD_H

#ifndef YES
#define YES				1
#define NO				0
#else
#error "Check #defines for YES and NO in other sources !"
#endif

#if (__GNUC__ * 100 + __GNUC_MINOR__) < 303
#error "This library requires AVR-GCC 3.3 or later, update to newer AVR-GCC compiler !"
#endif

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "i2c.h"

/* TODO: define displaycontroller */
#define SH1106	// or SSD1306, check datasheet of your display

#define LCD_I2C_ADDR    0x7A

#define LCD_DISP_OFF	0xAE
#define LCD_DISP_ON		0xAF

#define WHITE			0x01
#define BLACK			0x00

#define DISPLAY_WIDTH	128
#define DISPLAY_HEIGHT	64
#define DISPLAYSIZE		DISPLAY_WIDTH*DISPLAY_HEIGHT/8

void lcd_init(uint8_t dispAttr);
void lcd_home(void);

void lcd_command(uint8_t cmd[], uint8_t size);				// transmit command to display
void lcd_data(uint8_t data[], uint16_t size);				// transmit data to display
void lcd_gotoxy(uint8_t x, uint8_t y);		// set curser at pos x, y. x means character, y means line (page, refer lcd manual)
void lcd_clrscr(void);						// clear screen
void lcd_putc(char c);						// print character on screen
void lcd_puts(const char* s);				// print string, \n-terminated, from ram on screen
void lcd_puts_p(const char* progmem_s);		// print string from flash on screen
void lcd_invert(uint8_t invert);			// invert display
void lcd_set_contrast(uint8_t contrast);	// set contrast for display
#endif /*  LCD_H  */
