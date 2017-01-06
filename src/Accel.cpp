#include "Accel.h"
#include "UI.h"

extern UI ui;

enum accel_states {
    idle,
    soft_start,
    race,
    done
};


Accel accel;

Accel::Accel() {
    this->Reset();
}

void Accel::Update( void ) {
    uint16_t speed = gps.getSpeed();
    
    switch (state) {
        case idle:
           
            if (speed) {
                SoftStart();
            }
            break;
        case soft_start:
            if (speed) {
                
                ui.Accel_update_race_message(false);
                
                state = race;
            } else {
                Reset();
                break;
            }
            
        case race:
            
            BufferPush(&speed);
            CheckSpeed(&speed);
            
            if (millis() - startTime >= 60000L) {
                state = done;
            }
            break;
        case done:
            
            ui.Accel_update_race_message(true);
            
            break;
            
        
    }
}
uint16_t Accel::GetResult(int speed_enum) {
    return results[speed_enum];
}
void Accel::SetResult(int speed_enum, uint16_t val) {
    results[speed_enum] = val;
}
uint8_t show_tests = 0;
void Accel::CheckSpeed(uint16_t *speed) {
    if (*speed == 0) {
        Reset();
        return;
    }
    for (int i = 1; i < show_tests; i++) {
        if (results[i]) {
            continue;
        }
        if (*speed <= table[i]) {
            show_tests++;
            results[i] = millis() - startTime;
            ui.Accel_update_results_table(true);
        }
    }
}
void Accel::BufferPush(uint16_t *speed) {
    if (pos_buff == 10) {
        pos_buff = 0;
    }
    buff_speed[pos_buff] = *speed;
    buff_time[pos_buff] = millis() - startTime;
    
    pos_buff++;
}

void Accel::SoftStart( void ) {
    this->state = soft_start;
    this->startTime = millis();
}
void Accel::Reset( void ) {

    show_tests = 0;

    this->startTime = 0;
    this->state = idle;
    
    this->pos_buff = 0;
    memset(buff_speed, 0x0000, 10);
    memset(buff_time, 0x0000, 10);
    memset(results, 0x0000, 10);
    
    ui.Accel_update_race_message(true);

}
