#pragma once

//GLOBAL VARIABLES
extern uint32_t status; //global variable that contains the scan of the buffer
extern bool error;


//PROTOTYPES
int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();
