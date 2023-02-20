/*
pulse-dtmf-converter: application to converted pulse-dialing to DTMF-tones
Copyright (C) 2018  Christoph Tack

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * By pulling pin 7 of the ATtiny45, you can select to send a dtmf tone after each digit, or send them all at once after the last digit has been dialed.
 * 0: (pin 7 =  LOW) - tone after each digit
 * 1: (pin 7 HIGH, default) gather all digits then send tones
 */

/*
2023-02-20 Christoph Tack :
    * Correcting buffer size of pulseBuffer.  It overflowed the amount of RAM
    * Turn the amount of time between last dialed digit and start of DTMF-sequence into a configurable constant.
    * Migrate toolchain to PlatformIO. (build succeeds, but not tested onto the target). The header files have been left in the src folder for ease of use for people using the Arduino IDE.
*/

#include <avr/power.h>
#include <avr/sleep.h>
#include "dtmfgenerator.h"
#include "rotarydialer.h"
#include "pins.h"

DtmfGenerator dtmf;
RotaryDialer rd(PIN_DTMF_OUT); 

const bool REVERSE_DIAL_MODE = false;                   // set to true for reverse dial systems (e.g. in use on New-Zealand)
const int DTMF_DELAY_AFTER_LAST_DIALED_DIGIT_ms = 1200; //wait this after last dialed digit before sending DTMF sequence.
const int MAX_NR_OF_DIALED_DIGITS = 15;                 //your longest phone number should be shorter than this value.

unsigned long lastUpdate = 0;
byte pulsesCtr = 0;
byte pulsesBuffer[MAX_NR_OF_DIALED_DIGITS];
int mode = 0;                                           // select when to send DTMF tones (see explanation above)


void setup()
{
    pinMode(PIN_MODE_SELECT, INPUT_PULLUP);
    mode = digitalRead(PIN_MODE_SELECT) == LOW ? 0 : 1;

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
        if (rd.available() && pulsesCtr < MAX_NR_OF_DIALED_DIGITS)
        {
            pulsesBuffer[pulsesCtr++] = rd.readPulses();
            lastUpdate = millis() + DTMF_DELAY_AFTER_LAST_DIALED_DIGIT_ms;
        }
        if ((pulsesCtr > 0) && (millis() > lastUpdate))
        {
            for (byte k = 0; k < pulsesCtr; k++)
            {
                pulsesToDtmf(pulsesBuffer[k]);
                delay(80);
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
