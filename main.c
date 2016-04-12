#include "pourover.h"

int main()
{    
    DELAY_MS(1500);                     // Wait for peripherals to be powered up
                                        // Microcontroller initialization should happen
                                        // after the peripherals are ready. 
    
    /* Initialize to default values */
    port_initialization();              // Start up Bluetooth, OSC
    oscillator_initialization();        // Clock:  32Mhz
    uart_initialization();              // UART1:  115200 Baud
    oc_initialization();                // OC1, OC2, OC3, OC4: 0% duty cycle
    timer_initialization();             // timer1: 1Hz
                                        // timer2: 500Hz
                                        // timer3: 500Hz
    atd_initialization();
    bt_initialization();
    ls_initialization();
    // capacitive_sense_initialization();
    
    // pwm intialization
    timer_frequency(500, 2); // pump    @ timer2
    timer_frequency(50, 3);  // servo1  @ timer3
                             // servo2  @ timer3 
                             // ssrelay @ timer3
    // pwm intialization
  
    while(1)
    { 
        int bt_var_handle = -1;
        int loadCellValue = (int) ls_average_weight();
        DELAY_MS(5);
        int temperatureValue = ts_read();
        DELAY_MS(5);
        
        if(brew_flag)
        {
            brew_flag = 0;
            ls_tare(50);
            DELAY_MS(100);
            PORTBbits.RB4 = 0;
        }
        
        // BLE connected
        if (PORTFbits.RF5) {
            if (receive_flag > 0) {
                if (buffer_check(&rx_buffer,"Connected") == 1)
                    continue;
                else if (buffer_check(&rx_buffer, "Connection End") == 1)
                    continue;
                
                // BLE received data
                bt_var_handle = bt_wv();
            }
        }
        
        // process received data (state machine?)
        switch (bt_var_handle) {
            case 0: //start_brew 75dc2f8004f242f48ab048d642153c91
                oc_dutycycle(0, 1); // pump
                oc_dutycycle(0, 2); // servo1
                oc_dutycycle(0, 3); // servo2
                break;
            case 1: //brew_state d2025d7957084ff1bc76c01e1abebb4d
                oc_dutycycle(50, 1); // pump
                oc_dutycycle(10, 2); // servo1
                oc_dutycycle(10, 3); // servo2
                break;
            case 2: //brew_temp 7975652bf2a24f73a2da429ac3a83dfb
                bt_shw(server.c[bt_var_handle], temperatureValue);
                // another way without access to handle would be: bt_shw(&bt_brew_temp, ts_read());
                break; 
            case 3:
                break;
            case 4:
                break;
            case 5: // bean level 67b0653508394365bf7f8afc631e54a1
                oc_dutycycle(25, 1); // pump
                oc_dutycycle(5, 2);  // servo1
                oc_dutycycle(5, 3);  // servo2
                break;
            case 6: //brew_strength dbfde0ac2cf241269759042cd13e5681
                bt_shw(server.c[bt_var_handle], loadCellValue);
                break;
            case 7:
                break;
            default:
                break;
        }
    }
    
//    while(1)
//    {
//        char bt_var_handle = -1;
//        int i = 0;
//
//        // BLE connected
//        if (PORTFbits.RF5) {
//            if (receive_flag > 0) {
//                if (buffer_check(&rx_buffer,"Connected") == 1)
//                    continue;
//                else if (buffer_check(&rx_buffer, "Connection End") == 1)
//                    continue;
//                
//                // BLE received data
//                bt_var_handle = bt_wv();
//            }
//        }
//        
//        // process received data (state machine?)
//        switch (bt_var_handle) {
//            case 0: //start_brew
//                oc_dutycycle(0, 1); // pump
//                oc_dutycycle(0, 2); // servo1
//                oc_dutycycle(0, 3); // servo2
//                break;
//            case 1:
//                break;
//            case 2: //brew_temp 7975652bf2a24f73a2da429ac3a83dfb
//                oc_dutycycle(50, 1); // pump
//                oc_dutycycle(10, 2); // servo1
//                oc_dutycycle(10, 3); // servo2
//                break;
//            case 3:
//                break;
//            case 4:
//                break;
//            case 5:
//                break;
//            case 6: //brew_strength dbfde0ac2cf241269759042cd13e5681
//                oc_dutycycle(25, 1); // pump
//                oc_dutycycle(5, 2);  // servo1
//                oc_dutycycle(5, 3);  // servo2
//                break;
//            case 7:
//                break;
//            default:
//                break;
//        }
//        
//        switch (coffee_state)
//        {
//            case READY:
//                coffee_ready_flag = 0;
//                if(brew_flag)
//                {
//                    coffee_state = ERRORCHECK;
//                    brew_flag = 0;
//                }
//                break;
//            case ERRORCHECK:
//                // measure the water and bean level to see if brewing is possible
//                error_flag = 0;                
//                for(i = 0; i < 5; i++)
//                {
//                    water_level = capacitive_sense_raw(100);
//                    if(water_level < 200) error_flag = WATERERROR;
//                }
//
//                bean_level = get_light_value();
//                if(bean_level < 150) error_flag = BEANERROR;
//                
//                // if no bean or water flag, continue to next state
//                // else, transmit error
//                if(!error_flag)
//                {
//                    coffee_state = HEAT;
//                }
//                        
//                break;
//            case HEAT:
//                if(temp >= 200)
//                {
//                    heat_state = HEATEROFF;
//                    coffee_state = DISPENSE;
//                }
//                else
//                {
//                    heat_state = HEATERON;
//                }
//                
//                break;
//            case DISPENSE:
//                if(tare_flag)
//                {
//                    ls_tare(50);
//                    tare_flag = 0;
//                    dispense_flag = 1;
//                }
//                
//                if(coffee_weight < brew_size)
//                {
//                    coffee_weight = ls_average_weight();
//                }
//                else
//                {
//                    dispense_flag = 0;
//                    coffee_state = POUR;
//                    tare_flag = 1;
//                }
//                break;
//            case POUR:
//                if(tare_flag)
//                {
//                    ls_tare(50);
//                    tare_flag = 0;
//                    pour_flag = 0;
//                }
//                
//                pour_flag = 1;
//                
//                if(coffee_weight < brew_size * brew_strength)
//                {
//                    coffee_weight = ls_average_weight();
//                }
//                else
//                {
//                    pour_flag = 0;
//                    coffee_state = DONE;
//                    // may need to add code for a transition to the pause state.
//                    // also may not need to because of the pour rate.
//                    // needs to be tested in the system.
//                }
//                break;
//            case DONE:
//                PORTBbits.RB4 = 0;
//                coffee_ready_flag = 1;
//                break;
//            default:
//                break;
//        }
//        
//        //i think this should be in an interrupt????********
//        switch(heat_state)
//        {
//            case IDLE:
//                heater_flag = 0;
//                break;
//            case HEATERON:
//                heater_flag = 1;
//                break;
//            case HEATEROFF:
//                heater_flag = 0;
//                if(temp < 200)
//                {
//                    heat_state = HEATERON;
//                }
//                break;
//            default:
//                break;
//        }
//    }

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
    IFS0bits.T2IF = 0;
    
    pb_reading = PORTBbits.RB3;
    
    if(pb_reading == pb_state)
    {
        PORTBbits.RB4 = 1;
        
        pb_state = pb_reading;
        
        brew_flag = 1;
    }
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
        char r = U1RXREG;               // Read in receive register
        
        rx_buffer.b[rx_buffer.tail] = r;
        rx_buffer.tail = (rx_buffer.tail + 1) % BUFFER_SIZE;
        
        if (r == '\n')
            receive_flag++;
    }
}