#include "bluetooth.h"
#include <stdio.h>

void bt_initialization() {    
    /* Initialize buffer */
    buffer_empty(&rx_buffer);
    receive_flag = 0;
    
    /* Initialize BLE module */
    buffer_transmit_set("sn", "PourOver");
    buffer_transmit_set("sr", "20006400"); // Auto advertise, iOS mode, server only, no status string
    buffer_transmit_set("ss", "00000001");
    while(!buffer_transmit_check("pz", "AOK"));
    
    // coffee machine service
    while(!buffer_transmit_check("ps,11223344556677889900aabbccddeeff", "AOK"));    
    
    // characteristics
    while(!buffer_transmit_check("pc,75dc2f8004f242f48ab048d642153c91,0A,01", "AOK")); // start brew
    while(!buffer_transmit_check("pc,d2025d7957084ff1bc76c01e1abebb4d,0A,01", "AOK")); // brew state
    while(!buffer_transmit_check("pc,7975652bf2a24f73a2da429ac3a83dfb,0A,01", "AOK")); // brew temperature
    while(!buffer_transmit_check("pc,1cf1a1b203bb4f7ca28a8881169bede5,0A,01", "AOK")); // brew size
    while(!buffer_transmit_check("pc,538c13e23739428086ac91ab935a6ce1,02,01", "AOK")); // water level
    while(!buffer_transmit_check("pc,67b0653508394365bf7f8afc631e54a1,0A,01", "AOK")); // bean level
    while(!buffer_transmit_check("pc,dbfde0ac2cf241269759042cd13e5681,0A,01", "AOK")); // brew strength
    while(!buffer_transmit_check("pc,6ced5f74565c4608ba3d043f4b0297f9,00,01", "AOK")); // brew schedule
    
    // reboot
    while(!buffer_transmit_check("r,1", "Reboot"));
    
    // wait for command mode
    DELAY_MS(1500);
    uart_initialization();
    buffer_empty(&rx_buffer);
    receive_flag = 0;
    DELAY_MS(500);
    
    // initialize micro variables
    string_copy("11223344556677889900AABBCCDDEEFF", server.uuid);
    
    server.c[0] = &bt_start_brew;
    server.c[1] = &bt_brew_state;
    server.c[2] = &bt_brew_temp;
    server.c[3] = &bt_brew_size;
    server.c[4] = &bt_water_level;
    server.c[5] = &bt_bean_level;
    server.c[6] = &bt_brew_strength;
    server.c[7] = &bt_brew_schedule;
    
    string_copy("75DC2F8004F242F48AB048D642153C91", bt_start_brew.uuid);
    string_copy("D2025D7957084FF1BC76C01E1ABEBB4D", bt_brew_state.uuid);
    string_copy("7975652BF2A24F73A2DA429AC3A83DFB", bt_brew_temp.uuid);
    string_copy("1CF1A1B203BB4F7CA28A8881169BEDE5", bt_brew_size.uuid);
    string_copy("538C13E23739428086AC91AB935A6CE1", bt_water_level.uuid);
    string_copy("67B0653508394365BF7F8AFC631E54A1", bt_bean_level.uuid);
    string_copy("DBFDE0AC2CF241269759042CD13E5681", bt_brew_strength.uuid);
    string_copy("6CED5F74565C4608BA3D043F4B0297F9", bt_brew_schedule.uuid);
    bt_ls();
}

void bt_ls() {
    char temp[50];

    char u[35];
    char h[5];
    
    // list all characteristics associated with the service
    while (!buffer_transmit_check("ls", server.uuid));
    
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
        
        for (i = 0; i < CHARACTERISTIC_COUNT; i++) {
            if (string_compare(u, server.c[i]->uuid)) { 
                string_copy(h, server.c[i]->handle);
                break;
            }
        }
        
        buffer_read_segment(&rx_buffer, temp);
    }
}

void bt_shw(characteristic * c, int d) {
    char shw[30];
    char temp[25];
    
    // assign value to characteristic
    c->value = d;
    
    // shw,handle,data
    int_to_hexstring(c->value, temp);
    sprintf(shw, "shw,%s,%s", c->handle, temp);
    while (!buffer_transmit_check(shw, "AOK"));
}

/* NOTE:
 *   bt_shr(&bt_brew_temp) will probably be obsolete as values are read through bt_wv()
 *   and store into characteristic's value field.
 */
void bt_shr(characteristic * c) {
    char shr[10];
    char temp[25];
    
    // shr,handle
    sprintf(shr, "shr,%s", c->handle);
    buffer_transmit(shr);
    buffer_read_segment(&rx_buffer, temp);
    
    // assign value to characteristic
    c->value = hexstring_to_int(temp);
}

char bt_wv() {
    char temp[100];
    
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
        if (temp[i] == '0')
            break;
        
        d[i - offset] = temp[i];
        i++;
    }
    d[i - offset] = '\0';
    
    for (i = 0; i < CHARACTERISTIC_COUNT; i++) {
        if (string_compare(h, server.c[i]->handle)) { 
            server.c[i]->value = hexstring_to_int(d);
            return i;
        }
    }
    
    return -1;
}