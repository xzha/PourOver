// PIC24FJ128GA010's header
#include <xc.h>
#include "configurationbits.h"
#include "definitions.h"
#include "initializations.h"
#include "outputcompare.h"
#include "buffer.h"
#include "temperature.h"

/* Variables */
volatile buffer tx_buffer;
volatile buffer rx_buffer;
volatile char receive_flag = 0;

TEMPRETURN temperatureValue;

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
    bluetooth_initialization();
    
    while(1)
    {
        // BLE REQUEST
        if (PORTFbits.RF5) {
            // receive_flag = 0; 
            oc_frequency(1000,1);
            oc_dutycycle(15,1);
            
            oc_dutycycle(25,2);
            
            oc_dutycycle(65,3);
            
            oc_frequency(2500,4);
            oc_dutycycle(85,4);
        }
       
        // Grab the temperature from the temperature probe and check for any errors
        temperatureValue = readTemperature();
        
        // discard value, do not send to BT
        if (temperatureValue == ERROR_TEMP) {
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

void __attribute__ ((__interrupt__)) _T2Interrupt(void);
void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF= 0;
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
            receive_flag = 1;
    }
}