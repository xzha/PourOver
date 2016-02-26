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

//float currentMeas = 0;
//float oldMeas = 0;
//float creep = 0;

/* Functions */
int generateClock();
void powerUp();
void powerDown();
char shiftIn();
int isReady();
long readData();
long readAverage(int);

void setGain();
double getValue(int);
float getUnits(int);
void tare(int);
void setScale(float);
float getScale();
void setOffset(long);
long getOffset();

float averageWeight();
void initialize();
void calcCreep();

#endif	/* PourOver_loadSensor_h */