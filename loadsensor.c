#include "loadsensor.h"

int LOAD_GAIN = 1;
long LOAD_OFFSET = 0;
float LOAD_SCALE = 1.f;

float movingAvg[AVG];
float creepAvg[AVG];
float totalValue = 0;
float total = 0;
float diff = 0;

//extern float creep;
//extern float currentMeas;
//extern float oldMeas;
float currentMeas = 0;
float oldMeas = 0;
float creep = 0;

void setGain(int gain)
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
    
    readData();
}

void powerUp()
{
    Tx_CLK;
    Port_CLK = 0;
}

void powerDown()
{
    Tx_CLK;
    Port_CLK = 0;
    DELAY_US(1);
    Port_CLK = 1;
    DELAY_US(60);
}

char shiftIn()
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
            //val |= (1 << index);
        }
        
        Port_CLK = 0;
    }
    
    return(val);
}

int isReady()
{
    Rx_DATA;
    
    return(Port_DATA != 1);
}

long readData()
{
    char temp0, temp1, temp2;
    int index;
    char filler = 0x00;
    unsigned long value = 0;
    unsigned long value1 = 0, value2 = 0, value3 = 0, value4 = 0;
    
    while(!isReady());
    
    temp2 = shiftIn();
    temp1 = shiftIn();
    temp0 = shiftIn();
    
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
    
//    value = (((unsigned long) filler) << 24)
//            | (((unsigned long) temp2) << 16)
//            | (((unsigned long) temp1) << 8)
//            | ((unsigned long) temp0);
    
    return(value + 1);
}

long readAverage(int times)
{
    int index;
    long sum = 0;
    for(index = 0; index < times; index++)
    {
        sum += readData();
    }
    
    return(sum / times);
}

double getValue(int times)
{
    return(readAverage(times) - LOAD_OFFSET);
}

float getUnits(int times)
{
    return(getValue(times) / LOAD_SCALE);
}

void tare(int times)
{
    int i;
    
    for(i = 0; i < AVG; i++)
    {
        movingAvg[i] = 0;
        creepAvg[i] = 0;
    }
    
    long sum = readAverage(times);
    setOffset(sum);
}

void setScale(float scale)
{
    LOAD_SCALE = scale;
}

float getScale()
{
    return(LOAD_SCALE);
}

void setOffset(long offset)
{
    LOAD_OFFSET = offset;
}

long getOffset()
{
    return(LOAD_OFFSET);
}

float averageWeight()
{
    unsigned int i;
    
    totalValue = getUnits(1);
    
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

void initialize()
{
    unsigned int i;
    
    powerUp();
    
    for(i = 0; i < AVG; i++)
    {
        movingAvg[i] = 0;
        creepAvg[i] = 0;
    }
    
    setScale(CALIBRATION_FACTOR);
    DELAY_MS(10);
    tare(50);
    DELAY_MS(10);
}

void calcCreep()
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