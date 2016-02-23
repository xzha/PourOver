#include "temperature.h"

/* Resets the devices on the One-Wire Bus */
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

/* Writes 8 bits to the One-Wire Bus */
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

/* Reads 8 bits from the slave on the One-Wire Bus */
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

/* Reads the temperature from the temperature probe */
int read_temperature()
{
    int tempLow, tempHigh;
    int temperature;
    
    float finalTemperature;
    float fractionTemperature;

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
        
                                                            // Perform floating point operations (and convert to Farenheit)
        fractionTemperature = (float) (tempLow & 0x000F) / 16.0;
        finalTemperature = ((float) temperature + fractionTemperature) * 1.8 + 32.0;
    }
    else
    {                                           
        return(ERROR_TEMP);                                 // Error, device not found
    }
    
    return((int) finalTemperature);                         // Return an int
}