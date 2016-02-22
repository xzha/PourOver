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
 * File: temperature.h   
 * Author: Eric Murphy
 * Comments: Interface with temperature sensor. Basic implementation obtained 
 *           from "www.pic_examples.byethost3.com/DS18B20.html".
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PourOver_temperature_h
#define	PourOver_temperature_h

#include <xc.h> // include processor files - each processor file is guarded.  
#include "definitions.h"

/* One-Wire I/O Macros */
#define Port_18B20 PORTDbits.RD13
#define Tx_18B20 TRISDbits.TRISD13 = 0
#define Rx_18B20 TRISDbits.TRISD13 = 1
#ifdef TEMPFLOAT
    #define TEMPRETURN float
#else
    #define TEMPRETURN int
#endif

/* Constants */
#define SKIP_ROM 0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE
#define ALARM_SEARCH 0xEC
#define RECALL_EE 0xB8
#define ERROR_TEMP 0x7FFF

/* Functions */
char Reset_18B20();
void Write_18B20(char);
char Read_18B20();
TEMPRETURN readTemperature();

#endif	/* PourOver_temperature_h */