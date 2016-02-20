#include "buffer.h"

void string_copy(char * src, char * dest) {
    int i = 0;
    
    while(src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

char string_len(char * s) {
    int i = 0;
    
    while(s[i] != '\0')
        i++;
    
    return i;
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
    if (!receive_flag)
        return 0;
    
    int i = 0;
    
    for (; buff->head != buff->tail; buff->head = (buff->head + 1) % BUFFER_SIZE) {
        s[i++] = buff->b[buff->head];
        if (buff->b[buff->head] == '\n' && 
                buff->b[(buff->head - 1) % BUFFER_SIZE] == '\r') {
            buff->head = (buff->head + 1) % BUFFER_SIZE;
            receive_flag = 0;
            return 1;
        }
    }
    return 0;
}

char buffer_check(char * s) {
    int i = 0;
    char temp[20];
    
    if (!buffer_read_segment(&rx_buffer, temp))
        return 0;
    
    while(s[i] != '\0') {
        if (s[i] != temp[i])
            return 0;
        i++;
    }
    
    if (temp[i] != '\r')
        return 0;
    
    return 1;
}
 
char buffer_transmit_check(char * s, char * r) {
    buffer_transmit(s);
    DELAY_MS(500);
    
    if (!buffer_check(r))
        return 0;
    return 1;
}

void buffer_transmit_set(char * s, char * r) {
    char get[30];
    char set[30];
    int len = string_len(s);
    
    string_copy(s, get);
    get[0] = 'g';
    
    if(!buffer_transmit_check(get, r)) {
        string_copy(s, set);
        set[len] = ',';
        string_copy(r, &set[len+1]);
        while (!buffer_transmit_check(set, "AOK"));
    }
}

void buffer_suw(char * u, char * d) {
    char suw[50];
    
    string_copy("suw,", suw);
    string_copy(u,   &suw[string_len(suw) + 1]);
    string_copy(",", &suw[string_len(suw) + 1]);
    string_copy(d,   &suw[string_len(suw) + 1]);
    
    while (!buffer_transmit_check(suw, "AOK"));
}

void buffer_sur(char * u, char * d) {
    char sur[20];
    
    string_copy("sur,", sur);
    string_copy(u, &sur[string_len(sur) + 1]);
    
    buffer_transmit(sur);
    
    buffer_read_segment(&rx_buffer, d);
}

void bluetooth_initialization() {
    /* Initialize buffer */
    buffer_init(&tx_buffer);
    buffer_init(&rx_buffer);
    receive_flag = 0;
    
    //PORTFbits.RF4 = 1;                               // Wake up BT module
    //DELAY_MS(200);                                   // Wait for BT to wake up
    //while(!buffer_check("CMD"));

    /* Initialize bluetooth module */
    buffer_transmit_set("sn", "PourOver-4125");
    buffer_transmit_set("sr", "20004000");
    buffer_transmit_set("ss", "00000001");
    while(!buffer_transmit_check("pz", "AOK"));
    
    // coffee machine service
    while(!buffer_transmit_check("ps,b80600992804496d81a7c1c3807826c0", "AOK"));    
    
    // characteristics
    while(!buffer_transmit_check("pc,75dc2f8004f242f48ab048d642153c91,0A,01", "AOK")); // start brew
    while(!buffer_transmit_check("pc,d2025d7957084ff1bc76c01e1abebb4d,02,05", "AOK")); // brew state
    while(!buffer_transmit_check("pc,538c13e23739428086ac91ab935a6ce1,02,05", "AOK")); // water level
    while(!buffer_transmit_check("pc,67b0653508394365bf7f8afc631e54a1,02,05", "AOK")); // bean level
    while(!buffer_transmit_check("pc,7975652bf2a24f73a2da429ac3a83dfb,0A,10", "AOK")); // brew temperature
    while(!buffer_transmit_check("pc,dbfde0ac2cf241269759042cd13e5681,0A,10", "AOK")); // brew strength
    while(!buffer_transmit_check("pc,1cf1a1b203bb4f7ca28a8881169bede5,0A,10", "AOK")); // brew size
    while(!buffer_transmit_check("pc,6ced5f74565c4608ba3d043f4b0297f9,00,20", "AOK")); // brew schedule?

    // reboot
    while(!buffer_transmit_check("r,1", "Reboot"));
    DELAY_MS(3000);
    uart_initialization();
    buffer_empty(&tx_buffer);
    buffer_empty(&rx_buffer);
    DELAY_MS(500);
}