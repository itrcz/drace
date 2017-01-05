#include "Arduino.h"


void DEVICE_setIlde(bool state);
bool DEVICE_isIdle(void);

void DEVICE_setSatelliteReady(bool state);
bool DEVICE_getSatelliteReady(void);

void DEVICE_setAccelReady(bool state);
bool DEVICE_getAccelReady(void);

void DEVICE_setSDCardState(bool state);
bool DEVICE_getSDCardState(void);

void DEVICE_setReady(bool state);
bool DEVICE_isReady(void);

void DEVICE_setGPSTestMode(bool state);
bool DEVICE_getGPSTestMode(void);
