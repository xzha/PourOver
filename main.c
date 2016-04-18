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
    pwm_initialization();
    
    // patch for iOS application
    bt_shw(&bt_brew_strength, 1);

    while(1)
    {   
        // reset values
        bt_var_handle = -1;
        temperature = ts_read();
        weight = ls_average_weight();
        
        // Send the values to the app
        bt_shw(&bt_brew_state, coffee_state);
                
        // BLE connected
        if (PORTFbits.RF5) {
            if (receive_flag > 0) {
                if (buffer_check(&rx_buffer,"Connected") == 1) {
                    continue;
                }
                else if (buffer_check(&rx_buffer, "Connection End") == 1) {
                    // end connection
                    continue;
                }
                
                // BLE received data
                bt_var_handle = bt_wv();
            }
        }
        
        // process received data
        switch (bt_var_handle) {
            case 0: //start_brew 75dc2f8004f242f48ab048d642153c91
                brew_flag = 1;
                break;
            case 1: //brew_state d2025d7957084ff1bc76c01e1abebb4d
                break;
            case 2: //brew_temp 7975652bf2a24f73a2da429ac3a83dfb
                brew_temp_init = bt_brew_temp.value;
                break;
            case 3: //brew_size 1cf1a1b203bb4f7ca28a8881169bede5
                if(bt_brew_size.value == 0)
                {
                    brew_size_init = SMALL;
                }
                else if(bt_brew_size.value == 1)
                {
                    brew_size_init = MEDIUM;
                }
                else
                {
                    brew_size_init = LARGE;
                }
                break;
            case 4: //water_level 538c13e23739428086ac91ab935a6ce1
                break;
            case 5: //bean_level 67b0653508394365bf7f8afc631e54a1
                break;
            case 6: //brew_strength dbfde0ac2cf241269759042cd13e5681
                if(bt_brew_strength.value == 0)
                {
                    brew_strength_init = WEAK;
                }
                else if(bt_brew_strength.value == 1)
                {
                    brew_strength_init = REGULAR;
                }
                else
                {
                    brew_strength_init = BOLD;
                }
                break;
            case 7: //brew_schedule 6ced5f74565c4608ba3d043f4b0297f9
                break;
            default:
                break;
        }
        
        switch (coffee_state)
        {
            case READY:
                heat_state = IDLE;                  // state of heater
                error_flag = NOERROR;               // error flag
                heater_flag = 0;                    // flag to turn heater on and off
                pour_flag = 0;                      // flag to tell the water motors to begin pouring
                coffee_ready_flag = 0;              // flag to tell the user the coffee is done
                dispense_flag = 0;                  // flag to tell the bean motors to start dispensing
                tare_flag = 1;                      // set this to tare the coffee before use
                current_temp = 0;                   // water temperature
                brew_temp = brew_temp_init;
                coffee_weight = 0;                  // coffee weight
                final_weight = 0;                   // final weight for water calculated on the fly
                brew_strength = brew_strength_init; // integer for setting the strength of the brew
                brew_size = brew_size_init;         // float for setting the ground weight for brew size
                bloom_count = 0;
                if(brew_flag)
                {
                    PORTBbits.RB4 = 1;
                    coffee_state = HEAT;
                    heater_flag = 1;
                    brew_flag = 0;
                }
                break;
            case HEAT:
                if(!heater_flag)
                {
                    coffee_state = DISPENSE;
                }

                PORTBbits.RB4 = 1;
                DELAY_MS(200);
                PORTBbits.RB4 = 0;
                DELAY_MS(100);
                PORTBbits.RB4 = 1;
                break;
            case DISPENSE:
                if(tare_flag)
                {
                    ls_tare(50);
                    tare_flag = 0;
                    dispense_flag = 1;
                    DELAY_MS(100);
                    coffee_weight = weight;
                }
                
                if(coffee_weight < brew_size- 20)
                {
                    coffee_weight = weight;
                    PORTBbits.RB4 = 0;
                    DELAY_MS(50);
                    PORTBbits.RB4 = 1;
                    DELAY_MS(50);
                    PORTBbits.RB4 = 0;
                    DELAY_MS(50);
                    PORTBbits.RB4 = 1;
                    DELAY_MS(50);
                }
                else
                {
                    PORTBbits.RB4 = 1;
                    DELAY_MS(2000);
                    dispense_flag = 0;
                    coffee_state = BLOOM;
                    tare_flag = 1;
                }
                break;
            case BLOOM:
                if(tare_flag)
                {
                    ls_tare(50);
                    tare_flag = 0;
                    DELAY_MS(100);
                    coffee_weight = weight;
                }
                
                bloom_count++;
                if(bloom_count == 1) oc_dutycycle(100, 1); // pump
                
                
                if(coffee_weight < (brew_size * 2)- 20)
                {
                    pour_flag = 1;
                    coffee_weight = weight;
                }
                else
                {
                    pour_flag = 0;
                    oc_dutycycle(0, 1);
                }
                
                if(bloom_count >= 30)
                {
                    coffee_state = POUR;
                    tare_flag = 1;
                }
                else
                {
                    DELAY_MS(900);
                }
                break;
            case POUR:
                if(tare_flag)
                {
                    ls_tare(50);
                    tare_flag = 0;
                    DELAY_MS(100);
                    coffee_weight = weight;
                    oc_dutycycle(40, 1); // pump
                }

                pour_flag = 1;
                
                if(coffee_weight < (brew_size * brew_strength) - 20)
                {
                    coffee_weight = weight;
                }
                else
                {
                    oc_dutycycle(0, 1);
                    pour_flag = 0;
                    coffee_state = DONE;
                    // may need to add code for a transition to the pause state.
                    // also may not need to because of the pour rate.
                    // needs to be tested in the system.
                }
                break;
            case DONE:
                PORTBbits.RB4 = 0;
                coffee_ready_flag = 1;
                
                //Show the user the brewing process is done by flashing the LED
                for(i = 0; i < 4; i++)
                {
                    PORTBbits.RB4 = 1;
                    DELAY_MS(500);
                    PORTBbits.RB4 = 0;
                    DELAY_MS(500);
                }
                break;
            default:
                break;
        }
        
        //reset everything for the state machine and return all states to idle
        if(coffee_ready_flag)
        {
            coffee_state = READY;          // state of coffee machine
            brew_flag = 0;              // flag to tell if the user wants coffee
        }
        
        //i think this should be in an interrupt????********
        switch(heat_state)
        {
            case IDLE:
                if(heater_flag) heat_state = HEATERON;
                break;
            case HEATERON:
                current_temp = temperature;
                heater_flag = 1;
                if(current_temp >= (brew_temp-20))
                {
                    heat_state = HEATEROFF;
                    heater_flag = 0;
                }
                break;
            case HEATEROFF:
                heater_flag = 0;
                current_temp = temperature;
                if(current_temp < (brew_temp-20))
                {
                    heat_state = HEATERON;
                    heater_flag = 1;
                }
                break;
            default:
                break;
        }
        
        if(heater_flag)
        {
            oc_dutycycle(100, 4);   //heater on
        }
        else
        {
            oc_dutycycle(0, 4);     //heater off
        }
        
        if(pour_flag)
        {
            oc_dutycycle(40, 1); // pump
            move_servo_circular(10);
        }
        if(!pour_flag)
        {
            oc_dutycycle(0, 1); // pump
        }
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