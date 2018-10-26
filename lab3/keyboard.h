#pragma once

extern uint32_t status; //global variable that contains the scan of the buffer
extern uint32_t stat;
extern uint32_t counter;
extern int timer_counter;

#ifdef LAB3
int sys_inb_cnt(port_t port, uint32_t *byte);
#else
#define sys_inb_cnt(p,q) sys_inb(p,q) 
#endif

int kbc_scan_ih();

int kbd_subscribe_int(uint8_t *bit_no);

int kbd_unsubscribe_int();

uint8_t kbd_int_handler();

int kbd_scan_poll();

int kbd_poll_cmd();

void kbd_poll();
