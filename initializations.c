#include "initializations.h"

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
    
    /* Timer3 */
    T3CON = 0x00;                       // Stop the timer3 and reset control reg.
    
    TMR3 = 0x00;                        // Clear contents of timer register
    PR3 = 0x1F3F;                       // Period Register (7999)
    
    T3CONbits.TCS = 0;                  // Internal 4Mhz instruction cycle (FCY)
    T3CONbits.TCKPS = 0;                // 1:1 Prescale (4Mhz/1 = 4Mhz)

    T3CONbits.TON = 1;                  // Start 16-bit timer3 (500Hz)
}

void port_initialization(void) {
    /* Port B */
    AD1PCFGbits.PCFG2 = 1;              // AN2 as digital pin
    AD1PCFGbits.PCFG3 = 1;              // AN3 as digital pin
    
    TRISBbits.TRISB3 = 0;               // LED Power as output
    TRISBbits.TRISB2 = 1;               // Push Button sense as input
    
    PORTBbits.RB3 = 0;                  // Turn off PB LED
    
    /* Port F */
    TRISFbits.TRISF4 = 0;               // RF4 as output (BT module wake pin)
    TRISFbits.TRISF5 = 1;               // RF5 as input (BT module connection pin)
    
    PORTFbits.RF4 = 1;                  // Wake up BT module
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
    /* PWM1 for pump */
    OC1CONbits.OCM = 0b000;             // Disable Output Compare Module
    OC1CONbits.OCTSEL = 0;              // Select Timer 2 as output compare time base

    OC1RS = 0x0000;                     // Normalized duty cycle (0)
    OC1R = 0x0000;                      // Initial duty cycle (0)

    IFS0bits.OC1IF = 0;                 // Clear Output Compare 1 interrupt flag
    IEC0bits.OC1IE = 1;                 // Enable Output Compare 1 interrupts
    
    OC1CONbits.OCM = 0b110;             // Output compare operation mode
    
    /* PWM2 for servo 1 */
    OC2CONbits.OCM = 0b000;             // Disable Output Compare Module
    OC2CONbits.OCTSEL = 1;              // Select Timer 3 as output compare time base

    OC2RS = 0x0000;                     // Normalized duty cycle (0)
    OC2R = 0x0000;                      // Initial duty cycle (0)

    IFS0bits.OC2IF = 0;                 // Clear Output Compare 2 interrupt flag
    IEC0bits.OC2IE = 1;                 // Enable Output Compare 2 interrupts
    
    OC2CONbits.OCM = 0b110;             // Output compare operation mode
    
    /* PWM3 for servo 2 */
    OC3CONbits.OCM = 0b000;             // Disable Output Compare Module
    OC3CONbits.OCTSEL = 1;              // Select Timer 3 as output compare time base

    OC3RS = 0x0000;                     // Normalized duty cycle (0)
    OC3R = 0x0000;                      // Initial duty cycle (0)

    IFS1bits.OC3IF = 0;                 // Clear Output Compare 3 interrupt flag
    IEC1bits.OC3IE = 1;                 // Enable Output Compare 3 interrupts
    
    OC3CONbits.OCM = 0b110;             // Output compare operation mode
    
    /* PWM4 for SS Relay */
    OC4CONbits.OCM = 0b000;             // Disable Output Compare Module
    OC4CONbits.OCTSEL = 1;              // Select Timer 3 as output compare time base

    OC4RS = 0x0000;                     // Normalized duty cycle (0)
    OC4R = 0x0000;                      // Initial duty cycle (0)

    IFS1bits.OC4IF = 0;                 // Clear Output Compare 4 interrupt flag
    IEC1bits.OC4IE = 1;                 // Enable Output Compare 4 interrupts
    
    OC4CONbits.OCM = 0b110;             // Output compare operation mode
}