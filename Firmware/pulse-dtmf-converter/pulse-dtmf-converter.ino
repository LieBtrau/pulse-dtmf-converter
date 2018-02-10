#include "dtmfgenerator.h"
#include "rotarydialer.h"
#include <avr/power.h>
#include <avr/sleep.h>

DtmfGenerator dtmf;
RotaryDialer rd(0); // pin5

void setup()
{
    reducePower();
    dtmf.init();
    rd.init();
}

void loop()
{
    sleepNow();
    rd.update();
    if(rd.available())
    {
        //Pulse dial digit decoded, now create its DTMF equivalent
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
    bitSet(ACSR, ACD);      //disable analog comparator
#endif
    power_adc_disable();
}

void sleepNow()
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_mode();
    sleep_disable();
}
