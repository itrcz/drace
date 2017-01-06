#include "Gyro.h"


Gyro gyro;

Gyro::Gyro() {
    this->ax = 0;
    this->ay = 0;
    this->az = 0;
}
void Gyro::Init(void) {
    Wire.begin();
    
    this->write(MPU9250_ADDRESS, 0x1C, ACC_FULL_SCALE_4_G);
    //this->write(MPU9250_ADDRESS, 0x1B, GYRO_FULL_SCALE_250_DPS);
    //this->write(MPU9250_ADDRESS,0x37,0x02);
    //this->write(AK8963_ADDRESS,0x0A,0x01);
}
void Gyro::Update(void) {
    
    Wire.beginTransmission(MPU9250_ADDRESS);
    Wire.write(0x3B);
    Wire.endTransmission();
    
    Wire.requestFrom(MPU9250_ADDRESS, 6); //Request Z Accel Registers (0x3B - 0x40)
    
    this->ay =  (Wire.read()<<8 | Wire.read()) / 100;
    this->ax = -(Wire.read()<<8 | Wire.read()) / 100;
    this->az = -(Wire.read()<<8 | Wire.read()) / 100;
    
}

void Gyro::read(uint8_t Address, uint8_t Register, uint8_t Nbytes, uint8_t* Data) {
    // Set register address
    Wire.beginTransmission(Address);
    Wire.write(Register);
    Wire.endTransmission();
    
    // Read Nbytes
    Wire.requestFrom(Address, Nbytes);
    uint8_t index=0;
    while (Wire.available())
        Data[index++]=Wire.read();
}
void Gyro::write(uint8_t Address, uint8_t Register, uint8_t Data) {
    Wire.beginTransmission(Address);
    Wire.write(Register);
    Wire.write(Data);
    Wire.endTransmission();
}
