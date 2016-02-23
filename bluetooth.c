#include "bluetooth.h"

void bluetooth_initialization() {
    /* Initialize buffer */
    buffer_init(&tx_buffer);
    buffer_init(&rx_buffer);
    receive_flag = 0;

    /* Initialize bluetooth module */
    buffer_transmit_set("sn", "PourOver");
    buffer_transmit_set("sr", "20006400"); // Auto advertise, iOS mode, server only, no status string
    buffer_transmit_set("ss", "00000001");
    while(!buffer_transmit_check("pz", "AOK"));
    
    // coffee machine service
    while(!buffer_transmit_check("ps,11223344556677889900aabbccddeeff", "AOK"));    
    
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
    bluetooth_ls();
    receive_flag = 0;
}

void bluetooth_ls() {
    char temp[50];

    char u[35];
    char h[5];
    
    // list all characteristics associated with the service
    while (!buffer_transmit_check("ls", SERVER_UUID));
    
    buffer_read_segment(&rx_buffer, temp);
    
    while (!string_compare("END", temp)) {
        int i = 2;
        
        // read in uuid
        while (temp[i] != ',') {
            u[i-2] = temp[i];
            i++;
        }
        u[i-2] = '\0';
        
        int offset = ++i;
        
        // read in handle
        while (temp[i] != ',') {
            h[i - offset] = temp[i];
            i++;
        }
        h[i - offset] = '\0';
        
        if (string_compare(u, START_BREW_U))
            string_copy(h, START_BREW_H);
        else if (string_compare(u, BREW_STATE_U))
            string_copy(h, BREW_STATE_H);
        else if (string_compare(u, BREW_TEMP_U))
            string_copy(h, BREW_TEMP_H);
        else if (string_compare(u, BREW_SIZE_U))
            string_copy(h, BREW_SIZE_H);
        else if (string_compare(u, WATER_LEVEL_U))
            string_copy(h, WATER_LEVEL_H);
        else if (string_compare(u, BEAN_LEVEL_U))
            string_copy(h, BEAN_LEVEL_H);
        else if (string_compare(u, BREW_STRENGTH_U))
            string_copy(h, BREW_STRENGTH_H);
        else if (string_compare(u, BREW_SCHEDULE_U))
            string_copy(h, BREW_SCHEDULE_H);
        
        buffer_read_segment(&rx_buffer, temp);
    }
}

void bluetooth_shw(char * h, char * d) {
    char shw[30];
    
    string_copy("shw,", shw);
    string_copy(h,   &shw[string_len(shw)]);
    string_copy(",", &shw[string_len(shw)]);
    string_copy(d,   &shw[string_len(shw)]);
    
    while (!buffer_transmit_check(shw, "AOK"));
}

void bluetooth_shr(char * h, char * d) {
    char shr[10];
    
    string_copy("shr,", shr);
    string_copy(h, &shr[string_len(shr) + 1]);
    
    buffer_transmit(shr);
    
    buffer_read_segment(&rx_buffer, d);
}

char bluetooth_wv() {
    // WV,000B,123456789DEF.
    char temp[50];
    
    char d[35];
    char h[5];
    
    buffer_read_segment(&rx_buffer, temp);
    
    if (temp[0] != 'W' && temp[1] != 'V')
        return -1;
    
    int i = 3;
        
    // read in handle
    while (temp[i] != ',') {
        h[i-3] = temp[i];
        i++;
    }
    h[i-3] = '\0';
        
    int offset = ++i;
        
    // read in data
    while (temp[i] != '.') {
        d[i - offset] = temp[i];
        i++;
    }
    d[i - offset] = '\0';
    
    if (string_compare(h, START_BREW_H)) {
        START_BREW_V = hexstring_to_int(d);
        return 1;
    }
    else if (string_compare(h, BREW_STATE_H)) {
        BREW_STATE_V = hexstring_to_int(d);
        return 2;
    }
    else if (string_compare(h, BREW_TEMP_H)) {
        BREW_TEMP_V = hexstring_to_int(d);
        return 3;
    }
    else if (string_compare(h, BREW_SIZE_H)) {
        BREW_SIZE_V = hexstring_to_int(d);
        return 4;
    }
    else if (string_compare(h, WATER_LEVEL_H)) {
        WATER_LEVEL_V = hexstring_to_int(d);
        return 5;
    }
    else if (string_compare(h, BEAN_LEVEL_H)) {
        BEAN_LEVEL_V = hexstring_to_int(d);
        return 6;
    }
    else if (string_compare(h, BREW_STRENGTH_H)) {
        BREW_STRENGTH_V = hexstring_to_int(d);
        return 7;
    }
    else if (string_compare(h, BREW_SCHEDULE_H)) {
        BREW_SCHEDULE_V = hexstring_to_int(d);
        return 8;
    }
    
    return -1;
}