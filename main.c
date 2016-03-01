#include "pourover.h"

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
    ls_initialization();
    // pwm intialization
    timer_frequency(500, 2); // pump    @ timer2
    timer_frequency(50, 3);  // servo1  @ timer3
                             // servo2  @ timer3 
                             // ssrelay @ timer3
    // pwm initialization
    
    while(1)
    {
        char bt_var_handle = -1;

        // BLE connected
        if (PORTFbits.RF5) {
            if (receive_flag > 0) {
                if (buffer_check(&rx_buffer,"Connected") == 1)
                    continue;
                else if (buffer_check(&rx_buffer, "Connection End") == 1)
                    continue;
                
                // BLE received data
                bt_var_handle = bluetooth_wv();
            }
        }
        
        // process received data (state machine?)
        switch (bt_var_handle) {
            case 0: //start_brew
                oc_dutycycle(0, 1); // pump
                oc_dutycycle(0, 2); // servo1
                oc_dutycycle(0, 3); // servo2
                break;
            case 1:
                break;
            case 2: //brew_temp 7975652bf2a24f73a2da429ac3a83dfb
                oc_dutycycle(50, 1); // pump
                oc_dutycycle(15, 2); // servo1
                oc_dutycycle(10, 3); // servo2
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6: //brew_strength dbfde0ac2cf241269759042cd13e5681
                oc_dutycycle(25, 1); // pump
                oc_dutycycle(10, 2);  // servo1
                oc_dutycycle(5, 3);  // servo2
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
