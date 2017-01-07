#ifndef __CC9F284D_952EBC728
#define __CC9F284D_952EBC728

#include "../graphics/boot_img.xbm"
#include "../graphics/icon_gps.xbm"
#include "../graphics/warning.xbm"

#include "Arduino.h"
#include "display/ST7565.h"
#include "display/fonts/ThinPixel.h"
#include "display/fonts/ThinPixelRU.h"
#include "display/fonts/Minecraftia.h"


#define MIN_SAT_COUNT   4

#define LCD_CS          13
#define LCD_RST         12
#define LCD_A0          11
#define LCD_SDA         10
#define LCD_SCK         9
#define LCD_BACKLIGHT   8

#define BUTTON_PORT_RIGHT   5
#define BUTTON_PORT_MENU    4
#define BUTTON_PORT_LEFT    3
#define BUTTON_PORT_RESET   2

class UI {
public:
    UI();
    void Init( void );
    void Update( void );
    
    void PrintCyrilic( char * str );
    
    void buttonsHandler( void );
    
    //virtual void Load( void ) = 0;
  //  virtual void Update( void );
   

    //struct MainMenu {
    //    void Load ( void );
    //} Menu;
    
    void PageUnload( void );
    void PageLoad(uint8_t pageId);
    
    void Accel_load( void );
    void Menu_load( void );
    void Debug_load( void );
    
    void Accel_need_update( void );
    
private:
    enum screen_pages {
        page_splash,
        page_menu,
        page_accel,
        page_debug,
    };
    
    
    
    void Splash_load( void );
    void Splash_unload( void );
    void Splash_update( void );
    
    
    void Menu_select( void );
    void Menu_unload ( void );
    void Menu_update( void );
    void Menu_next( void );
    void Menu_prev( void );
    
    bool accel_need_update = false;
    
    void Accel_unload( void );
    void Accel_update( void );
    
    void Accel_update_sat( void );
    void Accel_update_sat_message( void );
    void Accel_update_race_message( bool clear );
    void Accel_update_speed( void );
    void Accel_update_results_table( bool update );
    void Accel_results_table_switch( bool next );
    void Accel_reset( void );
    
    void Debug_unload(void);
    void Debug_update(void);
    
    
    ST7565 LCD;
    
    int8_t currentPage;
    
    uint32_t timer_HALF_SEC_CHANGE;
    
    bool HALF_SEC_CHANGE;
};

#define BUTTON_DEBONCE_TIME 50
class Button {
public:
    inline void setup(uint8_t _dport) {
        _pressCount = 0;
        _port=_dport;
        _state= 0xFF;
        pinMode(_port, INPUT_PULLUP);
    };
    
    // Если true то можно выполнять операцию
    bool pressed();
private:
    
    uint8_t         _pressCount;
    uint8_t         _port;
    uint8_t         _state;
    uint32_t        _lastTime;
};
#endif;

