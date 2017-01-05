#include "Arduino.h"
#include "GPS.h"

#ifndef CLASS_ACCEL
#define CLASS_ACCEL

extern GPS gps;

#define SPEED_RESULTS_TABLE_COUNT 9


class Accel {
public:
    Accel();
    
    void Update( void );
    
    const uint8_t table[10] = { 0,2,4,6,10,12,15,20,28,30 };
    
    enum {
        speed_0,
        speed_20,
        speed_40,
        speed_60,
        speed_80,
        speed_100,
        speed_150,
        speed_200,
        speed_250,
        speed_280,
    } speed_enum;
    
    void SetResult(int speed_enum, uint16_t val);
    uint16_t GetResult(int speed_enum);
    
    void Reset( void );
    
private:
    void SoftStart( void );
    
    void BufferPush(uint16_t *speed);
    void CheckSpeed(uint16_t *speed);

private:
    
    uint8_t state;
    uint32_t startTime;

    uint8_t pos_buff;
    uint16_t buff_time[10];
    uint16_t buff_speed[10];
    
    uint16_t results[10];

};

#endif
