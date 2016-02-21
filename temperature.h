/* 
 * File:   temperature.h
 * Author: EE63pc6-user
 *
 * Created on February 20, 2016, 2:45 PM
 */

#ifndef PourOver_temperature_h
#define	PourOver_temperature_h

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <xc.h>
    
/* If TEMPFLOAT is defined, the calculated temp will be a floating point number, otherwise an integer */
#ifdef TEMPFLOAT
    #define TEMPRETURN float
#else
    #define TEMPRETURN int
#endif

/* Temperature Probe Related */
#define Port_18B20 PORTDbits.RD13
#define Tx_18B20 TRISDbits.TRISD13 = 0
#define Rx_18B20 TRISDbits.TRISD13 = 1
    
/* Macros for various bus transactions */
#define SKIP_ROM 0xCC
#define CONVERT_T 0x44
#define READ_SCRATCHPAD 0xBE
#define ALARM_SEARCH 0xEC
#define RECALL_EE 0xB8
#define ERROR_TEMP 0x7FFF

/* One-Wire related functions */
char Reset_18B20();
void Write_18B20(char);
char Read_18B20();

/* Generic wrapper function */
TEMPRETURN readTemperature();

#ifdef	__cplusplus
}
#endif

#endif	/* TEMPERATURE_H */

