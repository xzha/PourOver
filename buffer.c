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
        output[index++] = '0';
        output[index] = '\0';
        return;
    }
    
    if (size % 2) {
        output[index] = '0';
        size++;
    }
    
    while(input) {
        if (input % 16 < 10)
            c = (input % 16) + 0x30;
        else
            c = ((input % 16) - 10) + 0x61;
        
        output[size - index++ - 1] = c;
        input = input / 16;
    }
    
    output[index + ((size - 1) % 2)] = '\0';
}

int hexstring_to_int(char * s) {
    int i = 0;
    int r = 0;
    
    while (s[i] != '\0') {
        r <<= 4;
        
        if (s[i] >= 0x30 && s[i] <= 0x39)
            r += s[i] - 0x30;
        else if (s[i] >= 0x61 && s[i] <= 0x66)
            r += (s[i] - 0x61) + 10;
        
        i++;
    }
    
    return r;
}

/* Functions */
void buffer_empty(volatile buffer * buff) {
    buff->head = buff->tail;
}

void buffer_transmit(char * s) {
    int i = 0;
    
    while(s[i] != '\0') {
        while (U1STAbits.UTXBF);        // Wait for transmit buffer
        while (!U1STAbits.TRMT);        // Wait for transmitter 
        U1TXREG = s[i];
        i++;
    }
    
    while (U1STAbits.UTXBF);        // Wait for transmit buffer
    while (!U1STAbits.TRMT);        // Wait for transmitter 
    U1TXREG = '\r';
    
    while (U1STAbits.UTXBF);        // Wait for transmit buffer
    while (!U1STAbits.TRMT);        // Wait for transmitter 
    U1TXREG = '\n';    
}

char buffer_read_segment(volatile buffer * buff, char * s) {
    int i = 0;
    
    if (receive_flag > 0)
        receive_flag--;
    
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

char buffer_read_check(char * s) {
    char temp[35];
    
    // intrusive read
    if (!buffer_read_segment(&rx_buffer, temp))
        return -1; // empty buffer
    
    if (!string_compare(s, temp))
        return -2; // string does not match
    
    return 1;
}

char buffer_check(volatile buffer * buff, char * s) {
    char temp[50];
    int i = 0;
    char read = 0;
    
    int head = buff->head;
    int tail = buff->tail;
    
    // non-intrusive read
    for (; head != tail; head = (head + 1) % BUFFER_SIZE) {
        temp[i++] = buff->b[head];
        if (buff->b[head] == '\n' && 
                buff->b[(head - 1) % BUFFER_SIZE] == '\r') {
            temp[i - 2] = '\0';
            head = (head + 1) % BUFFER_SIZE;
            read = 1;
            break;
        }
    }
    
    if (!read)
        return 0;
    
    if (!string_compare(s, temp))
        return 0;
    
    // adjust pointers only on match
    buff->head = head;
    buff->tail = tail;
    
    // adjust receive_flag on read
    if (receive_flag > 0)
        receive_flag--;
    
    return 1;
}

char buffer_transmit_check(char * s, char * r) {
    char status;
    
    buffer_transmit(s);
    DELAY_MS(500);
    
    // keep reading segment until match or empty
    while ((status = buffer_read_check(r)) == -2);
    
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
