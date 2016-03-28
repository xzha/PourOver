#include "loadsensor.h"

int LOAD_GAIN = 1;
long LOAD_OFFSET = 0;
float LOAD_SCALE = 1.f;

float movingAvg[AVG];
float creepAvg[AVG];
float totalValue = 0;
float total = 0;
float diff = 0;

float currentMeas = 0;
float oldMeas = 0;
float creep = 0;

void ls_set_gain(int gain)
{
    switch(gain)
    {
        case 128:
            LOAD_GAIN = 1;
            break;
        case 64:
            LOAD_GAIN = 3;
            break;
        case 32:
            LOAD_GAIN = 2;
            break;
    }
    
    Tx_CLK;
    Port_CLK = 0;
    
    ls_ready_data();
}

void ls_power_up()
{
    Tx_CLK;
    Port_CLK = 0;
}

void ls_power_down()
{
    Tx_CLK;
    Port_CLK = 0;
    DELAY_US(1);
    Port_CLK = 1;
    DELAY_US(60);
}

char ls_shift_in()
{
    char val = 0;
    int index;
    Tx_CLK;
    Rx_DATA;
    
    for(index = 0; index < 8; index++)
    {
        Port_CLK = 1;
        DELAY_US(1);
        
        if(Port_DATA != 0)
        {
            val |= (1 << (7-index));
        }
        
        Port_CLK = 0;
    }
    
    return(val);
}

int ls_ready()
{
    Rx_DATA;
    
    return(Port_DATA != 1);
}

long ls_ready_data()
{
    char temp0, temp1, temp2;
    int index;
    char filler = 0x00;
    unsigned long value = 0;
    unsigned long value1 = 0, value2 = 0, value3 = 0, value4 = 0;
    
    while(!ls_ready());
    
    temp2 = ls_shift_in();
    temp1 = ls_shift_in();
    temp0 = ls_shift_in();
    
    DELAY_US(1);
    
    for(index = 0; index < LOAD_GAIN; index++)
    {
        Tx_CLK;
        Port_CLK = 1;
        DELAY_US(1);
        Port_CLK = 0;
    }
    
    temp2 = ~temp2;
    temp1 = ~temp1;
    temp0 = ~temp0;
    
    if(temp2 & 0x80)
    {
        filler = 0xFF;
    }
    else if((temp2 == 0x7F) && (temp1 == 0xFF) && (temp0 == 0xFF))
    {
        filler = 0xFF;
    }
    else
    {
        filler = 0x00;
    }
    
    value1 |= filler;
    value1 = (value1 << 24) & 0xFF000000;
    
    value2 |= temp2;
    value2 = (value2 << 16) & 0x00FF0000;
    
    value3 |= temp1;
    value3 = (value3 << 8) & 0x0000FF00;
    
    value4 |= temp0 & 0x000000FF;
    
    value = value1 | value2 | value3 | value4;
    
    return(value + 1);
}

long ls_read_average(int times)
{
    int index;
    long sum = 0;
    for(index = 0; index < times; index++)
    {
        sum += ls_ready_data();
    }
    
    return(sum / times);
}

double ls_get_value(int times)
{
    return(ls_read_average(times) - LOAD_OFFSET);
}

float ls_get_units(int times)
{
    return(ls_get_value(times) / LOAD_SCALE);
}

void ls_tare(int times)
{
    int i;
    
    for(i = 0; i < AVG; i++)
    {
        movingAvg[i] = 0;
        creepAvg[i] = 0;
    }
    
    long sum = ls_read_average(times);
    ls_set_offset(sum);
}

void ls_set_scale(float scale)
{
    LOAD_SCALE = scale;
}

float ls_get_scale()
{
    return(LOAD_SCALE);
}

void ls_set_offset(long offset)
{
    LOAD_OFFSET = offset;
}

long ls_get_offset()
{
    return(LOAD_OFFSET);
}

float ls_average_weight()
{
    unsigned int i;
    
    totalValue = ls_get_units(1);
    
    total = 0;
    
    for(i = 0; i < (AVG - 1); i++)
    {
        movingAvg[i] = movingAvg[i + 1];
        total += movingAvg[i];
    }
    
    movingAvg[AVG - 1] = totalValue;
    
    total += totalValue;
    total /= AVG;
    
    return(total);
}

void ls_initialization()
{
    unsigned int i;
    
    ls_power_up();
    
    for(i = 0; i < AVG; i++)
    {
        movingAvg[i] = 0;
        creepAvg[i] = 0;
    }
    
    ls_set_scale(CALIBRATION_FACTOR);
    DELAY_MS(10);
    ls_tare(50);
    DELAY_MS(10);
}

void ls_calc_creep()
{
    unsigned int i;
    float tempCreep;
    
    if((currentMeas-creep) > 0.24 && (currentMeas - creep) < -0.24)
    {
        for(i=0;i < AVG; i ++)
        {
            creepAvg[i] = creepAvg[AVG-1] + (currentMeas-creep);
        }
    }
    
    diff = currentMeas - oldMeas;
    tempCreep = 0;
    
    if(diff < 0.0)
    {
        diff *= -1;
    }
    
    if(diff < .25)
    {
        for(i = 0; i < (AVG - 1); i++)
        {
            creepAvg[i] = creepAvg[i + 1];
            tempCreep += creepAvg[i];
        }
        
        creepAvg[AVG - 1] = creepAvg[AVG - 1] + (currentMeas - oldMeas);
        
        tempCreep += creepAvg[AVG - 1];
        tempCreep /= AVG;
        
        creep = tempCreep;
    }
}