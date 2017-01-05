#include "State.h"


static struct device_state {
    bool idle           = false;
    bool sat_ready      = false;
    bool accel_ready    = false;
    bool sd_ready       = false;
    
    bool ready = false;
    
    bool gps_test_mode = false;
    
} device_state ;

void DEVICE_setIlde(bool state) {
    device_state.idle = state;
}
bool DEVICE_isIdle(void) {
    return !device_state.idle;
}
void DEVICE_setSatelliteReady(bool state) {
    device_state.sat_ready = state;
}
bool DEVICE_getSatelliteReady(void) {
    return device_state.sat_ready;
}
void DEVICE_setAccelReady(bool state) {
    device_state.accel_ready = state;
}
bool DEVICE_getAccelReady(void) {
    return device_state.accel_ready;
}
void DEVICE_setSDCardState(bool state) {
    device_state.sd_ready = state;
}
bool DEVICE_getSDCardState(void) {
    return device_state.sd_ready;
}
void DEVICE_setReady(bool state) {
    device_state.ready = state;
}
bool DEVICE_isReady(void) {
    return device_state.ready;
}


void DEVICE_setGPSTestMode(bool state) {
    device_state.gps_test_mode = state;
}
bool DEVICE_getGPSTestMode(void) {
    return device_state.gps_test_mode;
}
