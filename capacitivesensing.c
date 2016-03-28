#include "capacitivesensing.h"

unsigned long leastTotal;
unsigned int loopTimingFactor;
unsigned long CS_Timeout_Millis;
unsigned long CS_Autocal_Millis;
unsigned long lastCal;
unsigned long cap_total;

int savedIPL;

#define SET_CPU_IPL(ipl) {       \
  int DISI_save;                 \
                                 \
  DISI_save = DISICNT;           \
  asm volatile ("disi #0x3FFF"); \
  SRbits.IPL = ipl;              \
  __builtin_write_DISICNT( DISI_save); } (void) 0;

#define SET_AND_SAVE_CPU_IPL(save_to, ipl) { \
  save_to = SRbits.IPL; \
  SET_CPU_IPL(ipl); } (void) 0;

#define RESTORE_CPU_IPL(saved_to) SET_CPU_IPL(saved_to)

void disableInterrupts(void)
 {
    SET_AND_SAVE_CPU_IPL(savedIPL, 7);
 }
 
 void enableInterrupts(void)
 {
     RESTORE_CPU_IPL(savedIPL);
 }

void capacitive_sense_initialization(void)
{
    loopTimingFactor = 310;
    
    CS_Timeout_Millis = (2000 * (float)loopTimingFactor * (float)FOSC) / 8000000;
    CS_Autocal_Millis = 20000;
    
    sendPin_output;
    receivePin_input;
    port_send = 0;
    
    leastTotal = 0xFFFFFFFF;
    lastCal = 0;
}

long capacitive_sense_raw(char samples)
{
    cap_total = 0;
    char indexVar = 0;
    
    if(samples == 0) return(0);
    
    while(indexVar != samples)
    {
        if(sense_one_cycle() < 0) return(-2);
        indexVar++;
    }
    
    return(cap_total);
}

void set_CS_timeout_millis(unsigned long timeout_millis)
{
    CS_Timeout_Millis = (timeout_millis * (float)loopTimingFactor * (float)FOSC) / 8000000;
}

void reset_CS_autocal()
{
    leastTotal = 0xFFFFFFFF;
}

void set_CS_autocal_millis(unsigned long autoCal_millis)
{
    CS_Autocal_Millis = autoCal_millis;
}

int sense_one_cycle(void)
{
    disableInterrupts();
    port_send = 0;
    receivePin_input;
    receivePin_output;
    port_receive = 0;
    DELAY_US(10);
    receivePin_input;
    port_send = 1;
    enableInterrupts();
    
    while(!port_receive && (cap_total < CS_Timeout_Millis))
    {
        cap_total++;
    }
    
    if(cap_total > CS_Timeout_Millis)
    {
        return(-2);
    }
    
    disableInterrupts();
//    Port_receive = 1;
    receivePin_output;
    port_receive = 1;
    receivePin_input;
    port_send = 0;
    enableInterrupts();
    
#ifdef FIVE_VOLT_TOLERANCE_WORKAROUND
    receivePin_output;
    port_receive = 0;
    DELAY_US(10);
    receivePin_input;
#else
    while(port_receive && (cap_total < CS_Timeout_Millis))
    {
        cap_total++;
    }
#endif
    
    if(cap_total >= CS_Timeout_Millis)
    {
        return(-2);
    }
    else
    {
        return(1);
    }
}