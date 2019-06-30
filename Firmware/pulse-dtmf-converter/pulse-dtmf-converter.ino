#include "dtmfgenerator.h"
#include "rotarydialer.h"
#include <avr/power.h>
#include <avr/sleep.h>

DtmfGenerator dtmf;
RotaryDialer rd(0); // pin5

const int modePin = 2; // pin7

int mode = 1;  // 0: (modePin LOW)  original - tone after each digit
               // 1: (modePin HIGH) gather all digits then send tones
const bool REVERSE_DIAL_MODE = false; // set to true for reverse dial systems (e.g. in use on New-Zealand)

unsigned long lastUpdate = 0;
int i = 0;
int k = 0;
byte n[200];

void setup()
{
    pinMode(modePin,INPUT_PULLUP);
    if(digitalRead(modePin) == LOW)
    {
        mode = 0;
    }

    reducePower();
    dtmf.init();
    rd.init();

    i = 0;
    k = 0;
}

void loop()
{
    if(mode == 0)
    {
        sleepNow();
        rd.update();
        if(rd.available())
        {
            // Pulse dial digit decoded, now create its DTMF equivalent
            byte dialedDigit=rd.read();
            if(REVERSE_DIAL_MODE && dialedDigit!=10)
            {
                dialedDigit = 10 - dialedDigit;
            }
            dtmf.generateTone('0'+dialedDigit);
            delay(80);
            dtmf.stopTone();
        }
    }
    else
    {
        sleepNow();
        rd.update();
        if(rd.available())
        {
            n[i] = rd.read();
            i++;
            lastUpdate = millis() + 3000;
        }

        if((i > 0) && (millis() > lastUpdate))
        {
            for(k = 0; k < i; k++)
            {
                if(REVERSE_DIAL_MODE && n[k]!=10)
                {
                    n[k] = 10 - n[k];
                }
                dtmf.generateTone('0' + n[k]);
                delay(80);
                dtmf.stopTone();
                delay(80);
            }
            i = 0;
        }
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
