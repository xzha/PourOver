#include "initializations.h"

void oscillator_initialization(void) {
    /* Primary oscillator */
    asm volatile ("DISI #0x3FFF");
    __builtin_write_OSCCONH(0b011);                             // Select new oscillator as ECPLL
    __builtin_write_OSCCONL(OSCCON | 0x01);                     // Initiate oscillator switch
    asm volatile ("DISI #0x0000");
    while (OSCCONbits.COSC != 0b011 || OSCCONbits.OSWEN == 1);  // Wait for clock switch
    while (OSCCONbits.LOCK != 1);                               // Wait for PLL
    
    /* Secondary oscillator */
    asm volatile ("DISI #0x3FFF");
    __builtin_write_OSCCONL(OSCCON | 0x02);                     // Enable secondary oscillator
    asm volatile ("DISI #0x0000");
    
    /* Clock  */
    CLKDIV = 0x0000;           // 1:1 ratio for CPU and peripheral clock
}

void timer_initialization(void) {
    /* Timer1 */
//    T1CON = 0x00;                       // Stop the timer1 and reset control reg.
//
//    TMR1 = 0x00;                        // Clear contents of timer register
//    PR1 = 0x007D;                       // Period Register (125)
//
//    T1CONbits.TCS = 1;                  // External 32.78KHz oscillator 
//    T1CONbits.TCKPS = 0b11;             // 1:256 Prescale (32KHz/256 = 125Hz)
//
//    T1CONbits.TON = 1;                  // Start 16-bit timer1 (1Hz)
//    
//    IFS0bits.T1IF = 0;                  // clear timer1 interrupt status flag
//    IEC0bits.T1IE = 1;                  // enable timer1 interrupt
    
    /* Timer2 */
    T2CON = 0x00;                       // Stop the timer2 and reset control reg.
    
    TMR2 = 0x00;                        // Clear contents of timer register
    PR2 = 0x7CFF;                       // Period Register (31999)
    
    T2CONbits.TCS = 0;                  // Internal 16Mhz instruction cycle (FCY)
    T2CONbits.TCKPS = 0;                // 1:1 Prescale (16Mhz/1 = 16Mhz)
    T2CONbits.T32 = 0;                  // Independent 16-bit timer

    T2CONbits.TON = 1;                  // Start 16-bit timer2 (500Hz)
    
    IFS0bits.T2IF = 0;                  // clear timer1 interrupt status flag
    IEC0bits.T2IE = 1;                  // enable timer1 interrupt
    
    /* Timer3 */
    T3CON = 0x00;                       // Stop the timer3 and reset control reg.
    
    TMR3 = 0x00;                        // Clear contents of timer register
    PR3 = 0x7CFF;                       // Period Register (31999)
    
    T3CONbits.TCS = 0;                  // Internal 16Mhz instruction cycle (FCY)
    T3CONbits.TCKPS = 0;                // 1:1 Prescale (16Mhz/1 = 16Mhz)

    T3CONbits.TON = 1;                  // Start 16-bit timer3 (500Hz)
}

void port_initialization(void) {
    /* Port A */
    TRISAbits.TRISA5 = 0;               // RA5 as output (OSC1 standby pin)
    PORTAbits.RA5 = 1;                  // Put OSC1 in high impedance

    /* Port B */
    // RB0 & RB1 for capacitive sensing
    AD1PCFGbits.PCFG0 = 1;              // AN0 as digital pin
    AD1PCFGbits.PCFG1 = 1;              // AN1 as digital pin

    // RB2 for photocell
    AD1PCFGbits.PCFG2 = 0;              // AN2 as analog pin
    TRISBbits.TRISB2 = 1;               // Photocell pin as input
    
    // RB3 & RB4 for push button
    AD1PCFGbits.PCFG3 = 1;              // AN3 as digital pin
    TRISBbits.TRISB3 = 1;               // Push Button sense as input
    
    AD1PCFGbits.PCFG4 = 1;              // AN4 as digital pin
    TRISBbits.TRISB4 = 0;               // LED Power as output
    
    PORTBbits.RB4 = 0;                  // Turn off PB LED
    
    /* Port D */
    // RD6 (sdat) & RD7 (sck) for load cell sensors, see loadsensor.h
    // RD13 for temperature sensor, see temperature.h
    
    // RD 15 for bluetooth
    TRISDbits.TRISD15 = 0;              // RD15 as output (BT module command pin)
    PORTDbits.RD15 = 1;                 // Enable command mode
    
    /* Port F */
    // RF4 & RF5 for bluetooth
    TRISFbits.TRISF4 = 0;               // RF4 as output (BT module wake pin)
    TRISFbits.TRISF5 = 1;               // RF5 as input (BT module connection pin)
    
    PORTFbits.RF4 = 1;                  // Wake up BT module
}

void uart_initialization(void) {
    /* UART1 */
    U1MODE = 0;                         // Clear UART1 mode register
    U1STA = 0;                          // Stop UART1 and clear status register
    
    U1MODEbits.STSEL = 0;               // 1-Stop bit
    U1MODEbits.PDSEL = 0;               // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0;               // Auto-Baud disabled
    U1MODEbits.BRGH = 1;                // High-Speed mode
    
    U1BRG = BAUD_115200;                // Baud rate
    
    U1STAbits.UTXISEL0 = 0;             // Interrupt when receive buffer is 1/4 full
    U1STAbits.UTXISEL0 = 0;
    U1STAbits.URXISEL = 0;              // Interrupt when receive buffer is 1/4 full
    
    IFS0bits.U1TXIF = 0;                // Clear TX Interrupt flag
    IFS0bits.U1RXIF = 0;                // Clear RX Interrupt flag
    
    IEC0bits.U1TXIE = 0;                // Disable Transmit Interrupt
    IEC0bits.U1RXIE = 1;                // Enable Receive Interrupt
    
    U1MODEbits.UARTEN = 1;              // Enable UART
    U1STAbits.UTXEN = 1;                // Enable Transmit
    
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

void atd_initialization()
{
    /* ATD for AN2 */
    AD1CON1 = 0;
    AD1CON1bits.SSRC = 0b111;           // Set ADC to internal counter (auto-convert)
    
    AD1CON2 = 0;
    AD1CON2bits.SMPI = 0b1111;          // Read every 16th sample
    
    AD1CON3 = 0;
    AD1CON3bits.SAMC = 0b01111;         // Sample time = 15Tad, Tad = Tcy
    
    AD1CHS = 0;
    AD1CHSbits.CH0SA = 0b00010;         // Connect AN2 as positive input to MUXA
    
    AD1CSSL = 0x0000;                   // No inputs are scanned
    
    AD1CON1bits.ADON = 1;               // Turn ADC ON
}