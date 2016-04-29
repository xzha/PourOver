#include "outputcompare.h"

/* Variables */
double timer2_period = 0;               // timer2 period
double timer3_period = 0;               // timer3 period

float oc1_dutycycle = 0.0;              // pump
float oc2_dutycycle = 0.0;              // servo1
float oc3_dutycycle = 0.0;              // servo2
float oc4_dutycycle = 0.0;              // ss relay

int servo1_pos = 0;
int servo2_pos = 0;

int power(int base, int exp) {
    int i;
    int result = 1;
    
    for (i = 0; i < exp; i ++)
        result *= base;
    
    return result;
}

int abs(int x) {
    if (x < 0)
        return x * -1;
    return x;
}

void pwm_initialization() {
    timer_frequency(500, 2); // pump    @ timer2
    timer_frequency(50, 3);  // servo1  @ timer3, shoulder
                             // servo2  @ timer3, arm
                             // ssrelay @ timer3
    
    // arm and shoulder initialization
    set_servo(7, 1);
    set_servo(80, 2);
}

void timer_frequency(long int frequency, char tn) {
    /* Calculations to determine new frequency: 
       PWMFREQUENCY = 1/PWMPERIOD
       PWMPERIOD = (timer2_period + 1) * 1/FCY * (timer2_scaler) */
    
    if (tn != 2 && tn != 3)
        return;
    
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
    timer_period = temp / scaler - 1;

    switch(tn) {
        case 2: 
                // change frequency
                PR2 = (int) timer_period;
                T2CONbits.TCKPS = timer_scaler; 
                timer2_period = timer_period;
                
                // change duty cycle accordingly
                oc_dutycycle(oc1_dutycycle, 1);
                
                break;
        case 3: 
                // change frequency
                PR3 = (int) timer_period; 
                T3CONbits.TCKPS = timer_scaler;
                timer3_period = timer_period;
                
                // change duty cycle accordingly
                oc_dutycycle(oc2_dutycycle, 2);
                oc_dutycycle(oc3_dutycycle, 3);
                oc_dutycycle(oc4_dutycycle, 4);
                break;
    }
}

void oc_dutycycle(float percentage, char ocn) {
    float ratio = percentage / 100.0;
    
    switch(ocn) {
        case 1: OC1RS = (int) (ratio * timer2_period);
                oc1_dutycycle = percentage;
                break;
        case 2: OC2RS = (int) (ratio * timer3_period);
                oc2_dutycycle = percentage;
                break;
        case 3: OC3RS = (int) (ratio * timer3_period);
                oc3_dutycycle = percentage;
                break;
        case 4: OC4RS = (int) (ratio * timer3_period);
                oc4_dutycycle = percentage;
                break;
    }
}

void set_servo(int pos, char sn) {
    float percentage; 
    
    percentage = (pos / 180.0) * 10;
    percentage += 3;
    
    // 2 == shoulder, 3 == arm
    if (sn == 1) {
        servo1_pos = pos;
        oc_dutycycle(percentage, 2);
        
    }
    else if (sn == 2) {
        servo2_pos = pos;
        oc_dutycycle(percentage, 3);
    }
}

void move_servo(int x, int y, int delay) {
    int i;
    int j;    
    int mod;
    int ratio;
    int delta_x;
    int delta_y;
    
    delta_x = x - servo1_pos;
    delta_y = y - servo2_pos;
    
    if (delta_x == 0 || delta_y == 0) {
        return;
    }
    
    if (abs(delta_x) > abs(delta_y)) {
        ratio = abs(delta_x) / abs(delta_y);
        mod = abs(delta_x) % abs(delta_y);
        
        for (i = 0; i < abs(delta_y); i++) {
            set_servo(servo2_pos + (delta_y/abs(delta_y)), 2);
            DELAY_MS(delay);
            
            for (j = 0; j < ratio; j++) {
                set_servo(servo1_pos + (delta_x/abs(delta_x)), 1);
                DELAY_MS(delay);
            }
            
            if (mod) {
                set_servo(servo1_pos + (delta_x/abs(delta_x)), 1);
                DELAY_MS(delay);
                mod--;
            }
        }
        
    } else {
        ratio = abs(delta_y) / abs(delta_x);
        mod = abs(delta_y) % abs(delta_x);
        
        for (i = 0; i < abs(delta_x); i++) {
            set_servo(servo1_pos + (delta_x/abs(delta_x)), 1);
            DELAY_MS(delay);
            
            for (j = 0; j < ratio; j++) {
                set_servo(servo2_pos + (delta_y/abs(delta_y)), 2);
                DELAY_MS(delay);
            }
            
            if (mod) {
                set_servo(servo2_pos + (delta_y/abs(delta_y)), 2);
                DELAY_MS(delay);
                mod--;
            }
        }
    }
}

void move_servo_circular(int delay) {
    move_servo(38, 150, delay);
    move_servo(44, 160, delay);
    move_servo(51, 150, delay);
    move_servo(7, 80, delay);
}