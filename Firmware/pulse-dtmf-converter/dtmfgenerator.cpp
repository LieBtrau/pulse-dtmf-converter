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
along with this program.  If not, see <http://www.gnu.org/licenses/>.#ifndef DTMFGENERATOR_H

Based on :
    http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-dds-sinewave-generator
    https://github.com/antonmeyer/ArduinoDTMF
*/

#include "dtmfgenerator.h"
#include "avr/pgmspace.h"

static const byte sine256[] PROGMEM =
{
    64,67,70,73,76,79,82,85,88,91,94,96,99,102,104,106,109,111,113,115,117,118,120,
    121,123,124,125,126,126,127,127,127,127,127,127,127,126,126,125,124,123,121,120,
    118,117,115,113,111,109,106,104,102,99,96,94,91,88,85,82,79,76,73,70,67,64,60,57,
    54,51,48,45,42,39,36,33,31,28,25,23,21,18,16,14,12,10,9,7,6,4,3,2,1,1,0,0,0,0,0,
    0,0,1,1,2,3,4,6,7,9,10,12,14,16,18,21,23,25,28,31,33,36,39,42,45,48,51,54,57,60
};
//Precalculating DDS tuning word for efficiency
//tword_m = pow(2,bitwidth) * dfreq / refclk;
//  where refclk = F_CPU / 256 for fast PWM
//  where dfreq = desired frequency
//  where bitwidth = number of bits in tuning word
//  dtmf frequencies in Hz = {697,770,852,941,1209,1336,1477,1633};
#if F_CPU==12000000
static const word tWord[8] = {487,538,596,658,845,934,1032,1142};
#else
#error Your MCU frequency is not supported.
#endif
static volatile word twordLow;      //DDS tuning word for rows
static volatile word twordHigh;     //DDS tuning word for columns
static volatile word phaccuLow;     //DDS phase accumulator for rows
static volatile word phaccuHigh;    //DDS phase accumulator for columns

void DtmfGenerator::init (void)
{
    noInterrupts();                     // disable all interrupts
    //Clear OC2B on Compare Match, set OC2B at BOTTOM,(non-inverting mode).
    bitSet(TCCR2A, COM2B1);
    bitClear(TCCR2A, COM2B0);
    //Waveform Generation Mode Bit Description : Fast PWM = mode 3
    bitClear(TCCR2B,WGM22);
    bitSet(TCCR2A,WGM21);
    bitSet(TCCR2A,WGM20);

    //Clock select : No prescaling
    bitClear(TCCR2B, CS22);
    bitClear(TCCR2B, CS21);
    bitSet(TCCR2B, CS20);
    interrupts();                       // Interrupts enabled
}

bool DtmfGenerator::generateTone(char key)
{
    key = (key >= 'a' && key <= 'z' ? 'A' + key - 'a' : key);   // convert to upper case
    char* pch= strchr(keypad, key);
    if(!pch)
    {
        return false;
    }
    byte digitpos=pch-keypad;
    twordHigh = tWord[(digitpos & 0x03) + 4];
    twordLow = tWord[(digitpos >> 2)];
    bitSet(TIMSK2,TOIE2); // timer interrupt on
    pinMode(3, OUTPUT);                 // OCR2B-pin
    return true;
}

void DtmfGenerator::stopTone()
{
    //tone off
    bitClear(TIMSK2,TOIE2);
    pinMode(3, INPUT);                 // OCR2B-pin
}

//**************************************************************************
// Timer overflow interrupt service routine
// interrupt names can be found in Arduino folder: ./hardware/tools/avr/avr/include/avr/
//**************************************************************************
ISR(TIMER2_OVF_vect)
{
    phaccuHigh+=twordHigh;
    phaccuLow+=twordLow;
    byte pwmIndexHigh=phaccuHigh >> 8;
    byte pwmIndexLow=phaccuLow >> 8;      // use upper 8 bits for phase accu as frequency information
    // calculate PWM value: high frequency value + 3/4 low frequency value
    byte pwmVal=pgm_read_byte_near(sine256 + (pwmIndexLow & 0x7F));
    OCR2B = pgm_read_byte_near(sine256 + (pwmIndexHigh & 0x7F)) + pwmVal - (pwmVal>>2);
}
