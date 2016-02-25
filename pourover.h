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

/* Variables */
volatile buffer tx_buffer;
volatile buffer rx_buffer;
volatile char receive_flag = 0;

/* Service */
service server;

/* Characteristics */
characteristic start_brew;
characteristic brew_state;
characteristic brew_temp;
characteristic brew_size;
characteristic water_level;
characteristic bean_level;
characteristic brew_strength;
characteristic brew_schedule;

#endif	/* PourOver_h */

