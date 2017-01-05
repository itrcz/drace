#include "ST7565.h"


ST7565::ST7565(int8_t SID, int8_t SCLK, int8_t A0, int8_t RST, int8_t CS) : Adafruit_GFX(LCDWIDTH, LCDHEIGHT) {
    sid     = SID;
    sclk    = SCLK;
    a0      = A0;
    rst     = RST;
    cs      = CS;

}
/*
 Adfruit
*/
void ST7565::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
        return;
    
    // check rotation, move pixel around if necessary
    switch (getRotation()) {
        case 1:
            swap(x, y);
            x = WIDTH - x - 1;
            break;
        case 2:
            x = WIDTH - x - 1;
            y = HEIGHT - y - 1;
            break;
        case 3:
            swap(x, y);
            y = HEIGHT - y - 1;
            break;
    }
   
    // x is which column
    switch (color)
    {
        case BLACK:
            //lcd_buffer[x+ (y/8)*LCDWIDTH] |=  (1 << (y&7));
            lcd_buffer[x+ (y/8)*LCDWIDTH] |= _BV(7-(y%8));
            break;
        case WHITE:
            //lcd_buffer[x+ (y/8)*LCDWIDTH] &= ~(1 << (y&7));
            lcd_buffer[x+ (y/8)*LCDWIDTH] &= ~_BV(7-(y%8));
            break;
        case INVERSE:
            //lcd_buffer[x+ (y/8)*LCDWIDTH] ^=  (1 << (y&7));
            lcd_buffer[x+ (y/8)*LCDWIDTH] ^= _BV(7-(y%8));
            break;
    }
}

// the most basic function, set a single pixel
void ST7565::setpixel(uint8_t x, uint8_t y, uint8_t color) {
    if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
        return;
    
    // x is which column
    if (color)
        lcd_buffer[x+ (y/8)*LCDWIDTH] |= _BV(7-(y%8));
    else
        lcd_buffer[x+ (y/8)*LCDWIDTH] &= ~_BV(7-(y%8));
    
}


// the most basic function, get a single pixel
uint8_t ST7565::getpixel(uint8_t x, uint8_t y) {
    if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
        return 0;
    
    return (lcd_buffer[x+ (y/8)*LCDWIDTH] >> (7-(y%8))) & 0x1;
}

// clear everything
void ST7565::clear(void) {
    memset(lcd_buffer, 0, 1024);
}

void ST7565::fast_bitmap(uint8_t x, uint8_t y, const uint8_t *bitmap, uint8_t w, uint8_t h, uint8_t color) {
    for (uint8_t j=0; j<h; j++) {
        for (uint8_t i=0; i<w; i++ ) {
            if ( bitmap[i + (j/8)*w] & _BV(j%8)) {
                setpixel(x+j, y+i, color);
            }
        }
    }
}


// bresenham's algorithm - thx wikpedia
void ST7565::fast_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color) {
    uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    uint8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    
    int8_t err = dx / 2;
    int8_t ystep;
    
    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;}
    
    for (; x0<=x1; x0++) {
        if (steep) {
            setpixel(y0, x0, color);
        } else {
            setpixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}
// draw a rectangle
void ST7565::fast_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    // stupidest version - just pixels - but fast with internal buffer!
    for (uint8_t i=x; i<x+w; i++) {
        setpixel(i, y, color);
        setpixel(i, y+h-1, color);
    }
    for (uint8_t i=y; i<y+h; i++) {
        setpixel(x, i, color);
        setpixel(x+w-1, i, color);
    }
}
// filled rectangle
void ST7565::fast_rect_filled(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color) {
    // stupidest version - just pixels - but fast with internal buffer!
    for (uint8_t i=x; i<x+w; i++) {
        for (uint8_t j=y; j<y+h; j++) {
            setpixel(i, j, color);
        }
    }
}
// draw a circle outline
void ST7565::fast_circle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
    
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;
    
    setpixel(x0, y0+r, color);
    setpixel(x0, y0-r, color);
    setpixel(x0+r, y0, color);
    setpixel(x0-r, y0, color);
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        setpixel(x0 + x, y0 + y, color);
        setpixel(x0 - x, y0 + y, color);
        setpixel(x0 + x, y0 - y, color);
        setpixel(x0 - x, y0 - y, color);
        
        setpixel(x0 + y, y0 + x, color);
        setpixel(x0 - y, y0 + x, color);
        setpixel(x0 + y, y0 - x, color);
        setpixel(x0 - y, y0 - x, color);
    }
}

