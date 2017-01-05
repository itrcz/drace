#include "ui.h"
#include "GPS.h"

#include "../graphics/boot_img.xbm"
#include "../graphics/icon_gps.xbm"

#include "display/ST7565.h"
#include "display/fonts/ThinPixel.h"
#include "display/fonts/Minecraftia.h"


// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)

#define LCD_CS          13
#define LCD_RST         12
#define LCD_A0          11
#define LCD_SDA         10
#define LCD_SCK         9
#define LCD_BACKLIGHT   8


ST7565 LCD(LCD_SDA, LCD_SCK, LCD_A0, LCD_RST, LCD_CS);


static int8_t UI_page = -1;

static uint8_t UI_buttons = 0b00000000;

static uint8_t UI_ACTIVE_MENU = 0;

#define MAX_ITEMS_ON_SCREEN 4
#define UI_MENU_COUNT 4

enum page_update_params {
    __PAGE_SET,
    __PAGE_UNSET,
    __PAGE_UPDATE
};

enum screen_pages {
    page_splash,
    page_menu,
    page_accel,
    
    page_service_acceldebug,
    page_service_gpstest,
};

static struct {
    uint8_t id = 1;
    char * items[UI_MENU_COUNT] = { "Acceleration", "Accel debug", "Gps testing mode", "Test" };
    bool active = false;
    int8_t current = 0;
} UI_MENU;


void UI_Init(void) {
    
    pinMode(UI_BUTTON_MENU, INPUT_PULLUP);
    pinMode(UI_BUTTON_RIGHT, INPUT_PULLUP);
    pinMode(UI_BUTTON_LEFT, INPUT_PULLUP);
    
    LCD.st7565_init();
    LCD.clear();
    
    pinMode(LCD_BACKLIGHT, OUTPUT);
    digitalWrite(LCD_BACKLIGHT, HIGH);
    
    Page_action(__PAGE_SET, page_splash);
}
void UI_load_lastpage(void) {
    Page_action(__PAGE_SET, page_accel);
}

void Page_action(int8_t a, int8_t page) {
    /*
     Перед загрузкой экрана 
     нужно освободить предидущий 
     */
    if (a == __PAGE_SET) {
        Page_action(__PAGE_UNSET, UI_page);
        UI_page = page;
    }
    
    if (page < 0) {
        return;
    }
    switch (page) {
        case page_splash:
            if (a == __PAGE_SET)    Page_splash_load();
            if (a == __PAGE_UNSET)  Page_splash_unload();
            if (a == __PAGE_UPDATE) Page_splash_update();
            break;
        case page_menu:
            if (a == __PAGE_SET)    Page_menu_load();
            if (a == __PAGE_UNSET)  Page_menu_unload();
            if (a == __PAGE_UPDATE) Page_menu_update();
            break;
        case page_accel:
            if (a == __PAGE_SET)    Page_accel_load();
            if (a == __PAGE_UNSET)  Page_accel_unload();
            if (a == __PAGE_UPDATE) Page_accel_update();
            break;
        case page_service_acceldebug:
            if (a == __PAGE_SET)    Page_service_acceldebug_load();
            if (a == __PAGE_UNSET)  Page_service_acceldebug_unload();
            if (a == __PAGE_UPDATE) Page_service_acceldebug_update();
            break;
        case page_service_gpstest:
            if (a == __PAGE_SET)    Page_service_gpstest_load();
            if (a == __PAGE_UNSET)  Page_service_gpstest_unload();
            if (a == __PAGE_UPDATE) Page_service_gpstest_update();
            break;
    }
    if (a == __PAGE_UNSET) {
        UI_page = -1;
    }
}

void Page_splash_load(void) {
    LCD.clear();
    //Иконка спутника
    LCD.drawXBitmap(0, 0, boot_img_bits, boot_img_width, boot_img_height, BLACK);
    
    LCD.update();
}
void Page_splash_unload(void) {}
void Page_splash_update(void) {}

void Page_accel_load() {
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
    
    Page_accel_update_results_table(false);
    
    Page_accel_update_sat();
    Page_accel_update_speed();
    
    LCD.update();
}

static uint8_t current_ui_sat = 0;
static uint16_t current_ui_speed = 0;

void Page_accel_update() {
    uint8_t sat = gps.sat;
    uint16_t spd = gps.getSpeed();
    
    bool upd = false;
    
    if (current_ui_sat != sat) {
        current_ui_sat = sat;
        Page_accel_update_sat();
        upd = true;
    }
    
    if (current_ui_speed != spd) {
        current_ui_speed = spd;
        Page_accel_update_speed();
        upd = true;
    }
    if (upd) LCD.update();

}

