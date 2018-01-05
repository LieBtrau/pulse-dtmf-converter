//***************************************************************************
//* A P P L I C A T I O N   N O T E   F O R   T H E   A V R   F A M I L Y
//*
//* Number               : AVR314
//* File Name            : "dtmf.c"
//* Title                : DTMF Generator
//* Date                 : 00.06.27
//* Version              : 1.0
//* Target MCU           : Any AVR with SRAM, 8 I/O pins and PWM
//*
//* DESCRIPTION
//* This Application note describes how to generate DTMF tones using a single
//* 8 bit PWM output.
//*
//***************************************************************************

#include "dtmfgenerator.h"

static const byte auc_SinParam [128] =
{
    64,67,70,73,76,79,82,85,88,91,94,96,99,102,104,106,109,111,113,115,117,118,120,
    121,123,124,125,126,126,127,127,127,127,127,127,127,126,126,125,124,123,121,120,
    118,117,115,113,111,109,106,104,102,99,96,94,91,88,85,82,79,76,73,70,67,64,60,57,
    54,51,48,45,42,39,36,33,31,28,25,23,21,18,16,14,12,10,9,7,6,4,3,2,1,1,0,0,0,0,0,
    0,0,1,1,2,3,4,6,7,9,10,12,14,16,18,21,23,25,28,31,33,36,39,42,45,48,51,54,57,60
};

//**************************  global variables  ****************************
byte x_SWa = 0x00;               // step width of high frequency
byte x_SWb = 0x00;               // step width of low frequency
unsigned int  i_CurSinValA = 0;           // position freq. A in LUT (extended format)
unsigned int  i_CurSinValB = 0;           // position freq. B in LUT (extended format)
unsigned int  i_TmpSinValA;               // position freq. A in LUT (actual position)      //todo: might be byte
unsigned int  i_TmpSinValB;               // position freq. B in LUT (actual position)      //todo: might be byte


DtmfGenerator::DtmfGenerator()
{

}

//**************************************************************************
// Initialization
//**************************************************************************
void DtmfGenerator::init (void)
{
    noInterrupts();                     // disable all interrupts
    //Clear OC2B on Compare Match, set OC2B at BOTTOM,(non-inverting mode).
    bitSet(TCCR2A, COM2B1);
    bitClear(TCCR2A, COM2B0);
    //Waveform Generation Mode Bit Description : Fast PWM = mode 3
    //Interrupts every 21.3µs
    //f(OCR2B)=fclk/(N*256) = 12M/256 = 46.9KHz
    bitClear(TCCR2B,WGM22);
    bitSet(TCCR2A,WGM21);
    bitSet(TCCR2A,WGM20);
    //Clock select : No prescaling
    bitClear(TCCR2B, CS22);
    bitClear(TCCR2B, CS21);
    bitSet(TCCR2B, CS20);
    pinMode(3, OUTPUT);                 // OCR2B-pin
    interrupts();                       // Interrupts enabled
    bitClear(TIMSK2,TOIE0);
}

bool DtmfGenerator::generateTone(char key)
{
    char* pch= strchr(keypad, key); //get position
    if(!pch)
    {
        return false;
    }
    byte digitpos=pch-keypad;
    x_SWa = auc_frequencyCol[digitpos & 0x03];      // column of 4x4 DTMF Table
    x_SWb = auc_frequencyRow[digitpos >> 2];        // row of DTMF Table
    bitSet(TIMSK2,TOIE2); // timer interrupt on
    return true;
}

void DtmfGenerator::stopTone()
{
    //tone off
    bitClear(TIMSK2,TOIE2);
}

//**************************************************************************
// Timer overflow interrupt service routine
// interrupt names can be found in Arduino folder: ./hardware/tools/avr/avr/include/avr/
// http://interface.khm.de/index.php/lab/interfaces-advanced/arduino-dds-sinewave-generator/
//**************************************************************************
ISR(TIMER2_OVF_vect)
{
    // move Pointer about step width ahead
    i_CurSinValA += x_SWa;
    i_CurSinValB += x_SWb;
    // normalize Temp-Pointer
    i_TmpSinValA  =  (char)(((i_CurSinValA+4) >> 3)&(0x007F));
    i_TmpSinValB  =  (char)(((i_CurSinValB+4) >> 3)&(0x007F));
    // calculate PWM value: high frequency value + 3/4 low frequency value
    OCR2B = auc_SinParam[i_TmpSinValA];// + (auc_SinParam[i_TmpSinValB]-(auc_SinParam[i_TmpSinValB]>>2));
}
