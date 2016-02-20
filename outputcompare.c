#include "outputcompare.h"

/* Variables */
double timer2_period = 7999;            // timer2 period
double timer3_period = 7999;            // timer3 period

float oc1_dutycycle = 50.0;
float oc2_dutycycle = 50.0;
float oc3_dutycycle = 50.0;
float oc4_dutycycle = 50.0;

int power(int base, int exp) {
    int i;
    int result = 1;
    
    for (i = 0; i < exp; i ++)
        result *= base;
    
    return result;
}

void oc_frequency(long int frequency, char ocn) {
    /* Calculations to determine new frequency: 
       PWMFREQUENCY = 1/PWMPERIOD
       PWMPERIOD = (timer2_period + 1) * 1/FCY * (timer2_scaler) */
    
    int timer_scaler;
    double timer_period;
    double period = 1.0/frequency;
    double temp = (period*FCY);
    
    if (temp <= (65535 + 1)) 
        timer_scaler = 0b00;                  // timer prescale 1:1
    else if (temp <= (524280 + 8))
        timer_scaler = 0b01;                  // timer prescale 1:8
    else if (temp <= (4194240 + 64))
        timer_scaler = 0b10;                  // timer prescale 1:64
    else if (temp <= (16776960 + 256))
        timer_scaler = 0b11;                  // timer prescale 1:256

    int scaler = power(8, timer_scaler);
    timer_period = temp / scaler;

    switch(ocn) {
        case 1: 
        case 2: 
        case 3: PR2 = (int) timer_period --;
                T2CONbits.TCKPS = timer_scaler; 
                timer2_period = timer_period;
                break;
        case 4: PR3 = (int) timer_period --; 
                T3CONbits.TCKPS = timer_scaler;
                timer3_period = timer_period;
                break;
    }
    
    switch(ocn) {
        case 1: oc_dutycycle(oc1_dutycycle, 1); break;
        case 2: oc_dutycycle(oc2_dutycycle, 2); break;
        case 3: oc_dutycycle(oc3_dutycycle, 3); break;
        case 4: oc_dutycycle(oc4_dutycycle, 4); break;
    }
}

void oc_dutycycle(float percentage, char ocn) {
    float ratio = percentage / 100;
    
    switch(ocn) {
        case 1: OC1RS = (int) (ratio * timer2_period);
                oc1_dutycycle = percentage;
                break;
        case 2: OC2RS = (int) (ratio * timer2_period);
                oc2_dutycycle = percentage;
                break;
        case 3: OC3RS = (int) (ratio * timer2_period);
                oc3_dutycycle = percentage;
                break;
        case 4: OC4RS = (int) (ratio * timer3_period);
                oc4_dutycycle = percentage;
                break;
    }
}