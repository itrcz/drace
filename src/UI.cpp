#include "UI.h"

bool Button::pressed()
{
    uint32_t now = millis();
    uint8_t current_state = digitalRead(_port);
    
    //Если предидущее выполнение не установило время
    if (_lastTime == 0) {
        //если кнопка нажана, ставим текущее время
        if (current_state == 0x0) {
            _lastTime = now;
        }
        return false;
    }
    //если кнопка не нажата
    if (current_state) {
        _lastTime = 0;
        _pressCount = 0;
        return false;
    }
    
    //Если кнопка все еще нажаша
    if(now - _lastTime >= BUTTON_DEBONCE_TIME) {
        if (_pressCount == 0) {
            _pressCount++;
            return true;
        }
        uint16_t off = 128 * (5-_pressCount);
   
        if (_pressCount < 4) {
            if (now - _lastTime >= off) {
                _lastTime += off;
                _pressCount++;
                return true;
            }
            return false;
        }
        if (now - _lastTime >= off) {
            _lastTime += off;
            return true;
        }
        return false;
    }
    return false;
}

Button ButtonMenu;
Button ButtonRight;
Button ButtonLeft;
Button ButtonReset;

UI ui;

UI::UI() : LCD(LCD_SDA, LCD_SCK, LCD_A0, LCD_RST, LCD_CS) {
    
}
static struct {
    int8_t  sat             = -1;
    int16_t speed           = -1;
    int8_t  results_offset  = 0;
    
} screen_var;

void UI::Init( void ) {
    
    LCD.st7565_init();
    LCD.clear();
    
    this->PageLoad(page_splash);
    
    
    pinMode(LCD_BACKLIGHT, OUTPUT);
    
    for(uint8_t i = 0; i<254; i++) {
        analogWrite(LCD_BACKLIGHT, i);
        delay(2);
    }
    
    digitalWrite(LCD_BACKLIGHT, HIGH);
    
    ButtonMenu.setup(BUTTON_PORT_MENU);
    ButtonRight.setup(BUTTON_PORT_RIGHT);
    ButtonLeft.setup(BUTTON_PORT_LEFT);
    ButtonReset.setup(BUTTON_PORT_RESET);
};

/*
 Page core functions
 
 */

void UI::PageUnload( void ) {
    switch (currentPage) {
        case page_splash:
            this->Splash_unload();
            break;
        case page_menu:
            this->Menu_unload();
            break;
        case page_accel:
            this->Accel_unload();
            break;
        case page_debug:
            this->Debug_unload();
            break;
    }
    currentPage = -1;
}
void UI::PageLoad(uint8_t pageId) {
    if (currentPage >= 0) {
        this->PageUnload();
    }
    currentPage = pageId;
    
    switch (currentPage) {
        case page_splash:
            this->Splash_load();
            break;
        case page_menu:
            this->Menu_load();
            break;
        case page_accel:
            this->Accel_load();
            break;
        case page_debug:
            this->Debug_load();
            break;
    }
}
/*
 Page functions
 Boot
 */
void UI::Splash_load(void) {
    LCD.clear();
    LCD.drawXBitmap(0, 0, boot_img_bits, boot_img_width, boot_img_height, BLACK);
    LCD.update();
}
void UI::Splash_unload(void) {}
void UI::Splash_update(void) {}


/*
 Page functions
 MainMenu
 */

#define Menu_length 2
#define Menu_MAXONSCREEN 5
static struct {
    char * title[Menu_length] = { "Acceleration", "Debug" };
    int8_t current = 0;
} Menu;
enum menu_items {
    menu_acceleration,
    menu_debug,
};
void UI::Menu_select( void ) {
    switch (Menu.current) {
        case menu_acceleration:
            this->PageLoad(page_accel);
            break;
        case menu_debug:
            this->PageLoad(page_debug);
            break;
    }
}

