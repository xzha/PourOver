#include "outputcompare.h"

/* Variables */
double timer2_period;                   // timer2 period
long int timer2_scaler;                 // timer2 clock prescale
float oc1_dutycycle;

double timer3_period;                   // timer3 period
long int timer3_scaler;                 // timer3 clock prescale
float oc2_dutycycle;

int power(int base, int exp) {
    int i;
    int result = 1;
    
    for (i = 0; i < exp; i ++)
        result *= base;
    
    return result;
}

void oc_dutycycle(float percentage) {
    float ratio = percentage / 100;
    oc1_dutycycle = percentage;
    OC1RS = (int) ratio * timer2_period;
}

void oc_frequency(long int frequency) {
    /* Calculations to determine new frequency: 
       PWMFREQUENCY = 1/PWMPERIOD
       PWMPERIOD = (timer2_period + 1) * 1/FCY * (timer2_scaler) */
    
    double period = 1.0/frequency;
    double temp = (period*FCY);

    if (temp <= (65535 + 1)) 
        timer2_scaler = 0b00;                  // timer prescale 1:1
    else if (temp <= (524280 + 8))
        timer2_scaler = 0b01;                  // timer prescale 1:8
    else if (temp <= (4194240 + 64))
        timer2_scaler = 0b10;                  // timer prescale 1:64
    else if (temp <= (16776960 + 256))
        timer2_scaler = 0b11;                  // timer prescale 1:256

    int scaler = power(8, timer2_scaler);
    timer2_period = temp / scaler;

    PR2 = (int) timer2_period--;
    T2CONbits.TCKPS = timer2_scaler;
    oc_dutycycle(oc1_dutycycle);
}