void Page_accel_update_sat( void ) {
    
    LCD.setTextColor(BLACK);
    LCD.fast_rect_filled(10, 54, 14, 10, WHITE);
    LCD.setFont(&ThinPixel10pt7b);
    //Количество спутников
    LCD.setCursor(12, 61);
    LCD.print( current_ui_sat );
}
void Page_accel_update_speed( void ) {
    uint8_t spd_x_offset = 21;
    
    if (current_ui_speed > 99) {
        spd_x_offset = 9;
    } else if (current_ui_speed > 9) {
        spd_x_offset = 14;
    }
    LCD.setTextColor(BLACK);
    LCD.setFont(&Minecraftia8pt7b);
    LCD.setCursor(spd_x_offset,42);
    LCD.fast_rect_filled(9, 19, 34, 14, WHITE);
    LCD.print( current_ui_speed );
}
void Page_accel_unload() {
    
}

#define SPEED_RESULTS_TABLE_COUNT 9

static struct speed_results {
    int8_t offset = 0;
    uint8_t table[10] = { 0,4,6,8,10,12,15,20,28,30 };
    
} speed_results;

void Page_accel_update_results_table(bool update) {

    LCD.setFont(&ThinPixel10pt7b);

    //Scroll
    LCD.fast_rect_filled(123, 15, 3, 35, WHITE);
    LCD.fast_line(124, 15, 124, 49, BLACK);
    
    LCD.fast_rect_filled(123, 15 + speed_results.offset*5, 3, 5, BLACK);
    
    for(uint8_t i=1;i<4;i++) {
        LCD.fast_rect_filled(53, 5 + 12 * i, 65, 7, WHITE);
        
        
        LCD.setCursor(54,11 + 12 * i);
        LCD.print("0-");
        LCD.print(speed_results.table[i+speed_results.offset]);
        LCD.print(0);
        if (speed_results.table[i+speed_results.offset] < 10) {
            LCD.print(" ");
        }
        LCD.print(" --");
    }
    if (update) LCD.update();
}
void Page_accel_results_table_switch(bool next) {
    
    if (next) {
        speed_results.offset++;
        if (speed_results.offset + 2 >= SPEED_RESULTS_TABLE_COUNT) {
            speed_results.offset = 0;
        }
    } else {
        speed_results.offset--;
        if (speed_results.offset < 0) {
            speed_results.offset = SPEED_RESULTS_TABLE_COUNT - 3;
        }
    }
    Page_accel_update_results_table(true);
}

/*
 SCREEN
 ID: 2
 Debug MainMenu
*/

void Page_menu_load(void) {
    LCD.clear();
    LCD.setFont(&ThinPixel10pt7b);
    LCD.fast_rect(0, 0, 128, 64, BLACK);
    
    Page_menu_need_update = true;
    
    Page_menu_update();
}

void Page_menu_unload(void) {
   
}

void Page_menu_update(void) {
    if (!Page_menu_need_update) return;
    
    Page_menu_need_update = false;
    
    if (UI_ACTIVE_MENU) {
        
        LCD.fast_rect_filled(1, 1, 126, 62, WHITE);
        
        int offset = 0;
        
        if (UI_MENU.current > (MAX_ITEMS_ON_SCREEN - 1)) {
            offset = UI_MENU.current + 1 - MAX_ITEMS_ON_SCREEN;
        }
        for(int i = 0; i < UI_MENU_COUNT; i++) {
            LCD.setCursor(20, 14*i+13);
            if (UI_MENU.current == i+offset) {
                LCD.fast_rect_filled(3, 3 + ((UI_MENU.current-offset)*14), 122, 16, BLACK);
                LCD.setTextColor(INVERSE);
                LCD.print(UI_MENU.items[i+offset]);
            } else {
                LCD.setTextColor(BLACK);
                LCD.print(UI_MENU.items[i+offset]);
            }
        }
        LCD.update();
    }
}
/*
 SCREEN
 ID: 200
 Debug accel
*/
void Page_service_acceldebug_load(void) {
    LCD.setFont(&ThinPixel10pt7b);
    LCD.setTextColor(BLACK);
}
void Page_service_acceldebug_unload(void) {
    
}
void Page_service_acceldebug_update(void) {
    
    LCD.clear();
    
    int16_t accelx = gyro.getAX();
    int16_t accely = gyro.getAY();
    int16_t accelz = gyro.getAZ();
    
    LCD.setCursor(10, 15);
    LCD.print(accelx);
    
    LCD.setCursor(10, 30);
    LCD.print(accely);
    
    LCD.setCursor(10, 45);
    LCD.print(accelz);
    
    LCD.update();
}
void Page_service_gpstest_load(void) {
    DEVICE_setGPSTestMode(true);
    
    LCD.clear();
    
    LCD.setCursor(0,7);
    LCD.setFont(&ThinPixel10pt7b);
    LCD.print("GPS TEST MODE");
    LCD.update();
}
void Page_service_gpstest_unload(void) {
    DEVICE_setGPSTestMode(false);
    
}
void Page_service_gpstest_update(void) {
}

