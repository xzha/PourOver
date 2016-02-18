// PIC24FJ128GA010's header
#include <xc.h>
#include "configurationbits.h"
#include "definitions.h"
#include "outputcompare.h"
#include "buffer.h"

/* Variables */
volatile buffer tx_buffer;
volatile buffer rx_buffer;
volatile char receive_flag;

/* Initializations */
void oscillator_initialization(void) {
    /* Oscillator */
    asm volatile ("mov #OSCCON, W1");
    asm volatile ("mov.b #0x46, W2");   // Unlock sequence
    asm volatile ("mov.b #0x57, W3");
    asm volatile ("mov.b #0x02, W0");   // SOSCEN = 1
    asm volatile ("mov.b W2, [W1]");
    asm volatile ("mov.b W3, [W1]");
    asm volatile ("mov.b W0, [W1]");
    
    /* Clock  */
    CLKDIVbits.RCDIV = 0b000;           // Divide by 1 (8MHz)
}

void timer_initialization(void) {
    /* Timer1 */
    T1CON = 0x00;                       // Stop the timer1 and reset control reg.

    TMR1 = 0x00;                        // Clear contents of timer register
    PR1 = 0x007D;                       // Period Register (125)

    T1CONbits.TCS = 1;                  // External 32.78KHz oscillator 
    T1CONbits.TCKPS = 0b11;             // 1:256 Prescale (32KHz/256 = 125Hz)

    T1CONbits.TON = 1;                  // Start 16-bit timer1 (1Hz)
    
    IFS0bits.T1IF = 0;                  // clear timer1 interrupt status flag
    IEC0bits.T1IE = 1;                  // enable timer1 interrupt
    
    /* Timer2 */
    T2CON = 0x00;                       // Stop the timer2 and reset control reg.
    
    TMR2 = 0x00;                        // Clear contents of timer register
    PR2 = 0x1F3F;                       // Period Register (7999)
    
    T2CONbits.TCS = 0;                  // Internal 4Mhz instruction cycle (FCY)
    T2CONbits.TCKPS = 0;                // 1:1 Prescale (4Mhz/1 = 4Mhz)
    T2CONbits.T32 = 0;                  // Independent 16-bit timer

    T2CONbits.TON = 1;                  // Start 16-bit timer2 (500Hz)
}

void port_initialization(void) {
    /* Port C */
    TRISCbits.TRISC1 = 0;               // RC1 as output (BT module wake pin)
}

void uart_initialization(void) {
    /* UART1 */
    U1MODE = 0;                         // Clear UART1 mode register
    U1STA = 0;                          // Stop UART1 and clear status register
    U1BRG = BAUD_9600;                  // Baud rate
    
    U1MODE = 0x8000;                    // Enable UART, no flow control, BRGH = 1 for high speed baud mode

    U1STAbits.UTXEN = 1;                // Enable Transmit

    U2STAbits.URXISEL = 0;              // Interrupt when receive buffer is 1/4 full
    IFS0bits.U1TXIF = 0;                // Clear TX Interrupt flag
    IFS0bits.U1RXIF = 0;                // Clear TX Interrupt flag
    IEC0bits.U1RXIE = 1;                // Enable Receive Interrupt
    
    DELAY_MS(1);
}

void oc_initialization(void) {
    OC1CONbits.OCM = 0b000;             // Disable Output Compare Module
    OC1CONbits.OCTSEL = 0;              // Select Timer 2 as output compare time base

    OC1RS = 0x0FA0;                     // Normalized duty cycle
    OC1R = 0xFA00;                      // Initial duty cycle
    OC1CONbits.OCM = 0b110;             // Output compare operation mode

    IFS0bits.T2IF = 0;                  // Clear Output Compare 1 interrupt flag
    IEC0bits.T2IE = 1;                  // Enable Output Compare 1 interrupts
}

int main()
{
    /* Initialize to default values */
    oscillator_initialization();        // Clock:  8Mhz
    port_initialization();
    uart_initialization();              // UART1:  9600 Baud
    oc_initialization();                // OC1:    500Hz, 50% duty cycle
    timer_initialization();             // timer1: 1Hz
                                        // timer2: 500Hz
    
    /* Initialize buffer */
    buffer_init(&tx_buffer);
    buffer_init(&rx_buffer);
    receive_flag = 0;
    
    PORTCbits.RC1 = 1;                               // Wake up BT module
    DELAY_MS(200);                                   // Wait for BT to wake up
    while(!buffer_check("CMD"));

    /* Initialize bluetooth module */
    buffer_transmit_set("sn", "PourOver-4125");
    buffer_transmit_set("sr", "20004000");
    buffer_transmit_set("ss", "00000001");
    while(!buffer_transmit_check("pz", "AOK"));
    
    // coffee machine service
    while(!buffer_transmit_check("ps,b80600992804496d81a7c1c3807826c0", "AOK"));    
    
    // characteristics
    while(!buffer_transmit_check("pc,75dc2f8004f242f48ab048d642153c91,0A,01", "AOK")); // start brew
    while(!buffer_transmit_check("pc,d2025d7957084ff1bc76c01e1abebb4d,02,05", "AOK")); // brew state
    while(!buffer_transmit_check("pc,538c13e23739428086ac91ab935a6ce1,02,05", "AOK")); // water level
    while(!buffer_transmit_check("pc,67b0653508394365bf7f8afc631e54a1,02,05", "AOK")); // bean level
    while(!buffer_transmit_check("pc,7975652bf2a24f73a2da429ac3a83dfb,0A,10", "AOK")); // brew temperature
    while(!buffer_transmit_check("pc,dbfde0ac2cf241269759042cd13e5681,0A,10", "AOK")); // brew strength
    while(!buffer_transmit_check("pc,1cf1a1b203bb4f7ca28a8881169bede5,0A,10", "AOK")); // brew size
    while(!buffer_transmit_check("pc,6ced5f74565c4608ba3d043f4b0297f9,00,20", "AOK")); // brew schedule?

    // reboot
    buffer_transmit("r,1");
    DELAY_MS(3000);
    uart_initialization();
    buffer_empty(&tx_buffer);
    buffer_empty(&rx_buffer);
    
    while(1)
    {
        // BLE REQUEST
        if (receive_flag) {
            receive_flag = 0;
        }
        
        // BLE write on timer
    }

    return 0;
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