/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File: pourover.h   
 * Author: Xiongyao Zha
 * Comments: PourOver firmware.
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PourOver_h
#define	PourOver_h

#include <xc.h>
#include "configurationbits.h"
#include "definitions.h"
#include "initializations.h"
#include "outputcompare.h"
#include "bluetooth.h"
#include "temperature.h"
#include "loadsensor.h"
#include "lightsensor.h"

/* Enumerations */
typedef enum {
    READY,
    ERRORCHECK,
    HEAT,
    DISPENSE,
    BLOOM,
    POUR,
    DONE
} brew_state;

typedef enum {
    IDLE,
    HEATERON,
    HEATEROFF
} heater_state;

typedef enum {
    NOERROR,
    WATERERROR,              // water level error
    BEANERROR,               // bean level error
    HEATERROR,               // heater error
    MISCERROR                // misc error
} error;

/* Constants */
#define WEAK 20              // weak brew strength
#define REGULAR 15           // regular brew strength
#define BOLD 8              // bold brew strength

#define SMALL 20.6           // bean grams for small brew size
#define MEDIUM 27.5          // bean grams for medium brew size
#define LARGE 35.4           // bean grams for large brew size

/* Variables */
// states
brew_state coffee_state = READY;          // state of coffee machine
heater_state heat_state = IDLE;           // state of heater
int pb_state            = 0;              // last state of the PB

// flags
error error_flag        = NOERROR;        // error flag
int heater_flag         = 0;              // flag to turn heater on and off
int pour_flag           = 0;              // flag to tell the water motors to begin pouring
int coffee_ready_flag   = 0;              // flag to tell the user the coffee is done
int brew_flag           = 0;              // flag to tell if the user wants coffee
int dispense_flag       = 0;              // flag to tell the bean motors to start dispensing
int tare_flag           = 1;              // set this to tare the coffee before use
int bloom_count         = 1;
int brew_size_init      = MEDIUM;
int brew_strength_init  = REGULAR;
int brew_temp_init = 200;

// measurements
long water_level;                         // water level measurement
int bean_level;                           // bean level measurement
int current_temp = 0;                     // current water temperature
int brew_temp = 0;                        // temp to brew at
float coffee_weight = 0;                  // coffee weight
int pb_reading;                           // value read from PB

// coffee specifications
float final_weight = 0;      // final weight for water calculated on the fly
int brew_strength = BOLD;    // integer for setting the strength of the brew
float brew_size = MEDIUM;    // float for setting the ground weight for brew size

// buffers
volatile buffer rx_buffer = {0, 0, {'\0'}};
volatile char receive_flag = 0;

// service
service server;

// characteristics
characteristic bt_start_brew;
characteristic bt_brew_state;
characteristic bt_brew_temp;
characteristic bt_brew_size;
characteristic bt_water_level;
characteristic bt_bean_level;
characteristic bt_brew_strength;
characteristic bt_brew_schedule;

// main loop variables
int i = 0;                  // loop counter
char bt_var_handle;         // determines BT handle that is written to
int temperature;            // real time temperature value
float weight;               // real time weight value

#endif	/* PourOver_h */