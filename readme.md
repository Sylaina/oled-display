# OLED for AVR mikrocontrollers
Library for oled-displays with SSD1306, SSD1309 or SH1106 display-controller connected with I2C or SPI at an AVR Atmel Atmega like Atmega328P.

<img src="https://github.com/Sylaina/oled-display/blob/master/oled.jpg?raw=true" width="500">

This library allows you to display text or/and graphic at oled-display.
The library need less than 2 kilobytes flash-memory and 3 bytes sram in textmode, in graphicmode library need less than 3 kilobytes flash-memory and 1027 bytes static sram so you can use oled-displays e.g with Atmega48PA (only with textmode).
Library is only tested with 128x64 Pixel display, lower resolution not tested but should work too.

If you want to use your own I2C library you have to fit i2c-function at lcd-library.
Settings for I2C-bus have to set at i2c.h
Settings for display have to set at lcd.h

If you want to use characters like e.g. ä set your compiler input-charset to utf-8 and your compiler exec-charset to iso-8859-15 (look at makefile line 115).

Testcondition: Display: SSD1306 OLED, Compiler Optimizelevel: -Os, µC: Atmega328p @ 8 MHz internal RC

Memory:
<table>
  <tr>
    <th>Modul</th>
    <th>Flash</th>
    <th>Static RAM</th>
  </tr>
  <tr>
    <td>I2C-Core</td>
    <td>220 Bytes</td>
    <td>0 Bytes</td>
  </tr>
  <tr>
    <td>FONT</td>
    <td>660 Bytes</td>
    <td>0 Bytes</td>
  </tr>
  <tr>
    <td>OLED (Text-Mode)</td>
    <td>1437 Bytes</td>
    <td>2 Bytes</td>
  </tr>
  <tr>
    <td>OLED (Graphic-Mode)</td>
    <td>2560 Bytes</td>
    <td>1026 Bytes</td>
  </tr>
 </table>
  
  

Speed (print 20 charaters (1 line) in normal size to display):

<table>
  <tr>
    <th>Mode</th>
    <th>Time</th>
    <th>I2C-Speed</th>
  </tr>
  <tr>
    <td>OLED (Text-Mode)</td>
    <td>4.411 ms</td>
    <td>400 kHz</td>
  </tr>
  <tr>
    <td>OLED (Text-Mode)</td>
    <td>15.384 ms</td>
    <td>100 kHz</td>
  </tr>
  <tr>
    <td>OLED (Graphic-Mode)</td>
    <td>26.603 ms</td>
    <td>400 kHz</td>
  </tr>
  <tr>
    <td>OLED (Graphic-Mode)</td>
    <td>96.294 ms</td>
    <td>100 kHz</td>
  </tr>
 </table>


example:

```c
//****main.c****//
#include "lcd.h"


int main(void){
  lcd_init(LCD_DISP_ON);    // init lcd and turn on
  
  lcd_puts("Hello World");  // put string from RAM to display (TEXTMODE) or buffer (GRAPHICMODE)
  lcd_gotoxy(0,2);          // set cursor to first column at line 3
  lcd_puts_p(PSTR("String from flash"));  // puts string form flash to display (TEXTMODE) or buffer (GRAPHICMODE)
#if defined GRAPHICMODE
  lcd_drawCircle(64,32,7,WHITE); // draw circle to buffer
  lcd_display();                  // send buffer to display
#endif
  for(;;){
    //main loop
  }
  return 0;
}
```
example for chars with double height:
```c
//****main.c****//
#include "lcd.h"

int main(void){
    
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_set_contrast(0x00);
    lcd_gotoxy(4,1);
    lcd_puts("Normal Size");
    lcd_charMode(DOUBLESIZE);
    lcd_gotoxy(0,4);
    lcd_puts("  Double  \r\n   Size");
    lcd_charMode(NORMALSIZE);
        
#ifdef GRAPHICMODE
        lcd_display();
#endif
    for(;;){
    //main loop
    }   
    return 0;
}
```

<img src="https://github.com/Sylaina/oled-display/blob/master/bigchars.JPG?raw=true" width="500">