void ST7565::fast_circle_filled(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color) {
    
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;
    
    for (uint8_t i=y0-r; i<=y0+r; i++) {
        setpixel(x0, i, color);
    }
    
    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        for (uint8_t i=y0-y; i<=y0+y; i++) {
            setpixel(x0+x, i, color);
            setpixel(x0-x, i, color);
        }
        for (uint8_t i=y0-x; i<=y0+x; i++) {
            setpixel(x0+y, i, color);
            setpixel(x0-y, i, color);
        }
    }
}



void ST7565::st7565_init(void) {
    
    // set pin directions
    pinMode(sid,    OUTPUT);
    pinMode(sclk,   OUTPUT);
    pinMode(a0,     OUTPUT);
    pinMode(rst,    OUTPUT);
    pinMode(cs,     OUTPUT);
    
    digitalWrite(cs,    LOW);
    digitalWrite(rst,   LOW);
    _delay_ms(500);
    digitalWrite(rst,   HIGH);
    
    st7565_command(CMD_SET_DISP_START_LINE);
    st7565_command(CMD_SET_ADC_NORMAL);
    st7565_command(CMD_SET_COM_NORMAL);
    st7565_command(CMD_SET_DISP_NORMAL);
    st7565_command(CMD_SET_BIAS_9);
    st7565_command(0x2F);		                /* all power  control circuits on */
    st7565_command(0x27);		                /* set V0 voltage resistor ratio 0x20-0x27*/
    st7565_command(CMD_SET_BOOSTER_FIRST);      /* set booster ratio to */
    st7565_command(0x00);		                /* 4x */
    
    st7565_command(CMD_SET_CONTRAST);           /* set contrast */
    st7565_command(0x00);

    st7565_command(CMD_SET_STATIC_OFF);
    st7565_command(0x00);
    
    st7565_command(CMD_DISPLAY_ON);
    //_delay_ms(100);
    st7565_command(CMD_SET_ALLPTS_NORMAL);
}

inline void ST7565::spiwrite(uint8_t c) {
    int8_t i;
    for (i=7; i>=0; i--) {
        digitalWrite(sclk, LOW);
        if (c & _BV(i)) {
            digitalWrite(sid, HIGH);
        } else {
            digitalWrite(sid, LOW);
        }
        digitalWrite(sclk, HIGH);
    }
}
void ST7565::st7565_command(uint8_t c) {
    digitalWrite(a0, LOW);
    spiwrite(c);
}

void ST7565::st7565_data(uint8_t c) {
    digitalWrite(a0, HIGH);
    spiwrite(c);

}

void ST7565::update(void) {
    uint8_t col, maxcol, p;
    
    for(p = 0; p < 8; p++) {
        st7565_command(CMD_SET_PAGE | pagemap[p]);
        
        // start at the beginning of the row
        col = 0;
        maxcol = LCDWIDTH-1;
        
        st7565_command(CMD_SET_COLUMN_LOWER | (col & 0xf));
        st7565_command(CMD_SET_COLUMN_UPPER | ((col >> 4) & 0x0F));
        st7565_command(CMD_RMW);
        
        for(; col <= maxcol; col++) {
            st7565_data(lcd_buffer[(LCDWIDTH*p)+col]);
        }
    }
}

// this doesnt touch the buffer, just clears the display RAM - might be handy
void ST7565::st7565_clear(void) {
    uint8_t p, c;
    for(p = 0; p < 8; p++) {
        st7565_command(CMD_SET_PAGE | p);
        for(c = 0; c < 129; c++) {
            st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
            st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
            st7565_data(0x0);
        }
    }
}
