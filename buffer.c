#include "buffer.h"

unsigned char string_len(char * s) {
    unsigned char i = 0;
    
    while(s[i] != '\0')
        i++;
    
    return i;
}

void string_copy(char * src, char * dest) {
    int i = 0;
    
    while(src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

char string_compare(char * s1, char * s2) {
    int i = 0;
    
    if (string_len(s1) != string_len(s2)) {
        return 0;
    }
    
    while(s1[i] != '\0') {
        if (s1[i] != s2[i])
            return 0;
        i++;
    }
    
    return 1;
}

void int_to_ascii(int input, char * output)
{
    int index = 0;
    int size = 0;
    int tempInput = input;
    
    while(tempInput) {
        size++;
        tempInput = tempInput / 10;
    }
    
    if(input <= 0) {
        output[index++] = '0';
        output[index] = '\0';
        return;
    }
    
    while(input) {
        output[size - index++ - 1] = (input % 10) + 0x30;
        input = input / 10;
    }
    
    output[index] = '\0';
}

void int_to_hexstring(int input, char * output)
{
    int index = 0;
    int size = 0;
    char c;
    int tempInput = input;
    
    while(tempInput) {
        size++;
        tempInput = tempInput / 16;
    }
    
    if(input <= 0) {
        output[index++] = '0';
        output[index] = '\0';
        return;
    }
    
    while(input) {
        if (input % 16 < 10)
            c = (input % 16) + 0x30;
        else
            c = ((input % 16) - 10) + 0x61;
        
        output[size - index++ - 1] = c;
        input = input / 16;
    }
    
    output[index] = '\0';
}

/* Functions */
void buffer_init(volatile buffer * buff) {
    buff->head = 0;
    buff->tail = 0;
    
    int i;
    for (i = 0; i < BUFFER_SIZE; i ++)
        buff->b[i] = '\0';
}

void buffer_empty(volatile buffer * buff) {
    buff->head = buff->tail;
}

void buffer_add_char(volatile buffer * buff, char c) {
    buff->b[buff->tail] = c;
    buff->tail = (buff->tail + 1) % BUFFER_SIZE;
}

void buffer_add(char * s) {
    int i = 0;
    while (s[i] != '\0') {
        buffer_add_char(&tx_buffer, s[i]);
        i++;
    }
}

char buffer_write_segment(volatile buffer * buff) {
    for (; buff->head != buff->tail; buff->head = (buff->head + 1) % BUFFER_SIZE) {
        while (U1STAbits.UTXBF);        // Wait for transmit buffer
        while (!U1STAbits.TRMT);        // Wait for transmitter  
        U1TXREG = buff->b[buff->head];  // Place letter into transmit register
        if (buff->b[buff->head] == '\n' && 
                buff->b[(buff->head - 1) % BUFFER_SIZE] == '\r') {
            buff->head = (buff->head + 1) % BUFFER_SIZE;
            return 1;
        }
    }
    return 0;
}

void buffer_transmit(char * s) {
    buffer_add(s);
    buffer_add_char(&tx_buffer, '\r');
    buffer_add_char(&tx_buffer, '\n');
    while(buffer_write_segment(&tx_buffer));
}

char buffer_read_segment(volatile buffer * buff, char * s) {
    int i = 0;
    
    if (receive_flag)
        receive_flag = 0;
    
    for (; buff->head != buff->tail; buff->head = (buff->head + 1) % BUFFER_SIZE) {
        s[i++] = buff->b[buff->head];
        if (buff->b[buff->head] == '\n' && 
                buff->b[(buff->head - 1) % BUFFER_SIZE] == '\r') {
            s[i - 2] = '\0';
            buff->head = (buff->head + 1) % BUFFER_SIZE;
            return 1;
        }
    }
    return 0;
}

char buffer_check(char * s) {
    char temp[35];
    
    if (!buffer_read_segment(&rx_buffer, temp))
        return -1; // empty buffer
    
    if (!string_compare(s, temp))
        return -2; // string does not match
    
    return 1;
}
 
char buffer_transmit_check(char * s, char * r) {
    char status;
    
    buffer_transmit(s);
    DELAY_MS(500);
    
    // keep reading segment until match or empty
    while ((status = buffer_check(r)) == -2);
    
    // return 0 on empty
    if (status == -1)
        return 0;
        
    return 1;
}

void buffer_transmit_set(char * s, char * r) {
    char get[30];
    char set[30];
    unsigned char len = string_len(s);
    
    string_copy(s, get);
    get[0] = 'g';
    
    if(!buffer_transmit_check(get, r)) {
        string_copy(s, set);
        set[len] = ',';
        string_copy(r, &set[len+1]);
        while (!buffer_transmit_check(set, "AOK"));
    }
}
