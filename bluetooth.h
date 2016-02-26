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
 * File: bluetooth.h   
 * Author: Xiongyao Zha
 * Comments: RN4020 Interface
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PourOver_bluetooth_h
#define	PourOver_bluetooth_h

#include <xc.h> // include processor files - each processor file is guarded.  
#include "definitions.h"
#include "buffer.h"

/* Variables*/
#define CHARACTERISTIC_COUNT 8

/* Structures */
typedef struct {
    char uuid[35];
    char handle[5];
    int value;
} characteristic;

typedef struct {
    char uuid[35];
    characteristic * c[CHARACTERISTIC_COUNT];
} service;

/* Variables*/
extern service server;
extern characteristic start_brew;
extern characteristic brew_state;
extern characteristic brew_temp;
extern characteristic brew_size;
extern characteristic water_level;
extern characteristic bean_level;
extern characteristic brew_strength;
extern characteristic brew_schedule;

/* Functions*/
void bluetooth_initialization();
void bluetooth_ls();
void bluetooth_shw(char *, char *);
void bluetooth_shr(char *, char *);
char bluetooth_wv();

#endif	/* PourOver_bluetooth_h */
