#include "Accel.h"
#include "UI.h"
#include "GPS.h"

extern GPS gps;
extern UI ui;

Accel accel;

Accel::Accel() {
    this->Reset();
}

void Accel::Update( void ) {
    //Если модуль не активен завершаем операцию
    if (!this->active) return;
    
    if (gps.sat < 4) {
        return;
    }
    
    //Ставим флаг что нужно обновил экран при возможности
    ui.Accel_need_update();
    
    uint16_t speed = gps.getSpeed();
    
    switch (state) {
        //Ожидания полной остановки
        case awaiting:
            
            if (speed == 0) {
               state = idle;
            }
            break;
        //Ожидания начала движения
        case idle:
           //Если движения началось переходим в Soft режим замера
            if (speed) {
                SoftStart();
            }
            break;
        //Если после старта мы продолжаем ускорятся тогда переходим в полноценный режим замера
        case soft_start:
            if (speed) {
                state = race;
                
            } else {
                Reset();
                break;
            }
        //Режим замера
        case race:
            if (speed != 0) {
                BufferPush(&speed);
                CheckSpeed(&speed);
                
                if (millis() - startTime >= 60000L) {
                    state = done;
                }
            } else {
                state = done;
            }
            break;
        //Замер окончен, ожидания сброса
        case done:
            break;
    }
}
/*
 Метод запускает софт замер
 записывает метку времени начала замера
 */
void Accel::SoftStart( void ) {
    this->state = soft_start;
    this->startTime = millis();
}
/*
 Метод проверяет скорость при замере
 */
void Accel::CheckSpeed(uint16_t *speed) {
    for (int i = 0; i < sizeof table; i++) {
        
        //Если результата на текущую скорость еще нет
        if (! results[i]) {
            //Текущая метка скорости
            uint16_t speed_mark = table[i] * 10;
            
            //Проверяем если текущая скорость равна или больше чем метка
            if (buff_speed[ pos_buff - 1 ] >= speed_mark) {
                //Записываем результат в таблицу
                results[i] = buff_time[ pos_buff - 1 ];
            }
        }
    }
}
/*
 Метод добавляет текущую скорость и метку времени в буфер
 */
void Accel::BufferPush(uint16_t *speed) {
    if (pos_buff == 10) {
        pos_buff = 0;
    }

    buff_speed[pos_buff] = *speed;
    buff_time[pos_buff] = millis() - startTime;
        
    pos_buff++;
}
/*
 Метод возвращает текущее состояние
 */
int Accel::GetState( void ) {
    return this->state;
}
void Accel::SetActive( bool is_active ) {
    this->active = is_active;
}
/*
 Метод читает время замера по индексу таблицы
 */
uint16_t Accel::GetResult(int speed_enum) {
    return results[speed_enum];
}
/*
 Метод записывает время замера по индексу таблицы
 */
void Accel::SetResult(int speed_enum, uint16_t val) {
    results[speed_enum] = val;
}
/*
 Обнуляет таблицы и переходит в режим ожидания замера
 */
void Accel::Reset( void ) {
    
    //Ставим флаг что нужно обновил экран при возможности
    ui.Accel_need_update();
    
    this->startTime = 0;
    this->state = awaiting;
    
    this->pos_buff = 0;
    
    memset(buff_speed,  0x0000, 10);
    memset(buff_time,   0x0000, 10);
    memset(results,     0x0000, 10);

}
