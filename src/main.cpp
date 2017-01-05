#include "Gyro.h"
#include "GPS.h"
#include "Accel.h"
#include "Screen.h"

Screen  ui;
GPS     gps;
Gyro    gyro;
Accel   accel;

void system_init( void ) {
    init();
    delay(1);
    USBDevice.init();
    USBDevice.attach();
    delay(10);
}

int main() {
    system_init();
    SerialUSB.begin(9600);
    ui.Init();
    gps.Init();
    
    ui.Menu_load();
    
    while(true) {
        ui.Update();
        gps.Update();
    }
    
    return 0;
}