void UI::Menu_load(void) {
    LCD.clear();
    LCD.setFont(&ThinPixel10pt7b);
    LCD.fast_rect(0, 0, 128, 64, BLACK);
    
    this->Menu_update();
    
    currentPage = 1;
}
void UI::Menu_unload(void) {
    
}
void UI::Menu_update(void) {
    
    LCD.fast_rect_filled(1, 1, 126, 62, WHITE);
    
    int offset = 0;
    
    if (Menu.current > (Menu_MAXONSCREEN - 1)) {
        offset = Menu.current + 1 - Menu_MAXONSCREEN;
    }
    for(int i = 0; i < Menu_length; i++) {
        LCD.setCursor(5, 12*i+10);
        if (Menu.current == i+offset) {
            LCD.fast_rect_filled(1, 2 + ((Menu.current-offset)*12), 126, 12, BLACK);
            
            LCD.setTextColor(INVERSE);
            LCD.print(Menu.title[i+offset]);
        } else {
            LCD.setTextColor(BLACK);
            LCD.print(Menu.title[i+offset]);
        }
    }
    LCD.update();
}
void UI::Menu_next( void ) {
    Menu.current++;
    if (Menu.current >= Menu_length) {
        Menu.current = 0;
    }
    Menu_update();
}
void UI::Menu_prev( void ) {
    Menu.current--;
    if (Menu.current < 0) {
        Menu.current = Menu_length -1;
    }
    Menu_update();
}

/*
 Page functions
 Acceleration
 */

void UI::Accel_load() {
    LCD.clear();
    
    //Рамка заголовка
    LCD.fast_rect_filled(0, 0, 128, 13, BLACK);
    
    //Рамка скорости
    LCD.fast_rect(0, 11, 128, 42, BLACK);
    //Закраска км/ч
    LCD.fast_rect(2, 14, 48, 37, BLACK);
    LCD.fast_rect_filled(2, 38, 48, 13, BLACK);
    
    LCD.setFont(&ThinPixel10pt7b);
    LCD.setTextColor(BLACK);
    
    //Таблица результатов
    LCD.fast_rect(49, 14, 73, 37, BLACK);
    LCD.fast_line(50, 26, 121, 26, BLACK);
    LCD.fast_line(50, 38, 121, 38, BLACK);
    LCD.fast_line(50, 50, 121, 50, BLACK);
    
    LCD.setTextColor(INVERSE);
    //Надпись км/ч
    LCD.setCursor(9,47);
    LCD.print(" km/h");
    
    //Заголовок
    LCD.setCursor( (128-12*6-1)/2 , 9);
    LCD.print("ACCELERATION");
    
    //Иконка спутника
    LCD.drawXBitmap(0, 54, icon_gps_bits, icon_gps_width, icon_gps_height, BLACK);
    
    this->Accel_update_results_table(false);
    
    LCD.update();
    
    screen_var.sat      = -1;
    screen_var.speed    = -1;
}
void UI::Accel_unload() {
    
}

void UI::Accel_update() {

    LCD.update();
}


void UI::Accel_update_sat( void ) {
    if (currentPage != page_accel) {
        return;
    }
    uint16_t sat = gps.sat;
    
    Accel_update_sat_message();
    
    if (screen_var.sat == sat) return;
    screen_var.sat = sat;
    
    LCD.setTextColor(BLACK);
    LCD.fast_rect_filled(10, 54, 14, 10, WHITE);
    LCD.setFont(&ThinPixel10pt7b);
    //Количество спутников
    LCD.setCursor(12, 61);
    LCD.print( screen_var.sat );
    
    this->Accel_update();
}


void UI::Accel_update_sat_message() {
    
    if (currentPage != page_accel) {
        return;
    }
    if (gps.sat >= 4) return;
    
    if (millis() - timer_HALF_SEC_CHANGE > 250) {
        
        LCD.setFont(&ThinPixel10pt7b);
        
        char text[] = "wait for gps...";
        uint8_t text_width = 72;
        
        LCD.fast_rect_filled(128 - text_width - 4, 54, text_width + 4, 11, WHITE);
        
        if (gps.sat >= 4) return;
        
        if (HALF_SEC_CHANGE) {
            LCD.setTextColor(WHITE);
        } else {
            LCD.setTextColor(BLACK);
        }
        
        LCD.setCursor(128 - text_width, 60);
        
        LCD.print(text);
        
        LCD.update();
    }
}
void UI::Accel_update_race_message( bool clear ) {
    if (currentPage != page_accel) {
        return;
    }
    if (clear) {
        LCD.fast_rect_filled(40, 54, 88, 11, WHITE);
        LCD.update();
        return;
    }
 
    LCD.drawXBitmap(90, 54, warning_bits, warning_width, warning_height, BLACK);
    
    LCD.setFont(&ThinPixel10pt7b);
    
    char text[] = "race";
    uint8_t text_width = 24;
    
    LCD.fast_rect_filled(128 - text_width - 4, 54, text_width + 4, 11, BLACK);
    LCD.setTextColor(WHITE);
    
    LCD.setCursor(128 - text_width, 61);
    
    LCD.print(text);
    
    LCD.update();
    
}
void UI::Accel_update_speed( void ) {
    if (currentPage != page_accel) {
        return;
    }
    uint16_t spd = gps.getSpeed();
    if (screen_var.speed == spd) return;
    screen_var.speed = spd;
    
    uint8_t spd_x_offset = 21;
    
    if (screen_var.speed > 99) {
        spd_x_offset = 9;
    } else if (screen_var.speed > 9) {
        spd_x_offset = 14;
    }
    LCD.setTextColor(BLACK);
    LCD.setFont(&Minecraftia8pt7b);
    LCD.setCursor(spd_x_offset,42);
    LCD.fast_rect_filled(9, 19, 34, 14, WHITE);
    LCD.print( screen_var.speed );
    
    this->Accel_update();
}

