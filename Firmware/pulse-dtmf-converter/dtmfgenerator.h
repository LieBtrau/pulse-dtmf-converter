#ifndef DTMFGENERATOR_H
#define DTMFGENERATOR_H
#include "Arduino.h"
#include <string.h>

#define  Xtal       8000000          // system clock frequency
#define  prescaler  1                // timer1 prescaler
#define  N_samples  128              // Number of samples in lookup table
#define  Fck        Xtal/prescaler   // Timer1 working frequency
#define  delaycyc   10               // port B setup delay cycles

class DtmfGenerator
{
public:
    DtmfGenerator();
    void init (void);
    bool generateTone(char key);
    void stopTone();
    //************************** SIN TABLE *************************************
    // Samples table : one period sampled on 128 samples and,
    // quantized on 7 bit
    //**************************************************************************
private:
    //https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling
    const char* keypad = "123A456B789C*0#D"; // position for lookup into Freq. Table
    //***************************  x_SW  ***************************************
    //Table of x_SW (excess 8): x_SW = ROUND(8*N_samples*f*510/Fck)
    //**************************************************************************
    //high frequency (column)
    //1209hz  ---> x_SW = 79
    //1336hz  ---> x_SW = 87
    //1477hz  ---> x_SW = 96
    //1633hz  ---> x_SW = 107
    const byte auc_frequencyCol [4] = {79,87,96,107};      //todo: check if order is correctly

    //low frequency (row)
    //697hz  ---> x_SW = 46
    //770hz  ---> x_SW = 50
    //852hz  ---> x_SW = 56
    //941hz  ---> x_SW = 61

    const byte auc_frequencyRow [4] = {46,50,56,61};       //todo: check if order is correctly

};

#endif // DTMFGENERATOR_H
