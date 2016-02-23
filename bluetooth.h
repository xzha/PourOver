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
#include "initializations.h"

/* Variables*/
extern char SERVER_UUID[];

extern char START_BREW_U[];
extern char BREW_STATE_U[];
extern char BREW_TEMP_U[];
extern char BREW_SIZE_U[];
extern char WATER_LEVEL_U[];
extern char BEAN_LEVEL_U[];
extern char BREW_STRENGTH_U[];
extern char BREW_SCHEDULE_U[];

extern char START_BREW_H[];
extern char BREW_STATE_H[];
extern char BREW_TEMP_H[];
extern char BREW_SIZE_H[];
extern char WATER_LEVEL_H[];
extern char BEAN_LEVEL_H[];
extern char BREW_STRENGTH_H[];
extern char BREW_SCHEDULE_H[];

/* Functions*/
void bluetooth_initialization();
void bluetooth_ls();
void bluetooth_shw(char *, char *);
void bluetooth_shr(char *, char *);

#endif	/* PourOver_bluetooth_h */

