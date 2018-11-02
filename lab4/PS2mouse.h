#pragma once

//global variables
extern uint32_t status; //global variable that contains the scan of the buffer
extern uint32_t stat;
//extern uint32_t counter;
//extern int timer_counter;
//extern bool error;

//#ifdef LAB4
//int sys_inb_cnt(port_t port, uint32_t *byte);
//#else
//#define sys_inb_cnt(p,q) sys_inb(p,q) 
//#endif

//prototypes
int mouse_subscribe_int(uint8_t *bit_no);

int mouse_unsubscribe_int();

void (mouse_ih)(void) ;
