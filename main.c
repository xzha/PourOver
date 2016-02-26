#include "pourover.h"
#include<stdio.h>

float tempValue0;
extern float creep;
extern float currentMeas;
extern float oldMeas;

int tareFlag = 0;

void uart2_transmit(char s)
{
    while (U2STAbits.UTXBF);        // Wait for transmit buffer
    while (!U2STAbits.TRMT);        // Wait for transmitter  
    U2TXREG = s;                 // Place letter into transmit register
}

void uart2_transmit_string(char * s)
{
    while(*s)
    {
        uart2_transmit(*s++);
    }
}

void uart2_transmit_float(float f)
{
    char value[15];
    
    sprintf(value, "%f", f);
    
    uart2_transmit_string(value);
    uart2_transmit('\r');
    uart2_transmit('\n');
}

int main()
{
    /* Initialize to default values */
    oscillator_initialization();        // Clock:  8Mhz
    port_initialization();
    uart_initialization();              // UART1:  9600 Baud
    oc_initialization();                // OC1:    500Hz, 50% duty cycle (timer2)
                                        // OC2:    500Hz, 50% duty cycle (timer2)
                                        // OC3:    500Hz, 50% duty cycle (timer2)
                                        // OC4:    500Hz, 50% duty cycle (timer3)
    timer_initialization();             // timer1: 1Hz
                                        // timer2: 500Hz
                                        // timer3: 500Hz
    //bluetooth_initialization();
    
    DELAY_US(50);
    
    initialize();
    
    while(1)
    {
        
        if(tareFlag)
        {
            tareFlag = 0;
            tare(10);
            PORTDbits.RD10 = 0;
        }
        
        currentMeas = averageWeight();
        calcCreep();
        
        tempValue0 = currentMeas - creep;
        oldMeas = currentMeas;
        
        uart2_transmit_float(tempValue0);
        
        //setScale(CALIBRATION_FACTOR);
    }
    
    while(0)
    {
        powerUp();
        
        tare(5);
        
        tempValue0 = readData();
        
        tempValue0++;
        
        tempValue0 = readData();
        
        tempValue0++;
    }
    
    while(0)
    {
        char bt_var_handle = -1;

        // BLE connected
        if (PORTFbits.RF5) {
            if (receive_flag > 0) {
                if (buffer_check(&rx_buffer,"Connected") == 1)
                    continue;
                if (buffer_check(&rx_buffer, "Connection End") == 1)
                    continue;
                
                // BLE received data
                bt_var_handle = bluetooth_wv();
            }
        }
        
        char temp[20];
        
        // process received data (state machine?)
        switch (bt_var_handle) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                int_to_hexstring(read_temperature(), temp);
                bluetooth_shw(brew_temp.handle, temp);
                break;
            case 7:
                break;
            default:
                break;
        }
        
    }

    return(0);
}

/* Interrupts */ 
void __attribute__((__interrupt__)) _T1Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
}

int ledState = 0;
int buttonState = 0;

void __attribute__ ((__interrupt__)) _T2Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
    
    int reading = PORTDbits.RD11;
    
    if(reading != buttonState)
    {
        PORTDbits.RD10 = 1;
        
        buttonState = reading;
        
        tareFlag = 1;
    }
}

void __attribute__ ((__interrupt__)) _T3Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)
{
    IFS0bits.T3IF= 0;
}

void __attribute__((__interrupt__)) _U1TXInterrupt(void);
void __attribute__((__interrupt__, auto_psv)) _U1TXInterrupt(void)
{
    // enabling UTXEN bit will set UxTXIF flag (DS70000582E-page 16)
    IFS0bits.U1TXIF = 0;                // Clear TX Interrupt flag
}

void __attribute__((__interrupt__)) _U1RXInterrupt(void);
void __attribute__((__interrupt__, auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;                // Clear RX Interrupt flag
    if (U1STAbits.OERR)                 // Receive buffer overrun error
    {
        U1STAbits.OERR = 0;             // Clear error flag to keep UART receiving
        return;
    }
    
    if (U1STAbits.FERR)                 // Framing error
        return;

    if (U1STAbits.PERR)                 // Parity error
        return;
    
    if (U1STAbits.URXDA)                // Receive Buffer Data available
    {
        char r = U1RXREG;
        buffer_add_char(&rx_buffer, r);
        if (r == '\n')
            receive_flag++;
    }
}




// remove me
void __attribute__((__interrupt__)) _U2RXInterrupt(void);
void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
    char receive2;
    
    IFS1bits.U2RXIF = 0;                // Clear RX Interrupt flag
    if (U2STAbits.OERR)                 // Receive buffer overrun error
    {
        U2STAbits.OERR = 0;             // Clear error flag to keep UART receiving
        return;
    }
    
    if (U2STAbits.FERR)                 // Framing error
        return;

    if (U2STAbits.PERR)                 // Parity error
        return;
    
    if (U2STAbits.URXDA)                // Receive Buffer Data available
    {
        //receive_flag2 = 0;
        receive2 = U2RXREG;
    }
}

void __attribute__((__interrupt__)) _U2TXInterrupt(void);
void __attribute__((__interrupt__, auto_psv)) _U2TXInterrupt(void)
{
    // enabling UTXEN bit will set UxTXIF flag (DS70000582E-page 16)
    IFS1bits.U2TXIF = 0;                // Clear TX Interrupt flag
}