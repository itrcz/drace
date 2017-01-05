#include "Arduino.h"

#include "GPS.h"
#include "Gyro.h"

#include "State.h"

extern GPS gps;
extern Gyro gyro;

#define MAX_ITEMS_ON_SCREEN 4

#define UI_BUTTON_RIGHT 5
#define UI_BUTTON_MENU  4
#define UI_BUTTON_LEFT  3

#define BTN_PRESS_SHORT 0
#define BTN_PRESS_LONG 1


void UI_Init(void);
void UI_load_lastpage(void);
void UI_Update(void);

void Page_action(int8_t a, int8_t page);


void Page_splash_load(void);
void Page_splash_unload(void);
void Page_splash_update(void);

void Page_accel_load(void);
void Page_accel_unload(void);
void Page_accel_update(void);
void Page_accel_update_sat(void);
void Page_accel_update_speed(void);

void Page_accel_update_results_table(bool update);
void Page_accel_results_table_switch(bool next);

static bool Page_menu_need_update;

void Page_menu_load(void);
void Page_menu_unload(void);
void Page_menu_update(void);


void Page_service_acceldebug_load(void);
void Page_service_acceldebug_unload(void);
void Page_service_acceldebug_update();

void Page_service_gpstest_load(void);
void Page_service_gpstest_unload(void);
void Page_service_gpstest_update(void);
