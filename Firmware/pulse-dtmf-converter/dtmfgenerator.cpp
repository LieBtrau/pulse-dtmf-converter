/*
DtmfGenerator: library for generating DTMF-signals
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

Based on :
    http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-dds-sinewave-generator
    https://github.com/antonmeyer/ArduinoDTMF
*/

#include "dtmfgenerator.h"

//Sine samples LUT : one period sampled on 128 samples and quantized on 7 bit
static const byte sine128[] PROGMEM =
{
    64,67,70,73,76,79,82,85,88,91,94,96,99,102,104,106,109,111,113,115,117,118,120,
    121,123,124,125,126,126,127,127,127,127,127,127,127,126,126,125,124,123,121,120,
    118,117,115,113,111,109,106,104,102,99,96,94,91,88,85,82,79,76,73,70,67,64,60,57,
    54,51,48,45,42,39,36,33,31,28,25,23,21,18,16,14,12,10,9,7,6,4,3,2,1,1,0,0,0,0,0,
    0,0,1,1,2,3,4,6,7,9,10,12,14,16,18,21,23,25,28,31,33,36,39,42,45,48,51,54,57,60
};
//Precalculating DDS tuning words:
//  tword_m = pow(2,bitwidth) * dfreq / refclk;
//      where refclk = F_CPU / 256 for fast PWM, in [Hz]
//      where dfreq = desired frequency, in [Hz]
//      where bitwidth = number of bits in tuning word, here 15bit
//
//      dtmf frequencies in Hz = {697,770,852,941,1209,1336,1477,1633};
#if F_CPU==8000000
static const word tWord[8] = {731,807,893,987,1268,1401,1549,1712};
#elif F_CPU==12000000
static const word tWord[8] = {487,538,596,658,845,934,1032,1142};
#elif F_CPU==16000000
static const word tWord[8] = {365,404,447,493,634,700,774,856};
#else
#error Your MCU frequency is not supported.
#endif
static volatile word twordLf;       //DDS tuning word for LF (<1KHz)
static volatile word twordHf;       //DDS tuning word for HF (>1KHz)
static volatile word phaccuLf;      //DDS phase accumulator for LF
static volatile word phaccuHf;      //DDS phase accumulator for HF
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
const byte dtmfpin=3;
#elif ARDUINO_AVR_ATTINYX5
const byte dtmfpin=4;
#endif
void DtmfGenerator::init (void)
{
    noInterrupts();                     // disable all interrupts
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    //Waveform Generation Mode Bit Description : Fast PWM = mode 3
    bitClear(TCCR2B,WGM22);
    bitSet(TCCR2A,WGM21);
    bitSet(TCCR2A,WGM20);
#elif ARDUINO_AVR_ATTINYX5
    //PWM mode : B enable
    bitSet(GTCCR, PWM1B);
    OCR1C=255;
#endif

    //Clock select : No prescaling
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    bitClear(TCCR2B, CS22);
    bitClear(TCCR2B, CS21);
    bitSet(TCCR2B, CS20);
#elif ARDUINO_AVR_ATTINYX5
    bitClear(TCCR1,CS13);
    bitClear(TCCR1,CS12);
    bitClear(TCCR1,CS11);
    bitSet(TCCR1,CS10);
#endif
    interrupts();                       // Interrupts enabled
}

bool DtmfGenerator::generateTone(char key)
{
    key = (isLowerCase(key) ? 'A' + key - 'a' : key);   // convert to upper case
    char* pch= strchr(keypad, key);
    if(!pch)
    {
        return false;
    }
    byte digitpos=pch-keypad;
    twordHf = tWord[(digitpos & 0x03) + 4];
    twordLf = tWord[(digitpos >> 2)];

#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    //Connect PWM-pin to timer2: Clear OCR2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
    bitSet(TCCR2A, COM2B1);
    bitClear(TCCR2A, COM2B0);
    bitSet(TIMSK2,TOIE2);               // timer interrupt on
#elif ARDUINO_AVR_ATTINYX5
    //Connect PWM-pin to timer1: Clear OC1B on Compare Match, set OC1B at BOTTOM, (non-inverting mode).
    bitSet(GTCCR, COM1B1);
    bitClear(GTCCR, COM1B0);
    bitSet(TIMSK, TOIE1);               // timer interrupt on
#endif

    pinMode(dtmfpin, OUTPUT);
    return true;
}

void DtmfGenerator::stopTone()
{
    //tone off
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    bitClear(TIMSK2,TOIE2);             // timer interrupt off
#elif ARDUINO_AVR_ATTINYX5
    bitClear(TIMSK, TOIE1);
#endif
    digitalWrite(dtmfpin,0);            // OCR2B remains low after last generated pulse
}

//**************************************************************************
// Timer overflow interrupt service routine
// interrupt names can be found in Arduino folder: ./hardware/tools/avr/avr/include/avr/
//**************************************************************************
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
ISR(TIMER2_OVF_vect)
#elif ARDUINO_AVR_ATTINYX5
ISR(TIMER1_OVF_vect)
#endif
{
    //increase phase accu
    phaccuHf+=twordHf;
    phaccuLf+=twordLf;
    // use upper 8 bits of phase accu as frequency information
    byte pwmIndexHf=phaccuHf >> 8;
    byte pwmIndexLf=phaccuLf >> 8;
    // calculate PWM value for DTMF = HF value + 3/4 LF value
    byte pwmVal=pgm_read_byte_near(sine128 + (pwmIndexLf & 0x7F));
    byte ocr = pgm_read_byte_near(sine128 + (pwmIndexHf & 0x7F)) + pwmVal - (pwmVal>>2);
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    OCR2B = ocr;
#elif ARDUINO_AVR_ATTINYX5
    OCR1B = ocr;
#endif
}
