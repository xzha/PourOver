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
 * File: buffer.h   
 * Author: Xiongyao Zha
 * Comments: Set up configuration bits
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef PourOver_buffer_h
#define	PourOver_buffer_h

#include <xc.h> // include processor files - each processor file is guarded.  
#include "definitions.h"
#include "initializations.h"

/* Constants */
#define BUFFER_SIZE 512

/* Structures */
typedef struct {
    int head;
    int tail;
    char b[BUFFER_SIZE];
} buffer;

/* Variables*/
extern volatile buffer tx_buffer;
extern volatile buffer rx_buffer;
extern volatile char receive_flag;

/* Functions*/
unsigned char string_len(char *);
void string_copy(char *, char *);
char string_compare(char *, char *);

void int_to_hexstring(int, char *);
int hexstring_to_int(char *);

void buffer_init(volatile buffer *);
void buffer_empty(volatile buffer *);

void buffer_add_char(volatile buffer *,char);
void buffer_add(char *);

char buffer_write_segment(volatile buffer *);
void buffer_transmit(char *);

char buffer_read_segment(volatile buffer *, char *);
char buffer_read_check(char *);
char buffer_check(volatile buffer *, char *);


char buffer_transmit_check(char *, char *);
void buffer_transmit_set(char *, char *);

#endif	/* PourOver_buffer_h */

