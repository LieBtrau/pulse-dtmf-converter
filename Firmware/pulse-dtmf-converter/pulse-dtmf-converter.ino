#include "dtmfgenerator.h"
#include "rotarydialer.h"
#include <avr/power.h>
#include <avr/sleep.h>

DtmfGenerator dtmf;
RotaryDialer rd(0); // pin5

const int modePin = 2; // pin7

int mode = 1;                         // 0: (modePin LOW)  original - tone after each digit
                                      // 1: (modePin HIGH) gather all digits then send tones
const bool REVERSE_DIAL_MODE = false; // set to true for reverse dial systems (e.g. in use on New-Zealand)

unsigned long lastUpdate = 0;
byte pulsesCtr = 0;
byte pulsesBuffer[200];

void setup()
{
    pinMode(modePin, INPUT_PULLUP);
    if (digitalRead(modePin) == LOW)
    {
        mode = 0;
    }

    reducePower();
    dtmf.init();
    rd.init();

    pulsesCtr = 0;
}

void pulsesToDtmf(byte nrOfPulses)
{
    byte dtmfDigit;
    if (REVERSE_DIAL_MODE)
    {
        dtmfDigit = 10 - nrOfPulses;
    }
    else
    {
        dtmfDigit = nrOfPulses == 10 ? 0 : nrOfPulses;
    }
    dtmf.generateTone('0' + dtmfDigit);
    delay(80);
    dtmf.stopTone();
}

void loop()
{
    sleepNow();
    rd.update();
    if (mode == 0)
    {
        if (rd.available())
        {
            // Pulse dial digit decoded, now create its DTMF equivalent
            pulsesToDtmf(rd.readPulses());
        }
    }
    else
    {
        if (rd.available())
        {
            pulsesBuffer[pulsesCtr++] = rd.readPulses();
            lastUpdate = millis() + 3000;
        }
        if ((pulsesCtr > 0) && (millis() > lastUpdate))
        {
            for (byte k = 0; k < pulsesCtr; k++)
            {
                pulsesToDtmf(pulsesBuffer[k]);
            }
            pulsesCtr = 0;
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
    bitSet(ACSR, ACD); //disable analog comparator
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
