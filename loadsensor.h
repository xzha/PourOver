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
 * File: loadsensor.h   
 * Author: Eric Murphy
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PourOver_loadSensor_h
#define	PourOver_loadSensor_h

#include <xc.h> // include processor files - each processor file is guarded.  
#include "definitions.h"

/* One-Wire I/O Macros */
#define Port_CLK PORTDbits.RD7
#define Port_DATA PORTDbits.RD6
#define Tx_DATA TRISDbits.TRISD6 = 0
#define Rx_DATA TRISDbits.TRISD6 = 1
#define Tx_CLK TRISDbits.TRISD7 = 0

#define AVG 10
#define CALIBRATION_FACTOR -466

/* Functions */
void ls_power_up();
void ls_power_down();
char ls_shift_in();
int ls_ready();
long ls_ready_data();
long ls_read_average(int);

void ls_set_gain();
double ls_get_value(int);
float ls_get_units(int);
void ls_tare(int);
void ls_set_scale(float);
float ls_get_scale();
void ls_set_offset(long);
long ls_get_offset();

float ls_average_weight();
void ls_initialization();
void ls_calc_creep();

#endif	/* PourOver_loadSensor_h */