void SelectMenuItem(void) {
    if (UI_ACTIVE_MENU == 1) {
        switch (UI_MENU.current) {
            case 0:
                Page_action(__PAGE_SET, page_accel);
                break;
            case 1:
                Page_action(__PAGE_SET, page_service_acceldebug);
                break;
            case 2:
                Page_action(__PAGE_SET, page_service_gpstest);
                break;
            default:
                return;
                break;
        }
        UI_ACTIVE_MENU = 0;
        UI_MENU.current = 0;
    }
}
void ShowMenu(uint8_t menu_id) {
    if (menu_id != UI_ACTIVE_MENU) {
        UI_MENU.current = 0;
        
        UI_ACTIVE_MENU = UI_MENU.id;
    } else {
        UI_MENU.current++;
        if (UI_MENU.current >= UI_MENU_COUNT) {
            UI_MENU.current = 0;
        }
    }
    UI_ACTIVE_MENU = UI_MENU.id;
    if (UI_ACTIVE_MENU) {
        Page_action(__PAGE_SET, page_menu); // set screen menu;
    }
}
void SwitchMenu(bool next) {
    if (UI_ACTIVE_MENU) {
        if (next) {
            UI_MENU.current++;
            if (UI_MENU.current >= UI_MENU_COUNT) {
                UI_MENU.current = 0;
            }
        } else {
            UI_MENU.current--;
            if (UI_MENU.current < 0) {
                UI_MENU.current = UI_MENU_COUNT -1;
            }
        }
        
        Page_menu_need_update = true;
        Page_menu_update();
    }
}

void Button_action(uint8_t btn, uint8_t type) {
    
    switch (type) {
        case BTN_PRESS_LONG:
            switch (UI_page) {
                case page_menu:
                    break;
                case page_accel:
                    break;
            }
            break;
            
        case BTN_PRESS_SHORT:
        default:
            switch (UI_page) {
                case page_menu:
                    if (btn == UI_BUTTON_MENU) SelectMenuItem();
                    if (btn == UI_BUTTON_LEFT) SwitchMenu(false);
                    if (btn == UI_BUTTON_RIGHT) SwitchMenu(true);
                    break;
                case page_accel:
                    if (btn == UI_BUTTON_MENU) ShowMenu(1);
                    if (btn == UI_BUTTON_LEFT) Page_accel_results_table_switch(false);
                    if (btn == UI_BUTTON_RIGHT) Page_accel_results_table_switch(true);
                    
                    break;
                case page_service_acceldebug:
                    if (btn == UI_BUTTON_MENU) ShowMenu(1);
                    
                    break;
                case page_service_gpstest:
                    if (btn == UI_BUTTON_MENU) ShowMenu(1);
                    
                    break;
            }
           // _delay_ms(200);
            break;
    }
}

static uint8_t UI_button_press_id = 0;
static uint32_t UI_button_press_time = 0;
static uint32_t UI_button_press_pause = 0;

void Button_update(uint8_t button_id) {
    if (!digitalRead(button_id)) {
        if (UI_button_press_time > 0) {
            if (UI_button_press_time < millis() - 10) {
                Button_action(button_id, BTN_PRESS_SHORT);
                UI_button_press_id = 0;
                UI_button_press_time = 0;
                UI_button_press_pause = millis();
            }
        } else {
            UI_button_press_id = button_id;
            UI_button_press_time = millis();
        }
    } else {
        if (UI_button_press_id == button_id) {
            UI_button_press_id = 0;
            UI_button_press_time = 0;
        }
    }
}
void UI_Update() {
    if (UI_button_press_pause) {
        if (UI_button_press_pause < millis() - 100) {
            UI_button_press_pause = 0;
        }
    }
    if (!UI_button_press_pause) {
        if (UI_button_press_id > 0) {
            Button_update(UI_button_press_id);
        } else {
            Button_update(UI_BUTTON_MENU);
            Button_update(UI_BUTTON_LEFT);
            Button_update(UI_BUTTON_RIGHT);
        }
    }
    
    Page_action(__PAGE_UPDATE, UI_page);
}


