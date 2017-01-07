#include "GPS.h"

#include "UI.h"
#include "Accel.h"

extern UI     ui;
extern Accel   accel;

GPS gps;


GPS::GPS( void )
{
    memset( _rxBuffer, 0, 128 ) ;
}
void GPS::Init( void ) {

    UBX_UART.begin(9600);
    //Port settings
    this->write(UBX_CLASS_CFG, UBX_ID_PRT, UBX_PORT_SETTINGS, sizeof(UBX_PORT_SETTINGS));
    UBX_UART.end();
    delay(100);
    UBX_UART.begin(115200);
    
    //Power Settings
    //Set to full power
    //this->write(UBX_CLASS_CFG, UBX_ID_PMS, UBX_PMS_SETTINGS, sizeof(UBX_PMS_SETTINGS));
    
    //Engine settings
    this->write(UBX_CLASS_CFG, UBX_ID_NAV5, UBX_ENGINE_SETTINGS, sizeof(UBX_ENGINE_SETTINGS));
    
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GGA_CFG, sizeof(GGA_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GLL_CFG, sizeof(GLL_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GSA_CFG, sizeof(GSA_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GSV_CFG, sizeof(GSV_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, RMC_CFG, sizeof(RMC_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, VTG_CFG, sizeof(VTG_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GRS_CFG, sizeof(GRS_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GST_CFG, sizeof(GST_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, ZDA_CFG, sizeof(ZDA_CFG));
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, GBS_CFG, sizeof(GBS_CFG));
   
    //Enable SOL UBX messages
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, UBX_SOL_CFG, sizeof(UBX_SOL_CFG));
    //Enable VELNED UBX messages
    this->write(UBX_CLASS_CFG, UBX_ID_MSG, UBX_VELNED_CFG, sizeof(UBX_VELNED_CFG));
    
    //Change rate to 10Hz
    this->write(UBX_CLASS_CFG, UBX_ID_RATE, UBX_RATE_200MS, sizeof(UBX_RATE_200MS));
    

    this->configurated = true;
}
void GPS::Update( void ) {
    if (!configurated) {
        return;
    }
    
    while (UBX_UART.available()) {
        
        char b = UBX_UART.read();
        
        _rxBuffer[_rxPos] = b;
        
        //Поиск заголовка
        if (_rxPos < 2) {
            if ( b == UBX_HEADER[ _rxPos ] ) {
                _rxPos++;
                if (_rxPos == 2) {
                    memset(_rxCK, 0, 2);
                    _rxLength = 0;
                }
            }
            return;
        }
        //Получен ClassID
        if (_rxPos == 2 ) {
            
        }
        //Получен ID
        if (_rxPos == 3 ) {
            
        }
        //Получена длинна данных
        if (_rxPos == 4 ) {
            _rxLength = b;
        }
        if (_rxPos < _rxLength + 6) {
            //Расчет суммы
            _rxCK[0] += b;
            _rxCK[1] += _rxCK[0];
        }
        //Check CK_A
        if (_rxPos == _rxLength + 6) {
            if (b != _rxCK[0]) {
                if (SerialUSB) SerialUSB.println("CK_A ERROR");
                _rxPos = 0;
                return;
            }
        }
        if (_rxPos == _rxLength + 7) {
            
            if (b != _rxCK[1]) {
                if (SerialUSB) SerialUSB.println("CK_B ERROR");
                _rxPos = 0;
                return;
            }
            this->_decodePacket();
            
            _rxPos = 0;
            return;
        }
        _rxPos++;
    }
    
}



void GPS::_decodePacket( void ) {
    
    
    accel.Update();
    //TEST MODE
    return;
    
    //Если получен пакет с классом NAV
    if (_rxBuffer[UBX_POS_CLASS] == UBX_CLASS_NAV) {
        //Обработка зависит от ID
        switch (_rxBuffer[UBX_POS_ID]) {
            // 35.9 NAV-SOL (0x01 0x06)
            case UBX_ID_SOL:
                //Значение Fixed
                this->fix = _rxBuffer[16];
                //Количество спутников
                this->sat = _rxBuffer[53];
    
                //ui.Accel_update_sat();
          
                break;
            // 35.15 NAV-VELNED (0x01 0x12)
            case UBX_ID_VELNED:
                
                //Скорость на земле в см/сек
                uint8_t gSpeed[] = { _rxBuffer[26], _rxBuffer[27], _rxBuffer[28], _rxBuffer[29] };

                memcpy(&this->speed, gSpeed, 4);
                
                //ui.Accel_update_speed();
                
                //accel.Update();
                
                break;
        }
    }
}
void GPS::write(uint8_t Class, uint8_t Id, const uint8_t *Data, uint8_t Length) {
  
    //Send header
    for (uint8_t i=0; i < 2; i++) {
        UBX_UART.write(UBX_HEADER[i]);
    }
    //Allocate check summ mem
    uint8_t *CK = new uint8_t[2];
    memset(CK, 0, 2);
    
    //Sendign Class
    UBX_UART.write(Class);
    CK_recalc(CK, Class);
    
    //Sendign Id
    UBX_UART.write(Id);
    CK_recalc(CK, Id);
    
    //Sendign Length
    UBX_UART.write(Length);
    CK_recalc(CK, Length);
    UBX_UART.write((unsigned char)0x00);
    CK_recalc(CK, 0x00);
    
    //Sendign Data
    for (uint8_t i=0; i < Length; i++) {
        UBX_UART.write(Data[i]);
        //Recalculate CK
        CK_recalc(CK, Data[i]);
    }
    
    //Sending CK
    UBX_UART.write(CK[0]);
    UBX_UART.write(CK[1]);
    
    //Free mem
    delete [] CK;
    
    UBX_UART.print("\n\r");
}


uint16_t GPS::getSpeed ( void ) {
    
    //если меньше чем 13 см/с (0,5км/ч) возвращаем 0
    //if (this->speed <= 13) return 0;
    //Если колличество спутников менее 4 возвращаем 0
    if (this->sat < 4) return 0;
        
    uint16_t speed = (this->speed * 3600 / 100000L);
    
    if (speed >= 500) return 0;
    
    return speed;
}

