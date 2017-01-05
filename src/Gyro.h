#include "Arduino.h"
#include "Wire/Wire.h"

#ifndef CLASS_GYRO
#define CLASS_GYRO


#define    MPU9250_ADDRESS              0x68
#define    AK8963_ADDRESS               0x0C

#define    GYRO_FULL_SCALE_250_DPS      0x00
#define    GYRO_FULL_SCALE_500_DPS      0x08
#define    GYRO_FULL_SCALE_1000_DPS     0x10
#define    GYRO_FULL_SCALE_2000_DPS     0x18

#define    ACC_FULL_SCALE_2_G           0x00
#define    ACC_FULL_SCALE_4_G           0x08
#define    ACC_FULL_SCALE_8_G           0x10
#define    ACC_FULL_SCALE_16_G          0x18

#define    ACC_LSB_SENSITIVITY_2_G      16384
#define    ACC_LSB_SENSITIVITY_4_G      8192
#define    ACC_LSB_SENSITIVITY_8_G      4096
#define    ACC_LSB_SENSITIVITY_16_G     2048

class Gyro
{
public:
    Gyro( void );
    void Init( void );
    void Update( void );
    
    int16_t getAX(void) { return ax; }
    int16_t getAY(void) { return ay; }
    int16_t getAZ(void) { return az; }
    
private:
    int16_t ax;
    int16_t ay;
    int16_t az;
    // This function read Nbytes bytes from I2C device at address Address.
    // Put read bytes starting at register Register in the Data array.
    void read(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data);
    void write(uint8_t Address, uint8_t Register, uint8_t Data);
};

#endif
