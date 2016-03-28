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
 * File: capacitivesensing.h   
 * Author: 
 * Comments: Implementation of capacitive sensing.
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PourOver_capacitivesensing_h
#define	PourOver_capacitivesensing_h

#include <xc.h> // include processor files - each processor file is guarded.  
#include "definitions.h"

/* Constants */
#define port_receive PORTBbits.RB1
#define port_send PORTBbits.RB0
#define sendPin_input TRISBbits.TRISB0 = 1
#define sendPin_output TRISBbits.TRISB0 = 0
#define receivePin_input TRISBbits.TRISB1 = 1
#define receivePin_output TRISBbits.TRISB1 = 0

#define FIVE_VOLT_TOLERANCE_WORKAROUND
/* Structures */

/* Variables */

/* Functions */
void capacitive_sense_initialization(void);
long capacitive_sense_raw(char samples);
void set_CS_timeout_millis(unsigned long timeout_millis);
void reset_CS_autocal();
void set_CS_autocal_millis(unsigned long autoCal_millis);
int sense_one_cycle(void);

#endif	/* PourOver_capacitivesensing_h */