void UI::Accel_update_results_table(bool update) {
    
    LCD.setFont(&ThinPixel10pt7b);
    LCD.setTextColor(BLACK);
    
    //Scroll
    LCD.fast_rect_filled(123, 15, 3, 35, WHITE);
    LCD.fast_line(124, 15, 124, 49, BLACK);
    
    LCD.fast_rect_filled(123, 15 + screen_var.results_offset*5, 3, 5, BLACK);
    
    for(uint8_t i=1;i<4;i++) {
        LCD.fast_rect_filled(53, 5 + 12 * i, 65, 7, WHITE);
        
        LCD.setCursor(54,11 + 12 * i);
        LCD.print("0-");
        LCD.print(accel.table[i+screen_var.results_offset]);
        LCD.print(0);
        
        if (accel.table[i+screen_var.results_offset] < 10) {
            LCD.print(" ");
        }
        uint16_t timeResult = accel.GetResult(i+screen_var.results_offset);
        
        if (timeResult) {
            LCD.print(timeResult);
            LCD.print(" s");
        } else {
            LCD.print(" --");
        }
    }
    if (update) LCD.update();
}
void UI::Accel_results_table_switch(bool next) {
    
    if (next) {
        screen_var.results_offset++;
        if (screen_var.results_offset + 2 >= SPEED_RESULTS_TABLE_COUNT) {
            screen_var.results_offset = 0;
        }
    } else {
        screen_var.results_offset--;
        if (screen_var.results_offset < 0) {
            screen_var.results_offset = SPEED_RESULTS_TABLE_COUNT - 3;
        }
    }
    this->Accel_update_results_table(true);
}
void UI::Accel_reset( void ) {
    accel.Reset();
    Accel_update_results_table(true);
}
/*
 Page functions
 Debug mode
 */
void UI::Debug_load(void) {
    LCD.clear();
    
    LCD.setCursor(0,7);
    LCD.setFont(&ThinPixel10pt7b);
    LCD.print("GPS TEST MODE");
    LCD.update();
    
    gps.debug = true;
}
void UI::Debug_unload(void) {
    gps.debug = false;
    
}


void UI::buttonsHandler( void ) {
    switch (currentPage) {
        case page_menu:
            if (ButtonRight.pressed()) Menu_next();
            if (ButtonLeft.pressed())  Menu_prev();
            if (ButtonMenu.pressed())  Menu_select();
            if (ButtonReset.pressed()) {}
            break;
        case page_accel:
            if (ButtonRight.pressed()) this->Accel_results_table_switch(true);
            if (ButtonLeft.pressed())  this->Accel_results_table_switch(false);
            if (ButtonMenu.pressed())  this->PageLoad(page_menu);
            if (ButtonReset.pressed()) this->Accel_reset();
            break;
        case page_debug:
            if (ButtonRight.pressed()) {}//Menu_next();
            if (ButtonLeft.pressed())  {}//Menu_prev();
            if (ButtonMenu.pressed())  this->PageLoad(page_menu);
            if (ButtonReset.pressed()) {}
            break;
    }
}

void UI::Update( void ) {

    this->buttonsHandler();
    
    
    //boolSecond изменяет значения каждые пол секунды
    //Необходимо для переодических операций на экране
    if (!timer_HALF_SEC_CHANGE) {
        timer_HALF_SEC_CHANGE = millis();
    }
    if (millis() - timer_HALF_SEC_CHANGE > 500) {
        timer_HALF_SEC_CHANGE = millis();
        HALF_SEC_CHANGE = !HALF_SEC_CHANGE;
    }
};





