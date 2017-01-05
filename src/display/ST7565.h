#include "Arduino.h"
#include "Adafruit_GFX.h"

#ifndef _delay_ms
#define _delay_ms(t) delay(t)
#endif

#ifndef _BV
#define _BV(bit) (1<<(bit))
#endif

#define swap(a, b) { uint16_t t = a; a = b; b = t; }

#define BLACK       1
#define WHITE       0
#define INVERSE     2

#define LCDWIDTH    128
#define LCDHEIGHT   64

#define CMD_DISPLAY_OFF         0xAE
#define CMD_DISPLAY_ON          0xAF

#define CMD_SET_DISP_START_LINE 0x40
#define CMD_SET_PAGE            0xB0

#define CMD_SET_COLUMN_UPPER    0x10
#define CMD_SET_COLUMN_LOWER    0x00

#define CMD_SET_ADC_NORMAL      0xA0
#define CMD_SET_ADC_REVERSE     0xA1
#define CMD_SET_BIAS_9          0xA2
#define CMD_SET_BIAS_7          0xA3
#define CMD_SET_ALLPTS_NORMAL   0xA4
#define CMD_SET_ALLPTS_ON       0xA5
#define CMD_SET_DISP_NORMAL     0xA6
#define CMD_SET_DISP_REVERSE    0xA7

#define CMD_RMW                 0xE0
#define CMD_RMW_CLEAR           0xEE

#define CMD_INTERNAL_RESET      0xE2
#define CMD_SET_COM_NORMAL      0xC0
#define CMD_SET_COM_REVERSE     0xC8
#define CMD_SET_POWER_CONTROL   0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_CONTRAST        0x81

#define CMD_SET_STATIC_OFF      0xAC
#define CMD_SET_STATIC_ON       0xAD
#define CMD_SET_STATIC_REG      0x00

#define CMD_SET_BOOSTER_FIRST   0xF8
#define CMD_SET_BOOSTER_234     0x00
#define CMD_SET_BOOSTER_5       0x01
#define CMD_SET_BOOSTER_6       0x03
#define CMD_NOP                 0xE3
#define CMD_TEST                0xF0

// a handy reference to where the pages are on the screen
const uint8_t pagemap[] = { 7, 6, 5, 4, 3, 2, 1, 0 };

// a 5x7 font table
const extern uint8_t font[];


class ST7565 :public Adafruit_GFX {
public:
    ST7565(int8_t SID, int8_t SCLK, int8_t A0, int8_t RST, int8_t CS);
    /* Adfruit virtual fuction */
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    
    void st7565_init(void);
    
    void begin(uint8_t contrast);
    void st7565_command(uint8_t c);
    void st7565_data(uint8_t c);
    void st7565_clear(void);
    
    void setpixel(uint8_t x, uint8_t y, uint8_t color);
    uint8_t getpixel(uint8_t x, uint8_t y);
    
    void clear();
    void update();
    
    void fast_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
    void fast_bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color);
    void fast_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
    void fast_rect_filled(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
    void fast_circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
    void fast_circle_filled(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
    
private:
    // the memory buffer for the LCD
    uint8_t lcd_buffer[LCDWIDTH * LCDHEIGHT / 8];
    
    int8_t sid, sclk, a0, rst, cs;
    void spiwrite(uint8_t c);
};
