/*
DtmfGenerator: library for timer
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

#include "timer.h"

Timer timer;    //pre-instantiating (for ISR)

void Timer::initialize()
{
    noInterrupts();                     // disable all interrupts
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    //Waveform Generation Mode Bit Description : Fast PWM = mode 3
    bitClear(TCCR2B,WGM22);
    bitSet(TCCR2A,WGM21);
    bitSet(TCCR2A,WGM20);
#elif ARDUINO_AVR_ATTINYX5
    //PWM mode using PWM1A
    bitClear(TCCR1, CTC1);
    bitSet(TCCR1, PWM1A);
    bitClear(GTCCR, PWM1B);
    OCR1C=255;
#endif
    interrupts();                       // Interrupts enabled
}

void Timer::setPrescaler(PRESCALER p)
{
    noInterrupts();                     // disable all interrupts
    switch(p)
    {
    case CK1:
    #ifdef ARDUINO_AVR_PROTRINKET3FTDI
        bitClear(TCCR2B, CS22);
        bitClear(TCCR2B, CS21);
        bitSet(TCCR2B, CS20);
    #elif ARDUINO_AVR_ATTINYX5
        bitClear(PLLCSR, PCKE);           //Don't clock from PCK, because its source is the internal R/C.
        bitClear(TCCR1,CS13);
        bitClear(TCCR1,CS12);
        bitClear(TCCR1,CS11);
        bitSet(TCCR1,CS10);
    #endif
        break;
    default:
        break;
    }
    interrupts();                       // Interrupts enabled
}

void Timer::pwm()
{
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    //Connect PWM-pin to timer2: Clear OCR2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
    bitSet(TCCR2A, COM2B1);
    bitClear(TCCR2A, COM2B0);
#elif ARDUINO_AVR_ATTINYX5
    //Connect PWM-pin to timer1: Clear OC1A on Compare Match, set OC1A at BOTTOM, (non-inverting mode).
    bitSet(TCCR1, COM1A1);
    bitClear(TCCR1, COM1A0);
#endif
}

void Timer::disablePwm()
{
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    //Connect PWM-pin to timer2: Clear OCR2B on Compare Match, set OC2B at BOTTOM, (non-inverting mode).
    bitClear(TCCR2A, COM2B1);
    bitClear(TCCR2A, COM2B0);
#elif ARDUINO_AVR_ATTINYX5
    //Connect PWM-pin to timer1: Clear OC1A on Compare Match, set OC1A at BOTTOM, (non-inverting mode).
    bitClear(TCCR1, COM1A1);
    bitClear(TCCR1, COM1A0);
#endif

}

void Timer::detachInterrupt()
{
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    bitClear(TIMSK2,TOIE2);             // timer interrupt off
#elif ARDUINO_AVR_ATTINYX5
    bitClear(TIMSK, TOIE1);
#endif
}

void Timer::attachInterrupt(void (*isr)())
{
  isrCallback = isr;                    // register the user's callback with the real ISR
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    bitSet(TIMSK2,TOIE2);               // timer interrupt on
#elif ARDUINO_AVR_ATTINYX5
    bitSet(TIMSK, TOIE1);               // timer interrupt on
#endif
}

void Timer::setPwmDuty(byte ocr)
{
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
    OCR2B = ocr;
#elif ARDUINO_AVR_ATTINYX5
    OCR1A = ocr;
#endif
}


//**************************************************************************
// Timer overflow interrupt service routine
// Interrupt frequency = F_CPU / 256
//      12MHz -> 46.875KHz
//      8MHz  -> 31.250KHz
// interrupt names can be found in Arduino folder: ./hardware/tools/avr/avr/include/avr/
//**************************************************************************
#ifdef ARDUINO_AVR_PROTRINKET3FTDI
ISR(TIMER2_OVF_vect)
#elif ARDUINO_AVR_ATTINYX5
ISR(TIMER1_OVF_vect)
#endif
{
    timer.isrCallback();
}


