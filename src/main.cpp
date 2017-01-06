#include "Gyro.h"
#include "GPS.h"
#include "Accel.h"
#include "UI.h"
#include "Arduino.h"
#include "sam.h"

extern UI       ui;
extern GPS      gps;
extern Gyro     gyro;
extern Accel    accel;


/*
 Инициализация прерывания по таймеру TC3
 */
void init_TC3()
{
    // Enable clock for TC
    REG_GCLK_CLKCTRL = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID ( GCM_TCC2_TC3 ) ) ;
    
    while ( GCLK->STATUS.bit.SYNCBUSY == 1 ); // wait for sync
    
    // The type cast must fit with the selected timer mode
    TcCount16* TC = (TcCount16*) TC3; // get timer struct
    
    TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
    
    //Тип таймера 8/16/32 бит, чистота, множитель 1-1024
    TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16 | TC_CTRLA_WAVEGEN_NFRQ | TC_CTRLA_PRESCALER_DIV1;
    
    while (TC->STATUS.bit.SYNCBUSY == 1); // wait for sync
    
    TC->INTENSET.reg = 0;                   // Выключить прерывание
    TC->INTENSET.bit.OVF = 1;               // Разрешить прерывание по переполнению
    
    NVIC_EnableIRQ(TC3_IRQn);               // Добавляем вектор прерывания
    
    TC->CTRLA.reg |= TC_CTRLA_ENABLE;       // Включаем прерывание
}
/*
 Прерывание по таймеру TC3
 */
void TC3_Handler()
{
    TcCount16* TC = (TcCount16*) TC3;
    
    //Задаем значения счетчика
    //TC->COUNT.reg = 1000;//46875;//18660;// 1 s.
    
    // Таймер переполнен
    if (TC->INTFLAG.bit.OVF) {
        
        
        //Обновляем данные GPS
        gps.Update();
        //Обновляем данные гироскопа
        //gyro.Update();
        
        
        // Сброс таймера
        TC->INTFLAG.bit.OVF = true;
    }
    
}


int main() {
    //System startup
    init();
    //USB startup
    USBDevice.init();
    USBDevice.attach();
    
    //Инициализация прерываний
    init_TC3();
    

    //SerialUSB.begin(115200);
    
    //Инициализация классов
    ui.Init();
    gps.Init();
    
    ui.Menu_load();
    
    while(true) {
        ui.Update();
        //gps.Update();
    }
    
    return 0;
}
