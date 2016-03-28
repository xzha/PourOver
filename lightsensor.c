#include "lightsensor.h"

int get_light_value()
{
    int *ADC16Ptr;  // pointer for reading ADC buffer
    int light_value, count;
    
    light_value = 0;
    ADC16Ptr = (int *) &ADC1BUF0;   //start on buffer 0 and loop up
    IFS0bits.AD1IF = 0; // clear ADC interrupt flag
    AD1CON1bits.ASAM = 1; // auto start sampling for 31Tad
                          // then go to conversion
    
    while (!IFS0bits.AD1IF){}; // conversion done?
    
    AD1CON1bits.ASAM = 0; // yes then stop sample/convert
    
    for (count = 0; count < 16; count++) // average the 16 ADC value
    {
        light_value = light_value + *ADC16Ptr++;
    }
    
    return light_value >> 4;
}