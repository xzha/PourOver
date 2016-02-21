/*
 * File:   temperature.c
 * Author: EE63pc6-user
 *
 * Created on February 20, 2016, 2:47 PM
 */

#include "temperature.h"
#include "definitions.h"

char Reset_18B20()
{
    Tx_18B20;                                               // Set pin to output
    Port_18B20 = 0;                                         // Output a 0
    DELAY_US(480);                                          // Timing requirement
    Rx_18B20;                                               // Set pin to input
    DELAY_US(60);                                           // Timing requirement
    
    if(Port_18B20 == 0)
    {
        DELAY_US(480);                                      // Timing requirement
        return(0);                                          // One-Wire is present
    }
    else
    {
        DELAY_US(480);                                      // Timing requirement
        return(1);                                          // One-Wire is not present
    }
}
void Write_18B20(char command)
{
    char i;
    Rx_18B20;                                               // Set pin to input
    
    for(i = 0; i < 8; i++)
    {
        if((command & (1 << i)) != 0)
        {
                                                            // Write a 1
            Tx_18B20;                                       // Set pin to output
            Port_18B20 = 0;                                 // Output a 0
            DELAY_US(1);                                    // Timing requirement
            Rx_18B20;                                       // Set pin to input
            DELAY_US(60);                                   // Timing requirement
        }
        else
        {
                                                            // Write a 0
            Tx_18B20;                                       // Set pin to output
            Port_18B20 = 0;                                 // Output a 0
            DELAY_US(60);                                   // Timing requirement
            Rx_18B20;                                       // Set pin to input
        }
    }
}

char Read_18B20()
{
    char i;
    char result = 0;
    Rx_18B20;                                               // Set pin to input
    
    for(i = 0; i < 8; i++)
    {
        Tx_18B20;                                           // Set pin to output
        Port_18B20 = 0;                                     // Output a 0
        DELAY_US(2);                                        // Only output for a short amount of time
        Rx_18B20;                                           // Set pin to input
        
        DELAY_US(5);                                        // Wait to allow slave to write correct value
        
        if(Port_18B20 != 0)                                 // Set value according to value read in
        {
            result |= (1 << i);
        }
        
        DELAY_US(60);                                       // Timing requirement
    }
    
    return(result);
}


TEMPRETURN readTemperature()
{
    int tempLow, tempHigh;
    int temperature;
    
#ifdef TEMPFLOAT
    float finalTemperature;
    float fractionTemperature;
#endif

    if(!Reset_18B20())
    {
                                                            // A device was found on the One-Wire bus
        Reset_18B20();                                      // Send a reset to signal starting communication
        Write_18B20(SKIP_ROM);                              // Skip the ROM check since there is only one slave on the bus
        Write_18B20(CONVERT_T);                             // Tell the device to convert the temperature
        DELAY_MS(1000);                                     // Wait for the temperature conversion

        Reset_18B20();                                      // Send a reset to signal starting communication
        Write_18B20(SKIP_ROM);                              // Skip the ROM check since there is only one slave on the bus
        Write_18B20(READ_SCRATCHPAD);                       // Allow the master to read the contents of the scratchpad

        tempLow = Read_18B20() & 0x00FF;                    // Read a byte from the bus (The LS temperature register)
        tempHigh = Read_18B20() & 0x00FF;                   // Read a byte from the bus (The MS temperature register)
        temperature = (tempHigh << 8) | tempLow;            // Combine the two bytes that were read in
        temperature = temperature >> 4;                     // Divide by 16 to truncate the decimals
        
#ifdef TEMPFLOAT
                                                            // Perform floating point operations
        fractionTemperature = (float) (tempLow & 0x000F) / 16.0;
        finalTemperature = (float) temperature + fractionTemperature;
#else
        temperature = temperature + !!(tempLow & 0x0008);   // If the fraction is >= 0.5, round up
#endif 
    }
    else
    {
                                                            // Error, device not found
        return(ERROR_TEMP);
    }
    
#ifdef TEMPFLOAT
    return(finalTemperature);                                      // Return a float
#else
    return(temperature);                                    // Return an int
#endif
}