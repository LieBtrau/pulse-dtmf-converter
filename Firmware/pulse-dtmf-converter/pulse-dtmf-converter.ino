#include "dtmfgenerator.h"
#include "rotarydialer.h"
#include <avr/power.h>

DtmfGenerator dtmf;
RotaryDialer rd(5);

void setup()
{
    reducePower();
    dtmf.init();
    rd.init();
}

void loop()
{
    rd.update();
    if(rd.available())
    {
        byte dialedDigit=rd.read();
        dtmf.generateTone('0'+dialedDigit);
        delay(80);
        dtmf.stopTone();
    }
}

void reducePower()
{
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    power_twi_disable();
    power_timer1_disable();
    power_spi_disable();
    power_usart0_disable();
#elif ARDUINO_AVR_ATTINYX5
    power_usi_disable();
#endif
    power_adc_disable();
}
